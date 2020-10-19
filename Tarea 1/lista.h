#ifndef LISTA
#define LISTA

typedef struct tipoDato{
    void *contenido;
    char tipo;
}dato;

typedef struct tipoNodo{
    dato *info;
    struct tipoNodo *next;
}nodo;

typedef struct tipoLista{
    int length,currentPos;
    nodo *actual,*head,*tail;
}lista;

void init(lista **list);

void append(lista *list, dato *valor);

void next(lista *list);

void goTo(lista *list,int pos);

void prev(lista *list);

void insert(lista *list,int pos, dato *valor);

int length(lista* list);

void clear(lista *list);

void destroy(lista *list);

void removeNodo(lista *list,int pos);

dato *at(lista* list, int pos);

#endif //LISTA