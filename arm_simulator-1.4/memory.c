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

Vous devez avoir reçu une copie de la Licence Publique Générale GNU en même
temps que ce programme ; si ce n'est pas le cas, écrivez à la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
États-Unis.

Contact: Guillaume.Huard@imag.fr
	 Bâtiment IMAG
	 700 avenue centrale, domaine universitaire
	 38401 Saint Martin d'Hères
*/
#include <stdlib.h>
#include <stdio.h>
#include "memory.h"
#include "util.h"

struct memory_data
{
    int be;
    uint8_t *donnees;
    size_t taille;
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
        mem->donnees = malloc(sizeof(uint8_t) * size);
        if (mem->donnees == NULL)
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
    free(mem->donnees);
    free(mem);
}

int memory_read_byte(memory mem, uint32_t address, uint8_t *value)
{
    if (mem->be == 0)
    {
        // fprintf(stderr, "Value vaut : %d || ", *value);
        *value = *(mem->donnees + address);
        // fprintf(stderr, "L'adresse dans la mémoire vaut : %d || ", *(mem->donnees + address));
        // fprintf(stderr, "Value vaut : %d || ", *value);
        return 0;
    }
    else if (mem->be == 1)
    {
        *value = *(mem->donnees + (mem->taille - address - 1));
        return 0;
    }
    else
    {
        return 0;
    }
}

int memory_read_half(memory mem, uint32_t address, uint16_t *value)
{
    int i = 0;
    *value = 0;
    uint8_t v = 0;
    uint16_t v2 = 0;
    if (mem->be == 0)
    {
        // fprintf(stderr, "\n------------------------------------------\n\n");
        // fprintf(stderr, "Indien : %d || ", mem->be);
        // fprintf(stderr, "Byte : %i || ", address);
        // fprintf(stderr, "Taille : %ld\n", mem->taille);

        while (i < sizeof(uint16_t))
        {
            memory_read_byte(mem, i, &v);
            v2 = (uint16_t)v;
            // fprintf(stderr, "Bit: %d, Octet n°%i : %i\n", i * 8, i, v);
            *value |= (v2 << (i * 8));
            i++;
        }

        // fprintf(stderr, "\n\nLe mot est : %d", *value);
        // fprintf(stderr, "\n------------------------------------------\n\n");
        return 0;
    }
    else if (mem->be == 1)
    {
        while (i < sizeof(uint16_t))
        {
            memory_read_byte(mem, i, &v);
            v2 = (uint16_t)v;
            // fprintf(stderr, "Bit: %d, Octet n°%i : %i\n", i * 8, i, v);
            *value |= (v2 << (8 * (sizeof(uint16_t) - i - 1)));
            i++;
        }
        return 0;
    }
    else
    {
        return -1;
    }
}

int memory_read_word(memory mem, uint32_t address, uint32_t *value)
{
    int i = 0;
    *value = 0;
    uint8_t v = 0;
    uint32_t v2 = 0;
    if (mem->be == 0)
    {

        while (i < sizeof(uint32_t))
        {
            memory_read_byte(mem, i, &v);
            v2 = (uint32_t)v;
            // fprintf(stderr, "Bit: %d, Octet n°%i : %i\n", i * 8, i, v);
            *value |= (v2 << (i * 8));
            i++;
        }

        // fprintf(stderr, "\n\nLe mot est : %d", *value);
        // fprintf(stderr, "\n------------------------------------------\n\n");
        return 0;
    }
    else if (mem->be == 1)
    {
        while (i < sizeof(uint32_t))
        {
            memory_read_byte(mem, i, &v);
            v2 = (uint32_t)v;
            // fprintf(stderr, "Bit: %d, Octet n°%i : %i\n", i * 8, i, v);
            *value |= (v2 << (8 * (sizeof(uint32_t) - i - 1)));
            i++;
        }

        // fprintf(stderr, "\n\nLe mot est : %d", *value);
        return 0;
    }
    else
    {
        return -1;
    }
}

int memory_write_byte(memory mem, uint32_t address, uint8_t value)
{
    if (mem->be == 0)
    {
        *(mem->donnees + address) = value;
        return *(mem->donnees + address) == value ? 0 : -1;
    }
    else if (mem->be == 1)
    {
        *(mem->donnees + (mem->taille - address - 1)) = value;
        return *(mem->donnees + (mem->taille - address - 1)) == value ? 0 : -1;
    }
    else
    {
        return -1;
    }
}

int memory_write_half(memory mem, uint32_t address, uint16_t value)
{
    int i = 0;
    uint16_t value2 = 0;
    if (mem->be == 0)
    {
        while (i < sizeof(uint16_t))
        {
            value2 = (value + i) >> 8 * i;
            // fprintf(stderr, "Nb F : %d, Bit: %d, Octet n°%i : %d\n", (i+1), i * 8, i, value2);
            memory_write_byte(mem, i, value2);
            i++;
        }
        return 0;
    }
    else if (mem->be == 1)
    {
        while (i < sizeof(uint16_t))
        {
            int decal = (sizeof(uint16_t) - i - 1);
            value2 = (value + decal) >> 8 * decal;
            // fprintf(stderr, "Nb F : %d, Bit: %d, Octet n°%i : %d\n", (i+1), i * 8, i, value2);
            memory_write_byte(mem, i, value2);
            i++;
        }
        return -1;
    }
    else
    {
        return -1;
    }
}

int memory_write_word(memory mem, uint32_t address, uint32_t value)
{
    int i = 0;
    uint32_t value2 = 0;
    if (mem->be == 0)
    {
        while (i < sizeof(uint32_t))
        {
            value2 = (value + i) >> 8 * i;
            memory_write_byte(mem, i, value2);
            i++;
        }
        return -1;
    }
    else if (mem->be == 1)
    {
        while (i < sizeof(uint32_t))
        {
            int decal = (sizeof(uint32_t) - i - 1);
            value2 = (value + decal) >> 8 * decal;
            memory_write_byte(mem, i, value2);
            i++;
        }
        return -1;
    }
    else
    {
        return -1;
    }
}
