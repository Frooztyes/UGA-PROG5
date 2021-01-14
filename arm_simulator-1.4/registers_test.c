#include <stdio.h>
#include <stdlib.h>
#include "registers.c"
#include "registers.h"
#include "arm_constants.h"
#include "util.h"


void print_test(int result) {
    if (result)
        printf("Test succeded\n");
    else
        printf("TEST FAILED !!\n");
}

int compare(uint32_t a, uint32_t b){
    if(a == b){
        return 1;
    }else{
        return 0;
    }
}

int compare_with_sim(uint32_t val_a, registers r, uint8_t reg){
    uint32_t value;
    value = read_register(r,reg);
    if(val_a != value){
        return 0;
    }
    return 1;
}

int main(){
    uint32_t read_test;
    int Val_test1 = 1;
    int Val_test2 = 2;
    int Val_test3 = 3;
    int Val_test4 = 4;
    registers reg_test = registers_create();

    if(reg_test == NULL) {
        fprintf(stderr, "Erreur lors de la creation du registre de test\n");
        exit(1);
    }else{
        printf("Creation du registre avec succes\n");
    }
    printf("-----------------------------------------\n");
    printf("Test de write et read register\n");

    write_register(reg_test,1,Val_test1);
    printf("- Ecriture de la variable Val_test1 dans le Registre 1 : ");
    print_test(compare_with_sim(Val_test1,reg_test,1));
    

    read_test = read_register(reg_test,1);
    printf("- Lecture de la variable dans le registre 1 : ");
    print_test(compare(Val_test1, read_test));


    printf("-----------------------------------------\n");
    printf("Test de write et read register CPSR (Registre 16)\n");

    write_cpsr(reg_test,Val_test2);
    printf("- Ecriture de la variable Val_test2 dans le registre 16/CPSR : ");
    print_test(compare_with_sim(Val_test2, reg_test, 16));


    read_test = read_cpsr(reg_test);
    printf("- Lecture de la variable Val_test2 dans le Registre 16/CPSR : ");
    print_test(compare(Val_test2,read_test));

    printf("-----------------------------------------\n");
    printf("Test de write et read un registre USR \n");

    write_usr_register(reg_test, 3, Val_test4);
    printf("- Ecriture de la variable Val_test4 dans le registre USR 3 : ");
    print_test(compare_with_sim(Val_test4, reg_test, 3));

    read_test = read_usr_register(reg_test,3);
    printf("- Lecture de la variable Val_test4 dans le registre USR 3 : ");
    print_test(compare(Val_test4, read_test));

    printf("-----------------------------------------\n");
    printf("Test de write et read register SPSR (Registre 17)\n");
    printf("Changement du mode de USR à UND pour avoir le registre spsr : ");
    reg_test->mode = UND;
    print_test(get_mode(reg_test) != USR);


    write_spsr(reg_test, Val_test3);
    printf("- Ecriture de la variable Val_test3 dans le Registre 17/SPSR : ");
    print_test(compare_with_sim(Val_test3, reg_test, 25));

    read_test = read_spsr(reg_test);
    printf("- Lecture de la variable Val_test3 dans le Registre 17/SPSR : ");
    print_test(compare(Val_test3,read_test));

    return 0;

}