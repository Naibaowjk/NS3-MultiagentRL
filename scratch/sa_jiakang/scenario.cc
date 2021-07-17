#include "scenario.h"

NS_LOG_COMPONENT_DEFINE ("scenario");

Scenario::Scenario (/* args */)
    : port (9),
      num_uavNodes (4),
      num_ueNodes (10),
      num_crNodes (1),
      time_step (4),
      topo_type ("test"),
      uavHelper (NodeUAVhelper (num_uavNodes)),
      crHelper (NodeUEhelper (num_crNodes, time_step, "constant")),
      ueHelper (NodeUEhelper (num_ueNodes, time_step, topo_type == "test" ? "constant" : "random"))
{

  //Throughtout file path setting
  if (topo_type == "test")
    m_CSVfileName = "./scratch/sa_jiakang/throughput-test.csv";
  else if (topo_type == "static_full_energy")
    m_CSVfileName = "./scratch/sa_jiakang/throughput-static_full_energy.csv";
  else if (topo_type == "static_dynamic_energy")
    m_CSVfileName = "./scratch/sa_jiakang/throughput-static_dynamic_energy.csv";

  // Physic Setting
  wifiPhy = YansWifiPhyHelper::Default ();
  this->wifiPhy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);
  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::RangePropagationLossModel", "MaxRange", DoubleValue (100));
  this->wifiPhy.SetChannel (wifiChannel.Create ());
}

Scenario::Scenario (uint32_t num_uavNodes_arg, uint32_t num_ueNodes_arg, double time_step_arg,
                    string topo_type_arg)
    : port (9),
      num_uavNodes (num_uavNodes_arg),
      num_ueNodes (num_ueNodes_arg),
      num_crNodes (1),
      time_step (time_step_arg),
      topo_type (topo_type_arg),
      uavHelper (NodeUAVhelper (num_uavNodes)),
      crHelper (NodeUEhelper (num_crNodes, time_step, "constant")),
      ueHelper (NodeUEhelper (num_ueNodes, time_step, topo_type == "test" ? "constant" : "random"))
{
  //Throughtout file path setting
  if (topo_type == "test")
    m_CSVfileName = "./scratch/sa_jiakang/throughput-test.csv";
  else if (topo_type == "static_full_energy")
    m_CSVfileName = "./scratch/sa_jiakang/throughput-static_full_energy.csv";
  else if (topo_type == "static_dynamic_energy")
    m_CSVfileName = "./scratch/sa_jiakang/throughput-test_dynamic_energy.csv";

  // Physic Setting
  wifiPhy = YansWifiPhyHelper::Default ();
  this->wifiPhy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);
  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::RangePropagationLossModel", "MaxRange", DoubleValue (100));
  this->wifiPhy.SetChannel (wifiChannel.Create ());
}

Scenario::~Scenario ()
{
}

void
Scenario::checkthroughput_test ()
{
  double kbs = (crHelper.bytesTotal_timestep[0] * 8.0) / 1024 / time_step;
  stringstream log_time;
  log_time << "Simulation Time: " << Simulator::Now ().GetSeconds ()
           << " ------------------------------------------------------";
  NS_LOG_UNCOND (log_time.str ());
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

  std::ofstream out (m_CSVfileName.c_str (), std::ios::app);

  out << (Simulator::Now ()).GetSeconds () << "," << kbs << ","
      << crHelper.packetsReceived_timestep[0] << "," << crHelper.bytesTotal[0] << ","
      << crHelper.packetsReceived[0] << "" << std::endl;

  out.close ();
  crHelper.bytesTotal_timestep[0] = 0;
  crHelper.packetsReceived_timestep[0] = 0;
  Simulator::Schedule (Seconds (time_step), &Scenario::checkthroughput_test, this);
}

void
Scenario::checkthroughput_static_full_energy ()
{
  double kbs = (crHelper.bytesTotal_timestep[0] * 8.0) / 1024 / time_step;
  stringstream log_time;
  log_time << "Simulator Time: " << Simulator::Now ().GetSeconds ()
           << " ------------------------------------------------------";
  NS_LOG_UNCOND (log_time.str ());
  /* 这里添加对应的操作 */

  /* ---------------- */
  std::ofstream out (m_CSVfileName.c_str (), std::ios::app);
  out << (Simulator::Now ()).GetSeconds () << "," << kbs << ","
      << crHelper.packetsReceived_timestep[0] << "," << crHelper.bytesTotal[0] << ","
      << crHelper.packetsReceived[0] << "" << std::endl;

  out.close ();
  crHelper.bytesTotal_timestep[0] = 0;
  crHelper.packetsReceived_timestep[0] = 0;
  Simulator::Schedule (Seconds (time_step), &Scenario::checkthroughput_static_full_energy, this);
}

void
Scenario::checkthroughput_static_dynamic_energy ()
{
  double kbs = (crHelper.bytesTotal_timestep[0] * 8.0) / 1024 / time_step;
  stringstream log_time;
  log_time << "Simulation Time: " << Simulator::Now ().GetSeconds ()
           << " ------------------------------------------------------";
  NS_LOG_UNCOND (log_time.str ());
  /* 这里添加对应的操作 */

  /* ---------------- */
  std::ofstream out (m_CSVfileName.c_str (), std::ios::app);

  out << (Simulator::Now ()).GetSeconds () << "," << kbs << ","
      << crHelper.packetsReceived_timestep[0] << "," << crHelper.bytesTotal[0] << ","
      << crHelper.packetsReceived[0] << "" << std::endl;

  out.close ();
  crHelper.bytesTotal_timestep[0] = 0;
  crHelper.packetsReceived_timestep[0] = 0;
  Simulator::Schedule (Seconds (time_step), &Scenario::checkthroughput_static_dynamic_energy, this);
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
  NS_LOG_UNCOND ("Set 4 UAV in the position of center in construction site");
  uavHelper.setUAVPosition (0, Vector (100, 100, 10));
  uavHelper.setUAVPosition (1, Vector (200, 100, 10));
  uavHelper.setUAVPosition (2, Vector (100, 200, 10));
  uavHelper.setUAVPosition (3, Vector (200, 200, 10));

  ueHelper.init_UEs (internet_stack);
  crHelper.init_UEs (internet_stack);
  crHelper.setUEPosition (0, Vector (50, 50, 0));
  crHelper.connect_to_UAV (0, wifiPhy, uavHelper, 0);
  crHelper.setPacketReceive (0, port);
  //修正路由,Checkthroughtout会出错
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

  //CR 用作接受， UE用作发送
  checkthroughput_test ();

  //测试更改Application
  Simulator::Schedule (Seconds (30), &ue_app_datarate_handler, this, 2,
                       DataRateValue (DataRate ("4096bps")));
  Simulator::Schedule (Seconds (50), &ue_app_datarate_handler, this, 5,
                       DataRateValue (DataRate ("204800bps")));
  for (uint32_t i = 0; i < num_ueNodes; i++)
    {
      Simulator::Schedule (Seconds (70), &ue_app_state_handler, this, i, "off");
    }
  Simulator::Schedule (Seconds (90), &uav_state_handler, this, 0, "down");

  //输出参数表头
  //blank out the last output file and write the column headers
  std::ofstream out (m_CSVfileName.c_str ());
  out << "Simulation Second,"
      << "Receive Rate[kbps],"
      << "Packets Received in " << time_step << ","
      << "Byte Received Total,"
      << "Packets Received Total" << std::endl;
  out.close ();

  // Animation Setting
  string animfile = "./scratch/sa_jiakang/test.xml";
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

  NS_LOG_UNCOND ("Set UAV position in specfic position");
  uavHelper.setUAVPosition (0, Vector (50, 50, 10));
  uavHelper.setUAVPosition (1, Vector (185, 85, 10));
  uavHelper.setUAVPosition (2, Vector (100, 135, 10));
  uavHelper.setUAVPosition (3, Vector (170, 205, 10));
  msg_info << "---------------------------------------------" << endl
           << "------------------Postion UAV----------------" << endl
           << "--- UAV0:" << uavHelper.getUAVPosition (0) << endl
           << "--- UAV1:" << uavHelper.getUAVPosition (1) << endl
           << "--- UAV2:" << uavHelper.getUAVPosition (2) << endl
           << "--- UAV3:" << uavHelper.getUAVPosition (3) << endl;

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
  NS_LOG_INFO (crHelper.interfaces[0].GetAddress (0));
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
    }

  if (topo_type == "static_full_energy")
    {
      checkthroughput_static_full_energy ();
    }
  if (topo_type == "static_dynamic_energy")
    {
      checkthroughput_static_dynamic_energy ();
    }
  //输出参数表头
  //blank out the last output file and write the column headers
  std::ofstream out (m_CSVfileName.c_str ());
  out << "Simulation Second,"
      << "Receive Rate[kbps],"
      << "Packets Received in " << time_step << ","
      << "Byte Received Total,"
      << "Packets Received Total" << std::endl;
  out.close ();

  //随机选择速率
  vector<string> rate_list = {"2048bps", "4096bps", "10240bps", "102400bps", "204800bps"};

  for (uint32_t i = 0; i < num_ueNodes; i++)
    {
      Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
      x->SetAttribute ("Min", DoubleValue (0));
      x->SetAttribute ("Max", DoubleValue (rate_list.size()-1));
      int rate_index=x->GetInteger();
      NS_LOG_DEBUG ("current rate_index: " + to_string (rate_index));
      Simulator::Schedule (Seconds (30), &ue_app_datarate_handler, this, i,
                           DataRateValue (DataRate (rate_list[rate_index])));
      Simulator::Schedule (Seconds (80), &ue_app_datarate_handler, this, i,
                           DataRateValue (DataRate (rate_list[rate_list.size()-1])));
    }
  //Simulator::Schedule (Seconds (90), &uav_state_handler, this, 0, "down");

  // Animation Setting
  string animfile;
  if (topo_type == "static_full_energy")
    {
      animfile = "./scratch/sa_jiakang/static_full_energy.xml";
    }
  if (topo_type == "static_dynamic_energy")
    {
      animfile = "./scratch/sa_jiakang/static_dynamic_energy.xml";
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
  NS_LOG_UNCOND ("Start Simulation");
  // Start simulatation
  Simulator::Stop (Seconds (100));
  Simulator::Run ();
  Simulator::Destroy ();
}

void
Scenario::ue_app_datarate_handler (Scenario *scenario, uint32_t i, DataRateValue value)
{
  scenario->ueHelper.setDateRate (i, value);
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
