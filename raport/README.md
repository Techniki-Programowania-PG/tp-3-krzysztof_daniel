# Signal Analyzer

Signal Analyzer to biblioteka do Pythona napisna w C++, służąca do:
- generowania sygnałów (sinus, cosinus, prostokątny, piłokształtny),
- obliczania DFT/FFT,
- filtrowania 1D/2D (konwolucja),
- wizualizacji sygnałów i obrazów za pomocą Matplot++.



## Menu
[Strona główna](../README.md)



## Spis treści

1. [Wymagania](#wymagania)
2. [Instalacja i budowanie](./BUILD.md)
3. [Szybki start / Usage](./USAGE.md)
4. [API Reference](#api-reference)
- Pełny opis → [./API.md](./API.md)
5. [Testy](./TESTS.md)
6. [Architektura](./ARCHITECTURE.md)



---

## Wymagania

- CMake ≥ 3.15
- kompilator C++17 (g++ / clang++)
- Python ≥ 3.7
- pybind11
- Matplot++ (pobierane automatycznie przez CMake)
- (opcjonalnie) numpy, scipy do testów
- Wszystkie wymagane i opcjonalne biblioteki do pythona znajdują się w pliku requirements.txt

---


## API Reference

Poniżej krótki przegląd; pełna dokumentacja funkcji i sygnatur w [./API.md](./API.md).

- `generate_signal(type, freq, sampling_rate, duration)`: zwraca próbki sygnału
- `DFT(x, sampling_rate, show=False)`: oblicza dyskretną transformatę Fouriera
- `FFT(x, sampling_rate, show=False)`: szybka transformata Fouriera
- `filter1D(signal, kernel, circular=False)`: konwolucja 1D
- `filter2D(image, kernel, circular=False)`: konwolucja 2D
- `showSimpleSignal(x, y)`, `showComplexSignal(...)`, `showImage(...)`: wizualizacje
