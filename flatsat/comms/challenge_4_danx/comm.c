#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define TELEM_PORT 4580

#define CMD_PING    0x1 << 0
#define CMD_COMM    0x1 << 1
#define CMD_SEND    0x1 << 2

typedef struct pager_message_entry pager_message_entry;
typedef struct pager_packet pager_packet;
typedef struct pager_packet_node pager_packet_node;

struct pager_packet {
    uint8_t id;
    uint8_t order;
    uint8_t data_size;
    uint8_t data[];
};

struct pager_packet_node {
    pager_packet_node *fd;
    pager_packet_node *bk;
    pager_packet packet;
};

#define NODE_SIZE   19

uint8_t *flag;
int serv_sock, telemetry_sock, counter;
struct sockaddr_in servaddr, dispatch_addr;
struct pager_packet_node *message_table[7];
struct pager_packet_node *message_stack[7];

pager_packet_node *alloc_packet(size_t data_size)
{
    return malloc(NODE_SIZE + data_size);
}

void free_old_message()
{
    pager_packet_node *val = message_stack[6];
    for (int i=0; i < 7; i++) {
        if (message_table[i]->packet.id == val->packet.id) {
            pager_packet_node *head = message_table[i];
            pager_packet_node *temp = NULL;
            while (head != NULL) {
                temp = head;
                head = head->fd;
                free(temp);
            }
            message_table[i] = NULL;
            break;
        }
    }
    message_stack[6] = NULL;
    counter--;
}

void update_lifo_que(pager_packet_node *packet)
{
    int pos = 0;
    pager_packet_node *val;

    for (int i=0; i < 7; i++) {
        if (message_stack[i] == NULL) {
            pos = i;
            val = packet;
            message_stack[i] = val;
            break;
        }
        else if (message_stack[i]->packet.id == packet->packet.id) {
            pos = i;
            val = message_stack[i];
            break;
        }
    }

    for (int j=pos; j > 0; j--) {
        message_stack[j] = message_stack[j-1];
    }
    message_stack[0] = val;
}

void create_new_message(pager_packet *packet)
{
    pager_packet_node *new_pkt = alloc_packet(packet->data_size);
    new_pkt->packet.id = packet->id;
    new_pkt->packet.order = packet->order;
    new_pkt->packet.data_size = packet->data_size;
    memcpy(new_pkt->packet.data, packet->data, new_pkt->packet.data_size);

    if (counter == 7) {
        free_old_message();
    }

    for (int i=0; i < 7; i++) {
        if (message_table[i] == NULL) {
            message_table[i] = new_pkt;
            break;
        }
    }
    update_lifo_que(new_pkt);
    counter++;
}

/* dont realloc, but overwrite it and do it with a heap byte overflow */
void overwrite_current_order(pager_packet_node *dst, pager_packet *src)
{
    if (src->data_size > sizeof(pager_packet) + dst->packet.data_size) {
        return;
    }
    dst->packet.data_size = src->data_size;
    memcpy(dst->packet.data, src->data, dst->packet.data_size);
    return;
}

void parse_packet(pager_packet *packet)
{
    for (int i=0; i < 7; i++) {
        if (message_table[i] != NULL) {
            /* Check if message with existing ID exist */
            if (packet->id == message_table[i]->packet.id) {
                /* Check if packet exist with current order */
                for (pager_packet_node *cur=message_table[i]; cur != NULL; cur = cur->fd) {
                    if (packet->order == cur->packet.order) {
                        /* OVERWRITE ORDER / UPDATE */
                        overwrite_current_order(cur, packet);
                        update_lifo_que(cur);
                        return;
                    }
                }
                /* Order didnt exist, add it to the message */
                pager_packet_node *new_pkt = alloc_packet(packet->data_size);
                new_pkt->packet.id = packet->id;
                new_pkt->packet.order = packet->order;
                new_pkt->packet.data_size = packet->data_size;
                memcpy(new_pkt->packet.data, packet->data, new_pkt->packet.data_size);

                pager_packet_node *head = message_table[i];
                while(1) {
                    if (packet->order < head->packet.order) {
                        pager_packet_node *new_pkt = alloc_packet(packet->data_size);

                        new_pkt->fd = head;
                        new_pkt->bk = head->bk;
                        new_pkt->bk->fd = new_pkt;
                        head->bk = new_pkt;

                        update_lifo_que(new_pkt);
                        return;
                    }
                    else if (head->fd == NULL) {
                        head->fd = new_pkt;
                        new_pkt->bk = head;
                        new_pkt->fd = NULL;
                        return;
                    }
                    head = head->fd;
                }

            }
        }
    }
    create_new_message(packet);
}

void send_to_telemetry(char *val)
{
    write(telemetry_sock, val, 8);
}

int main(int argc, char **argv)
{
    char buf[1024] = {0};
    char pkt_buf[9] = {0};

    if ((serv_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Failed to create socket\n");
        exit(-1);
    }

    int enable = 1;
    if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0){
        perror("setsockopt(SO_REUSEADDR) failed");
        exit(-1);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("0.0.0.0");
    servaddr.sin_port = htons(TELEM_PORT);
    // Binding newly created socket to given IP and verification
    if ((bind(serv_sock, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) {
        perror("socket bind failed...\n");
        exit(-1);
    }

    // Now server is ready to listen and verification
    if ((listen(serv_sock, 5)) != 0) {
        perror("Listen failed...\n");
        exit(-1);
    }

    int len = sizeof(dispatch_addr);
    telemetry_sock = accept(serv_sock, (struct sockaddr *)&dispatch_addr, &len);
    if (telemetry_sock < 0) {
        perror("Server failed to accept connection from dispatcher\n");
        exit(-1);
    }

    flag = mmap((void *)0x800000, 0x1000, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    if (flag == MAP_FAILED || (u_int64_t)flag != 0x800000) {
        perror("mmap failed to allocate RWX region\n");
        close(serv_sock);
        close(telemetry_sock);
        exit(-1);
    }

    char *val = getenv("FLAG");
    if (val == NULL) {
        perror("Failed to get FLAG environment variable");
        close(serv_sock);
        close(telemetry_sock);
        exit(-1);
    }
    memcpy(flag, val, 8);

    printf("Connection established with telemetry dispatcher.\n");
    send_to_telemetry("COMREADY");
    while (1) {
        size_t count = read(0, buf, 1024);
        if (!strncmp(buf, "KILL", 4)) {
            printf("Exiting Space Comm\n");
            exit(0);
        }
        else if (!strncmp(buf, "CHECK", 4)) {
            send_to_telemetry("COMMISUP");
        }
        else if (count > sizeof(pager_packet)) {
            parse_packet((struct pager_packet *)buf);
            sprintf(pkt_buf, "PKT-%02x%02x", buf[0], buf[1]);
            send_to_telemetry(pkt_buf);
        }
        else {
            send_to_telemetry("PKTINVAL");
        }
        memset(buf, 0, 1024);
    }
}
