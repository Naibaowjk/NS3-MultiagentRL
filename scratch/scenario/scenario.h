#ifndef SCENARIO_H

#define SCENARIO_H

#include "nodehelper.h"


class Scenario
{
private:

  uint32_t port;
  uint32_t bytesTotal;
  uint32_t packetsReceived;


  std::string m_CSVfileName;
  int m_nSinks;
  std::string m_protocolName;
  double m_txp;
  bool m_traceMobility;

  // For Scenario Building
  uint32_t num_uavNodes = 4;
  uint32_t num_ueNodes = 10;
  uint32_t num_crNodes = 1;

  double time_step;
  string topo_type;
  NodeUAVhelper uavHelper;
  NodeUEhelper crHelper;
  NodeUEhelper ueHelper;
  

  YansWifiPhyHelper wifiPhy;

  void CheckThroughput ();
  void init_Topo_test(InternetStackHelper &internet_stack);
    // For Throughput & Output


public:
  Scenario (/* args */);

  // Handle Argments in CommandLine
  std::string CommandSetup (int argc, char **argv);

  // Callback function for postion Change
  static void CourseChangeCallback (std::string path, Ptr<const MobilityModel> model);

  //get WifiPhy
  YansWifiPhyHelper get_wifiPhy ();

  // Demo for Static
  void init_Topo ();

  // Event Handler
  static void ue_app_datarate_handler(Scenario *scenario, uint32_t i, DataRateValue value);  
  static void ue_app_state_handler(Scenario *scenario, uint32_t i, string state);
  static void uav_state_handler(Scenario *scenario, uint32_t i, string state);
  ~Scenario ();
};

#endif