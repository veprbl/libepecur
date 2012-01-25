cmake_minimum_required (VERSION 2.6)
project (EPECUR)

enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})

find_package( Boost 1.48 COMPONENTS iostreams program_options regex test_exec_monitor )
find_package( BoostEndian )
find_package( ROOT )

set( EPECUR_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src/ )

execute_process(COMMAND git rev-parse HEAD
	OUTPUT_VARIABLE GIT_COMMIT_ID
	RESULT_VARIABLE err_code
	ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE)

if (${err_code} EQUAL 127)
	message(FATAL_ERROR "Git is not avaliable!")
endif()

if (${err_code} EQUAL 128)
	message(SEND_ERROR "Git repository is not avaliable!")
endif()

message(STATUS "Last Git commit is ${GIT_COMMIT_ID}")

execute_process(COMMAND git diff --quiet
	RESULT_VARIABLE a
	OUTPUT_QUIET)

execute_process(COMMAND git diff --cached --quiet
	RESULT_VARIABLE b
	OUTPUT_QUIET)

math(EXPR c "${a} | ${b}")

if (${c} EQUAL 1)
	set(GIT_COMMIT_ID "durty working tree")
	message(AUTHOR_WARNING "Git working tree isn't clean! Please don't use compiled uncommited code for real work. You will never remember what exactly changes you did to obtain your results.")
else()
	message(FATAL_ERROR "Error while retrieving last commit id.")
endif()

add_definitions(-DGIT_COMMIT_ID="${GIT_COMMIT_ID}")

set(CMAKE_CXX_FLAGS_RELEASE "-std=c++0x -Wall -pedantic -O2")
set(CMAKE_CXX_FLAGS_DEBUG "-std=c++0x -Wall -pedantic -g")
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_RELEASE})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}) 
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}) 

add_subdirectory(src)
add_subdirectory(tests)
