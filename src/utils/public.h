#pragma once
#include <cassert>
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
using string_list_t = std::vector<std::string>;
template <typename T>
using unique_list_t = std::vector<std::unique_ptr<T>>;
template <typename T>
using shared_list_t = std::vector<std::shared_ptr<T>>;
template <typename T>
using weak_list_t = std::vector<std::weak_ptr<T>>;

#ifdef __xcdoc_debug__
#define TODO                                                 \
    do {                                                     \
        ERR << __FILE__ << ":" << __LINE__ << " TODO " ENDL; \
        assert(false && "Called TODO");                      \
    } while (1)
#else
#define TODO                            \
    do {                                \
        assert(false && "Called TODO"); \
    } while (1)
#endif

}  // namespace utils