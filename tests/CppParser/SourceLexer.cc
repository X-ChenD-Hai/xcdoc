#include <CppParser/PreCompiledLexer.h>
#include <CppParser/SourceLexer.h>
#include <gmock/gmock.h>
#include <utils/public.h>
#define __RS(p) XCDOC_TESTS_RESOURCES_DIR "SourceLexer/" p
namespace testing {
using SourceLexer_ = ::SourceLexer;
class SourceLexer : public testing::Test {
   public:
    std::unique_ptr<PreCompiledLexer> pre_compiled_lexer;
    std::unique_ptr<SourceLexer_> source_lexer;

    void SetUp() override {
        auto source = utils::read_file(__RS("test.cc"));
        pre_compiled_lexer = std::make_unique<PreCompiledLexer>(&source);
        source_lexer = std::make_unique<SourceLexer_>(pre_compiled_lexer.get());
    }
};
TEST_F(SourceLexer, Test) {}
}  // namespace testing
