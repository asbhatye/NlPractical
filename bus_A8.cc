/*-*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*-*/

//Adding header file
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/csma-module.h"
#include "ns3/ipv4-global-routing-helper.h"

// Adding namespace declaration
using namespace ns3;

//Define log component where log msgs will be saved
NS_LOG_COMPONENT_DEFINE("busExample");

// Main function
int main(int argc, char *argv[])
{
	uint32_t nCsma=3;//declare no. of nodes in bus topology
	
	// read the command line arguments
	CommandLine cmd(__FILE__);
	
	// Process the command line arguments
	cmd.Parse(argc, argv);
	
	// Set time Resolution to 1 nano second
	Time::SetResolution(Time::NS);
	
	//Enable Logging Client And Server application
	LogComponentEnable("UdpEchoClientApplication",LOG_LEVEL_INFO);
	LogComponentEnable("UdpEchoServerApplication",LOG_LEVEL_INFO);
	
	// Create NodeContainer object to store our nodes
	NodeContainer p2pNodes;
	p2pNodes.Create(2);//Create 2 p2pNodes
	
	// create object of the point-to-point helper object class to configure net device and the channels
	PointToPointHelper pointToPoint;
	
	// Configure the net Device
	pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps")); // Set Data Rate
	
	// Configure the Channel
	pointToPoint.SetChannelAttribute("Delay", StringValue("2ms")); // Set Delay Attribute
	
	// Install p2p net devices on p2pnodes and connect with channel
	NetDeviceContainer p2pDevices;
	p2pDevices=pointToPoint.Install(p2pNodes);// install netdevices on node and connect with the Channels	
	//Bus Topology
	//Create nodes in bus topology
	NodeContainer csmaNodes;
	csmaNodes.Add(p2pNodes.Get(1));//adding node n1 to bus topology
	csmaNodes.Create(nCsma);//creating extra nCsma=3 nodes
	
	//Configure csma net devices and channel
	CsmaHelper csma;
	csma.SetChannelAttribute("DataRate",StringValue("100Mbps"));
	csma.SetChannelAttribute("Delay",TimeValue(NanoSeconds(6560)));
	
	//install csma net devices on csma nodes and connect them with Channel
	NetDeviceContainer csmaDevices; 	
	csmaDevices=csma.Install(csmaNodes);
	
	// Configure and Install protocol suits on nodes
	InternetStackHelper stack;
	
	//install stack on node n0
	stack.Install(p2pNodes.Get(0));
	
	//install stack on all csma node 
	stack.Install(csmaNodes); 
	
	//Assign IP Addresses
	// configure network IP address and subnet mask for network
	Ipv4AddressHelper address;
	// Configure netword IP Address for p2p topology
	address.SetBase("10.0.0.0","255.0.0.0");
	
	// Assign IP addresses to the interfaces of p2pnetDevices
	Ipv4InterfaceContainer p2pInterfaces = address.Assign(p2pDevices);		
	
	//Configure network IP address for bus Topology
	address.SetBase("20.0.0.0","255.0.0.0");
	
	Ipv4InterfaceContainer csmaInterfaces = address.Assign(csmaDevices);
	
	// Configure UDPEchoServerApplication
	UdpEchoServerHelper echoServer(9); // Setting port number of server application
	
	//install server application in last node in bus topology
	ApplicationContainer serverApp = echoServer.Install(csmaNodes.Get(3));
	
	// Configure start and stop time of server Application
	serverApp.Start(Seconds(1.0)); // server app should start first
	serverApp.Stop(Seconds(10.0)); // server app should stop 
	
	// Configure UdpEchoClientApplication
	UdpEchoClientHelper echoClient(csmaInterfaces.GetAddress(3),9);
	
	// Configure the attribute of client Application
	echoClient.SetAttribute("MaxPackets", UintegerValue (3));
	echoClient.SetAttribute("Interval", TimeValue (Seconds(1.0)));
	echoClient.SetAttribute("PacketSize", UintegerValue (1024));
	
	// Install Client Application on Node 0
	ApplicationContainer clientApp = echoClient.Install(p2pNodes.Get(0));
	
	// Configure Start and Stop Time
	clientApp.Start(Seconds(2.0));
	clientApp.Stop(Seconds(10.0));
	
	//Enable routing between 10.0.0.0 and 20.0.0.0
	Ipv4GlobalRoutingHelper::PopulateRoutingTables();
	
	//Capture packets
	pointToPoint.EnablePcapAll("p2p_packet");
	csma.EnablePcap("bus_packet",csmaDevices.Get(1),true);
	
	//create object of Animation interface
	AnimationInterface anim("busTopologyAnime.xml");
	anim.SetConstantPosition(p2pNodes.Get(0),20.0,30.0);
	anim.SetConstantPosition(p2pNodes.Get(1),40.0,30.0);
	anim.SetConstantPosition(csmaNodes.Get(1),60.0,30.0);
	anim.SetConstantPosition(csmaNodes.Get(2),80.0,30.0);
	anim.SetConstantPosition(csmaNodes.Get(3),100.0,30.0);
	// Simulation on Run and start
	Simulator::Run();
	
	// Destory this Resourses
	Simulator::Destroy();
	
	return 0;
}
	

