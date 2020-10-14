/**
 * Autor - Rodrigo Quiroz Reyes & Esteban Manrique de Lara Sirvent
 * Fecha - 04/09/2020
 * Actividad individual 3: Administración de un SO 
 * Ejercicio 1 - Simulación de la candelarización de procesos en un CPU mediante un Round Robin
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "queue.h"

const int QUANTUM = 4; //Tiempo que pasa un proceso en el CPU

struct Proceso
{
    int id;
    int gang;
    int tiempo;
    int numeroHilos;
}; //Estructura que simboliza los componentes de un proceso acorde a el problema formulado

struct CPU
{
    int* cores;
};

int quantums=0; //Variable global para dar formato al ciclo de impresión de cores

int recopilarInformacion(struct Proceso*, int); //Metodo para recopilar toda la información de cada proceso mediante el input del usuario
void ordernarPorGrupos(struct Proceso*, int); //Reordena los procesos tomando como parámetro la familia considerandolá como una prioridad
void swap(struct Proceso*, struct Proceso*); //Función auxiliar para hacer un swap en el ordenamiento
void impresion(struct Proceso*, int); //Funcion para imprimir el arreglo de procesos
void roundRobin(struct Proceso*, int, int, struct CPU*); // Funcion que almacena el ciclo de candelarización de proceso
void print_cores(int*,int);// Impresión de lo que pasa dentro del array del CPU

int main()
{
    struct Proceso* procesos;
    struct CPU *cpu;
    cpu=(struct CPU*)malloc(sizeof(struct CPU));
    int numeroCPUs = 0;

    printf("%s\n", "Introducir numero de procesos deseados");
    int numeroProcesos = 0;
    scanf("%d", &numeroProcesos);

    procesos = (struct Proceso*)malloc(numeroProcesos * sizeof(struct Proceso));
    numeroCPUs = recopilarInformacion(procesos, numeroProcesos);
    cpu->cores=(int*)malloc(numeroCPUs*sizeof(int));
    
    ordernarPorGrupos(procesos, numeroProcesos);
    impresion(procesos, numeroProcesos);
    roundRobin(procesos, numeroProcesos, numeroCPUs,cpu);

    free(cpu->cores);
    free(cpu);
    free(procesos);
    return 0;
}

/**
 * Método para extraer toda la informacion de los procesos a simular a través
 * del input del usuario y regresa una variable para luego se usaba en el malloc
 * de la creación del array de CPUs
 * 
 * @param struct Proceso* - Array de procesos para hacer el llenado de datos
 * @param int numeroProcesos - Numeros de procesos declarados previamente 
 * 
 * @return int - Valor para inicializar el array de CPUs
 */
int recopilarInformacion(struct Proceso* procesos, int numeroProcesos)
{
    printf("%s\n", "Introducir numero de CPU's deseados");
    int numeroCPUs = 0;
    scanf("%d", &numeroCPUs); //Input del número de cores

    struct Proceso* aux; //Variable auxiliar para recorrer el arreglo de procesos
    int id = 1;
    int tiempo = 0;
    int gang = 0;
    int numeroHilos = 0;

    for(aux = procesos;aux<(procesos + numeroProcesos);++aux)
    {
        //Introducción de datos necesarios para los procesos
        printf("%s\n", "Introducir tiempo necesario de procesamiento.");
        scanf("%d", &tiempo);
        printf("%s\n", "Introducir grupo/gang a la que pertence el proceso.");
        scanf("%d", &gang);
        printf("%s\n", "Introducir numero de hilos con los que se requiere trabajar con este proceso.");
        scanf("%d", &numeroHilos);
        
        //Asignación de variables
        aux->gang = gang;
        aux->tiempo = tiempo;
        aux->id = id;
        aux->numeroHilos = numeroHilos;
        id++;
    }
    return numeroCPUs;
}

/**
 * Método para ordenar los procesos usando como parámetro la familia asignada
 * a cada estructura
 * 
 * @param struct Proceso* - Array de procesos para hacer el llenado de datos
 * @param int numeroProcesos - Numeros de procesos declarados previamente 
 * 
 */
void ordernarPorGrupos(struct Proceso* procesos, int numeroProcesos)
{
    //Declaración de variables auxiliares para el recorrido y realocación de valores
    struct Proceso* minimum;
    struct Proceso* aux1 = procesos;
    struct Proceso* aux2;
  
    //Uso del algoritmo de selectionSort para ordenar el arreglo
    for (;aux1<(procesos + (numeroProcesos - 1));aux1++) 
    { 
        minimum = aux1; 
        for (aux2 = aux1+1; aux2 < (procesos + numeroProcesos);aux2++) 
        {
            if (aux2->gang < minimum->gang)
            {
                minimum = aux2;
            }
        } 
        swap(aux1,minimum);
    } 
    return;
}   

/**
 * Método para hacer el swap entre 2 elementos del arreglo de procesos
 * 
 * @param struct Proceso1* - Elemento del arreglo para hacer el swap
 * @param struct Proceso2* - Elemento del arreglo para hacer el swap
 * 
 */
void swap(struct Proceso* proceso1, struct Proceso* proceso2)
{
    //Se asignan variables temporales a cada tipo de dato y componente de la estructura
    int idTemp, timeTemp, gangTemp, numeroHilosTemp;
    idTemp = proceso1->id;
    timeTemp = proceso1->tiempo;
    gangTemp = proceso1->gang;
    numeroHilosTemp = proceso1->numeroHilos;
     
    proceso1->id = proceso2->id;
    proceso1->tiempo = proceso2->tiempo;
    proceso1->gang = proceso2->gang;
    proceso1->numeroHilos = proceso2->numeroHilos;

    proceso2->id = idTemp;
    proceso2->tiempo = timeTemp;
    proceso2->gang = gangTemp; 
    proceso2->numeroHilos = numeroHilosTemp;
    return;
}

/**
 * Método para hacer la impresión en consola de los datos que contiene el
 * array de procesos
 * 
 * @param struct Proceso1* - Elemento del arreglo para hacer el swap
 * @param int numeroProcesos - Numeros de procesos declarados previamente 
 * 
 */
void impresion(struct Proceso* procesos, int numeroProcesos)
{
    struct Proceso* aux;
    struct Proceso* final=(procesos + numeroProcesos);
    for(aux = procesos; aux<final; ++aux)
    {
        printf("Proceso %d, tiempo %d, gang %d y numero de hilos %d.\n", aux->id, aux->tiempo, aux->gang, aux->numeroHilos);
        
    }

}

/**
 * Método para hacer la impresión en consola del array de cores y ver
 * qué proceso está siendo dando espacio en el CPU y simula mediante un
 * sleep el tiempo de ejecución. El -1 indica que ese core no está en uso
 * por un proceso
 * 
 * @param struct Proceso1* - Elemento del arreglo para hacer el swap
 * @param int numeroProcesos - Numeros de procesos declarados previamente 
 * 
 */
void print_cores(int* cores, int numeroCPU){
    
    printf("\n");
    int*auxCP=cores;
    printf("Q%d: ", quantums);
    
    for(;auxCP<(cores+numeroCPU);++auxCP){
        printf("|\t%d   ", *auxCP);
    }
    printf("|");
    printf("\n");
    return;
    
}

/**
 * Método para candelarizar los procesos mediante un ciclo
 * Para mantener un orden de los procesos se utiliza una estructura 
 * de datos tipo queue genérica que acepta cualquier tipo de dato y el
 * ciclo concluye cuando ya no hay procesos existentes
 * 
 * @param struct Proceso1 * - Elemento del arreglo para hacer el swap
 * @param int numeroProcesos - Numeros de procesos declarados previamente 
 * @param int numeroProcesos - Numeros de procesos declarados previamente 
 * @param struct CPU * - 
 * 
 */
void roundRobin(struct Proceso* procesos, int numeroProcesos, int numeroCPU, struct CPU* cpu)
{  
    //Variables auxiliares para el recorrido de los arreglos de procesos y Cores
    struct Proceso *auxP=procesos;
    int *auxC;
    int thread_needed=0;
    int auxNoP=numeroProcesos;

    //Inicialización para el uso de la estructura queue
    node_t *head = NULL;
    void* ret;

    //Encola todos los procesos en la queue
    for(;auxP<(procesos+numeroProcesos);++auxP){
        enqueue(&head, auxP);
    }

    //Obtiene el primer proceso de la cola
    auxP=procesos;
    ret=dequeue(&head);
    thread_needed=((struct Proceso*)ret)->numeroHilos;

    //Ciclo para la candelarización
    while (auxNoP>0 )
    {
    int i=0; //Variable que ayuda en el caso de que no se usen todos los cores
        for(auxC=cpu->cores;auxC<(cpu->cores+numeroCPU)&&ret!=NULL;++auxC,i++){  

            *auxC=((struct Proceso*)ret)->id; //Asignación de la id a un core
            
            ((struct Proceso*)ret)->tiempo=((struct Proceso*)ret)->tiempo-QUANTUM; //Se le resta un quantum al proceso
            --thread_needed;
            
            if(((struct Proceso*)ret)->tiempo<=0){ //Caso en que un proceso se acabe su tiempo de ejecución
                --auxNoP;
                ret=dequeue(&head); //Se remueve de la cola por completo
                
                if(ret!=NULL){//Condición para no causar un apuntdor a un NULL
                    thread_needed=((struct Proceso*)ret)->numeroHilos; //Se resetan los threads al siguiente proceso
                }
            } else if(thread_needed<=0)
            {  
                enqueue(&head,((struct Proceso*)ret));// Si es que todavia le falta tiempo para ejecutarse se vuelve a encolar
                ret=dequeue(&head); //Agarra el siguiente proceso
                thread_needed=((struct Proceso*)ret)->numeroHilos;//Se resetan los threads al siguiente proceso
            }

        }
        //Rellena los espacios vacios que hayan sobrado en los cores de procesamiento con -1 para indicar que no se esta utilizando
        if(i<numeroCPU){
            for (auxC=(cpu->cores)+i;auxC<(cpu->cores+numeroCPU);++auxC)
            {
                *auxC=-1; 
            }
        }

        ++quantums;//Incremento de la varible global para dar formato a los quantums
        sleep(QUANTUM/2); //Sleep para simular el tiempo de ejecución
        print_cores(cpu->cores,numeroCPU);
    }    
}