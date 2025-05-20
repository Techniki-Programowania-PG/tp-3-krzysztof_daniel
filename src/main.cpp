#include <pybind11/pybind11.h>
#include <matplot/matplot.h>
#include <pybind11/stl.h>
#include <pybind11/complex.h>
#include <vector>
#include <cmath>
#include <complex>
#include <algorithm>

namespace py = pybind11;
using namespace matplot;

const double PI = acos(-1);

enum class SignalType {
    Sin,
    Cos,
    Square,
    Sawtooth
};

void showSignal(std::vector<double> x, std::vector<double> y) {

    plot(x, y);
    title("Signal");
    show();
}
    
// Funkcja generująca sygnał wybranego typu (Sin, Cos, Square, Sawtooth)
std::vector<double> generate_signal(SignalType type, double freq, double sampling_rate, double duration) {
    // Obliczenie liczby próbek na podstawie częstotliwości próbkowania i czasu trwania
    size_t n_samples = static_cast<size_t>(sampling_rate * duration);

    // Rezerwacja miejsca na próbki
    std::vector<double> signal;
    signal.reserve(n_samples);

    // Generowanie kolejnych próbek
    for(size_t n = 0; n < n_samples; n++) {
        double t = static_cast<double>(n) / sampling_rate; // Czas próbki
        double value = 0.0;
        switch(type) {
            case SignalType::Sin:
                value = sin(2 * M_PI * freq * t);
                break;
            case SignalType::Cos:
                value = cos(2 * M_PI * freq * t);
                break;
            case SignalType::Square:
                value = (sin(2 * M_PI * freq * t) >= 0) ? 1.0 : -1.0;
                break;
            case SignalType::Sawtooth:
                value = 2.0 * (t * freq - floor(0.5 + t * freq));
                break;
        }
        // Dodanie próbki do sygnału
        signal.push_back(value);
    }
    return signal;
}

// Funkcja wykonująca DFT
std::vector<std::complex<double>> DFT(const std::vector<double>& x, double sampling_rate, bool displayPlot = false) {
	int N = x.size();
	std::vector<std::complex<double>> X(N);
	for (int k = 0; k < N; ++k) {
		std::complex<double> sum(0.0, 0.0);
		for (int n = 0; n < N; ++n) {
			double angle = -2 * PI * k * n / N;
			std::complex<double> expTerm(cos(angle), sin(angle));
			sum += x[n] * expTerm;
		}
		X[k] = sum;
	}
	if (displayPlot) {
		std::vector<double> amplitudes(N);
		std::vector<double> frequencies(N);
		int half_N = N / 2;
		for (int k = 0; k < N; ++k) {
			amplitudes[k] = std::abs(X[k]);
		}
		// Przesunięcie DFT: przesuń drugą połowę na początek
		std::rotate(amplitudes.begin(), amplitudes.begin() + half_N, amplitudes.end());
		for (int k = 0; k < N; ++k) {
			// Oś częstotliwości: od -Fs/2 do Fs/2
			frequencies[k] = ((double)k - half_N) * sampling_rate / N;
		}
		// Wykres
		stem(frequencies, amplitudes);
		xlabel("Częstotliwość [Hz]");
		ylabel("Amplituda");
		title("Wynik DFT (Oś -Fs/2 do Fs/2)2)");
		show();
	}


	return X;
}

// Funkcja wykonująca FFT (zoptymalizowane DFT)
std::vector<std::complex<double>> FFT(const std::vector<double>& x_real,
    double sampling_rate,
    bool displayPlot = false) {
    
    // oryginalna długość
    size_t N0 = x_real.size();
    
    //najbliższa większa potęga dwójki
    size_t N = 1;
    while (N < N0) N <<= 1;
    
    // wektor zespolony z dopełnieniem zerami
    std::vector<std::complex<double>> x(N);
    for (size_t i = 0; i < N0; ++i)
        x[i] = x_real[i];
    for (size_t i = N0; i < N; ++i)
        x[i] = 0;
    
    // N jest potęgą dwójki, wykonaj FFT na x[0..N-1]
    int levels = 0;
    while ((1u << levels) < N) ++levels;
    
    // tablica indeksów w odwrotnej kolejności bitowej
    std::vector<int> rev(N);
    rev[0] = 0;
    for (int i = 1; i < (int)N; ++i) {
        rev[i] = (rev[i >> 1] >> 1) | ((i & 1) << (levels - 1));
    }
    
    // dane do nowej kolejności
    std::vector<std::complex<double>> a(N);
    for (int i = 0; i < (int)N; ++i) {
        a[i] = x[rev[i]];
    }
    
    // algorytm Cooleya–Tukeya (operacje w miejscu)
    for (int size = 2; size <= (int)N; size <<= 1) {
        double angle = -2 * M_PI / size;
        std::complex<double> wlen(std::cos(angle), std::sin(angle));
        for (int start = 0; start < (int)N; start += size) {
            std::complex<double> w(1.0, 0.0);
            for (int j = 0; j < size/2; ++j) {
                auto u = a[start + j];
                auto v = a[start + j + size/2] * w;
                a[start + j] = u + v;
                a[start + j + size/2] = u - v;
                w *= wlen;
            }
        }
    }
    
    if (displayPlot) {
        std::vector<double> amplitudes(N), frequencies(N);
        int half_N = N / 2;
        for (int k = 0; k < N; ++k) {
            amplitudes[k] = std::abs(a[k]);
        }
        // przesuń częstotliwość zerową do środka wykresu
        std::rotate(amplitudes.begin(), amplitudes.begin() + half_N, amplitudes.end());
        for (int k = 0; k < N; ++k) {
            frequencies[k] = (k - half_N) * sampling_rate / N;
        }
        stem(frequencies, amplitudes);
        xlabel("Częstotliwość [Hz]");
        ylabel("Amplituda");
        title("Wynik FFT (Oś -Fs/2 do Fs/2)");
        show();
    }
    
    return a;
}

    

PYBIND11_MODULE(_core, m) {
    m.doc() = R"pbdoc(
        Pybind11 example plugin
        -----------------------

        .. currentmodule:: signal_analyzer

        .. autosummary::
           :toctree: _generate
    )pbdoc";

    py::enum_<SignalType>(m, "SignalType")
        .value("Sin", SignalType::Sin)
        .value("Cos", SignalType::Cos)
        .value("Square", SignalType::Square)
        .value("Sawtooth", SignalType::Sawtooth)
        .export_values();

    m.def("showSignal", &showSignal, "Rysuje wykres",
        py::arg("x"),
        py::arg("y"));

    m.def("generate_signal", &generate_signal,
        py::arg("type"),
        py::arg("freq"),
        py::arg("sampling_rate"),
        py::arg("duration"),
        "Generowanie próbek sygnału (sin, cos, prostokątny, piłokształtny)");

    m.def("DFT", &DFT, "Wylicza DFT podanego sygnału",
        py::arg("x"),
        py::arg("sampling_rate"),
        py::arg("show"));

    m.def("FFT", &FFT, "Compute FFT of a real-valued signal (power-of-2 length)",
        py::arg("x"),
        py::arg("sampling_rate"),
        py::arg("show") = false);

m.attr("__version__") = "dev";
// #ifdef VERSION_INFO
//     m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
// #else
//     m.attr("__version__") = "dev";
// #endif
}
