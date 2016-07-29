#include "common.h"

#include <vector>
#include <algorithm>

using std::vector;


const int64_t N = 1e11;
const int D = 2017;

struct p_x {
    int64_t p;
    int x;
    int64_t p_pow_x;
};


bool summation_overflow(int r, int x) {
    int64_t s = 1;  // s = 1 + r + r^2 + ... + r^x
    int64_t a = r;  // a = r^i
    for (int i = 1; i <= x; i++) {
        s += a;
        if (s > N) {
            return true;
        }
        a *= r;
    }
    return false;
}

inline int64_t mypow(int64_t p, int x) {
    int64_t v = 1;
    for (int i = 1; i <= x; i++) {
        v *= p;
    }
    return v;
}

int main() {
    const int SQRT_N = ::sqrt(N) + 1;
    verify(is_prime(D));
    int MAX_POW = 1 + log(N) / log(2.0);  // 2^MAX_POW > N
    printf("max prime power = %d\n", MAX_POW);
    
    vector<p_x> p_and_x;

    // find r and x, such that 1 <= r < D, 1 <= x <= MAX_POW,
    // and (1 + r + r^2 + ... + r^x) % D == 0

    for (int r = 1; r < D - 1; r++) {
        int64_t s = 1;  // s = 1 + r + r^2 + ... + r^x
        int64_t a = r;  // a = r^i
        for (int x = 1; x <= MAX_POW && s <= N; x++) {
            s += a;
            if (s <= N && s % D == 0) {
                printf("found r=%d, x=%d\n", r, x);
                // for each r, find primes p = D*k + r
                int k = 0;
                for (;;) {
                    int64_t p = D * k + r;
                    if (p > N || summation_overflow(p, x)) {
                        break;
                    }
                    if (is_prime(p)) {
                        // printf("  found p=%lld, x=%d\n", p, x);
                        p_x px;
                        px.p = p;
                        px.x = x;
                        px.p_pow_x = mypow(p, x);
                        p_and_x.push_back(px);
                    }
                    k++;
                }
            }
            a *= r;
        }
    }

    // special case for r = D - 1 (r mod D == -1), advancing x by 2 at a time
    {
        int r = D - 1;
        int64_t s = 0;
        int64_t a = 1 + r;
        for (int x = 1; x <= MAX_POW && s <= N; x += 2) {
            s += a;
            if (s <= N && s % D == 0) {
                printf("found r=%d, x=%d\n", r, x);
                // for each r, find primes p = D*k + r
                int k = 0;
                for (;;) {
                    int64_t p = D * k + r;
                    if (p > N || summation_overflow(p, x)) {
                        break;
                    }
                    if (is_prime(p)) {
                        // printf("  found p=%lld, x=%d\n", p, x);
                        p_x px;
                        px.p = p;
                        px.x = x;
                        px.p_pow_x = mypow(p, x);
                        p_and_x.push_back(px);
                    }
                    k++;
                }
            }
            a *= r * r;
        }
    }

    printf("got %ld p,x pairs\n", static_cast<long>(p_and_x.size()));

    std::sort(p_and_x.begin(), p_and_x.end(), [] (const p_x& a, const p_x& b) {
        return a.p_pow_x < b.p_pow_x;
    });
    
    printf("smallest 3 p^x values: %lld %lld %lld\n", p_and_x[0].p_pow_x, p_and_x[1].p_pow_x, p_and_x[2].p_pow_x);
    verify(p_and_x[0].p_pow_x * p_and_x[1].p_pow_x * p_and_x[2].p_pow_x > N);

    int64_t s_value = 0;
    for (const p_x& px : p_and_x) {
        int64_t n = N / px.p_pow_x;
        int64_t n2 = n / px.p;
        // p^x * (1 + ... + N/p^x), no term in "1 + ... + N/p^x" divisable by p
        s_value += px.p_pow_x * (n * (n + 1) / 2 - px.p * n2 * (n2 + 1) / 2);
    }

    // remove duplicated terms p^x * (... q^y ...) + q^y * (... p^x ...)
    // based on smallest p^x values and N, only need to do 2 for loops
    for (size_t i = 0; i < p_and_x.size(); i++) {
        if (i % 1000 == 0) {
            printf("progress: %d/%d\n", static_cast<int>(i), static_cast<int>(p_and_x.size()));
        }
        for (size_t j = i + 1; j < p_and_x.size(); j++) {
            // continue;
            const p_x& A = p_and_x[i];
            const p_x& B = p_and_x[j];
            if (A.p_pow_x * B.p_pow_x < N) {
                int64_t n = N / A.p_pow_x / B.p_pow_x;
                int64_t n1 = n / A.p_pow_x;
                int64_t n2 = n / B.p_pow_x;
                int64_t n3 = n / A.p_pow_x / B.p_pow_x;
                s_value -= A.p_pow_x * B.p_pow_x * (
                    n * (n + 1) / 2
                    - A.p_pow_x * n1 * (n1 + 1) / 2
                    - B.p_pow_x * n2 * (n2 + 1) / 2
                    + A.p_pow_x * B.p_pow_x * n3 * (n3 + 1) / 2
                );
            } else {
                break;
            }
        }
    }

    // S=2992480851924313898
    printf("S=%lld\n", s_value);

    return 0;
}
