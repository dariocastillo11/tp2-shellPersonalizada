/**
 * @file main.h
 * @brief Cabecera que contiene las declaraciones de funciones y variables globales
 * para el manejo de señales y el control de la shell interactiva.
 * Created by DARIO on 27/10/24.
 */
#ifndef MAIN_H
#define MAIN_H
/**
 * @brief Defi
 */
#define TRUE 1
/**
 * @brief Defi
 */
#define FALSE !TRUE

// Identificadores y modos de la shell
extern pid_t GBSH_PID;
extern pid_t GBSH_PGID;
extern int GBSH_IS_INTERACTIVE;
extern struct termios GBSH_TMODES;

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
 * @param p
 */
void signalHandler_tstp(int p);

/**
 * @brief signalHandler_quit
 * @param p
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
/**
 * @brief
 *
 * @param args
 * @return int
 */
int commandHandler(char* args[]);
/**
 * @brief
 *
 */
void init(void);
/**
 * @brief
 *
 */
void welcomeScreen(void);
/**
 * @brief
 *
 * @param args
 * @param option
 * @return int
 */
int manageEnviron(char* args[], int option);
/**
 * @brief
 *
 */
void shellPrompt(void);
/**
 * @brief
 *
 */
void fileIO(char* args[], char* inputFile, char* outputFile, int option);
/**
 * @brief
 *
 */
void pipeHandler(char* args[]);
#endif // MAIN_H
