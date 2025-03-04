import numpy as np
import matplotlib.pyplot as plt
from matplotlib.gridspec import GridSpec 
import pandas as pd
from scipy.optimize import curve_fit
import seaborn as sns

# Set the plotting theme and enable LaTeX text rendering
sns.set_theme(style="darkgrid")
plt.rcParams["text.usetex"] = True

# -------------------------------------------------------
# Data Import and Processing
# -------------------------------------------------------
data = pd.read_csv("capacitors/data/distribution.csv", delimiter=",")

# combine samples of each measurement
data["voltage_mean"] = data.iloc[:, 1:].mean(axis=1)
data["voltage_std"]  = data.iloc[:, 1:].std(axis=1)

# human error in position
data["position_std"] = np.full(data.shape[0], 0.002)

# -------------------------------------------------------
# Define the Model and Fit the Data
# -------------------------------------------------------
def distribution(r, a, b, c, d):
    return a / (1 + np.exp(-(r + b) / d)) + c

popt, pcov = curve_fit(
    distribution,
    xdata=data["position"],
    ydata=data["voltage_mean"],
    sigma=data["voltage_std"],
    absolute_sigma=True
)

print(popt)
print(pcov)

# -------------------------------------------------------
# Plotting
# -------------------------------------------------------
fig, (graph, disk) = plt.subplots(nrows=2, ncols=1, figsize=(12, 8), layout="constrained")

graph.sharex(disk)
graph.set_box_aspect(aspect=1)
disk.set_box_aspect(aspect=1)

graph.errorbar(
    x=data["position"],
    y=data["voltage_mean"],
    xerr=data["position_std"],
    yerr=data["voltage_std"],
    marker='o',
    linestyle="",
    capsize=4,
    label=r"Data points $V_C(r)$"
)

x = np.linspace(
    -0.02 + data["position"].min(),
    0.02 + data["position"].max(),
    1000
)

a, b, c, d = popt
graph.plot(
    x,
    distribution(x, *popt),
    linestyle="-",
    label=r"The best-fit function $V_C(r) = \frac{a}{1 + e^{-\frac{r + b}{d}}} + c$"
)

graph.set_ylim(-0.1 + data["voltage_mean"].min(), 0.1 + data["voltage_mean"].max())

graph.set_ylabel(r"The proximal (near $r$) capacitor voltage, $V_C (V)$", fontsize=14)
graph.legend(fontsize=10)
graph.set_title("Surface charge distribution", fontsize=20)

# -----------------------------------------------------
# Visualize the Disk Top-Down
# ------------------------------------------------------
circle_outline = plt.Circle(
    (0, 0),
    data["position"].max(),
    fill=False,
    edgecolor="black",
    linewidth=2
)
disk.add_patch(circle_outline)

disk.set_xlim(-0.002, data["position"].max() + 0.002)
disk.set_ylim(-0.002, data["position"].max() + 0.002)

disk.set_ylabel(r"The radial distance, $r (m)$")
disk.set_xlabel(r"The radial distance from the center of the capacitor plate, $r (m)$", fontsize=14)

X, Y = np.meshgrid(x, x)
R = np.sqrt(X**2 + Y**2)
R[R > data["position"].max()] = np.nan
V = distribution(R, *popt)

surface = disk.contourf(X, Y, V, levels=1000, cmap="Reds")
cbar = fig.colorbar(surface, ax=disk, format="%.2f", orientation="horizontal", shrink=0.3)
cbar.set_label("charge density degree")

plt.show()
