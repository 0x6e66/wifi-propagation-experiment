#include <iostream>
#include <fstream>
#include <bits/stdc++.h>

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/wifi-phy-band.h"

#include "./helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("WifiPropagationExperiment");

// configure basic parameters
const double TX_POWER = 10;
const double ANTENNA_GAIN = 1;
const uint32_t PACKET_SIZE = 1450;
const double DATA_RATE = 75e6;
const Time PACKET_INTERVAL = Seconds((PACKET_SIZE * 8) / DATA_RATE);

void analyze_throughput_runtime(double distance, std::string model_name, double time_start, double time_end, double time_step);
void analyze_throughput_distance(double distance_max, double distance_step,double simulation_time, std::string model_name);
void setup(double distance, double simulation_time, std::string model_name);

int main(int argc, char* argv[]) {
    std::vector<std::string> models = {"friis", "fixed_rss", "three_log", "two_ray", "nakagami"};

    // determine runtime needed for simulation to settle down
    // analyze_throughput_runtime(20, "two_ray", 0, 20, 0.1);
    std::string model;

    CommandLine cmd;
    cmd.AddValue("model", "select propagation model [friis, fixed_rss, three_log, two_ray, nakagami]", model);
    cmd.Parse (argc, argv);

    std::cout << model << std::endl;

    // based on previous analysis: set simulation time to 5 seconds
    analyze_throughput_distance(300, 1, 5, model);

    return 0;
}

// simulate the scenario for a given model
void analyze_throughput_distance(double distance_max, double distance_step, double simulation_time, std::string model_name){
    std::ofstream out(model_name + ".csv");
    out << "distance,";
    out << "flow_throughput,"; 
    out << "rssi_average" << std::endl; 

    for (double distance = 0; distance <= distance_max; distance+= distance_step) {
        setup(distance, simulation_time, model_name);

        FlowMonitorHelper flowmon;
        Ptr<FlowMonitor> monitor = flowmon.InstallAll();

        Simulator::Run();

        monitor->CheckForLostPackets();
        Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());
        std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();

        for (const auto& flow : stats) {
            double flow_throughput = flow.second.rxPackets * PACKET_SIZE * 8.0 / (flow.second.timeLastRxPacket.GetSeconds() - flow.second.timeFirstTxPacket.GetSeconds()) / 1e6;

            if(flow_throughput < 1e-3) {
                std::cout << "Troughput is zero. Terminating" << std::endl;
                return;
            }

            double rssi_average = calc_mean(rssi);
            
            out << distance << ",";
            out << flow_throughput << ","; 
            out << rssi_average << std::endl; 
        }
        Simulator::Destroy();
    }
    
    out.close();
}

// simulate the scenario for a given distance
void analyze_throughput_runtime(double distance, std::string model_name, double time_start, double time_end, double time_step) {
    std::ofstream out("determine_simulation_time.csv");
    out << "simulation_time,";
    out << "flow_throughput" << std::endl; 

    for (double runtime = time_start; runtime <= time_end; runtime += time_step)
    {
        setup(runtime, runtime, model_name);

        FlowMonitorHelper flowmon;
        Ptr<FlowMonitor> monitor = flowmon.InstallAll();

        Simulator::Run();

        monitor->CheckForLostPackets();
        std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();

        for (const auto &flow : stats)
        {
            double flow_throughput = flow.second.rxPackets * PACKET_SIZE * 8.0 / (flow.second.timeLastRxPacket.GetSeconds() - flow.second.timeFirstTxPacket.GetSeconds()) / 1e6;
            out << runtime << "," << flow_throughput << std::endl;
        }

        Simulator::Destroy();
    }
}

// setup the scenario with all the layers and the configured basic parameters
void setup(double distance, double simulation_time, std::string model_name) {
    rssi.clear();
    snr.clear();
    noise.clear();

    NodeContainer nodes;
    nodes.Create(2);

    // select propagation model
    YansWifiChannelHelper wifi_channel;
    if(model_name == "friis") {
        wifi_channel.AddPropagationLoss(
            "ns3::FriisPropagationLossModel",
            "Frequency", DoubleValue(5e9)
        );
    }
    else if(model_name == "fixed_rss") {
        wifi_channel.AddPropagationLoss(
            "ns3::FixedRssLossModel",
            "Rss", DoubleValue(-75.0)
        );
    }
    else if(model_name == "three_log") {
        wifi_channel.AddPropagationLoss(
            "ns3::ThreeLogDistancePropagationLossModel",
            "Distance0", DoubleValue(1.0),
            "Distance1", DoubleValue(10.0),
            "Distance2", DoubleValue(100.0),
            "Exponent0", DoubleValue(2.0),
            "Exponent1", DoubleValue(2.7),
            "Exponent2", DoubleValue(3.5)
        );
    }
    else if(model_name == "two_ray") {
        wifi_channel.AddPropagationLoss(
            "ns3::TwoRayGroundPropagationLossModel",
            "Frequency", DoubleValue(5e9),
            "HeightAboveZ", DoubleValue(1)
        );
    }
    else if(model_name == "nakagami") {
        wifi_channel.AddPropagationLoss("ns3::NakagamiPropagationLossModel");
    }
    else {
        NS_FATAL_ERROR("incorrect model");
    }
    wifi_channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");

    // apply basic parameters
    YansWifiPhyHelper phy;
    phy.SetChannel(wifi_channel.Create());
    phy.Set("TxPowerStart", DoubleValue(TX_POWER));
    phy.Set("TxPowerEnd", DoubleValue(TX_POWER));
    phy.Set("RxGain", DoubleValue(ANTENNA_GAIN));
    phy.Set("TxGain", DoubleValue(ANTENNA_GAIN));
    phy.Set("ChannelSettings", StringValue("{0, 40, BAND_5GHZ, 0}"));

    WifiHelper wifi;
    wifi.SetStandard(WifiStandard::WIFI_STANDARD_80211n);

    WifiMacHelper mac;
    mac.SetType("ns3::AdhocWifiMac");

    NetDeviceContainer devices = wifi.Install(phy, mac, nodes);

    // set callback function for collecting data on physical layer
    Config::Connect("/NodeList/1/DeviceList/*/Phy/MonitorSnifferRx", MakeCallback(&callback_recv_packets));

    // set distance between the two nodes
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> pos_alloc = CreateObject<ListPositionAllocator>();
    pos_alloc->Add(Vector(0,0,0));
    pos_alloc->Add(Vector(distance,0,0));
    mobility.SetPositionAllocator(pos_alloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

    InternetStackHelper stack;
    stack.Install(nodes);

    // configure IPv4
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    // configure UDP
    uint16_t port = 9;
    UdpServerHelper server(port);
    ApplicationContainer serverApp = server.Install(nodes.Get(1));
    serverApp.Start(Seconds(0.0));
    serverApp.Stop(Seconds(simulation_time));

    UdpClientHelper client(interfaces.GetAddress(1), port);
    client.SetAttribute("MaxPackets", UintegerValue(UINT32_MAX));
    client.SetAttribute("Interval", TimeValue(PACKET_INTERVAL));
    client.SetAttribute("PacketSize", UintegerValue(PACKET_SIZE));

    ApplicationContainer clientApp = client.Install(nodes.Get(0));
    clientApp.Start(Seconds(1.0));
    clientApp.Stop(Seconds(simulation_time));

    Simulator::Stop(Seconds(simulation_time));
}

