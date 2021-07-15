#include "scenario.h"
Scenario::Scenario (/* args */)
    : port (9),
      bytesTotal (0),
      packetsReceived (0),
      m_CSVfileName ("./scratch/scenario/route.csv"),
      m_traceMobility (false),
      time_step (4),
      topo_type ("test"),
      uavHelper (NodeUAVhelper (num_uavNodes)),
      crHelper (NodeUEhelper (num_crNodes, time_step)),
      ueHelper (NodeUEhelper (num_ueNodes, time_step))

{
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
Scenario::CheckThroughput ()
{
  double kbs = (crHelper.bytesTotal_timestep[0] * 8.0) / 1024 / time_step;
  stringstream log_time;
  log_time<<"Simulation Time: "<<Simulator::Now ().GetSeconds ()
          <<" ------------------------------------------------------";
  NS_LOG_UNCOND (log_time.str());
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

  out << (Simulator::Now ()).GetSeconds () << "," 
      << kbs << "," 
      << crHelper.packetsReceived_timestep[0] << ","
      << crHelper.bytesTotal[0] << "," 
      << crHelper.packetsReceived[0] << "" 
      << std::endl;

  out.close ();
  crHelper.bytesTotal_timestep[0] = 0;
  crHelper.packetsReceived_timestep[0] = 0;
  Simulator::Schedule (Seconds (time_step), &Scenario::CheckThroughput, this);
}

std::string
Scenario::CommandSetup (int argc, char **argv)
{
  CommandLine cmd;
  // Throughout
  cmd.AddValue ("CSVfileName", "The name of the CSV output file name", m_CSVfileName);
  cmd.AddValue ("traceMobility", "Enable mobility tracing", m_traceMobility);

  //Scenario Building
  cmd.AddValue ("numUAVs", "number of UAVs", num_uavNodes);
  cmd.AddValue ("numUEs", "number of UEs", num_ueNodes);
  cmd.AddValue ("topo_type", "type of Topo", topo_type);
  //cmd.AddValue ("ue_mobility_type", "type of UE Mobility", ueHelper.mobility_type);

  cmd.Parse (argc, argv);
  return m_CSVfileName;
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
}

void
Scenario::init_Topo_test (InternetStackHelper &internet_stack)
{
  /*
  
  */
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
  CheckThroughput ();

  //测试更改Application
  Simulator::Schedule(Seconds(30),&ue_app_datarate_handler,this,2,DataRateValue(DataRate("4096bps")));
  Simulator::Schedule(Seconds(50),&ue_app_datarate_handler,this,5,DataRateValue(DataRate("204800bps")));
  for (uint32_t i = 0; i < num_ueNodes; i++)
  {
    Simulator::Schedule(Seconds(70),&ue_app_state_handler,this,i,"off");
  }
  Simulator::Schedule(Seconds(90),&uav_state_handler,this,0,"down");
  

  //输出参数表头
  //blank out the last output file and write the column headers
  std::ofstream out (m_CSVfileName.c_str ());
  out << "Simulation Second,"
      << "Receive Rate[kbps],"
      << "Packets Received in "<<time_step<<","
      << "Byte Received Total,"
      << "Packets Received Total"
      << std::endl;
  out.close ();

  // Animation Setting
  AnimationInterface anim ("./scratch/scenario/static-case.xml");
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
  anim.UpdateNodeSize(num_uavNodes,10,10);
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


void Scenario::ue_app_datarate_handler(Scenario *scenario, uint32_t i, DataRateValue value)
{
  scenario->ueHelper.setDateRate(i,value);
}

void Scenario::ue_app_state_handler(Scenario *scenario, uint32_t i, string state)
{
  scenario->ueHelper.setOnOffState(i,state);
}

void Scenario::uav_state_handler(Scenario *scenario, uint32_t i, string state)
{
  if (state=="down") scenario->uavHelper.set_down(i);
  else if (state=="up") scenario->uavHelper.set_up(i);
}

int
main (int argc, char **argv)
{
  NS_LOG_UNCOND ("Start!");
  Scenario scen;
  scen.CommandSetup (argc, argv);
  scen.init_Topo ();

  return 0;
}