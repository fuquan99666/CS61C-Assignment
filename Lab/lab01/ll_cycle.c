#include <stddef.h>
#include "ll_cycle.h"

int ll_has_cycle(node *head) {
    node* turtle = head;
    if(head == NULL){
        return 0;
    }
    node* rabbit = head->next;

    while(turtle!=rabbit){
        if(rabbit->next==NULL){  //nullptr
            return 0;
        }

        rabbit=rabbit->next->next;

        if(rabbit==NULL){
            return 0;
        }
        turtle = turtle->next;

    }
    return 1;
}