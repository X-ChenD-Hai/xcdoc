#include <gtest/gtest.h>
#include <utils/public.h>
#include <utils/string_slice_view.h>

#include <string>

using namespace std;

TEST(string_slice_view, basic) {
    string a{"123"};

    string_slice_view ss;
    string_slice_view ss1;
    ss.push(&a);
    ss.push(&a);
    ss1.push(ss.begin() + 1, ss.begin() + 5);

    a = ss;
    OUT NV(ss) ENDL;
    OUT NV(ss1) ENDL;
}