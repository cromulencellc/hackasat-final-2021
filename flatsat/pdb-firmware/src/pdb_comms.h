#ifndef _pdb_comms_h
#define _pdb_comms_h


#include "Arduino.h"


#include <string>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
// #include <circular_buffer.h>

extern "C" {
    #include "app_cfg.h"
    #include "cfe_sb.h"
}

// typedef struct recv_pkt {
//     uint8_t data[RECV_PKT_SIZE];
// } recv_pkt_t;

class PdbComms {

    enum class ConnectionStatus {
        NO_ERROR = 0,
        PHY_ERROR = 1,
        LINK_ERROR = 2,
        UDP_ERROR = 3,
    };

    public:
        void begin(std::string ip, uint16_t recvPort, uint16_t sendPort);
        void connect(void);
        CFE_SB_Msg_t* recvData(void);
        int sendData(void* srcBuffer, uint16_t len);

    private:
        EthernetClass Ethernet_;
        EthernetUDP Udp_;
        HardwareSerial* SerialOutput_;
        EthernetHardwareStatus EthernetHardwareStatus_;
        EthernetLinkStatus EthernetLinkStatus_;

        // circular_buffer<recv_pkt_t> packetRecvBuffer_;  // buffer to hold incoming packet
        uint8_t packetSendBuffer_[RECV_PKT_SIZE];
        uint8_t packetRecvBuffer_[RECV_PKT_SIZE];
        uint16_t recvPort_;
        uint16_t sendPort_;
        IPAddress ip_;
        IPAddress sendIp_;
        ConnectionStatus status_;
        std::string ipAddressToString_(IPAddress addr);

};

#endif