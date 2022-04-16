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

NS_LOG_COMPONENT_DEFINE("Assignment-8");

int main(int argc, char *argv[])
{
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    NodeContainer cnc_ccc_p2p;
    cnc_ccc_p2p.Create(2);

    NodeContainer ccc_lan_p2p;
    ccc_lan_p2p.Add(cnc_ccc_p2p.Get(1));
    ccc_lan_p2p.Create(1);

    NodeContainer ccc_dss_p2p;
    ccc_dss_p2p.Add(ccc_lan_p2p.Get(0));
    ccc_dss_p2p.Create(1);

    NodeContainer cnc;           // csmaNodes
    cnc.Add(cnc_ccc_p2p.Get(0)); // router
    cnc.Create(2);               // 2 servers

    NodeContainer lan;
    lan.Add(ccc_lan_p2p.Get(1));
    lan.Create(4);

    NodeContainer dss;
    dss.Create(3);

    NodeContainer wifi_ap = ccc_dss_p2p.Get(1);

    PointToPointHelper cnc_ccc_p2p_ch;
    cnc_ccc_p2p_ch.SetDeviceAttribute("DataRate", StringValue("2Mbps"));
    cnc_ccc_p2p_ch.SetChannelAttribute("Delay", StringValue("20ms"));

    NetDeviceContainer cnc_ccc_p2p_ndc;
    cnc_ccc_p2p_ndc = cnc_ccc_p2p_ch.Install(cnc_ccc_p2p);

    PointToPointHelper ccc_lan_p2p_ch;
    ccc_lan_p2p_ch.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    ccc_lan_p2p_ch.SetChannelAttribute("Delay", StringValue("20ms"));

    NetDeviceContainer ccc_lan_p2p_ndc;
    ccc_lan_p2p_ndc = ccc_lan_p2p_ch.Install(ccc_lan_p2p);

    PointToPointHelper ccc_dss_p2p_ch;
    ccc_dss_p2p_ch.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    ccc_dss_p2p_ch.SetChannelAttribute("Delay", StringValue("20ms"));

    NetDeviceContainer ccc_dss_p2p_ndc;
    ccc_dss_p2p_ndc = ccc_dss_p2p_ch.Install(ccc_dss_p2p);

    CsmaHelper cnc_ch;
    cnc_ch.SetChannelAttribute("DataRate", StringValue("5Mbps"));
    cnc_ch.SetChannelAttribute("Delay", StringValue("20ms"));

    NetDeviceContainer cnc_ndc;
    cnc_ndc = cnc_ch.Install(cnc);

    CsmaHelper lan_ch;
    lan_ch.SetChannelAttribute("DataRate", StringValue("5Mbps"));
    lan_ch.SetChannelAttribute("Delay", StringValue("20ms"));

    NetDeviceContainer lan_ndc;
    lan_ndc = lan_ch.Install(lan);

    Ptr<RateErrorModel> em = CreateObject<RateErrorModel>();
    em->SetAttribute("ErrorRate", DoubleValue(0.0015));
    lan_ndc.Get(0)->SetAttribute("ReceiveErrorModel", PointerValue(em));
    lan_ndc.Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(em));
    lan_ndc.Get(2)->SetAttribute("ReceiveErrorModel", PointerValue(em));
    lan_ndc.Get(3)->SetAttribute("ReceiveErrorModel", PointerValue(em));
    lan_ndc.Get(4)->SetAttribute("ReceiveErrorModel", PointerValue(em));

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());

    WifiHelper wifi;
    wifi.SetRemoteStationManager("ns3::AarfWifiManager");

    WifiMacHelper mac;
    Ssid ssid = Ssid("ns-3-ssid");
    mac.SetType("ns3::StaWifiMac",
                "Ssid", SsidValue(ssid),
                "ActiveProbing", BooleanValue(false));

    NetDeviceContainer wifi_nic;
    wifi_nic = wifi.Install(phy, mac, dss);

    mac.SetType("ns3::ApWifiMac",
                "Ssid", SsidValue(ssid));

    NetDeviceContainer wifi_ap_nic;
    wifi_ap_nic = wifi.Install(phy, mac, wifi_ap);

    MobilityHelper mobility;

    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue(0.0),
                                  "MinY", DoubleValue(0.0),
                                  "DeltaX", DoubleValue(5.0),
                                  "DeltaY", DoubleValue(10.0),
                                  "GridWidth", UintegerValue(3),
                                  "LayoutType", StringValue("RowFirst"));

    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                              "Bounds", RectangleValue(Rectangle(-50, 50, -50, 50)));
    mobility.Install(dss);

    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifi_ap);

    InternetStackHelper stack;
    stack.Install(cnc);
    stack.Install(lan);
    stack.Install(dss);
    stack.Install(ccc_dss_p2p);

    Ipv4AddressHelper address;

    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer cnc_if = address.Assign(cnc_ndc);

    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer cnc_ccc_p2p_if = address.Assign(cnc_ccc_p2p_ndc);

    address.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer ccc_lan_p2p_if = address.Assign(ccc_lan_p2p_ndc);
    Ipv4InterfaceContainer ccc_dss_p2p_if = address.Assign(ccc_dss_p2p_ndc);

    address.SetBase("10.1.4.0", "255.255.255.0");
    Ipv4InterfaceContainer lan_if = address.Assign(lan_ndc);

    address.SetBase("10.1.5.0", "255.255.255.0");
    Ipv4InterfaceContainer wifi_ap_if = address.Assign(wifi_ap_nic);
    Ipv4InterfaceContainer wifi_nic_if = address.Assign(wifi_nic);

    UdpEchoServerHelper echoServer1(9);
    UdpEchoServerHelper echoServer2(10);

    ApplicationContainer serverApps = echoServer1.Install(cnc.Get(1));
    serverApps.Add(echoServer2.Install(cnc.Get(2)));

    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(100.0));

    UdpEchoClientHelper echoClient1(cnc_if.GetAddress(1), 9);
    echoClient1.SetAttribute("MaxPackets", UintegerValue(100));
    echoClient1.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient1.SetAttribute("PacketSize", UintegerValue(1024));

    UdpEchoClientHelper echoClient2(cnc_if.GetAddress(2), 10);
    echoClient1.SetAttribute("MaxPackets", UintegerValue(100));
    echoClient1.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient1.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient1.Install(lan.Get(1));
    clientApps.Add(echoClient2.Install(dss.Get(0)));

    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(100.0));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    AnimationInterface anim("q1real.xml");
    anim.SetConstantPosition(cnc_ccc_p2p.Get(0), 20.0, 15.0);
    anim.SetConstantPosition(cnc_ccc_p2p.Get(1), 30.0, 15.0);
    anim.SetConstantPosition(ccc_lan_p2p.Get(1), 40.0, 30.0);
    anim.SetConstantPosition(ccc_dss_p2p.Get(1), 40.0, 10.0);
    anim.SetConstantPosition(cnc.Get(1), 10.0, 10.0);
    anim.SetConstantPosition(cnc.Get(2), 10.0, 20.0);
    anim.SetConstantPosition(lan.Get(1), 50.0, 35.0);
    anim.SetConstantPosition(lan.Get(2), 60.0, 35.0);
    anim.SetConstantPosition(lan.Get(3), 50.0, 25.0);
    anim.SetConstantPosition(lan.Get(4), 60.0, 25.0);
    anim.SetConstantPosition(dss.Get(0), 50.0, 10.0);
    anim.SetConstantPosition(dss.Get(1), 60.0, 10.0);
    anim.SetConstantPosition(dss.Get(2), 70.0, 10.0);

    AsciiTraceHelper ascii;
    cnc_ccc_p2p_ch.EnableAsciiAll(ascii.CreateFileStream("cnc_ccc_p2preal.tr"));
    ccc_lan_p2p_ch.EnableAsciiAll(ascii.CreateFileStream("ccc_lan_p2preal.tr"));
    ccc_dss_p2p_ch.EnableAsciiAll(ascii.CreateFileStream("ccc_dss_p2preal.tr"));
    lan_ch.EnableAsciiAll(ascii.CreateFileStream("lanreal.tr"));
    cnc_ch.EnableAsciiAll(ascii.CreateFileStream("cncreal.tr"));
    phy.EnableAsciiAll(ascii.CreateFileStream("phyreal.tr"));

    Simulator::Stop(Seconds(100));

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}