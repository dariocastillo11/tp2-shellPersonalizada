# CMakeLists.txt para las pruebas

# Incluir los archivos fuente y Unity
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/lib/unity/src)

# Crear el ejecutable para las pruebas
add_executable(test_shell test_main.c test_tools.c)

# Enlazar las bibliotecas necesarias para las pruebas
target_link_libraries(test_shell PRIVATE unity::unity cjson::cjson)

# Habilitar la recopilación de cobertura si RUN_COVERAGE está activado
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(test_shell PRIVATE --coverage)
    target_link_libraries(test_shell PRIVATE --coverage)
endif()
