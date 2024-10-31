//
// Created by DARIO on 27/10/24.
//
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
/**
 * @brief start_monitor
 * Lanza el monitor de sistema en backgroud
 * utiliza SIG
 */
#define MAX_PATH 1024
#define METRIC_RELATIVE_PATH "../../so-i-24-chp2-dariocastillo11/metrics"

// pid_t metric_pid = 0;
/**
 * @brief start_monitor
 * Inicia el monitor en background y devuelve el pid del proceso monitor de forma indefinida
 * @return
 */
pid_t start_monitor();

// Estructura para almacenar la configuración del proceso
typedef struct
{
    pid_t pid;        // Almacena id del proceso
    int timeout;      // tiempo de vida del proceso
    int period;       // intervalo de tiempo del proceso
    int is_running;   // actividad del proceso
    timer_t timer_id; // id de timer
} ProcessControl;

int check_process(pid_t pid);
/**
 * @brief stop_process
 * pausa el monitor de sistema
 * utiliza SIG
 */
pid_t stop_process(pid_t pid);
/**
 * @brief launchProg
 * lanza un determinado proceso, con la opcion & de ejecutarlo en segundo plano
 * utiliza SIG
 */

void launchProg(char** args, int background);

void cjson_manager(int doc);
void reset_terminal_mode(void);
void handle_keypress();
void set_noncanonical_mode();

void timer_handler(int sig);

void setup_timer(int period);

int process_control_json(const char* json_string, int pid);

void timeout_handler(int sig);

char* read_json_file(const char* filename);

#endif // TOOL_H
