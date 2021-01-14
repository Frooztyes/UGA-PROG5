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

#define ADD 1
#define SUB 0

/* Decoding functions for different classes of instructions 
*/
int arm_data_processing_shift(arm_core p, uint32_t ins);
int arm_data_processing_immediate_msr(arm_core p, uint32_t ins);

/* Récupère la valeur du registre Rs entre les bornes max et min 
*/
uint32_t get_rs_bits(arm_core p, uint32_t rs, uint8_t max, uint8_t min);

/* Récupère la valeur du registre Rs au bit ind 
*/
uint32_t get_rm_bit(arm_core p, uint32_t rm, uint8_t ind) ;

/* TEQ (Test Equivalence) compares a register value with another arithmetic value. 
The condition flags are updated, based on the result of logically exclusive-ORing the two values, so that subsequent instructions can be conditionally executed. */
void teq_process(arm_core p, uint8_t rn, uint8_t rd, uint8_t s, uint32_t shift_value,  uint32_t shifter_carry_out);

/* TST (Test) compares a register value with another arithmetic value. 
The condition flags are updated, based on the result of logically ANDing the two values, so that subsequent instructions can be conditionally executed.*/
void tst_process(arm_core p, uint8_t rn, uint8_t rd, uint8_t s, uint32_t shift_value,  uint32_t shifter_carry_out);

/* EOR (Exclusive OR) performs a bitwise Exclusive-OR of two values. 
The first value comes from a register.
The second value can be either an immediate value or a value from a register, and can be shifted before the exclusive OR operation.*/
void eor_process(arm_core p, uint8_t rn, uint8_t rd, uint8_t s, uint32_t shift_value, uint32_t shifter_carry_out);

/* SUB (Subtract) subtracts one value from a second value.
The second value comes from a register. 
The first value can be either an immediate value or a value from a register, and can be shifted before the subtraction. */
void sub_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s);

/* RSB (Reverse Subtract) subtracts a value from a second value.
The first value comes from a register. 
The second value can be either an immediate value or a value from a register, and can be shifted before the subtraction. 
This is the reverse of the normal order of operands in ARM assembler language. */
void rsb_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s);

/* AND performs a bitwise AND of two values. 
The first value comes from a register. 
The second value can be either an immediate value or a value from a register, and can be shifted before the AND operation.*/
void and_process(arm_core p, uint8_t rn, uint8_t rd, uint8_t s, uint32_t shift_value, uint32_t shifter_carry_out);

/* ADD adds two values. 
The first value comes from a register. 
The second value can be either an immediate value or a value from a register, and can be shifted before the addition.*/
void add_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s);

/* ADC (Add with Carry) adds two values and the Carry flag. 
The first value comes from a register.
The second value can be either an immediate value or a value from a register, and can be shifted before the addition.*/
void adc_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s);

/* SBC (Subtract with Carry) subtracts the value of its second operand and the value of NOT(Carry flag) from the value of its first operand. 
The first operand comes from a register. 
The second operand can be either an immediate value or a value from a register, and can be shifted before the subtraction.*/
void sbc_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s);

/* RSC (Reverse Subtract with Carry) subtracts one value from another, taking account of any borrow from a preceding less significant subtraction. 
The normal order of the operands is reversed, to allow subtraction from a shifted register value, or from an immediate value. */
void rsc_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s);

/* ORR (Logical OR) performs a bitwise (inclusive) OR of two values. 
The first value comes from a register.
The second value can be either an immediate value or a value from a register, and can be shifted before the OR operation.*/
void orr_process(arm_core p, uint8_t rn, uint8_t rd, int s, uint32_t shift_value, uint32_t shifter_carry_out);

/* MOV (Move) writes a value to the destination register. 
The value can be either an immediate value or a value from a register, and can be shifted before the write. */
void mov_process(arm_core p, uint8_t rd, int s, uint32_t shift_value, uint32_t shifter_carry_out);

/* BIC (Bit Clear) performs a bitwise AND of one value with the complement of a second value. 
The first value comes from a register. 
The second value can be either an immediate value or a value from a register, and can be shifted before the BIC operation.*/
void bic_process(arm_core p, uint8_t rn, uint8_t rd, int s, uint32_t shift_value, uint32_t shifter_carry_out);

/* MVN (Move Not) generates the logical ones complement of a value. 
The value can be either an immediate value or a value from a register, and can be shifted before the MVN operation. */
void mvn_process(arm_core p, uint8_t rd, int s, uint32_t shift_value, uint32_t shifter_carry_out);

/* CMP (Compare) compares two values. 
The first value comes from a register. 
The second value can be either an immediate value or a value from a register, and can be shifted before the comparison.*/
void cmp_process(arm_core p, uint8_t rn, uint32_t shift_value);

/* CMN (Compare Negative) compares one value with the twos complement of a second value. 
The first value comes from a register. 
The second value can be either an immediate value or a value from a register, and can be shifted before the comparison.*/
void cmn_process(arm_core p, uint8_t rn, uint32_t shift_value);

/* Vérifie que, lors d'une addition, le résultat ne dépasse de 32 bits. 
Si c'est le cas, 1 est retournée, sinon 0.
*/
int carryFrom(uint64_t res);

/* Vérifie que lors d'une addition, les 32èmes bits de chaque opérande sont les mêmes et qu'ils sont différents du 32ème bit du résultat. 
Si cette condition est valide, 1 est retournée, sinon 0.
Vérifie que lors d'une soustraction, les 32èmes bits de chaque opérande sont différents et que le bit 32 de la première opérande soit différent du 32ème bit du résultat. 
Si cette condition est valide, 1 est retournée, sinon 0.
*/
int overflowFrom(uint32_t operande1, uint32_t operande2, uint64_t res, int operation);

/* Vérifie que lors d'une soustraction la valeur du résultat ne soit pas négative.
*/
int borrowFrom(uint32_t res);

/* Prends en paramètres les différentes valeurs des indicateurs Z N C V modifiés ou non et mets à jour les valeurs des indicateurs Z N C V dans le registre CPSR.
*/
void update_flags(arm_core p, uint8_t z, uint8_t n, uint8_t c, uint8_t v);

#endif
 