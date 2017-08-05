cmake_minimum_required(VERSION 3.8)
project(cw_icfpc_2017)

include_directories(include SYSTEM)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
		punter/punter.cpp
		model/GameState.cpp
		model/Dijkstra.cpp
		model/GameState.h
		model/Solver.cpp
		model/Solver.h
		model/Empire.cpp
		model/Empire.h punter/OfflineProtocol.cpp punter/OfflineProtocol.h)

if (MINGW)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic")
endif (MINGW)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
add_executable(punter ${SOURCE_FILES})
target_link_libraries(punter PRIVATE)

set(SOURCE_FILES punter2/punter2.cpp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
add_executable(punter2 ${SOURCE_FILES})
target_link_libraries(punter2 PRIVATE)

set(SOURCE_FILES local_runner/local_runner.cpp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
add_executable(local_runner ${SOURCE_FILES})

set(SOURCE_FILES tests/tests-main.cpp tests/tests-dijkstra.cpp model/Dijkstra.cpp tests/TestGameState.cpp model/GameState.cpp punter/OfflineProtocol.cpp punter/OfflineProtocol.h tests/OfflineProtocolTest.cpp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
add_executable(tests ${SOURCE_FILES})
