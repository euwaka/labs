import pandas as pd
import seaborn as sb
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import FormatStrFormatter
from scipy.signal import find_peaks, correlate
from sklearn.preprocessing import StandardScaler
from scipy.optimize import curve_fit

N = 500
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
fig, axes = plt.subplots(1, 2)
ax1, ax2 = axes

ax1.scatter(driven_freqs[:, 1], ampls[:, 1], color="black")
ax1.errorbar(driven_freqs[:, 1], ampls[:, 1], xerr=driven_freqs[:, 2], yerr=ampls[:, 2], fmt="o", label="Data", alpha = 0.4, color="black")

def ampl_model(x, a, b, c):
     return a / np.sqrt( (b - x * x) * (b - x * x) + c * x * x )

popt, pcov = curve_fit(ampl_model, driven_freqs[:, 1], ampls[:, 1], sigma=ampls[:, 2])
a, b, c = popt
b_err = np.sqrt(pcov[1, 1])
c_err = np.sqrt(pcov[2, 2])

print(f"a = {a:.2f}\tb = {b:.2f}\tc = {c:.2f}\nerr_b = {b_err:.2f}\t err_c = {c_err:.2f}")

t = np.linspace(1, 5.5, 500)
samples = np.random.multivariate_normal(popt, pcov, N)
sampled_curves = np.array([ampl_model(t, *sample) for sample in samples])

# Calculate mean and standard deviation for the sampled curves
mean_curve = np.mean(sampled_curves, axis=0)
std_curve = np.std(sampled_curves, axis=0)

# Plot the confidence interval (mean ± 1σ)
ax1.fill_between(t, mean_curve - std_curve, mean_curve + std_curve, color="green", alpha=0.3, label=r"$1\sigma$ best-fit confidence")

# Find the resonance frequency from amplitude-frequency graph
resonance_ampl = t[find_peaks(ampl_model(t, a, b, c))[0]][0]
temp = np.sqrt( b - c / 2 )
resonance_ampl_err = resonance_ampl * np.sqrt( np.power( b_err / 2 / temp, 2 ) + np.power( c_err / 4 / temp, 2 ))
print(f"Resonance Frequency from Amplitude/Frequency = {resonance_ampl:.2f} ± {resonance_ampl_err:.1g}")

ax1.axvline(resonance_ampl, color="red", linestyle="--", label="Resonance Frequency Line")

plt.rcParams['text.usetex'] = True
ax1.plot(t, ampl_model(t, a, b, c), color="green", linestyle="--", label=r"Best-fit $A(x, a, b, c) = \frac{a}{\sqrt{ (b-x^2)^2 + cx^2 }}$")
ax1.xaxis.set_major_formatter(FormatStrFormatter("%.2f"))
ax1.yaxis.set_major_formatter(FormatStrFormatter("%.2f"))
ax1.set_xticks(list(np.arange(1, 5.6, 0.5)) + [resonance_ampl])
ax1.set_xlabel("Driving Frequency (rad/s)")
ax1.set_ylabel("Amplitude (rad)")
ax1.tick_params(axis="x", rotation=45)
ax1.legend(fontsize=18)
ax1.grid(True)

def phase_model(x, a, b):
     return np.arctan(a * x / ( b - x * x )) 

initial_guess = [4, 19]

t = np.linspace(2, 6, 2000)
popt, pcov = curve_fit( phase_model, driven_freqs[:, 1], phases[:, 1], p0=initial_guess)
a, b = popt
err_b = np.sqrt( pcov[1, 1] )

print(f"a = {a:.2f}\tb = {b:.2f}\nerr_b = {b_err:.2f}")

samples = np.random.multivariate_normal(popt, pcov, N)
sampled_curves = [phase_model( t, *sample ) for sample in samples]
mean_curve = np.mean(sampled_curves, axis=0)
std_curve = np.std(sampled_curves, axis=0)

# Plot the confidence interval (mean ± 1σ)
ax2.fill_between(t, mean_curve - std_curve, mean_curve + std_curve, color="green", alpha=0.3, label="$1\sigma$ best-fit confidence")
ax2.plot(t, phase_model(t, a, b), color="green", linestyle="--", label=r"Best-fit $\phi(x, a, b) = \arctan \frac{ax}{b - x^2}$")

# find the resonance frequency from the phase-frequency graph
resonance_phase = t[find_peaks(phase_model(t, a, b))[0]][0]
resonance_phase_err = err_b / 2 / np.sqrt(b)

print(f"Resonance Frequency from Phase/Frequency = {resonance_phase:.2f} ± {resonance_phase_err:.1g}")
ax2.axvline(resonance_phase, color="red", linestyle="--", label="Resonance Frequency Line")

ax2.scatter(driven_freqs[:, 1], phases[:, 1], color="black")
ax2.errorbar(driven_freqs[:, 1], phases[:, 1], xerr=driven_freqs[:, 2], yerr=phases[:, 2], fmt="o", alpha=0.4, color="black", label="Data")
ax2.xaxis.set_major_formatter(FormatStrFormatter("%.2f"))
ax2.yaxis.set_major_formatter(FormatStrFormatter("%.2f"))
ax2.set_xticks(list(np.arange(2, 5.6, 0.55)) + [resonance_phase])
ax2.tick_params(axis="x", rotation=45)
ax2.set_xlabel("Driving Frequency (rad/s)")
ax2.set_ylabel("Phase (rad)")
ax2.legend(fontsize=16)
ax2.grid(True)

# Output
natural_freq, natural_freq_err = EvalNaturalFrequency(simple_harmonics_d)
print(f"Natural Frequency is {natural_freq:.2g} ± {natural_freq_err:.1g} (rad/s)")

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

plt.show()
