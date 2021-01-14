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
#include <stdio.h>


int8_t matriceMode[32][18] = {
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
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, -1} // SYS = 0x1f = 31
};

struct registers_data {
    uint32_t reg[37];
    uint8_t mode;
};

typedef struct registers_data *registers;

registers registers_create() {
    registers r = NULL;
    r = malloc(sizeof(struct registers_data));
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
    return (get_mode(r) != SYS && get_mode(r) != USR);
}

int in_a_privileged_mode(registers r) {
    return get_mode(r) != USR;
}

uint32_t read_register(registers r, uint8_t reg) {
    uint32_t value=r->reg[reg];
    return value;
}

uint32_t read_usr_register(registers r, uint8_t reg) {
    if(get_mode(r) != USR){
        return -1;
    }else{
        return r->reg[reg];
    }
}

uint32_t read_cpsr(registers r) {
    uint32_t value=r->reg[16];
    return value;
}

uint32_t read_spsr(registers r) {
    uint32_t value=0;
    if(!current_mode_has_spsr(r)){
        return -1;
    }else{
        value = r->reg[matriceMode[get_mode(r)][17]];
    }
    return value;
}

void write_register(registers r, uint8_t reg, uint32_t value) {
    if(reg<37){
        uint8_t nbReg = matriceMode[r->mode][reg];
        if(nbReg>=0){
            r->reg[nbReg] = value;
        }
    }
}

void write_usr_register(registers r, uint8_t reg, uint32_t value) {
    if(!in_a_privileged_mode(r)){
        write_register(r, reg, value);
    }
}

void write_cpsr(registers r, uint32_t value) {
    write_register(r,16,value);
}

void write_spsr(registers r, uint32_t value) {
    if(current_mode_has_spsr(r)){
        write_register(r,17,value);
    }
}
