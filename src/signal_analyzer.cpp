#include <pybind11/pybind11.h>

int add(int a, int b) {
    return a + b;
}

PYBIND11_MODULE(sygnal_analyzer, m) {
    m.def("add", &add, "Dodaje dwie liczby");
}
