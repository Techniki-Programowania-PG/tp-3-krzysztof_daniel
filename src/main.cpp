#include <pybind11/pybind11.h>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

float analyze_signal(int i, int j) {
    return i*j;
}

namespace py = pybind11;

PYBIND11_MODULE(_core, m) {
    m.doc() = R"pbdoc(
        Pybind11 example plugin
        -----------------------

        .. currentmodule:: signal_analyzer

        .. autosummary::
           :toctree: _generate
    )pbdoc";

    m.def("analyze_signal", &analyze_signal, R"pbdoc(
        Multiplies two numbers
    )pbdoc");


#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
