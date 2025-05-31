# Architektura

## Menu
[Powrót](./README.md)

## 1. C++ Core (src/main.cpp)

- Namespace `py = pybind11`
- Moduł `_core` rejestrujący funkcje dla Pythona
- Operacje:
- Generacja sygnałów
- DFT / FFT (Cooley–Tukey)
- 1D / 2D konwolucja
- Wykresy via Matplot++

## 2. Python Wrapper (src/signal_analyzer/__init__.py)

- `from ._core import *` – przekazuje wszystkie symbole do pakietu

## 3. Build System

- CMake + scikit-build-core
- `FetchContent` do pobrania Matplot++
- `python_add_library` do zbudowania modułu Python

## 4. Testy

- `test.py` – podstawowa demonstracja generacji i transformacji
- `test2.py` – zaawansowana analiza danych I/Q z pliku WAV