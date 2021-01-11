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


uint32_t get_rs_bits(arm_core p, uint32_t rs, uint8_t max, uint8_t min) {
	return get_bits(arm_read_register(p, rs), max, min);
}

uint32_t get_rm_bit(arm_core p, uint32_t rm, uint8_t ind) {
	return get_bit(arm_read_register(p, rm), ind);
}

uint32_t get_C_flag(arm_core p) {
	return get_bit(arm_read_cpsr(p), 29);
}


void shifter_value_calculator(uint32_t *shift_value, arm_core p, uint8_t I, uint8_t shift_op, uint8_t Rs, uint8_t Rm, uint32_t shift_imm){
	uint32_t Rs_reg, Rm_reg = arm_read_register(p, Rm);

	if(I){
		Rs_reg = arm_read_register(p, Rs);
		uint32_t rs_7_0 = get_rs_bits(p, Rs, 7, 0);
		uint32_t rm_31 = get_rm_bit(p, Rm, 31);
		uint32_t rs_4_0 = get_rs_bits(p, Rs, 4, 0);
		switch (shift_op)
		{
			case LSL: // Logical shift left
				if(rs_7_0 == 0) {
					*shift_value = Rm_reg;
					// shifter carry out = C Flag
				} else if( rs_7_0 < 32) {
					*shift_value = Rm_reg << rs_7_0;
					// shifter carry out = Rm_reg[Rs[7:0] - 1]
				}else if(rs_7_0 == 32){
					*shift_value = 0;
					// shifter carry out = Rm_reg[31]
				}else{
					*shift_value = 0;
					// shifter carry out = 0
				}
				break;

			case LSR: // Logical shift right
				if(rs_7_0 == 0){
					*shift_value = Rm_reg;
					// shifter_carry_out = C Flags
				}else if(rs_7_0 < 32){
					*shift_value = Rm_reg >> rs_7_0;
					// shifter_carry_out = Rm[Rs[7:0] - 1]
				}else if(rs_7_0 == 32){
					*shift_value = 0;
					// shifter carry out = Rm_reg[31]
				}else{
					*shift_value = 0;
					//shifter carry out = 0
				}
				break;

			case ASR: // Logical shift right
				
				if(Rs_reg == 0) {
					*shift_value = Rm_reg;
					// shifter_carry_out = C Flags
				} else if(Rs_reg < 32) {
					*shift_value = asr(Rm_reg, Rs_reg);
					// shifter_carry_out = Rm[Rs[7:0] - 1]
				} else if(Rs_reg >= 32) {
					if(rm_31 == 1) {
						*shift_value = 0;
						// shifter carry out = Rm_reg[31]
					}  else {
						*shift_value = 0xFFFFFFFF;
						//shifter carry out = 0
					}
				}
				break;

			case ROR: // Rotate right
				if(rs_7_0 == 0) {
					*shift_value = Rm_reg;
					// shifter_carry_out = C Flags
				}else if(rs_4_0 == 0){
					*shift_value = Rm_reg;
					// shifter_carry_out = Rm[31]
				} else if(rs_4_0 > 0) {
					*shift_value = ror(Rm_reg, rs_4_0);
					// shifter_carry_out = Rm[Rs[4:0] - 1]
				}
				break;
			
			default:
				*shift_value = Rm_reg;
				break;
		}
		// data pro register shift
	} else {
		switch (shift_op)
		{
			case LSL: // Immediate shift left
				if(shift_imm == 0) {
					*shift_value = Rm;
					// shifter carry out = Rm_reg[31];
				} else {
					*shift_value = Rm_reg << shift_imm;
					// shifter carry out = Rm_reg[31 - shift_imm]
				}
				break;

			case LSR: // Logical shift right
				if(shift_imm == 0){
					*shift_value = 0;
					// shifter_carry_out = Rm[31]
				}else {
					*shift_value = Rm >> shift_imm;
					// shifter_carry_out = Rm[shift_imm - 1];
				}
				break;

			case ASR: // Logical shift right
				if (shift_imm == 0)
					if (get_rm_bit (p, Rm, 31) == 0)
						*shift_value = 0;
						//shifter_carry_out = Rm[31]
					else /* Rm[31] == 1 */
						*shift_value = 0xFFFFFFFF;
						//shifter_carry_out = Rm[31]
				else /* shift_imm > 0 */
					*shift_value = asr(Rm, shift_imm);
					//shifter_carry_out = Rm[shift_imm - 1]

				break;

			case ROR: // Logical shift right
				if(shift_imm == 0){
					shift_op = (get_C_flag(p) << 31) || (Rm >> 1);
					//shifter_carry_out = Rm[0]
				}else{
					*shift_value = ror(Rm_reg, shift_imm);
					//shifter_carry_out = Rm[shift_imm - 1];
				}
				break;
			
			default:
				*shift_value = Rm_reg;
				break;
		}
		
		// data pro immediate shift  
	}
}


void and_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value) {
	// Rd := Rn AND shifter_operand
	uint32_t reg_new;

	// read the value of rn in the register and do bit to bit with shifter operand
	reg_new = arm_read_register(p, rn) & shift_value;
	arm_write_register(p, rd, reg_new);
}

void eor_processing(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value,  int s)
{
	//Rd := Rn EOR shifter_operand
	uint32_t reg_new;
	
	reg_new = arm_read_register(p,rn) ^ shift_value;
    arm_write_register(p,rd, reg_new);
}

void substract_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s) {
	//Rd := Rn - shifter_operand
	uint32_t reg_new;
	
	reg_new = arm_read_register(p,rn) - shift_value;
    arm_write_register(p,rd, reg_new);
	
}


void reverse_sub_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s){
	// Rd := shifter_operand - Rn
	uint32_t reg_new;
	
	reg_new = shift_value - arm_read_register(p,rn);
    arm_write_register(p,rd, reg_new);
}

void add_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s){
	// Rd = Rn + Shift_value
	uint32_t reg_new;
	
	reg_new = arm_read_register(p,rn) + shift_value;
    arm_write_register(p,rd, reg_new);
}

void adc_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s){
	// Rd = Rn + Shift_value + Carry Flag
	uint32_t reg_new;
	
	reg_new = arm_read_register(p,rn) + shift_value + get_C_flag(p);
    arm_write_register(p,rd, reg_new);
}

void sbc_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s){
	// Rd = Rn + Shift_value - NOT(Carry flag)
	uint32_t reg_new;
	
	reg_new = arm_read_register(p,rn) + shift_value - ~(get_C_flag(p));
    arm_write_register(p,rd, reg_new);
}

void rsc_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s){
	// Rd = Rn + Shift_value - NOT(Carry flag)
	uint32_t reg_new;
	
	reg_new = shift_value + arm_read_register(p,rn) - ~(get_C_flag(p));
    arm_write_register(p,rd, reg_new);
}

void orr_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s){
	// Rd = Rn OR Shift_value
	uint32_t reg_new;
	
	reg_new = arm_read_register(p,rn) | shift_value;
    arm_write_register(p,rd, reg_new);
}

void mov_process(arm_core p, uint8_t rd, uint32_t shift_value, int s){
	// Rd = Shift_value
	uint32_t reg_new;
	
	reg_new = shift_value;
    arm_write_register(p,rd, reg_new);
}

void bic_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s){
	// Rd = Rn AND NOT(Shift_value)
	uint32_t reg_new;
	
	reg_new = arm_read_register(p,rn) & ~shift_value;
    arm_write_register(p,rd, reg_new);
}

void mvn_process(arm_core p, uint8_t rd, uint32_t shift_value, int s){
	// Rd = NOT(Shift_value)
	uint32_t reg_new;
	
	reg_new = ~shift_value;
    arm_write_register(p,rd, reg_new);
}


/* Decoding functions for different classes of instructions */
int arm_data_processing_shift(arm_core p, uint32_t ins) {
	fprintf(stderr, "In arm_data_processing_shift\n");
	int /*cond,*/ rn, rd, opcode, s;
	uint8_t shift_amnt, rm, rs, I, shift_op;
	uint32_t shift_value;

	opcode = get_bits(ins, 24, 21);

	// destination register
	rd = get_bits(ins, 15, 12);
	// shifter operand
	shift_op = get_bits(ins, 6, 5);
	I = get_bit(ins, 4);
	shift_amnt = get_bits(ins, 11, 7);
	rs = get_bits(ins, 11, 6);
	rm = get_bits(ins, 3, 0);
	shifter_value_calculator(&shift_value, p, I, shift_op, rs, rm, shift_amnt);

	
	rn = get_bits(ins, 19, 16);
	s = get_bit(ins, 20);
	// cond = get_bits(ins, 31, 28);


	switch (opcode)
	{
		case 0:
			// Logical AND
			// Yanis
			and_process(p, rn, rd, shift_value);
			break;

		case 1:
			// Logical exclusive OR
			// NiCON
			eor_processing(p, rn, rd, shift_value, s);
			break;

		case 2:
			// Substract
			// Mathis
			substract_process(p, rn, rd, shift_value, s);
			break;

		case 3: // rémi le bg
			rsc_process(p, rn, rd, shift_value, s);
			// Reverse substract
			break;

		case 4:
			// Add
			add_process(p, rn, rd, shift_value, s);
			break;

		case 5:
			// Add with carry
			adc_process(p, rn, rd, shift_value, s);
			break;

		case 6:
			// Substract with carry
			sbc_process(p, rn, rd, shift_value, s);
			break;

		case 7:
			// Reverse substract with carry
			rsc_process(p, rn, rd, shift_value, s);
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
			orr_process(p, rn, rd, shift_value, s);
			break;

		case 13:
			// Move
			mov_process(p, rd, shift_value, s);
			break;

		case 14:
			// Bit clear
			bic_process(p, rn, rd, shift_value, s);
			break;

		case 15:
			// Move not
			mvn_process(p, rd, shift_value, s);
			break;
	}

    return UNDEFINED_INSTRUCTION;
}

int arm_data_processing_immediate_msr(arm_core p, uint32_t ins) {
    return UNDEFINED_INSTRUCTION;
}

