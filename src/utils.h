#pragma once
#include <fstream>
#include <iostream>
#include <string>

#ifdef __xcdoc_debug__
#define ERR std::cerr
#define WERR std::wcerr
#define OUT std::cout
#define WOUT std::wcout
#define NV(v) << #v << ": " << (v) << ", "
#define SV(n, v) << #n << ": " << (v) << ", "
#define VV(v) << (v) << " "
#define ENDL << std::endl
#else
#define ERR
#define OUT
#define WOUT
#define WERR
#define NV(v)
#define SV(n, v)
#define VV(v)
#define ENDL
#endif
namespace utiles {
inline std::string read_file(const std::string &filename) {
    std::ifstream ifs(filename);
    if (!ifs) {
        ERR VV("Failed to open file: ") VV(filename) ENDL;
        return "";
    }
    std::string content((std::istreambuf_iterator<char>(ifs)),
                        std::istreambuf_iterator<char>());
    return content;
}
}  // namespace utiles