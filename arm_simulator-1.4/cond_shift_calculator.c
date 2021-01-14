#include "arm_data_processing.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_branch_other.h"
#include "util.h"
#include "debug.h"

int get_N_flag(arm_core p) {
	return get_bit(arm_read_cpsr(p), N);
}

int get_Z_flag(arm_core p) {
	return get_bit(arm_read_cpsr(p), Z);
}

int get_C_flag(arm_core p) {
	return get_bit(arm_read_cpsr(p), C);
}

int get_V_flag(arm_core p) {
	return get_bit(arm_read_cpsr(p), V);
}

int cond_verifier(arm_core p, uint8_t cond) {

	switch (cond)
	{
		case 0x0:
			// EQ
			// Z set
			return get_Z_flag(p);
			break;

		case 0x1:
			// NE
			// Z clear
			return !get_Z_flag(p);
			break;

		case 0x2:
			// CS/HS
			// C set
			return get_C_flag(p);
			break;
			
		case 0x3:
			// CC/LO
			// C clear
			return !get_C_flag(p);
			break;

		case 0x4:
			// MI
			// N set
			return get_N_flag(p);
			break;

		case 0x5:
			// PL
			// N clear
			return !get_N_flag(p);
			break;

		case 0x6:
			// VS
			// V set
			return get_V_flag(p);
			break;

		case 0x7:
			// VC
			// V clear
			return !get_V_flag(p);
			break;

		case 0x8:
			// HI Unsigned higher 
			// C set and Z clear
			return get_C_flag(p) && get_Z_flag(p);
			break;
		
		case 0x9:
			// LS Unsigned lower or equal
			// C clear or Z set
			return !get_C_flag(p) && get_Z_flag(p);
			break;

		case 0xA:
			// GE signed greater than or equal 
			// (N set and V set) or (N clear and V clear)
			return get_N_flag(p) == get_V_flag(p);
			break;

		case 0xB:
			// LT 
			// (N set and V clear) or (N clear and V set)
			return get_N_flag(p) != get_V_flag(p);
			break;

		case 0xC:
			// GT signed greater than
			// Z clear and either N set and V set or (N clear and V clear)
			return get_Z_flag(p) == 0 &&  get_N_flag(p) == get_V_flag(p);
			break;

		case 0xD:
			// LE signed less than or equal
			// (N set and V clear) or (N clear and V set)
			return get_Z_flag(p) == 1 || get_N_flag(p) != get_V_flag(p);
			break;

		case 0xE:
			// AL Always (unconditionnal)
			// true
			return 1;
			break;

		case 0xF:
			// -
			// UNPREDICTABLE
			return 0xF;
			break;

		default:
			return 1;
			break;
	}
	return 0;
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
