/**
 * Autor - Rodrigo Quiroz Reyes & Esteban Manrique de Lara Sirvent
 * Fecha - 04/09/2020
 * Queue genérica - Codigo Auxiliar
*/

#include <stdio.h>
#include <stdlib.h>


typedef struct node {
    void* val;
    struct node *next;
} node_t; // Estrucutra nodo para pasar de un elemento al siguiente y almacena un apuntador void*

/**
 * Método para encolar cualquier valor con cualquier tipo de dato
 * 
 * @param node_t **head - La cabeza en donde empieza la cola
 * @param void* val - Apuntador al valor que se quiere encolar
 */
void enqueue(node_t **head, void* val) {
    node_t *new_node =(node_t*) malloc(sizeof(node_t));
    if (!new_node) {
        return;
    }

    new_node->val = val;//Guarda el valor
    new_node->next = *head; //Apuntador al siguiente valor 
    
    *head = new_node;
}

/**
 * Método para quitar de la cola el último elemento de la estructura
 * 
 * @param node_t **head - La cabeza en donde empieza la cola
 */
void * dequeue(node_t **head) {
    node_t *current, *prev = NULL;
    void* retval; //Variable que va a regresar
    int temp=-1;
    retval=&temp;

    if (*head == NULL){

        return NULL;
    } 

    current = *head;
    while (current->next != NULL) {
        prev = current;
        current = current->next;
    }

    retval = current->val;
    free(current); //Libera la memoria del nodo
    
    if (prev)
        prev->next = NULL;
    else
        *head = NULL;

    return retval; //Regresa el último valor de la cola
}
