#pragma once
#include <iostream>  // IWYU pragma: keep
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
namespace utils {
std::string read_file(const std::string &filename);
std::string unquote(std::string_view raw);
}  // namespace utils