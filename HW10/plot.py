import numpy as np 
import matplotlib.pyplot as plt

sample = 1024
t = np.linspace(0, 100, sample)
signal = np.zeros(sample)
for i in range(0, sample):
    signal[i] = 2*np.sin(2*np.pi*t[i]) + np.sin(2*np.pi*2*t[i]) + 3*np.sin(2*np.pi*4*t[i])

fourier_transform = np.fft.fft(signal)
amplitude = np.abs(fourier_transform)[0:512]

freq = np.linspace(0, 100, 512)/(2*np.pi*np.pi)
plt.plot(freq, amplitude)
plt.xlabel("Frequency")
plt.ylabel("Amplitude")
plt.title("Frequency Spectrum")
