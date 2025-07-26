#include "CppParser/CommandParserImpl.h"

#include "utils/public.h"

// class CommandParser implementation
std::string_view CommandParser::ltrim(std::string_view s) {
    size_t pos = s.find_first_not_of(" \t\r\n");
    return pos == std::string_view::npos ? "" : s.substr(pos);
}
std::vector<std::string_view> CommandParser::__tokenize(std::string_view s) {
    std::vector<std::string_view> __tokens;
    while (!s.empty()) {
        s = ltrim(s);
        if (s.empty()) break;
        if (s.front() == '"') {
            size_t next = s.find('"', 1);
            if (next == std::string_view::npos) next = s.size();
            __tokens.emplace_back(s.substr(0, next + 1));
            s = s.substr(next + 1);
        } else {
            size_t next = s.find_first_of(" \t\r\n");
            __tokens.emplace_back(s.substr(0, next));
            s = next == std::string_view::npos ? "" : s.substr(next);
        }
    }
    return __tokens;
}
std::unique_ptr<CommandParser> CommandParser::parse(std::string_view cmd) {
    auto __tokens = __tokenize(cmd);
    if (__tokens.empty()) return nullptr;
    auto first = std::string(__tokens.front());
    std::string exe(first);
    std::transform(exe.begin(), exe.end(), exe.begin(), ::tolower);

    if (exe.find("clang-cl") != std::string::npos)
        return std::make_unique<ClangClCommandParser>(cmd, __tokens);

    if (exe.find("clang++") != std::string::npos ||
        (exe.find("clang") != std::string::npos &&
         exe.find("cl") == std::string::npos))
        return std::make_unique<ClangCommandParser>(cmd, __tokens);

    if (exe.find("g++") != std::string::npos ||
        exe.find("gcc") != std::string::npos)
        return std::make_unique<GccCommandParser>(cmd, __tokens);

    if (exe.find("cl.exe") != std::string::npos ||
        exe.find("cl") != std::string::npos)
        return std::make_unique<MsvcCommandParser>(cmd, __tokens);

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

// class GccCommandParser implementation
std::vector<std::pair<std::string, std::string>> GccCommandParser::macros()
    const {
    return extract_prefixed("-D");
}
std::vector<std::pair<std::string, std::string>> GccCommandParser::options()
    const {
    std::vector<std::pair<std::string, std::string>> out;
    for (auto tok : tokens()) {
        if (tok.starts_with('-') && tok.size() > 1 && !tok.starts_with("-D") &&
            !tok.starts_with("-I") && !tok.starts_with("-o")) {
            size_t eq = tok.find('=');
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
std::vector<std::string> GccCommandParser::include_paths() const {
    std::vector<std::string> out;
    for (auto tok : tokens()) {
        if (tok.starts_with("-I"))
            out.emplace_back(tok.substr(2));
        else if (tok.starts_with("-isystem"))
            out.emplace_back(tok.substr(8));
    }
    return out;
}
std::vector<std::string> GccCommandParser::input_files() const {
    std::vector<std::string> out;
    for (auto tok : tokens()) {
        if (tok.ends_with(".c") || tok.ends_with(".cc") ||
            tok.ends_with(".cpp") || tok.ends_with(".cxx") ||
            tok.ends_with(".c++"))
            out.emplace_back(tok);
    }
    return out;
}
std::vector<std::string> GccCommandParser::output_files() const {
    std::vector<std::string> out;
    for (size_t i = 0; i < tokens().size(); ++i) {
        if (tokens()[i] == "-o" && i + 1 < tokens().size())
            out.emplace_back(tokens()[i + 1]);
    }
    return out;
}
std::vector<std::pair<std::string, std::string>>
GccCommandParser::extract_prefixed(std::string_view flag) const {
    std::vector<std::pair<std::string, std::string>> out;
    for (auto tok : tokens()) {
        if (tok.starts_with(flag)) {
            auto body = tok.substr(flag.size());
            size_t eq = body.find('=');
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
std::vector<std::pair<std::string, std::string>> MsvcCommandParser::macros()
    const {
    return extract_prefixed("/D");
}
std::vector<std::pair<std::string, std::string>> MsvcCommandParser::options()
    const {
    std::vector<std::pair<std::string, std::string>> out;
    for (auto tok : tokens()) {
        if (tok.starts_with('/') && !tok.starts_with("/D") &&
            !tok.starts_with("/I") && !tok.starts_with("/Fo") &&
            !tok.starts_with("/Fd") && !tok.starts_with("/Fe")) {
            size_t sep = tok.find(':', 1);
            if (sep == std::string_view::npos) {
                out.emplace_back(std::string(tok.substr(1)), "");
            } else {
                out.emplace_back(std::string(tok.substr(1, sep - 1)),
                                 utils::unquote(tok.substr(sep + 1)));
            }
        }
    }
    return out;
}
std::vector<std::string> MsvcCommandParser::include_paths() const {
    std::vector<std::string> out;
    for (auto tok : tokens()) {
        if (tok.starts_with("/I"))
            out.emplace_back(utils::unquote(tok.substr(2)));
    }
    return out;
}
std::vector<std::string> MsvcCommandParser::input_files() const {
    std::vector<std::string> out;
    for (auto tok : tokens()) {
        if (tok.ends_with(".c") || tok.ends_with(".cc") ||
            tok.ends_with(".cpp") || tok.ends_with(".cxx") ||
            tok.ends_with(".c++"))
            out.emplace_back(tok);
    }
    return out;
}
std::vector<std::string> MsvcCommandParser::output_files() const {
    std::vector<std::string> out;
    for (auto tok : tokens()) {
        if (tok.starts_with("/Fo") || tok.starts_with("/Fd") ||
            tok.starts_with("/Fe"))
            out.emplace_back(tok.substr(3));
    }
    return out;
}
std::vector<std::pair<std::string, std::string>>
MsvcCommandParser::extract_prefixed(std::string_view flag) const {
    std::vector<std::pair<std::string, std::string>> out;
    for (auto tok : tokens()) {
        if (tok.starts_with(flag)) {
            auto body = tok.substr(flag.size());
            size_t sep = body.find('=');
            if (sep == std::string_view::npos) {
                out.emplace_back(std::string(body), "");
            } else {
                out.emplace_back(std::string(body.substr(0, sep)),
                                 utils::unquote(body.substr(sep + 1)));
            }
        }
    }
    return out;
}