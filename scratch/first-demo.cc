#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/log.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/mobility-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/mobility-model.h"
#include "ns3/olsr-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/box.h"
#include "ns3/aodv-module.h"
#include "ns3/ssid.h"
#include "ns3/boolean.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("First-Demo");

void
ReceivePacket (Ptr<Socket> socket)
{
  while (socket->Recv ())
    {
      NS_LOG_UNCOND ("Received one packet!");
    }
}

static void
GenerateTraffic (Ptr<Socket> socket, uint32_t pktSize, uint32_t pktCount, Time pktInterval)
{
  if (pktCount > 0)
    {
      socket->Send (Create<Packet> (pktSize));
      Simulator::Schedule (pktInterval, &GenerateTraffic, socket, pktSize, pktCount - 1,
                           pktInterval);
    }
  else
    {
      socket->Close ();
    }
}

int
main (int argc, char *argv[])
{
  std::string phyMode ("DsssRate1Mbps");
  double distance = 50; // m
  uint32_t packetSize = 1000; // bytes
  uint32_t numPackets = 1;
  uint32_t numUEs = 2;
  uint32_t numUAVs = 4;
  double interval = 1.0; // seconds
  bool verbose = false;
  bool tracing = false;
  /// Print routes if true
  bool printRoutes = true;

  CommandLine cmd;
  cmd.AddValue ("phyMode", "Wifi Phy mode", phyMode);
  cmd.AddValue ("printRoutes", "Print routing table dumps.", printRoutes);
  cmd.AddValue ("packetSize", "size of application packet sent", packetSize);
  cmd.AddValue ("numPackets", "number of packets generated", numPackets);
  cmd.AddValue ("interval", "interval (seconds) between packets", interval);
  cmd.AddValue ("verbose", "turn on all WifiNetDevice log components", verbose);
  cmd.AddValue ("tracing", "turn on ascii and pcap tracing", tracing);
  cmd.AddValue ("numUEs", "number of UEs", numUEs);
  cmd.AddValue ("nunUAVs", "number of UAVs", numUAVs);
  cmd.Parse (argc, argv);
  // Convert to time object
  Time interPacketInterval = Seconds (interval);

  // Fix non-unicast data rate to be the same as that of unicast
  Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue (phyMode));

  // NodeContainer for UEs & CR & UAVs
  NodeContainer c_UEs, c_CR, c_UAVs;
  c_UEs.Create (numUEs);
  c_CR.Create (1);
  c_UAVs.Create (numUAVs);

  //Mobility UEs & CR & UAVs
  MobilityHelper mobility;
  mobility.SetMobilityModel (
      "ns3::GaussMarkovMobilityModel", "Bounds", BoxValue (Box (0, 300, 0, 300, 0, 300)),
      "TimeStep", TimeValue (Seconds (0.5)), "Alpha", DoubleValue (0.85), "MeanVelocity",
      StringValue ("ns3::UniformRandomVariable[Min=0|Max=3]"), "MeanDirection",
      StringValue ("ns3::UniformRandomVariable[Min=0|Max=6.283185307]"), "MeanPitch",
      StringValue ("ns3::UniformRandomVariable[Min=0.05|Max=0.05]"), "NormalVelocity",
      StringValue ("ns3::NormalRandomVariable[Mean=0.0|Variance=0.0|Bound=0.0]"), "NormalDirection",
      StringValue ("ns3::NormalRandomVariable[Mean=0.0|Variance=0.2|Bound=0.4]"), "NormalPitch",
      StringValue ("ns3::NormalRandomVariable[Mean=0.0|Variance=0.02|Bound=0.04]"));
  mobility.SetPositionAllocator ("ns3::RandomBoxPositionAllocator", "X",
                                 StringValue ("ns3::UniformRandomVariable[Min=0|Max=300]"), "Y",
                                 StringValue ("ns3::UniformRandomVariable[Min=0|Max=300]"), "Z",
                                 StringValue ("ns3::UniformRandomVariable[Min=0|Max=0]"));
  mobility.Install (c_UEs);

  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (-50, -50, 0.0));
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (c_CR);

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator", "MinX", DoubleValue (100), "MinY",
                                 DoubleValue (100), "DeltaX", DoubleValue (distance), "DeltaY",
                                 DoubleValue (distance), "GridWidth", UintegerValue (2),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (c_UAVs);

  WifiHelper wifi;
  if (verbose)
    {
      wifi.EnableLogComponents (); // Turn on all Wifi logging
    }

  //Physic
  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  // set it to zero; otherwise, gain will be added
  wifiPhy.Set ("RxGain", DoubleValue (-10));
  // ns-3 supports RadioTap and Prism tracing extensions for 802.11b
  wifiPhy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);
  // Pcap
  wifiPhy.EnablePcapAll (std::string ("aodv"));
  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel");
  wifiPhy.SetChannel (wifiChannel.Create ());

  //Mac STA:UEs,CR AP:UAVs Adhoc:UAVs
  // Add an upper mac and disable rate control
  WifiMacHelper wifiMac;
  wifi.SetStandard (WIFI_PHY_STANDARD_80211b);
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue (phyMode),
                                "ControlMode", StringValue (phyMode));
  // Set it to adhoc mode
  wifiMac.SetType ("ns3::AdhocWifiMac");
  NetDeviceContainer devices_UAVs_Adhoc = wifi.Install (wifiPhy, wifiMac, c_UAVs);
  // Set it to sta mode
  Ssid ssid = Ssid ("ns-3-ssid");
  wifiMac.SetType ("ns3::StaWifiMac", "Ssid", SsidValue (ssid), "ActiveProbing",
                   BooleanValue (false));
  NetDeviceContainer devices_UEs = wifi.Install (wifiPhy, wifiMac, c_UEs);
  NetDeviceContainer devices_CR = wifi.Install (wifiPhy, wifiMac, c_CR);
  // Set it to ap mode
  wifiMac.SetType ("ns3::ApWifiMac", "Ssid", SsidValue (ssid));
  NetDeviceContainer devices_UAVs_Ap = wifi.Install (wifiPhy, wifiMac, c_UAVs);

  // Routing Protocol
  AodvHelper aodv;
  OlsrHelper olsr;
  Ipv4StaticRoutingHelper staticRouting;
  // Ipv4GlobalRoutingHelper globalRouting;
  Ipv4ListRoutingHelper list;
  //list.Add (globalRouting,-10);
  list.Add (staticRouting, 0);
  list.Add (aodv, 10);
  if (printRoutes)
  {
    Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("aodv.routes", std::ios::out);
    aodv.PrintRoutingTableAllAt (Seconds (8), routingStream);
  }

  //Internet Stack
  InternetStackHelper stack;
  stack.SetRoutingHelper(aodv);
  stack.Install(c_CR);
  stack.Install(c_UAVs);
  stack.Install(c_UEs);
  

  Ipv4AddressHelper address;
  address.SetBase ("10.0.0.0", "255.255.255.0");
  Ipv4InterfaceContainer i_CR = address.Assign (devices_CR);
  address.SetBase ("10.0.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i_UAVs_Adhoc = address.Assign (devices_UAVs_Adhoc);
  address.SetBase ("10.0.2.0", "255.255.255.0");
  Ipv4InterfaceContainer i_UAVs_Ap = address.Assign (devices_UAVs_Ap);
  // address.SetBase ("10.0.3.0", "255.255.255.0");
  Ipv4InterfaceContainer i_UEs = address.Assign (devices_UEs);


  //Socket Testing
  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> recvSink = Socket::CreateSocket (c_UEs.Get(0), tid);
  InetSocketAddress local = InetSocketAddress (i_UEs.GetAddress(0,0), 80);
  recvSink->Bind (local);
  recvSink->SetRecvCallback (MakeCallback (&ReceivePacket));

  Ptr<Socket> source = Socket::CreateSocket (c_UAVs.Get (1), tid);
  InetSocketAddress remote = InetSocketAddress (i_UAVs_Adhoc.GetAddress(1,0), 80);
  source->Bind(remote);
  source->Connect (local);

  if (tracing == true)
    {
      AsciiTraceHelper ascii;
      wifiPhy.EnableAsciiAll (ascii.CreateFileStream ("first-demo.tr"));
      wifiPhy.EnablePcap ("first-demo", devices_UEs);
      // Trace routing tables
      Ptr<OutputStreamWrapper> routingStream =
          Create<OutputStreamWrapper> ("first-demo.routes", std::ios::out);
      aodv.PrintRoutingTableAllEvery (Seconds (2), routingStream);
      Ptr<OutputStreamWrapper> neighborStream =
          Create<OutputStreamWrapper> ("first-demo.neighbors", std::ios::out);
      aodv.PrintNeighborCacheAllEvery (Seconds (2), neighborStream);

      // To do-- enable an IP-level trace that shows forwarding events only
    }
   
  
  
  // Give OLSR time to converge-- 30 seconds perhaps
  Simulator::Schedule (Seconds (30.0), &GenerateTraffic, source, packetSize, numPackets,
                       interPacketInterval);

  
  // Setting a global route protocol
  // Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  // Output what we are doing
  NS_LOG_UNCOND ("Testing from UAV0 to UAV1 ");

  Simulator::Stop (Seconds (33.0));
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}