#include "nodehelper.h"

NS_LOG_COMPONENT_DEFINE ("nodehelper");

NodeUAVhelper::NodeUAVhelper (uint32_t num_uavNodes)
{
  this->num_uavNodes = num_uavNodes;
  vector<uint32_t> uavs_battery_init (num_uavNodes, 100);
  this->uavs_battery = uavs_battery_init;
  this->NC_UAVs_adhoc.Create (num_uavNodes);
  this->ssidString = "wifi-UAV";
  this->ipAddrs_ap =vector<Ipv4AddressHelper>(num_uavNodes);
}

NodeUAVhelper::~NodeUAVhelper ()
{
}

void
NodeUAVhelper::setUAVbattery (uint32_t i, uint32_t val)
{
  /*
        i is the index, maximun is length()-1
     */
  if (i >= num_uavNodes)
    {
      NS_LOG_ERROR ("Error: out of index for set battery!");
      return;
    }
  if (val < 0 || val > 100)
    {
      NS_LOG_ERROR ("Error: out of range for set battery!");
      return;
    }
  this->uavs_battery[i] = val;
}

uint32_t
NodeUAVhelper::getUAVbattery (uint32_t i)
{
  return this->uavs_battery[i];
}

void
NodeUAVhelper::setUAVUp (uint32_t i)
{
  // set up adhoc
  pair<Ptr<Ipv4>, uint32_t> returnValue = interfaces_adhoc.Get (i);
  Ptr<Ipv4> ipv4 = returnValue.first;
  uint32_t index = returnValue.second;
  Ptr<Ipv4Interface> iface = ipv4->GetObject<Ipv4L3Protocol> ()->GetInterface (index);
  NS_LOG_INFO (Simulator::Now ().GetSeconds ()
               << "Set adhoc: " << iface->GetAddress (0).GetLocal () << " up.");
  ipv4->SetUp (index);
  // set up ap
  returnValue = interfaces_ap[i].Get (0);
  ipv4 = returnValue.first;
  index = returnValue.second;
  iface = ipv4->GetObject<Ipv4L3Protocol> ()->GetInterface (index);
  NS_LOG_INFO (Simulator::Now ().GetSeconds ()
               << "Set ap:" << iface->GetAddress (0).GetLocal () << " up.");
  ipv4->SetUp (index);
}

void
NodeUAVhelper::setUAVdown(uint32_t i)
{
  // set down adhoc
  pair<Ptr<Ipv4>, uint32_t> returnValue = interfaces_adhoc.Get (i);
  Ptr<Ipv4> ipv4 = returnValue.first;
  uint32_t index = returnValue.second;
  Ptr<Ipv4Interface> iface = ipv4->GetObject<Ipv4L3Protocol> ()->GetInterface (index);
  NS_LOG_INFO (Simulator::Now ().GetSeconds ()
               << "Set adhoc: " << iface->GetAddress (0).GetLocal () << " down.");
  ipv4->SetDown (index);
  // set down ap
  returnValue = interfaces_ap[i].Get (0);
  ipv4 = returnValue.first;
  index = returnValue.second;
  iface = ipv4->GetObject<Ipv4L3Protocol> ()->GetInterface (index);
  NS_LOG_INFO (Simulator::Now ().GetSeconds ()
               << "Set ap:" << iface->GetAddress (0).GetLocal () << " down.");
  ipv4->SetDown (index);
}

void
NodeUAVhelper::setUAVPosition (uint32_t i, Vector position)
{
  Ptr<ConstantPositionMobilityModel> posi_model_adhoc =
      this->NC_UAVs_adhoc.Get (i)->GetObject<ConstantPositionMobilityModel> ();
  posi_model_adhoc->SetPosition (position);
}

Vector
NodeUAVhelper::getUAVPosition (uint32_t i)
{
  Ptr<ConstantPositionMobilityModel> posi_model_adhoc =
      this->NC_UAVs_adhoc.Get (i)->GetObject<ConstantPositionMobilityModel> ();
  Vector ret = posi_model_adhoc->GetPosition ();
  return ret;
}

Ssid
NodeUAVhelper::get_UAV_SSID (uint32_t i)
{
  std::stringstream ss;
  ss << i;
  return Ssid (ssidString + ss.str ());
}

void
NodeUAVhelper::init_UAVs (YansWifiPhyHelper &wifiPhy, InternetStackHelper &internet_stack)
{
  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode",
                                StringValue ("OfdmRate54Mbps"));

  // MAC Adhoc
  WifiMacHelper wifiMac;
  wifiMac.SetType ("ns3::AdhocWifiMac");
  NDC_UAVs_adhoc = wifi.Install (wifiPhy, wifiMac, NC_UAVs_adhoc);

  NS_LOG_UNCOND ("Setting Route Protocol: AODV for adhoc");
  AodvHelper aodv;

  internet_stack.SetRoutingHelper (aodv);
  internet_stack.Install (NC_UAVs_adhoc);

  // IP Install in Device adhoc
  ipAddrs_adhoc.SetBase ("192.168.0.0", "255.255.255.0");
  interfaces_adhoc = ipAddrs_adhoc.Assign (NDC_UAVs_adhoc);

  //设定移动方式为constant，初始位置位于（0，0）
  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator", "MinX", DoubleValue (0), "MinY",
                                 DoubleValue (0), "DeltaX", DoubleValue (0), "DeltaY",
                                 DoubleValue (0), "GridWidth", UintegerValue (2), "LayoutType",
                                 StringValue ("RowFirst"));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (NC_UAVs_adhoc);

  NS_LOG_UNCOND ("Setting UAVs Topo for AP & Adhoc");
  // Reset the address base-- for ap nodes UAVs
  // the "172.16 address space
  
  for (uint32_t i = 0; i < num_uavNodes; i++)
    {
      //设定Ap和Adhoc节点的P2P连接，并设置Ap的第二类ip
      /* NodeContainer p2pNodes;
      p2pNodes.Add (NC_UAVs_adhoc.Get (i));
      p2pNodes.Add (NC_UAVs_ap.Get (i));

      PointToPointHelper pointToPoint;
      pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("1000Mbps"));
      pointToPoint.SetChannelAttribute ("Delay", StringValue ("10ns"));
      p2p_NDC.push_back (pointToPoint.Install (p2pNodes));

      internet_stack.Install (p2pNodes);
      ipAddrs.Assign (p2p_NDC[i]);
      ipAddrs.NewNetwork (); */
      std::stringstream ss;
      ss << i;
      string ip="172.16."+ss.str()+".0";
      ipAddrs_ap[i].SetBase (ip.c_str(), "255.255.255.0");
      WifiMacHelper wifiMac;
      wifiMac.SetType ("ns3::ApWifiMac", "Ssid", SsidValue (this->get_UAV_SSID (i)));
      NDC_UAVs_ap.push_back (wifi.Install (wifiPhy, wifiMac, NC_UAVs_adhoc.Get (i)));
      interfaces_ap.push_back (ipAddrs_ap[i].Assign (NDC_UAVs_ap[i]));
    }
}

NodeUEhelper::NodeUEhelper (uint32_t num_ueNodes, double time_step)
{
  this->num_ueNodes = num_ueNodes;
  this->NC_UEs.Create (num_ueNodes);
  this->time_step = time_step;
  this->NDC_UEs=vector<NetDeviceContainer>(num_ueNodes);
  this->interfaces=vector<Ipv4InterfaceContainer>(num_ueNodes);
}

void
NodeUEhelper::init_UEs (InternetStackHelper &internet_stack)
{
  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode",
                                StringValue ("OfdmRate54Mbps"));

  internet_stack.Install (NC_UEs);
  //设定Mobility
  MobilityHelper mobility;
  mobility.SetMobilityModel (
      "ns3::GaussMarkovMobilityModel", "Bounds", BoxValue (Box (0, 300, 0, 300, 0, 300)),
      "TimeStep", TimeValue (Seconds (time_step)), "Alpha", DoubleValue (0.85), "MeanVelocity",
      StringValue ("ns3::UniformRandomVariable[Min=0|Max=3]"), "MeanDirection",
      StringValue ("ns3::UniformRandomVariable[Min=0|Max=6.283185307]"), "MeanPitch",
      StringValue ("ns3::UniformRandomVariable[Min=0.05|Max=0.05]"), "NormalVelocity",
      StringValue ("ns3::NormalRandomVariable[Mean=0.0|Variance=0.0|Bound=0.0]"), "NormalDirection",
      StringValue ("ns3::NormalRandomVariable[Mean=0.0|Variance=0.2|Bound=0.4]"), "NormalPitch",
      StringValue ("ns3::NormalRandomVariable[Mean=0.0|Variance=0.02|Bound=0.04]"));
  mobility.SetPositionAllocator ("ns3::RandomBoxPositionAllocator", "X",
                                 StringValue ("ns3::UniformRandomVariable[Min=0|Max=300]"), "Y",
                                 StringValue ("ns3::UniformRandomVariable[Min=0|Max=300]"), "Z",
                                 StringValue ("ns3::UniformRandomVariable[Min=0|Max=0]"));
  mobility.Install (NC_UEs);
}

Vector
NodeUEhelper::getUEPosition (uint32_t i)
{
  Ptr<GaussMarkovMobilityModel> posi_model_ue =
      this->NC_UEs.Get (i)->GetObject<GaussMarkovMobilityModel> ();
  Vector ret = posi_model_ue->GetPosition ();
  return ret;
}

void
NodeUEhelper::setUEPosition (uint32_t i, Vector position)
{
  Ptr<GaussMarkovMobilityModel> posi_model_ue =
      this->NC_UEs.Get (i)->GetObject<GaussMarkovMobilityModel> ();
  posi_model_ue->SetPosition (position);
}

uint32_t
NodeUEhelper::getUEBlock (uint32_t i)
{
  uint32_t ret;
  Vector position = this->getUEPosition (i);
  // 计算所属的区域
  if (position.x >= 0 && position.x < 100)
    ret = 1;
  else if (position.x >= 100 && position.x < 200)
    ret = 2;
  else
    ret = 3;

  if (position.y >= 0 && position.y < 100)
    ret += 0;
  else if (position.y >= 100 && position.y < 200)
    ret += 3;
  else
    ret += 6;

  return ret;
}

vector<uint32_t>
NodeUEhelper::getUEBlock_All ()
{
  vector<uint32_t> ret;
  for (uint32_t i = 0; i < num_ueNodes; i++)
    {
      ret.push_back (getUEBlock (i));
    }
  return ret;
}

void NodeUEhelper::Connect_to_Ap (uint32_t i_UE, Ssid ssid, YansWifiPhyHelper &wifiPhy,Ipv4AddressHelper &ipAddr)
{
  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode",
                                StringValue ("OfdmRate54Mbps"));
  WifiMacHelper mac_sta;
  mac_sta.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid));
  NDC_UEs[i_UE]= wifi.Install (wifiPhy, mac_sta, NC_UEs.Get(i_UE));

  interfaces[i_UE]=(ipAddr.Assign(NDC_UEs[i_UE]));

}

void NodeUEhelper::Connect_to_UAV (uint32_t i_UE, YansWifiPhyHelper &wifiPhy, NodeUAVhelper &uavhelper,
                       uint32_t i_UAV)
{
    Ssid ssid=uavhelper.get_UAV_SSID(i_UAV);
    Connect_to_Ap(i_UE,ssid,wifiPhy,uavhelper.ipAddrs_ap[i_UAV]);
}

NodeUEhelper::~NodeUEhelper(){}