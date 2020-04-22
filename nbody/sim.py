import numpy as np
from matplotlib import pyplot as plt
from matplotlib.animation import FuncAnimation

N = 10
M = 1000
r = np.zeros([N, 3, M])
u = np.zeros([N, 3])
a = np.zeros_like(u)

r[:, 0, 0] = 5 * np.random.random(N) - 2.5
r[:, 1, 0] = 5 * np.random.random(N) - 2.5
r[:, 2, 0] = 5 * np.random.random(N) - 2.5

u[:, 0] = 2 * r[:, 1, 0]
u[:, 1] = -2 * r[:, 0, 0]

m = np.ones(N) * 100#np.random.random(N) * 1000.

G = 100.
dt = 0.03

plt.figure()

for s in range(1, M):
    for i in range(N):
        a[i, :] = [0., 0., 0.]

        for j in range(N):
            if i==j:
                continue

            dx = r[j, :, s-1] - r[i, :, s-1]

            rn = np.linalg.norm(dx)

            a[i, :] = G * m[j] * dx / (rn**2 + 1)

        a[i, :] *= 1. / m[i]

    u += dt * a
    r[:, :, s] = r[:, :, s-1] + dt * u



fig, ax = plt.subplots()
xdata, ydata = [], []
pts = []
pts, = plt.plot(r[:, 0, 0], r[:, 1, 0], 'o')
plt.xlim(-25, 25)
plt.ylim(-25, 25)

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
