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
        angular = data[cols[1]].astype("float")
        
        time.name = f"time{i}"
        angular.name = f"angular{i}"

        result.append(pd.concat([time, angular], axis=1))
        
    result = pd.concat(result, axis=1)
    result.to_csv("gyroscope/data/data.csv", float_format="%.6f", index=False, sep=";", decimal=".")

def average_angvel(start, end):
    pass
    
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

    # print(f"{i}: {mean:0.6f} ± {stderr:0.6f}")

# compute true values with errors and T3
pairs = [(1, 11), (7, 8), (6, 10), (2, 13), (9, 16), (5, 14)]
counterfeits = [3, 4, 12, 15]
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

for idx, (T3, dT3), (weight, dweight, lever, dlever), ang, err in result:
    print(f"{idx}\t\t{T3:0.3f} ± {dT3:0.3f} (s) \t\t{weight} ± {dweight}(g), {lever} ± {dlever}(cm)\t\t{ang:0.6f} ± {err:0.6f} (rad/s)")
    
fig, axes = plt.subplots(nrows=4, ncols=4)
for i in range(0, 4):
    for j in range(0, 4):
        axis = axes[i][j]
        
        idx = i * 4 + j + 1
        time = data[f"time{idx}"]
        angular = data[f"angular{idx}"]

        n = measurements["measurement"][idx - 1]
        mass = measurements["weight"][idx - 1]
        lever = measurements["lever_arm"][idx - 1]
        
        axis.plot(time, angular)
        # axis.text(10, 10, f"{mass}kg, {lever}cm")

        axis.text(0.05, 0.95, f"{n}: {mass}kg, {lever}cm", transform=axis.transAxes, ha="left", va="top", fontsize=10, color="black")
    
plt.show()
