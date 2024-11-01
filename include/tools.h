/**
 * @file tools.h
 * @brief
 * Created by DARIO on 27/10/24.
 * declaracones defuncones relacionadas
 * con herramentas y utildades
 */

#ifndef TOOL_H
#define TOOL_H

#include <cjson/cJSON.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define MAX_PATH 1024
#define METRIC_RELATIVE_PATH "../../so-i-24-chp2-dariocastillo11/metrics"


/**
 * @brief Estructura que representa la configuración de control
 * de un proceso.
 */
typedef struct
{
    pid_t pid;        // Almacena id del proceso
    int timeout;      // tiempo de vida del proceso
    int period;       // intervalo de tiempo del proceso
    int is_running;   // actividad del proceso
    timer_t timer_id; // id de timer
} ProcessControl;



/**
 * @brief Inicia el monitor en segundo plano y devuelve el PID del proceso monitor.
 * @return pid_t El PID del proceso monitor, o -1 en caso de error.
 */
pid_t start_monitor();




/**
 * @brief Verifica si un proceso con el PID dado está activo.
 *
 * @param pid El ID del proceso a verificar.
 * @return int 1 si el proceso está activo, 0 si no lo está.
 */
int check_process(pid_t pid);



/**
 * @brief Detiene un proceso monitor en ejecución.
 *
 * @param pid El ID del proceso a detener.
 * @return pid_t El PID del proceso detenido, o -1 en caso de error.
 */
pid_t stop_process(pid_t pid);



/**
 * @brief Lanza un proceso con los argumentos proporcionados y permite la opción de ejecutarlo en segundo plano.
 *
 * @param args Los argumentos del proceso a ejecutar.
 * @param background 1 para ejecutar el proceso en segundo plano, 0 para ejecutarlo en primer plano.
 */
void launchProg(char** args, int background);



/**
 * @brief Administra y maneja el contenido JSON utilizando la biblioteca cJSON.
 *
 * @param doc Identificador o código del documento JSON a procesar.
 */
void cjson_manager(int doc);



/**
 * @brief Restablece el modo de la terminal a la configuración original (modo canónico).
 */
void reset_terminal_mode(void);


/**
 * @brief Detecta y maneja la pulsación de teclas en la terminal.
 */
void handle_keypress();



/**
 * @brief Configura la terminal en modo no canónico para capturar las teclas inmediatamente.
 */
void set_noncanonical_mode();


/**
 * @brief Manejador de señales para eventos de temporizador.
 *
 * @param sig La señal que desencadena el manejo del temporizador.
 */
void timer_handler(int sig);


/**
 * @brief Configura un temporizador con un intervalo determinado en segundos.
 *
 * @param period El período de tiempo en segundos para el temporizador.
 */
void setup_timer(int period);


/**
 * @brief Procesa un JSON para controlar el comportamiento del proceso basado en la configuración.
 *
 * @param json_string La cadena JSON que contiene los parámetros de control del proceso.
 * @param pid El ID del proceso a controlar.
 * @return int 1 si el procesamiento se realizó correctamente, 0 en caso de error.
 */
int process_control_json(const char* json_string, int pid);


/**
 * @brief Manejador de señal que se activa cuando el temporizador alcanza su límite.
 *
 * @param sig La señal que indica el tiempo de espera cumplido.
 */
void timeout_handler(int sig);


/**
 * @brief Lee un archivo JSON y devuelve su contenido como una cadena.
 *
 * @param filename El nombre del archivo JSON a leer.
 * @return char* Una cadena con el contenido del archivo JSON, o NULL en caso de error.
 *         La memoria para la cadena debe liberarse después de su uso.
 */
char* read_json_file(const char* filename);

#endif // TOOL_H
