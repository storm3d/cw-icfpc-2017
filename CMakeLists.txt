cmake_minimum_required(VERSION 3.8)
project(cw_icfpc_2017)

include_directories(include SYSTEM)
include_directories(model solver)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(SOURCE_FILES
		punter/punter.cpp
		model/GameState.cpp
		model/GameState.h
		solver/Solver.cpp
		solver/Solver.h
		model/Empire.cpp
		model/Empire.h
		punter/OfflineProtocol.cpp
		punter/OfflineProtocol.h)

if (MINGW)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic")
endif (MINGW)

add_executable(punter ${SOURCE_FILES})
target_link_libraries(punter PRIVATE)

set(SOURCE_FILES local_runner/local_runner.cpp)
add_executable(local_runner ${SOURCE_FILES})

set(SOURCE_FILES
		tests/tests-main.cpp
		model/GameState.cpp
		tests/TestGameState.cpp
		punter/OfflineProtocol.cpp
		punter/OfflineProtocol.h
		tests/OfflineProtocolTest.cpp
		solver/Solver.cpp
		model/Empire.cpp
		model/Empire.h
		tests/SolverTests.cpp
		)
add_executable(tests ${SOURCE_FILES})
