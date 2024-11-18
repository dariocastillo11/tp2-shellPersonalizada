#include "unity.h"
#include "tools.h"  // Aquí incluirías las cabeceras de tus funciones a probar

void setUp(void) {
    // Este código se ejecuta antes de cada prueba
}

void tearDown(void) {
    // Este código se ejecuta después de cada prueba
}

void test_comando_echo(void) {
    // Aquí pruebas si el comando 'echo' funciona correctamente
    TEST_ASSERT_EQUAL_STRING("Hello", echo("Hello"));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_comando_echo);
    return UNITY_END();
}
