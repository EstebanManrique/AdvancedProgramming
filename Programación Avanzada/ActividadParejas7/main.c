/**
 * Autores - Esteban Manrique de Lara Sirvent y Rodrigo Quiroz Reyes
 * Fecha - 08/10/2020
 * Actividad 7: Conjuntos de senales
*/

#include <stdio.h>
#include <signal.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

struct InfoArchivos //Estructura usada para almacenar informacion de los archivos generados
{
    char* ruta;
    char* nombre;
    long int tam;
};

void bloquearSignals();
void obtenerValoresConsola(int, char* const*);
void verificarSubdirectorio(char*, DIR*);
void eliminacionArchviosSubDirectorio(char*, DIR*);
void sig_alarm_handler(int);
void generacionArchivos(char*, struct InfoArchivos*);

int numeroArchivos = 0; 
int segundosTemporizador = 0;
int grabar = 1; //Variable global que determina si hay escritura en archivo o no
sigset_t signals; //Conjunto de variables que solamente EXCLUYEN a SIGALRM
int fileGlobal = 0; //Variable global que permite identificar archivo en donde se debe escribir
sigset_t pendientes; //Conjunto de variables pendientes

int main(int argc, char* const* argv)
{
    char* directorioActualRuta;
    DIR* dir;
    directorioActualRuta = (char*)malloc(1000*sizeof(char)); //Se almacenara path actual donde se este ejecutando el programa

    obtenerValoresConsola(argc, argv);//Obtencion de numero Archivos y segundos temporizador

    //Creacion de estructuras para archivos y alocacion de memoria para los mismos
    struct InfoArchivos* archivos;
    archivos = (struct InfoArchivos*)malloc(numeroArchivos * sizeof(struct InfoArchivos));
    struct InfoArchivos* aux = archivos;
    for(int i = 0; i<numeroArchivos; i++)
    {
        aux->nombre = (char*)malloc(10 * sizeof(char));
        aux->ruta = (char*)malloc(500 * sizeof(char));
        aux++;
    }
    archivos = realloc(archivos, numeroArchivos * sizeof(struct InfoArchivos));

    //Definicion conjunto signals y bloqueo 
    bloquearSignals();
    
    if(numeroArchivos <= 0 || segundosTemporizador <= 0) //En caso de tener valores de CERO o MENOS, se cierra programa
    {
        printf("Valores introducidos para numero Archivos o segundos Temporizador fueron invalidos.\n");
        free(directorioActualRuta);
        closedir(dir);
        return(-1);
    }
   
    printf("Numero archivos a crear: %d\n", numeroArchivos);
    printf("Segundos asignados a temporizador: %d\n", segundosTemporizador);

    //Verificacion de Subdirectorio "datos"
    verificarSubdirectorio(directorioActualRuta, dir);

    //Handler para SIGALRM
    signal(SIGALRM, sig_alarm_handler);

    //Generacion de archivos e invocacion de SIGALRM
    generacionArchivos(directorioActualRuta, archivos);

    aux = archivos;
    printf("%17s %20s\n", "Nombre del archivo", "Tam. Archivo (Bytes)");

    //Impresion de informacion de archivos, liberacion de memoria de arreglos dentro de estructura de arhcivos
    for(int i = 0; i<numeroArchivos; i++)
    {
        printf("%10s %25ld\n", aux->nombre, aux->tam);
        free(aux->nombre);
        free(aux->ruta); 
        aux++;
    }
    
    //Liberacion de memoria y cierre de archivo
    free(directorioActualRuta);
    free(archivos);
    closedir(dir);
    return 0;
}


/**
 * Funcion encargada de establecer el bloqueo de todas las signals que lleguen a ser recibidas por programa
 * excepto SIGALRM. Se establece conjunto de signals y se usar SIG_BLOCK
 *  
 **/
void bloquearSignals()
{
    sigfillset(&signals);
    sigdelset(&signals, SIGALRM);
    sigprocmask(SIG_BLOCK, &signals, NULL);
    return;
}


/**
 * Funcion encargada de guardar en variables del programa el numero de archivos y tiempo para cada uno, definidos por entrada
 * de teclado en consola por parte del usuario
 * 
 * @param numeroArgumentos, el cual contiene el numero de elementos introducidos en consola por el usuario
 * @param argumentos, el cual contiene los argumentos introducidos por el usuario en consola
 * 
 **/
void obtenerValoresConsola(int numeroArgumentos, char* const* argumentos)
{
    int aux;
    int auxValidacion;
    while((aux= getopt(numeroArgumentos, argumentos, "n:t:")) != -1)
    {
        auxValidacion = (atoi(optarg) == 0) ? 1  : (log10((atoi(optarg))) + 1); //Validacion de que valor introducido por usuario no contenga caracteres ajenos a los numericos
        if(auxValidacion == strlen(optarg) && atoi(optarg)>0 && strstr(optarg, ".") == NULL) //Se verifica que valor sea mayor a CERO, No sea decimal
        {
            switch(aux)
            {
            case 'n':
                numeroArchivos = atoi(optarg); //Numero de archvios se establece
                break;
            case 't':
                segundosTemporizador = atoi(optarg); //Numero de segundos para cada SIGALRM es establecido
                break;
            case '?':
                if (optopt == 'n')
                {
                    fprintf (stderr, "Opción -%c requiere un argumento. Introducir -n NUMERO DE PROCESOS -t SEGUNDOS TEMPORTIZADOR (ej. -n 4 -t 25)\n", optopt); //Se imprime una pequena ayuda para el usuario
                }
                else if (isprint (optopt))
                {
                    fprintf (stderr, "Opción desconocida.'-%c'. Introducir -n NUMERO DE PROCESOS -t SEGUNDOS TEMPORTIZADOR (ej. -n 4 -t 25) \n", optopt); //Se imprime una pequena ayuda para el usuario
                }
                else
                {
                    fprintf (stderr, "Opción desconocida.'\\x%x'. Introducir -n NUMERO DE PROCESOS -t SEGUNDOS TEMPORTIZADOR (ej. -n 4 -t 25) \n",optopt); //Se imprime una pequena ayuda para el usuario
                }
                break;
            default:
                break;
            }
        }
        else
        {
            printf("La opcion introducida era MENOR o IGUAL a ZERO o NO ERA un entero.\n"); //Entradas invalidas, se acaba metodo y posteriormente programa
            return;
        }
    }
    return;
}

/**
 * Funcion encargada de verificar la existencia o falta de la misma del directorio "datos" en el directorio en el que se cuentre
 * ejecutandose este programa. En caso de NO existir, crea el directorio. En caso ontrario, borra todo contenido previo en el directorio "datos"
 * 
 * @param directorioActual, contiene la ruta del directorio desde donde se esta ejecutando el programa
 * @param dir, estructura auxiliar a ser utilizada en la busqueda del directorio datos
 * 
 **/
void verificarSubdirectorio(char* directorioActualRuta, DIR* dir)
{
    char* rutaCompletaArchivo;
    getcwd(directorioActualRuta, 1000);
    dir = opendir(directorioActualRuta);
    struct dirent* entrada;
    struct stat estadistica; 
    while((entrada = readdir(dir)))
    {
        rutaCompletaArchivo = (char*)malloc(512 * sizeof(char));
        sprintf(rutaCompletaArchivo, "%s/%s", directorioActualRuta, entrada->d_name);
        stat(rutaCompletaArchivo, &estadistica);
        if(S_ISDIR(estadistica.st_mode)) //En caso de ser Directorio
        {
            if(strcmp(entrada->d_name, "datos") != 0)
            {
                continue;
            }
            else
            {
                printf("Si existe, hay que borrar todos los archivos \n");
                eliminacionArchviosSubDirectorio(rutaCompletaArchivo, dir);
                memcpy(directorioActualRuta, rutaCompletaArchivo, 1000);
                free(rutaCompletaArchivo);
                return;
            }
        }
        else
        {
            continue;
        }
    }
    printf("No existe el directorio, hay que crearlo.\n");
    mkdir("datos", 0777);
    getcwd(directorioActualRuta, 1000);
    sprintf(directorioActualRuta, "%s%s" ,directorioActualRuta ,"/datos");
    free(rutaCompletaArchivo);
    return;
}

/**
 * Funcion llamada desde verificarSubdirectorio(), la cual elimina todos los contenidos del subdirectorio "datos" en caso
 * de ya existir en el directorio desde donde de este ejecutando el programa
 * 
 * @param directorioActual, contiene la ruta del directorio desde donde se esta ejecutando el programa
 * @param dir, estructura auxiliar a ser utilizada en la busqueda del directorio datos
 * 
 **/
void eliminacionArchviosSubDirectorio(char* directorioActualRuta, DIR* dir)
{
    char* rutaCompletaArchivos;
    dir = opendir(directorioActualRuta);
    struct dirent* entrada;
    struct stat estadistica; 
    while((entrada = readdir(dir)))
    {
        rutaCompletaArchivos = (char*)malloc(512 * sizeof(char));
        sprintf(rutaCompletaArchivos, "%s/%s", directorioActualRuta, entrada->d_name);
        stat(rutaCompletaArchivos, &estadistica);
        if (strcmp(entrada->d_name, ".") != 0 && strcmp(entrada->d_name, "..") != 0 )
        {
            remove(rutaCompletaArchivos);
        }
    }
    free(rutaCompletaArchivos);
    return;
}

/**
 * Funcion encargada de manejar la llamada de la senal SIGALRM. Modifica la variable global grabar, escribe en archivo las signals
 * que se han activado
 * 
 * @param sig, senal recibida por handler
 * 
 **/
void sig_alarm_handler(int signall)
{
    if(grabar == 1)
    {
        for(int i = 1; i<NSIG; i++)
        {
            if(sigismember(&pendientes, i) && i != 14) //El 14 es de SIGALRM
            {
                char* nombreSignal = (char*)malloc(50 * sizeof(char));
                const char* nombreSignal2 = sys_siglist[i];
                sprintf(nombreSignal, "%s %s%d", nombreSignal2, "Signal numero: ", i);
                write(fileGlobal, "\n", 1);
                write(fileGlobal, nombreSignal, 80);
                write(fileGlobal, "\n", 1);
                free(nombreSignal);
            }
        }
        grabar = 0;
        return;
    }
    else
    {
        grabar = 1;
        close(fileGlobal);
        return;
    }
}

/**
 * Funcion encargada de ir generando archivos, activar el SIGARLM y escribir en archvios el caracter x mientras la
 * variable grabar lo permita. Tambien va almacenando los datos de cada archivo para luego imprimirlos
 * 
 * @param directorioActual, contiene la ruta del directorio desde donde se esta ejecutando el programa
 * @param archvios, arreglo en donde se guada informacion de cada archivo
 * 
 **/
void generacionArchivos(char* directorioActualRuta, struct InfoArchivos* archvios)
{
    struct InfoArchivos* aux = archvios;
    DIR* dir;
    dir = opendir(directorioActualRuta);
    for(int i = 0; i<numeroArchivos; i++)
    {
        printf("Introducir signals a aparecer en archivo %d.\n\n", i);
        sprintf(aux->nombre, "a%d.txt", i);
        sprintf(aux->ruta, "%s/%s", directorioActualRuta, aux->nombre);
        int file = creat(aux->ruta, S_IRWXU);
        fileGlobal = file;
        open(aux->ruta, O_RDWR);
        alarm(segundosTemporizador);
        grabar = 1;
        while(grabar==1)
        {
            sigpending(&pendientes);
            write(file, "x", sizeof(char));
            usleep(5);
        }
        alarm(2);
        struct stat estadistica;
        stat(aux->ruta, &estadistica);
        aux->tam = estadistica.st_size; 
        aux++;
    }
    return;
}