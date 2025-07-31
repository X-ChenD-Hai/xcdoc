#pragma once
#include <cassert>
#include <iostream>  // IWYU pragma: keep
#include <string>
#ifdef __xcdoc_debug__
#include <magic_enum/magic_enum.hpp>
#endif
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
class string_slice_view;

#ifdef __xcdoc_debug__

template <class T>
    requires std::is_enum_v<T>
std::ostream& operator<<(std::ostream& os, const T& t) {
    return os << magic_enum::enum_name(t);
}

#endif

namespace utils {
using ssv_uptr = std::unique_ptr<string_slice_view>;
using ssv_ptr = string_slice_view*;
std::string read_file(const std::string& filename);
std::string unquote(std::string_view raw);
using string_list_t = std::vector<std::string>;
using string_slice_list_t = std::vector<string_slice_view>;
template <typename T>
using unique_list_t = std::vector<std::unique_ptr<T>>;
template <typename T>
using shared_list_t = std::vector<std::shared_ptr<T>>;
template <typename T>
using weak_list_t = std::vector<std::weak_ptr<T>>;
class TodoType {};
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