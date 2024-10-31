/*
 * @autor DARIO ALBERTO CASTILLO
 *
 */
#include "tools.h"
#include "util.h"
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
#include <cjson/cJSON.h>

#define LIMIT 256    // max number of tokens for a command
#define MAXLINE 1024 // Limite de caracteres para el usuario
#define  MIN_PS_ID 30000 // Valor minimo posible de proceso a controlar
#define MAX_PS_ID 100000// Valor maximo posible de proceso a controlar
struct sigaction act_child;
struct sigaction act_int;
pid_t pid;

pid_t metric_pid = -1;
int no_reprint_prmpt;

/**
 * @brief init
 * Funcion de inicializacion de shell recomendada por GNU
 * http://www.gnu.org/software/libc/manual/html_node/Initializing-the-Shell.html
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
 * imprimir una presentacion de bienvenida
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

    printf("\n\tMenú de Opciones:\n");
    printf("\t1. start_monitor para iniciar la ejecucion \n");
    printf("\t2. status_monitor para comprobar el estado del monitor\n");
    printf("\t3. stop_monitor para matar el monitor\n");
    printf("\t3. config_monitor para configurar tiempos del monitor\n");

    printf("\tIngrese un comando: \n ");
}

/**
 * SIGNAL HANDLERS
 */

/**
 * @brief signalHandler_child
 * signal handler for SIGCHLD
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
 *	Muestra por pantalla el promt de la shell
 */
void shellPrompt()
{
    char hostname[1024];
    char cwd[1024];

    // Get hostname
    if (gethostname(hostname, sizeof(hostname)) == -1)
    {
        perror("gethostname");
        exit(EXIT_FAILURE);
    }

    // Get current working directory
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }

    // colorization
    printf("\033[1;32m%s\033[0m@\033[1;34m%s\033[0m \033[1;36m%s\033[0m |--> ", getenv("USER"), hostname, cwd);
}

/**
 * @brief changeDirectory
 * metodo para manejar y cambiar de directorio
 */
int changeDirectory(char* args[])
{
    if (args[1] == NULL)
    {
        // Cambiar al directorio home
        if (chdir(getenv("HOME")) == -1)
        {
            fprintf(stderr, "Error al cambiar al directorio home: %s\n", strerror(errno));
            return -1;
        }
    }
    else
    {
        // Cambiar al directorio especificado
        if (chdir(args[1]) == -1)
        {
            switch (errno)
            {
            case ENOENT:
                fprintf(stderr, "El directorio '%s' no existe.\n", args[1]);
                break;
            case EACCES:
                fprintf(stderr, "No tienes permiso para acceder al directorio '%s'.\n", args[1]);
                break;
            default:
                perror(args[1]);
            }
            return -1;
        }
    }
    return 0;
}

/**
 * @brief manageEnviron
 * Administrar las variables de entorno con distintas opciones
 * si se ejecuta "environ" esto imprimira nuestras variables de entorno
 * si ejecuta "setenv": configurara nuestras variables de entorno a un valor
 * si se ejecuta "unsetenv": elimina una variable de entorno
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
 */

/**
 * Method used to manage I/O redirection
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
 * Manipulacion de pipelines
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
 * metodo para receptar y manejar los comandos.
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

    // 'exit' command quits the shell
    if (strcmp(args[0], "exit") == 0)
        exit(0);
    // 'pwd' command imprime el directorio actual
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
    else if (strcmp(args[0], "clear") == 0)
        system("clear");
    // 'cd' command cambia de directorio
    else if (strcmp(args[0], "cd") == 0)
        changeDirectory(args);
    // 'environ' command lista variables de entorno
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
    // 'setenv' command to set environment variables
    else if (strcmp(args[0], "setenv") == 0)
        manageEnviron(args, 1);
    // 'unsetenv' command to undefine environment variables
    else if (strcmp(args[0], "unsetenv") == 0)
        manageEnviron(args, 2);
    // 'start_monitor' command que inicia el monitor de sistema
    else if (strcmp(args[0], "start_monitor") == 0)
    {
        metric_pid = start_monitor();
        if (metric_pid <= 0)
        {
            printf("Error al iniciar el proceso\n");
            return 1;
        }
        printf("Proceso iniciado con PID: %d\n", metric_pid);
    }
    else if (strcmp(args[0], "status_monitor") == 0)
    {
        const char* status[] = {"Error: ID de proceso inválido o fallo en la verificación",
                                "Proceso terminó normalmente", "Proceso sigue ejecutándose",
                                "Proceso terminó por una señal", "Proceso está detenido"};

        // Verificar si el monitor está iniciado
        if (metric_pid <= 0)
        {
            printf("El monitor aún no se ha iniciado. Para iniciarlo, escriba el comando: start_monitor\n");
            return 1;
        }

        // Obtener el estado del proceso
        int status_code = check_process(metric_pid);

        // Validar el índice para evitar accesos fuera de rango
        if (status_code >= -1 && status_code <= 4)
        {
            printf("%s\n", status[status_code]); // Ajuste de índice para el array `status`
        }
        else
        {
            printf("Código de estado desconocido: %d\n", status_code);
        }
    }

    else if (strcmp(args[0], "stop_monitor") == 0)
    {
        // Detener
        if (stop_process(metric_pid) == 0)
        {
            printf("Proceso detenido correctamente\n");
        }
        else
        {
            printf("Error matando el proceso monitor :c");
        }
    }
    else if (strcmp(args[0], "config_process") == 0 ){
        signal(SIGALRM, timer_handler); // conecto la señal SIGALRM a los handlers correspondientes
        //signal(SIGALRM, timeout_handler);
        if(metric_pid > MIN_PS_ID && metric_pid < MAX_PS_ID){
            // Leer el archivo config.json
            char *json_content = read_json_file("config.json");
            if (json_content == NULL) {
                printf("Error al leer el archivo config.json\n");
                return -1;
            }
            // Procesar el contenido JSON
            if (process_control_json(json_content, metric_pid) != 0) {
                free(json_content);
                return -1;
            }

            // Liberar la memoria del contenido JSON
            free(json_content);

            printf("Configuración cargada exitosamente de config.json\n");
        }
        else{
            printf("El monitor aun no se ha iniciado, por favor, si desea manipular el monitor ejecute el comando start_monitor");
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
 * Main of shell
 */
int main(int argc, char* argv[], char** envp)
{
    cJSON* root = cJSON_CreateObject();
    char line[MAXLINE];  // buffer for the user input
    char* tokens[LIMIT]; // array for the different tokens in the command
    int numTokens;
    struct termios termios;

    // Habilitar el modo de línea de comandos y el procesamiento de secuencias de escape
    termios.c_lflag |= ICANON;
    termios.c_lflag |= ISIG;
    no_reprint_prmpt = 0; // to prevent the printing of the shell
                          // after certain methods
    pid = -10;            // we initialize pid to an pid that is not possible

    // initialization and the welcome screen
    init();
    welcomeScreen();
    // asigno las variables de entorno
    environ = envp;
    // Asigno al directorio actual la shell
    setenv("shell", getcwd(currentDirectory, 1024), 1);
    //Asigno la variable local LD_LIBRARY_PATH /usr/local/lib64/:$LD_LIBRARY_PATH
    /*
     * char envi[2] = {"LD_LIBRARY_PATH","/usr/local/lib64/:$LD_LIBRARY_PATH"} ;
    manageEnviron(envi, 1);
     */
    // Main loop, where the user input will be read and the prompt
    // will be printed
    // Configuración en modo no canónico antes del bucle principal
    //set_noncanonical_mode();

    while (TRUE) {
        // Imprimir el prompt si no hay necesidad de reimpresión
        if (no_reprint_prmpt == 0) {
            shellPrompt();
        }
        no_reprint_prmpt = 0;

        // Vaciado de la línea y captura del ingreso del usuario
        memset(line, '\0', MAXLINE);

        // Manejar teclas de control antes de leer el comando
        //handle_keypress();

        // Ingreso del usuario
        fgets(line, MAXLINE, stdin);

        // Si la línea está vacía, continúa el bucle
        if ((tokens[0] = strtok(line, " \n\t")) == NULL) {
            continue;
        }

        // Recorrer los comandos y dividirlos en tokens
        numTokens = 1;
        while ((tokens[numTokens] = strtok(NULL, " \n\t")) != NULL) {
            numTokens++;
        }

        // Llamada al manejador de comandos
        commandHandler(tokens);
    }

    // Restaurar modo original cuando la terminal cierre
    reset_terminal_mode();

    tcsetattr(STDIN_FILENO, TCSANOW, &termios);
    exit(0);
}
