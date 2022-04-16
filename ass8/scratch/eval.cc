#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"
#include "ns3/error-model.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Assignment-8-eval");

int main(int argc, char *argv[])
{
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    NodeContainer lab;
    lab.Create(8);

    CsmaHelper lab_ch;
    lab_ch.SetChannelAttribute("DataRate", StringValue("5Mbps"));
    lab_ch.SetChannelAttribute("Delay", StringValue("20ms"));

    NetDeviceContainer lab_ndc;
    lab_ndc = lab_ch.Install(lab);

    InternetStackHelper stack;
    stack.Install(lab);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer lab_if = address.Assign(lab_ndc);

    UdpEchoServerHelper echoServer(10);
    ApplicationContainer serverApps = echoServer.Install(lab.Get(2));

    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(100.0));

    UdpEchoClientHelper echoClient(lab_if.GetAddress(2), 10);
    echoClient.SetAttribute("MaxPackets", UintegerValue(100));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(lab.Get(4));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(100.0));

    AnimationInterface anim("eval.xml");
    anim.SetConstantPosition(lab.Get(0), 0.0, 0.0);
    anim.SetConstantPosition(lab.Get(1), 10.0, 0.0);
    anim.SetConstantPosition(lab.Get(2), 20.0, 0.0);
    anim.SetConstantPosition(lab.Get(3), 30.0, 0.0);
    anim.SetConstantPosition(lab.Get(4), 0.0, 10.0);
    anim.SetConstantPosition(lab.Get(5), 10.0, 10.0);
    anim.SetConstantPosition(lab.Get(6), 20.0, 10.0);
    anim.SetConstantPosition(lab.Get(7), 30.0, 10.0);

    AsciiTraceHelper ascii;
    lab_ch.EnableAsciiAll(ascii.CreateFileStream("lab.tr"));

    Simulator::Stop(Seconds(100));

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
