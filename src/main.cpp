#include <pybind11/pybind11.h>
#include <matplot/matplot.h>
#include <pybind11/stl.h>
#include <vector>

namespace py = pybind11;
using namespace matplot;

void showSignal(std::vector<double> x, std::vector<double> y) {

    plot(x, y);
    title("Signal");
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

    m.def("showSignal", &showSignal, "Rysuje prosty wykres");

m.attr("__version__") = "dev";
// #ifdef VERSION_INFO
//     m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
// #else
//     m.attr("__version__") = "dev";
// #endif
}
