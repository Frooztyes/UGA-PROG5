/*
Armator - simulateur de jeu d'instruction ARMv5T � but p�dagogique
Copyright (C) 2011 Guillaume Huard
Ce programme est libre, vous pouvez le redistribuer et/ou le modifier selon les
termes de la Licence Publique G�n�rale GNU publi�e par la Free Software
Foundation (version 2 ou bien toute autre version ult�rieure choisie par vous).

Ce programme est distribu� car potentiellement utile, mais SANS AUCUNE
GARANTIE, ni explicite ni implicite, y compris les garanties de
commercialisation ou d'adaptation dans un but sp�cifique. Reportez-vous � la
Licence Publique G�n�rale GNU pour plus de d�tails.

Vous devez avoir re�u une copie de la Licence Publique G�n�rale GNU en m�me
temps que ce programme ; si ce n'est pas le cas, �crivez � la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
�tats-Unis.

Contact: Guillaume.Huard@imag.fr
	 B�timent IMAG
	 700 avenue centrale, domaine universitaire
	 38401 Saint Martin d'H�res
*/
#include "arm_instruction.h"
#include "arm_exception.h"
#include "arm_data_processing.h"
#include "arm_load_store.h"
#include "arm_branch_other.h"
#include "arm_constants.h"
#include "util.h"






void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;
    
    for (i = size-1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            fprintf(stderr, "%u", byte);
        }
    }
    puts("");
}


static int arm_execute_instruction(arm_core p) {
    printf("Salut\n");
    uint32_t ins;
    arm_fetch(p,&ins);
    printf("ins = %d\n", ins);
    printf("%08x \n",ins);
    printf("%0d \n",ins);
    printBits(sizeof(ins), &ins);
    switch(get_bits(ins,27,25)){
        case 0b000:
            printf("Je suis dans arm shift 0\n");
            arm_data_processing_shift(p,ins);
            break;
        case 0b001:
            arm_data_processing_immediate_msr(p,ins);
            break;
        case 0b010:
            break;
        case 0b011:
            break;
        case 0b100:
            arm_load_store_multiple(p,ins);
            break;
        case 0b101:
            arm_branch(p,ins);
            break;
        case 0b110:
            arm_coprocessor_load_store(p,ins);
            break;
        case 0b111:
            break;
        default:
            break;
    }
    return 0;
}

int arm_step(arm_core p) {
    int result;
    
    result = arm_execute_instruction(p);
    if (result)
        arm_exception(p, result);
    return result;
}
