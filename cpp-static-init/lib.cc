#include <stdio.h>

void dummy() { }

class BLAH {
public:
    BLAH() {
        printf("BLAH init done!\n");
    }
};

BLAH blah;
