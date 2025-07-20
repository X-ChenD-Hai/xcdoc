#define AA int aq = 1
AA;
#define 中行(a, b) a b;
中行(class A, { A(int a, int b){auto s = (a,b);
}
});
#define 中行1(a, abc) class a##abc;
中行1(A1, q);
#define 中行2(a, bc) class a##c##bc;
中行2(A2, q22);
#define 中行3(a, b) auto a = #b "123";
中行3(A2, zq22 dsfds dsaf);
#define A()
A()
class Aaaa;
class A中国;