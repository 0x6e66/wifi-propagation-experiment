#pragma once

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/spectrum-module.h"
#include "ns3/propagation-module.h"
#include "ns3/flow-monitor-module.h"
#include <numeric>
#include <stdexcept>
#include <fstream>

std::vector<double> rssi;
std::vector<double> snr;
std::vector<double> noise;

// callback function to collect data on physical layer
void callback_recv_packets(
    std::string context,
    ns3::Ptr<const ns3::Packet> packet,
    uint16_t channelFreqMhz,
    ns3::WifiTxVector txVector,
    ns3::MpduInfo aMpdu,
    ns3::SignalNoiseDbm signalNoise,
    uint16_t staId
) {
    rssi.push_back(signalNoise.signal);
    noise.push_back(signalNoise.noise);
    snr.push_back(signalNoise.signal / signalNoise.noise);
}

// helper function to calculate the mean of a given vector
double calc_mean(std::vector<double> &vec) {
    return vec.empty() ? 0 : std::accumulate(vec.begin(), vec.end(), 0.0) / vec.size();
}

// helper function to calculate the standard deviation of a given vector and mean
double calc_std_dev(std::vector<double> &vec, double mean) {
    double sq_sum = std::accumulate(
        vec.begin(),
        vec.end(),
        0,
        [mean](double acc, int value) { return acc + pow(value - mean, 2);});
    return sqrt(sq_sum / vec.size());
}