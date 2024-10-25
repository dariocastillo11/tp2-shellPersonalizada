# Verfy cmake version
cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)
#create project
project(
    "ShellProject"
    VERSION 1.0.0
    DESCRIPTION "A nice shell project"
    LANGUAGES C
)

#Enable testing
include(CTest)

# Flags for compilng
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAG_DEBUG "-g3 -OO -Wall -Wextra -Wpedantc -Werror -Wunused-parameter -Wmissing-prototypes -Wstric-prototipes -fhardened")

# include h-eaders
include_directories(include)

#Find dependences
find_package(cJSON REQUIRED)
find_package(unity REQUIRED)

# add_executable -->.exe/bnary
#add_library ---> .a/.so/.dll STATC SHARED
add_executable(${PROJECT_NAME} src/main.c)

target_link_libraries(${PROJECT_NAME} PRIVATE cjson::cjson)

if(RUN_COVERAGE EQUAL 1)
    message("Run with coverage")
    #add_subdirectory(test)
endif()
