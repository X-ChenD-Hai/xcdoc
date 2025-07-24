#include "CppParser/CommandParserImpl.h"

#include "utils/public.h"

// class CommandParser implementation
std::string_view CommandParser::ltrim(std::string_view s) const {
    size_t pos = s.find_first_not_of(" \t\r\n");
    return pos == std::string_view::npos ? "" : s.substr(pos);
}
std::vector<std::string_view> CommandParser::tokens() const {
    std::vector<std::string_view> res;
    std::string_view s = command();
    while (!s.empty()) {
        s = ltrim(s);
        if (s.empty()) break;
        if (s.front() == '"') {
            size_t next = s.find('"', 1);
            if (next == std::string_view::npos) next = s.size();
            res.emplace_back(s.substr(0, next + 1));
            s = s.substr(next + 1);
        } else {
            size_t next = s.find_first_of(" \t\r\n");
            res.emplace_back(s.substr(0, next));
            s = next == std::string_view::npos ? "" : s.substr(next);
        }
    }
    return res;
}
std::unique_ptr<CommandParser> CommandParser::parse(std::string_view command) {
    if (command.find("clang-cl") != std::string_view::npos)
        return std::make_unique<ClangClCommandParser>(command);
    return nullptr;
}

// class ClangClCommandParser implementation
std::vector<std::pair<std::string, std::string>> ClangClCommandParser::macros()
    const {
    return extract_prefixed("-D");
}
std::vector<std::pair<std::string, std::string>> ClangClCommandParser::options()
    const {
    // 这里只把 /NAME[...] 或 -clang:-NAME[...] 视为 option
    std::vector<std::pair<std::string, std::string>> out;
    for (auto tok : tokens()) {
        if (tok.starts_with("/") && !tok.starts_with("/Fo") &&
            !tok.starts_with("/Fd")) {
            size_t eq = tok.find_first_of(":=", 1);
            if (eq == std::string_view::npos) {
                out.emplace_back(std::string(tok.substr(1)), "");
            } else {
                out.emplace_back(std::string(tok.substr(1, eq - 1)),
                                 std::string(tok.substr(eq + 1)));
            }
        } else if (tok.starts_with("-clang:")) {
            auto inner = tok.substr(7);
            size_t eq = inner.find_first_of("=:");
            if (eq == std::string_view::npos) {
                out.emplace_back(std::string(inner), "");
            } else {
                out.emplace_back(std::string(inner.substr(0, eq)),
                                 std::string(inner.substr(eq + 1)));
            }
        } else if (tok.starts_with("-") && tok.size() > 1 &&
                   !tok.starts_with("-D") && !tok.starts_with("-I") &&
                   !tok.starts_with("-clang:") && !tok.starts_with("-imsvc")) {
            // 形如 -MDd -std=c++23 ...
            size_t eq = tok.find_first_of("=:", 1);
            if (eq == std::string_view::npos) {
                out.emplace_back(std::string(tok.substr(1)), "");
            } else {
                out.emplace_back(std::string(tok.substr(1, eq - 1)),
                                 std::string(tok.substr(eq + 1)));
            }
        }
    }
    return out;
}
std::vector<std::string> ClangClCommandParser::include_paths() const {
    std::vector<std::string> out;
    for (auto tok : tokens()) {
        if (tok.starts_with("-I")) {
            out.emplace_back(tok.substr(2));
        } else if (tok.starts_with("/I")) {
            out.emplace_back(tok.substr(2));
        } else if (tok.starts_with("-imsvc")) {
            out.emplace_back(tok.substr(6));
        }
    }
    return out;
}
std::vector<std::string> ClangClCommandParser::input_files() const {
    std::vector<std::string> out;
    for (auto tok : tokens()) {
        if (tok.ends_with(".c") || tok.ends_with(".cc") ||
            tok.ends_with(".cpp") || tok.ends_with(".cxx") ||
            tok.ends_with(".c++")) {
            out.emplace_back(tok);
        }
    }
    return out;
}
std::vector<std::string> ClangClCommandParser::output_files() const {
    std::vector<std::string> out;
    for (auto tok : tokens()) {
        if (tok.starts_with("/Fo") || tok.starts_with("/Fd")) {
            out.emplace_back(tok.substr(3));
        }
    }
    return out;
}

std::vector<std::pair<std::string, std::string>>
ClangClCommandParser::extract_prefixed(std::string_view flag) const {
    std::vector<std::pair<std::string, std::string>> out;
    for (auto tok : tokens()) {
        if (tok.starts_with(flag)) {
            auto body = tok.substr(flag.size());
            size_t eq = body.find_first_of("=:");
            if (eq == std::string_view::npos) {
                out.emplace_back(std::string(body), "");
            } else {
                out.emplace_back(std::string(body.substr(0, eq)),
                                 utils::unquote(body.substr(eq + 1)));
            }
        }
    }
    return out;
}
