import matplotlib
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import FormatStrFormatter

RESOLUTION = 1000
LINEAR_SIZE_COLS = 2
LINEAR_SIZE_ROWS = 3
# EPS = 1e-5

matplotlib.rcParams.update({'font.size': 14})

def phase_func(driving_freq, damping, natural_freq):
    return np.arctan( 2 * damping * driving_freq / (natural_freq * natural_freq - driving_freq * driving_freq) )

fig, axs = plt.subplots(LINEAR_SIZE_ROWS, LINEAR_SIZE_COLS)
fig.tight_layout(pad=1.0)

damping_arr = np.array([[2.0, 0.2],
                        [45.0, 17.0],
                        [4.0, 13.4]])
natural_freq_arr = np.array([[5.0, 1.0],
                             [33.0, 6.0],
                             [4.0, 13.4]])

for i in range(0, LINEAR_SIZE_ROWS):
    for j in range(0, LINEAR_SIZE_COLS):
        damping = damping_arr[i, j]
        natural_freq = natural_freq_arr[i, j]
        ax = axs[i, j]
            
        driving_freq = np.linspace(0, 2 * natural_freq, RESOLUTION)
        phase = phase_func(driving_freq, damping, natural_freq)
    
        ax.plot(driving_freq, phase, label="plot")
        ax.axhline(y = np.pi / 2, color='r', linestyle='-')

        type_damping = "Underdamping: " if damping < natural_freq else ("Overdamping: " if damping > natural_freq else "Critical damping: ")
        ax.set_title(type_damping + f"γ={damping} (1/s), ω={natural_freq} (1/s)")
        ax.grid(True)
        ax.set_xlabel("ω_d (1/s)")
        ax.set_ylabel("φ (rad)")
        
        # Custom radian ticks and tick labels
        custom_ticks = [-3*np.pi/4, -np.pi/2, -np.pi/4, 0, np.pi/4, np.pi/2, 3*np.pi/4]
        custom_tick_labels = ['$-3\pi/4$', '-$\pi/2$', '$-\pi/4$', '$0$', '$\pi/4$', '$\pi/2$', '$3\pi/4$']
        ax.set_yticks(custom_ticks)
        ax.set_yticklabels(custom_tick_labels)
        ax.xaxis.set_major_formatter(FormatStrFormatter('%0.1f'))
        
plt.show()
