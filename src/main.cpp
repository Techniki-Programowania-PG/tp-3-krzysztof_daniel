#include <pybind11/pybind11.h>
#include <matplot/matplot.h>
#include <pybind11/stl.h>
#include <vector>
#include <cmath>

namespace py = pybind11;
using namespace matplot;

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

    m.def("showSignal", &showSignal, "Rysuje prosty wykres");

    m.def("generate_signal", &generate_signal,
        py::arg("type"),
        py::arg("freq"),
        py::arg("sampling_rate"),
        py::arg("duration"),
        "Generowanie próbek sygnału (sin, cos, prostokątny, piłokształtny)");

m.attr("__version__") = "dev";
// #ifdef VERSION_INFO
//     m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
// #else
//     m.attr("__version__") = "dev";
// #endif
}
