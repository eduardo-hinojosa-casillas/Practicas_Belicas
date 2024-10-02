#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<wait.h>
#include<sys/types.h>
#include<string.h>

int main()
{
   // Creamos las variables para tener los nombres de los archivos
   char *nombreArchivoMatriz = "matriz.txt";
   char *nombreArchivoResultados = "resultado.txt";
   
   ////////////////////////////////////////////////////////////////////////////////////////////////////
   
   //En el caso de que existan los archivos, lo que haremos será eliminarlos para que se realice desde 0
   
   // Eliminar el archivo si existe
   
   remove(nombreArchivoMatriz);
   remove(nombreArchivoResultados);
   
   //////////////////////////////////////  ALMACENAR EN ARCHIVO DE TEXTO LA MATRIZ  ///////////////////////////////////
   
   // Creamos la matriz 
   int matriz[3][9] = {{1,2,3,4,5,6,7,8,9},
   		       {1,3,5,7,9,11,13,15,17},
   		       {2,4,6,8,10,12,14,16,18}};
   
    // Abrimos un archivo en modo escritura para guardar la matriz
    FILE *archivo1 = fopen(nombreArchivoMatriz, "w");

    // Verificamos de que se abra correctamente el archivo
    if (archivo1 == NULL) {
        fprintf(stderr, "No se pudo abrir el archivo.\n");
        return 1;  // Salir del programa con código de error
    }

    // Escribimos la matriz en el archivo separado por un espacio
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 9; ++j) {
            fprintf(archivo1, "%d ", matriz[i][j]);
        }
        fprintf(archivo1, "\n"); // Al final de la primera fila escribimos un salto de linea
    }

    // Cerrar el archivo
    fclose(archivo1);
    
    ////////////////////////////////////  EL PROCESO PADRE GENERA EL ARCHIVO PARA GUARDAR RESULTADOS  ///////////////////////////
    
    // Abrimos un archivo en modo escritura para guardar los resultados de las multiplicaciones
    FILE *archivo2 = fopen(nombreArchivoResultados, "w");

    // Verificamos de que se abra correctamente el archivo
    if (archivo2 == NULL) {
	fprintf(stderr, "No se pudo abrir el archivo.\n");
	return 1;  // Salir del programa con código de error
    }
	    
    printf("\nSoy el proceso padre y he generado el archivo txt para guardar los datos de mis hijos\n\n");
    // Cerramos el archivo para que los hijos lo abran independientemente
    fclose(archivo2);
    
    //////////////////////////////////////  PROCESO PADRE GENERA A LOS PROCESOS HIJOS NECESARIOS  ///////////////////////

    // En este bucle generamos cada uno de los procesos hijos según el numero de columnas
    for (int i = 0; i < 9; ++i) {
    
        pid_t pid = fork(); // Creamos el proceso hijo
        
        // En este caso el iterador le indicara con la variable i al proceso hijo el numero de columna que debe de enfocarse

        if (pid == -1) {
            // Error a la hora de crear el proceso
            perror("\nError al crear el proceso\n");
      	    exit(-1);
        } else if (pid == 0) {
        
            //////////////////////////////////////  INSTRUCCIONES QUE REALIZA UN PROCESO HIJO //////////////////////////////
            
            
            // Abrimos el archivo de la matriz para leerlo
            FILE *archivo = fopen(nombreArchivoMatriz, "r");

            if (archivo == NULL) {
                fprintf(stderr, "No se pudo abrir el archivo.\n");
                return 1;
            }

            int multiplicacion = 1; // Variable para guardar el resultado de la multiplicacion
            int numero; // Variable para leer el numero del archivo
	    int contador = 0; // Variable que nos permitira saber el numero de la columna en la que estemos
	    
	    // Recorremos el archivo de texto mientras podamos leer un valor entero
	    while (fscanf(archivo, "%d", &numero) == 1){
	        if(contador == 9) // Si el contador es igual a 9, reinicia contador para el salto de linea
	            contador = 0;  // Igualamos a 0 contador
	        
	        if(contador == i) // Si el contador es igual al contador, entonces es el valor que esperamos
	            multiplicacion *= numero; // Multiplicamos el valor actual con la acumulacion

	        contador++; // Aumentamos el contador para saber que nos movemos a la siguiente columna de la matriz
	    }

            printf("\nMultiplicacion columna %d es %d\n", i + 1, multiplicacion); // Imprmimos el valor de la multiplicacion en consola
            fclose(archivo); // Cerramos el archivo de la matriz

	    // Abrimos el archivo de resultados creado por el proceso padre para agregarle elementos al final
            FILE *archivo_resultados = fopen(nombreArchivoResultados, "a");
            if (archivo_resultados == NULL) {
                fprintf(stderr, "No se pudo abrir el archivo de resultados.\n");
                return 1;
            }
            
            fprintf(archivo_resultados, "Resultado columna %d: %d\n", i + 1, multiplicacion); // Guardamos el hijo y el valor calculado
            fclose(archivo_resultados); // Cerramos el archivo de resultados

            exit(0); // Terminamos el proceso hijos
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////

    // Esperaramos a que los hijos terminen
    for (int i = 0; i < 9; ++i) {
        wait(NULL);
    }

    printf("\nTodos los hijos han terminado. Proceso padre finalizado.\n"); // Mostramos en consola que se han terminado todos los procesos hijos
}