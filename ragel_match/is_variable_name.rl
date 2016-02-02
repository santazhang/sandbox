#include <stdio.h>
#include <string.h>

%%{
    machine is_variable_name;

    main := [_a-zA-Z][_a-zA-Z0-9]*;

    write data;
}%%

int is_variable_name(const char* str) {
    const char* p = str;
    const char* pe = str + strlen(str);
    int cs;

    %%write init;
    %%write exec;

    if (cs == is_variable_name_first_final) {
        return 1;
    }
    return 0;
}

int main() {
    printf("%d\n", is_variable_name("abc"));
    printf("%d\n", is_variable_name("abc "));
    printf("%d\n", is_variable_name("0abc"));
    return 0;
};
