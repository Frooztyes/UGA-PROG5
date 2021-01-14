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

#include "arm_load_store.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "util.h"
#include "debug.h"
#include <assert.h>
#include "cond_shift_calculator.h"

int number_of_set_bits_in(uint32_t bitfield)
{
    int nb = 0;
    for(int i = 0; i< 31; i++)
        get_bit(bitfield, i) ? nb++ : 0;
    return nb;
}

uint32_t address_calculator(arm_core p, uint32_t offset_12, uint32_t Rn, uint8_t U, uint8_t P, uint8_t W, uint8_t bit_25, uint32_t cond) {
    if(P == 0) {
        if(W == 0) {
            if(bit_25 == 0) {
                // immediate post-indexed
                uint32_t address = arm_read_register(p, Rn);
                if(cond_verifier(p, cond)) {
                    (U == 1) ? 
                        arm_write_register(p, Rn, arm_read_register(p, Rn) + offset_12) : 
                        arm_write_register(p, Rn, arm_read_register(p, Rn) - offset_12);
                }
                return address;
            } else {
                if(get_bits(offset_12, 11, 4) == 0) {
                    // register post-indexed
                    uint32_t address = arm_read_register(p, Rn);
                    if(cond_verifier(p, cond)) {
                        uint32_t Rm = get_bits(offset_12, 3, 0);
                        (U == 1) ? 
                            arm_write_register(p, Rn, arm_read_register(p, Rn) + arm_read_register(p, Rm)) : 
                            arm_write_register(p, Rn, arm_read_register(p, Rn) - arm_read_register(p, Rm));
                    }
                    return address;
                } else {
                    // scaled register post-indexed
                    uint8_t shift_op = get_bits(offset_12, 6, 5);
                    uint8_t shift_imm = get_bits(offset_12, 11, 7);
                    uint8_t I = get_bit(offset_12, 4);
                    uint8_t Rm = get_bits(offset_12, 3, 0);
                    uint8_t Rs = get_bits(offset_12, 11, 6);
                    uint32_t shift_value, shift_carry_out;
                    shifter_value_calculator(&shift_value, &shift_carry_out, p, I, shift_op, Rs, Rm, shift_imm);
                    return U == 1 ? arm_read_register(p, Rn) + shift_value: arm_read_register(p, Rn) - shift_value;                
                }
                
            }
        } else {
            // not handle
            return 0;
        }
    } else /* P == 1 */ {
        if(W == 0) {
            if(bit_25 == 0) {
                // imediate offset 
                return U == 1 ? arm_read_register(p, Rn) + offset_12 : arm_read_register(p, Rn) - offset_12;
            } else {
                if(get_bits(offset_12, 11, 4) == 0) {
                    // register offset
                    uint32_t Rm = get_bits(offset_12, 3, 0);
                    return U == 1 ? 
                        arm_read_register(p, Rn) + arm_read_register(p, Rm) : 
                        arm_read_register(p, Rn) - arm_read_register(p, Rm);
                } else {
                    // scaled register offset
                    uint8_t shift_op = get_bits(offset_12, 6, 5);
                    uint8_t shift_imm = get_bits(offset_12, 11, 7);
                    uint8_t I = get_bit(offset_12, 4);
                    uint8_t Rm = get_bits(offset_12, 3, 0);
                    uint8_t Rs = get_bits(offset_12, 11, 6);
                    uint32_t shift_value, shift_carry_out;
                    shifter_value_calculator(&shift_value, &shift_carry_out, p, I, shift_op, Rs, Rm, shift_imm);
                    return (U == 1) ? arm_read_register(p, Rn) + shift_value: arm_read_register(p, Rn) - shift_value;   
                }
            }
        } else {
            if(bit_25 == 0) {
                // immediate pre-indexed
                uint32_t address = (U == 1) ? arm_read_register(p, Rn) + offset_12 : arm_read_register(p, Rn) - offset_12;
                cond_verifier(p, cond) ? arm_write_register(p, Rn, address) : 0;
                return address;
            } else {
                if(get_bits(offset_12, 11, 4) == 0) {
                    // register pre-indexed
                    uint32_t Rm = get_bits(offset_12, 3, 0);
                    uint32_t address = (U == 1) ? arm_read_register(p, Rn) + arm_read_register(p, Rm) : arm_read_register(p, Rn) - arm_read_register(p, Rm);
                    (cond_verifier(p, cond) == 1) ? arm_write_register(p, Rn, address) : 0;
                    return address;
                } else {
                    // scaled register pre-indexed
                    uint8_t shift_op = get_bits(offset_12, 6, 5);
                    uint8_t shift_imm = get_bits(offset_12, 11, 7);
                    uint8_t I = get_bit(offset_12, 4);
                    uint8_t Rm = get_bits(offset_12, 3, 0);
                    uint8_t Rs = get_bits(offset_12, 11, 6);
                    uint32_t shift_value, shift_carry_out;
                    shifter_value_calculator(&shift_value, &shift_carry_out, p, I, shift_op, Rs, Rm, shift_imm);
                    uint32_t address = (U == 1) ? arm_read_register(p, Rn) + shift_value: arm_read_register(p, Rn) - shift_value;   
                    (cond_verifier(p, cond) == 1) ? arm_write_register(p, Rn, address) : 0;
                    return address;
                }
                
            }
        }
    }
    return 0;
}

uint32_t misc_address_calculator(arm_core p, uint32_t offset_12, uint32_t Rn, uint8_t U, uint8_t B, uint8_t W, uint8_t P, uint32_t cond) {
    uint32_t immedH, immedL;
    immedH = get_bits(offset_12, 11, 8);
    immedL = get_bits(offset_12, 3, 0);
    if(B == 1) {
        if(W == 0) {
            if(P == 0) {
                // immediate post-index
                uint32_t address = arm_read_register(p, Rn);
                uint32_t offset_8 = (immedH << 4) | immedL;
                if(cond_verifier(p, cond)) {
                    (U == 1) ? 
                        arm_write_register(p, Rn, arm_read_register(p, Rn) + offset_8) : 
                        arm_write_register(p, Rn, arm_read_register(p, Rn) - offset_8);
                }
                return address;
            } else {
                // immediate offset
                uint32_t offset_8 = (immedH << 4) | immedL;
                return (U == 1) ? arm_read_register(p, Rn) + offset_8 : arm_read_register(p, Rn);
            }
        } else /* W == 1 */{
            // immediate pre-index
            uint32_t offset_8 = (immedH << 4) | immedL;
            uint32_t address = (U == 1) ? arm_read_register(p, Rn) + offset_8 : arm_read_register(p, Rn);
            (cond_verifier(p, cond) == 1) ? arm_write_register(p, Rn, address) : 0;
            return address;
        }
    } else /* B == 0 */ {
        if(W == 1) {
            // register pre-index
            // here immedL has the same value as Rm
            uint32_t address = (U == 1) ? 
                arm_write_register(p, Rn, arm_read_register(p, Rn) + arm_read_register(p, immedL)) : 
                arm_write_register(p, Rn, arm_read_register(p, Rn) - arm_read_register(p, immedL));
            (cond_verifier(p, cond) == 1) ? arm_write_register(p, Rn, address) : 0;
            return address;
        } else /* W == 0*/{
            if(P == 0) {
                // register post-indexed
                uint32_t address = arm_read_register(p, Rn);
                if(cond_verifier(p, cond)) {
                    (U == 1) ? 
                        arm_write_register(p, Rn, arm_read_register(p, Rn) + arm_read_register(p, immedL)): 
                        arm_write_register(p, Rn, arm_read_register(p, Rn) - arm_read_register(p, immedL));
                }
                return address;
            } else {
                // register offset
                // here immedL has the same value as Rm
                return (U == 1) ? 
                    arm_write_register(p, Rn, arm_read_register(p, Rn) + arm_read_register(p, immedL)) : 
                    arm_write_register(p, Rn, arm_read_register(p, Rn) - arm_read_register(p, immedL));
            }
        }
    }
}

void mult_address_calculator(arm_core p, uint32_t *start_address, uint32_t *end_address, uint32_t type_modes, uint32_t Rn, uint32_t cond, uint32_t register_list, uint8_t W) {
    switch (type_modes)
    {
        case 0b01:
            // increment after
            *start_address = arm_read_register(p, Rn);
            *end_address = arm_read_register(p, Rn) + (number_of_set_bits_in(register_list) * 4) - 4;
            if(cond_verifier(p, cond) && W == 1) {
                arm_write_register(p, Rn, arm_read_register(p, Rn) + (number_of_set_bits_in(register_list) * 4));
            }
            break;

        case 0b11:
            // increment before
            *start_address = arm_read_register(p, Rn) + 4;
            *end_address = arm_read_register(p, Rn) + (number_of_set_bits_in(register_list) * 4);
            if(cond_verifier(p, cond) && W == 1) {
                arm_write_register(p, Rn, arm_read_register(p, Rn) + (number_of_set_bits_in(register_list) * 4));
            }
            break;

        case 0b00:
            // decrement after
            *start_address = arm_read_register(p, Rn) - (number_of_set_bits_in(register_list) * 4) + 4;
            *end_address = arm_read_register(p, Rn);
            if(cond_verifier(p, cond) && W == 1) {
                arm_write_register(p, Rn, arm_read_register(p, Rn) - (number_of_set_bits_in(register_list) * 4));
            }
            break;

        case 0b10:
            // decrement before
            *start_address = arm_read_register(p, Rn) - (number_of_set_bits_in(register_list) * 4);
            *end_address = arm_read_register(p, Rn) - 4;
            if(cond_verifier(p, cond) && W == 1) {
                arm_write_register(p, Rn, arm_read_register(p, Rn) - (number_of_set_bits_in(register_list) * 4));
            }
            break;
        
        default:
            *start_address = 0;
            *end_address = 0;
            break;
    }
}

int arm_load_store(arm_core p, uint32_t ins) {
	fprintf(stderr, "In arm_load_store\n");

    uint8_t P, U, I, B, W, L, word_or_byte, offset_12;
    uint32_t Rn, Rd, cond;

    //recuperating every useful bits 
    cond = get_bits(ins,31,28);

    I = get_bit(ins,25); //immediate (0) or register (1) offset
    P = get_bit(ins,24); // post-indexed addressing(0) or pre-indexed addressing(1)
    U = get_bit(ins,23); // +offset (1) or -offset(0)
    W = get_bit(ins,21); //

    // type de données
    L = get_bit(ins,20); // Load (1) or Store (0)
    B = get_bit(ins,22); // byte(1) or word(0)
    
    Rn = get_bits(ins,19,16); // Address, base register
    Rd = get_bits(ins,15,12); // register whose content are to be loaded or stored

    word_or_byte = get_bits(ins, 27, 26); // See if word/byte or half/double

    offset_12 = get_bits(ins, 11, 0);
    
    int result = 0;

    uint8_t byte;
    uint16_t half;
    uint32_t word, address;
    if(word_or_byte == 0b01) { 
        // unsigned byte and word instructions (get_bits(ins, 27, 26))
        address = address_calculator(p, offset_12, Rn, U, P, W, I, cond);
        if(!B) { 
            // Word instruction
            if(L) { 
                // Load register LDR
                result = arm_read_word(p, address, &word); //(word)
                result = (!P && W) ? arm_write_usr_register(p, Rd, (uint32_t) word) : arm_write_register(p, Rd, (uint32_t) word);

            } else {
                // bit 20 à 0
                // STR Instruction
                word = (!P && W) ? arm_read_usr_register(p, Rd) : arm_read_register(p, Rd);
                result = arm_write_word(p, address, (uint32_t) word);

            }
        } else { 
            // unsigned byte Instruction
            if(L) {
                //LDRB Instruction
                result = arm_read_byte(p, address, &byte); //(byte)
                result = (!P && W) ? arm_write_usr_register(p, Rd, (uint8_t) byte) : arm_write_register(p, Rd, (uint8_t) byte);
    
            } else { 
                // STRB Instruction
                byte = (!P && W) ? arm_read_usr_register(p, Rd) : arm_read_register(p, Rd);
                result = arm_write_byte(p, address, (uint8_t) byte);  //(byte)
            }
        }
    } else if(get_bits(ins, 27, 25) == 0b000) {
        // signed byte, halfword and doubleword
        address = misc_address_calculator(p, offset_12, Rn, U, B, W, P, cond);
        if(L) {
            //LDRH Instruction
            result = arm_read_half(p, address, &half); //(half)
            result = (!P && W) ? arm_write_usr_register(p, Rd, (uint16_t) half) : arm_write_register(p, Rd, (uint16_t) half);

        } else {
            //STRH Instruction
            half = (!P && W) ? arm_read_usr_register(p, Rd) : arm_read_register(p, Rd);
            result = arm_write_half(p, address, (uint16_t) half);

        }
    }
    return result;
}

int arm_load_store_multiple(arm_core p, uint32_t ins) {
	fprintf(stderr, "In arm_load_store_multiple\n");
    uint8_t W, L;
    uint32_t Rn, cond, type_modes, register_list;

    cond = get_bits(ins,31,28);

    W = get_bit(ins, 21);
    L = get_bit(ins, 20);
    
    Rn = get_bits(ins, 19, 16);

    
    type_modes = get_bits(ins, 23, 22);
    register_list = get_bits(ins, 15, 0);
    
    cond = get_bits(ins, 31, 28);
    if(cond_verifier(p, cond)){
        uint32_t start_address;
        uint32_t end_address;
        mult_address_calculator(p, &start_address, &end_address, type_modes, Rn, cond, register_list, W);
        uint32_t address = start_address, value;

        if(L) {
            // LDM
            for (int i = 0; i < 15; i++)
            {
                if(get_bit(register_list, i)){
                    arm_read_word(p, address, &value);
                    if(get_bit(ins, 22) && get_bit(ins, 20) && !get_bit(ins,15)) {
                        // LDM 2 for user mode
                        arm_write_usr_register(p, i, value);
                    } else {
                        arm_write_register(p, i, value);
                    }
                    address += 4;
                }
            }
            if(!get_bit(ins, 22) && get_bit(ins, 20)) {
                // LDM 1 suite
                if(get_bit(register_list, 15)) {
                    value = arm_read_word(p, address, &value);
                    arm_write_register(p, 15, value & 0xFFFFFFFE);
                }
            } else if(get_bit(ins, 22) && get_bit(ins, 20) && get_bit(ins, 15)) {
                // LDM 2 suite
                if(arm_current_mode_has_spsr(p)) {
                    arm_write_cpsr(p, arm_read_spsr(p));
                } else {
                    // UNPREDICTABLE
                }
                value = arm_read_word(p, address, &value);
                arm_write_register(p, 15, value);
            }

            address += 4;
            assert(end_address == address - 4);
        } else {
            // STM
            for (int i = 0; i < 15; i++)
            {
                if(get_bit(register_list, i)){
                    if(get_bit(ins, 22)) {
                        // STM (2)
                        arm_write_word(p, address, arm_read_usr_register(p, i));
                    } else {
                        // STM (1)
                        arm_write_word(p, address, arm_read_register(p, i));
                    }
                    address += 4;
                }
            }
            assert(end_address == address - 4);
        }
    }
    return 0;
}

int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
	fprintf(stderr, "In arm_coprocessor_load_store\n");
    /* Not implemented */
    return UNDEFINED_INSTRUCTION;
}




// Load / store halfword / doubleword  | load signed byte instructions
// LDR, LDRB, LDRBT, LDRD, LDREX, LDRH, LDRSB, LDRSH, LDRT
// STR, STRB, STRBT, STRD, STREX, STRH, STRT

// Load / store multiple instructions :
// LDM, LDM (user register), LDM (restore CPSR), STM, STM (User register)

// Mode d'addressage 2 : Load / Store pour Word | Unsigned Byte    
// Tout dispo pour LDR, LDRB, STR, STRB  || Que les 3 derniers pour LDRBT, LDRT, STRBT
//[<Rn>, #+/-<offset_12>]