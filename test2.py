import signal_analyzer as sa
import numpy as np
from scipy.io import wavfile

# 1) Wczytaj
#fs, data = wavfile.read("./sdr-tests/SDRconnect_IQ_20250520_164031_137110000HZ.wav")
fs, data = wavfile.read("./sdr-tests/SDRconnect_IQ_20250520_113333_99900000HZ.wav")

print(f"Liczba próbek: {data.shape[0]}, liczba kanałów: {data.shape[1]}")
print(f"Częstotliwość próbkowania: {fs} Hz")

# 2) Normalizuj (jeśli int16)
if data.dtype == np.int16:
    data = data.astype(np.float32) / 32768.0
#print(data)

# 3) Wyodrębnij I/Q
I = data[:,0]
Q = data[:,1]
iq = I + 1j*Q

# 4) Wybierz sygnał y
y = I # lub iq

# 5) Oblicz liczbę próbek dla 0.4 ms
duration_sec = 0.004
samples = int(duration_sec * fs)

# 6) Wytnij pierwsze x ms
y_short = y[:samples]
t_short = np.arange(samples) / fs

N = len(y_short)



# === Generate a 1D signal ===
#sig = sa.generate_signal(sa.SignalType.Square, freq=5, sampling_rate=500, duration=1)
sig=y_short

temp=sa.DFT(sig, fs, False)
sa.showComplexSignal(temp, 500, "title", 'x', "Y")


new=[]
for el in temp:
    new.append(float(el.imag))
sig=new
#exit()
# === Apply 1D smoothing filter ===
kernel1d = [1/3, 1/3, 1/3]
filt = sa.filter1D(sig, kernel1d)

sa.showComplexSignal(filt, 500, "title", 'x', "Y")

image0 = [sig for _ in range(13)]

# === Prepare a 2D image from the filtered 1D signal ===
# Replicate the filtered 1D signal vertically to make a 2D image
image_2d = [filt for _ in range(10)]  # 100 rows of the 1D filtered signal

# === Apply 2D filtering ===
kernel2d = [[1/9]*3 for _ in range(3)]  # 3x3 averaging kernel
filtered_image = sa.filter2D(image_2d, kernel2d)
sa.showComplexSignal(filtered_image, 500, "title", 'x', "Y")

# === Show images ===
#print(image_2d)

sa.showImage(image0, title="Image from DFT")
sa.showImage(image_2d, title="Image from 1D Filtered Signal")
sa.showImage(filtered_image, title="2D Filtered Image")