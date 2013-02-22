#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>
#include <assert.h>

#include "lua.hpp"

using namespace std;

int my_sum(lua_State* L) {
    int argc = lua_gettop(L);

    lua_Number sum = 0;
    for (int i = 1; i <= argc; i++) {
        sum += lua_tonumber(L, i);
    }
    assert(sum == 1 + 2 + 3 + 4 + 5);

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

void* lua_interop_mt(void* ignored) {
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
    double sec1 = stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec) / 1000000.0;
    double qps1 = n / sec1;

    gettimeofday(&start, NULL);
    for (int i = 0; i < n; i++) {
        lua_getglobal(L, "my_sum");
        for (int j = 1; j <= 5; j++) {
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
    double sec2 = stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec) / 1000000.0;
    double qps2 = n / sec2;

    gettimeofday(&start, NULL);
    for (int i = 0; i < n; i++) {
        raw_cpp_call();
    }
    gettimeofday(&stop, NULL);
    double sec_raw = stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec) / 1000000.0;
    double qps_raw = n / sec_raw;

    printf("compile/precompile/c++: %10.0lf %10.0lf (%8.2lfx) %10.0lf (%8.2lfx, %6.2lfx)\n",
        qps1, qps2, qps2 / qps1, qps_raw, qps_raw / qps1, qps_raw / qps2);

    lua_close(L);

    pthread_exit(NULL);
    return NULL;
}

int main() {
    const int n = 64;
    pthread_t th[n];
    for (int i = 0; i < n; i++) {
        pthread_create(&th[i], NULL, lua_interop_mt, NULL);
    }
    for (int i = 0; i < n; i++) {
        pthread_join(th[i], NULL);
    }
    return 0;
}


