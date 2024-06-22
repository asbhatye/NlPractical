/*-*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*-*/

// add required header files

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/point-to-point-layout-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("StarExample");

int main(int argc, char *argv[]){
	//On the Hub Node we install Packet sync Application and packet contains on-off application

//Set up some default values for the animation
Config::SetDefault("ns3::OnOffApplication::PacketSize",UintegerValue(137));
Config::SetDefault("ns3::OnOffApplication::DataRate",StringValue("14kb/s"));

//specify spoke nodes in topology
uint32_t nSpokes=8;

//read and process the commandline arguments
CommandLine cmd(__FILE__);

cmd.Parse(argc, argv);

//configure point-to-point net devices and channels to be installed on spoke nodes
PointToPointHelper pointToPoint;

pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps")); 
pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));
//new helper class
PointToPointStarHelper star(nSpokes,pointToPoint);
NS_LOG_INFO("Star topology created!");//display info msg

//install protocol stack on nodes
InternetStackHelper stack;
star.InstallStack(stack);//installs stack on all the nodes in star topology
NS_LOG_INFO("Install Protocol Stack on all nodes in topology!");

//Assign Ip addresses to interfaces of spoke nodes and hub
star.AssignIpv4Addresses(Ipv4AddressHelper("10.0.0.0","255.0.0.0"));
NS_LOG_INFO("IPV4 Addresses are assign to spoke nodes and hub interfaces");
NS_LOG_INFO("IP Addresses assigned to intefaces of hub are");

for(uint32_t h_interfaces;h_interfaces<star.SpokeCount();++h_interfaces)
{
NS_LOG_INFO("Address of hub interfaces:"<<star.GetHubIpv4Address(h_interfaces));
}
NS_LOG_INFO("Ip address of spoke nodes");
for(uint32_t j=0;j<star.SpokeCount();++j)
{
NS_LOG_INFO("Address of SpokeNode :"<<j);
NS_LOG_INFO("Address :"<<star.GetSpokeIpv4Address(j));
}
//configuring packet sink application on HUB
uint16_t port=50000;//specifying port no of hub
//configuring socket address of hub
Address hubLocalAddress(InetSocketAddress(Ipv4Address::GetAny(),port));

PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory",hubLocalAddress);

//install packet sink app on HUb
ApplicationContainer hubApp =packetSinkHelper.Install(star.GetHub());
//configure start and stop time of sink application

hubApp.Start(Seconds(1.0));
hubApp.Stop(Seconds(10.0));

//configure on-off app on SpokeNodes
OnOffHelper onOffHelper("ns3::TcpSocketFactory",Address());
onOffHelper.SetAttribute("OnTime",StringValue("ns3::ConstantRandomVariable[Constant=1]"));
onOffHelper.SetAttribute("OffTime",StringValue("ns3::ConstantRandomVariable[Constant=0]"));

//install on off app of all spoke nodes
ApplicationContainer spokeApps; 
for(uint32_t i=0;i<star.SpokeCount();++i){
	AddressValue remoteAddress(InetSocketAddress(star.GetHubIpv4Address(i),port));
	onOffHelper.SetAttribute("Remote",remoteAddress);
	spokeApps.Add(onOffHelper.Install(star.GetSpokeNode(i)));
}
//configure start and stop time of ON OFF Application
spokeApps.Start(Seconds(1.0));
spokeApps.Stop(Seconds(10.0));
//enable routing so that packet will be routed across star topology
Ipv4GlobalRoutingHelper::PopulateRoutingTables();
//enables packet capture on all nodes
pointToPoint.EnablePcapAll("star");
//Animate the Star topology
AnimationInterface anim ("AB_star.xml");
star.BoundingBox(1,1,100,100);
//$ export NS_LOG=StarExample=info
	Simulator::Run();
	
	// Destory this Resourses
	Simulator::Destroy();
	NS_LOG_INFO("Done");
	return 0;
}

