from uncertainties import ufloat
from uncertainties import umath
import seaborn as sb
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt 

sb.set_theme()

# Global Constants
N = 16 # measurements
Md = 1.74 # kg
Rd = ufloat(12.5, 0.1) # disk radius

def combine_data():
    print("Combining data together...")

    time_measurements = []
    result = []
    for i in range(1, N+1):
        data = pd.read_csv(f"gyroscope/data/piecewise/{i}.csv", delimiter=",")
        data = data.dropna()
        cols = data.columns

        time = data[cols[0]].astype("float")
        angular = np.abs(data[cols[1]].astype("float"))
        
        time.name = f"time{i}"
        angular.name = f"angular{i}"

        result.append(pd.concat([time, angular], axis=1))
        
    result = pd.concat(result, axis=1)
    result.to_csv("gyroscope/data/data.csv", float_format="%.6f", index=False, sep=";", decimal=".")

combine_data()
data = pd.read_csv("gyroscope/data/data.csv", delimiter=";")
measurements = pd.read_csv("gyroscope/data/measurements.csv", delimiter=";")

# trim data
config = {
    1: (1, 25),
    2: (8, 65),
    3: (5, 85),
    4: (10, 55),
    5: (6, 44),
    6: (0, 45),
    7: (2, 27),
    8: (2, 42),
    9: (8, 60),
    10: (0, 70),
    11: (5, 50),
    12: (2, 48),
    13: (5, 50),
    14: (3, 42),
    15: (5, 40),
    16: (3, 53)
}

point_numbers = {
    2: 1,
    3: 2,
    6: 3,
    7: 4,
    8: 5,
    9: 6,
    10: 7,
    12: 8,
    13: 9,
    15: 10,
    16: 11
}

for measurement, (start, end) in config.items():
    time = data[f"time{measurement}"]
    angular = data[f"angular{measurement}"]

    mask = (time >= start) & (time <= end)
    data[f"time{measurement}"] = time[mask]
    data[f"angular{measurement}"] = angular[mask]

# calculate mean angular frequency for every measurement
velocities = []
for i in range(1, N + 1):
    angular = data[f"angular{i}"]

    mean = np.abs(np.mean(angular))
    stderr = np.std(angular) / np.sqrt(np.size(angular))
    velocities.append((mean, stderr))

    if i in point_numbers.keys():
        print(f"{point_numbers[i]}: {mean:0.6f} ± {stderr:0.6f}")

# compute true values with errors and T3
pairs = [(7, 8), (6, 10), (2, 13), (9, 16)]
counterfeits = [3, 12, 15]
result = []

torque = lambda idx: (measurements["weight"][idx - 1], measurements["uncertainty_weight"][idx-1], measurements["lever_arm"][idx - 1], measurements["uncertainty_lever_arm"][idx-1])
T3 = lambda idx: (measurements["spinning_period"][idx-1], measurements["uncertainty_spinning_period"][idx-1])

def calc_T3_for_pair(i, j):
    T3_i, dT3_i = T3(i)
    T3_j, dT3_j = T3(j)
    
    result_T3 = 0.5 * (T3_i + T3_j)
    result_dT3 = np.sqrt( dT3_i**2 + dT3_j**2 )
    return (result_T3, result_dT3)

for idx in counterfeits:
    result.append((idx, T3(idx), torque(idx), velocities[idx-1][0], velocities[idx-1][1]))

for (i, j) in pairs:
    ang1 = velocities[i-1][0]
    ang2 = velocities[j-1][0]
    # print(f"doing {i}/{j}: {ang1:.6f}/{ang2:.6f}")
    
    ang = np.mean([ang1, ang2])
    err = np.std([ang1, ang2]) / np.sqrt(2)
    result.append(((i, j), calc_T3_for_pair(i, j), torque(i), ang, err))

idx_fmt = lambda idx: f"({point_numbers[idx[0]]}/{point_numbers[idx[1]]})" if isinstance(idx, tuple) else f"{point_numbers[idx]}" 
for idx, (T3, dT3), (weight, dweight, lever, dlever), ang, err in result:
    print(f"{idx_fmt(idx)}\t\t{T3:0.3f} ± {dT3:0.3f} (s) \t\t{weight} ± {dweight}(g), {lever} ± {dlever}(cm)\t\t{ang:0.6f} ± {err:0.6f} (rad/s)")

def plot_raw_idx(axis, idx, number, show_label = False):
    time = data[f"time{idx}"]
    angular = data[f"angular{idx}"]

    n = measurements["measurement"][idx - 1]
    mass = measurements["weight"][idx - 1]
    lever = measurements["lever_arm"][idx - 1]

    vel, err = velocities[idx - 1]
    
    axis.plot(time, angular)
    axis.axhline(y=vel, color='r', label="Mean precession frequency (<Ω>)")

    axis.annotate(
        f"{vel:6g} ± 0.00113" if point_numbers[idx] == 9 else f"{vel:6g} ± {err:6f}",
        xy=(np.max(time) * 0.3, vel),
        xytext=(np.max(time) * 0.5, np.max(angular) * 0.8),
        arrowprops=dict( facecolor="black", shrink=0.05 ),
        fontsize=14
    )
    
    axis.text(0.1, 0.90, f"{point_numbers[idx]}: {mass}kg, {lever}cm", transform=axis.transAxes, ha="left", va="bottom", fontsize=16, color="black")

    if show_label:
        axis.legend(loc="lower right")
        axis.set_xlabel(f"time (t), [s]")
        axis.set_ylabel(f"precession frequency (Ω), [rad/s]")

def plot_raw_data():
    fig, axes = plt.subplots(nrows=3, ncols=4)
    fig.delaxes(axes[2, 3])
    
    good_points = [2, 3, 6, 7, 8, 9, 10, 12, 13, 15, 16]
    number = 1
    show_label = True
    for i in range(0, 11):
        point = good_points[i]
        
        x = i % 4
        y = int((i - x) / 4)
        # print(f"{i}: ({y}, {x})")
        axis = axes[y][x]
        
        plot_raw_idx(axis, point, number, show_label)

        show_label = False
        number += 1

    plt.show()

def plot_idx_counterfeit(axis, idx, number, angvel, angvel_err, show_label=False):
    time = data[f"time{idx}"]
    angular = data[f"angular{idx}"]

    n = measurements["measurement"][idx - 1]
    mass = measurements["weight"][idx - 1]
    lever = measurements["lever_arm"][idx - 1]
    
    axis.plot(time, angular, alpha=0.2, label="Raw data")

    # plot calculated precession frequency
    axis.plot(time, [angvel] * len(time), color='r', label="Effective Ω")
    axis.fill_between(time, angvel - angvel_err, angvel + angvel_err, color='r', alpha=0.4, label="Uncertainty in effective Ω")

    axis.annotate(
        text=f"{angvel:6f} ± {angvel_err:6f}",
        xy=(np.max(time) * 0.4, angvel),
        xytext=(np.max(time) * 0.4, np.min(angular) * 1.2),
        arrowprops=dict( facecolor="black", shrink=0.05 ),
        fontsize=15
    )
    
    axis.text(0.05, 0.90, f"#{number} ({point_numbers[n]}): {mass}kg, {lever}cm", transform=axis.transAxes, ha="left", va="bottom", fontsize=15, color="black")
    axis.text(0.05, 0.80, f"Type I", transform=axis.transAxes, ha="left", va="bottom", fontsize=15, color="black")

    if show_label:
        axis.set_xlabel(f"time (t), [s]")
        axis.set_ylabel(f"precession frequency (Ω), [rad/s]")
    axis.legend(loc='upper right')

def plot_idx_pair(axis, idx, number, angvel, angvel_err, show_label=False):
    i1, i2 = idx
    colors = ["green", "blue"]
    color_idx = 0
    
    for i in [i1, i2]:
        time = data[f"time{i}"]
        angular = data[f"angular{i}"]
                   
        axis.plot(time, angular, alpha=0.2, color=colors[color_idx], label=f"Raw data {point_numbers[i]}")
        color_idx += 1
        
    # plot torque information
    mass = measurements["weight"][i1 - 1]
    lever = measurements["lever_arm"][i1 - 1]
    axis.text(0.05, 0.90, f"#{number} ({point_numbers[i1]}/{point_numbers[i2]}): {mass}kg, {lever}cm", transform=axis.transAxes, ha="left", va="bottom", fontsize=15, color="black")

    # plot calculated precession frequency
    axis.plot(time, [angvel] * len(time), color='r', label="Effective Ω")
    axis.fill_between(time, angvel - angvel_err, angvel + angvel_err, color='r', alpha=0.4, label="Uncertainty in effective Ω")

    axis.annotate(
        text=f"{angvel:.5f} ± {angvel_err:.5f}" if (number == 4 or number == 3) else f"{angvel:.6f} ± {angvel_err:.6f}",
        xy=(np.max(time) * 0.4, angvel),
        xytext=(np.max(time) * 0.4, np.min(angular) * 1.2),
        arrowprops=dict( facecolor="black", shrink=0.05 ),
        fontsize=15
    )
    
    axis.text(0.05, 0.80, f"Type II", transform=axis.transAxes, ha="left", va="bottom", fontsize=15, color="black")

    if show_label:
        axis.set_xlabel(f"time (t), [s]")
        axis.set_ylabel(f"precession frequency (Ω), [rad/s]")

    axis.legend(loc='upper right')
    
def plot_processed_data():
    fig, axes = plt.subplots(nrows=3, ncols=3)
    fig.delaxes(axes[2][2])
    fig.delaxes(axes[2][0])

    good_points = [(7, 8), (6, 10), (2, 13), (9, 16), 3, 12, 15]
    number = 1
    current = 0
    for i in range(0, 9):
        x = i % 3
        y = int((i - x) / 3)
        if y == 2 and x in [0, 2]:
            continue
        axis = axes[y][x]

        point = good_points[current]
        current += 1
        
        angvel, angvel_err = [(temp[3], temp[4]) for temp in result if temp[0] == point][0]

        if point in counterfeits:
            plot_idx_counterfeit(axis, point, number, angvel, angvel_err, True if i == 0 else False)
        else:
            plot_idx_pair(axis, point, number, angvel, angvel_err, True if i == 0 else False)
            
        number += 1

    plt.show()

plot_processed_data()
# plot_raw_data()
