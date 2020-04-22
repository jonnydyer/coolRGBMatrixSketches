import numpy as np
from matplotlib import pyplot as plt
from matplotlib.animation import FuncAnimation

N = 15
M = 1000
r = np.zeros([N, 3, M])
u = np.zeros([N, 3])

r[:, 0, 0] = 10 * np.random.random(N) - 5
r[:, 1, 0] = 10 * np.random.random(N) - 5
r[:, 2, 0] = 10 * np.random.random(N) - 5

m = np.ones(N) * 100 #np.random.random(N) * 1000.

G = 100.
dt = 0.01

Mc = 500.

dist_min = 0.2
G_exp = 2.

uc = 8.

u[:, 0] = -uc * r[:, 1, 0]
u[:, 1] = uc * r[:, 0, 0]

u[-1, 0] = - np.sum(m[0:-1] * u[0:-1, 0]) / m[-1]
u[-1, 1] = - np.sum(m[0:-1] * u[0:-1, 1]) / m[-1]


def a_func(r, m, G, G_exp, Mc, dist_min):
    a = np.zeros_like(u)
    for i in range(N):
        for j in range(N):
            if i==j:
                continue

            dx = r[j, :] - r[i, :]

            rn = np.linalg.norm(dx) + dist_min

            a[i, :] += G * m[j] * dx / (rn**G_exp)

        rn = np.linalg.norm(r[i, :]) + dist_min
        a[i, :] += -G * Mc * r[i, :] / rn**G_exp
        a[i, :] *= 1. / m[i]
    return a

ai = a_func(r[:, :, 0], m, G, G_exp, Mc, dist_min)
u1_2 = u + ai*dt
r[:, :, 1] = r[:, :, 0] + u1_2 * dt

for i in range(1, M-1):
    # Following https://en.wikipedia.org/wiki/Leapfrog_integration
    ai = a_func(r[:, :, i], m, G, G_exp, Mc, dist_min)
    u1_2 = u1_2 + ai * dt
    r[:, :, i+1] = r[:, :, i] + u1_2 * dt


fig, ax = plt.subplots()
xdata, ydata = [], []
pts = []
pts, = plt.plot(r[:, 0, 0], r[:, 1, 0], 'o')
plt.xlim(-16, 16)
plt.ylim(-8, 8)

def init():
    return pts,

def update(frame):
    pts.set_data(r[:, 0, frame], r[:, 1, frame])
    return pts,

ani = FuncAnimation(fig, update, frames=np.arange(0, M),
                    init_func=init, interval=30)

#plt.plot(r[:, 0, :].T,
#         r[:, 1, :].T, '.')
plt.show()
