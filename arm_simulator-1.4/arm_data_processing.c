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
#include "arm_data_processing.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_branch_other.h"
#include "util.h"
#include "debug.h"

uint32_t shift(arm_core p, uint8_t shift,uint8_t shift_imm, uint8_t Rm,uint8_t Rs,uint8_t I){

	int32_t Rm_value = arm_read_register(p,Rm);
	uint32_t Rs_value;


	if(I){
		Rs_value = arm_read_register(p,Rs);

		
	}
	else{

	}

}

int and_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_operand) {
	// Rd := Rn AND shifter_operand
	uint32_t reg_new;

	// read the value of rn in the register and do bit to bit with shifter operand
	reg_new = arm_read_register(p, rn) & shift_operand;
	arm_write_register(p, rd, reg_new);
	return 0;
}

void eor_processing(arm_core p, uint8_t rn, uint8_t rd, uint32_t val_sh,  int s)
{
	//Rd := Rn EOR shifter_operand
	uint32_t resultat;
}

void substract_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t val_shift, int s) {
	//Rd := Rn - shifter_operand
    uint32_t resultat;
	
}


int reverse_sub_processing(arm_core p, uint8_t rn, uint8_t rd, uint32_t val_sh, int s){
	// Rd := shifter_operand - Rn



	return 0;
}


}
/* Decoding functions for different classes of instructions */
int arm_data_processing_shift(arm_core p, uint32_t ins) {
	fprintf(stderr, "In arm_data_processing_shift\n");
	int cond, rn, rd, opcode, s;
	uint8_t shift_amnt, rm;
	uint32_t val_shift;

	cond = get_bits(ins, 31, 28);
	opcode = get_bits(ins, 24, 21);

	// source register
	rn = get_bits(ins, 19, 16);

	// destination register
	rd = get_bits(ins, 15, 12);
	

	// shifter operand
	I = get_bit(ins, 4);
	shift_amnt = get_bits(ins, 11, 7);
	s = get_bit(ins, 20);
	rn = get_bits(ins, 31, 28);
	rm = get_bits(ins, 3, 0);

	val_shift = shift();

	switch (opcode)
	{
		case 0:
			// Logical AND
			// Yanis
			and_process(p, rn, rd, val_shift);
			break;

		case 1:
			// Logical exclusive OR
			//Niconnard 
			eor_processing(p, rn, rd, val_sh, s);
			break;

		case 2:
			// Substract
			//Mathis
			substract_process(p, rn, rd, val_shift, s);
			break;

		case 3: //rémi le bg
			reverse_sub_processing();
			// Reverse substract
			break;

		case 4:
			// Add
			break;

		case 5:
			// Add with carry
			break;

		case 6:
			// Substract with carry
			break;

		case 7:
			// Reverse substract with carry
			break;

		case 8:
			// Test
			break;

		case 9:
			// Test equivalence
			break;

		case 10:
			// Compare
			break;
			
		case 11:
			// Compare negated
			break;

		case 12:
			// Logical (inclusive) OR
			break;

		case 13:
			// Move
			break;

		case 14:
			// Bit clear
			break;

		case 15:
			// Move not
			break;

		default:
			break;
	}

    return UNDEFINED_INSTRUCTION;
}

int arm_data_processing_immediate_msr(arm_core p, uint32_t ins) {
    return UNDEFINED_INSTRUCTION;
}

