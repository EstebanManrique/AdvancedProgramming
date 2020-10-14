/**
 * Autores - Esteban Manrique de Lara Sirvent y Rodrigo Quiroz Reyes
 * Fecha - 03/09/2020
 * Actividad 3: Administracion de un SO
 * Ejercicio 2
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int clasificarArchivos(int*, char*, int, int*);
void impresion(int*, int);

int numeroArcas = 25000; //Limite maximo de arcas a considerar
int mayorPresencia = 0; //Permite ver si se requiere un sistema de proporcion para numero de asteriscos en histograma

int main()
{
    DIR *dir; //Estructura DIR para apertura y cierre y directorios
    int* sizesArchivos; //Se va guardando en este arreglo la cantidad de archivos en cada una de las arcas que se requieran
    char* nombre; //Nombre del directorio desde donde parte el programa
    int sizeArcas; //Longitud de cada una de las arcas a ser impresas en el histograma
    int *maximo;
    maximo = (int*)malloc(1 * sizeof(int));

    nombre = (char*)malloc(512 * sizeof(char));
    printf("Introduzca el path de donde quiere empezar el recorrido.\n");
    scanf(" %[^\n]", nombre);
    dir = opendir(nombre);
    if (dir==NULL) //Path inexistente o mal escrito; termina con el programa de manera inmediata
    {
        printf("Path invalido, posiblemente se escribio erroneamente.\n");
        closedir(dir);
        return 1;
    }
    do
    {
        printf("Introduzca el tamano en BYTES de cada una de las arcas del histograma.\n"); //Numero de BYTES debe ser mayor a ZERO
        scanf(" %d", &sizeArcas);
    }while(sizeArcas<=0);

    sizesArchivos = (int*)malloc(numeroArcas * sizeof(int));
    int* aux;
    aux = sizesArchivos;
    for(; aux<(sizesArchivos + numeroArcas); aux++) //Ayuda a que no haya probremas con los apuntadores y sus valores al ir contabilizando los archivos
    {
        *aux = 0;
    }

    numeroArcas = clasificarArchivos(sizesArchivos, nombre, sizeArcas, maximo); //Se define el numero de arcas a utilizar
    
    if(numeroArcas<=0) //Hay Directorios que arrojan un numero de arcas 0. Esto evita eso mediante el conocimiento del tamano de arca y del archivo mas grande
    {
        numeroArcas = ((*maximo)/sizeArcas) + 1;
    }
    
    sizesArchivos = realloc(sizesArchivos, numeroArcas * sizeof(int)); //Realocacion de memoria correspondiente
    impresion(sizesArchivos, sizeArcas); //Impresion de histograma

    free(sizesArchivos); //Libera arreglo con contadores de cada arca
    free(nombre); //Libera arreglo donde se guardaba path desde donde inicia el programa
    free(maximo);
    closedir(dir); //Cierra el DIR abierto para el manejo del programa
    return 0;
}

/**
 * Funcion recursiva que se encarga de ir recorriendo arbol de archivos, abrir carpetas subsecuentes,
 * identificar archivos e ir contabilizando dependiendo del tamano del mismo.
 * 
 * @param archivos, el cual es el arreglo donde se lleva la contabilidad de archivos por arca
 * @param path, el cual es el camino desde donde se inicia el recorrido del programa
 * @param sizeArca, el cual contiene el tamano de cada arca a ser manejada e impresa posteriormente en el histograma
 * 
 **/ 
int clasificarArchivos(int* archivos, char* path, int sizeArca, int* maximo)
{
    DIR* directorio;
    struct dirent* entrada; //Estructura utilizada para obtener nombre de archivo. Ayuda a que no se haga una ciclo infinito con . y ..
    struct stat estadistica; //Estructura que ayuda a la obtencion de datos como el tamano y tipo de cada elemento siendo recorrido en el arbol de archivos
    directorio = opendir(path);
    int ultimaArca = 0; //Se guarda el numero de arcas a utilizar para la impresion
    while((entrada = readdir(directorio)))
    {
        int* aux;
        aux = archivos;
        char* nomreCompleto;
        nomreCompleto = (char*)malloc(512 * sizeof(char));

        sprintf(nomreCompleto, "%s/%s", path, entrada->d_name); //Concatena y prepara la ruta de cada archivo a ser analizada por la funcion
        stat(nomreCompleto, &estadistica); //Con ayuda de esto, se puede posteriormente sacar tipo y tamano de cada archivo
        if(S_ISDIR(estadistica.st_mode)) //En caso de ser Directorio
        {
            if (strcmp(entrada->d_name, ".") != 0 && strcmp(entrada->d_name, "..") != 0 ) // to not infinite loop
            {
                clasificarArchivos(archivos, nomreCompleto, sizeArca, maximo); //Recursividad de la funcion
            }
        }
        else if(S_ISREG(estadistica.st_mode)) //En caso de ser Archivo Regular
        {
            int i = 1;
            int bandera = 0;
            for(; aux<(archivos + (numeroArcas)); aux++) //Recorrido de arreglo con contadores para ver en que urna se contabiliza dicho archivo
            {
                if(estadistica.st_size < ((sizeArca * i) - 1))
                {
                    if(estadistica.st_size>*maximo)
                    {
                        *(maximo) = estadistica.st_size;
                    }
                    *aux = *aux + 1;
                    *(archivos + (i-1)) = *aux;
                    if(*aux>mayorPresencia) 
                    {
                        mayorPresencia = *aux; //Si mediante este incremento, la variable llega a sobrepasar los 150, se adopta un modo de proporcionalidad con los *
                    }
                    if(ultimaArca < i) //Aactualiza numero de urnas necesarias para la impresion de histograma
                    {
                        ultimaArca = i;
                    }
                    bandera = 1;
                    break;
                }
                i++;
            }
            if(bandera==0) //Dificultad para analizar archivo
            {
                printf("Archivo presenta algun detalle que evitar que pueda ser leido o su tamano sobrepasa el limite pactado por los desarrolladores\n");
            }
        }
        free(nomreCompleto);
    }
    closedir(directorio);
    return ultimaArca;
}

void impresion(int* arreglo, int sizeArcas)
{
    int* aux = arreglo;
    int i = 1;
    printf("%17s %20s %13s\n", "Lim. inf   Lim. sup", "Num. Arch", "Histograma");
    for(; aux<(arreglo + numeroArcas); aux++)
    {
        int numeroAsteriscosImpresion = *aux;
        printf("%9d  %9d   %15d        ", (i-1) * sizeArcas, (i * sizeArcas) -1, *aux);
        if((mayorPresencia)>100)
        {
            numeroAsteriscosImpresion = (*aux)/10;  
        }
        for (int count = 0; count < numeroAsteriscosImpresion; count++)
        {
            printf("*");
        }
        printf("\n");
        i++;
    }
}