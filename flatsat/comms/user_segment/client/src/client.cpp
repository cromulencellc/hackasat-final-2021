/*
 * Cromulence
 * Authors: meenmachine, Tormund
 * 
 * This is the client binary which was part of challenge 3's crypto challenge
 * and used to send to the usersegment server for challenges 4 and 5
*/

#include "client.hpp"
#include <fstream>

#define GAME_ENV    /* Comment this out for testing */

#ifndef GENERIC_SOCKET_ERROR
    #define GENERIC_SOCKET_ERROR -1
#endif

#define MAX_MSG_SIZE  240
#define USER_SEG_ADDR "127.0.0.1"
#define USER_SEG_PORT 31337

int check(int ret, const char *err_s, int fatal){
    if(ret == GENERIC_SOCKET_ERROR){
        perror(err_s);
        if(fatal){
            LOG("check failed");
            exit(EXIT_FAILURE);
        }
    }
    return ret;
}

void get_response(int sockfd){
    int ret = 0;
    char resp_buff[512] = {0};
    struct timeval timeout;

    timeout.tv_sec  = 2;
    timeout.tv_usec = 0;

    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    ret = read(sockfd, resp_buff, sizeof(resp_buff));
    check(ret, "While reading response from user segment server.", 1);

    printf("Response from user segment server: %s\n", resp_buff);
}

int main(int argc, char **argv){

    argparse::ArgumentParser program{"User Segment Client"};

    program.add_argument("-i", "--id")
        .required()
        .help("Satellite ID")
        .action([](const std::string& value) { return std::stoul(value); });

    program.add_argument("-k", "--key")
        .required()
        .help("Attribution key")
        .action([](const std::string& value) { return std::stoul(value); });

    program.add_argument("-m", "--message")
        .help("Hex message (to be converted to bytes) to send to user segment. Must be EVEN-length string. Ex: -m 414141 would be converted to AAA");

    program.add_argument("-f", "--key-file")
        .help("Path to private key file.")
        .required()
        .default_value(std::string("id_rsa"));

    program.add_argument("-p", "--port")
        .scan<'i', int>()
        .help("Port used to connect to the user segment server")
        .default_value(USER_SEG_PORT);

    program.add_argument("-a", "--address")
        .help("Address used to connect to the user segment server")
        .default_value(std::string(USER_SEG_ADDR));

    program.add_argument("-d", "--data-file")
        .help("Path to data file. Contents will be used to send to user segment.")
        .default_value(std::string(""));

    program.add_argument("-s", "--danx-service")
        .help("Send message to the DANX service instead of the Comm Payload Message Server")
        .default_value(false)
        .implicit_value(true);

    UserSegmentPacket pkt;

    int user_seg_port = USER_SEG_PORT;
    std::string data;

    try {
        program.parse_args(argc, argv);
        user_seg_port    = program.get<int>("--port");

        if(user_seg_port < 0 || user_seg_port > 65'535){
            throw std::runtime_error{"Port is not within 1 and 65535"};
        }
    }
    catch (const std::runtime_error &err) {
        std::cout << err.what() << "\n" << std::endl;
        std::cout << program;
        exit(-1);
    }

    std::string data_file = program.get<std::string>("--data-file");
    bool alreadyBytes = false;
    if(!data_file.empty()){
        std::ifstream f{data_file.c_str()};

        if(!f.good()){
            std::cout << "Error: Data file '" << data_file << "' doesn't exist." << std::endl;
            exit(-1);
        }
        std::ifstream t{data_file};

        std::stringstream buf;
        buf << t.rdbuf();
        data.assign(buf.str());

        alreadyBytes = true;
    }
    else{
        try{
            data = program.get<std::string>("--message");
        }
        catch(const std::logic_error& err){
            std::cout << "Must provide either argument: --message or --data-file\n" << std::endl;
            std::cout << program;
            exit(-1);
        }
    }
    
    if(data.size() > MAX_MSG_SIZE){
        std::cout << "Error: Message can only have a max size of " << MAX_MSG_SIZE << " bytes" << std::endl;
        exit(-1);
    }

    try{
        pkt = {
            data,
            program.get<std::string>("--key-file"),
            program.get<unsigned long>("--id"),
            program.get<unsigned long>("--key"),
            program.get<bool>("-s"),
            alreadyBytes
        };
    }
    catch(const std::runtime_error &err){
        std::cout << err.what() << std::endl;
        exit(-1);
    }


    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    check(sockfd, "Error while making socket", true);

    sockaddr_in servaddr = {};

    servaddr.sin_addr.s_addr = inet_addr(program.get<std::string>("--address").c_str());
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons((uint16_t)user_seg_port);

    int ret = connect(sockfd, reinterpret_cast<sockaddr*>(&servaddr), sizeof(servaddr));
    check(ret, "Error while connecting to user segment server", true);

    std::vector<uint8_t> *pkt_packed = nullptr;
    
    try{
        pkt_packed = pkt.getPackedMsg();
    }
    catch(const std::runtime_error& err){
        std::cout << err.what() << std::endl;
        exit(-1); 
    }

    send(sockfd, pkt_packed->data(), pkt_packed->size(), 0);

#ifndef GAME_ENV
    pkt_packed = pkt.askForPubKey(4);
    send(sockfd, pkt_packed->data(), pkt_packed->size(), 0);
#endif

    get_response(sockfd);

    return 0;
}