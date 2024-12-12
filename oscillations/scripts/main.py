import pandas as pd
import seaborn as sb
import matplotlib.pyplot as plt
import numpy as np
from scipy.signal import find_peaks

# Appearance
sb.set()
pd.options.display.max_rows = 9999


# Globals
dt = 0.050 # from CapStone

# Utils
def GetData(filepath) -> pd.DataFrame:
     data = pd.read_csv(filepath, delimiter=";")
     data = data.dropna()

     return data
    
# Acquire data
simple_harmonics_d = GetData("oscillations/data/simple_harmonics.csv")

# Lab Exercises
def EvalNaturalFrequency(data: pd.DataFrame, graph=False) -> (float, float):
    global dt

    time = data["time"].astype("float")
    angle = data["angle"].astype("float")

    # calculate mean period
    peaks = find_peaks(angle)[0][0:-1]
    periods = np.diff(time[peaks])
    period = np.mean(periods)

    # calculate natural frequency and error
    natural_frequency = 2 * np.pi / period
    error = 2 * np.pi * dt / np.power(period, 2)

    if graph:
        plt.plot(time, angle, label="Time versus ")
        
        for peak in peaks:
            plt.axvline(time[peak])

    return (natural_frequency, error)



# Output
freq, err = EvalNaturalFrequency(simple_harmonics_d, graph=True)
print(f"Natural Frequency is {freq:0.1f} +- {err:0.1f} (rad/s)")

plt.show()
