#include "./public.h"

#include <fstream>

std::string utils::read_file(const std::string &filename) {
    std::ifstream ifs(filename);
    if (!ifs) {
        ERR VV("Failed to open file: ") VV(filename) ENDL;
        return "";
    }
    std::string content((std::istreambuf_iterator<char>(ifs)),
                        std::istreambuf_iterator<char>());
    return content;
}
std::string utils::unquote(std::string_view raw) {
    if (raw.size() >= 2 && raw.front() == '"' && raw.back() == '"') {
        raw = raw.substr(1, raw.size() - 2);
    }
    std::string out;
    out.reserve(raw.size());
    for (size_t i = 0; i < raw.size(); ++i) {
        if (raw[i] == '\\' && i + 1 < raw.size()) {
            switch (raw[i + 1]) {
                case '"':
                    out += '"';
                    ++i;
                    break;
                case '\\':
                    out += '\\';
                    ++i;
                    break;
                default:
                    out += raw[i];
            }
        } else {
            out += raw[i];
        }
    }
    return out;
}
