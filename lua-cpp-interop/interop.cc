#include <stdio.h>
#include <sys/time.h>

#include "lua.hpp"

using namespace std;

int my_sum(lua_State* L) {
    int argc = lua_gettop(L);

    lua_Number sum = 0;
    for (int i = 1; i <= argc; i++) {
        sum += lua_tonumber(L, i);
    }

    // return value
    lua_pushnumber(L, sum);

    // number of return value
    return 1;
}

static void report_errors(lua_State* L, int status) {
    if (status != 0) {
        printf("-- %s\n", lua_tostring(L, -1));
        lua_pop(L, 1); // remove error msg
    }
}

void raw_cpp_call() {
    int arr[5] = {1, 2, 3, 4, 5};
    int sum = 0;
    for (int i = 0; i < 5; i++) {
        sum += arr[i];
    }
}

int main() {
    lua_State* L = luaL_newstate();

    lua_register(L, "my_sum", my_sum);
    int s;
    const int n = 100000;
    struct timeval start, stop;
    gettimeofday(&start, NULL);
    for (int i = 0; i < n; i++) {
        s = luaL_dostring(L, "my_sum(1, 2, 3, 4, 5)");
        report_errors(L, s);
    }
    gettimeofday(&stop, NULL);
    double sec = stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec) / 1000000.0;
    printf("approach 1: call per sec=%lf\n", n / sec);
    double base = sec;

    gettimeofday(&start, NULL);
    for (int i = 0; i < n; i++) {
        lua_getglobal(L, "my_sum");
        for (int j = i; j < i + 5; j++) {
            lua_pushnumber(L, j);
        }

        // call with 5 arguments, 1 return value
        lua_call(L, 5, 1);

        // get return value
        int r = (int) lua_tointeger(L, -1);
        lua_pop(L, 1);

        report_errors(L, s);
    }
    gettimeofday(&stop, NULL);
    sec = stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec) / 1000000.0;
    printf("approach 2: call per sec=%lf (%.2lf x)\n", n / sec, base / sec);

    gettimeofday(&start, NULL);
    for (int i = 0; i < n; i++) {
        raw_cpp_call();
    }
    gettimeofday(&stop, NULL);
    double sec2 = stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec) / 1000000.0;
    printf("raw cpp: call per sec=%lf (%.2lf x) (%.2lf x)\n", n / sec, base / sec2, sec / sec2);

    lua_close(L);
    return 0;
}


