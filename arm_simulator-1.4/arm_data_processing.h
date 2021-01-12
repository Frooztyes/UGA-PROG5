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
#ifndef __ARM_DATA_PROCESSING_H__
#define __ARM_DATA_PROCESSING_H__
#include <stdint.h>
#include "arm_core.h"


/* Decoding functions for different classes of instructions 
*/
int arm_data_processing_shift(arm_core p, uint32_t ins);
int arm_data_processing_immediate_msr(arm_core p, uint32_t ins);

/* Récupère la valeur du registre Rs entre les bornes max et min 
*/
uint32_t get_rs_bits(arm_core p, uint32_t rs, uint8_t max, uint8_t min);

/* Récupère la valeur du registre Rs au bit ind
*/
uint32_t get_rm_bit(arm_core p, uint32_t rm, uint8_t ind);

/* Récupère le flag N dans le registre CPSR au bit 31
*/
int get_N_flag(arm_core p);

/* Récupère le flag Z dans le registre CPSR au bit 30
*/
int get_Z_flag(arm_core p);

/* Récupère le flag C dans le registre CPSR au bit 29
*/
int get_C_flag(arm_core p);

/* Récupère le flag V dans le registre CPSR au bit 28
*/
int get_V_flag(arm_core p);

/* Modifie la valeur de shift_value et de shifter_carry_out grâce aux différentes valeurs de l'instruction
*/
void shifter_value_calculator(uint32_t *shift_value, uint32_t *shifter_carry_out, arm_core p, uint8_t I, uint8_t shift_op, uint8_t Rs, uint8_t Rm, uint32_t shift_imm);

/* TODO : DESCRIPTION */
void teq_process(arm_core p,uint8_t rn,uint32_t val_sh);

/* TODO : DESCRIPTION */
void and_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value);

/* TODO : DESCRIPTION */
void eor_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value,  int s);

/* TODO : DESCRIPTION */
void substract_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s);

/* TODO : DESCRIPTION */
void reverse_sub_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s);

/* TODO : DESCRIPTION */
void add_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s);

/* TODO : DESCRIPTION */
void adc_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s);

/* TODO : DESCRIPTION */
void sbc_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s);

/* TODO : DESCRIPTION */
void rsc_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s);

/* TODO : DESCRIPTION */
void orr_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s);

/* TODO : DESCRIPTION */
void mov_process(arm_core p, uint8_t rd, uint32_t shift_value, int s);

/* TODO : DESCRIPTION */
void bic_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s);

/* TODO : DESCRIPTION */
void mvn_process(arm_core p, uint8_t rd, uint32_t shift_value, int s);

/* TODO : DESCRIPTION */
void comp_processing(arm_core p, uint8_t rn, uint32_t shift_value);

/* TODO : DESCRIPTION */
void compN_processing(arm_core p, uint8_t rn, uint32_t shift_value);

/* Vérifie les valeurs des différents flags Z, N, V et C
en fonction des bits cond (31 à 28) dans l'instruction.
*/
int cond_verifier(arm_core p, uint32_t cond);

#endif
 