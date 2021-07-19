#include "scenario.h"

uint32_t num_uavNodes=4;
uint32_t num_ueNodes=10;

double time_step=4;
string topo_type="test";

void CommandSetup (int argc, char **argv)
{

  CommandLine cmd;
  //Scenario Building
  cmd.AddValue ("numUAVs", "number of UAVs", num_uavNodes);
  cmd.AddValue ("numUEs", "number of UEs", num_ueNodes);
  cmd.AddValue ("topo_type", "type of Topo", topo_type);
  cmd.AddValue ("time_step", "timestep in simulator", time_step);

  //cmd.AddValue ("ue_mobility_type", "type of UE Mobility", ueHelper.mobility_type);
  cmd.Parse(argc,argv);

} 


int 
main (int argc, char **argv)
{
  NS_LOG_UNCOND ("Start!");
  CommandSetup (argc, argv);
  //topo_type="static_full_energy";
  Scenario scen(num_uavNodes,num_ueNodes,time_step,topo_type);
  scen.init_Topo ();
  return 0;
}