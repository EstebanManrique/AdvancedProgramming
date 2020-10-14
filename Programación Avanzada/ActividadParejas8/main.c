#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "queue.h"

#define CAJEROS_NORMALES  5
#define CAJEROS_EMPRESARIALES  3
#define CLIENTES_NORMALES  100
#define CLIENTES_EMPRESARIALES 50

struct Cajero //Estructura usada para modelar a Cajeros Normales y Empresariales
{
    char* ID;
    char* tipoCajero;
    int numeroClientesAtendidos;
    int atendidosTotales;
};

struct Cliente //Estructura uasada para modelar a Clientes Normales y Empresariales
{
    char* ID;
    int tipoCliente;
};

//Variables para llevar control de clientes Normales y Empresariales atendidos
int atendidos = 0;
int atendidosEm = 0;

//Mutexes y variables de condicion para bloquear hilos cuando es necesario
pthread_mutex_t mutexFilaNormal = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexFilaEmpresariales = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t consumeNormal_t = PTHREAD_COND_INITIALIZER;
pthread_cond_t produceNormal_t = PTHREAD_COND_INITIALIZER;
pthread_cond_t consumeEmpresarial_t = PTHREAD_COND_INITIALIZER;
pthread_cond_t produceEmpresarial_t = PTHREAD_COND_INITIALIZER;

//Inicios de Filas para clientes Normales y Empresariales
node_t* inicioColaNormal = NULL;
node_t* inicioColaEmpresarial = NULL;

void* filaBancoNormal(void*);
void* filaBancoEmpresarial(void*);
void* cajeroNormal(void*);
void* cajeroEmpresarial(void*);
struct Cajero* crearCajero(char*, int);
struct Cliente* crearCliente(int, int);
void descansarCajero(struct Cajero*);
void atenderCliente(struct Cajero*, struct Cliente*);
struct Cliente* asignarCliente(void*, struct Cajero*);

int main(int args, char* const* argv)
{
    srand(time(0));
    
    //Creacion Hilos Cajeros
    pthread_t* hilosCajeros = (pthread_t *) malloc((CAJEROS_NORMALES + CAJEROS_EMPRESARIALES) * sizeof(pthread_t));
    pthread_t* auxCajeros = hilosCajeros;
    int j = 0;
    for(int i = 0; i<(CAJEROS_NORMALES + CAJEROS_EMPRESARIALES); i++)
    {
        if(i<CAJEROS_NORMALES)
        {
            pthread_create(auxCajeros, NULL, cajeroNormal, (void*) i);
        }
        else
        {
            pthread_create(auxCajeros, NULL, cajeroEmpresarial, (void*) j);
            j++;
        }
        auxCajeros++;
    }

    //Creacion Hilos Colas
    pthread_t* hilosColas = (pthread_t *) malloc(2 * sizeof(pthread_t));
    pthread_t* auxHilos = hilosColas;
    pthread_create(auxHilos, NULL, filaBancoNormal, (void*) 0);
    auxHilos++;
    pthread_create(auxHilos, NULL, filaBancoEmpresarial, (void*) 0);
    
    //Join Hilos Colas
    auxHilos = hilosColas;
    pthread_join(*auxHilos, NULL);
    auxHilos++;
    pthread_join(*auxHilos, NULL);
    
    //Join Hilos Cajeros
    auxCajeros = hilosCajeros;
    for(int i = 0; i<(CAJEROS_NORMALES + CAJEROS_EMPRESARIALES); i++)
    {
        pthread_join(*auxCajeros, NULL);
        auxCajeros++;
    }

    printf("Clientes Normales atendidos: %d --- Clientes Empresariales atendidos: %d. \n", atendidos, atendidosEm);
    
    //Liberacion memoria arreglos de hilos
    free(hilosCajeros);
    free(hilosColas);
    return 0;
}

/**
 * Funcion encargada de simular el comportamiento de cada uno de los cajeros Normales en el programa.
 * Revisa variables globales, checa si hay cliente por atender, descanda en caso de ser necesario y 
 * actualiza numero de clientes atendidos
 * 
 * @param arg, usado para generar el ID del Cajero
 * 
 **/
void* cajeroNormal(void* arg)
{
    struct Cajero* cajero = crearCajero("Normal", (int)arg);
    while(atendidos < CLIENTES_NORMALES) //Primera linea de defensa. Se corrobora si todavia hay clientes por atender
    {
        usleep(rand() % 1000);

        //Se actualiza numero de clientes atendidos hasta el momento
        pthread_mutex_lock(&mutexFilaNormal);
        int aten = atendidos;
        int atenEmp = atendidosEm;
        pthread_mutex_unlock(&mutexFilaNormal);
        double porcentajeParticipacion = 0.0;
        struct Cliente* cliente;
        void* clienteAAsignar = NULL;
        if(aten < CLIENTES_NORMALES) //Segundo filtro; con numero de clientes atendidos actualizados, se vuelve a verificar si aun hay clientes por atender
        {
            if(cajero->numeroClientesAtendidos == 5) //Descanso en caso de haber atendido a 5 clientes
            {
                descansarCajero(cajero);
            }
            else
            {
                pthread_mutex_lock(&mutexFilaNormal);
                clienteAAsignar=dequeue(&inicioColaNormal);
                if(clienteAAsignar!=NULL) //Se verifica si en el inicio de la fila de clientes hay alguien esperando
                {
                    atendidos = atendidos + 1;
                    pthread_cond_signal(&produceNormal_t);
                    pthread_mutex_unlock(&mutexFilaNormal);
                    cliente = asignarCliente(clienteAAsignar, cajero);
                    (cajero->atendidosTotales++);
                    atenderCliente(cajero, cliente);
                }
                else
                {
                    if(atendidos<=99) //En caso de que no y que la cuenta de clientes atendidos sea menor/igual a 99, se manda wait a consumidores
                    {
                        pthread_cond_wait(&consumeNormal_t, &mutexFilaNormal);
                    }
                    pthread_mutex_unlock(&mutexFilaNormal);
                }
            }
        }
        else //Salida de Hilo
        {
            pthread_exit(NULL);
        }
    }
    pthread_exit(NULL);
}

/**
 * Funcion encargada de simular el comportamiento de cada uno de los cajeros Empresariales en el programa.
 * Revisa variables globales, checa si hay cliente por atender(Empresarial o General), descanda en caso de ser 
 * necesario y actualiza numero de clientes atendidos
 * 
 * @param arg, usado para generar el ID del Cajero
 * 
 **/
void* cajeroEmpresarial(void* arg)
{
    struct Cajero* cajero = crearCajero("Empresarial", (int)arg);
    int bandera = 0;
    while(atendidosEm < CLIENTES_EMPRESARIALES || atendidos < CLIENTES_NORMALES) //Primera linea de defensa. Se corrobora si todavia hay clientes por atender
    {
        bandera = 0; //Bandera a ser utilizada para que cajero Empresarial pueda atender cliente General en caso de no haber cliente Empresarial esperando
        usleep(rand() % 1000);

        //Se actualiza numero de clientes atendidos hasta el momento
        pthread_mutex_lock(&mutexFilaEmpresariales);
        int aten = atendidos;
        int atenEmp = atendidosEm;
        pthread_mutex_unlock(&mutexFilaEmpresariales);
        double porcentajeParticipacion = 0.0;
        struct Cliente* cliente;
        void* clienteAAsignar = NULL;
        if(cajero->numeroClientesAtendidos == 5) //Descanso en caso de haber atendido a 5 clientes
        {
            descansarCajero(cajero);
        }
        if(atenEmp < CLIENTES_EMPRESARIALES) //Segundo filtro; con numero de clientes atendidos actualizados, se vuelve a verificar si aun hay clientes por atender (EMP)
        {
            pthread_mutex_lock(&mutexFilaEmpresariales);
            clienteAAsignar=dequeue(&inicioColaEmpresarial);
            if(clienteAAsignar!=NULL) //Se verifica si en el inicio de la fila de clientes Empresariales hay alguien esperando
            {
                atendidosEm = atendidosEm + 1;
                pthread_cond_signal(&produceEmpresarial_t);            
                pthread_mutex_unlock(&mutexFilaEmpresariales);
                cliente = asignarCliente(clienteAAsignar, cajero);
                (cajero->atendidosTotales++);
                atenderCliente(cajero, cliente);
                bandera = 1; //Bandera evita que se atienda a un cliente General si se está atendiendo a un Empresarial
            }
            else
            {
                if(atendidosEm <= 49) //En caso de que no y que la cuenta de clientes Empresariales atendidos sea menor/igual a 49, se manda wait a consumidores
                {
                    pthread_cond_wait(&consumeEmpresarial_t, &mutexFilaEmpresariales);
                }
                pthread_mutex_unlock(&mutexFilaEmpresariales);
            }
        }
        if(aten < CLIENTES_NORMALES && bandera == 0) //Segmento de codigo que permite la atencion de un cliente General en caso de no haber cliente Empresarial en fila
        {
            pthread_mutex_lock(&mutexFilaNormal);
            clienteAAsignar=dequeue(&inicioColaNormal);
            if(clienteAAsignar!=NULL) //Se verifica si en el inicio de la fila de clientes Normales hay alguien esperando
            {
                atendidos = atendidos + 1;
                pthread_cond_signal(&produceNormal_t);
                pthread_mutex_unlock(&mutexFilaNormal);
                cliente = asignarCliente(clienteAAsignar, cajero);
                (cajero->atendidosTotales++);
                atenderCliente(cajero, cliente);
                usleep(50);
            }
            else
            {
                if(atendidos<=99) //En caso de que no y que la cuenta de clientes Generales atendidos sea menor/igual a 99, se manda wait a consumidores
                {
                    pthread_cond_wait(&consumeNormal_t, &mutexFilaNormal);
                }
                pthread_mutex_unlock(&mutexFilaNormal);
            }
        }
        else if(atenEmp >= CLIENTES_EMPRESARIALES && aten >= CLIENTES_NORMALES) //Salida de Hilo
        {
            pthread_exit(NULL);
        }
    }
    pthread_exit(NULL);
}

/**
 * Funcion encargada de simular el comportamiento generacion de Cliente Normal llegando a banco, e introduccion
 * del mismo en la fila. Esta fila es exclusivamente para clientes Normales del banco
 * 
 * @param arg, parametro pasado al momento de generar el hilo en el main()
 * 
 **/
void* filaBancoNormal(void* arg)
{
    for(int i = 0; i<CLIENTES_NORMALES; i++) //Se genera el numero de clientes Normales previamente establecido
    {   
        int waitTime = ((rand() %  (22 - 5 + 1)) + 5) * 1000000; 

        pthread_mutex_lock(&mutexFilaNormal);
        
        usleep(waitTime);
        struct Cliente* cliente = crearCliente((i + 1), 0);
        enqueue(&inicioColaNormal, cliente);
        pthread_cond_broadcast(&consumeNormal_t);

        pthread_mutex_unlock(&mutexFilaNormal);
        usleep(rand() % 1000);
    }
    pthread_exit(0);
}

/**
 * Funcion encargada de simular el comportamiento generacion de Cliente Empresarial llegando a banco, e introduccion
 * del mismo en la fila. Esta fila es exclusivamente para clientes Empresariales del banco
 * 
 * @param arg, parametro pasado al momento de generar el hilo en el main()
 * 
 **/
void* filaBancoEmpresarial(void* arg) //Se genera el numero de clientes Empresariales previamente establecido
{
    for(int i = 0; i<CLIENTES_EMPRESARIALES; i++)
    {
        int waitTime = ((rand() %  (34 - 9 + 1)) + 9) * 1000000; 

        pthread_mutex_lock(&mutexFilaEmpresariales);

        usleep(waitTime);
        struct Cliente* cliente = crearCliente((i + 1), 1);
        enqueue(&inicioColaEmpresarial, cliente);
        pthread_cond_broadcast(&consumeEmpresarial_t);

        pthread_mutex_unlock(&mutexFilaEmpresariales);
        usleep(rand() % 1000);
    }
    pthread_exit(0);
}

/**
 * Funcion encargada generar cajeros a ser utilizado en cada uno de los hilos que manejan cajeros.
 * Metodo implementado para cajeros Normales y Empresariales. Se genera ID y se inician variables.
 * 
 * @param tipo, parametro que inidca el tipo de cajero a ser creado
 * @param id, parametro que ayuda en la generacion del ID del cajero
 * 
 * return CAJERO generado y a ser implementado por hilos.
 * 
 **/
struct Cajero* crearCajero(char* tipo, int id)
{
    struct Cajero* cajero = (struct Cajero*)malloc(1 * sizeof(struct Cajero));
    cajero->numeroClientesAtendidos = 0;
    cajero->tipoCajero = tipo;
    cajero->atendidosTotales = 0;
    char* inicio = (char*)malloc(25 * sizeof(char));
    char *numeroCajero = (char*)malloc(2 * sizeof(char));
    sprintf(inicio, "%s%d", cajero->tipoCajero, id);
    cajero->ID = inicio;
    printf("Creacion Cajero %s%d\n", cajero->tipoCajero, id);
    return cajero;
}

/**
 * Funcion encargada generar clientes a ser procesados en por los diferentes cajeros del banco.
 * Se le asigna a cada cliente un ID.
 * 
 * @param tipo, parametro que inidca el tipo de cliente a ser creado
 * @param id, parametro que ayuda en la generacion del ID del cliente
 * 
 * return CLIENTE generado y a ser procesado por los distintos cajeros del banco
 * 
 **/
struct Cliente* crearCliente(int id, int tipo)
{
    struct Cliente* cliente = (struct Cliente*)malloc(1 * sizeof(struct Cliente));
    cliente->tipoCliente = tipo;
    char* inicio = (char*)malloc(25 * sizeof(char));
    char *numeroCliente = (char*)malloc(2 * sizeof(char));
    char* tipoo;
    if(tipo==0)
    {
        tipoo = "Normal";
    }
    else
    {
        tipoo = "Empresarial";
    }
    sprintf(inicio, "%s%d%s", "Cliente", id, tipoo);
    cliente->ID = inicio;
    printf("Llegada %s\n", cliente->ID);
    return cliente;
}

/**
 * Funcion encargada de dormir al hilo de cjaro que quiera descansar por 3 SEGUNDOS.
 * Se imprime mensaje a consola para que usuario pueda visualizar esta accion
 * 
 * @param cajero, quien es el cajero que sera puesto a descansar
 * 
 **/
void descansarCajero(struct Cajero* cajero)
{
    printf("Cajero %s se va a descansar\n", cajero->ID);
    usleep(3000000);
    cajero->numeroClientesAtendidos = 0;
    printf("Cajero %s ha regresado de descansar\n", cajero->ID);
    return;
}

/**
 * Funcion encargada de simular el tiempo de atencion de cliente por parte de alguno de
 * los cajeros en el banco, sin importar su tipo. Se imprimen mensajes para que usuario
 * pueda visualizar la accion
 * 
 * @param cajero, quien es el cajero que atendera a cliente
 * @param cliente, quien simula el cliente a ser atendido
 * 
 **/
void atenderCliente(struct Cajero* cajero, struct Cliente* cliente)
{
    printf("Cajero %s esta atendiendo a cliente %s\n", cajero->ID, cliente->ID);
    int waitTime = ((rand() %  (5 - 3 + 1)) + 3) * 1000000;
    usleep(waitTime);
    printf("Cajero %s y cliente %s han acabado su operacion\n", cajero->ID, cliente->ID);
    free(cliente->ID);
    return;
}

/**
 * Funcion encargada de sacar de la fila a cliente a ser atendido para luego ser 
 * atendido por algun cajero del banco. Se incrementa el numero de clientes atendidos
 * al cajero que vaya a atender al cliente
 * 
 * @param clienteAAsignar, quien simulara el cliente a ser atendido
 * @param cajero, quien es el cajero que atendera a cliente 
 **/
struct Cliente* asignarCliente(void* clienteAAsignar, struct Cajero* cajero)
{
    struct Cliente* cliente;
    cliente =((struct Cliente*)clienteAAsignar);
    (cajero->numeroClientesAtendidos)++;
    return cliente; 
}