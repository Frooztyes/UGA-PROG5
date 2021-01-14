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
#include "cond_shift_calculator.h"

uint32_t get_rs_bits(arm_core p, uint32_t rs, uint8_t max, uint8_t min) {
	return get_bits(arm_read_register(p, rs), max, min);
}

uint32_t get_rm_bit(arm_core p, uint32_t rm, uint8_t ind) {
	return get_bit(arm_read_register(p, rm), ind);
}

int carryFrom(uint64_t res) {
	return get_bit(res >> 32, 0);
}

int overflowFrom(uint32_t operande1, uint32_t operande2, uint64_t res, int operation) {
	if(operation == ADD)
	{
		return (get_bit(operande1, 31) == get_bit(operande2, 31) && get_bit(operande1, 31) != get_bit(res, 31));
	}
	else if(operation == SUB) 
	{
		return (get_bit(operande1, 31) != get_bit(operande2, 31) && get_bit(operande1, 31) != get_bit(res, 31));
	}
	return 0;
}

int borrowFrom(uint32_t res) {
	return res < 0;
}

void update_flags(arm_core p, uint8_t z, uint8_t n, uint8_t c, uint8_t v){
	z ? arm_write_cpsr(p, (arm_read_cpsr(p) | 1 << Z)) : arm_write_cpsr(p, (arm_read_cpsr(p) & ~(1 << Z)));
	n ? arm_write_cpsr(p, (arm_read_cpsr(p) | 1 << N)) : arm_write_cpsr(p, (arm_read_cpsr(p) & ~(1 << N)));
	c ? arm_write_cpsr(p, (arm_read_cpsr(p) | 1 << C)) : arm_write_cpsr(p, (arm_read_cpsr(p) & ~(1 << C)));
	v ? arm_write_cpsr(p, (arm_read_cpsr(p) | 1 << V)) : arm_write_cpsr(p, (arm_read_cpsr(p) & ~(1 << V)));
}

void teq_process(arm_core p, uint8_t rn, uint8_t rd, uint8_t s, uint32_t shift_value,  uint32_t shifter_carry_out) {
	
	// Update flags after Rn EOR shifter_operand

	uint32_t alu_out;
	uint8_t n, z, v, c;

	alu_out = arm_read_register(p,rn) ^ shift_value;
	arm_write_register(p, rd, alu_out);

	n = get_bit(alu_out, 31);
	z = rd == 0 ? 1 : 0;
	c = shifter_carry_out;
	v = get_V_flag(p);	
	update_flags(p, z, n, c, v);
}

void tst_process(arm_core p, uint8_t rn, uint8_t rd, uint8_t s, uint32_t shift_value,  uint32_t shifter_carry_out) {
	
	// Update flags after Rn AND shifter_operand
	uint32_t alu_out;
	uint8_t n, z, v, c;

	alu_out = arm_read_register(p, rn) & shift_value;
	arm_write_register(p, rd, alu_out);

	n = get_bit(alu_out, 31);
	z = rd == 0 ? 1 : 0;
	c = shifter_carry_out;
	v = get_V_flag(p);	
	update_flags(p, z, n, c, v);
}

void and_process(arm_core p, uint8_t rn, uint8_t rd, uint8_t s, uint32_t shift_value, uint32_t shifter_carry_out){
	// Rd := Rn AND shifter_operand
	uint8_t z, n, c, v;
	uint32_t reg_new;
	// read the value of rn in the register and do bit to bit with shifter operand
	reg_new = arm_read_register(p, rn) & shift_value;
	arm_write_register(p, rd, reg_new);

	if(s == 1 && rd == arm_read_register(p, 15)) {
		if(arm_current_mode_has_spsr(p)) {
			arm_write_cpsr(p, arm_read_spsr(p));
		} else {
			// UNPREDICTABLE;
		}
	} else if(s == 1) {
		n = get_bit(reg_new, 31);
		z = rd == 0 ? 1 : 0;
		c = shifter_carry_out;
		v = get_V_flag(p);
		update_flags(p, z, n, c, v);
	}
}

void eor_process(arm_core p, uint8_t rn, uint8_t rd, uint8_t s, uint32_t shift_value, uint32_t shifter_carry_out)
{
	//Rd := Rn EOR shifter_operand
	uint32_t reg_new;
	uint8_t n,c,z,v;
	reg_new = arm_read_register(p,rn) ^ shift_value;
    arm_write_register(p,rd, reg_new);

	if(s == 1 && rd == arm_read_register(p, 15)) {
		if(arm_current_mode_has_spsr(p)) {
			arm_write_cpsr(p, arm_read_spsr(p));
		} else {
			// UNPREDICTABLE;
		}
	} else if(s == 1) {
		n = get_bit(reg_new, 31);
		z = rd == 0 ? 1 : 0;
		c = shifter_carry_out;
		v = get_V_flag(p);
		update_flags(p, z, n, c, v);
	}
}

void sub_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s) {
	//Rd := Rn - shifter_operand
	uint32_t reg_new;
	uint8_t n,c,z,v;
	reg_new = arm_read_register(p,rn) - shift_value;
    arm_write_register(p,rd, reg_new);

	if(s == 1 && rd == arm_read_register(p, 15)) {
		if(arm_current_mode_has_spsr(p)) {
			arm_write_cpsr(p, arm_read_spsr(p));
		} else {
			// UNPREDICTABLE;
		}
	} else if(s == 1) {
		n = get_bit(reg_new, 31);
		z = rd == 0 ? 1 : 0;
		c = !borrowFrom(arm_read_register(p,rn) - shift_value);
		v = overflowFrom(arm_read_register(p,rn), shift_value, reg_new, SUB);
		update_flags(p, z, n, c, v);
	}
	
}

void rsb_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s) {
	// Rd := shifter_operand - Rn
	uint32_t reg_new;
	uint8_t n,z,c,v;
	
	reg_new = shift_value - arm_read_register(p,rn);
    arm_write_register(p,rd, reg_new);

	if(s == 1 && rd == arm_read_register(p, 15)) {
		if(arm_current_mode_has_spsr(p)) {
			arm_write_cpsr(p, arm_read_spsr(p));
		} else {
			// UNPREDICTABLE
		}
	} else if(s == 1) {
		n = get_bit(reg_new, 31);
		z = rd == 0 ? 1 : 0;
		c = !borrowFrom(shift_value - arm_read_register(p,rn));
		v = overflowFrom(shift_value, arm_read_register(p,rn), reg_new, SUB);
		update_flags(p, z, n, c, v);
	}
}

void add_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s) {
	// Rd = Rn + Shift_value
	uint8_t z, n, c, v;
	uint32_t reg_new;
	reg_new = arm_read_register(p,rn) + shift_value;
	arm_write_register(p,rd, reg_new);

	if(s == 1 && rd == arm_read_register(p, 15)) {
		if(arm_current_mode_has_spsr(p)) {
			arm_write_cpsr(p, arm_read_spsr(p));
		} else {
			//return UNDEFINED_INSTRUCTION;
		}
	} else if(s == 1) {
		n = get_bit(reg_new, 31);
		z = rd == 0 ? 1 : 0;
		c = carryFrom(((uint64_t)arm_read_register(p,rn)) + ((uint64_t) shift_value));
		v = overflowFrom(arm_read_register(p,rn), shift_value, reg_new, ADD);
		update_flags(p, z, n, c, v);
		}
}

void adc_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s) {
	// Rd = Rn + Shift_value + Carry Flag
	uint8_t z,n,c,v;
	uint32_t reg_new;
	reg_new = arm_read_register(p,rn) + shift_value + get_C_flag(p);
	arm_write_register(p, rd, reg_new);

	if(s == 1 && rd == arm_read_register(p, 15)) {
		if(arm_current_mode_has_spsr(p)) {
			arm_write_cpsr(p, arm_read_spsr(p));
		} else {
			// UNPREDICTABLE
		}
	} else if(s == 1) {
		z = rd == 0 ? 1 : 0;
		n = get_bit(reg_new, 31);
		c = carryFrom(((uint64_t)arm_read_register(p,rn)) + ((uint64_t) (shift_value + get_C_flag(p))));
		v = overflowFrom(arm_read_register(p,rn), shift_value + get_C_flag(p), reg_new, ADD);
		update_flags(p, z, n, c, v);
	}   
}

void sbc_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s) {
	// Rd = Rn + Shift_value - NOT(Carry flag)
	uint32_t reg_new;
	uint8_t n,c,v,z;
	
	reg_new = arm_read_register(p,rn) - shift_value - ~(get_C_flag(p));
    arm_write_register(p,rd, reg_new);

	if(s == 1 && rd == arm_read_register(p, 15)) {
		if(arm_current_mode_has_spsr(p)) {
			arm_write_cpsr(p, arm_read_spsr(p));
		} else {
			// UNPREDICTABLE
		}
	} else if(s == 1) {
		z = rd == 0 ? 1 : 0;
		n = get_bit(reg_new, 31);
		c = !borrowFrom(arm_read_register(p,rn) - shift_value - ~(get_C_flag(p)));
		v = overflowFrom(arm_read_register(p,rn), shift_value - ~get_C_flag(p), reg_new, SUB);
		update_flags(p, z, n, c, v);
	}   
}

void rsc_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s) {
	// Rd = Rn + Shift_value - NOT(Carry flag)
	uint32_t reg_new;
	uint8_t n, c, v, z;
	
	reg_new = shift_value - arm_read_register(p,rn) - !(get_C_flag(p));
    arm_write_register(p,rd, reg_new);

	if(s == 1 && rd == arm_read_register(p, 15)) {
		if(arm_current_mode_has_spsr(p)) {
			arm_write_cpsr(p, arm_read_spsr(p));
		} else {
			// UNPREDICTABLE
		}
	} else if(s == 1) {
		z = rd == 0 ? 1 : 0;
		n = get_bit(reg_new, 31);
		c = !borrowFrom(shift_value - arm_read_register(p,rn) - !(get_C_flag(p)));
		v = overflowFrom(shift_value, arm_read_register(p,rn) - !(get_C_flag(p)), reg_new, SUB);
		update_flags(p, z, n, c, v);
	}   
}

void orr_process(arm_core p, uint8_t rn, uint8_t rd, int s, uint32_t shift_value, uint32_t shifter_carry_out) {
	// Rd = Rn OR Shift_value
	uint32_t reg_new;
	uint8_t n,c,v,z;

	reg_new = arm_read_register(p,rn) | shift_value;
    arm_write_register(p,rd, reg_new);

	if(s == 1 && rd == arm_read_register(p, 15)) {
		if(arm_current_mode_has_spsr(p)) {
			arm_write_cpsr(p, arm_read_spsr(p));
		} else {
			//UNPREDICTABLE
		}
	} else if(s == 1) {
		z = rd == 0 ? 1 : 0;
		n = get_bit(reg_new, 31);
		c = shifter_carry_out;
		v = get_V_flag(p);
		update_flags(p, z, n, c, v);
	}
}

void mov_process(arm_core p, uint8_t rd, int s, uint32_t shift_value, uint32_t shifter_carry_out) {
	// Rd = Shift_value
	uint32_t reg_new;
	int n, c, v, z;

	reg_new = shift_value;
    arm_write_register(p,rd, reg_new);

	if(s == 1 && rd == arm_read_register(p, 15)) {
		if(arm_current_mode_has_spsr(p)) {
			arm_write_cpsr(p, arm_read_spsr(p));
		} else {
			// UNPREDICTABLE
		}
	} else if(s == 1) {
		z = rd == 0 ? 1 : 0;
		n = get_bit(reg_new, 31);
		c = shifter_carry_out;
		v = get_V_flag(p);
		update_flags(p, z, n, c, v);
	}
}

void bic_process(arm_core p, uint8_t rn, uint8_t rd, int s, uint32_t shift_value, uint32_t shifter_carry_out) {
	// Rd = Rn AND NOT(Shift_value)
	uint32_t reg_new;
	int n, z, c, v;
	
	reg_new = arm_read_register(p,rn) & ~shift_value;
    arm_write_register(p,rd, reg_new);

	if(s == 1 && rd == arm_read_register(p, 15)) {
		if(arm_current_mode_has_spsr(p)) {
			arm_write_cpsr(p, arm_read_spsr(p));
		} else {
			// UNPREDICTABLE
		}
	} else if(s == 1) {
		z = rd == 0 ? 1 : 0;
		n = get_bit(reg_new, 31);
		c = shifter_carry_out;
		v = get_V_flag(p);
		update_flags(p, z, n, c, v);
	}
}

void mvn_process(arm_core p, uint8_t rd, int s, uint32_t shift_value, uint32_t shifter_carry_out) {
	// Rd = NOT(Shift_value)
	uint32_t reg_new;
	uint8_t n, z, c, v;

	reg_new = ~shift_value;
    arm_write_register(p,rd, reg_new);

	if(s == 1 && rd == arm_read_register(p, 15)) {
		if(arm_current_mode_has_spsr(p)) {
			arm_write_cpsr(p, arm_read_spsr(p));
		} else {
			//UNPREDICTABLE
		}
	} else if(s == 1) {
		z = rd == 0 ? 1 : 0;
		n = get_bit(reg_new, 31);
		c = shifter_carry_out;
		v = get_V_flag(p);
		update_flags(p, z, n, c, v);
	}

}

void cmp_process(arm_core p, uint8_t rn, uint32_t shift_value) {
	
	/*
	alu_out = Rn - shifter_operand
	N Flag = alu_out[31]
	Z Flag = if alu_out == 0 then 1 else 0
	C Flag = NOT BorrowFrom(Rn - shifter_operand)
	V Flag = OverflowFrom(Rn - shifter_operand)
	*/

	uint32_t alu_out;
	uint8_t z, n, c, v;

	alu_out = arm_read_register(p, rn) - shift_value;
	arm_write_register(p, rn, alu_out);
	
	n = get_bit(alu_out, 31);
	z = alu_out == 0 ? 1 : 0;
	c = !borrowFrom(arm_read_register(p, rn) - shift_value);
	v = overflowFrom(arm_read_register(p, rn), shift_value, alu_out, SUB);
	update_flags(p, z, n, c, v);
}

void cmn_process(arm_core p, uint8_t rn, uint32_t shift_value) {

	/*
	alu_out = Rn + shifter_operand
	N Flag = alu_out[31]
	Z Flag = if alu_out == 0 then 1 else 0
	C Flag = CarryFrom(Rn + shifter_operand)
	V Flag = OverflowFrom(Rn + shifter_operand)
	*/

	uint32_t alu_out;
	uint8_t z, n, c, v;

	alu_out = arm_read_register(p, rn) + shift_value;
	arm_write_register(p, rn, alu_out);
	
	n = get_bit(alu_out, 31);
	z = alu_out == 0 ? 1 : 0;
	c = carryFrom(((uint64_t)arm_read_register(p,rn)) + ((uint64_t) shift_value));
	v = overflowFrom(arm_read_register(p, rn), shift_value, alu_out, ADD);
	update_flags(p, z, n, c, v);
}

int arm_data_processing_shift(arm_core p, uint32_t ins) {
	fprintf(stderr, "In arm_data_processing_shift\n");
	int cond, rn, rd, opcode, s;
	uint8_t shift_amnt, rm, rs, I, shift_op;
	uint32_t shift_value, shifter_carry_out;

	opcode = get_bits(ins, 24, 21);

	// destination register
	rd = get_bits(ins, 15, 12);
	// shifter operand
	shift_op = get_bits(ins, 6, 5);
	I = get_bit(ins, 4);
	shift_amnt = get_bits(ins, 11, 7);
	rs = get_bits(ins, 11, 6);
	rm = get_bits(ins, 3, 0);
	shifter_value_calculator(&shift_value, &shifter_carry_out, p, I, shift_op, rs, rm, shift_amnt);

	
	rn = get_bits(ins, 19, 16);
	s = get_bit(ins, 20);
	cond = get_bits(ins, 31, 28);
	
	if(cond_verifier(p, cond)) {
		switch (opcode)
		{
			case 0:
				// Logical AND
				fprintf(stderr, "In and_process\n");
				and_process(p, rn, rd, s, shift_value, shifter_carry_out);
				break;

			case 1:
				// Logical exclusive OR
				fprintf(stderr, "In eor_process\n");
				eor_process(p, rn, rd, s, shift_value, shifter_carry_out);
				break;

			case 2:
				// Substract
				fprintf(stderr, "In sub_process\n");
				sub_process(p, rn, rd, shift_value, s);
				break;

			case 3:
				fprintf(stderr, "In rsc_process\n");
				rsc_process(p, rn, rd, shift_value, s);
				// Reverse substract
				break;

			case 4:
				// Add
				fprintf(stderr, "In add_process\n");
				add_process(p, rn, rd, shift_value, s);
				break;

			case 5:
				// Add with carry
				fprintf(stderr, "In adc_process\n");
				adc_process(p, rn, rd, shift_value, s);
				break;

			case 6:
				// Substract with carry
				fprintf(stderr, "In sbc_process\n");
				sbc_process(p, rn, rd, shift_value, s);
				break;

			case 7:
				// Reverse substract with carry
				fprintf(stderr, "In rsc_process\n");
				rsc_process(p, rn, rd, shift_value, s);
				break;

			case 8:
				// Test
				fprintf(stderr, "In tst_process\n");
				tst_process(p, rn, rd, s, shift_value, shifter_carry_out);
				break;

			case 9:
				// Test equivalence
				fprintf(stderr, "In teq_process\n");
				teq_process(p, rn, rd, s, shift_value, shifter_carry_out);
				break;

			case 10:
				// Compare 
				fprintf(stderr, "In cmp_process\n");
				cmp_process(p, rn, shift_value);
				break;
				
			case 11:
				// Compare negated
				fprintf(stderr, "In cmn_process\n");
				cmn_process(p, rn, shift_value);
				break;

			case 12:
				// Logical (inclusive) OR
				fprintf(stderr, "In orr_process\n");
				orr_process(p, rn, rd, s, shift_value, shifter_carry_out);
				break;

			case 13:
				// Move
				fprintf(stderr, "In mov_process\n");
				mov_process(p, rd, s, shift_value, shifter_carry_out);
				break;

			case 14:
				// Bit clear
				fprintf(stderr, "In bic_process\n");
				bic_process(p, rn, rd, s, shift_value, shifter_carry_out);
				break;

			case 15:
				// Move not
				fprintf(stderr, "In mvn_process\n");
				mvn_process(p, rd, s, shift_value, shifter_carry_out);
				break;

			default:
				fprintf(stderr, "Default\n");
				break;
		}
	}
    return 0;
}

int arm_data_processing_immediate_msr(arm_core p, uint32_t ins) {
	fprintf(stderr, "In arm_data_processing_immediate_msr\n");

	int cond, rn, rd, opcode, s;
	uint8_t rotate_imm;
	uint32_t shift_value, immed_8, shifter_carry_out;

	opcode = get_bits(ins, 24, 21);

	rd = get_bits(ins, 15, 12);
	rn = get_bits(ins, 19, 16);
	s = get_bit(ins, 20);
	cond = get_bits(ins, 31, 28);

 	rotate_imm = get_bits(ins, 11, 8);
    immed_8 = get_bits(ins, 7, 0);
    shift_value = ror(immed_8, rotate_imm * 2);
	if(rotate_imm == 0) {
		shifter_carry_out = get_C_flag(p);
	} else {
		shifter_carry_out = get_bit(shift_value, 31);
	}

    if(cond_verifier(p, cond)) {
    	switch(opcode) {
			case 0:
				// Logical AND
				and_process(p, rn, rd, s, shift_value, shifter_carry_out);
				break;

			case 1:
				// Logical exclusive OR
				eor_process(p, rn, rd, s, shift_value, shifter_carry_out);
				break;

			case 2:
				// Substract
				sub_process(p, rn, rd, shift_value, s);
				break;

			case 3:
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
				tst_process(p, rn, rd, s, shift_value, shifter_carry_out);
				break;

			case 9:
				// Test equivalence
				teq_process(p, rn, rd, s, shift_value, shifter_carry_out);
				break;

			case 10:
				// Compare 
				cmp_process(p, rn, shift_value);
				break;
				
			case 11:
				// Compare negated
				cmn_process(p, rn, shift_value);
				break;

			case 12:
				// Logical (inclusive) OR
				orr_process(p, rn, rd, s, shift_value, shifter_carry_out);
				break;

			case 13:
				// Move
				mov_process(p, rd, s, shift_value, shifter_carry_out);
				break;

			case 14:
				// Bit clear
				bic_process(p, rn, rd, s, shift_value, shifter_carry_out);
				break;

			case 15:
				// Move not
				mvn_process(p, rd, s, shift_value, shifter_carry_out);
				break;
		}
	}
    return 0;
}
