/*-*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*-*/

// adding required header files

#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"

using namespace ns3;

//Define log component where log msgs will be saved
NS_LOG_COMPONENT_DEFINE("DhcpExample");

// Main function
int main(int argc, char *argv[]){
	// read the command line arguments
	CommandLine cmd(__FILE__);
	// Process the command line arguments
	cmd.Parse(argc, argv);

	// Set time Resolution to 1 nano second
	Time::SetResolution(Time::NS);
	
	// Logging enable for applications
	LogComponentEnable("DhcpServer",LOG_LEVEL_ALL);
	LogComponentEnable("DhcpClient",LOG_LEVEL_ALL);
	LogComponentEnable("UdpEchoServerApplication",LOG_LEVEL_INFO);
	LogComponentEnable("UdpEchoClientApplication",LOG_LEVEL_INFO);
	
	
	//create nodes
	NS_LOG_INFO("Create Nodes");
	// Create NodeContainer object to store our nodes
	NodeContainer nodes;//represent dhcp client nodes
	NodeContainer router;//represent dhcp sever nodes
	nodes.Create(3);//create 3 client nodes
	router.Create(2);//create 2 router nodes
	//combine nodes objects And router obj into a single object.
	NodeContainer net(nodes,router);
	
	//configure net devices and channel
	CsmaHelper csma;
	
	csma.SetChannelAttribute("DataRate",StringValue("5Mbps"));
	csma.SetChannelAttribute("Delay",StringValue("2ms"));
	csma.SetDeviceAttribute("Mtu",UintegerValue(1500));
	//install netdevices on all nodes and routers and connect them with network
	NetDeviceContainer devices=csma.Install(net);
	//creating p2p topology
	NodeContainer p2pNodes;
	
	//Add router R1 to p2p topology
	p2pNodes.Add(net.Get(4));
	
	//create new node A
	p2pNodes.Create(1);
	
	//configure p2p net devices and p2p channel
	PointToPointHelper pointToPoint;
	pointToPoint.SetDeviceAttribute("DataRate",StringValue("5Mbps"));
	pointToPoint.SetChannelAttribute("Delay",StringValue("2ms"));
	
	//install p2p net devices on router R1 and node A and connect with p2p channel
	NetDeviceContainer p2pDevices;
	p2pDevices=pointToPoint.Install(p2pNodes);
	
	//Install protocol stack on all nodes
	InternetStackHelper stack;
	stack.Install(net);
	stack.Install(p2pNodes.Get(1));//installed on Node A
	
	//Assign IP address to p2p interfaces
	Ipv4AddressHelper address;
	address.SetBase("20.0.0.0","255.0.0.0");
	Ipv4InterfaceContainer p2pInterfaces;
	p2pInterfaces=address.Assign(p2pDevices);
	
	//Configuring the DHCP sever application
	//configuring dhcp server
	DhcpHelper dhcpHelper;
	
	//Setting fixed IP Address for default gateway R1
	Ipv4InterfaceContainer fixedNodes=dhcpHelper.InstallFixedAddress(devices.Get(4),Ipv4Address("10.0.0.17"),Ipv4Mask("/8"));

	//enabling ip forwarding
	fixedNodes.Get(0).first->SetAttribute("IpForward",BooleanValue(true));	
	
	//enbale routing between 2 networks
	Ipv4GlobalRoutingHelper::PopulateRoutingTables();
	
	//configure and install dhcp server on R0
	ApplicationContainer dhcpServerApp=dhcpHelper.InstallDhcpServer(devices.Get(3),Ipv4Address("10.0.0.12"),Ipv4Address("10.0.0.0"),Ipv4Mask("/8"),Ipv4Address("10.0.0.10"),Ipv4Address("10.0.0.15"),Ipv4Address("10.0.0.17")); 
	
	//configure start and stop time of server
	dhcpServerApp.Start(Seconds(0.0));
	dhcpServerApp.Stop(Seconds(20.0));
	
	//configuring dhcp client
	NetDeviceContainer dhcpClientNetDev;
	
	//combining net devices on dscp client nodes in single object
	dhcpClientNetDev.Add(devices.Get(0));
	dhcpClientNetDev.Add(devices.Get(1));
	dhcpClientNetDev.Add(devices.Get(2));
	
	//install dhcp client application on node N0,N1,N2
	ApplicationContainer dhcpClients=dhcpHelper.InstallDhcpClient(dhcpClientNetDev);
	
	//configure start and stop time of dhcpClients
	dhcpClients.Start(Seconds(1.0));
	dhcpClients.Stop(Seconds(20.0));
	
	//COnfigure udpEchoServerApplication
	UdpEchoServerHelper echoServer(9); // Setting port number of server application
	
	// Application Container create object to store server application and install on node(1)
	ApplicationContainer serverApp = echoServer.Install(p2pNodes.Get(1)); // indexed 1 server
	
	// Configure start and stop time of server Application
	serverApp.Start(Seconds(0.0)); // server app should start first
	serverApp.Stop(Seconds(20.0)); // server app should stop 
	
	// Configure UdpEchoClientApplication for node n1
	UdpEchoClientHelper echoClient(p2pInterfaces.GetAddress(1),9);
	
	// Configure the attribute of client Application
	echoClient.SetAttribute("MaxPackets", UintegerValue (1));
	echoClient.SetAttribute("Interval", TimeValue (Seconds(1.0)));
	echoClient.SetAttribute("PacketSize", UintegerValue (1024));
	
	// Install Client Application on Node 0
	ApplicationContainer clientApp = echoClient.Install(net.Get(1));
	
	// Configure Start and Stop Time
	clientApp.Start(Seconds(10.0));
	clientApp.Stop(Seconds(20.0));
	
	//configuring start and stop time of UdpEchoCLient
	Simulator::Stop(Seconds(30.0));
	Simulator::Run();
	Simulator::Destroy();
	
	return 0;
		
	
}


	
	
	
	
