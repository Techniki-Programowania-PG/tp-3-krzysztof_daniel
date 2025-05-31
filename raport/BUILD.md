# Instalacja i budowanie

## Menu

[Powrót](./README.md)

## 1. Klonowanie repozytorium

```bash
git clone <URL_REPO>
cd <repo>
```
## 2. Wymagane zależności
Python ≥ 3.7
CMake ≥ 3.15
Kompilator C++17
pip (zalecane)

## 3. Instalacja pybind11 i scikit-build-core
pip install pybind11 scikit-build-core

## 4. Budowanie za pomocą CMake
mkdir build
cd build
cmake ..
cmake --build . --config Release
Po zakończeniu w folderze build pojawi się moduł _core (shared object / dll). Aby zainstalować pakiet do Pythona:

cmake --install . --prefix "${VIRTUAL_ENV:-/usr/local}"
Lub ręcznie skopiuj katalog signal_analyzer wraz z _core.* do Twojego site-packages.

## 5. Problemy typowe
Brak PythonLibs: sprawdź ścieżki do instalacji Python-dev
Nieudane FetchContent dla Matplot++: sprawdź połączenie sieciowe lub wskaż GIT_TAG