#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

// Estructura para almacenar información sobre un proceso
struct ProcessInfo {
    char id[10];
    char name[100];
    int isKernel;
};

// Función para mostrar la información del proceso
void displayProcessInfo(const struct ProcessInfo *process) {
    printf("ID de proceso: %s, Nombre del proceso: %s, Tipo de proceso: %s\n",
           process->id, process->name, process->isKernel ? "Kernel" : "Usuario");
}

// Función para determinar si un proceso es de kernel o de usuario
int isKernelProcess(const char *username) {
    return (strcmp(username, "root") == 0);
}

// Función para obtener información sobre los procesos del sistema
void getProcessInfo() {
    DIR *dir = opendir("/proc");
    if (dir == NULL) {
        perror("Error al abrir el directorio /proc");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        // Verificar si el nombre del directorio es un número (ID de proceso)
        if (entry->d_type == DT_DIR && atoi(entry->d_name) != 0) {
            struct ProcessInfo process;
            strcpy(process.id, entry->d_name);

            // Leer el archivo 'comm' para obtener el nombre del proceso
            char commPath[256];
            sprintf(commPath, "/proc/%s/comm", entry->d_name);
            FILE *commFile = fopen(commPath, "r");
            if (commFile != NULL) {
                fscanf(commFile, "%s", process.name);
                fclose(commFile);
            } else {
                perror("Error al abrir el archivo 'comm'");
                exit(EXIT_FAILURE);
            }

            // Leer el archivo 'status' para obtener el nombre de usuario y determinar el tipo de proceso
            char statusPath[256];
            sprintf(statusPath, "/proc/%s/status", entry->d_name);
            FILE *statusFile = fopen(statusPath, "r");
            if (statusFile != NULL) {
                char username[100];
                while (fscanf(statusFile, "%*s %s", username) == 1) {
                    if (strcmp(username, "Uid:") == 0) {
                        fscanf(statusFile, "%s", username);
                        process.isKernel = isKernelProcess(username);
                        break;
                    }
                }
                fclose(statusFile);
            } else {
                perror("Error al abrir el archivo 'status'");
                exit(EXIT_FAILURE);
            }

            // Mostrar la información del proceso
            displayProcessInfo(&process);
        }
    }

    closedir(dir);
}

int main() {
    printf("Procesos activos del sistema:\n");
    getProcessInfo();
    return 0;
}

