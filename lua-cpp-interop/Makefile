LUA_SRC_DIR=../../lynx/lua-5.2.1/src

all: interop

interop: interop.cc
	g++ interop.cc -I $(LUA_SRC_DIR) -L $(LUA_SRC_DIR) -llua -o interop

clean:
	rm -f interop

