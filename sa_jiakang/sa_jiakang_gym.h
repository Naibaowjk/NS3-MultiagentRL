#ifndef SA_JIAKANG_GYM_H

#define SA_JIAKANG_GYM_H

#include "ns3/opengym-module.h"
#include "ns3/nstime.h"
#include "scenario.h"

namespace ns3 {

class SA_GymEnv : public OpenGymEnv
{
public:
  SA_GymEnv ();
  SA_GymEnv (uint32_t agentId, Time stepTime);
  virtual ~SA_GymEnv ();
  static TypeId GetTypeId (void);
  virtual void DoDispose ();

  Ptr<OpenGymSpace> GetActionSpace();
  Ptr<OpenGymSpace> GetObservationSpace();
  bool GetGameOver();
  Ptr<OpenGymDataContainer> GetObservation();
  float GetReward();
  std::string GetExtraInfo();
  bool ExecuteActions(Ptr<OpenGymDataContainer> action);

private:
  void ScheduleNextStateRead();

  uint32_t m_agentId;
  Time m_interval;
  uint32_t num_uavNodes;
};

}

#endif