//
// Created by heros on 19/09/24.
//

#include "lists.h"
#include <stdlib.h>

//todo hacerlas por array reservado de forma dinámica
typedef struct node{
    void * data;
    struct node * next;
}* list;

//mem management instructions
list list_new(){
    list l = NULL;

    return l;
}
void list_free(list * l){
    list curr;
    list next;

    for(curr = *l; curr != NULL; curr = next){
        next = curr->next;
        free(curr->data);
        free(curr);
    }

    *l = NULL;
}

//list management
void list_append(list * l , void * element){
    list * parent = l, curr = *l;
    while (curr != NULL){
        parent = &curr->next;
        curr = *parent;
    }
    *parent = malloc(sizeof(struct node));
    curr = *parent;
    curr->next = NULL;
    curr->data = element;
}
void list_remove(list * l, int pos){
    list * parent = l, curr = *l;
    for(int i = 0; i < pos; i++){
        parent = &curr->next;
        curr = *parent;
    }
    *parent = curr->next;
    free(curr);
}
void list_add(list * l, int pos, void * element){
    list * parent = l, curr = *l, aux;
    for(int i = 0; i < pos; i++){
        parent = &curr->next;
        curr = *parent;
    }
    *parent = aux = malloc(sizeof(struct node));
    aux->data = element;
    aux->next = curr;
}

//element management
void * list_get(list l, int pos){
    for(int i = 0; i < pos; i++)
        l = l->next;
    return l->data;
}
void list_set(list l, int pos, void * element){
    for(int i = 0; i < pos; i++)
        l = l->next;
    l->data = element;
}
int list_search(list l, void * element, int (* comparator)(const void *, const void *)){
    for(int i = 0; l != NULL; i++){
        if(!comparator(l->data, element))
            return i;
        l = l->next;
    }
    return -1;
}
int list_length(list l){
    int i;
    for(i = 0; l != NULL; i++)
        l = l->next;
    return i;
}