cmake_minimum_required(VERSION 3.8)
project(cw_icfpc_2017)

include_directories(include)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
		punter/punter.cpp
		model/GameState.cpp
		model/Dijkstra.cpp
		model/GameState.h
		model/Solver.cpp
		model/Solver.h
		model/Empire.cpp
		model/Empire.h)

if(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic")
endif(CMAKE_COMPILER_IS_GNUCC)

add_executable(punter ${SOURCE_FILES})
target_link_libraries(punter PRIVATE)

set(SOURCE_FILES local_runner/local_runner.cpp)
add_executable(local_runner ${SOURCE_FILES})

set(SOURCE_FILES tests/tests-main.cpp tests/tests-dijkstra.cpp model/Dijkstra.cpp tests/TestGameState.cpp model/GameState.cpp)
add_executable(tests ${SOURCE_FILES})
