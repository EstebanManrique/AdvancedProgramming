#include <stdio.h>
#include <string.h>
#include <stdlib.h>

size_t numeroCamasInicial = 2; //Numero de camas con las que se inicia el hospital

typedef struct
{
    char* nombre;
    char* Apellidos;
    char* telefono;
    int edad;
} Paciente;

typedef struct
{
    Paciente* paciente;
    int numeroCama;
    int estado; //Ocupada o Desocupada
} CamaHospital;

enum EstadoCamaHospital
{
    OCUPADA, //OCUPADA = 0; DESOCUPADA = 1;
    DESOCUPADA
};

CamaHospital* hospital;

void liberarMemoriaPaciente(Paciente *pacienteALiberar) //Libera memoria dinamica de pacientes dados de alta o al terminar el programa
{
    free(pacienteALiberar->nombre);
    free(pacienteALiberar->Apellidos);
    free(pacienteALiberar->telefono);
    free(pacienteALiberar);
    return;
}

void creacionHospital() //Crea las camas predeterminadas del hospital con base en la variable numeroCamasInicial
{
    for(int i = 0; i<numeroCamasInicial; i++)
    {
        (hospital + i)->estado = DESOCUPADA;
        (hospital + i)->numeroCama = i + 1;
        printf("Creacion de la cama %d.\n", (hospital + i)->numeroCama);
    }
    return;
}

void ampliacionHospital() //Amplia en 5 la capacidad del hospital en caso de que se trate de incluir un paciente y ya no haya espacio
{
    printf("Se requiere una expansion de 5 camas en el hospital.\n");
    int camasActuales = numeroCamasInicial; //Se guarda el numero de camas actuales para continuar con la creacion de las mismas
    numeroCamasInicial = numeroCamasInicial + 5;
    hospital = (CamaHospital*)realloc(hospital, numeroCamasInicial * sizeof(CamaHospital)); //Realocacion de memoria dinamica con numero de camas ACTUALIZADO
    for(camasActuales; camasActuales<numeroCamasInicial; camasActuales++)
    {
        (hospital + camasActuales)->estado = DESOCUPADA;
        (hospital + camasActuales)->numeroCama = camasActuales + 1;
        printf("Creacion de la cama %d.\n", (hospital + camasActuales)->numeroCama);
    }
    return;
}

void incorporarPacienteAux(Paciente* paciente)
{
    paciente->nombre = (char*)malloc(sizeof(char) * 30); //Uso de memoria dinamica para cadenas de caracteres
    printf("Introducir el nombre del paciente.\n");
    scanf(" %[^\n]", paciente->nombre);
    paciente->Apellidos = (char*)malloc(sizeof(char)* 35); //Uso de memoria dinamica para cadenas de caracteres
    printf("Introducir el apellido del paciente.\n");
    scanf(" %[^\n]", paciente->Apellidos);
    printf("Introducir la edad del paciente.\n");
    scanf("%d", &(paciente->edad));
    paciente->telefono = (char*)malloc(sizeof(char) * 10); //Uso de memoria dinamica para cadenas de caracteres
    printf("Introducir el telefono del paciente.\n");
    scanf("%s", paciente->telefono);
    return;
}

void IncorporarPaciente() //Se piden los datos del paciente para ser dado de alta en el hospital
{
    for(int i = 0; i<numeroCamasInicial; i++) //Ciclo que busca la primera cama Desocupada para dar de alta al paciente
    {
        if((hospital + i)->estado == DESOCUPADA)
        {
            (hospital + i)->paciente = (Paciente*)malloc(sizeof(Paciente));
            incorporarPacienteAux((hospital + i)->paciente);
            (hospital + i)->estado = OCUPADA;
            printf("El paciente %s %s se encuentra en la cama %d.\n\n", (hospital + i)->paciente->nombre, (hospital + i)->paciente->Apellidos,(hospital + i)->numeroCama);
            return;
        }
    }
    int camaAInsertar = numeroCamasInicial; //Se guarda este valor en caso de que No haya habido cama disponible; se usara una vez se aumente numero de camas.

    ampliacionHospital(); //Se aumenta en 5 el numero de camas.
    (hospital + camaAInsertar)->paciente = (Paciente*)malloc(sizeof(Paciente));
    incorporarPacienteAux((hospital + camaAInsertar)->paciente);
    (hospital + camaAInsertar)->estado = OCUPADA;
    printf("El paciente %s %s se encuentra en la cama %d.\n\n", (hospital + camaAInsertar)->paciente->nombre, (hospital + camaAInsertar)->paciente->Apellidos, (hospital + camaAInsertar)->numeroCama);
    return;
}

void checarCamaPaciente() //Checa que paciente hay en una cama determinada. Arroja que cama está Disponible si NO HAY PACIENTE e indica que Hospital no tiene cama si es el caso
{
    int numeroCama;
    printf("De que cama se quiere conocer al paciente?\n");
    scanf("%d", &numeroCama);
    if(numeroCama>numeroCamasInicial) //Numero de cama no fue encontrado en el hospital
    {
        printf("El hospital no cuneta con ese numero de cama.\n\n");
        return;
    }
    for(int i = 0; i<numeroCamasInicial; i++)
    {
        if((hospital + i)->numeroCama == numeroCama && (hospital + i)->estado == DESOCUPADA) //Cama introducida por usuario esta DESOCUPADA
        {
            printf("La cama numero %d esta desocupada y puede ser utilizada.\n\n", numeroCama);
            return;
        }
        else if((hospital + i)->numeroCama == numeroCama && (hospital + i)->estado == OCUPADA) //Cama introducida por usuario esta OCUPADA
        {
            printf("El paciente %s %s se encuentra en la cama %d.\n\n", (hospital + i)->paciente->nombre, (hospital + i)->paciente->Apellidos, numeroCama);
            return;
        }
        else
        {
            continue;
        }
    }
    return;
}

void darDeAlta() //Se da de alta a un paciente de acuerdo a su numero de cama
{
    int numeroCama;
    printf("De que cama se quiere conocer al paciente?\n");
    scanf("%d", &numeroCama);
    if((hospital + (numeroCama - 1))->estado == DESOCUPADA) //Si la el numero de cama introducido esta vacio, se le notifica el usaurio
    {
        printf("La cama se encuentra sin paciente, por lo que NO se puede cumplir su peticion.\n\n");
        return;
    }
    else if(numeroCama>numeroCamasInicial)
    {
        printf("El hospital no cuenta con la cama con ese numero.\n\n");
        return;
    }
    else
    {
        (hospital + (numeroCama - 1))->estado = DESOCUPADA;
        printf("La cama numero %d fue liberada y el paciente %s %s dado de alta.\n\n", numeroCama, (hospital + (numeroCama - 1))->paciente->nombre, (hospital + (numeroCama - 1))->paciente->Apellidos);
        liberarMemoriaPaciente((hospital + (numeroCama - 1))->paciente); //Se libera la memoria dinamica del usuario cuando se da de alta para evitar fugas de memoria
        return;
    }
}

void listadoPacientes() //Imprime lista de pacientes, con respectivo numero de camas, que se encuentran en el hospital
{
    for(int i = 0; i<numeroCamasInicial; i++)
    {
       if((hospital + i)->estado == OCUPADA)
       {
            printf("El paciente %s %s se encuentra en la cama %d.\n", (hospital + i)->paciente->nombre, (hospital + i)->paciente->Apellidos, (i + 1));
       }
    }
    printf("\n");
    return;
}

void camasDisponiblesYOcupadas() //Se hace un recuento y se imprime el numero de camas ocupadas y desocupadas en el hospital
{
    int camasDisponibles = 0;
    int camasOcupadas = 0;
    for(int i = 0; i<numeroCamasInicial; i++)
    {
        if((hospital + i)->estado == OCUPADA)
        {
            camasOcupadas++;
        }
        else
        {
            camasDisponibles++;
        }
    }
    printf("En el hospital hay %d camas Ocupadas y %d camas Disponibles.\n\n", camasOcupadas, camasDisponibles);
    return;
}

typedef void (*opcion_t)();

void menuAplicacion() //Menu principal de la Aplicacion
{
    creacionHospital();
    printf("Creacion de las camas predeterminadas del hospital completada.\n\n");

    opcion_t * menu = (opcion_t *) malloc(5 * sizeof(opcion_t));
    *menu = IncorporarPaciente;
    *(menu+1) = checarCamaPaciente;
    *(menu+2) = darDeAlta;
    *(menu+3) = listadoPacientes;
    *(menu+4) = camasDisponiblesYOcupadas;

    int opcion = 0;
    while(opcion != 6)
    {
        printf("1.- Incorporar Paciente a hospital.\n");
        printf("2.- Conocer que paciente se encuentra en cierta cama.\n");
        printf("3.- Dar de alta a paciente en cierta cama.\n");
        printf("4.- Mostrar listado de pacientes en el hospital.\n");
        printf("5.- Conocer numero de camas ocupadas y desocupadas en el hospital.\n");
        printf("6.- Salir de la aplicacion.\n\n");
        printf("Favor de escoger la opcion que quiere ejecutar.\n");
        scanf("%d", &opcion);
        if(opcion!=6 && opcion>=0 && opcion<=5)
        {
            (*(menu+opcion-1))(opcion);
        }
        else if(opcion!=6)
        {
            printf("La opcion introducida no es valida. \n\n");
            continue;
        }
        else
        {
            break;
        }

    }
    for(int i = 0; i<numeroCamasInicial; i++)
    {
        if((hospital + i)->estado == OCUPADA)
        {
            liberarMemoriaPaciente((hospital + (i))->paciente); //Cuando se decide acabar con la aplicacion, se LLIBERA TODA la memoria de pacientes que no hayan sido dados de alta
        }
    }
    printf("Se ha decidido salir de la aplicacion.\n");
    free(menu);
    return;
}

int main()
{
    hospital = (CamaHospital*)malloc(numeroCamasInicial * sizeof(CamaHospital)); //Alocacion de memoria dinamica inicial para las camas predeterminadas del hospital
    menuAplicacion(); //Menu de la aplicacion
    free(hospital); //Se libera la memoria usada para guardar todas las camas del hospital.
    return 0;
}