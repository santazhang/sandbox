#include <type_traits>

#include <stdio.h>

using namespace std;

void f(const int** pa, const char*** pb) {
    static const int a[] = {1, 2, 3};
    static const char* b[] = {"hello", "world"};
    printf("%d\n", a[0]);
    printf("%p\n", &a);
    printf("%s\n", b[0]);
    *pa = (const int *) &a;
    *pb = (const char **) &b;
}

int main() {
    const int* my_a;
    const char** my_b;
    f(&my_a, &my_b);
    printf("%d %d %d %s %s\n", my_a[0], my_a[1], my_a[2], my_b[0], my_b[1]);
    return 0;
}
