#include <stdio.h>
# define STACK_SIZE 4096

char pile1[STACK_SIZE];
char pile2[STACK_SIZE];
char pile3[STACK_SIZE];
char pile4[STACK_SIZE];



typedef void * coroutine_t;
/* Quitte le contexte courant et charge les registres et la pile de CR. */
void enter_coroutine(coroutine_t cr);
/* Sauvegarde le contexte courant dans p_from, et entre dans TO. */
void switch_coroutine(coroutine_t *p_from, coroutine_t to);
/* Initialise la pile et renvoie une coroutine telle que, lorsqu’on entrera dedans,
elle commencera à s’exécuter à l’adresse initial_pc. */
coroutine_t init_coroutine(void *stack_begin, unsigned int stack_size, void (*initial_pc)(void));
coroutine_t qi;
coroutine_t da;

coroutine_t init_coroutine(void *stack_begin, unsigned int stack_size, void (*initial_pc)(void)){
    char *stack_end = ((char *)stack_begin) + stack_size;
    void **ptr = (void*)stack_end;
    ptr--;
    *ptr = initial_pc;
    ptr--;
    *ptr = 0;
    ptr--;
    *ptr = 0;
    ptr--;
    *ptr = 0;
    ptr--;
    *ptr = 0;
    ptr--;
    *ptr = 0;
    ptr--;
    *ptr = 0;
    // ptr--;
    // ptr = stack_begin;
    return ptr;
};


void ziqi(){
    int i = 0;
    while(1){
        printf("%d, ziqi is a cute girl.\n",i++);
        switch_coroutine(&qi,da);
    }
};
void dada(){
    int d = 0;
    while(1){
        printf("%d, dada is a silly boy.\n",d++);
        switch_coroutine(&da,qi);
    }
};


int main(){
    qi = init_coroutine(pile1,STACK_SIZE,ziqi);
    da = init_coroutine(pile2,STACK_SIZE,dada);
    enter_coroutine(da);
    return 0;
}