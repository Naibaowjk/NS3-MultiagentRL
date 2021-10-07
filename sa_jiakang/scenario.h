#ifndef SCENARIO_H

#define SCENARIO_H

#include "nodehelper.h"

namespace ns3 {

class Scenario: public OpenGymEnv
{
private:
  // For Maddpg
  vector<double> pktlossrate; // recv/send rate, higher is better
  uint32_t uav_id;
  vector<Vector> last_posi;


  uint32_t port;
  // For Scenario Building
  uint32_t construction_size;
  Vector charge_position;


  uint32_t num_uavNodes;
  uint32_t num_ueNodes;
  uint32_t num_crNodes;

  double time_step;
  double simulation_time;
  string topo_type;
  AddressValue remoteAddress;

  NodeUAVhelper uavHelper;
  NodeUEhelper crHelper;
  NodeUEhelper ueHelper;

  YansWifiPhyHelper wifiPhy;
  string m_datapath;

  void auto_connect(AddressValue remoteAddress);
  void checkthroughput();
  void checkip();
  void checksenderinfo();
  void init_Topo_test (InternetStackHelper &internet_stack);
  void init_Topo_static(InternetStackHelper &internet_stack);
  void manual_connect(uint32_t num_ue_ingroup, AddressValue remoteAddress);
  void timestep_handler();
  void print_header();
  void rl_test ();
  void rl_static_full_energy ();
  void rl_static_dynamic_energy ();
  

public:
  Scenario (/* args */);
  Scenario (uint32_t num_uavNodes_arg, uint32_t num_ueNodes_arg, double time_step_arg, double simulation_time_arg,
            string topo_type_arg, uint32_t construction_size_arg, Vector charge_posi_arg, uint32_t uav_id_arg, Ptr<OpenGymInterface> openGymInterface_arg);
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

  // RL env
  Ptr<OpenGymInterface> openGymInterface;

  static TypeId GetTypeId (void);
  virtual void DoDispose ();

  void Set_uav_id(uint32_t uav_id_arg);
  uint32_t Get_uav_id();
  
  Ptr<OpenGymSpace> GetActionSpace();
  Ptr<OpenGymSpace> GetObservationSpace();
  bool GetGameOver();
  Ptr<OpenGymDataContainer> GetObservation();
  float GetReward();
  std::string GetExtraInfo();
  bool ExecuteActions(Ptr<OpenGymDataContainer> action);

  ~Scenario ();
};

}
#endif