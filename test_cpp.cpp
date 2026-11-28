#include "inifile.h"
#include <iostream>
int main(int argc, char **argv) {
    IniFile ini(argc > 1 ? argv[1] : "testfile.ini");
    std::string value;
    if (!ini.ReadIniStringFromFile("SIZE", &value)) return 1;
    std::cout << "SIZE: " << value << '\n';
    if (!ini.WriteIniStringToFile("test", "Updated from C++")) return 1;
    if (!ini.ReadIniStringFromFile("test", &value) || value != "Updated from C++") return 1;
    std::cout << "C++ read/write test passed\n";
    return 0;
}
