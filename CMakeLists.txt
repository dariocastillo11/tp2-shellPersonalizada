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
#find_package(cJSON REQUIRED)
find_package(unity REQUIRED)


# Incluir el submódulo tp1-s.op
add_subdirectory(lib/tp1-s.op)
# Agregar Unity como subdirectorio
add_subdirectory(lib/unity)


add_executable(Shelltp2 src/main.c src/tools.c )

target_link_libraries(Shelltp2 PRIVATE cjson::cjson unity::unity)

# Crear un ejecutable para el submódulo
add_executable(monitoreo
    lib/tp1-s.op/src/main.c  # El archivo main.c del submódulo
    lib/tp1-s.op/src/expose_metrics.c
    lib/tp1-s.op/src/metrics.c)
# Enlazar bibliotecas para el submódulo
target_link_libraries(monitoreo PRIVATE tp1-s-op)


if(RUN_COVERAGE EQUAL 1)
    message("Run with coverage")
    #add_subdirectory(test)
endif()
