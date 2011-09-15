

wx-sdl: wx-sdl.cc
	g++ -W -Wall -g `wx-config --cxxflags` `sdl-config --cflags` wx-sdl.cc -o wx-sdl `wx-config --libs` `sdl-config --static-libs`

clean:
	rm -rf wx-sdl wx-sdl.dSYM
