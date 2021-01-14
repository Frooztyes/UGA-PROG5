#include <stdio.h>
#include <stdlib.h>
#include "arm_core.h"
#include "arm_data_processing.h"
#include "arm_constants.h"
#include "util.h"


void print_test(int result) {
    if (result)
        printf("Test succeded\n");
    else
        printf("TEST FAILED !!\n");
}

int carryFrom(uint64_t res) {
    return get_bit(res >> 32, 0);
}

//Faire un cast des operandes avant de faire le carryFrom sinon c'est toujours egale a 0

int main(){
 
    uint64_t op1 = 0x80000000;
    uint64_t op2 = 0x10000000;
    printf("--------------------------------------\n");
    printf("- Test de la fonction CarryFrom\n");
    printf("- Test quand il y a un debordement : ");
    print_test(carryFrom(op1 + op1) == 1);
    printf("- Test quand il n'y a pas de debordement : ");
    print_test(carryFrom(op2 + op2) == 0);
    return 0;
}