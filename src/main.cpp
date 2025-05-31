#include <pybind11/pybind11.h>
#include <matplot/matplot.h>
#include <pybind11/stl.h>
#include <pybind11/complex.h>
#include <vector>
#include <cmath>
#include <complex>
#include <algorithm>
#include <string>

namespace py = pybind11;
using namespace matplot;

const double PI = acos(-1);

enum class SignalType {
    Sin,
    Cos,
    Square,
    Sawtooth
};

void showSimpleSignal(std::vector<double> x, std::vector<double> y) {
    plot(x, y);
    title("Signal");
    xlabel("X");
    ylabel("Y");
    show();
}

void showComplexSignal(std::vector<std::complex<double>> signal, double sampling_rate, std::string plotTitle, std::string xLabel, std::string yLabel){
    int N = signal.size();
    std::vector<double> amplitudes(N);
    std::vector<double> frequencies(N);
    int half_N = N / 2;
    for (int k = 0; k < N; ++k) {
        amplitudes[k] = std::abs(signal[k]);
    }
    // Przesunięcie DFT: przesuń drugą połowę na początek
    std::rotate(amplitudes.begin(), amplitudes.begin() + half_N, amplitudes.end());
    for (int k = 0; k < N; ++k) {
        // Oś częstotliwości: od -Fs/2 do Fs/2
        frequencies[k] = ((double)k - half_N) * sampling_rate / N;
    }
    // Wykres
    stem(frequencies, amplitudes);
    xlabel(xLabel);
    ylabel(yLabel);
    title(plotTitle);
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
		showComplexSignal(X, sampling_rate, "Wynik DFT (Oś -Fs/2 do Fs/2)", "Częstotliwość [Hz]", "Amplituda");
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
		showComplexSignal(a, sampling_rate, "Wynik FFT (Oś -Fs/2 do Fs/2)", "Częstotliwość [Hz]", "Amplituda");
	}
    
    return a;
}

// Filtracja 1D: prosta konwolucja sygnału z jądrem
std::vector<double> filter1D(const std::vector<double>& signal,
const std::vector<double>& kernel,
bool circular = false) {
    int N = (int)signal.size();
    int M = (int)kernel.size();
    int half = M / 2;
    std::vector<double> out(N, 0.0);

    for (int i = 0; i < N; ++i) {
        double sum = 0.0;
        for (int k = 0; k < M; ++k) {
            int idx = i + (k - half);
            if (circular) {
                // modulo
                idx = (idx % N + N) % N;
            }
            if (idx >= 0 && idx < N) {
                sum += signal[idx] * kernel[k];
            }
        }
        out[i] = sum;
    }
    return out;
}

// Filtracja 2D: konwolucja macierzy z jądrem
std::vector<std::vector<double>> filter2D(
const std::vector<std::vector<double>>& image,
const std::vector<std::vector<double>>& kernel,
bool circular = false)
{
    int H = (int)image.size();
    int W = H ? (int)image[0].size() : 0;
    int Mh = (int)kernel.size();
    int Mw = Mh ? (int)kernel[0].size() : 0;
    int h2 = Mh/2, w2 = Mw/2;

    // wynikowa macierz tej samej wielkości
    std::vector<std::vector<double>> out(H, std::vector<double>(W, 0.0));

    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            double sum = 0.0;
            for (int u = 0; u < Mh; ++u) {
                for (int v = 0; v < Mw; ++v) {
                    int ii = i + (u - h2);
                    int jj = j + (v - w2);
                    if (circular) {
                        ii = (ii % H + H) % H;
                        jj = (jj % W + W) % W;
                    }
                    if (ii >= 0 && ii < H && jj >= 0 && jj < W) {
                        sum += image[ii][jj] * kernel[u][v];
                    }
                }
            }
            out[i][j] = sum;
        }
    }
    return out;
}

std::vector<double> generateGaussianKernel1D(int size, double sigma) {
    std::vector<double> kernel(size);
    int half = size / 2;
    double sum = 0.0;

    for (int i = 0; i < size; ++i) {
        int x = i - half;
        kernel[i] = std::exp(-(x * x) / (2 * sigma * sigma));
        sum += kernel[i];
    }

    // Normalizacja
    for (double& val : kernel)
        val /= sum;

    return kernel;
}

std::vector<std::vector<double>> generateGaussianKernel2D(int size, double sigma) {
    std::vector<std::vector<double>> kernel(size, std::vector<double>(size));
    int half = size / 2;
    double sum = 0.0;

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            int x = i - half;
            int y = j - half;
            kernel[i][j] = std::exp(-(x*x + y*y) / (2 * sigma * sigma));
            sum += kernel[i][j];
        }
    }

    // Normalizacja
    for (auto& row : kernel)
        for (double& val : row)
            val /= sum;

    return kernel;
}


// Wizualizacja macierzy 2D
void showImage(const std::vector<std::vector<double>>& image,
std::string titleStr = "Image",
std::string xLabel = "X",
std::string yLabel = "Y") {
    // Zamień na wektor podwójny do matplot++
    int H = (int)image.size();
    int W = H ? (int)image[0].size() : 0;
    imagesc(image);
    title(titleStr);
    xlabel(xLabel);
    ylabel(yLabel);
    colorbar();
    show();
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

    m.def("showSimpleSignal", &showSimpleSignal, "Rysuje wykres sygnału podanego w postaci x y",
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
        py::arg("show") = false);

    m.def("FFT", &FFT, "Compute FFT of a real-valued signal (power-of-2 length)",
        py::arg("x"),
        py::arg("sampling_rate"),
        py::arg("show") = false);
    
    m.def("showComplexSignal", &showComplexSignal, "Rysuje wykres sygnału podanego w postaci zespolonej",
        py::arg("signal"),
        py::arg("sampling_rate"),
        py::arg("plotTitle") = "Signal",
        py::arg("xLabel") = "X",
        py::arg("yLabel") = "Y");

    m.def("filter1D", &filter1D,
    "1D convolution of signal with kernel",
    py::arg("signal"),
    py::arg("kernel"),
    py::arg("circular") = false);

    m.def("filter2D", &filter2D,
    "2D convolution of image with kernel",
    py::arg("image"),
    py::arg("kernel"),
    py::arg("circular") = false);

    m.def("showImage", &showImage,
    "Show 2D matrix as image",
    py::arg("image"),
    py::arg("title") = "Image",
    py::arg("xLabel") = "X",
    py::arg("yLabel") = "Y");

    m.def("generateGaussianKernel2D", &generateGaussianKernel2D,
        "Generate Gaussian Kernel 2D",
        py::arg("size"),
        py::arg("sigma"));

        
    m.def("generateGaussianKernel1D", &generateGaussianKernel1D,
        "Generate Gaussian Kernel 1D",
        py::arg("size"),
        py::arg("sigma"));

    m.attr("__version__") = "dev";
// #ifdef VERSION_INFO
//     m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
// #else
//     m.attr("__version__") = "dev";
// #endif
}
