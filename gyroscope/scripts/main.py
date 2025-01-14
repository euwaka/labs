from uncertainties import ufloat
from uncertainties import umath
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt 

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

# combine_data()
data = pd.read_csv("gyroscope/data/data.csv", delimiter=";")

plt.figure(figsize=(10, 6))
for i in range(1, N + 1):
    time = data[f"time{i}"]
    angular = data[f"angular{i}"]

    plt.plot(time, angular)

plt.show()
