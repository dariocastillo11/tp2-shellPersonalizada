/**
 * @file main.c
 * @brief
 * Created by DARIO on 27/10/24.
 * declaracones defuncones relacionadas
 * con herramentas y utildades
 * @author dario castillo
 */
#include "tools.h"
#include "util.h"
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

#define LIMIT 256        // max number of tokens for a command
#define MAXLINE 1024     // Limite de caracteres para el usuario
#define MIN_PS_ID 30000  // Valor minimo posible de proceso a controlar
#define MAX_PS_ID 100000 // Valor maximo posible de proceso a controlar

struct sigaction act_child;
struct sigaction act_int;

pid_t pid;
pid_t metric_pid = -1;
pid_t monitor_pid = -1; // Variable global para almacenar el PID del proceso de monitoreo

int no_reprint_prmpt;

/**
 * @brief init
 * Funcion de inicializacion de shell recomendada por GNU
 * @link http://www.gnu.org/software/libc/manual/html_node/Initializing-the-Shell.html
 *
 * Inicializa las variables, configuraciones y estructuras necesarias
 *  para el funcionamiento del programa.
 *
 * Funcionalidad:
 *   Configura elementos iniciales como variables globales, estructuras
 *   de datos y recursos que el programa utilizará.
 *   Puede establecer el entorno de ejecución, como inicializar
 *   configuraciones de sistema, establecer señales o configuraciones
 *   predeterminadas para asegurar que el programa arranque en un
 *   estado controlado.
 */
void init()
{
    GBSH_PID = getpid();
    GBSH_IS_INTERACTIVE = isatty(STDIN_FILENO);
    if (GBSH_IS_INTERACTIVE)
    {
        while (tcgetpgrp(STDIN_FILENO) != (GBSH_PGID = getpgrp()))
            kill(GBSH_PID, SIGTTIN);

        // configuracion de handlers for SIGCHILD and SIGINT
        act_child.sa_handler = signalHandler_child;
        act_int.sa_handler = signalHandler_int;

        sigaction(SIGCHLD, &act_child, 0);
        sigaction(SIGINT, &act_int, 0);
        // Put ourselves in our own process group
        setpgid(GBSH_PID, GBSH_PID); // Nuestra shell tendra pid de grupo
        GBSH_PGID = getpgrp();
        if (GBSH_PID != GBSH_PGID)
        {
            printf("Error, la shell no pudo hacerse del pid de grupo");
            exit(EXIT_FAILURE);
        }
        // Tomar control de la terminal
        if (tcsetpgrp(STDIN_FILENO, GBSH_PGID) < 0)
        {
            perror("Error al tomar el control de la terminal");
            exit(EXIT_FAILURE);
        }
        // Guardar los atributos de terminal por defecto
        if (tcgetattr(STDIN_FILENO, &GBSH_TMODES) < 0)
        {
            perror("Error al obtener los atributos de la terminal");
            exit(EXIT_FAILURE);
        }
        // Get the current directory that will be used in different methods
        currentDirectory = (char*)calloc(1024, sizeof(char));
    }
    else
    {
        printf("Could not make the shell interactive.\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief welcomeScreen
 * Muestra una pantalla de bienvenida o introducción al usuario.
 */
void welcomeScreen()
{
    struct utsname uts;
    time_t now;
    struct tm* tm_now;
    uname(&uts);
    now = time(NULL);
    tm_now = localtime(&now);
    printf("\033[1;32m"); // Texto verde y negrita
    printf("\n\t============================================\n");
    printf("\t               Mini shell en C 1.0.3\n");
    printf("\t--------------------------------------------\n");
    printf("\t     Trabajo Practico Nº2 Sistemas Operativos\n");
    printf("\t============================================\n");
    printf("\t     Autor: DARIO ALBERTO CASTILLO\n");
    printf("\t============================================\n");
    printf("\t  Sistema: %s\n", uts.sysname);
    printf("\t  Fecha: %d/%02d/%d Hora: %02d:%02d:%02d\n", tm_now->tm_mday, tm_now->tm_mon + 1, tm_now->tm_year + 1900,
           tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
    printf("\t============================================\n");
    printf("\033[0m"); // Restablecer color y estilo predeterminados

    printf("\tIngrese un comando: \n ");
}

/**
 * SIGNAL HANDLERS
 */
/**
 * @brief signalHandler_child
 * signal handler for SIGCHLD
 *  Maneja señales enviadas cuando un proceso hijo cambia de estado.
 * Funcionalidad:
 *   Captura señales como SIGCHLD, que son enviadas al proceso padre
 *   cuando un proceso hijo termina o cambia de estado.
 *   Generalmente, esta función utiliza waitpid para limpiar
 *   correctamente los procesos hijos y evitar procesos zombis.
 *   Es útil para aplicaciones que lanzan procesos secundarios y
 *   necesitan monitorear su estado sin bloquear el programa principal.
 */
void signalHandler_child(int p)
{
    /* Wait for all dead processes.
     * We use a non-blocking call (WNOHANG) to be sure this signal handler will not
     * block if a child was cleaned up in another part of the program. */
    while (waitpid(-1, NULL, WNOHANG) > 0)
    {
    }
    printf("\n");
}

/**
 * @signalHandler_int
 * Signal handler for SIGINT
 *  Maneja la señal de interrupción (SIGINT) generada, al presionar Ctrl+C.
 *
 * Funcionalidad:
 *   Permite capturar SIGINT para que el programa reaccione
 *   correctamente en vez de finalizar abruptamente.
 *   Esta función puede cerrar archivos, liberar memoria, desactivar
 *   temporizadores, o realizar cualquier otra tarea de limpieza
 *   antes de que el programa termine.
 *   Permite que el programa salga de forma controlada al recibir
 *   una señal de interrupción del usuario.
 */
void signalHandler_int(int p)
{
    // We send a SIGTERM signal to the child process
    if (kill(pid, SIGTERM) == 0)
    {
        printf("\nEl proceso %d recibio una señal de interrupcion SIGINT\n", pid);
        no_reprint_prmpt = 1;
    }
    else
    {
        printf("\n");
    }
}

/** @brief shellPrompt
 * punto 1 del tp2
 *	Muestra por pantalla el promt de la shell
 *  Muestra el nombre del directorio actual donde está la shell,
 *  esperando la entrada del usuario para el próximo comando.
 */
void shellPrompt()
{
    char hostname[1024];
    char cwd[1024];

    // Obtener el nombre del host
    if (gethostname(hostname, sizeof(hostname)) == -1)
    {
        perror("gethostname");
        exit(EXIT_FAILURE);
    }
    // Obtener el directorio de trabajo actual
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }
    // Obtener el nombre de usuario
    char* username = getenv("USER");

    // Convertir el directorio home a ~
    char* home = getenv("HOME");
    char currentDir[1024];

    if (strncmp(cwd, home, strlen(home)) == 0)
    {
        snprintf(currentDir, sizeof(currentDir), "~%s", cwd + strlen(home)); // Añadir '~' antes del directorio relativo
    }
    else
    {
        snprintf(currentDir, sizeof(currentDir), "%s", cwd); // Usar el camino completo
    }

    // Imprimir el prompt con el texto "sheldario -->"
    printf("ShellDarìo --> \033[1;32m%s\033[0m@\033[1;34m%s\033[0m:\033[1;36m%s\033[0m$ ", username, hostname,
           currentDir);

    fflush(stdout); // para que se muestre el prompt correctamente
}

/**
 * @brief changeDirectory  CD
 * metodo para manejar y cambiar de directorio
 * Cambia el directorio de trabajo actual al especificado en path.
 * Funcionamiento:
 * Utiliza chdir(path) y verifica si el cambio fue exitoso.
 * Si falla, muestra un mensaje de error indicando que no
 * pudo cambiar al directorio solicitado.
 */
int changeDirectory(char* args[])
{
    char* prev_dir = getenv("OLDPWD"); // Guarda el directorio anterior
    char current_dir[1024];
    getcwd(current_dir, sizeof(current_dir)); // Obtén el directorio actual

    if (args[1] == NULL)
    {
        // Cambiar al directorio home
        if (chdir(getenv("HOME")) == -1)
        {
            fprintf(stderr, "Error al cambiar al directorio home: %s\n", strerror(errno));
            return -1;
        }
    }
    else if (strcmp(args[1], "-") == 0)
    {
        if (prev_dir != NULL)
        {
            printf("%s\n", prev_dir); // Imprime el directorio anterior
            if (chdir(prev_dir) == -1)
            {
                fprintf(stderr, "Error al cambiar al directorio anterior: %s\n", strerror(errno));
                return -1;
            }
        }
        else
        {
            fprintf(stderr, "No hay un directorio anterior.\n");
            return -1;
        }
    }
    else
    {
        // Cambiar al directorio especificado
        if (chdir(args[1]) == -1)
        {
            perror(args[1]);
            return -1;
        }
    }

    setenv("OLDPWD", current_dir, 1);                           // Actualiza OLDPWD
    setenv("PWD", getcwd(current_dir, sizeof(current_dir)), 1); // Actualiza PWD
    return 0;
}

/**
 * @brief manageEnviron
 * Administrar las variables de entorno con distintas opciones
 * si se ejecuta "environ" esto imprimira nuestras variables de entorno
 * si ejecuta "setenv  x": configurara nuestras variables de entorno a un valor
 * si se ejecuta "unsetenv  x": elimina una variable de entorno
 */
int manageEnviron(char* args[], int option)
{
    char** env_aux;
    switch (option)
    {
    case 0:
        for (env_aux = environ; *env_aux != 0; env_aux++)
        {
            printf("%s\n", *env_aux);
        }
        break;
    case 1:
        if ((args[1] == NULL) && args[2] == NULL)
        {
            printf("%s", "No hay suficientes argumentos de entrada\n");
            return -1;
        }
        // We use different output for new and overwritten variables
        if (getenv(args[1]) != NULL)
        {
            printf("%s", "La variable fue sobreescrita correctamente\n");
        }
        else
        {
            printf("%s", "La variable fue creada correctamente\n");
        }
        // If we specify no value for the variable, we set it to ""
        if (args[2] == NULL)
        {
            setenv(args[1], "", 1);
            // We set the variable to the given value
        }
        else
        {
            setenv(args[1], args[2], 1);
        }
        break;
    //
    case 2:
        if (args[1] == NULL)
        {
            printf("%s", "No hay suficientes argumentos de entrada\n");
            return -1;
        }
        if (getenv(args[1]) != NULL)
        {
            unsetenv(args[1]);
            printf("%s", "La variable fue eliminada correctamente\n");
        }
        else
        {
            printf("%s", "La variable no existe\n");
        }
        break;
    }
    return 0;
}

/**
 * @brief launchProg
 * metodo para ejecutar un proceso en segundo plano
 * Maneja la entrada y salida de archivos.
 * Abre archivos y redirige la entrada (<) o salida (>)
 * estándar según el option indicado.
 * Permite ejecutar comandos con entrada/salida redireccionadas
 * desde/hacia archivos.
 */
void fileIO(char* args[], char* inputFile, char* outputFile, int option)
{
    int err = -1;

    int fileDescriptor; // between 0 and 19, describing the output or input file

    if ((pid = fork()) == -1)
    {
        printf("Child process could not be created\n");
        return;
    }
    if (pid == 0)
    {
        // Option 0: output redirection
        if (option == 0)
        {
            // We open (create) the file truncating it at 0, for write only
            fileDescriptor = open(outputFile, O_CREAT | O_TRUNC | O_WRONLY, 0600);
            // We replace de standard output with the appropriate file
            dup2(fileDescriptor, STDOUT_FILENO);
            close(fileDescriptor);
            // Option 1: input and output redirection
        }
        else if (option == 1)
        {
            // We open file for read only (it's STDIN)
            fileDescriptor = open(inputFile, O_RDONLY, 0600);
            // We replace de standard input with the appropriate file
            dup2(fileDescriptor, STDIN_FILENO);
            close(fileDescriptor);
            // Same as before for the output file
            fileDescriptor = open(outputFile, O_CREAT | O_TRUNC | O_WRONLY, 0600);
            dup2(fileDescriptor, STDOUT_FILENO);
            close(fileDescriptor);
        }
        setenv("parent", getcwd(currentDirectory, 1024), 1);
        if (execvp(args[0], args) == err)
        {
            printf("err");
            kill(getpid(), SIGTERM);
        }
    }
    waitpid(pid, NULL, 0);
}

/**
 * Manipulacion de pipelines. Gestiona la señal de error SIGPIPE.
 *
 *Funcionalidad:
 *   SIGPIPE se genera cuando un proceso intenta escribir en un
 *   pipe o socket que ha sido cerrado por el otro extremo.
 *   Esta función captura y maneja este error para evitar que el
 *   programa se cierre inesperadamente cuando ocurre un error en
 *   la comunicación entre procesos o conexiones.
 *   Puede reintentar la operación o registrar el error para informar
 *   al usuario, manteniendo la estabilidad del programa cuando
 *   se producen errores de conexión o comunicación.
 */
void pipeHandler(char* args[])
{
    int filedes[2]; // pos. 0 salida, pos. 1 entrada del pipe
    int filedes2[2];
    int num_cmds = 0;
    char* command[LIMIT]; // Array auxiliar para almacenar el comando actual
    pid_t pid;
    int err = -1;
    int end = 0;
    // Variables utilizadas para los bucles
    int i = 0;
    int j = 0;
    int k = 0;
    int l = 0;

    // Cálculo del número de comandos, usando el símbolo '|'
    while (args[l] != NULL)
    {
        if (strcmp(args[l], "|") == 0)
        {
            num_cmds++;
        }
        l++;
    }
    num_cmds++; // Aumento en uno para contar el último comando
    // Bucle principal. Para cada comando entre '|', configurará los pipes y
    // reemplazará la entrada/salida estándar para la ejecución del comando
    while (args[j] != NULL && end != 1)
    {
        k = 0;
        // Array auxiliar para almacenar el comando que será ejecutado en cada iteración
        while (strcmp(args[j], "|") != 0)
        {
            command[k] = args[j];
            j++;
            if (args[j] == NULL)
            {
                // La variable 'end' evita que el programa entre de nuevo en el bucle
                // cuando no hay más argumentos
                end = 1;
                k++;
                break;
            }
            k++;
        }
        command[k] = NULL; // Última posición del comando será NULL para `exec`

        j++; // Avanza el índice para el siguiente argumento en `args`

        // Según la iteración, se establecen diferentes descriptores de pipes
        // Esto permite conectar las entradas y salidas de dos comandos distintos
        if (i % 2 != 0)
        {
            pipe(filedes); // Para índices impares
        }
        else
        {
            pipe(filedes2); // Para índices pares
        }
        pid = fork();

        if (pid == -1)
        {
            if (i != num_cmds - 1)
            {
                if (i % 2 != 0)
                {
                    close(filedes[1]); // Cierra el pipe en índice impar
                }
                else
                {
                    close(filedes2[1]); // Cierra el pipe en índice par
                }
            }
            printf("No se pudo crear el proceso hijo\n");
            return;
        }
        if (pid == 0)
        {
            // Si es el primer comando, redirige la salida estándar al pipe
            if (i == 0)
            {
                dup2(filedes2[1], STDOUT_FILENO);
            }
            // Si es el último comando, se redirige la entrada estándar
            // dependiendo de si el número de comandos es par o impar
            else if (i == num_cmds - 1)
            {
                if (num_cmds % 2 != 0)
                {
                    dup2(filedes[0], STDIN_FILENO); // Número impar de comandos
                }
                else
                {
                    dup2(filedes2[0], STDIN_FILENO); // Número par de comandos
                }
            }
            // Si el comando está en el medio, utiliza dos pipes, uno para
            // entrada y otro para salida
            else
            {
                if (i % 2 != 0)
                {
                    dup2(filedes2[0], STDIN_FILENO);
                    dup2(filedes[1], STDOUT_FILENO);
                }
                else
                {
                    dup2(filedes[0], STDIN_FILENO);
                    dup2(filedes2[1], STDOUT_FILENO);
                }
            }
            // Ejecuta el comando y mata el proceso si falla
            if (execvp(command[0], command) == err)
            {
                kill(getpid(), SIGTERM);
            }
        }
        // CIERRA DESCRIPTORES EN EL PADRE
        if (i == 0)
        {
            close(filedes2[1]); // Cierra la salida del primer pipe
        }
        else if (i == num_cmds - 1)
        {
            if (num_cmds % 2 != 0)
            {
                close(filedes[0]); // Cierra la entrada del pipe en caso impar
            }
            else
            {
                close(filedes2[0]); // Cierra la entrada del pipe en caso par
            }
        }
        else
        {
            if (i % 2 != 0)
            {
                close(filedes2[0]);
                close(filedes[1]);
            }
            else
            {
                close(filedes[0]);
                close(filedes2[1]);
            }
        }
        waitpid(pid, NULL, 0); // Espera a que termine el proceso hijo
        i++;                   // Incrementa el contador para la siguiente iteración
    }
}

/**
 * @brief commandHandler
 * metodo para receptar y manejar los comandos ingresados por
 * el usuario.
 * se verfica el comando ingresado y se ejecutan acciones
 * especificas como:
 * exit; pwd, cd, envron, setenv, unsetenv, start_monitor
 * status_monitor, stop_monitor, config_proces
 */
int commandHandler(char* args[])
{

    int i = 0;
    int j = 0;
    int fileDescriptor;
    int standardOut;
    int aux;
    int background = 0;
    char* args_aux[LIMIT];
    no_reprint_prmpt = 0;
    // caracteres especiales

    while (args[j] != NULL)
    {
        if ((strcmp(args[j], ">") == 0) || (strcmp(args[j], "<") == 0) || (strcmp(args[j], "&") == 0))
        {
            break;
        }
        args_aux[j] = args[j];
        j++;
    }

    // 'exit' Termina la shell.
    if (strcmp(args[0], "quit") == 0)
    {
        printf("Cerrando shell de dario castillo...\n");
        exit(0);
    }

    // 'pwd' command  Imprime el directorio de trabajo actual.
    // Puede redirigir la salida a un archivo.
    else if (strcmp(args[0], "pwd") == 0)
    {
        if (args[j] != NULL)
        {
            // para guardar en archivo
            if ((strcmp(args[j], ">") == 0) && (args[j + 1] != NULL))
            {
                fileDescriptor = open(args[j + 1], O_CREAT | O_TRUNC | O_WRONLY, 0600);
                // We replace de standard output with the appropriate file
                standardOut = dup(STDOUT_FILENO); // first we make a copy of stdout
                                                  // because we'll want it back
                dup2(fileDescriptor, STDOUT_FILENO);
                close(fileDescriptor);
                printf("%s\n", getcwd(currentDirectory, 1024));
                dup2(standardOut, STDOUT_FILENO);
            }
        }
        else
        {
            printf("%s\n", getcwd(currentDirectory, 1024));
        }
    }

    // 'clear' command limpia pantalla
    else if (strcmp(args[0], "clr") == 0)
        system("clear");

    // 'cd' command cambia el directorio actuale
    else if (strcmp(args[0], "cd") == 0)
        changeDirectory(args);

    //'echo'  Muestra <comentario> en la pantalla seguido
    // de una nueva línea.
    // Soporta variables de entorno utilizando $
    else if (strcmp(args[0], "echo") == 0)
    {
        for (int k = 1; args[k] != NULL; k++)
        {
            if (args[k][0] == '$')
            {
                char* env_value = getenv(args[k] + 1); // +1 para omitir el '$'
                if (env_value != NULL)
                {
                    printf("%s ", env_value);
                }
                else
                {
                    // Intentar buscar la variable en minúsculas también
                    char lower_var[256];
                    snprintf(lower_var, sizeof(lower_var), "%s", args[k] + 1);
                    for (int i = 0; lower_var[i]; i++)
                    {
                        lower_var[i] = tolower(lower_var[i]);
                    }
                    env_value = getenv(lower_var);
                    if (env_value != NULL)
                    {
                        printf("%s ", env_value);
                    }
                    else
                    {
                        printf("%s ", args[k]); // Imprimir el nombre de la variable si no existe
                    }
                }
            }
            else
            {
                printf("%s ", args[k]);
            }
        }
        printf("\n");
    }

    // 'environ' command Imprime las variables de entorno.
    else if (strcmp(args[0], "environ") == 0)
    {
        if (args[j] != NULL)
        {
            // para guardar en archivo
            if ((strcmp(args[j], ">") == 0) && (args[j + 1] != NULL))
            {
                fileDescriptor = open(args[j + 1], O_CREAT | O_TRUNC | O_WRONLY, 0600);
                // We replace de standard output with the appropriate file
                standardOut = dup(STDOUT_FILENO); // first we make a copy of stdout
                                                  // because we'll want it back
                dup2(fileDescriptor, STDOUT_FILENO);
                close(fileDescriptor);
                manageEnviron(args, 0);
                dup2(standardOut, STDOUT_FILENO);
            }
        }
        else
        {
            manageEnviron(args, 0);
        }
    }

    // 'setenv' command para setear las variables de entorno
    else if (strcmp(args[0], "setenv") == 0)
        manageEnviron(args, 1);

    // 'unsetenv' command maneja variables de entorno
    else if (strcmp(args[0], "unsetenv") == 0)
        manageEnviron(args, 2);

    // 'start_monitor' command que inicia el monitor de sistema
    else if (strcmp(args[0], "start_monitor") == 0)
    {
        pid_t pid = fork(); // Crear un proceso hijo

        if (pid == -1)
        {
            // Error al crear el proceso
            printf("Error al iniciar el proceso de monitoreo.\n");
            return 1;
        }

        if (pid == 0)
        {
            // En el proceso hijo: Ejecutar el ejecutable 'monitoreo'
            char* exec_args[] = {"./monitoreo",
                                 NULL};      // Usar ruta relativa si el ejecutable está en el mismo directorio
            execvp(exec_args[0], exec_args); // Reemplazar el proceso actual por 'monitoreo'

            // Si execvp falla
            perror("Error al ejecutar monitoreo");
            exit(1);
        }
        else
        {
            // En el proceso padre: Guardar el PID del proceso de monitoreo
            monitor_pid = pid;
            printf("Monitor iniciado con PID: %d\n", pid);
        }
    }

    // Comando 'status_monitor' que obtiene y muestra las métricas del monitor
    else if (strcmp(args[0], "status_monitor") == 0)
    {
        // Ejecutar 'curl' desde la shell para obtener las métricas
        int result = system("curl -s http://localhost:8080/metrics"); // Utiliza 'curl' para obtener las métricas

        if (result == -1)
        {
            printf("Error al obtener las métricas del monitor.\n");
        }
    }

    else if (strcmp(args[0], "stop_monitor") == 0)
    {
        if (monitor_pid == -1)
        {
            // No se ha iniciado el monitor
            printf("No se ha iniciado el proceso de monitoreo.\n");
        }
        else
        {
            // Enviar señal SIGTERM para terminar el proceso de monitoreo
            if (kill(monitor_pid, SIGTERM) == 0)
            {
                printf("Proceso de monitoreo detenido correctamente.\n");
                monitor_pid = -1; // Resetear el PID después de detener el proceso
            }
            else
            {
                perror("Error al intentar detener el proceso de monitoreo");
            }
        }
    }

    else if (strcmp(args[0], "config_process") == 0)
    {
        signal(SIGALRM, timer_handler); // conecto la señal SIGALRM a los handlers correspondientes
        // signal(SIGALRM, timeout_handler);
        if (metric_pid > MIN_PS_ID && metric_pid < MAX_PS_ID)
        {
            // Leer el archivo config.json
            char* json_content = read_json_file("config.json");
            if (json_content == NULL)
            {
                printf("Error al leer el archivo config.json\n");
                return -1;
            }
            // Procesar el contenido JSON
            if (process_control_json(json_content, metric_pid) != 0)
            {
                free(json_content);
                return -1;
            }
            // Liberar la memoria del contenido JSON
            free(json_content);
            printf("Configuración cargada exitosamente de config.json\n");
        }
        else
        {
            printf("El monitor aun no se ha iniciado, por favor, si desea manipular el monitor ejecute el comando "
                   "start_monitor");
        }
    }

    else
    {
        // If none of the preceding commands were used, we invoke the
        // specified program. We have to detect if I/O redirection,
        // piped execution or background execution were solicited
        while (args[i] != NULL && background == 0)
        {
            // If background execution was solicited (last argument '&')
            // we exit the loop
            if (strcmp(args[i], "&") == 0)
            {
                background = 1;
                // If '|' is detected, piping was solicited, and we call
                // the appropriate method that will handle the different
                // executions
            }
            else if (strcmp(args[i], "|") == 0)
            {
                pipeHandler(args);
                return 1;
                // If '<' is detected, we have Input and Output redirection.
                // First we check if the structure given is the correct one,
                // and if that is the case we call the appropriate method
            }
            else if (strcmp(args[i], "<") == 0)
            {
                aux = i + 1;
                if (args[aux] == NULL || args[aux + 1] == NULL || args[aux + 2] == NULL)
                {
                    printf("Not enough input arguments\n");
                    return -1;
                }
                else
                {
                    if (strcmp(args[aux + 1], ">") != 0)
                    {
                        printf("Usage: Expected '>' and found %s\n", args[aux + 1]);
                        return -2;
                    }
                }
                fileIO(args_aux, args[i + 1], args[i + 3], 1);
                return 1;
            }
            // If '>' is detected, we have output redirection.
            // First we check if the structure given is the correct one,
            // and if that is the case we call the appropriate method
            else if (strcmp(args[i], ">") == 0)
            {
                if (args[i + 1] == NULL)
                {
                    printf("Not enough input arguments\n");
                    return -1;
                }
                fileIO(args_aux, NULL, args[i + 1], 0);
                return 1;
            }
            i++;
        }
        // We launch the program with our method, indicating if we
        // want background execution or not
        args_aux[i] = NULL;
        launchProg(args_aux, background);

        /**
         * For the part 1.e, we only had to print the input that was not
         * 'exit', 'pwd' or 'clear'. We did it the following way
         */
        //	i = 0;
        //	while(args[i]!=NULL){
        //		printf("%s\n", args[i]);
        //		i++;
        //	}
    }
    return 1;
}

/**
 * @brief Main of shell
 * Propósito: Este es el punto de entrada de la shell.
   Funcionalidad principal:
    Configuración inicial:
        Establece la terminal en modo
        de línea de comandos, asigna variables de entorno, muestra
        la pantalla de bienvenida e inicializa variables clave.
    Bucle principal de la shell:
        Imprime el prompt para el usuario.
        Toma el comando ingresado y lo descompone en tokens.
        Llama a commandHandler para manejar la ejecución del comando
        ingresado.
    Control de la terminal:
        Establece el modo no canónico para capturar
        la entrada del usuario y restaurar el modo original cuando
        la shell se cierre.
 */
int main(int argc, char* argv[], char** envp)
{
    cJSON* root = cJSON_CreateObject();
    char line[MAXLINE];  // buffer for the user input
    char* tokens[LIMIT]; // array for the different tokens in the command
    int numTokens;
    struct termios termios, original_mode;
    int no_reprint_prmpt = 0; // to prevent the printing of the shell after certain methods
    pid = -10;                // we initialize pid to an pid that is not possible

    // Obtener la configuración original de la terminal
    tcgetattr(STDIN_FILENO, &original_mode);

    // Habilitar el modo de línea de comandos y el procesamiento de secuencias de escape
    termios = original_mode;
    termios.c_lflag |= ICANON;
    termios.c_lflag |= ISIG;

    // Configurar el manejador de señales
    signal(SIGINT, signalHandler_int);

    // Inicialización y la pantalla de bienvenida
    init();
    welcomeScreen();
    // Asigno las variables de entorno
    environ = envp;
    // Asigno al directorio actual la shell
    setenv("shell", getcwd(currentDirectory, 1024), 1);

    // Abre el archivo si se pasa como argumento. parte 4 tp2
    FILE* fp = NULL;
    if (argc > 1)
    {
        fp = fopen(argv[1], "r");
        if (fp == NULL)
        {
            perror("Error abriendo el archivo");
            return EXIT_FAILURE;
        }
    }

    // Main loop, donde se leerá la entrada del usuario y se imprimirá el prompt
    while (TRUE)
    {
        memset(line, '\0', MAXLINE);

        // Imprimir el prompt si no hay necesidad de reimpresión

        shellPrompt();

        // Vaciado de la línea y captura del ingreso del usuario
        // Lee desde el archivo si está abierto, o desde stdin si no
        if (fp)
        {
            if (fgets(line, MAXLINE, fp) == NULL)
            {
                break; // Si no hay más líneas, salir del bucle
            }
        }
        else
        {
            if (fgets(line, MAXLINE, stdin) == NULL)
            {
                break; // Salir si hay un error en stdin
            }
        }

        // Si la línea está vacía, continúa el bucle
        if ((tokens[0] = strtok(line, " \n\t")) == NULL)
        {
            continue;
        }

        // Recorrer los comandos y dividirlos en tokens
        numTokens = 1;
        while ((tokens[numTokens] = strtok(NULL, " \n\t")) != NULL)
        {
            numTokens++;
        }

        // Llamada al manejador de comandos
        commandHandler(tokens);
    }

    // Cierre del archivo si fue abierto. parte 4 tp2
    if (fp)
    {
        fclose(fp);
    }

    // Restaurar modo original cuando la terminal cierre
    reset_terminal_mode();

    tcsetattr(STDIN_FILENO, TCSANOW, &termios);
    exit(0);
}
