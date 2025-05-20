import numpy as np
from scipy.io import wavfile
import signal_analyzer as sa

# 1) Wczytaj
fs, data = wavfile.read("./SDRconnect_IQ_20250520_164031_137110000HZ.wav")
#fs, data = wavfile.read("./SDRconnect_IQ_20250520_113333_99900000HZ.wav")

print(f"Liczba próbek: {data.shape[0]}, liczba kanałów: {data.shape[1]}")
print(f"Częstotliwość próbkowania: {fs} Hz")

# 2) Normalizuj (jeśli int16)
if data.dtype == np.int16:
    data = data.astype(np.float32) / 32768.0
print(data)

# 3) Wyodrębnij I/Q
I = data[:,0]
Q = data[:,1]
iq = I + 1j*Q

# 4) Wybierz sygnał y
y = I # lub iq

# 5) Oblicz liczbę próbek dla 5 ms
duration_sec = 0.05
samples = int(duration_sec * fs)

# 6) Wytnij pierwsze 5 ms
y_short = y[:samples]
t_short = np.arange(samples) / fs

N = len(y_short)

# 7) Rysuj i oblicz DFT dla tego fragmentu
#sa.showSignal(t_short, y_short)

sa.DFT(y_short, fs, True)
sa.FFT(y_short, fs, True)
