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
set(CMAKE_C_FLAGS_DEBUG "-g3 -O0 -Wall -Wextra -Wpedantic -Werror -Wunused-parameter -Wmissing-prototypes -Wstrict-prototypes -fhardened")

# include h-eaders
include_directories(include)

#Find dependences
find_package(cJSON REQUIRED)
find_package(unity REQUIRED)

# Incluir el submódulo de prometheus-client-c
add_subdirectory(../lib/prometheus-client-c)
# Incluir el submódulo tp1-s.op
add_subdirectory(../lib/tp1-s.op)
# Agregar Unity como subdirectorio
add_subdirectory(lib/unity)
# add_executable -->.exe/bnary
# Crear el ejecutable
#add_library ---> .a/.so/.dll STATC SHARED
add_executable(ShellProject src/main.c src/tools.c)

target_link_libraries(ShellProject PRIVATE cjson::cjson unity::unity)

if(RUN_COVERAGE EQUAL 1)
    message("Run with coverage")
    #add_subdirectory(test)
endif()

# Vincular las bibliotecas del submódulo tp1-s.op
target_link_libraries(ShellProject PRIVATE tp1-s.op)
