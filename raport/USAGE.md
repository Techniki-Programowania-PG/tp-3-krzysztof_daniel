# Szybki start / Usage

## Menu
[Powrót](./README.md)

## 1. Generowanie i rysowanie sygnału

```python
import signal_analyzer as sa

# parametr
freq = 10 # Hz
fs = freq * 50 # Hz
dur = 1.0 # s

# wygeneruj sinus
x = sa.generate_signal(sa.SignalType.Sin, freq, fs, dur)
t = [n/fs for n in range(len(x))]

# pokaż
sa.showSimpleSignal(t, x)
```

## 2. DFT i FFT
```python
# DFT z wykresem
X_dft = sa.DFT(x, fs, show=True)

# FFT z wykresem
X_fft = sa.FFT(x, fs, show=True)
```

## 3. Filtracja
```python
# 1D: uśrednianie z 3 próbek
kernel1d = [1/3]*3
y_filt = sa.filter1D(x, kernel1d, circular=False)

# 2D: stworzenie obrazu z 1D
image = [y_filt for _ in range(100)]
kernel2d = [[1/9]*3 for _ in range(3)]
img_filt = sa.filter2D(image, kernel2d)

# rysuj
sa.showImage(image, title="Original")
sa.showImage(img_filt, title="Filtered")
```

## 4. Przykładowe skrypty
test.py – demonstracja DFT/FFT na sygnale piłokształtnym
test2.py – analiza plików WAV (I/Q), filtrowanie i wizualizacja
