#include "sa_jiakang_gym.h"


namespace ns3 {

NS_LOG_COMPONENT_DEFINE("sa_jiakang_gym");
NS_OBJECT_ENSURE_REGISTERED (SA_GymEnv);

//NS_OBJECT_ENSURE_REGISTERED (SA_GymEnv);

SA_GymEnv::SA_GymEnv ()
{
  NS_LOG_FUNCTION (this);
  m_interval = Seconds(0.1);

  Simulator::Schedule (Seconds(0.0), &SA_GymEnv::ScheduleNextStateRead, this);
}

SA_GymEnv::SA_GymEnv (uint32_t num_uavNodes, Time stepTime)
{
  NS_LOG_FUNCTION (this);
  // m_agentId = agentId;
  m_interval = stepTime;
  this->num_uavNodes = num_uavNodes;

  Simulator::Schedule (Seconds(0.0), &SA_GymEnv::ScheduleNextStateRead, this);
}

void
SA_GymEnv::ScheduleNextStateRead ()
{
  NS_LOG_FUNCTION (this);
  Simulator::Schedule (m_interval, &SA_GymEnv::ScheduleNextStateRead, this);
  Notify();
}

SA_GymEnv::~SA_GymEnv ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
SA_GymEnv::GetTypeId (void)
{
  static TypeId tid = TypeId ("SA_GymEnv")
    .SetParent<OpenGymEnv> ()
    .SetGroupName ("OpenGym")
    .AddConstructor<SA_GymEnv> ()
  ;
  return tid;
}

void
SA_GymEnv::DoDispose ()
{
  NS_LOG_FUNCTION (this);
}

/*
Define observation space
*/
Ptr<OpenGymSpace>
SA_GymEnv::GetObservationSpace()
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
  double max_posi = 300;
  vector<uint32_t> shape_uav_posi = {length_uav_posi,};
  string dtype_uav_posi = TypeNameGet<double>();
  Ptr<OpenGymBoxSpace> uav_posi_box =
      CreateObject<OpenGymBoxSpace> (min_posi, max_posi, shape_uav_posi, dtype_uav_posi);
  
  // UAV battery 的状态
  double min_battery = 0;
  double max_battery = 100;
  vector<uint32_t> shape_uav_battery = {1,}; 
  string dtype_uav_battery = TypeNameGet<double>();
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
SA_GymEnv::GetActionSpace()
{
  // 上下左右,工作,返回充电
  uint32_t action_n = 6;
  Ptr<OpenGymDiscreteSpace> action_space = CreateObject<OpenGymDiscreteSpace> (action_n);

  return action_space;
}

/*
Define game over condition
*/
bool
SA_GymEnv::GetGameOver()
{
  bool isGameOver = false;
  bool test = false;
  static double stepCounter = 0.0;
  stepCounter += 1;
  if (stepCounter == 10 && test) {
      isGameOver = true;
  }
  NS_LOG_UNCOND ("AgentID: " << m_agentId << " MyGetGameOver: " << isGameOver);
  return isGameOver;
}

/*
Collect observations
*/
Ptr<OpenGymDataContainer>
SA_GymEnv::GetObservation()
{
    uint32_t nodeNum = 5;
  uint32_t low = 0.0;
  uint32_t high = 10.0;
  Ptr<UniformRandomVariable> rngInt = CreateObject<UniformRandomVariable> ();

  std::vector<uint32_t> shape = {nodeNum,};
  Ptr<OpenGymBoxContainer<uint32_t> > box = CreateObject<OpenGymBoxContainer<uint32_t> >(shape);

  // generate random data
  for (uint32_t i = 0; i<nodeNum; i++){
    uint32_t value = rngInt->GetInteger(low, high);
    box->AddValue(value);
  }

  Ptr<OpenGymDiscreteContainer> discrete = CreateObject<OpenGymDiscreteContainer>(nodeNum);
  uint32_t value = rngInt->GetInteger(low, high);
  discrete->SetValue(value);

  Ptr<OpenGymDictContainer> data = CreateObject<OpenGymDictContainer> ();
  data->Add("myVector",box);
  data->Add("myValue",discrete);

  // Print data from tuple
  Ptr<OpenGymBoxContainer<uint32_t> > mbox = DynamicCast<OpenGymBoxContainer<uint32_t> >(data->Get("myVector"));
  Ptr<OpenGymDiscreteContainer> mdiscrete = DynamicCast<OpenGymDiscreteContainer>(data->Get("myValue"));
  NS_LOG_UNCOND ("MyGetObservation: " << data);
  NS_LOG_UNCOND ("---" << mbox);
  NS_LOG_UNCOND ("---" << mdiscrete);

  return data;
}

/*
Define reward function
*/
float
SA_GymEnv::GetReward()
{
  static float reward = 0.0;
  reward += 1;
  return reward;
}

/*
Define extra info. Optional
*/
std::string
SA_GymEnv::GetExtraInfo()
{
  std::string myInfo = "testInfo";
  myInfo += "|123";
  NS_LOG_UNCOND("AgentID: " << m_agentId << " MyGetExtraInfo: " << myInfo);
  return myInfo;
}

/*
Execute received actions
*/
bool
SA_GymEnv::ExecuteActions(Ptr<OpenGymDataContainer> action)
{
  Ptr<OpenGymDictContainer> dict = DynamicCast<OpenGymDictContainer>(action);
  Ptr<OpenGymBoxContainer<uint32_t> > box = DynamicCast<OpenGymBoxContainer<uint32_t> >(dict->Get("box"));
  Ptr<OpenGymDiscreteContainer> discrete = DynamicCast<OpenGymDiscreteContainer>(dict->Get("discrete"));

  NS_LOG_UNCOND ("AgentID: " << m_agentId << " MyExecuteActions: " << action);
  NS_LOG_UNCOND ("---" << box);
  NS_LOG_UNCOND ("---" << discrete);
  return true;
}

} // ns3 namespace