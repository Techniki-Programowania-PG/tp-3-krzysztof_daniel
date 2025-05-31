# API Reference

## Menu
[Powrót](./README.md)

## Enumeracje

### SignalType
- `Sin`
- `Cos`
- `Square`
- `Sawtooth`

## Funkcje

### generate_signal(type, freq, sampling_rate, duration) → List[float]
Generuje sygnał:
- type: `SignalType`
- freq: częstotliwość [Hz]
- sampling_rate: [Hz]
- duration: czas trwania [s]

### DFT(x, sampling_rate, show=False) → List[complex]
Dyskretna Transformata Fouriera:
- x: List[float]
- sampling_rate: [Hz]
- show: bool – wyświetl wykres

### FFT(x, sampling_rate, show=False) → List[complex]
Szybka Transformata Fouriera (padding do potęgi 2):
- parametry jak wyżej

### filter1D(signal, kernel, circular=False) → List[float]
1D convolution:
- signal: List[float]
- kernel: List[float]
- circular: bool – filtr cykliczny

### filter2D(image, kernel, circular=False) → List[List[float]]
2D convolution:
- image: List[List[float]]
- kernel: List[List[float]]
- circular: bool

### showSimpleSignal(x, y)
Prosty wykres liniowy.

### showComplexSignal(signal, sampling_rate, plotTitle="Signal", xLabel="X", yLabel="Y")
Wykres widma (stem) od –Fs/2 do Fs/2.

### showImage(image, title="Image", xLabel="X", yLabel="Y")
Wyświetla macierz jako obraz (`imagesc` + colorbar).

---

## Atrybuty

- `__version__` – wersja biblioteki