#include "nodehelper.h"

NS_LOG_COMPONENT_DEFINE ("nodehelper");

NodeUAVhelper::NodeUAVhelper ()
{
  NodeUAVhelper (0);
}

NodeUAVhelper::NodeUAVhelper (uint32_t num_uavNodes)
{

  this->num_uavNodes = num_uavNodes;
  vector<uint32_t> uavs_battery_init (num_uavNodes, 100);
  this->uavs_battery = uavs_battery_init;
  this->NC_UAVs_adhoc.Create (num_uavNodes);
  this->ssidString = "wifi-UAV";
  this->ipAddrs_ap = vector<Ipv4AddressHelper> (num_uavNodes);
  this->ip_flag = vector<vector<bool>> (num_uavNodes, vector<bool> (255, false));
  for (uint32_t i = 0; i < num_uavNodes; i++)
    {
      ip_flag[i][0] = true;
    }
}

NodeUAVhelper::~NodeUAVhelper ()
{
}

uint32_t
NodeUAVhelper::getUAVbattery (uint32_t i)
{
  return this->uavs_battery[i];
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

string
NodeUAVhelper::get_new_Address (uint32_t i)
{
  std::stringstream ss;

  string ip;
  for (uint32_t j = 0; j < 255; j++)
    {
      if (ip_flag[i][j] == false)
        {
          ss << "172.16." << i << "." << j;
          ip = ss.str ();
          ip_flag[i][j] = true;
          break;
        }
    }
  return ip;
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
  OlsrHelper olsr;
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
      stringstream ss;
      ss << "172.16." << i << ".0";
      ipAddrs_ap[i].SetBase (ss.str ().c_str (), "255.255.255.0");
      WifiMacHelper wifiMac;
      wifiMac.SetType ("ns3::ApWifiMac", "Ssid", SsidValue (this->get_UAV_SSID (i)));
      NDC_UAVs_ap.push_back (wifi.Install (wifiPhy, wifiMac, NC_UAVs_adhoc.Get (i)));
      interfaces_ap.push_back (ipAddrs_ap[i].Assign (NDC_UAVs_ap[i]));
      ip_flag[i][1] = true;
    }
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

void
NodeUAVhelper::set_up (uint32_t i)
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
NodeUAVhelper::set_down (uint32_t i)
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

/* -----------------------------------------------------------------------------*/
NodeUEhelper::NodeUEhelper ()
{
  NodeUEhelper (0, 4, "constant", "./scratch/sa_jiakang/static_full/");
}

NodeUEhelper::NodeUEhelper (uint32_t num_ueNodes, double time_step, string mobility_type,
                            string print_recv_path)
{
  this->num_ueNodes = num_ueNodes;
  this->NC_UEs.Create (num_ueNodes);
  this->time_step = time_step;
  this->NDC_UEs = vector<NetDeviceContainer> (num_ueNodes);
  this->interfaces = vector<Ipv4InterfaceContainer> (num_ueNodes);
  this->is_de_init = vector<bool> (num_ueNodes, false);
  this->is_app_init = vector<bool> (num_ueNodes, false);
  this->connect_uav_index = vector<uint32_t> (num_ueNodes, -1);
  this->mobility_type = mobility_type;
  this->print_recv_path = print_recv_path;
  this->packetsReceived = vector<uint32_t> (num_ueNodes, 0);
  this->packetsReceived_timestep = vector<uint32_t> (num_ueNodes, 0);
  this->bytesTotal = vector<uint32_t> (num_ueNodes, 0);
  this->bytesTotal_timestep = vector<uint32_t> (num_ueNodes, 0);
  this->app_c = vector<ApplicationContainer> (num_ueNodes);
  this->onoffstate = vector<uint32_t> (num_ueNodes, 1);

  // init onoff Application
  this->onoffhelper.SetAttribute ("PacketSize", UintegerValue (1024));
  this->onoffhelper.SetAttribute ("DataRate", DataRateValue (DataRate ("2048bps")));
  this->onoffhelper.SetAttribute ("OnTime",
                                  StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
  this->onoffhelper.SetAttribute ("OffTime",
                                  StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));
  this->onoffhelper.SetAttribute ("EnableSeqTsSizeHeader", BooleanValue (true));

  // print title for recv csv
  for (uint32_t i = 0; i < num_ueNodes; i++)
    {
      stringstream outfile_path;
      outfile_path << print_recv_path << "/receiver/recv_from_ue_" << i << ".csv";
      ofstream out (outfile_path.str ());
      out << "UE_id "
          << ","
          << "UE_ip_current "
          << ","
          << "Recv time[s] "
          << ","
          << "Sequence num"
          << ","
          << "Parket size[byte]"
          << ","
          << "Sent time[s]"
          << ","
          << "Delay[ms]"
          << "" << endl;
      out.close ();
    }
  // print title for unknown csv
  stringstream outfile_path;
  outfile_path << print_recv_path << "/receiver/recv_from_ue_" << 9999 << ".csv";
  ofstream out (outfile_path.str ());
  out << "UE_id "
      << ","
      << "UE_ip_current "
      << ","
      << "Recv time[s] "
      << ","
      << "Sequence num"
      << ","
      << "Parket size[byte]"
      << ","
      << "Sent time[s]"
      << ","
      << "Delay[ms]"
      << "" << endl;
    out.close ();
}

NodeUEhelper::~NodeUEhelper ()
{
}

void
NodeUEhelper::connect_to_Ap (uint32_t i_UE, Ssid ssid, YansWifiPhyHelper &wifiPhy,
                             Ipv4AddressHelper &ipAddr)
{

  if (is_de_init[i_UE] == false)
    {
      WifiHelper wifi;
      wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode",
                                    StringValue ("OfdmRate54Mbps"));
      WifiMacHelper mac_sta;
      mac_sta.SetType ("ns3::StaWifiMac", "Ssid", SsidValue (ssid));
      NDC_UEs[i_UE] = wifi.Install (wifiPhy, mac_sta, NC_UEs.Get (i_UE));
      interfaces[i_UE] = (ipAddr.Assign (NDC_UEs[i_UE]));
    }
  else
    {
      //NS_LOG_UNCOND ("get WifiNetDevice");
      Ptr<WifiNetDevice> wnd = NDC_UEs[i_UE].Get (0)->GetObject<WifiNetDevice> ();
      //NS_LOG_UNCOND (wnd);
      Ptr<WifiMac> mac_sta = wnd->GetMac ();
      mac_sta->SetSsid (ssid);
      ostringstream msg;
      msg << "set " << i_UE << "-UE MAC SSID: " << ssid;
      NS_LOG_INFO (msg.str ());
    }
}

void
NodeUEhelper::connect_to_UAV (uint32_t i_UE, YansWifiPhyHelper &wifiPhy, NodeUAVhelper &uavhelper,
                              uint32_t i_UAV)
{
  if (is_de_init[i_UE] == false)
    {
      stringstream msg_connect_to_uav;
      msg_connect_to_uav << "  UE: " << i_UE << " Connect to UAV: " << i_UAV;
      NS_LOG_UNCOND (msg_connect_to_uav.str ());
      Ssid ssid = uavhelper.get_UAV_SSID (i_UAV);

      connect_to_Ap (i_UE, ssid, wifiPhy, uavhelper.ipAddrs_ap[i_UAV]);
      int flag_index = 0;
      while (uavhelper.ip_flag[i_UAV][flag_index] == true)
        {
          flag_index++;
        }
      uavhelper.ip_flag[i_UAV][flag_index] = true;
      /*       Ptr<Ipv4> ipv4proto=NC_UEs.Get(i_UE)->GetObject<Ipv4>();
      int32_t ifIndex=ipv4proto->GetInterfaceForDevice (NDC_UEs[i_UE].Get(0));
      string ip=uavhelper.get_new_Address(i_UAV);
      Ipv4InterfaceAddress ipv4Addr = Ipv4InterfaceAddress (ip.c_str(),Ipv4Mask ("/24"));
      ipv4proto->AddAddress (ifIndex, ipv4Addr);
 */
      is_de_init[i_UE] = true;
      connect_uav_index[i_UE] = i_UAV;
    }
  else if (connect_uav_index[i_UE] != i_UAV)
    {
      stringstream msg_connect_to_uav;
      msg_connect_to_uav << "  UE: " << i_UE << " Connect to UAV: " << i_UAV;
      NS_LOG_UNCOND (msg_connect_to_uav.str ());
      Ssid ssid = uavhelper.get_UAV_SSID (i_UAV);
      Ptr<Ipv4> ipv4 = NC_UEs.Get (i_UE)->GetObject<Ipv4> ();
      uint32_t index = ipv4->GetInterfaceForDevice (NDC_UEs[i_UE].Get (0));
      Ipv4Address ip_remove = ipv4->GetAddress (index, 0).GetLocal ();
      stringstream msg_remove;
      msg_remove << "Ip Address remove:" << ip_remove;
      NS_LOG_INFO (msg_remove.str ());
      ipv4->RemoveAddress (index, 0);
      //获取子网的ip编号，设定ip_flag为false
      int flag = 0;
      uint32_t j;
      stringstream ss;
      ss << ip_remove;
      string ip_str = ss.str ();
      for (long unsigned int index = 0; index < ip_str.length (); index++)
        {
          if (ip_str[index] == '.')
            flag++;
          if (flag == 3)
            {
              j = atoi (ip_str.substr (index + 1).c_str ());
              break;
            }
        }
      uavhelper.ip_flag[i_UAV][j] = false;
      //设置新地址
      string ip = uavhelper.get_new_Address (i_UAV);
      stringstream msg_setnew;
      msg_setnew << "set new address:" << ip;
      NS_LOG_INFO (msg_setnew.str ());
      Ipv4InterfaceAddress ipv4Addr = Ipv4InterfaceAddress (ip.c_str (), Ipv4Mask ("/24"));
      ipv4->AddAddress (index, ipv4Addr);
      //更改Ssid
      connect_to_Ap (i_UE, ssid, wifiPhy, uavhelper.ipAddrs_ap[i_UAV]);
      connect_uav_index[i_UE] = i_UAV;
    }
}

Vector
NodeUEhelper::getUEPosition (uint32_t i)
{

  Ptr<MobilityModel> posi_model_ue = this->NC_UEs.Get (i)->GetObject<MobilityModel> ();
  Vector ret = posi_model_ue->GetPosition ();
  return ret;
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

DataRateValue
NodeUEhelper::getDataRate (uint32_t i)
{
  DataRateValue ret;
  Ptr<OnOffApplication> onoff = dynamic_cast<OnOffApplication *> (GetPointer (app_c[i].Get (0)));
  onoff->GetAttribute ("DataRate", ret);
  return ret;
}

string
NodeUEhelper::getOnoffState (uint32_t i)
{
  string state;
  if (onoffstate[i] == 1)
    state = "on";
  else
    state = "off";
  return state;
}

void
NodeUEhelper::init_UEs (InternetStackHelper &internet_stack)
{
  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode",
                                StringValue ("OfdmRate54Mbps"));
  setMobility ();
  internet_stack.Install (NC_UEs);
}

void
NodeUEhelper::printReceivedPacket (uint32_t node_index, Ptr<Socket> socket, Ptr<Packet> packet,
                                   Address senderAddress)
{
  std::ostringstream oss;

  oss << "Simulation Time:" << Simulator::Now ().GetSeconds () << " Receiver:" << node_index;

  if (InetSocketAddress::IsMatchingType (senderAddress))
    {
      //Get ip from ip.temp
      ifstream in (print_recv_path + "ip.temp", ios::in);
      vector<Ipv4Address> v_ip;
      uint32_t ue_ip_find = 9999;
      uint32_t ue_num = 0;
      string ip;
      while (getline (in, ip))
        {
          v_ip.push_back (Ipv4Address (ip.c_str ()));
          ue_num++;
        }
      in.close ();
      InetSocketAddress addr = InetSocketAddress::ConvertFrom (senderAddress);
      Ipv4Address ip_send = addr.GetIpv4 ();
      //check ip find which UE send.
      for (uint32_t i = 0; i < ue_num; i++)
        {
          if (ip_send == v_ip[i])
            {
              ue_ip_find = i;
              break;
            }
        }

      //oss << " received one packet from " << ip_send <<endl;
      SeqTsSizeHeader stsheader;
      packet->PeekHeader (stsheader);
      uint32_t seq_num = stsheader.GetSeq ();
      double sent_time = stsheader.GetTs ().GetSeconds ();
      uint32_t size = stsheader.GetSize ();
      stringstream outfile_path;
      outfile_path << print_recv_path << "/receiver/recv_from_ue_" << ue_ip_find << ".csv";
      ofstream out (outfile_path.str (), ios::app);
      out << ue_ip_find << "," << ip_send << "," << Simulator::Now ().GetSeconds () << ","
          << seq_num << "," << size << "," << sent_time << ","
          << Simulator::Now ().GetMilliSeconds () -  stsheader.GetTs ().GetMilliSeconds()<< "" << endl;
      out.close ();
    }
  else
    {
      oss << " received one packet!";
    }
}

void
NodeUEhelper::receivePacket (Ptr<Socket> socket)
{
  Ptr<Packet> packet;
  Address senderAddress;
  Ptr<Node> receiveNode = socket->GetNode ();
  uint32_t node_index;
  for (node_index = 0; node_index < num_ueNodes; node_index++)
    {
      if (receiveNode == NC_UEs.Get (node_index))
        break;
    }
  while ((packet = socket->RecvFrom (senderAddress)))
    {
      bytesTotal[node_index] += packet->GetSize ();
      bytesTotal_timestep[node_index] += packet->GetSize ();
      packetsReceived[node_index] += 1;
      packetsReceived_timestep[node_index] += 1;
      NodeUEhelper::printReceivedPacket (node_index, socket, packet, senderAddress);
    }
}

void
NodeUEhelper::setMobility ()
{
  //设定Mobility
  MobilityHelper mobility;
  if (mobility_type == "random")
    {
      /*       mobility.SetMobilityModel (
          "ns3::GaussMarkovMobilityModel", "Bounds", BoxValue (Box (0, 300, 0, 300, 0, 300)),
          "TimeStep", TimeValue (Seconds (time_step)), "Alpha", DoubleValue (0.85), "MeanVelocity",
          StringValue ("ns3::UniformRandomVariable[Min=0|Max=3]"), "MeanDirection",
          StringValue ("ns3::UniformRandomVariable[Min=0|Max=6.283185307]"), "MeanPitch",
          StringValue ("ns3::UniformRandomVariable[Min=0.05|Max=0.05]"), "NormalVelocity",
          StringValue ("ns3::NormalRandomVariable[Mean=0.0|Variance=0.0|Bound=0.0]"),
          "NormalDirection",
          StringValue ("ns3::NormalRandomVariable[Mean=0.0|Variance=0.2|Bound=0.4]"), "NormalPitch",
          StringValue ("ns3::NormalRandomVariable[Mean=0.0|Variance=0.02|Bound=0.04]")); */
      mobility.SetMobilityModel ("ns3::RandomDirection2dMobilityModel", "Bounds",
                                 RectangleValue (Rectangle (0, 300, 0, 300)), "Speed",
                                 StringValue ("ns3::ConstantRandomVariable[Constant=2]"), "Pause",
                                 StringValue ("ns3::ConstantRandomVariable[Constant=0.2]"));
      mobility.SetPositionAllocator ("ns3::RandomBoxPositionAllocator", "X",
                                     StringValue ("ns3::UniformRandomVariable[Min=0|Max=300]"), "Y",
                                     StringValue ("ns3::UniformRandomVariable[Min=0|Max=300]"), "Z",
                                     StringValue ("ns3::UniformRandomVariable[Min=0|Max=0]"));
    }
  if (mobility_type == "constant")
    {
      mobility.SetPositionAllocator ("ns3::GridPositionAllocator", "MinX", DoubleValue (130),
                                     "MinY", DoubleValue (130), "DeltaX", DoubleValue (10),
                                     "DeltaY", DoubleValue (10), "GridWidth", UintegerValue (4),
                                     "LayoutType", StringValue ("RowFirst"));
      mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    }

  mobility.Install (NC_UEs);
}

void
NodeUEhelper::setUEPosition (uint32_t i, Vector position)
{
  Ptr<MobilityModel> posi_model_ue = this->NC_UEs.Get (i)->GetObject<MobilityModel> ();
  posi_model_ue->SetPosition (position);
}

void
NodeUEhelper::setPacketReceive (uint32_t i, uint32_t port)
{
  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> sink = Socket::CreateSocket (NC_UEs.Get (i), tid);
  InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), port);
  sink->Bind (local);
  sink->SetRecvCallback (MakeCallback (&NodeUEhelper::receivePacket, this));
}

void
NodeUEhelper::setApplication (uint32_t i, AddressValue remoteAddress)
{
  //设定Application
  stringstream ss;
  uint64_t rate = 60 * 1024 * 8 * 1024; //60MBps
  ss << rate;
  string rate_str = ss.str () + "bps";

  onoffhelper.SetAttribute ("Remote", remoteAddress);

  Ptr<UniformRandomVariable> var = CreateObject<UniformRandomVariable> ();
  app_c[i] = onoffhelper.Install (NC_UEs.Get (i));
  app_c[i].Start (Seconds (var->GetValue (10, 11)));
  app_c[i].Stop (Seconds (100));
}

void
NodeUEhelper::setDataRate (uint32_t i, DataRateValue value)
{
  Ptr<OnOffApplication> onoff = dynamic_cast<OnOffApplication *> (GetPointer (app_c[i].Get (0)));
  onoff->SetAttribute ("DataRate", value);
}

void
NodeUEhelper::setOnOffState (uint32_t i, string state)
{
  Ptr<OnOffApplication> onoff = dynamic_cast<OnOffApplication *> (GetPointer (app_c[i].Get (0)));
  if (state == "on")
    {
      onoff->SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
      onoff->SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));
      onoffstate[i] = 1;
    }
  else if (state == "off")
    {
      onoff->SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));
      onoff->SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
      onoffstate[i] = 0;
    }
}