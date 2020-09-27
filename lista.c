#include<stdio.h>
#include<stdlib.h>
#include"lista.h"
#include"readFiles.h"

void init(lista **list){ /*inicializacion de lista, todos los punteros de la lista apuntan a un mismo nodo dummy*/
    *list = malloc(sizeof(lista));
    nodo *dummy = (nodo*)malloc(sizeof(nodo));
    dummy->next = NULL;
    (*list)->length = (*list)->currentPos =0;
    (*list)->head = dummy;
    (*list)->actual = dummy;
    (*list)->tail = dummy;
}
/*
init()
inicializa la memoria y datos de una lista (lista*) y le asigna también
memoria al primer nodo de la lista (dummy).
——————————————–
Inputs:
(*lista list) lista que se desea inicializazr. 
——————————————–
Output:
void, no hace retorno, solo cambia valores
*/

void append(lista *list, dato *valor){
    /*se inicializa el dcontenento recibido dentro de un nodo*/
    nodo *aux = (nodo*)malloc(sizeof(nodo));
    aux->info = valor;
    aux->next = NULL;
    /*si la lista no tiene elementos, entonces solo se agrega el nodo al principio. En caso contrario, 
    se agrega al final y se mueve el puntero cola al siguiente nodo para no perder la referencia*/
    if (list->length == 0) list->head->next = aux;
    else
    {
        list->tail->next->next = aux;
        list->tail =list->tail->next;
    };
    list->length += 1;
    return;
}
/*
append()
agrega un nodo en la posición final de la lista
——————————————–
Inputs:
(*lista) lista a la cual se desea agregar un nodo
(* dato) dato que se agrega a la lista, el dato será inicializado dentro de un nodo,
——————————————–
Output:
void, no hace retorno
*/

void next(lista *list){
    /*en caso de que el puntero "actual" se encuentre en la misma posicion que la cola de la lista,
    ya no se puede avanzar, por lo tanto no se mueve el puntero. De este modo se evita también que se 
    mueva el puntero al haber 0 o 1 elemento en la lista  */
    if (list->actual == list->tail) return;
    else
    {
        list->actual = list->actual->next;
        list->currentPos += 1;
    }
}
/*
next()
avanza el puntero "actual" una posición en la lista.
NO hace nada en caso de que el puntero esté al final de la lista
——————————————–
Inputs:
(*lista) lista en la cual se moverá el puntero actual
——————————————–
Output:
void, no hace retorno, solo cambia valores(direcciones de mem y valor de (int)curr)
*/

void goTo(lista *list,int pos){
    if (pos >= list->length || pos < 0) return;
    if (pos < list->currentPos){
        list->currentPos = 0;
        list->actual = list->head;
    }
    while (list->currentPos != pos)
    {
        next(list);
    }
}
/*
goTo()
redirige el puntero "actual" a una posición indicada de la lista.
si la posición está fuera de la lista no hace nada.
——————————————–
Inputs:
(*lista) lista en la cual se moverá el puntero "actual"
(int pos) posicion a la cual será movido el puntero actual
——————————————–
Output:
void, no hace retorno
*/

void prev(lista *list){
    if (list->actual == list->head) return;
    else
    {
        nodo *aux = list->actual;   
        goTo(list,0);
        while (list->actual->next != aux)
            {
            next(list);
            }
    }
}
/*
prev()
mueve el puntero "actual" a la posicion anterior contigua en la lista.
NO hace nada en caso de que el puntero esté al principio de la lista
——————————————–
Inputs:
(*lista) lista en que se moverá el puntero actual
——————————————–
Output:
void, no hace retorno
*/

void insert(lista *list,int pos, dato *valor){
    if (list->length < pos || pos < 0) return;
    if (list->length == pos){
        append(list,valor);
        return;
    }
    else
    {
        /*se inicializa el dato recibido dentro de un nodo*/
        goTo(list,pos);
        nodo *aux = (nodo*)malloc(sizeof(nodo));
        aux->info = valor;
        aux->next = list->actual->next;
        if (list->actual == list->tail){
            list->tail = aux;
        };
        list->actual->next = aux;
        list->length += 1;
    }
}
/*
insert()
agrega un nodo en una posición indicada de la lista.
——————————————–
Inputs:
(*lista) lista a la cual se desea agregar un nodo.
(int pos) posición en la cual se desea agregar el nodo a la lista.
(* dato) dato que se agrega a la lista, el dato será inicializado dentro de un nodo,
——————————————–
Output:
void, no hace retorno
*/


int length(lista* list){
    return list->length;
}
/*
length()
sirve para consultar la cantidad de elementos en la lista,
no hace distinción entre que tipo de elementos son.
——————————————–
Inputs:
(*lista) lista de la cual se consultará su largo
——————————————–
Output:
int, cantidad de elementos en la lista
*/

void clear(lista *list){
    if (list->length == 0) return;
    nodo *aux1,*aux2;
    aux2 = list->head->next;
    for (int i = 0; i<list->length ;i++){
        aux1 = aux2;
        aux2 = aux1->next;
        if (aux1->info->tipo == 'l')
        {
            clear((lista*)aux1->info->contenido);
            destroy((lista*)aux1->info->contenido);
            free(aux1->info);
            free(aux1);
        }
        else if(aux1->info->tipo == 'J'){
            tipoJuego* auxJuego = aux1->info->contenido;
            clear(auxJuego->categorias);
            destroy(auxJuego->categorias);
            free(aux1->info->contenido);
            free(aux1->info);
            free(aux1); 
        }
        else
        {
            free(aux1->info->contenido);
            free(aux1->info);
            free(aux1);
        }
    }
    list->length = list->currentPos = 0;
    list->head->next = NULL;
    list->tail = list->actual = list->head;
}
/*
clear()
borra todos los elmentos de la lista y libera la memoria de los mismos,
reinicializando la lista como vacía, largo de la lista va a 0.
——————————————–
Inputs:
(*lista) puntero a la lista que se desea reinicializar.
——————————————–
Output:
void, no hace retorno
*/

void destroy(lista *list){
    clear(list);
    free(list->head);
    free(list);
}
/*
destroy()
libera la memoria asociada a una lista
——————————————–
Inputs:
(*lista)
puntero a la lista que se desea destruir y cuya memoria será liberada.
——————————————–
Output:
void, no hace retorno
*/

void removeNodo(lista *list,int pos){
    if (list->length == 0 || pos > list->length) return;
    goTo(list, pos);
    nodo *aux = list->actual->next;
    list->actual->next = aux->next;

    if (list->actual == list->tail) 
    {
        prev(list);
        list->tail = list->actual;
    }
    if (aux == list->tail) list->tail = list->actual;
    
    if (aux->info->tipo == 'l')
    {
        clear((lista*)aux->info->contenido);
        destroy(aux->info->contenido);
    }
    else
    {
        free(aux->info->contenido);
    }
    free(aux);
    list->length -= 1;
}
/*
removeNodo()
La función quita un nodo de una lista y libera su memoria
——————————————–
Inputs:
(*lista list) lista de la cual se desea quitar un nodo
(int pos) posicion de la lista en la cual se encuentra el nodo que se desea quitar
——————————————–
Output:
void, no hace retorno
*/

dato *at(lista* list, int pos){
    goTo(list,pos);
    return (list->actual->next->info);
}
/*
at()
con esta funcion se pueden hacer consultas acerca de los tipos de
datos, y los datos, que se encuentran en la lista.
——————————————–
Inputs:
(*lista list) puntero a la lista que será consultada.
(int pos) posición de la lista que se desea consultar.
——————————————–
Output:
(*dato) retorna un puntero al tipo dato en la posicion "pos" de la lista.
*/