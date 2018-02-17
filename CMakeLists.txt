cmake_minimum_required (VERSION 2.8.12)
project (game-of-life)

set (CMAKE_CXX_COMPILER "clang++")
set (CMAKE_EXPORT_COMPILE_COMMANDS "ON")

include (FindPkgConfig)
pkg_check_modules (SDL2 sdl2 REQUIRED)
include_directories (include ${SDL2_INCLUDE_DIRS})

add_compile_options(-std=c++14 -Werror -Wall -pedantic -fno-omit-frame-pointer -Ofast -march=native)
#add_compile_options(-std=c++14 -Werror -Wall -pedantic -fno-omit-frame-pointer -fsanitize=undefined,leak,address)

add_library (DisplayLib src/display.cpp)
add_library (BufferLib src/buffer.cpp)

add_executable (game-of-life src/main.cpp)
target_link_libraries (game-of-life
	DisplayLib
	BufferLib
	#-fsanitize=leak,address,undefined
	${SDL2_LDFLAGS}
	-pthread
	-s
)

add_custom_target (valgrind
	DEPENDS game-of-life
	COMMAND valgrind --track-origins=yes --show-leak-kinds=all  --leak-check=full -v ./game-of-life
)

add_custom_target (perf
	DEPENDS game-of-life
	COMMAND perf stat ./game-of-life
)

add_custom_target (report
	DEPENDS game-of-life
	COMMAND perf record -g ./game-of-life
	COMMAND perf report -g 'graph,0.5,caller' --sort comm,dso,sym
)

add_custom_target (tiny
	DEPENDS game-of-life
	COMMAND sstrip ./game-of-life
	COMMAND gzip -cn9 ./game-of-life >small
	COMMAND cat ../data/tiny ./small >compact
	COMMAND chmod +x ./compact
	COMMAND rm ./small ./game-of-life
	COMMAND mv ./compact ./game-of-life
)

add_custom_target (tidy
	DEPENDS game-of-life
	COMMAND clang-tidy -p=. ../src/*.cpp
)
