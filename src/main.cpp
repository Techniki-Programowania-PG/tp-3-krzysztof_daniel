#include <pybind11/pybind11.h>
#include <matplot/matplot.h>

namespace py = pybind11;
using namespace matplot;

void pokaz_wykres() {
    std::vector<double> x = {1, 2, 3, 4, 5};
    std::vector<double> y = {1, 4, 9, 16, 25};

    plot(x, y);
    title("Kwadraty");
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

    m.def("pokaz_wykres", &pokaz_wykres, "Rysuje prosty wykres");

m.attr("__version__") = "dev";
// #ifdef VERSION_INFO
//     m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
// #else
//     m.attr("__version__") = "dev";
// #endif
}
