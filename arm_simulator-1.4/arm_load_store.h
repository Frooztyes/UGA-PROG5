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
#ifndef __ARM_LOAD_STORE_H__
#define __ARM_LOAD_STORE_H__
#include <stdint.h>
#include "arm_core.h"

/* Compte le nombre de bit à 1 dans un champ de bit.
*/
int number_of_set_bits_in(uint32_t bitfield);

/* Calcul de l’adresse à partir de l’instruction et du registre pour le stockage ou le chargement d’un mot ou d’un octet non signé dans la mémoire.
*/
uint32_t address_calculator(arm_core p, uint32_t offset_12, uint32_t Rn, uint8_t U, uint8_t P, uint8_t W, uint8_t bit_25, uint32_t cond);

/* Calcul de l’adresse à partir de l’instruction et du registre pour le stockage ou le chargement d’un demi-mot, d’un octet signé ou d’un double mot dans la mémoire.
*/
uint32_t misc_address_calculator(arm_core p, uint32_t offset_12, uint32_t Rn, uint8_t U, uint8_t B, uint8_t W, uint8_t P, uint32_t cond);

/* Calcule et modifie les valeurs des paramètres start_address et end_address grâce aux valeurs de l’instruction et du registre. 
Cette fonction permet de charger ou stocker un ensemble de registre dans la mémoire ou inversement.
*/
void mult_address_calculator(arm_core p, uint32_t *start_address, uint32_t *end_address, uint32_t type_modes, uint32_t Rn, uint32_t cond, uint32_t register_list, uint8_t W);

int arm_load_store(arm_core p, uint32_t ins);
int arm_load_store_multiple(arm_core p, uint32_t ins);
int arm_coprocessor_load_store(arm_core p, uint32_t ins);

#endif
