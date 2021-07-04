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
#include "ns3/point-to-point-helper.h"
#include <vector>

NS_LOG_COMPONENT_DEFINE ("nodehelper");

using namespace ns3;
using namespace std;

class NodeUAVhelper
{
private:
  /* data */
  uint32_t num_uavNodes;
  vector<uint32_t> uavs_battery;
  vector<NetDeviceContainer> NDC_UAVs_ap;
  string ssidString;

public:
  NodeUAVhelper (uint32_t num_uavNodes);
  ~NodeUAVhelper ();

  NodeContainer NC_UAVs_adhoc;
  NetDeviceContainer NDC_UAVs_adhoc;

  vector<Ipv4InterfaceContainer> interfaces_ap;
  Ipv4InterfaceContainer interfaces_adhoc;

  Ipv4AddressHelper ipAddrs_adhoc;
  vector<Ipv4AddressHelper> ipAddrs_ap;

  Ssid get_UAV_SSID (uint32_t i);

  void init_UAVs (YansWifiPhyHelper &wifiPhy, InternetStackHelper &internet_stack);
  void setUAVbattery (uint32_t i, uint32_t val);
  uint32_t getUAVbattery (uint32_t i);

  void setUAVPosition (uint32_t i, Vector position);
  Vector getUAVPosition (uint32_t i);

  void setUAVUp (uint32_t i);
  void setUAVdown (uint32_t i);
};

class NodeUEhelper
{
public:
  NodeUEhelper (uint32_t num_ueNodes, double time_step);
  ~NodeUEhelper ();

  NodeContainer NC_UEs;
  vector<NetDeviceContainer> NDC_UEs;

  void init_UEs (InternetStackHelper &internet_stack);
  void setUEPosition (uint32_t i, Vector position);
  Vector getUEPosition (uint32_t i);
  void Connect_to_UAV (uint32_t i_UE, YansWifiPhyHelper &wifiPhy, NodeUAVhelper &uavhelper,
                       uint32_t i_UAV);
  void Connect_to_Ap (uint32_t i_UE, Ssid ssid, YansWifiPhyHelper &wifiPhy,
                      Ipv4AddressHelper &ipAddr);
  uint32_t getUEBlock (uint32_t i);
  vector<uint32_t> getUEBlock_All ();

private:
  uint32_t num_ueNodes;
  string ipbase1 = "172.16.";
  string ipbase2 = ".1";
  double time_step;
};

#endif