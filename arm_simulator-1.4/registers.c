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
#include "registers.h"
#include "arm_constants.h"
#include <stdlib.h>

struct registers_data {
    uint32_t reg[37];
    uint8_t mode;
};

uint8_t matriceReg[32][18] = {
    {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {},
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, -1}, // USR = 0x10 = 16
    {0, 1, 2, 3, 4, 5, 6, 7, 29, 30, 31, 32, 33, 34, 35, 15, 16, 36}, // FIQ = 0x11 = 17
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 23, 26, 27, 16, 28}, // IRQ = 0x12 = 18
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 17, 18, 15, 16, 19}, // SVC = 0x13 = 19
    {}, {}, {},
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 20, 21, 15, 16, 22}, // ABT = 0x17 = 23,
    {}, {}, {}, 
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 23, 24, 15, 16, 25}, // UND = 0x1b = 27,
    {}, {}, {},
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, -1}, // SYS = 0x1f = 31
};


registers registers_create() {
    registers r = NULL;
    r = malloc(sizeof(struct registers_data));
    if(r == NULL){
        return NULL;
    }
    r->mode = USR;
    return r;
}

void registers_destroy(registers r) {
    free(r);
}

uint8_t get_mode(registers r) {
    return r->mode;
} 

int current_mode_has_spsr(registers r) {
    return (get_mode(r) != USR && get_mode(r) != SYS);
}

int in_a_privileged_mode(registers r) {
    return (get_mode(r) != USR);
}

uint32_t read_register(registers r, uint8_t reg) {
    uint32_t value = r->reg[reg];
    return value;
}

uint32_t read_usr_register(registers r, uint8_t reg) {
    if(r->mode != USR)
    {
        return -1;
    }
    else
    {
        return r->reg[reg];
    }
}

uint32_t read_cpsr(registers r) {
    uint32_t value = r->reg[16];
    return value;
}

uint32_t read_spsr(registers r) {
    if(current_mode_has_spsr(r)) 
    {
        uint32_t value = r->reg[17];
        return value;
    }
    else
    {
        return -1;
    }
}

void write_register(registers r, uint8_t reg, uint32_t value) {
    if( reg < 37){
        // Il faut récupérer le numéro du registre selon le mode qu'on est.
        uint8_t num_reg_mode = matriceReg[r->mode][reg];
        if(num_reg_mode != -1)
            r->reg[num_reg_mode] = value;
    }
    
}

void write_usr_register(registers r, uint8_t reg, uint32_t value) {
    if(r->mode == USR){
        write_register(r, reg, value);
    }
}

void write_cpsr(registers r, uint32_t value) {
    write_register(r, 16, value);
}

void write_spsr(registers r, uint32_t value) {
    if(current_mode_has_spsr(r)) 
    {
        write_register(r, 17, value);
    }
}
