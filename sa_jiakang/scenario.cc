#include "scenario.h"

namespace ns3{

NS_LOG_COMPONENT_DEFINE ("scenario");
NS_OBJECT_ENSURE_REGISTERED (Scenario);

Scenario::Scenario (/* args */)
{
  this->port = 9;
  this->construction_size = 300;
  this->charge_position = Vector(0,20,0);
  this->num_uavNodes = 4;
  this->num_ueNodes = 10;
  this->num_crNodes = 1;
  this->time_step = 1;
  this->simulation_time = 100;
  this->topo_type = "test";
  this->uav_id = 1;
  this->openGymInterface = CreateObject<OpenGymInterface>(5555);
  this->SetOpenGymInterface(openGymInterface);
  //output file path setting
  if (topo_type == "test")
    m_datapath = "./scratch/sa_jiakang/test/";
  else if (topo_type == "static_full_energy")
    m_datapath = "./scratch/sa_jiakang/static_full/";
  else if (topo_type == "static_dynamic_energy")
    m_datapath = "./scratch/sa_jiakang/static_dynamic/";
  this->uavHelper = NodeUAVhelper (num_uavNodes, 300, Vector(0,20,0));
  this->crHelper = NodeUEhelper (num_crNodes, time_step, "constant", m_datapath, 300);
  this->ueHelper =
      NodeUEhelper (num_ueNodes, time_step, topo_type == "test" ? "constant" : "random", m_datapath,
                    300);

  // Physic Setting
  wifiPhy = YansWifiPhyHelper ();
  this->wifiPhy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);
  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::RangePropagationLossModel", "MaxRange", DoubleValue (150));
  this->wifiPhy.SetChannel (wifiChannel.Create ());
  init_Topo();
}

Scenario::Scenario (uint32_t num_uavNodes_arg, uint32_t num_ueNodes_arg, double time_step_arg, double simulation_time_arg,
                    string topo_type_arg, uint32_t construction_size_arg, Vector charge_posi_arg, uint32_t uav_id_arg, Ptr<OpenGymInterface> openGymInterface_arg)
{
  this->port = 9;
  this->construction_size = construction_size_arg;
  this->charge_position = charge_posi_arg;
  this->num_uavNodes = num_uavNodes_arg;
  this->num_ueNodes = num_ueNodes_arg;
  this->num_crNodes = 1;
  this->time_step = time_step_arg;
  this->simulation_time = simulation_time_arg;
  this->topo_type = topo_type_arg;
  this->uav_id = uav_id_arg;
  this->openGymInterface = openGymInterface_arg;
  this->SetOpenGymInterface(openGymInterface);
  // output file path setting
  if (topo_type == "test")
    m_datapath = "./scratch/sa_jiakang/test/";
  else if (topo_type == "static_full_energy")
    m_datapath = "./scratch/sa_jiakang/static_full/";
  else if (topo_type == "static_dynamic_energy")
    m_datapath = "./scratch/sa_jiakang/static_dynamic/";
  this->uavHelper = NodeUAVhelper (num_uavNodes, construction_size, charge_position);
  this->crHelper = NodeUEhelper (num_crNodes, time_step, "constant", m_datapath , construction_size);
  this->ueHelper = NodeUEhelper (num_ueNodes, time_step,
                                 topo_type == "test" ? "constant" : "random", m_datapath, construction_size);

  // Physic Setting
  wifiPhy = YansWifiPhyHelper ();
  this->wifiPhy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);
  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::RangePropagationLossModel", "MaxRange", DoubleValue (100));
  this->wifiPhy.SetChannel (wifiChannel.Create ());
  init_Topo();
}

Scenario::~Scenario ()
{
}

void 
Scenario::auto_connect(AddressValue remoteAddress)
{
  for(uint32_t i = 0; i < num_ueNodes; i++)
  {
    uint32_t uav_index = 0;
    double mindistance = pow(construction_size,2) * 2 ;
    for(uint32_t j = 0; j < num_uavNodes; j++)
    {
      Vector curr_ue_posi = ueHelper.getUEPosition(i);  
      Vector curr_uav_posi = uavHelper.getUAVPosition(j);
      double curr_dis = pow(curr_ue_posi.x - curr_uav_posi.x, 2) +
                        pow(curr_ue_posi.y - curr_uav_posi.y, 2);
      if ( curr_dis < mindistance) {
          mindistance = curr_dis;
          uav_index = j;
      }
    }
    ueHelper.connect_to_UAV(i, wifiPhy, uavHelper, uav_index);
  }
  
  uint32_t uav_index = 0;
  double mindistance = pow(construction_size,2) * 2 ;
  for(uint32_t j = 0; j < num_uavNodes; j++)
  {
    Vector curr_cr_posi = crHelper.getUEPosition(0);  
    Vector curr_uav_posi = uavHelper.getUAVPosition(j);
    double curr_dis = pow(curr_cr_posi.x - curr_uav_posi.x, 2) +
                      pow(curr_cr_posi.y - curr_uav_posi.y, 2);
    if ( curr_dis < mindistance) {
        mindistance = curr_dis;
        uav_index = j;
    }
  }
  crHelper.connect_to_UAV(0, wifiPhy, uavHelper, uav_index);
 }

void
Scenario::checkthroughput ()
{
  
  string sendpkt_in_timestep_file = m_datapath + "sender/sentpkt_in_" + to_string(time_step) + ".txt";
  string csv_file = m_datapath + "receiver/throughoutput.csv";
  
  uint32_t sendpkt_in_timestep = 0;
 
  
  // 获取发了多少个 
  
  std::ostringstream os;
  os << "---------- Checkthroughoutput ---------- " << endl
     << "SENTPACKET_NUM_IN_TIMESTEP ： " << SENTPACKET_NUM_IN_TIMESTEP << endl
     << "CURRENT_TIMESTEP : " << CURRENT_TIMESTEP << endl
     << "GetSeconds () / TIME_STEP : " << Simulator::Now ().GetSeconds () / TIME_STEP << endl;
  string s = os.str();
  NS_LOG_UNCOND(s);
  if (CURRENT_TIMESTEP == ((uint32_t) (Simulator::Now ().GetSeconds () / TIME_STEP)  - 1))
  {
    sendpkt_in_timestep = SENTPACKET_NUM_IN_TIMESTEP;
  }
  if ( sendpkt_in_timestep !=0)
    pktlossrate.push_back((double)crHelper.packetsReceived_timestep[0]/sendpkt_in_timestep);
  else 
    pktlossrate.push_back(0);

  stringstream throughput_msg;
  throughput_msg << "print throughput info in '" << csv_file << "'";
  NS_LOG_UNCOND (throughput_msg.str ());

  std::ofstream out (csv_file.c_str (), std::ios::app);
  double kbs = (crHelper.bytesTotal_timestep[0] * 8.0) / 1024 / time_step;
  out << (Simulator::Now ()).GetSeconds () << "," 
      << kbs << ","
      << crHelper.packetsReceived_timestep[0] << "," 
      << crHelper.bytesTotal[0] << ","
      << crHelper.packetsReceived[0] << ","
      << sendpkt_in_timestep << ","
      << pktlossrate[pktlossrate.size()-1]
      << "" << std::endl;

  out.close ();

  crHelper.bytesTotal_timestep[0] = 0;
  crHelper.packetsReceived_timestep[0] = 0;
}

void
Scenario::checkip()
{
  ofstream out(m_datapath+"ip.temp",ios::out);
  stringstream ip_temp_msg;
  ip_temp_msg << "print ip info in '" << m_datapath <<"ip.temp" << "'";
  NS_LOG_UNCOND (ip_temp_msg.str ());
  for (uint32_t i = 0; i < num_ueNodes; i++)
  {
    Ptr<Ipv4> ipv4 = ueHelper.NC_UEs.Get (i)->GetObject<Ipv4> ();
    uint32_t index = ipv4->GetInterfaceForDevice (ueHelper.NDC_UEs[i].Get (0));
    Ipv4Address ip = ipv4->GetAddress (index, 0).GetLocal ();
    out<<ip<<endl;
  }
  out.close();
}

void
Scenario::checksenderinfo()
{
  NS_LOG_UNCOND("print sender info in"+m_datapath);
  for (uint32_t i = 0; i < num_ueNodes; i++)
  {
    string csv_file;
    stringstream file_stream;
    file_stream<<m_datapath<<"sender/ue_"<<i<<".csv";
    csv_file=file_stream.str();
    ofstream out(csv_file.c_str(),ios::app);
    Ptr<Ipv4> ipv4 = ueHelper.NC_UEs.Get (i)->GetObject<Ipv4> ();
    uint32_t index = ipv4->GetInterfaceForDevice (ueHelper.NDC_UEs[i].Get (0));
    Ipv4Address ip = ipv4->GetAddress (index, 0).GetLocal ();
    out <<Simulator::Now().GetSeconds()<<","
        <<ueHelper.connect_uav_index[i]<<","
        <<ip<<","
        <<ueHelper.getUEPosition(i)<<","
        <<ueHelper.getUEBlock(i)<<","
        <<ueHelper.onoffstate[i]<<","
        <<ueHelper.getDataRate(i).Get()<<endl;
    out.close();
  }
}

void
Scenario::CourseChangeCallback (std::string path, Ptr<const MobilityModel> model)
{
  Vector position = model->GetPosition ();
  std::cout << "CourseChange " << path << " x=" << position.x << ", y=" << position.y
            << ", z=" << position.z << std::endl;
}

YansWifiPhyHelper
Scenario::get_wifiPhy ()
{
  return this->wifiPhy;
}

void
Scenario::init_Topo ()
{
  InternetStackHelper internet_stack;
  Ipv4AddressHelper ipAddrs;
  uavHelper.init_UAVs (wifiPhy, internet_stack);

  if (topo_type == "test")
    init_Topo_test (internet_stack);
  if (topo_type == "static_full_energy")
    {
      init_Topo_static (internet_stack);
    }
  if (topo_type == "static_dynamic_energy")
    {
      init_Topo_static (internet_stack);
    }
}

void
Scenario::init_Topo_test (InternetStackHelper &internet_stack)
{
  NS_LOG_UNCOND ("Initial Topo: Test");
  NS_LOG_UNCOND ("Set  UAVs in the position of center in construction site");
  uavHelper.setUAVPosition (0, Vector (50, 50, 10));  
  uavHelper.setUAVPosition (1, Vector (100, 135, 10));
  // uavHelper.setUAVPosition (1, Vector (185, 85, 10));
  // uavHelper.setUAVPosition (2, Vector (100, 135, 10));
  // uavHelper.setUAVPosition (3, Vector (170, 205, 10)); 

  ueHelper.init_UEs (internet_stack);
  crHelper.init_UEs (internet_stack);
  crHelper.setUEPosition (0, Vector (0, 0, 0));
  crHelper.connect_to_UAV (0, wifiPhy, uavHelper, 0);
  crHelper.setPacketReceive (0, port);
  
  //setting connection
  for (uint32_t i = 0; i < num_ueNodes; i++)
    {
      uint32_t block = ueHelper.getUEBlock (i);
      switch (block)
        {
        case 1:
          ueHelper.connect_to_UAV (i, wifiPhy, uavHelper, 0);
          break;
        case 3:
          ueHelper.connect_to_UAV (i, wifiPhy, uavHelper, 1);
          break;
        case 7:
          ueHelper.connect_to_UAV (i, wifiPhy, uavHelper, 2);
          break;
        case 9:
          ueHelper.connect_to_UAV (i, wifiPhy, uavHelper, 3);
          break;
        default:
          ueHelper.connect_to_UAV (i, wifiPhy, uavHelper, 1);
          break;
        }
      AddressValue remoteAddress (InetSocketAddress (crHelper.interfaces[0].GetAddress (0), port));
      ueHelper.setApplication (i, remoteAddress);
    }



  //setting Application
  Simulator::Schedule (Seconds (30), &ue_app_datarate_handler, this, 1,
                       DataRateValue (DataRate ("4096bps")));
  for (uint32_t i = 0; i < num_ueNodes; i++)
    {
      Simulator::Schedule (Seconds (70), &ue_app_state_handler, this, i, "off");
    }
  Simulator::Schedule (Seconds (90), &uav_state_handler, this, 0, "down");

  // print_header
  print_header();

  // run time_step handler
  timestep_handler ();

  // Animation Setting
  string animfile = m_datapath + "anim.xml";
  AnimationInterface anim (animfile.c_str ());
  anim.EnablePacketMetadata (true);
  for (uint32_t i = 0; i < num_uavNodes; i++)
    {
      stringstream ss;
      ss << i;
      string name = "UAV" + ss.str ();
      anim.UpdateNodeDescription (i, name);
      anim.UpdateNodeSize (i, 8, 8);
    }
  anim.UpdateNodeDescription (num_uavNodes, "CR");
  anim.UpdateNodeSize (num_uavNodes, 10, 10);
  for (uint32_t i = 0; i < num_ueNodes; i++)
    {
      stringstream ss;
      ss << i;
      string name = "UE" + ss.str ();
      anim.UpdateNodeDescription (i + num_uavNodes + 1, name);
      anim.UpdateNodeSize (i + num_uavNodes + 1, 5, 5);
    }

  // Start simulatation
  Simulator::Stop (Seconds (100));
  Simulator::Run ();
  Simulator::Destroy ();
}

void
Scenario::init_Topo_static (InternetStackHelper &internet_stack)
{
  NS_LOG_UNCOND ("Initial Topo: Static Full Energy");
  stringstream msg_info;
  msg_info << endl << "----------------Basic Information------------" << endl;
  NS_LOG_UNCOND ("Time Step : " << time_step << ", Simulation Time : " << simulation_time);

  NS_LOG_UNCOND ("Set UAV position in specfic position");
  for (uint32_t i = 0; i < this->num_uavNodes; i++)
  {
    this->uavHelper.setUAVPosition(i,charge_position);
  }
  uavHelper.setUAVPosition (0, Vector (50, 50, 10));  
  uavHelper.setUAVPosition (1, Vector (185, 85, 10));
  uavHelper.setUAVPosition (2, Vector (100, 135, 10));
  uavHelper.setUAVPosition (3, Vector (170, 205, 10)); 
    for (uint32_t i = 0; i < this->num_uavNodes; i++)
  {
    this->last_posi.push_back(uavHelper.getUAVPosition(i));
  }

/*   
  msg_info << "---------------------------------------------" << endl
           << "------------------Postion UAV----------------" << endl
           << "--- UAV0:" << uavHelper.getUAVPosition (0) << endl
           << "--- UAV1:" << uavHelper.getUAVPosition (1) << endl
           << "--- UAV2:" << uavHelper.getUAVPosition (2) << endl
           << "--- UAV3:" << uavHelper.getUAVPosition (3) << endl;
 */

  NS_LOG_UNCOND ("Set UE postition in 3 Area");
  msg_info << "---------------------------------------------" << endl
           << "------------------Postion UE-----------------" << endl;
  ueHelper.init_UEs (internet_stack);
  uint32_t num_ue_ingroup = num_ueNodes / 3;
  double x_mean = 200;
  double y_mean = 50;
  double variance = 20;
  msg_info << "--- Area1: NormalRandom" << endl
           << "--- mean = [" << x_mean << "," << y_mean << "]" << endl
           << "--- variance = " << variance << endl
           << "--- " << endl;
  for (uint32_t i = 0; i < num_ue_ingroup; i++)
    {
      Ptr<NormalRandomVariable> x = CreateObject<NormalRandomVariable> ();
      x->SetAttribute ("Mean", DoubleValue (x_mean));
      x->SetAttribute ("Variance", DoubleValue (variance));
      x->SetAttribute ("Bound", DoubleValue (50));
      Ptr<NormalRandomVariable> y = CreateObject<NormalRandomVariable> ();
      y->SetAttribute ("Mean", DoubleValue (y_mean));
      y->SetAttribute ("Variance", DoubleValue (variance));
      y->SetAttribute ("Bound", DoubleValue (20));
      double x_value = x->GetValue ();
      double y_value = y->GetValue ();
      double z_value = 0;
      ueHelper.setUEPosition (i, Vector (x_value, y_value, z_value));
    }
  x_mean = 100;
  y_mean = 200;
  variance = 20;
  msg_info << "--- Area2: NormalRandom" << endl
           << "--- mean = [" << x_mean << "," << y_mean << "]" << endl
           << "--- variance = " << variance << endl
           << "--- " << endl;
  for (uint32_t i = num_ue_ingroup; i < num_ue_ingroup * 2; i++)
    {
      Ptr<NormalRandomVariable> x = CreateObject<NormalRandomVariable> ();
      x->SetAttribute ("Mean", DoubleValue (x_mean));
      x->SetAttribute ("Variance", DoubleValue (variance));
      x->SetAttribute ("Bound", DoubleValue (20));
      Ptr<NormalRandomVariable> y = CreateObject<NormalRandomVariable> ();
      y->SetAttribute ("Mean", DoubleValue (y_mean));
      y->SetAttribute ("Variance", DoubleValue (variance));
      y->SetAttribute ("Bound", DoubleValue (20));
      double x_value = x->GetValue ();
      double y_value = y->GetValue ();
      double z_value = 0;
      ueHelper.setUEPosition (i, Vector (x_value, y_value, z_value));
    }
  x_mean = 250;
  y_mean = 250;
  variance = 20;
  msg_info << "--- Area3: NormalRandom" << endl
           << "--- mean = [" << x_mean << "," << y_mean << "]" << endl
           << "--- variance = " << variance << endl
           << "--- " << endl;
  for (uint32_t i = num_ue_ingroup * 2; i < num_ueNodes; i++)
    {
      Ptr<NormalRandomVariable> x = CreateObject<NormalRandomVariable> ();
      x->SetAttribute ("Mean", DoubleValue (x_mean));
      x->SetAttribute ("Variance", DoubleValue (variance));
      x->SetAttribute ("Bound", DoubleValue (20));
      Ptr<NormalRandomVariable> y = CreateObject<NormalRandomVariable> ();
      y->SetAttribute ("Mean", DoubleValue (y_mean));
      y->SetAttribute ("Variance", DoubleValue (variance));
      y->SetAttribute ("Bound", DoubleValue (20));
      double x_value = x->GetValue ();
      double y_value = y->GetValue ();
      double z_value = 0;
      ueHelper.setUEPosition (i, Vector (x_value, y_value, z_value));
    }

  NS_LOG_UNCOND ("Set BaseStation in specfic position");
  crHelper.init_UEs (internet_stack);
  crHelper.setUEPosition (0, Vector (0, 0, 0));
  msg_info << "---------------------------------------------" << endl
           << "------------------Postion CR-----------------" << endl
           << "--- position: [0,0,0]" << endl;

  NS_LOG_UNCOND ("Set Connection & Checkthroughoutput & Simulator");
  crHelper.connect_to_UAV (0, wifiPhy, uavHelper, 0);
  crHelper.setPacketReceive (0, port);
  AddressValue remoteAddress (InetSocketAddress (crHelper.interfaces[0].GetAddress (0), port));
  this->remoteAddress = remoteAddress;
  NS_LOG_INFO (crHelper.interfaces[0].GetAddress (0));

  // connect 
  // manual_connect(num_ue_ingroup, remoteAddress);
  auto_connect(remoteAddress);
  for (uint32_t i =0; i< num_ueNodes; i++) 
  {
    ueHelper.setApplication (i, remoteAddress);
  }
/*   // 手动设置链接uav
  for (uint32_t i = 0; i < num_ue_ingroup; i++)
    {
      ueHelper.connect_to_UAV (i, wifiPhy, uavHelper, 1);
      ueHelper.setApplication (i, remoteAddress);
    }
  for (uint32_t i = num_ue_ingroup; i < num_ue_ingroup * 2; i++)
    {
      ueHelper.connect_to_UAV (i, wifiPhy, uavHelper, 2);
      ueHelper.setApplication (i, remoteAddress);
    }
  for (uint32_t i = num_ue_ingroup * 2; i < num_ueNodes; i++)
    {
      ueHelper.connect_to_UAV (i, wifiPhy, uavHelper, 3);
      ueHelper.setApplication (i, remoteAddress);
    } */


  //print header
  print_header();

  if (topo_type == "static_full_energy")
    {
      timestep_handler ();
    }
  if (topo_type == "static_dynamic_energy")
    {
      timestep_handler ();
    }
  


/*   
  //随机选择速率, 暂时屏蔽
  vector<string> rate_list = {"2048bps", "4096bps", "10240bps", "102400bps", "204800bps"};

  for (uint32_t i = 0; i < num_ueNodes; i++)
    {
      Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
      x->SetAttribute ("Min", DoubleValue (0));
      x->SetAttribute ("Max", DoubleValue (rate_list.size () - 1));
      int rate_index = x->GetInteger ();
      NS_LOG_DEBUG ("current rate_index: " + to_string (rate_index));
      Simulator::Schedule (Seconds (30), &ue_app_datarate_handler, this, i,
                           DataRateValue (DataRate (rate_list[rate_index])));
      Simulator::Schedule (Seconds (80), &ue_app_datarate_handler, this, i,
                           DataRateValue (DataRate (rate_list[rate_list.size () - 1])));
    }
     */
  //Simulator::Schedule (Seconds (90), &uav_state_handler, this, 0, "down");

  // Animation Setting
  string animfile;
  if (topo_type == "static_full_energy")
    {
      animfile = m_datapath + "full_energy.xml";
    }
  if (topo_type == "static_dynamic_energy")
    {
      animfile = m_datapath + "dynamic_energy.xml";
    }
  AnimationInterface anim (animfile.c_str ());
  //anim.EnablePacketMetadata (true);
  anim.SetMaxPktsPerTraceFile (99999999999999);
  for (uint32_t i = 0; i < num_uavNodes; i++)
    {
      stringstream ss;
      ss << i;
      string name = "UAV" + ss.str ();
      anim.UpdateNodeDescription (i, name);
      anim.UpdateNodeSize (i, 8, 8);
    }
  anim.UpdateNodeDescription (num_uavNodes, "CR");
  anim.UpdateNodeSize (num_uavNodes, 10, 10);
  for (uint32_t i = 0; i < num_ueNodes; i++)
    {
      stringstream ss;
      ss << i;
      string name = "UE" + ss.str ();
      anim.UpdateNodeDescription (i + num_uavNodes + 1, name);
      anim.UpdateNodeSize (i + num_uavNodes + 1, 5, 5);
    }
  NS_LOG_INFO (msg_info.str ());

  // Start simulatation

  NS_LOG_UNCOND ("Start Simulation");
  Simulator::Stop (Seconds (simulation_time));
  Simulator::Run ();

  NS_LOG_UNCOND ("Simulation stop");
  

  openGymInterface->NotifySimulationEnd();
  Simulator::Destroy();
  

}

void
Scenario::manual_connect(uint32_t num_ue_ingroup, AddressValue remoteAddress)
{
    // 手动设置链接uav
  for (uint32_t i = 0; i < num_ue_ingroup; i++)
    {
      ueHelper.connect_to_UAV (i, wifiPhy, uavHelper, 1);
    }
  for (uint32_t i = num_ue_ingroup; i < num_ue_ingroup * 2; i++)
    {
      ueHelper.connect_to_UAV (i, wifiPhy, uavHelper, 2);
    }
  for (uint32_t i = num_ue_ingroup * 2; i < num_ueNodes; i++)
    {
      ueHelper.connect_to_UAV (i, wifiPhy, uavHelper, 3);
    }

}

void
Scenario::print_header()
{
    // throuphoutput header;
  string csv_file = m_datapath + "/receiver/throughoutput.csv";
  std::ofstream out (csv_file.c_str ());
  out << "Simulation Second,"
      << "Receive Rate[kbps],"
      << "Packets Received in " << time_step << ","
      << "Byte Received Total,"
      << "Packets Received Total," 
      << "Packets Sent in " <<time_step << "," 
      << "Packets Loss Rate "<<std::endl;
  out.close ();
  
  // print ip_temp in file
  out=ofstream(m_datapath+"ip.temp");
  for (uint32_t i = 0; i < num_ueNodes; i++)
  {
    Ptr<Ipv4> ipv4 = ueHelper.NC_UEs.Get (i)->GetObject<Ipv4> ();
    uint32_t index = ipv4->GetInterfaceForDevice (ueHelper.NDC_UEs[i].Get (0));
    Ipv4Address ip = ipv4->GetAddress (index, 0).GetLocal ();
    out<<ip<<endl;
  }
  out.close();
  
  // sender info header
    //for each ue
  for (uint32_t i = 0; i < num_ueNodes; i++)
  {
    stringstream file_stream;
    file_stream<<m_datapath<<"sender/ue_"<<i<<".csv";
    csv_file=file_stream.str();
    out=ofstream(csv_file.c_str());
    out <<"Simulation Time"<<","
        <<"Connect with UAV"<<","
        <<"Current ip"<<","
        <<"Current postion"<<","
        <<"Current block"<<","
        <<"Sender State"<<","
        <<"Sender Rate"<<endl;
    out.close();
  }
}

void
Scenario::rl_test ()
{
  //修正路由
  for (uint32_t i = 0; i < num_ueNodes; i++)
    {
      uint32_t block = ueHelper.getUEBlock (i);
      switch (block)
        {
        case 1:
          ueHelper.connect_to_UAV (i, wifiPhy, uavHelper, 0);
          break;
        case 3:
          ueHelper.connect_to_UAV (i, wifiPhy, uavHelper, 1);
          break;
        case 5:
          ueHelper.connect_to_UAV (i, wifiPhy, uavHelper, 1);
          break;
        case 7:
          ueHelper.connect_to_UAV (i, wifiPhy, uavHelper, 2);
          break;
        case 9:
          ueHelper.connect_to_UAV (i, wifiPhy, uavHelper, 3);
          break;
        default:
          ueHelper.connect_to_UAV (i, wifiPhy, uavHelper, 1);
          break;
        }
    }
}

void
Scenario::rl_static_full_energy ()
{
}

void
Scenario::rl_static_dynamic_energy ()
{
}

void
Scenario::timestep_handler ()
{
  stringstream log_time;
  log_time << "Simulation Time: " << Simulator::Now ().GetSeconds ()
           << " ------------------------------------------------------";
  NS_LOG_UNCOND (log_time.str ());

  // openGymInterface->NotifyCurrentState();

  
  if (topo_type == "test")
    rl_test ();
  else if (topo_type == "static_full_energy")
    rl_static_full_energy ();
  else if (topo_type == "static_dynamic_energy")
    rl_static_dynamic_energy ();
  
  // print throughput
  this->checkthroughput ();
  // print connect information flow
  this->checksenderinfo();
  // print all ip_address in to file for receive
  this->checkip();
  Simulator::Schedule (Seconds (time_step), &Scenario::timestep_handler, this);
  Notify();
}

void
Scenario::ue_app_datarate_handler (Scenario *scenario, uint32_t i, DataRateValue value)
{
  scenario->ueHelper.setDataRate (i, value);
}

void
Scenario::ue_app_state_handler (Scenario *scenario, uint32_t i, string state)
{
  scenario->ueHelper.setOnOffState (i, state);
}

void
Scenario::uav_state_handler (Scenario *scenario, uint32_t i, string state)
{
  if (state == "down")
    scenario->uavHelper.set_down (i);
  else if (state == "up")
    scenario->uavHelper.set_up (i);
}



TypeId
Scenario::GetTypeId (void)
{
  static TypeId tid = TypeId ("Scenario")
    .SetParent<OpenGymEnv> ()
    .SetGroupName ("OpenGym")
    .AddConstructor<Scenario> ()
  ;
  return tid;
}

void
Scenario::DoDispose ()
{
  NS_LOG_FUNCTION (this);
}

Ptr<OpenGymSpace>
Scenario::GetObservationSpace()
{
// UE的状态，9个block中UE的个数  
  uint32_t blocknum = 9;
  uint32_t min_ues_inblock = 0;
  uint32_t max_ues_inblock = 100;
  vector<uint32_t> shape_block = {blocknum,};
  string dtype_block = TypeNameGet<uint32_t> ();
  Ptr<OpenGymBoxSpace> block_box = 
      CreateObject<OpenGymBoxSpace>(min_ues_inblock, max_ues_inblock, shape_block, dtype_block);
  
  // UAV position 的状态
  uint32_t length_uav_posi = 3;
  double min_posi = 0;
  double max_posi = construction_size;
  vector<uint32_t> shape_uav_posi = {length_uav_posi * num_uavNodes,};
  string dtype_uav_posi = TypeNameGet<double>();
  Ptr<OpenGymBoxSpace> uav_posi_box =
      CreateObject<OpenGymBoxSpace> (min_posi, max_posi, shape_uav_posi, dtype_uav_posi);
  
  // UAV battery 的状态
  uint32_t min_battery = 0;
  uint32_t max_battery = 100;
  vector<uint32_t> shape_uav_battery = {num_uavNodes,}; 
  string dtype_uav_battery = TypeNameGet<uint32_t>();
  Ptr<OpenGymBoxSpace> uav_battery_box = 
      CreateObject<OpenGymBoxSpace>(min_battery, max_battery, shape_uav_battery, dtype_uav_battery);

  //丢标率的状态
  double min_pktlossrate = 0.0;
  double max_pktlossrate = 1.0;
  vector<uint32_t> shape_pktlossrate = {1,};
  string dtype_pktlossrate = TypeNameGet<double>();
  Ptr<OpenGymBoxSpace> pktlossrate_box = 
      CreateObject<OpenGymBoxSpace>(min_pktlossrate, max_pktlossrate, shape_pktlossrate, dtype_pktlossrate);
      

  
  // 总状态 
  Ptr<OpenGymDictSpace> space = CreateObject<OpenGymDictSpace> ();
  space->Add("block", block_box);
  space->Add("uav_posi", uav_posi_box);
  space->Add("uav_battery", uav_battery_box);
  space->Add("pktlossrate", pktlossrate_box);

  NS_LOG_UNCOND ("MyGetObservationSpace: " << space);
  return space;
}

/*
Define action space
*/
Ptr<OpenGymSpace>
Scenario::GetActionSpace()
{
  // 工作,充电,上下左右,回到充电前的位置
  uint32_t min_action = 0;
  uint32_t max_action = 5;
  vector<uint32_t> shape_action_space = {num_uavNodes,};
  string dtype_action_space = TypeNameGet<uint32_t>();
  Ptr<OpenGymBoxSpace> action_space = 
    CreateObject<OpenGymBoxSpace>(min_action, max_action, shape_action_space, dtype_action_space);
  return action_space;
}

/*
Define game over condition
*/
bool
Scenario::GetGameOver()
{
  // 暂时先没动
  NS_LOG_UNCOND("GetGameOver: " << Seconds(simulation_time) << " & " << Simulator::Now());
  static double stepCounter = 0.0;
  stepCounter += 1;
  for (uint32_t i = 0; i < num_uavNodes; i++)
  {
    if (uavHelper.getUAVbattery(i) <= 0 ) return true;
  }
  if ( stepCounter >= (double)simulation_time/(double)time_step) {
      return true;
  }
  return false;
}

/*
Collect observations
*/
Ptr<OpenGymDataContainer>
Scenario::GetObservation()
{
  NS_LOG_UNCOND("GetObservation");
  // 获取 ue 的Oberservation
  uint32_t num_block = 9;
  vector<uint32_t> ue_in_blocks_num(9,0);
  vector<uint32_t> shape_block = {num_block,};
  Ptr<OpenGymBoxContainer<uint32_t>> box_block = CreateObject<OpenGymBoxContainer<uint32_t>>(shape_block);
  for(uint32_t i = 0; i < num_ueNodes; i++)
  {
    ue_in_blocks_num[ueHelper.getUEBlock(i)-1]++;
  }
  for(uint32_t i = 0; i < ue_in_blocks_num.size(); i++)
  {
    box_block->AddValue(ue_in_blocks_num[i]);
  }
  
  // 获取 uav_id 的 position
  uint32_t length_uav_posi = 3;
  vector<uint32_t> shape_posi = {length_uav_posi * num_uavNodes,};
  Ptr<OpenGymBoxContainer<double>> box_posi = CreateObject<OpenGymBoxContainer<double>>(shape_posi);
  for(uint32_t uav_id = 0; uav_id < num_uavNodes; uav_id++)
  {
    vector<double> uav_posi = {
      this->uavHelper.getUAVPosition(uav_id).x,
      this->uavHelper.getUAVPosition(uav_id).y,
      this->uavHelper.getUAVPosition(uav_id).z
    };

    for(uint32_t i = 0; i < 3; i++)
    {
      box_posi->AddValue(uav_posi[i]);
    }
  }
  


  // 获取uav_id的battery
  vector<uint32_t> shape_battery = {num_uavNodes,};
  Ptr<OpenGymBoxContainer<uint32_t>> box_battery = CreateObject<OpenGymBoxContainer<uint32_t>>(shape_battery);
  for(uint32_t uav_id = 0; uav_id < num_uavNodes; uav_id++)
  {
    box_battery->AddValue(uavHelper.getUAVbattery(uav_id));
  }

  // 获取丢包率的状态
  vector<uint32_t> shape_pktlossrate = {1,};
  Ptr<OpenGymBoxContainer<double>> box_pktlossrate = CreateObject<OpenGymBoxContainer<double>>(shape_pktlossrate);
  double pktlossrate = 0; // 没有发送也没有接收
  if ( this->pktlossrate.size()!=0 )
  {
    pktlossrate = this->pktlossrate[this->pktlossrate.size()-1];
  }
  box_pktlossrate->AddValue(pktlossrate);

  Ptr<OpenGymDictContainer> observation = CreateObject<OpenGymDictContainer>();
  observation->Add("block", box_block);
  observation->Add("uav_posi", box_posi);
  observation->Add("uav_battery", box_battery);
  observation->Add("pktlossrate", box_pktlossrate);

  return observation;

}

/*
Define reward function
*/
float
Scenario::GetReward()
{
  // pktlossrate & battery
  NS_LOG_UNCOND("GetReward");
  static float reward = 0.0;
  double weight_battery = 0.5;
  double weight_pktlossrate = 1 - weight_battery;

  for(uint32_t uav_id = 0; uav_id < num_uavNodes; uav_id++)
  {
    // battery = 0; 
    if (uavHelper.getUAVbattery(uav_id) <= 0){
      reward = -10;
      return reward;
    }
  }

  float mean_battery = 0.0;
  for(uint32_t uav_id = 0; uav_id < num_uavNodes; uav_id++)
  {
    mean_battery += (float)uavHelper.getUAVbattery(uav_id);
  }
  mean_battery /= num_uavNodes;

  if (pktlossrate.size() == 0 )
  {
    reward = mean_battery/ 100;
  }
  else
  {
    reward = weight_battery * mean_battery / 100 + 
             weight_pktlossrate * (float) pktlossrate[pktlossrate.size()-1];
  }

  return reward;
}

/*
Define extra info. Optional
*/
std::string
Scenario::GetExtraInfo()
{
  string myInfo = "testInfo : ";
  myInfo += "by now no use!";
  return myInfo;
}

uint32_t
Scenario::Get_uav_id()
{
  return uav_id;
}

void 
Scenario::Set_uav_id(uint32_t uav_id_arg)
{
  this->uav_id = uav_id_arg;
}

/*
Execute received actions
*/
bool
Scenario::ExecuteActions(Ptr<OpenGymDataContainer> action)
{
  NS_LOG_UNCOND("ExecuteAcitons");
  Ptr<OpenGymBoxContainer<uint32_t>> action_space = DynamicCast<OpenGymBoxContainer<uint32_t>>(action);
  
  for (uint32_t uav_id = 0; uav_id < num_uavNodes; uav_id++)
  {
    uint32_t action_val = action_space->GetValue(uav_id);
    uint32_t curr_battery = uavHelper.getUAVbattery(uav_id);
    double posi_x = 0;
    double posi_y = 0;
    switch (action_val)
    {
      
      case 0: // work, battery - 10
        /*         
        if(curr_battery > 0 ) uavHelper.set_up(uav_id);
        else uavHelper.set_down(uav_id);
        */
        if( curr_battery >= 10 ) uavHelper.setUAVbattery(uav_id, curr_battery - 10);
        else uavHelper.setUAVbattery(uav_id, 0);
        /*
        if( curr_battery <= 0 ) uavHelper.set_down(uav_id); 
        */
        break;
      
      case 1: //charge, battery + 20
        // uavHelper.set_down(uav_id);
        // if(uavHelper.getUAVPosition(uav_id)!= charge_position) last_posi[uav_id] = uavHelper.getUAVPosition(uav_id);
        if( curr_battery <=80 ) uavHelper.setUAVbattery(uav_id, curr_battery+20);
        else uavHelper.setUAVbattery(uav_id,100);
        uavHelper.setUAVPosition(uav_id, charge_position);
        break;

      case 2: //up , battery - 15, posi_step = 50
        if( curr_battery >=15 ) uavHelper.setUAVbattery(uav_id, curr_battery-15);
        else uavHelper.setUAVbattery(uav_id, 0);
        posi_y = uavHelper.getUAVPosition(uav_id).y;
        if( posi_y >= 50 )
        {
          uavHelper.setUAVPosition(uav_id, Vector(
            uavHelper.getUAVPosition(uav_id).x,
            posi_y - 50,
            uavHelper.getUAVPosition(uav_id).z
          ));
        }
        else
        {
          uavHelper.setUAVPosition(uav_id, Vector(
            uavHelper.getUAVPosition(uav_id).x,
            0,
            uavHelper.getUAVPosition(uav_id).z
          ));
        }
        auto_connect(remoteAddress);
        break;
      
      case 3: //down , battery - 15, posi_step = 50 
        if( curr_battery >=15 ) uavHelper.setUAVbattery(uav_id, curr_battery-15);
        else uavHelper.setUAVbattery(uav_id, 0);
        posi_y = uavHelper.getUAVPosition(uav_id).y;
        if( posi_y <= (double)construction_size - 50 )
        {
          uavHelper.setUAVPosition(uav_id, Vector(
            uavHelper.getUAVPosition(uav_id).x,
            posi_y + 50,
            uavHelper.getUAVPosition(uav_id).z
          ));
        }
        else
        {
          uavHelper.setUAVPosition(uav_id, Vector(
            uavHelper.getUAVPosition(uav_id).x,
            (double)construction_size,
            uavHelper.getUAVPosition(uav_id).z
          ));
        }
        auto_connect(remoteAddress);
        break;

      case 4: //left , battery - 15, posi_step = 50 
        if( curr_battery >= 15 ) uavHelper.setUAVbattery(uav_id, curr_battery-15);
        else uavHelper.setUAVbattery(uav_id, 0);
        posi_x = uavHelper.getUAVPosition(uav_id).x;
        if( posi_x >= 50 )
        {
          uavHelper.setUAVPosition(uav_id, Vector(
            posi_x - 50,
            uavHelper.getUAVPosition(uav_id).y,
            uavHelper.getUAVPosition(uav_id).z
          ));
        }
        else
        {
          uavHelper.setUAVPosition(uav_id, Vector(
            0,
            uavHelper.getUAVPosition(uav_id).y,
            uavHelper.getUAVPosition(uav_id).z
          ));
        }
        auto_connect(remoteAddress);
        break;

      case 5: //right , battery - 15, posi_step = 50 
        if( curr_battery >=15 ) uavHelper.setUAVbattery(uav_id, curr_battery-15);
        else uavHelper.setUAVbattery(uav_id, 0);
        posi_x = uavHelper.getUAVPosition(uav_id).x;
        if( posi_x <= (double)construction_size - 50 )
        {
          uavHelper.setUAVPosition(uav_id, Vector(
            posi_x + 50,
            uavHelper.getUAVPosition(uav_id).y,
            uavHelper.getUAVPosition(uav_id).z
          ));
        }
        else
        {
          uavHelper.setUAVPosition(uav_id, Vector(
            (double)construction_size,
            uavHelper.getUAVPosition(uav_id).y,
            uavHelper.getUAVPosition(uav_id).z
          ));
        }
        auto_connect(remoteAddress);
        break;
/*       case 6: // go back to last position before charge
        if( curr_battery >=15 ) uavHelper.setUAVbattery(uav_id, curr_battery-15);
        else uavHelper.setUAVbattery(uav_id, 0);
        uavHelper.setUAVPosition(uav_id,last_posi[uav_id]);
        break; */
      
      default:
        break;
    }     
  }

  NS_LOG_UNCOND("finish Excutetion");
  return true;
}


}