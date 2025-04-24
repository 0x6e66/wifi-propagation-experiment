from matplotlib import pyplot as plt
import pandas as pd
import numpy as np

def main():
    # runtime_throughput()
    distance_throughput()


def distance_throughput():
    data = {
        "friis": pd.read_csv("friis.csv").iloc[1:],
        "fixed_rss": pd.read_csv("fixed_rss.csv").iloc[1:],
        "three_log":  pd.read_csv("three_log.csv").iloc[1:],
        "two_ray": pd.read_csv("two_ray.csv").iloc[1:],
        "nakagami": pd.read_csv("nakagami.csv").iloc[1:],
    }

    fig, ax = plt.subplots(2, 1, layout="constrained")

    plot_throughput(ax[0], data)
    plot_rssi(ax[1], data)
    # plot_snr(ax[2], data)
    # plot_noise(ax[3], data)

    plt.show()


def plot_throughput(ax: plt.Axes, data: dict):
    for model, df in data.items():
        distance = df["distance"]
        flow_throughput = df["flow_throughput"]

        ax.plot(distance, flow_throughput, label=model)

    ax.set_xlabel("Distance (m)")
    ax.set_ylabel("Throughput (Mbps)")
    ax.set_title("Flow Throughput")
    ax.legend()
    ax.grid(True)


def plot_snr(ax: plt.Axes, data: dict):
    for model, df in data.items():
        distance = df["distance"]
        snr = df["snr_average"]

        ax.plot(distance, 1-snr, label=model)

    ax.set_xlabel("Distance (m)")
    ax.set_title("SNR")
    ax.legend()
    ax.grid(True)


def plot_rssi(ax: plt.Axes, data: dict):
    for model, df in data.items():
        distance = df["distance"]
        rssi = df["rssi_average"]

        ax.plot(distance, rssi, label=model)

    ax.set_xlabel("Distance (m)")
    ax.set_ylabel("Signal Strength (dBm)")
    ax.set_title("RSSI")
    ax.legend()
    ax.grid(True)


def plot_noise(ax: plt.Axes, data: dict):
    for model, df in data.items():
        distance = df["distance"]
        noise = df["noise_average"]

        ax.plot(distance, noise, label=model)

    ax.set_xlabel("Distance (m)")
    ax.set_ylabel("Signal Strength (dBm)")
    ax.set_title("Noise")
    ax.legend()
    ax.grid(True)


def runtime_throughput():
    df = pd.read_csv("determine_simulation_time.csv")

    distance = df["distance"]
    simulation_time = df["flow_throughput"]

    a, b = np.polyfit(np.exp(-distance), simulation_time, 1)
    curve = a * np.exp(-distance) + b

    fig, ax = plt.subplots()
    ax.plot(distance, simulation_time, label="raw data")
    ax.plot(distance, curve, label="fitted curve")
    ax.set_xlabel("Simulation Time (s)")
    ax.set_ylabel("Throughput (Mbps)")
    ax.set_title("Throughput for TwoRay-Model at 20m")
    ax.legend()
    ax.grid(True)

    plt.show()


if __name__ == '__main__':
    main()