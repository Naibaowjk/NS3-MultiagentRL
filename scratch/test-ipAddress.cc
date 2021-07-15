#include "ns3/core-module.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-address-helper.h"

using namespace ns3;

int main()
{
    Ipv4Address ip1("192.168.0.1");
    Ipv4Address ip2("192.168.0.1");
    bool is_equal=true;
    NS_LOG_UNCOND(is_equal);
}