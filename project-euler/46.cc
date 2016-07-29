#include "common.h"

int main() {
    int x = 9;
    for (;;) {
        if (is_prime(x)) {
            x += 2;
            continue;
        }
        int y = 1;
        bool ok = false;
        for (;;) {
            int z = x - 2 * y * y;
            if (z <= 2) {
                break;
            }
            if (is_prime(z)) {
                ok = true;
                break;
            }
            y++;
        }
        if (!ok) {
            printf("%d -> OOPS\n", x);
            break;
        } else {
            printf("%d = %d + 2 * %d * %d\n", x, x - 2 * y * y, y, y);
        }
        x += 2;
    }
    return 0;
}
