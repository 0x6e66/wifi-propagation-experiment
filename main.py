from matplotlib import pyplot as plt
import pandas as pd
import numpy as np

def main():
    # runtime_throughput()
    distance_throughput()
    # rssi_throughput()


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

    plt.show()


def rssi_throughput():
    data = {
        "friis": pd.read_csv("friis.csv").iloc[1:],
        "three_log":  pd.read_csv("three_log.csv").iloc[1:],
        "two_ray": pd.read_csv("two_ray.csv").iloc[1:],
    }
    lines = []

    fig, ax0 = plt.subplots()
    for model, df in data.items():
        distance = df["distance"]
        flow_throughput = df["flow_throughput"]

        for i in range(1, len(flow_throughput)-1):
            if abs(flow_throughput[i] - flow_throughput[i+1]) > 3:
                x = (2*i+1) / 2
                if x not in lines:
                    lines.append(x)

        ax0.plot(distance, flow_throughput, label=model)

    ax0.set_xlabel("Distance (m)")
    ax0.set_ylabel("Throughput (Mbps)")
    ax0.legend()
    ax0.grid(True)

    lines.sort()

    ax1 = ax0.twinx()

    for model, df in data.items():
        distance = df["distance"]
        rssi = df["rssi_average"]

        ax1.plot(distance, rssi, label=model)

    ax1.vlines(x=lines, ymin=[-79, -81.5, -79, -81.5], ymax=[-60,-60,-60,-60], ls=":", colors="red")
    ax1.hlines(y=[-77.4, -80.6, -81.96], xmin=[60, 80, 97], xmax=[150, 220, 245], ls=":", colors="red")
    ax1.set_ylabel("Signal Strength (dBm)")

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