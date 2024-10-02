#include <stdio.h>   // Librería estándar de entrada y salida
#include <stdlib.h>  // Librería para manejo de memoria dinámica, procesos, y otras utilidades
#include <unistd.h>  // Librería para hacer uso del sistema UNIX, como fork() y exec()
#include <wait.h>    // Librería para esperar la terminación de procesos hijo
#include <sys/types.h> // Librería para definir tipos de datos como pid_t (ID del proceso)
#include <string.h>  // Librería para manipulación de cadenas

// Función que se encarga de abrir un archivo en el modo especificado y verificar si se pudo abrir correctamente
FILE* abrir_archivo(const char* nombre_archivo, const char* modo) {
    FILE *archivo = fopen(nombre_archivo, modo);  // Abrir el archivo en el modo especificado
    if (archivo == NULL) {  // Si no se pudo abrir el archivo, muestra un error
        fprintf(stderr, "No se pudo abrir el archivo %s.\n", nombre_archivo);
        exit(1);  // Sale del programa con un código de error
    }
    return archivo;  // Retorna el puntero al archivo abierto
}

int main() {
    // Definimos los nombres de los archivos para almacenar la matriz y los resultados
    char *nombreArchivoMatriz = "matriz.txt";
    char *nombreArchivoResultados = "resultado.txt";

    // Eliminamos los archivos anteriores, si existen, para empezar desde cero
    remove(nombreArchivoMatriz);
    remove(nombreArchivoResultados);

    // Definimos la matriz que vamos a guardar en el archivo "matriz.txt"
    int matriz[3][9] = {
        {1, 2, 3, 4, 5, 6, 7, 8, 9},
        {1, 3, 5, 7, 9, 11, 13, 15, 17},
        {2, 4, 6, 8, 10, 12, 14, 16, 18}
    };

    // Calculamos el tamaño de la fila dinámicamente (cantidad de columnas)
    int tamano_fila = sizeof(matriz[0]) / sizeof(matriz[0][0]);  // Calcula cuántas columnas tiene la primera fila

    // Abrimos el archivo matriz.txt en modo escritura para almacenar la matriz
    FILE *archivo_matriz = abrir_archivo(nombreArchivoMatriz, "w");

    // Escribimos la matriz en el archivo de texto, con cada número separado por un espacio
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < tamano_fila; ++j) {
            fprintf(archivo_matriz, "%d ", matriz[i][j]);  // Escribimos el número en el archivo
        }
        fprintf(archivo_matriz, "\n");  // Al final de cada fila escribimos un salto de línea
    }

    // Cerramos el archivo matriz.txt después de escribir la matriz
    fclose(archivo_matriz);

    // El proceso padre crea el archivo donde se guardarán los resultados de las multiplicaciones
    FILE *archivo_resultados = abrir_archivo(nombreArchivoResultados, "w");
    printf("\nSoy el proceso padre y he generado el archivo para guardar los resultados de mis hijos\n\n");
    fclose(archivo_resultados);  // Cerramos el archivo para que luego los hijos lo usen

    // Proceso padre genera procesos hijos, según el tamaño dinámico de la fila
    for (int i = 0; i < tamano_fila; ++i) {  // Creamos un proceso hijo por cada columna de la matriz
        pid_t pid = fork();  // fork() crea un nuevo proceso hijo
        if (pid == -1) {
            // Si fork() falla, mostramos un error y terminamos el programa
            perror("Error al crear el proceso\n");
            exit(-1);  // Salir con código de error
        } else if (pid == 0) {
            // Código que ejecuta cada proceso hijo

            // Abrimos el archivo matriz.txt para leer los valores
            FILE *archivo_matriz_hijo = abrir_archivo(nombreArchivoMatriz, "r");

            int multiplicacion = 1;  // Variable para almacenar el resultado de la multiplicación de una columna
            int numero;  // Variable para almacenar el número leído del archivo
            int contador = 0;  // Contador para identificar la columna actual

            // Leemos el archivo número por número: El bucle while sigue ejecutándose mientras fscanf siga leyendo correctamente números enteros del archivo
            while (fscanf(archivo_matriz_hijo, "%d", &numero) == 1) {
                if (contador == tamano_fila) {  // Si hemos alcanzado el final de una fila (es decir, leído todos sus elementos)
                    contador = 0;  // Reiniciamos el contador a 0 para comenzar a procesar la siguiente fila
                }
                if (contador == i) {  // Si el contador indica que estamos en la columna correspondiente a este proceso hijo
                    multiplicacion *= numero;  // Multiplicamos el valor actual de la columna por el acumulado de la variable 'multiplicacion'
                }
                contador++;  // Incrementamos el contador para pasar al siguiente número en la fila
            }

            // Mostramos el resultado de la multiplicación en consola
            printf("\nMultiplicación columna %d es %d\n", i + 1, multiplicacion);

            // Cerramos el archivo de la matriz
            fclose(archivo_matriz_hijo);

            // Abrimos el archivo resultado.txt en modo adjuntar para escribir el resultado de este hijo
            FILE *archivo_resultados_hijo = abrir_archivo(nombreArchivoResultados, "a");

            // Escribimos el resultado de la multiplicación en el archivo
            fprintf(archivo_resultados_hijo, "Resultado columna %d: %d\n", i + 1, multiplicacion);

            // Cerramos el archivo resultados
            fclose(archivo_resultados_hijo);

            exit(0);  // Terminamos el proceso hijo
        }
    }

    // Esperamos a que todos los procesos hijos terminen
    for (int i = 0; i < tamano_fila; ++i) {
        wait(NULL);  // El proceso padre espera a cada hijo
    }

    // Indicamos que todos los hijos han terminado
    printf("\nTodos los hijos han terminado. Proceso padre finalizado.\n");

    return 0;  // Salida exitosa del programa
}
