#ifndef FILEDIALOG_HPP
#define FILEDIALOG_HPP

#include <string>

std::string openFileDialogWindows();
std::string saveFileDialogWindows(const std::string& filter = "", const std::string& defExt = "");

#endif