import numpy as np
import matplotlib.pyplot as plt

def z_func(x, y):
    return 0.5 * np.sqrt(16 - 4*x*x - y*y)

def y_func(x, A):
    return A * np.power(x, 1/4)

x = np.linspace(0, 4, 5000)
y = np.linspace(0, 4, 5000)
X, Y = np.meshgrid(x, y)

Z = z_func(X, Y)

# For curves
Y_A1 = y_func(x, 1.0)
Y_A2 = y_func(x, 2.0)
Y_A3 = y_func(x, 1.3)
Y_A4 = y_func(x, 4.1)

Z_A1 = z_func(x, Y_A1)
Z_A2 = z_func(x, Y_A2)
Z_A3 = z_func(x, Y_A3)
Z_A4 = z_func(x, Y_A4)


plt.gca().set_aspect("equal", adjustable="box")
plt.grid(True)

def draw_contour_and_path(savepath=None):    
    contour = plt.contour(X, Y, Z, 7, cmap="viridis")
    plt.colorbar(contour, label="Height (z)")
    plt.clabel(contour, fmt="%.2f", colors="black")

    plt.plot(x, Y_A1, color="red", label="A = 1.0")
    plt.plot(x, Y_A2, color="red", label="A = 2.0")
    plt.plot(x, Y_A3, color="red", label="A = 1.3")
    plt.plot(x, Y_A4, color="red", label="A = 4.1")
    
    plt.xlabel('x')
    plt.ylabel('y')
    plt.title('Contour plot with path')
    plt.legend()

    if savepath:
        plt.savefig(savepath, format="png", dpi=300)
    
def draw3D(savepath=None):
    fig = plt.figure()
    ax = fig.add_subplot(projection="3d")

    ax.plot_surface(X, Y, Z, alpha=0.5,
                   label="surface: 4x^2+y^2+4z^2=16, z>=0")

    # plot paths for different values of A
    
    path1, = ax.plot(x, Y_A1, Z_A1, label="Curve (A=1.0)")
    path2, = ax.plot(x, Y_A2, Z_A2, label="Curve (A=2.0)")
    path3, = ax.plot(x, Y_A3, Z_A3, label="Curve (A=1.3)")
    path4, = ax.plot(x, Y_A4, Z_A4, label="Curve (A=4.1)")

    ax.legend(handles=[path1, path2, path3, path4]);
    ax.set_aspect("equal")

    if savepath:
        plt.savefig(savepath, format="png", dpi=300)
    
draw_contour_and_path("calculus/W3/img/raindrop.png")
draw3D("calculus/W3/img/surface.png")
# plt.show()
