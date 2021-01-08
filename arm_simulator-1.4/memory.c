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
#include <stdlib.h>
#include <stdio.h>
#include "memory.h"
#include "util.h"
#include <stdio.h>

struct memory_data
{
    uint8_t *data;
    size_t taille;
    int be;
};

memory memory_create(size_t size, int is_big_endian)
{
    memory mem = NULL;
    mem = malloc(sizeof(struct memory_data));
    if (mem == NULL)
    {
        return NULL;
    }
    else
    {
        mem->data = malloc(sizeof(uint8_t *) * size);
        if (mem->data == NULL)
        {
            free(mem);
            mem = NULL;
        }
        else
        {
            mem->taille = size;
            mem->be = is_big_endian;
        }
    }
    return mem;
}

size_t memory_get_size(memory mem)
{
    return mem->taille;
}

void memory_destroy(memory mem)
{
    free(mem->data);
    free(mem);
}

int memory_read_byte(memory mem, uint32_t address, uint8_t *value)
{
    if(address < memory_get_size(mem)){
        *value = *(mem->data+address);
        return 0;
    } else {
        return -1;
    }
}

int memory_read_half(memory mem, uint32_t address, uint16_t *value)
{
    if(address < memory_get_size(mem)){
        *value = 0;
        uint8_t v = 0;
        uint16_t v2 = 0;
        for (int i = 0; i < sizeof(uint16_t); i++)
        {
            memory_read_byte(mem, i, &v);
            v2 = (uint16_t)v;
            *value |= !mem->be ? (v2 << (8 * i)) : (v2 << (8 * (sizeof(uint16_t) - i - 1)));
        }
        return 0;
    } else {
        return -1;
    }
}

int memory_read_word(memory mem, uint32_t address, uint32_t *value)
{
    if(address < memory_get_size(mem)){
        *value = 0;
        uint8_t v = 0;
        uint32_t v2 = 0;
        for (int i = address; i < sizeof(uint32_t); i++)
        {
            memory_read_byte(mem, i, &v);
            v2 = (uint32_t)v;
            *value |= !mem->be ? (v2 << (8 * i)) : (v2 << (8 * (sizeof(uint32_t) - i - 1)));
        }
        return 0;
    } else {
        return -1;
    }
}

int memory_write_byte(memory mem, uint32_t address, uint8_t value)
{
    if(address < memory_get_size(mem)){
        *(mem->data + address) = value;
        return 0;
    } else {
        return -1;
    }
}

int memory_write_half(memory mem, uint32_t address, uint16_t value)
{
    if(address < memory_get_size(mem)){
        uint8_t value2 = 0;
        for (int i = address; i < sizeof(uint16_t); i++)
        {
            value2 = !mem->be ? (value + i) >> 8 * i : (value + (sizeof(uint16_t) - i - 1)) >> 8 * (sizeof(uint16_t) - i - 1);
            memory_write_byte(mem, i, value2);
        }
        return 0;
    } else {
        return -1;
    }
}

int memory_write_word(memory mem, uint32_t address, uint32_t value)
{
    if(address < memory_get_size(mem)){
        uint8_t value2 = 0;
        for (int i = address; i < sizeof(uint32_t); i++)
        {
            value2 = !mem->be ? (value + i) >> 8 * i : (value + (sizeof(uint32_t) - i - 1)) >> 8 * (sizeof(uint32_t) - i - 1);
            memory_write_byte(mem, i, value2);
        }
        return 0;
    } else {
        return -1;
    }
}
