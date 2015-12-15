#!/usr/bin/env python

from __future__ import print_function

cache = {}

def P(N, S):
    if N < 0:
        return 0
    elif N == 0:
        return 1
    elif len(S) == 1 and S[0] == 1:
        return 1
    last_S = S[-1]
    k = (N, last_S)
    global cache
    if k in cache:
        return cache[k]
    new_S = S[:-1]
    val = 0
    for i in range(N / last_S + 1):
        val += P(N - i * last_S, new_S)
    cache[k] = val
    return val

print(P(1, [1]))
print(P(1, [1, 2]))
print(P(3, [1, 2]))
print(P(4, [1, 2]))
print(P(200, [1, 2, 5, 10, 20, 50, 100, 200]))
