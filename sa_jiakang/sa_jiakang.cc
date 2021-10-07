#include "scenario.h"


using namespace ns3;

uint32_t num_uavNodes=2;
uint32_t num_ueNodes=5;

string topo_type="test";

// Parameters of the scenario
uint32_t simSeed = 1;
double simulationTime = 50; //seconds
double envStepTime = 4; //seconds, ns3gym env step time interval
uint32_t openGymPort = 5555;
uint32_t testArg = 0;

void CommandSetup (int argc, char **argv)
{

  CommandLine cmd;
  //Scenario Building
    // required parameters for OpenGym interface
  cmd.AddValue ("openGymPort", "Port number for OpenGym env. Default: 5555", openGymPort);
  cmd.AddValue ("simSeed", "Seed for random generator. Default: 1", simSeed);
  // optional parameters
  cmd.AddValue ("simTime", "Simulation time in seconds. Default: 10s", simulationTime);
  cmd.AddValue ("stepTime", "Gym Env step time in seconds. Default: 0.1s", envStepTime);
  cmd.AddValue ("testArg", "Extra simulation argument. Default: 0", testArg);


  cmd.AddValue ("numUAVs", "number of UAVs", num_uavNodes);
  cmd.AddValue ("numUEs", "number of UEs", num_ueNodes);
  cmd.AddValue ("topo_type", "type of Topo", topo_type);
  cmd.AddValue ("time_step", "timestep in simulator", envStepTime);

  //cmd.AddValue ("ue_mobility_type", "type of UE Mobility", ueHelper.mobility_type);
  cmd.Parse(argc,argv);

} 


int 
main (int argc, char **argv)
{
  NS_LOG_UNCOND ("Start!");
  CommandSetup (argc, argv);
  //topo_type="static_full_energy";
  Ptr<OpenGymInterface> OpenGymInterface_0 = CreateObject<OpenGymInterface>(openGymPort);
  Scenario scen(num_uavNodes, num_ueNodes, envStepTime, simulationTime ,topo_type, 300, Vector(0,0,0), 0, OpenGymInterface_0);
  // scen.init_Topo ();
  return 0;
}