#include <stdio.h>
# define STACK_SIZE 4096
# define bloque 0
# define pret 1

char pile1[STACK_SIZE];
char pile2[STACK_SIZE];
char pile3[STACK_SIZE];
char pile4[STACK_SIZE];



typedef void * coroutine_t;
typedef struct thread{
    coroutine_t pointeur;
    int statut;
} thread;
/* Quitte le contexte courant et charge les registres et la pile de CR. */
void enter_coroutine(coroutine_t cr);
/* Sauvegarde le contexte courant dans p_from, et entre dans TO. */
void switch_coroutine(coroutine_t *p_from, coroutine_t to);
/* Initialise la pile et renvoie une coroutine telle que, lorsqu’on entrera dedans,
elle commencera à s’exécuter à l’adresse initial_pc. */
coroutine_t init_coroutine(void *stack_begin, unsigned int stack_size, void (*initial_pc)(void));

coroutine_t coroutine_ordo;
coroutine_t coroutine1;
coroutine_t coroutine2;
coroutine_t coroutine3;

thread *thread_glo;
thread thread1;
thread thread2;
thread thread3;


void yield(void){
    switch_coroutine(&thread_glo->pointeur,coroutine_ordo);
    // thread_glo->statut = bloque;
}

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

void test(){
    int d = 0;
    while(1){
        printf("d = %d.\n",d++);   
        yield();  
    }
}

void algorithme(){
    while(1){
        thread_glo = &thread1;
        switch_coroutine(&coroutine_ordo,thread1.pointeur);
        thread_glo = &thread2;
        switch_coroutine(&coroutine_ordo,thread2.pointeur);
        thread_glo = &thread3;
        switch_coroutine(&coroutine_ordo,thread3.pointeur);
    }

};
int main(){
    coroutine_ordo = init_coroutine(pile1, STACK_SIZE, &algorithme); //ordonnanceur !
    printf("coroutine_ordo init\n");
    coroutine1 = init_coroutine(pile2, STACK_SIZE, &test);
    thread1.pointeur = coroutine1;
    printf("coroutine1 init\n");
    coroutine2 = init_coroutine(pile3, STACK_SIZE, &test);
    thread2.pointeur = coroutine2;
    printf("coroutine2 init\n");
    coroutine3 = init_coroutine(pile4, STACK_SIZE, &test);
    thread3.pointeur = coroutine3;
    printf("coroutine3 init\n");
    enter_coroutine(coroutine_ordo);
    return 0;
}