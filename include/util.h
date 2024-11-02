/**
 * @file util.h
 * @brief Cabecera que contiene las declaraciones de funciones y variables globales
 * para el manejo de señales y el control de la shell interactiva.
 * Created by DARIO on 27/10/24.
 */
#ifndef UTIL_H
#define UTIL_H
#define TRUE 1
#define FALSE !TRUE

// Identificadores y modos de la shell
static pid_t GBSH_PID;             /**< PID del proceso de la shell. */
static pid_t GBSH_PGID;            /**< PGID de la shell para gestionar el grupo de procesos. */
static int GBSH_IS_INTERACTIVE;    /**< Indica si la shell está en modo interactivo. */
static struct termios GBSH_TMODES; /**< Estructura para almacenar los modos de terminal actuales. */

static char* currentDirectory; /**< Almacena la ruta del directorio actual. */
extern char** environ;         /**< Entorno global para acceder a las variables de entorno del sistema. */

/**
 * @brief Manejador de la señal SIGCHLD.
 * parte  tp2
 * Este manejador gestiona la señal SIGCHLD, que se
 *  envía cuando un proceso hijo finaliza.
 * Permite a la shell controlar la terminación de
 *  procesos hijos sin quedar en estado zombie.
 * @param p Código de la señal recibida.
 */
void signalHandler_child(int p);

/**
 * @brief Manejador de la señal SIGINT.
 *parte 6 tp2
 * Gestiona la señal SIGINT, permitiendo a la shell interceptar interrupciones
 * (como Ctrl+C) y evitar que terminen el proceso principal de la shell.
 *
 * @param p Código de la señal recibida.
 */
void signalHandler_int(int p);


/**
 * @brief signalHandler_tstp
 *
*/
void signalHandler_tstp(int p);

/**
 * @brief signalHandler_quit
 *
*/
void signalHandler_quit(int p);

/**
 * @brief Cambia el directorio de trabajo actual.
 *
 * Esta función cambia el directorio de trabajo actual de la shell al especificado en `args[1]`.
 * Si no se proporciona un directorio, cambia al directorio inicial.
 *
 * @param args Argumentos de la línea de comandos. `args[1]` debe contener el directorio de destino.
 * @return int 0 si el cambio fue exitoso, -1 si hubo un error.
 */
int changeDirectory(char* args[]);

#endif // UTIL_H
