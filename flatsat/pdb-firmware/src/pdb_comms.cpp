#include <pdb_comms.h>
#include <pdb_logger.hpp>
#include <stdio.h>
#include <vector>

static  uint8_t deviceMac[6];

void teensyMAC(uint8_t *mac) {
    for(uint8_t by=0; by<2; by++) mac[by]=(HW_OCOTP_MAC1 >> ((1-by)*8)) & 0xFF;
    for(uint8_t by=0; by<4; by++) mac[by+2]=(HW_OCOTP_MAC0 >> ((3-by)*8)) & 0xFF;
    LOG_DEBUG("DEVICE MAC: %02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}


std::string macToString(uint8_t *mac) {
    char buffer[18];
    snprintf(buffer, sizeof(buffer), "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    std::string macString = buffer;
    return macString;
}

// std::vector<uint8_t> macToVector(const uint8_t* iMac)  {
//     std::vector<uint8_t> mac = { iMac[0], iMac[1], iMac[2], iMac[3], iMac[4], iMac[5] };
//     return mac;
// }

/**
 * Public Functions
 * 
 */
void PdbComms::begin(std::string ip, uint16_t recvPort, uint16_t sendPort) {
    // std::vector<uint8_t> mac = macFromString(MAC_ADDRESS);
    teensyMAC(deviceMac);
    std::string macString = macToString(deviceMac);
    LOG_DEBUG("Start Ethernet. MAC: %s:, IP: %s", macString.c_str(), ip.c_str());
    // SerialOutput_ = serialOutput;
    ip_.fromString(String(ip.c_str()));
    std::string remote_ip = IP_ADDRESS_SEND;
    sendIp_.fromString(String(remote_ip.c_str()));

    recvPort_ = recvPort;
    sendPort_ = sendPort;
    Ethernet_.begin(deviceMac, ip_);
    connect();
    LOG_INFO("Ethernet Started. MAC: %s, Recv Endpoint: %s:%d, Send Endpoint: %s:%d", macString.c_str(), ip.c_str(), recvPort, remote_ip.c_str(), sendPort);

        // Check for Ethernet hardware present

}

void PdbComms::connect(void) {
    status_ = ConnectionStatus::NO_ERROR;
    EthernetHardwareStatus_ = Ethernet.hardwareStatus();
    if (EthernetHardwareStatus_ == EthernetNoHardware) {
        LOG_ERROR("Ethernet PHY not found");
        status_ = ConnectionStatus::PHY_ERROR;
        return;
    }
    EthernetLinkStatus_ = Ethernet.linkStatus();
    if (EthernetLinkStatus_ == LinkOFF) {
        LOG_ERROR("Ethernet cable is not connected.");
        status_ = ConnectionStatus::LINK_ERROR;
        return;
    }

    // start UDP
    // Serial2.printf("Start UDP Server. RecvPort: %d, Send Port: %d\n", recvPort, sendPort);
    if(Udp_.begin(recvPort_)) {
        status_ = ConnectionStatus::UDP_ERROR;
        return;
    };

    LOG_INFO("UDP Socket Initialization Successful");
}


 CFE_SB_Msg_t* PdbComms::recvData(void) {
    // if (status_ != ConnectionStatus::NO_ERROR) {
    //     connect();
    //     if (status_ != ConnectionStatus::NO_ERROR) {
    //         SerialOutput_ ->println("ERROR: Could not open UDP socket.");
    //         return NULL;
    //     }
            
    // }
    CFE_SB_Msg_t*  MsgPtr = NULL;
    int packetSize = Udp_.parsePacket();

    if (packetSize) {
        memset(packetRecvBuffer_, 0, sizeof(packetRecvBuffer_));
        // IPAddress remote = Udp_.remoteIP();

        // read the packet into packetBufffer
        packetSize = Udp_.read(packetRecvBuffer_, UDP_TX_PACKET_MAX_SIZE);
        if (packetSize) {
            MsgPtr = (CFE_SB_Msg_t*)&packetRecvBuffer_;
        }
    }
    return MsgPtr;
}

int PdbComms::sendData(void* srcBuffer, uint16_t len) {
    // send a reply to the IP address and port that sent us the packet we received
    if(len > sizeof(packetSendBuffer_)) {
        return -1;
    }
    memset(packetSendBuffer_, 0, sizeof(packetSendBuffer_));
    memcpy(packetSendBuffer_, srcBuffer, len);
    Udp_.beginPacket(sendIp_, sendPort_);
    size_t dataSendSize = Udp_.write(packetSendBuffer_, len);
    Udp_.endPacket();
    return dataSendSize;
}

/**
 * Private Functions
 * 
 */

std::string PdbComms::ipAddressToString_(IPAddress addr) {
    char buf[20];
    Serial2.println(addr);
    uint32_t ip = (uint32_t) addr;
    Serial2.println(ip);
    snprintf(buf, 20, "%u.%u.%u.%u", (uint8_t) (ip & 0xFF), (uint8_t) ((ip>>8) & 0xFF), (uint8_t)((ip>>16) & 0xFF), (uint8_t)((ip>>24) & 0xFF));
    return std::string(buf);
}