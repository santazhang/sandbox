#include <stdio.h>
#include <sys/time.h>

#include "lua.hpp"

using namespace std;

// function to be called from lua
int my_function(lua_State* L) {
    int argc = lua_gettop(L);

//    printf("-- my_function() called with %d args\n", argc);


    for (int i = 1; i <= argc; i++) {
//        printf("-- arg %d: %s\n", i, lua_tostring(L, i));
    }

    // return value
    lua_pushnumber(L, 123);

    // number of return value
    return 1;
}

static void report_errors(lua_State* L, int status) {
    if (status != 0) {
        printf("-- %s\n", lua_tostring(L, -1));
        lua_pop(L, 1); // remove error msg
    }
}

void raw_cpp_call() {}

int main() {
    lua_State* L = luaL_newstate();

    lua_register(L, "my_function", my_function);
    int s;
    const int n = 100000;
    struct timeval start, stop;
    gettimeofday(&start, NULL);
    for (int i = 0; i < n; i++) {
        s = luaL_dostring(L, "my_function(5)");
        report_errors(L, s);
    }
    gettimeofday(&stop, NULL);
    double sec = stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec) / 1000000.0;
    printf("approach 1: call per sec=%lf\n", n / sec);
    double base = sec;

    gettimeofday(&start, NULL);
    for (int i = 0; i < n; i++) {
        lua_getglobal(L, "my_function");
        lua_pushstring(L, "x");

        // call with 1 arguments, 1 return value
        lua_call(L, 1, 1);

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
    sec = stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec) / 1000000.0;
    printf("raw cpp: call per sec=%lf (%.2lf x)\n", n / sec, base / sec);

    lua_close(L);
    return 0;
}


