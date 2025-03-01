import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from scipy.optimize import curve_fit
import seaborn as sns

# Set the plotting theme and enable LaTeX text rendering
sns.set_theme(style="darkgrid")
plt.rcParams["text.usetex"] = True

# -------------------------------------------------------
# Data Import and Processing
# -------------------------------------------------------
data = pd.read_csv("capacitors/data/capacitor_voltage.csv", delimiter=",")

# combine samples of each measurement
data["voltage_mean"] = data.iloc[:, 2:].mean(axis=1)
data["voltage_std"]  = data.iloc[:, 2:].std(axis=1)

# separate data based on the "separation" value (2mm and 4mm)
data2 = data[data["separation"] == 0.002]
data4 = data[data["separation"] == 0.004]

print(data)

# -------------------------------------------------------
# Define the Model and Fit the Data
# -------------------------------------------------------
def capacitance(q, k, b):
    """ Linear model for capacitor voltage: V = kq + b """
    return q * k + b

# Fit the model for the group with separations 2mm and 4mm
def fit_model(data):
    return curve_fit(
        capacitance,
        data["charges"],
        data["voltage_mean"],
        sigma=data["voltage_std"],
        absolute_sigma = True
    )

popt2, pcov2 = fit_model(data2)
popt4, pcov4 = fit_model(data4)

print(popt2)
print(pcov2)

print(popt4)
print(pcov4)

# ------------------------------------------------------
# Plotting
# ------------------------------------------------------
fig, ax = plt.subplots(figsize=(8, 6))

def plot_error_bars(data, sep, color):
    ax.errorbar(
        data["charges"],
        data["voltage_mean"],
        yerr=data["voltage_std"],
        fmt="o",
        color=f"tab:{color}",
        capsize=4,
        label=rf"Data $s = {sep} (mm)$"
    )

def plot_bestfit(data, popt, color, sep):
    x = np.linspace(data["charges"].min(), data["charges"].max(), 500)

    ax.plot(
        x,
        capacitance(x, *popt),
        color=f"tab:{color}",
        linestyle="-",
        label=rf"Fit ($s = {sep} (mm)$: $V(n) = ({popt[0]:.2f})n + ({popt[1]:.2f})$)"
    )

plot_error_bars(data2, "2", "blue")
plot_error_bars(data4, "4", "orange")
plot_bestfit(data2, popt2, "blue", "2")
plot_bestfit(data4, popt4, "orange", "4")

ax.set_xlabel(r"Number of charges applied, $n$", fontsize=14)
ax.set_ylabel(r"Capacitor voltage, $V_C$ (V)", fontsize=14)
ax.set_title(r"Capacitor voltage vs. Number of charges applied ($V_c(n) = kn + b$)", fontsize=16)

ax.tick_params(axis="both", which="major", labelsize=12)
ax.legend(fontsize=12)

plt.tight_layout()
plt.show()
