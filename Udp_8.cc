/*-*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*-*/

// add required header files

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"

// Adding namespace declaration
using namespace ns3;

//Define log component where log msgs will be saved
NS_LOG_COMPONENT_DEFINE("UdpExample");

// Main function
int main(int argc, char *argv[]){
	// read the command line arguments
	CommandLine cmd(__FILE__);
	
	// Process the command line arguments
	cmd.Parse(argc, argv);
	
	// Set time Resolution to 1 nano second
	Time::SetResolution(Time::NS);
	
	// Logging
	LogComponentEnable("UdpEchoClientApplication",LOG_LEVEL_INFO);
	LogComponentEnable("UdpEchoServerApplication",LOG_LEVEL_INFO);
	
	// Create NodeContainer object to store our nodes
	NodeContainer nodes;
	
	// Create 2 nodes 
	nodes.Create(2);
	
	// create object of the point-to-point helper object class to configure net device and the channels
	PointToPointHelper pointToPoint;
	
	// Configure the net Device
	pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps")); // Set Data Rate
	
	// Configure the Channel
	pointToPoint.SetChannelAttribute("Delay", StringValue("2ms")); // Set Delay Attribute
	
	// Install net devices on nodes
	NetDeviceContainer devices;
	devices=pointToPoint.Install(nodes);// install netdevices on node and connect with the Channels
	
	// Configure and Install protocol suits on nodes
	InternetStackHelper stack;
	stack.Install (nodes);

	// configure network IP address and subnet mask for network
	Ipv4AddressHelper address;
	// set data
	address.SetBase("10.0.0.0","255.0.0.0");
	
	// Assign IP addresses to the interfaces of netDevices
	Ipv4InterfaceContainer interfaces = address.Assign(devices);
	
	// Configure our Applications
	// Configure UDPEchoServerApplication
	UdpServerHelper UdpServer(9); // Setting port number of server application
	
	// Application Container create object to store server application and install on node(1)
	ApplicationContainer serverApp = UdpServer.Install(nodes.Get(1)); // indexed 1 server
	
	// Configure start and stop time of server Application
	serverApp.Start(Seconds(1.0)); // server app should start first
	serverApp.Stop(Seconds(10.0)); // server app should stop 
	
	// Configure UdpEchoClientApplication
	UdpClientHelper UdpClient(interfaces.GetAddress(1),9);
	
	// Configure the attribute of client Application
	UdpClient.SetAttribute("MaxPackets", UintegerValue (1));
	UdpClient.SetAttribute("Interval", TimeValue (Seconds(1.0)));
	UdpClient.SetAttribute("PacketSize", UintegerValue (1024));
	
	// Install Client Application on Node 0
	ApplicationContainer clientApp = UdpClient.Install(nodes.Get(0));
	
	// Configure Start and Stop Time
	clientApp.Start(Seconds(2.0));
	clientApp.Stop(Seconds(10.0));
	
	// for Running the code
	AnimationInterface anim("UDPAnimation.xml");
	anim.SetConstantPosition(nodes.Get(0),20.0,30.0);
	anim.SetConstantPosition(nodes.Get(1),40.0,30.0);
	
	// Simulation on Run and start
	Simulator::Run();
	
	// Destory this Resourses
	Simulator::Destroy();
	
	return 0;
}
