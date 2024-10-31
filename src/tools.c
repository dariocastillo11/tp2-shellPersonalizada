//
// Created by DARIO on 27/10/24.
//
#include "tools.h"
#include "util.h"
struct termios original_mode;
ProcessControl process = {0};

void launchProg(char** args, int background)
{
    int pid;

    if (args[0] == NULL)
    {
        fprintf(stderr, "Error: No se ha especificado un comando.\n");
        return;
    }
    if ((pid = fork()) == -1)
    {
        perror("Error al crear el proceso hijo");
        return;
    }
    if (pid == 0)
    {
        // Proceso hijo
        signal(SIGINT, SIG_IGN);
        if (setenv("parent", getcwd(currentDirectory, 1024), 1) == -1)
        {
            perror("Error al establecer la variable de entorno 'parent'");
            exit(EXIT_FAILURE);
        }

        if (execlp(args[0], args[0], args[1], args[2], NULL) == -1)
        {
            perror("Error al ejecutar el comando");
            exit(EXIT_FAILURE);
        }

        // Si llegamos aquí, hubo un error
        exit(EXIT_FAILURE);
    }

    // Proceso padre
    if (background == 0)
    {
        // Esperar a que el hijo termine
        waitpid(pid, NULL, 0);
    }
    else
    {
        // Proceso en segundo plano
        printf("Proceso creado con PID: %d\n", pid);
    }
}
int check_process(pid_t pid)
{
    if (pid <= 0)
        return -1; // ID de proceso inválido

    int status;
    pid_t result = waitpid(pid, &status, WNOHANG);

    if (result == 0)
    {
        return 2; // Proceso sigue ejecutándose
    }
    else if (result == pid)
    {
        if (WIFEXITED(status))
        {
            return 1; // Proceso terminó normalmente
        }
        else if (WIFSIGNALED(status))
        {
            return 3; // Proceso terminó debido a una señal
        }
        else if (WIFSTOPPED(status))
        {
            return 4; // Proceso está detenido
        }
    }
    return 0; // Error en `waitpid` o proceso no encontrado
}
pid_t start_monitor(void)
{
    char current_path[MAX_PATH];
    char binary_path[MAX_PATH];

    // Obtener el directorio actual
    if (getcwd(current_path, sizeof(current_path)) == NULL)
    {
        fprintf(stderr, "Error obteniendo directorio actual: %s\n", strerror(errno));
        return -1;
    }
    // Construir la ruta completa al binario
    snprintf(binary_path, sizeof(binary_path), "%s/%s", current_path, METRIC_RELATIVE_PATH);

    // Verificar si el archivo existe y es ejecutable
    if (access(binary_path, X_OK) == -1)
    {
        fprintf(stderr, "Error: El binario no existe o no es ejecutable: %s\n", strerror(errno));
        return -1;
    }
    pid_t pid_m = fork();
    if (pid_m < 0)
    {
        // Error en el fork
        fprintf(stderr, "Error al crear proceso hijo: %s\n", strerror(errno));
        return -1;
    }

    if (pid_m == 0)
    {
        // Proceso hijo
        // Crear una nueva sesión
        if (setsid() < 0)
        {
            fprintf(stderr, "Error al crear nueva sesión: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        // ejecuto el binario con seguridad
        execl(binary_path, binary_path, NULL);
        // Si execl retorna, hubo un error
        fprintf(stderr, "Error al ejecutar el binario: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return pid_m;
}
int stop_process(pid_t pid)
{
    if (pid <= 0)
        return -1;

    if (kill(pid, SIGTERM) == 0)
    {
        int status;
        // Esperar hasta 500ms para que termine
        for (int i = 0; i < 5; i++)
        {
            if (waitpid(pid, &status, WNOHANG) != 0)
            {
                return 0;
            }
            usleep(100000); // 100ms
        }
        // Si no terminó, usar SIGKILL
        kill(pid, SIGKILL);
        waitpid(pid, &status, 0);
        return 0;
    }

    return -1;
}

void set_noncanonical_mode(void) {
    struct termios new_mode;
    tcgetattr(STDIN_FILENO, &original_mode);
    atexit(reset_terminal_mode);  // Reset terminal al salir
    new_mode = original_mode;
    new_mode.c_lflag &= ~(ICANON | ECHO);  // Desactiva el modo canónico y el eco de la terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &new_mode);
}

void reset_terminal_mode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_mode);
}

void handle_keypress() {
    char ch;
    while (read(STDIN_FILENO, &ch, 1) == 1) {
        if (ch == 27) {  // Primer caracter de una secuencia de escape
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) == 1 && read(STDIN_FILENO, &seq[1], 1) == 1) {
                if (seq[0] == '[') {
                    switch (seq[1]) {
                    case 'A': printf("Flecha Arriba\n"); break;
                    case 'B': printf("Flecha Abajo\n"); break;
                    case 'C': printf("Flecha Derecha\n"); break;
                    case 'D': printf("Flecha Izquierda\n"); break;
                    default: break;
                    }
                }
            }
        } else {
            printf("Tecla presionada: %c\n", ch);
        }
    }
}

// Manejador para el timeout
void timeout_handler(int sig) {
    if (process.pid > 0) {
        kill(process.pid, SIGTERM);
        //printf("Proceso terminado por timeout \n");
    }
}
// Manejador para el temporizador
void timer_handler(int sig) {
    if (process.is_running) {
        kill(process.pid, SIGSTOP);
        process.is_running = 0;
    } else {
        kill(process.pid, SIGCONT);
        process.is_running = 1;
    }
}

// Configurar temporizador
void setup_timer(int period) {
    struct sigevent sev;/* Structure to transport application-defined values with signals.  */
    struct itimerspec its;/* POSIX.1b structure for timer start values and intervals.  */

    // Configurar el evento del temporizador
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGALRM;
    sev.sigev_value.sival_ptr = &process.timer_id;

    // Crear el temporizador
    timer_create(CLOCK_REALTIME, &sev, &process.timer_id);

    // Configurar el intervalo del temporizador
    its.it_value.tv_sec = period;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = period;
    its.it_interval.tv_nsec = 0;

    timer_settime(process.timer_id, 0, &its, NULL);
}

// Función para procesar el JSON y controlar el proceso
int process_control_json(const char *json_string, int monitor_pid) {
    cJSON *root = cJSON_Parse(json_string);
    if (!root) {
        printf("Error al parsear JSON\n");
        return -1;
    }

    // Obtener PID del proceso
    cJSON *pid_json = cJSON_GetObjectItem(root, "pid");// por seguridad, solo puedo controlar el funcionamiento del monitor
    if (pid_json && pid_json->type == cJSON_Number) {
        process.pid = monitor_pid ; //pid_json->valueint; esta linea se debe incorporar si se desea ampliar el control de procesos
    }

    // Obtener acción a realizar
    cJSON *action = cJSON_GetObjectItem(root, "action");
    if (action && action->type == cJSON_String) {
        if (strcmp(action->valuestring, "start") == 0) {
            kill(process.pid, SIGCONT);
            process.is_running = 1;
        }
        else if (strcmp(action->valuestring, "stop") == 0) {
            kill(process.pid, SIGSTOP);
            process.is_running = 0;
        }
        else if (strcmp(action->valuestring, "terminate") == 0) {
            kill(process.pid, SIGTERM);
            process.is_running = 0;
        }
    }

    // Configurar temporización si está especificada
    cJSON *period = cJSON_GetObjectItem(root, "period");
    if (period && period->type == cJSON_Number) {
        process.period = period->valueint;
        setup_timer(process.period);
    }

    // Configurar timeout si está especificado
    cJSON *timeout = cJSON_GetObjectItem(root, "timeout");
    if (timeout && timeout->type == cJSON_Number) {
        process.timeout = timeout->valueint;
        // Configurar temporizador para terminar el proceso
        alarm(process.timeout);
    }

    cJSON_Delete(root);
    return 0;
}



// Función para leer archivo JSON
char* read_json_file(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error al abrir el archivo %s\n", filename); // comprueba que el archivo sea legible
        return NULL;
    }

    // Obtener el tamaño del archivo
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Asignar memoria para el contenido
    char *buffer = (char*)malloc(file_size + 1);
    if (buffer == NULL) {
        fclose(file);
        printf("Error de asignación de memoria\n");
        return NULL;
    }

    // Leer el archivo
    size_t read_size = fread(buffer, 1, file_size, file);
    buffer[read_size] = '\0';

    fclose(file);
    return buffer;
}
