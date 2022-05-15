from ulab import numpy as np 
import time

sample = 1024
t = np.linspace(0, 100, sample)
signal = np.zeros(sample)
for i in range(0, sample):
    signal[i] = 2*np.sin(2*np.pi*t[i]) + np.sin(2*np.pi*2*t[i]) + 3*np.sin(2*np.pi*4*t[i])

fourier_transform = np.fft.fft(signal)
amplitude = abs(fourier_transform[0])[0:int(sample/2)]
freq = np.linspace(0, 100, int(sample/2))/(2*np.pi*np.pi)

print(amplitude)
print(freq)
