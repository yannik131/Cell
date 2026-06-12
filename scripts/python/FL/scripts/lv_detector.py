import pandas as pd 
import numpy as np 
from pathlib import Path 
import matplotlib.pyplot as plt 
from scipy.signal import savgol_filter, find_peaks, correlate, correlation_lags

HERE = Path(__file__).resolve().parent 

df = pd.read_csv(HERE / "../datasets/example/bad.csv")
t, prey, predator = df["ElapsedTime[s]"], df['Prey'], df['Predator']

fig, axes = plt.subplots(3, 1)
df.drop(columns=["Resource"]).plot(x="ElapsedTime[s]", ax=axes[0])
axes[0].set_title("Original")

dt = 0.003
T = 3.0
A = 300.0

W = int(round(T / 10 / dt)) | 1
prom = 0.05 * A 

prey = savgol_filter(prey, W, 3)
predator = savgol_filter(predator, W, 3)

px, _ = find_peaks(prey, prominence=prom)
py, _ = find_peaks(predator, prominence=prom)

for x in t.iloc[px]:
    axes[1].axvline(x, color="green", linestyle="--", alpha=0.5)

for x in t.iloc[py]:
    axes[1].axvline(x, color="red", linestyle="--", alpha=0.5)

axes[1].plot(t, prey, label="prey", color="green")
axes[1].plot(t, predator, label="predator", color="red")
axes[1].set_title("Smoothed + detected peaks")

prey = (prey - prey.mean()) / prey.std()
predator = (predator - predator.mean()) / predator.std()

r = correlate(prey, predator, mode="full")
lags = correlation_lags(len(prey), len(predator), mode="full") * dt 
i = np.argmax(r)
best_lag = lags[i]
best_corr = r[i]
mask = (lags >= -30) & (lags <= 30)

axes[2].plot(lags[mask], r[mask])
axes[2].set_title(f"Correlation lags, peak at {best_lag}, {best_corr}")

plt.legend()
plt.show()