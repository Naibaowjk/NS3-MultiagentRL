#ifndef NODEHELPER_H

#define NODEHELPER_H

#include <fstream>
#include <iostream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/log.h"
#include "ns3/animation-interface.h"
#include "ns3/csma-helper.h"
#include "ns3/ssid.h"
#include "ns3/command-line.h"
#include "ns3/aodv-module.h"
#include "ns3/olsr-helper.h"
#include "ns3/point-to-point-helper.h"
#include <vector>
#include "ns3/udp-socket-factory.h"
#include "ns3/udp-l4-protocol.h"
#include "ns3/wifi-net-device.h"
#include "ns3/wifi-mac.h"
#include "ns3/ipv4-address.h"
#include "ns3/opengym-module.h"
#include "ns3/nstime.h"

using namespace ns3;
using namespace std;


extern string NODEPATH;
extern uint32_t SENTPACKET_NUM ;
extern uint32_t SENTPACKET_NUM_IN_TIMESTEP;
extern uint32_t TIME_STEP;
extern uint32_t CURRENT_TIMESTEP;

class NodeUAVhelper
{
private:
  /* data */
  uint32_t num_uavNodes;
  vector<uint32_t> uavs_battery;
  vector<NetDeviceContainer> NDC_UAVs_ap;
  string ssidString;
  uint32_t construction_size;
  Vector charge_position;

public:
  NodeUAVhelper ();
  NodeUAVhelper (uint32_t num_uavNodes, uint32_t construction_size, Vector charge_position);
  ~NodeUAVhelper ();

  NodeContainer NC_UAVs_adhoc;
  NetDeviceContainer NDC_UAVs_adhoc;

  vector<Ipv4InterfaceContainer> interfaces_ap;
  Ipv4InterfaceContainer interfaces_adhoc;

  Ipv4AddressHelper ipAddrs_adhoc;
  vector<Ipv4AddressHelper> ipAddrs_ap;
  vector<vector<bool>> ip_flag;

  Ssid get_UAV_SSID (uint32_t i);
  string get_new_Address (uint32_t i);
  Vector getUAVPosition (uint32_t i);

  void init_UAVs (YansWifiPhyHelper &wifiPhy, InternetStackHelper &internet_stack);
  void setUAVbattery (uint32_t i, uint32_t val);
  uint32_t getUAVbattery (uint32_t i);

  void setUAVPosition (uint32_t i, Vector position);

  void set_up (uint32_t i);
  void set_down (uint32_t i);
};

class NodeUEhelper
{
public:
  NodeUEhelper();
  NodeUEhelper (uint32_t num_ueNodes, double time_step, string mobility_type,
                string print_recv_path, uint32_t construction_size);
  ~NodeUEhelper ();

  uint32_t construction_size;
  Vector charge_position;

  NodeContainer NC_UEs;
  vector<NetDeviceContainer> NDC_UEs; // 第一次初始化使用，由于SSID不同，所以需要用vector
  vector<Ipv4InterfaceContainer> interfaces; // 第一次初始化使用，由于SSID不同，所以需要用vector
  vector<bool> is_de_init; // 是否初始化的flag
  vector<bool> is_app_init; // 是否注册了Application
  vector<uint32_t> connect_uav_index; //管理连接到的UAV的index
  vector<ApplicationContainer> app_c; //管理连接到的Application
  OnOffHelper onoffhelper=OnOffHelper("ns3::UdpSocketFactory", Address ());//管理OnoffApplication
  vector<uint32_t> bytesTotal;
  vector<uint32_t> bytesTotal_timestep;
  vector<uint32_t> packetsReceived;
  vector<uint32_t> packetsReceived_timestep;

  vector<uint32_t> onoffstate; 

  void connect_to_UAV (uint32_t i_UE, YansWifiPhyHelper &wifiPhy, NodeUAVhelper &uavhelper,
                       uint32_t i_UAV);
  Vector getUEPosition (uint32_t i);
  uint32_t getUEBlock (uint32_t i);
  vector<uint32_t> getUEBlock_All ();
  string getOnoffState(uint32_t i);
  DataRateValue getDataRate(uint32_t i);
  void init_UEs (InternetStackHelper &internet_stack);
  void setMobility ();
  void setUEPosition (uint32_t i, Vector position);
  void setPacketReceive(uint32_t i,uint32_t port);
  void setApplication(uint32_t i, AddressValue remoteAddress);
  void setDataRate(uint32_t i,DataRateValue value);
  void setOnOffState(uint32_t i,string state);
  /*
  void TxwithSeqTsSize_Callback (Ptr< const Packet > p, const Address &from, const Address &to, const SeqTsSizeHeader &header);
  */

private:
  uint32_t num_ueNodes;
  double time_step;
  string mobility_type; //设定mobility是static或是random
  string print_recv_path;
  void connect_to_Ap (uint32_t i_UE, Ssid ssid, YansWifiPhyHelper &wifiPhy,
                    Ipv4AddressHelper &ipAddr);
  void receivePacket (Ptr<Socket> socket);
  void printReceivedPacket (uint32_t node_index, Ptr<Socket> socket, Ptr<Packet> packet, Address senderAddress);
};

void
TxwithSeqTsSize_Callback (Ptr<const Packet> p, const Address &from, const Address &to,
                                        const SeqTsSizeHeader &header);

#endif