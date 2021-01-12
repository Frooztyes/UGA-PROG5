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

#define ADD = 1;
#define SUB = 0;

uint32_t get_rs_bits(arm_core p, uint32_t rs, uint8_t max, uint8_t min) {
	return get_bits(arm_read_register(p, rs), max, min);
}

uint32_t get_rm_bit(arm_core p, uint32_t rm, uint8_t ind) {
	return get_bit(arm_read_register(p, rm), ind);
}

int get_N_flag(arm_core p) {
	return get_bit(arm_read_cpsr(p), 31);
}

int get_Z_flag(arm_core p) {
	return get_bit(arm_read_cpsr(p), 30);
}

int get_C_flag(arm_core p) {
	return get_bit(arm_read_cpsr(p), 29);
}

int get_V_flag(arm_core p) {
	return get_bit(arm_read_cpsr(p), 28);
}

int carryFrom(uint64_t res){
	return res >> 32;
}

int overflowFrom(uint32_t operande1, uint32_t operande2, uint64_t res, int op){
	if(op == ADD){
		if(get_bit(operande1, 31) == get_bit(operande2,31) && get_bit(operande1, 31) != get_bit(res)){
			return 1;
		}else if(get_bit(operande1,31) != get_bit(operande) && get_bit(operande1,31) != get_bit(res)){
			return 1;
		}
		return 0;
	}
}


void shifter_value_calculator(uint32_t *shift_value, uint32_t *shifter_carry_out, arm_core p, uint8_t I, uint8_t shift_op, uint8_t Rs, uint8_t Rm, uint32_t shift_imm){
	uint32_t Rs_reg, Rm_reg = arm_read_register(p, Rm);
	uint32_t rm_31 = get_rm_bit(p, Rm, 31);

	
	if(I){
		Rs_reg = arm_read_register(p, Rs);
		uint32_t rs_7_0 = get_rs_bits(p, Rs, 7, 0);
		uint32_t rs_4_0 = get_rs_bits(p, Rs, 4, 0);
		switch (shift_op)
		{
			case LSL: // Logical shift left
				if(rs_7_0 == 0) {
					*shift_value = Rm_reg;
					*shifter_carry_out = get_C_flag(p);
				} else if( rs_7_0 < 32) {
					*shift_value = Rm_reg << rs_7_0;
					// shifter carry out = Rm_reg[Rs[7:0] - 1]
					*shifter_carry_out = get_rm_bit(p, Rm_reg, rs_7_0 - 1);
				}else if(rs_7_0 == 32){
					*shift_value = 0;
					// shifter carry out = Rm_reg[31]
					*shifter_carry_out = rm_31;
				}else{
					*shift_value = 0;
					// shifter carry out = 0
					*shifter_carry_out = 0;
				}
				break;

			case LSR: // Logical shift right
				if(rs_7_0 == 0){
					*shift_value = Rm_reg;
					// shifter_carry_out = C Flags
					*shifter_carry_out = get_C_flag(p);
				}else if(rs_7_0 < 32){
					*shift_value = Rm_reg >> rs_7_0;
					// shifter_carry_out = Rm[Rs[7:0] - 1]
					*shifter_carry_out = get_rm_bit(p, Rm_reg, rs_7_0 - 1);
				}else if(rs_7_0 == 32){
					*shift_value = 0;
					// shifter carry out = Rm_reg[31]
					*shifter_carry_out = rm_31;
				}else{
					*shift_value = 0;
					//shifter carry out = 0
					*shifter_carry_out = 0;
				}
				break;

			case ASR: // Logical shift right
				
				if(Rs_reg == 0) {
					*shift_value = Rm_reg;
					// shifter_carry_out = C Flags
					*shifter_carry_out = get_C_flag(p);
				} else if(Rs_reg < 32) {
					*shift_value = asr(Rm_reg, Rs_reg);
					// shifter_carry_out = Rm[Rs[7:0] - 1]
					*shifter_carry_out = get_rm_bit(p, Rm_reg, rs_7_0 - 1);
				} else if(Rs_reg >= 32) {
					if(rm_31 == 1) {
						*shift_value = 0;
						// shifter carry out = Rm_reg[31]
						*shifter_carry_out = rm_31;
					}  else {
						*shift_value = 0xFFFFFFFF;
						//shifter carry out = 0
						*shifter_carry_out = 0;
					}
				}
				break;

			case ROR: // Rotate right
				if(rs_7_0 == 0) {
					*shift_value = Rm_reg;
					// shifter_carry_out = C Flags
					*shifter_carry_out = get_C_flag(p);
				}else if(rs_4_0 == 0){
					*shift_value = Rm_reg;
					// shifter_carry_out = Rm[31]
					*shifter_carry_out = rm_31;
				} else if(rs_4_0 > 0) {
					*shift_value = ror(Rm_reg, rs_4_0);
					// shifter_carry_out = Rm[Rs[4:0] - 1]
					*shifter_carry_out = get_rm_bit(p, Rm_reg, rs_4_0 - 1);
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
					*shifter_carry_out = rm_31;
				} else {
					*shift_value = Rm_reg << shift_imm;
					// shifter carry out = Rm_reg[31 - shift_imm]
					*shifter_carry_out = get_rm_bit(p, Rm_reg, 31 - shift_imm);
				}
				break;

			case LSR: // Logical shift right
				if(shift_imm == 0){
					*shift_value = 0;
					// shifter_carry_out = Rm[31]
					*shifter_carry_out = rm_31;
				}else {
					*shift_value = Rm >> shift_imm;
					// shifter_carry_out = Rm[shift_imm - 1];
					*shifter_carry_out = get_rm_bit(p, Rm_reg, shift_imm - 1);
				}
				break;

			case ASR: // Logical shift right
				if (shift_imm == 0) {
					if (get_rm_bit (p, Rm, 31) == 0) {
						*shift_value = 0;
						//shifter_carry_out = Rm[31]
						*shifter_carry_out = rm_31;
					} else { /* Rm[31] == 1 */
						*shift_value = 0xFFFFFFFF;
						//shifter_carry_out = Rm[31]
						*shifter_carry_out = rm_31;
					}
				} else {/* shift_imm > 0 */
					*shift_value = asr(Rm, shift_imm);
					//shifter_carry_out = Rm[shift_imm - 1]
					*shifter_carry_out = get_rm_bit(p, Rm_reg, shift_imm - 1);
				}
				break;

			case ROR: // Logical shift right
				if(shift_imm == 0){
					shift_op = ( ((uint32_t) get_C_flag(p)) << 31) || (Rm >> 1);
					//shifter_carry_out = Rm[0]
					*shifter_carry_out = get_rm_bit(p, Rm_reg, 0);
				}else{
					*shift_value = ror(Rm_reg, shift_imm);
					//shifter_carry_out = Rm[shift_imm - 1];
					*shifter_carry_out = get_rm_bit(p, Rm_reg, shift_imm - 1);
				}
				break;
			
			default:
				*shift_value = Rm_reg;
				break;
		}
		// data pro immediate shift  
	}
}

void teq_process(arm_core p,uint8_t rn,uint32_t shift_value){
	
	// Update flags after Rn EOR shifter_operand

	uint32_t reg_new, rn;
	int N, Z, V, C;

	rn = arm_read_register(p,rn);

	reg_new = rn ^ shift_value;
	
	// Get new flags value
	N = get_bit(reg_new, 31);
	Z = if reg_new == 0 then 1 else 0;
	V = get_V_flag();
	C = shifter_carry_out;
	
	// Update flags
	update_flags(p, Z, N, C, V);
	
	
}

void and_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value) {
	// Rd := Rn AND shifter_operand
	uint32_t reg_new;

	// read the value of rn in the register and do bit to bit with shifter operand
	reg_new = arm_read_register(p, rn) & shift_value;
	arm_write_register(p, rd, reg_new);
}

void eor_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value,  int s)
{
	//Rd := Rn EOR shifter_operand
	uint32_t reg_new;
	
	reg_new = arm_read_register(p,rn) ^ shift_value;
    arm_write_register(p,rd, reg_new);

	if(s == 1 && rd == 15){
		if(arm_current_mode_has_spsr(p)){
			arm_write_cpsr(p, arm_read_spsr(p));
		}else{
			//return UNDEFINED_INSTRUCTION;
		}
	} 
	else if(s == 1){
		teq_process(p,rn,shift_value);
	}
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
	uint8_t z,n,c,v;
	uint32_t reg_new;
	reg_new = arm_read_register(p,rn) + shift_value;
	arm_write_register(p,rd, reg_new);
	// pour le S je sais pas trop ce que c'est pas contre, S = ?? (a quoi il correspond)
	//moi non plus
	if(s == 1 && rd == 15){
		if(arm_current_mode_has_spsr(p)){
			arm_write_cpsr(p, arm_read_spsr(p));
		}else{
			//return UNDEFINED_INSTRUCTION;
		}
	} else if(s == 1){
		if(rd == 0){
			z = 1;
		}else{
			z = 0;
		}
		n = get_bit(reg_new, 31);
		c = carryFrom(reg_new);
		v = overflowFrom(arm_read_register(p,rn),shift_value,reg_new,ADD);
	}
		
}

/*
A revoir surtout pour OverflowFrom c'est un peu bizarre -> Vincent
*/

void adc_process(arm_core p, uint8_t rn, uint8_t rd, uint32_t shift_value, int s){
	// Rd = Rn + Shift_value + Carry Flag
	uint8_t z,n,c,v;
	uint32_t reg_new;
	reg_new = arm_read_register(p,rn) + shift_value + get_C_flag(p);
	arm_write_register(p,rd, reg_new);
	if(s == 1 && rd == 15){
		if(arm_current_mode_has_spsr(p)){
			arm_write_cpsr(p, arm_write_spsr(p));
		}else{
			//UNPREDICTABLE
		}
	}else if(s == 1){
		if(rd == 0){
			z = 1;
		}else{
			z = 0;
		}
		n = get_bit(reg_new, 31);
		c = carryFrom(reg_new);
		v = overflowFrom(arm_read_register(p,rn), shift_value + get_C_flag(p), reg_new, ADD);
	}
	
	reg_new = arm_read_register(p,rn) + shift_value + get_C_flag(p);
    
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

void comp_process(arm_core p, uint8_t rn, uint32_t shift_value){
	
	/*
	alu_out = Rn - shifter_operand
	N Flag = alu_out[31]
	Z Flag = if alu_out == 0 then 1 else 0
	C Flag = NOT BorrowFrom(Rn - shifter_operand)
	V Flag = OverflowFrom(Rn - shifter_operand)
	*/

	uint32_t reg_new, op1;
	uint8_t z = 0, n = 0, c = 0, v = 0;
	
	op1 = arm_read_register(p, rn);
	reg_new = op1 - shift_value;

	if (get_bit(reg_new,31) == 1)
		n = 1;
	if (reg_new == 0)
		z = 1;
	if (get_bit(op1, 31) == get_bit(shift_value,31) && get_bit(op1,31) != get_bit(reg_new,31)))
		v = 1
	if (OverflowFrom(op1, reg_new, SUB))
		c = 1

	//on update les flags celon les erreurs
	update_flags(p, z, n, c, v);

	arm_write_register(p, reg_new);
	
	
	
}

void update_flags(arm_core p, uint8_t z, uint8_t n, uint8_t c, uint8_t v){

	//étape 1 : récup le registre cpsr
	uint32_t new_cpsr, Old_cpsr = arm_read_cpsr(p);

    //étape 2 : effacer les anciennes valeurs des flags dans le registre
    Old_cpsr = Old_cpsr & ~(0xf << 28);

	//étape 3 : modifier les valeurs du ou des bits n correspondant au flag
    if (z) 
		new_cpsr = set_bit(Old_cpsr, Z); 
    if (n) 
		new_cpsr = set_bit(Old_cpsr, N);
    if (c) 
		new_cpsr = set_bit(Old_cpsr, C);
    if (v) 
		new_cpsr = set_bit(Old_cpsr, V);
    
    //étape 4 : écrire dans le registre cpsr
    arm_write_cpsr(p, new_cpsr);

	
	/*
	uint32_t decalN = (1<<31);
	uint32_t decalZ = (1<<30);
	uint32_t decalC = (1<<29);
	uint32_t decalV = (1<<28);
	uint32_t OldCpsr = arm_read_cpsr(p);

	//flag Z
	if(reg == 0)
		arm_write_cpsr(p, (OldCpsr | decalZ));
	else
		arm_write_cpsr(p, (OldCpsr & (~decalZ)));
	

	//flag N
	if (get_bit(reg,31) == 1)
		arm_write_cpsr(p, (OldCpsr | decalN));
	else
		arm_write_cpsr(p, (OldCpsr & (~decalN)));


	//flag C
	arm_write_cpsr(p, (OldCpsr & (~decalC)));


	//flag V
	if(get_bit(operande_1,31) == get_bit(val_sh,31) && get_bit(operande_1,31) != get_bit(reg,31))
		arm_write_cpsr(p, (OldCpsr | decalV));
	else
		arm_write_cpsr(p, (OldCpsr & (~decalV)));
*/
}

void compN_process(arm_core p, uint8_t rn, uint32_t shift_value){
	
}

int cond_verifier(arm_core p, uint32_t cond) {

	switch (cond)
	{
		case 0:
			// EQ
			// Z set
			return get_Z_flag(p);
			break;

		case 1:
			// NE
			// Z clear
			return !get_Z_flag(p);
			break;

		case 2:
			// CS/HS
			// C set
			return get_C_flag(p);
			break;
			
		case 3:
			// CC/LO
			// C clear
			return !get_C_flag(p);
			break;

		case 4:
			// MI
			// N set
			return get_N_flag(p);
			break;

		case 5:
			// PL
			// N clear
			return !get_N_flag(p);
			break;

		case 6:
			// VS
			// V set
			return get_C_flag(p);
			break;

		case 7:
			// VC
			// V clear
			return !get_C_flag(p);
			break;

		case 8:
			// HI Unsigned higher 
			// C set and Z clear
			return get_C_flag(p) && get_Z_flag(p);
			break;
		
		case 9:
			// LS Unsigned lower or equal
			// C clear or Z set
			return !get_C_flag(p) && get_Z_flag(p);
			break;

		case 10:
			// GE signed greater than or equal 
			// (N set and V set) or (N clear and V clear)
			return (get_N_flag(p) && get_V_flag(p)) || (!get_N_flag(p) && !get_V_flag(p));
			break;

		case 11:
			// LT 
			// (N set and V clear) or (N clear and V set)
			return (get_N_flag(p) && !get_V_flag(p)) || (!get_N_flag(p) && get_V_flag(p));
			break;

		case 12:
			// GT signed greater than
			// Z clear and either N set and V set or (N clear and V clear)
			return !get_Z_flag(p) && ((get_N_flag(p) && get_V_flag(p)) || (!get_N_flag(p) && !get_Z_flag(p)));
			break;

		case 13:
			// LE signed less than or equal
			// (N set and V clear) or (N clear and V set)
			return get_Z_flag(p) ||  ((get_N_flag(p) && !get_V_flag(p)) || (!get_N_flag(p) && !get_V_flag(p)));
			break;

		case 14:
			// AL Always (unconditionnal)
			// true
			return 1;
			break;

		case 15:
			// -
			// UNPREDICTABLE
			return -1;
			break;

		default:
			break;
	}
	return 0;
}

int arm_data_processing_shift(arm_core p, uint32_t ins) {
	fprintf(stderr, "In arm_data_processing_shift\n");
	int cond, rn, rd, opcode, s;
	uint8_t shift_amnt, rm, rs, I, shift_op;
	uint32_t shift_value, shifter_carry_out;

	// parlez sur discord c'est plus simple jav
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
				// Yanis
				and_process(p, rn, rd, shift_value);
				break;

			case 1:
				// Logical exclusive OR
				// NiCONla
				eor_process(p, rn, rd, shift_value, s);
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
				//A faire	
				// Compare 
				comp_process(p, rn, shift_value);
				break;
				
			case 11:
				//A faire
				// Compare negated
				compN_process(p, rn, shift_value);
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
	}
    return UNDEFINED_INSTRUCTION;
}

int arm_data_processing_immediate_msr(arm_core p, uint32_t ins) {
    return UNDEFINED_INSTRUCTION;
}
