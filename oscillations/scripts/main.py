import pandas as pd
import seaborn as sb
import matplotlib.pyplot as plt
import numpy as np
from scipy.signal import find_peaks, correlate
from sklearn.preprocessing import StandardScaler
from scipy.optimize import curve_fit

MEASUREMENTS = 2
SAMPLES = 17

# Appearance
sb.set_theme()
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
driven_force_d = GetData("oscillations/data/driven_force_data.csv")
time = driven_force_d["time"].astype("float")


def EvalNaturalFrequency(data: pd.DataFrame, graph=False):
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


def EvaluateAmplitudeSample(angle):     
     maxima = find_peaks(angle)[0]
     minima = find_peaks(-angle)[0]

     min_length = min([ len(maxima), len(minima) ])
     maxima = maxima[0:min_length]
     minima = minima[0:min_length]
     
     amplitudes = 0.5 * (np.abs(np.array(angle[maxima])) + np.abs(np.array(angle[minima])))
     mean_amplitude = np.mean(amplitudes)

     return mean_amplitude


def EvaluateDrivingFrequencySample(time, driven_angle):
     peaks = find_peaks(driven_angle)[0]

     periods = np.diff(time[peaks])
     mean_period = np.mean(periods)
     return 2.0 * np.pi / mean_period 

def EvaluatePhaseSample(time, angle, driven_angle):
     peaks = find_peaks(driven_angle)[0]

     periods = np.diff(time[peaks])
     mean_period = np.mean(periods)
     
     data = np.zeros((len(time), 2))
     data[:, 0] = angle
     data[:, 1] = driven_angle

     xcorr = correlate(data[:, 0], data[:, 1], mode='full')
     dt = np.linspace(
          -time[len(time)-1],
          time[len(time)-1],
          2 * len(time) - 1
     )
     time_shift = dt[xcorr.argmax()]
     
     phase_shift = 2*np.pi*time_shift/mean_period

     return np.arctan(np.sin(phase_shift) / np.cos(phase_shift))


ampls = []
driven_freqs = []
phases = []
for S in range(1, SAMPLES + 1):
     ampls_per_M = []
     driven_freqs_per_M = []
     phases_per_M = []
     voltage = None
     for M in range(1, MEASUREMENTS + 1):
          angle = driven_force_d[f"{M}angle{S}"].astype("float")
          driven_angle = driven_force_d[f"{M}driven_angle{S}"].astype("float")
          voltage = driven_force_d[f"{M}voltage{S}"].astype("float")[0]
          
          ampls_per_M.append(EvaluateAmplitudeSample(angle))
          driven_freqs_per_M.append(EvaluateDrivingFrequencySample(time, driven_angle))
          phases_per_M.append(EvaluatePhaseSample(time, angle, driven_angle))
          
     ampl = np.mean(ampls_per_M)
     ampl_err = 0.5 * np.abs( ampls_per_M[1] - ampls_per_M[0] )

     driven_freq = np.mean(driven_freqs_per_M)
     driven_freq_err = 0.5 * np.abs( driven_freqs_per_M[1] - driven_freqs_per_M[0] )

     phase = np.mean(phases_per_M)
     phase_err = 0.5 * np.abs( phases_per_M[1] - phases_per_M[0] )
     
     ampls.append(np.array( [voltage, ampl, ampl_err] ))
     driven_freqs.append(np.array( [voltage, driven_freq, driven_freq_err] ))
     phases.append(np.array( [voltage, phase, phase_err] ))
     
     
ampls = np.array(ampls)
driven_freqs = np.array(driven_freqs)
phases = np.array(phases)

# Plot
plt.figure(figsize=(10, 6))
plt.errorbar(driven_freqs[:, 1], ampls[:, 1], xerr=driven_freqs[:, 2], yerr=ampls[:, 2], fmt="o", label="Amplitude v/s Driven Frequency")

def ampl_model(x, a, b, c):
     return a / np.sqrt( (b - x * x) * (b - x * x) + c * x * x )

(a, b, c), _ = curve_fit(ampl_model, driven_freqs[:, 1], ampls[:, 1], sigma=ampls[:, 2])
t = np.linspace(1, 5.5, 500)
plt.plot(t, ampl_model(t, a, b, c), color="green", linestyle="--", label="Best-fit")
plt.title("Amplitude v/s Driving Frequency")
plt.xticks(np.arange(1, 5.6, 0.5))
plt.xlabel("Driving Frequency (rad/s)")
plt.ylabel("Amplitude (rad)")
plt.legend()
plt.grid(True)
plt.show()

def phase_model(x, a, b):
     return np.arctan(a * x / ( b - x * x )) 

initial_guess = [4, 19]

t = np.linspace(2, 6, 2000)
(a, b), _ = curve_fit( phase_model, driven_freqs[:, 1], phases[:, 1], p0=initial_guess)
plt.figure(figsize=(10, 6))
plt.plot(t, phase_model(t, a, b), color="green", linestyle="--", label="Best-fit")
plt.errorbar(driven_freqs[:, 1], phases[:, 1], xerr=driven_freqs[:, 2], yerr=phases[:, 2], fmt="o", label="Phase v/s Driven Frequency")
plt.title("Phase v/s Driving Frequency")
plt.xticks(np.arange(1, 5.6, 0.5))
plt.xlabel("Driving Frequency (rad/s)")
plt.ylabel("Phase (rad)")
plt.legend()
plt.grid(True)
plt.show()

# Output
natural_freq, natural_freq_err = EvalNaturalFrequency(simple_harmonics_d)
print(f"Natural Frequency is {natural_freq:.2g} +- {natural_freq_err:.2g} (rad/s)")

print("V\tAmplitude\tDriving Freq\tPhase")
for i in range(len(ampls)):
     voltage = ampls[i][0]
     
     # amplitude
     A = ampls[i][1]
     dA = ampls[i][2]

     # driving frequency
     F = driven_freqs[i][1]
     dF = driven_freqs[i][2]
     
     # phase
     P = phases[i][1]
     dP = phases[i][2]

     print(f"{voltage:.2f}\t{A:.2f} ± {dA:.1g}\t{F:.2f} ± {dF:.1g}\t{P:.2f} ± {dP:.1g}")

