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

<<<<<<< HEAD
Vous devez avoir re�u une copie de la Licence Publique G�n�rale GNU en m�me
temps que ce programme ; si ce n'est pas le cas, �crivez � la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
�tats-Unis.

Contact: Guillaume.Huard@imag.fr
	 B�timent IMAG
	 700 avenue centrale, domaine universitaire
	 38401 Saint Martin d'H�res
=======
Vous devez avoir reçu une copie de la Licence Publique Générale GNU en même
temps que ce programme ; si ce n'est pas le cas, écrivez à la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
États-Unis.

Contact: Guillaume.Huard@imag.fr
	 Bâtiment IMAG
	 700 avenue centrale, domaine universitaire
	 38401 Saint Martin d'Hères
>>>>>>> Add start of memory read
*/
#include <stdlib.h>
#include <stdio.h>
#include "memory.h"
#include "util.h"
#include <stdio.h>

struct memory_data
{
<<<<<<< HEAD
    uint8_t *data;
    size_t taille;
    int be;
=======
    int be;
    uint8_t *donnees;
    size_t taille;
>>>>>>> Add start of memory read
};

memory memory_create(size_t size, int is_big_endian)
{
<<<<<<< HEAD
=======
    fprintf(stderr, "La taille est de : %ld\n", size);
>>>>>>> Add start of memory read
    memory mem = NULL;
    mem = malloc(sizeof(struct memory_data));
    if (mem == NULL)
    {
        return NULL;
    }
    else
    {
<<<<<<< HEAD
        mem->data = malloc(sizeof(uint8_t *) * size);
        if (mem->data == NULL)
=======
        mem->donnees = malloc(sizeof(uint8_t) * size);
        if (mem->donnees == NULL)
>>>>>>> Add start of memory read
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
<<<<<<< HEAD
    free(mem->data);
=======
    free(mem->donnees);
>>>>>>> Add start of memory read
    free(mem);
}

int memory_read_byte(memory mem, uint32_t address, uint8_t *value)
{
<<<<<<< HEAD
    if(address < memory_get_size(mem)){
        *value = *(mem->data+address);
        return 0;
    } else {
        return -1;
=======
    if (mem->be == 0)
    {
        fprintf(stderr, "Value vaut : %d || ", *value);
        *value = *(mem->donnees + address);        
        fprintf(stderr, "L'adresse dans la mémoire vaut : %d || ", *(mem->donnees + address));
        fprintf(stderr, "Value vaut : %d || ", *value);
        return 0;
    }
    else if (mem->be == 1)
    {
        value = (mem->donnees + (mem->taille - address - 1));
        return 0;
    }
    else
    {
        return 0;
>>>>>>> Add start of memory read
    }
}

int memory_read_half(memory mem, uint32_t address, uint16_t *value)
{
<<<<<<< HEAD
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
=======
    return -1;
>>>>>>> Add start of memory read
}

int memory_read_word(memory mem, uint32_t address, uint32_t *value)
{
<<<<<<< HEAD
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
=======
    if (mem->be == 0)
    {
        fprintf(stderr, "\n------------------------------------------\n\n");
        fprintf(stderr, "Indien : %d || ", mem->be);
        fprintf(stderr, "Byte : %i || ", address);
        fprintf(stderr, "Taille : %ld || ", mem->taille);
        fprintf(stderr, "L'adresse dans la mémoire avant est : %d\n\n", *(mem->donnees + (mem->taille - address - 1)));
        
        int i = 0;
        uint8_t v = 0;
        while(i < 4){
            memory_read_byte(mem, i, &v);
            fprintf(stderr, "Byte n°%i : %i\n", i, v);
            i++;

        }
        fprintf(stderr, "\n\nL'adresse de base est : %ls", value);
        fprintf(stderr, "L'adresse dans la mémoire est : %d\n", *(mem->donnees + address));
        fprintf(stderr, "\n------------------------------------------\n\n");
        return -1;
    }
    else if (mem->be == 1)
    {
        fprintf(stderr, "Indien : %d || ", mem->be);
        fprintf(stderr, "Byte : %i || ", address);
        fprintf(stderr, "Taille : %ld || ", mem->taille);
        fprintf(stderr, "Byte BE : %ld || ", (mem->taille - address - 1));
        fprintf(stderr, "L'adresse dans la mémoire avant est : %d || ", *(mem->donnees + (mem->taille - address - 1)));
        fprintf(stderr, "L'adresse de base est : %ls || ", value);
        // *(mem->donnees + address) = value;
        fprintf(stderr, "L'adresse dans la mémoire est : %d\n", *(mem->donnees + address));
        return -1;
    }
    else
    {
>>>>>>> Add start of memory read
        return -1;
    }
}

int memory_write_byte(memory mem, uint32_t address, uint8_t value)
{
<<<<<<< HEAD
    if(address < memory_get_size(mem)){
        *(mem->data + address) = value;
        return 0;
    } else {
=======
    if (mem->be == 0)
    {
        // fprintf(stderr, "L'adresse dans la mémoire avant est : %d || ", *(mem->donnees + address));
        // fprintf(stderr, "L'adresse de base est : %d || ", value);
        *(mem->donnees + address) = value;
        // fprintf(stderr, "L'adresse dans la mémoire est : %d\n", *(mem->donnees + address));
        return *(mem->donnees + address) == value ? 0 : -1;
    }
    else if (mem->be == 1)
    {
        // fprintf(stderr, "Indien : %d || ", mem->be);
        // fprintf(stderr, "Byte : %d || ", address);
        // fprintf(stderr, "Taille : %ld || ", mem->taille);
        // fprintf(stderr, "Byte BE : %ld || ", (mem->taille - address - 1));
        // fprintf(stderr, "L'adresse dans la mémoire avant est : %d || ", *(mem->donnees + (mem->taille - address - 1)));
        // fprintf(stderr, "L'adresse de base est : %d || ", value);
        *(mem->donnees + (mem->taille - address - 1)) = value;
        // fprintf(stderr, "L'adresse dans la mémoire est : %d\n", *(mem->donnees + (mem->taille - address - 1)));
        return *(mem->donnees + (mem->taille - address - 1)) == value ? 0 : -1;
    }
    else
    {
>>>>>>> Add start of memory read
        return -1;
    }
}

int memory_write_half(memory mem, uint32_t address, uint16_t value)
{
<<<<<<< HEAD
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
=======
    return -1;
>>>>>>> Add start of memory read
}

int memory_write_word(memory mem, uint32_t address, uint32_t value)
{
<<<<<<< HEAD
    if(address < memory_get_size(mem)){
        uint8_t value2 = 0;
        for (int i = address; i < sizeof(uint32_t); i++)
        {
            value2 = !mem->be ? (value + i) >> 8 * i : (value + (sizeof(uint32_t) - i - 1)) >> 8 * (sizeof(uint32_t) - i - 1);
            memory_write_byte(mem, i, value2);
        }
        return 0;
    } else {
=======
    if (mem->be == 0)
    {
        // fprintf(stderr, "L'adresse dans la mémoire avant est : %d || ", *(mem->donnees + address));
        // fprintf(stderr, "L'adresse de base est : %d || ", value);
        // *(mem->donnees + address) = value;
        // fprintf(stderr, "L'adresse dans la mémoire est : %d\n", *(mem->donnees + address));
        // return *(mem->donnees + address) == value ? 0 : -1;
        return -1;
    }
    else if (mem->be == 1)
    {
        // fprintf(stderr, "Indien : %d || ", mem->be);
        // fprintf(stderr, "Byte : %d || ", address);
        // fprintf(stderr, "Taille : %ld || ", mem->taille);
        // fprintf(stderr, "Byte BE : %ld || ", (mem->taille - address - 1));
        // fprintf(stderr, "L'adresse dans la mémoire avant est : %d || ", *(mem->donnees + (mem->taille - address - 1)));
        // fprintf(stderr, "L'adresse de base est : %d || ", value);
        // *(mem->donnees + (mem->taille - address - 1)) = value;
        // fprintf(stderr, "L'adresse dans la mémoire est : %d\n", *(mem->donnees + (mem->taille - address - 1)));
        // return *(mem->donnees + (mem->taille - address - 1)) == value ? 0 : -1;
        return -1;
    }
    else
    {
>>>>>>> Add start of memory read
        return -1;
    }
}
