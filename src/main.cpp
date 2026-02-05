#include "../include/Windows.hpp"
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif

int main(int, char**) {
    #ifdef _WIN32
    char exePath[MAX_PATH]; GetModuleFileNameA(NULL, exePath, MAX_PATH);
    std::string dir = std::string(exePath);
    dir = dir.substr(0, dir.find_last_of("\\/"));
    SetEnvironmentVariableA("PATH", (dir + "\\lib;" + dir).c_str());
    #endif

    Windows app(1280, 800);
    if (app.initialize()) app.run();
    return 0;
}