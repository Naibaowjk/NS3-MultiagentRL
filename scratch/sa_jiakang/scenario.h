#ifndef SCENARIO_H

#define SCENARIO_H

#include "nodehelper.h"

class Scenario
{
private:
  uint32_t port;
  // For Scenario Building
  uint32_t num_uavNodes;
  uint32_t num_ueNodes;
  uint32_t num_crNodes;

  double time_step;
  string topo_type;

  NodeUAVhelper uavHelper;
  NodeUEhelper crHelper;
  NodeUEhelper ueHelper;

  YansWifiPhyHelper wifiPhy;
  string m_CSVfileName;

  void checkthroughput_test ();
  void checkthroughput_static_full_energy ();
  void checkthroughput_static_dynamic_energy ();
  void init_Topo_test (InternetStackHelper &internet_stack);
  void init_Topo_static(InternetStackHelper &internet_stack);
  // For Throughput & Output

public:
  Scenario (/* args */);
  Scenario (uint32_t num_uavNodes_arg, uint32_t num_ueNodes_arg, double time_step_arg,
            string topo_type_arg);
  // Handle Argments in CommandLine

  // Callback function for postion Change
  static void CourseChangeCallback (std::string path, Ptr<const MobilityModel> model);

  //get WifiPhy
  YansWifiPhyHelper get_wifiPhy ();

  // Demo for Static
  void init_Topo ();

  // Event Handler
  static void ue_app_datarate_handler (Scenario *scenario, uint32_t i, DataRateValue value);
  static void ue_app_state_handler (Scenario *scenario, uint32_t i, string state);
  static void uav_state_handler (Scenario *scenario, uint32_t i, string state);
  ~Scenario ();
};

#endif