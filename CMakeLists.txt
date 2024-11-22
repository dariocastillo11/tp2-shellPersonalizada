cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# Crear proyecto
project(
    "ShellProject"
    VERSION 1.0.0
    DESCRIPTION "A nice shell project"
    LANGUAGES C
)

# Incluir configuraciones de Conan

include(${CMAKE_BINARY_DIR}/Release/generators/conan_toolchain.cmake)


# Habilitar testing
include(CTest)

# Flags para compilación
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS_DEBUG "-g3 -O0 -Wall -Wextra -Wpedantic -Werror -Wunused-parameter -Wmissing-prototypes -Wstrict-prototypes -fhardened")

# Incluir headers
include_directories(include)

# Añadir subdirectorios
add_subdirectory(lib/tp1-s.op)
add_subdirectory(lib/unity)

# Crear ejecutable y enlazar bibliotecas
add_executable(Shelltp2 src/main.c src/tools.c)
target_link_libraries(Shelltp2 PRIVATE cjson unity)

# Crear ejecutable para monitoreo
add_executable(monitoreo
    lib/tp1-s.op/src/main.c
    lib/tp1-s.op/src/expose_metrics.c
    lib/tp1-s.op/src/metrics.c
)
target_link_libraries(monitoreo PRIVATE tp1-s-op)
