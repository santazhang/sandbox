#include <stdio.h>

#define P(arr, a, b) (arr[a-1][b-1])

int main() {
    double P[9][36];
    double C[6][36];
    for (int i = 1; i <= 36; i++) {
        for (int j = 1; j <= 9; j++) {
            P(P, j, i) = 0;
        }
        for (int j = 1; j <= 6; j++) {
            P(C, j, i) = 0;
        }
    }
    for (int i = 1; i <= 4; i++) {
        P(P, 1, i) = 0.25;
    }
    for (int i = 1; i <= 6; i++) {
        P(C, 1, i) = 1.0/6.0;
    }
    for (int j = 2; j <= 9; j++) {
        for (int i = 1; i <= 36; i++) {
            double p = 0;
            for (int k = 1; k <= 4; k++) {
                if (i - k >= 1) {
                    p += P(P, j - 1, i - k) * P(P, 1, k);
                }
            }
            P(P, j, i) = p;
        }
    }
    for (int j = 2; j <= 6; j++) {
        for (int i = 1; i <= 36; i++) {
            double p = 0;
            for (int k = 1; k <= 6; k++) {
                if (i - k >= 1) {
                    p += P(C, j - 1, i - k) * P(C, 1, k);
                }
            }
            P(C, j, i) = p;
        }
    }

    printf("P:\n");
    for (int j = 1; j <= 9; j++) {
        for (int i = 1; i <= 36; i++) {
            printf(" %.6lf", P(P, j, i));
        }
        printf("\n");
    }
    printf("\n");
    
    printf("C:\n");
    for (int j = 1; j <= 6; j++) {
        for (int i = 1; i <= 36; i++) {
            printf(" %.6lf", P(C, j, i));
        }
        printf("\n");
    }
    printf("\n");

    double p = 0;
    for (int i = 1; i <= 36; i++) {
        for (int j = 1; j < i; j++) {
            p += P(P, 9, i) * P(C, 6, j);
        }
    }
    printf("result=%.7lf\n", p);

    return 0;
}
