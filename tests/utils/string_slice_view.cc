#include <gtest/gtest.h>
#include <utils/string_slice_view.h>  // 你的头文件

using ssv = string_slice_view;

// 构造 & 基本遍历
TEST(string_slice_view, ConstructAndIterate) {
    const char* txt = "hello world";
    ssv v(txt, txt + 11);
    EXPECT_EQ(v.to_string(), "hello world");

    std::string s;
    for (auto c : v) s += c;
    EXPECT_EQ(s, "hello world");
}

// 从 std::string* 构造
TEST(string_slice_view, FromStdString) {
    std::string str = "abcXYZ";
    ssv v(&str);
    EXPECT_EQ(v.to_string(), "abcXYZ");
}

// push / pop_back
TEST(string_slice_view, PushPop) {
    ssv v;
    v.push("abc", 3);
    v.push("XYZ", 3);
    EXPECT_EQ(v.to_string(), "abcXYZ");

    v.pop_back();
    EXPECT_EQ(v.to_string(), "abcXY");
}

// 区间构造
TEST(string_slice_view, RangeCtor) {
    std::string str = "0123456789";
    ssv whole(&str);
    ssv sub(whole.begin() + 2, whole.begin() + 7);
    EXPECT_EQ(sub.to_string(), "23456");
}

// insert (size_t 版本)
TEST(string_slice_view, InsertByIndex) {
    std::string a = "abc";
    std::string b = "XYZ";
    ssv v(&a);
    ssv other(&b);
    v.insert(1, other);
    EXPECT_EQ(v.to_string(), "aXYZbc");
    ssv v1(&a);
    v1.insert(2, other);
    EXPECT_EQ(v1.to_string(), "abXYZc");
}

// insert (iterator 版本)
TEST(string_slice_view, InsertByIterator) {
    std::string a = "abc";
    std::string b = "XYZ";
    ssv v(&a);
    ssv other(&b);
    v.insert(v.begin() + 1, other.begin(), other.end());
    EXPECT_EQ(v.to_string(), "aXYZbc");
    ssv v1(&a);
    v1.insert(v1.begin() + 2, other.begin(), other.end());
    EXPECT_EQ(v1.to_string(), "abXYZc");
}

// erase
TEST(string_slice_view, Erase) {
    std::string str = "hello world";
    ssv v(&str);
    auto it = v.earse(v.begin() + 6, v.begin() + 11);
    EXPECT_EQ(v.to_string(), "hello ");
    EXPECT_EQ(*it, '\0');  // end() 的哨兵
}

// replace
TEST(string_slice_view, Replace) {
    std::string str = "hello world";
    ssv v(&str);
    std::string newStr = "C++";
    ssv replacement(&newStr);

    v.replace(v.begin() + 6, v.end(), replacement.begin(), replacement.end());
    EXPECT_EQ(v.to_string(), "hello C++");
}

// 拷贝构造 & 赋值
TEST(string_slice_view, Copy) {
    std::string s = "copy";
    ssv v1(&s);
    ssv v2(v1);
    EXPECT_EQ(v2.to_string(), "copy");

    ssv v3;
    v3 = v1;
    EXPECT_EQ(v3.to_string(), "copy");
}

// 与 std::string 互转
TEST(string_slice_view, ConvertToStdString) {
    std::string s = "convert";
    ssv v(&s);
    std::string out = v;  // operator std::string()
    EXPECT_EQ(out, "convert");
}

// 输出流
TEST(string_slice_view, Ostream) {
    std::string s = "ostream";
    ssv v(&s);
    std::ostringstream oss;
    oss << v;
    EXPECT_EQ(oss.str(), "ostream");
}