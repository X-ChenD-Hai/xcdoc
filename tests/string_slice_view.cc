#include "string_slice_view.h"

#include <string>

#include "utils.h"

using namespace std;

int main() {
    string a{"123"};

    string_slice_view ss;
    string_slice_view ss1;
    ss.push(&a);
    ss.push(&a);
    ss1.push(ss.begin() + 1, ss.begin() + 5);

    a = ss;
    OUT NV(ss) ENDL;
    OUT NV(ss1) ENDL;

    return 0;
}