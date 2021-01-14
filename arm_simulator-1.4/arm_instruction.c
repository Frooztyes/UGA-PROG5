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
#include "arm_instruction.h"
#include "arm_exception.h"
#include "arm_data_processing.h"
#include "arm_load_store.h"
#include "arm_branch_other.h"
#include "arm_constants.h"
#include "util.h"
#include "cond_shift_calculator.h"

void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;
    
    for (i = size-1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            fprintf(stderr, "%u", byte);
        }
    }
    puts("");
}

static int arm_execute_instruction(arm_core p) {
    uint32_t ins;
    int error = arm_fetch(p, &ins);

    if(error != 0) return 1;
    if(!cond_verifier(p, get_bits(ins, 31, 28))) return 1;

    switch (get_bits(ins, 27, 25))
    {
        case 0b000:
            switch ((get_bits(ins, 24, 23) << 3) | (get_bit(ins, 20) << 2) | (get_bit(ins, 7) << 1) | get_bit(ins, 4))
            {
                case 0b10000: // misc instruction
                case 0b10010: // misc instruction
                case 0b10001: // misc instruction
                    fprintf(stderr, "misc instruction\n");
                    return arm_miscellaneous(p, ins);
                    break;

                case 0b00000: // data processing immediate shift
                case 0b00001: // data processing register shift
                    fprintf(stderr, "data processing register shift / data processing immediate shift\n");
                    return arm_data_processing_shift(p, ins);
                    break;
                    
                default:
                    if(get_bit(ins, 4) && get_bit(ins, 7)){
                        // multiply or extraload
                        printf("arm_load_store\n");
                        return arm_load_store(p, ins);
                    } else {
                        printf("arm_data_processing_shift\n");
                        return arm_data_processing_shift(p, ins);
                    }
                    break;
            }
            break;

        case 0b001:
            switch (get_bits(ins, 24, 20))
            {
                case 0b10000:
                case 0b10100:
                    fprintf(stderr, "undefined instruction\n");
                    // undefined instruction
                    return UNDEFINED_INSTRUCTION;
                    break;
                
                case 0b10010:
                case 0b10110:
                    fprintf(stderr, "move immediate to status register\n");
                    // move immediate to status register
                    return UNDEFINED_INSTRUCTION;
                    break;

                default:
                    // opcode + S ; data processing immediate
                    fprintf(stderr, "data processing immediate\n");
                    return arm_data_processing_immediate_msr(p,ins);
                    break;
            }
            break;

        case 0b010: // Load/store immediate offset 
            fprintf(stderr, "Load/store immediate offset\n");
            return arm_load_store(p, ins);
            break;

        case 0b011:
            if (!get_bit(ins, 4)){
                fprintf(stderr, "load/store register offset\n");
                // load/store register offset
                return arm_load_store(p, ins);
            } else{
                if(get_bits(ins, 24, 20) && get_bits(ins, 7, 5)){
                    fprintf(stderr, "media instructions / architecturally undefined\n");
                    // media instructions
                    // architecturally undefined
                    return UNDEFINED_INSTRUCTION;
                } else {
                    fprintf(stderr, "media instructions\n");
                    // media instructions     
                    return UNDEFINED_INSTRUCTION;                  
                }
            }
            break;

        case 0b100: // Load/store multiple
            fprintf(stderr, "Load/store multiple\n");
            return arm_load_store_multiple(p, ins);
            break;

        case 0b101: // Branch and branch with link
            fprintf(stderr, "Branch and branch with link\n");
            return arm_branch(p, ins);
            break;

        case 0b110: // Coprocessor load/store and double register transfers
            fprintf(stderr, "Coprocessor load/store\n");
            return arm_coprocessor_load_store(p, ins);
            break;

        case 0b111:
            fprintf(stderr, "coprocessor_others_swi\n");
            return arm_coprocessor_others_swi(p, ins);
            break;
        
        default:
            printf("Default base\n");
            return UNDEFINED_INSTRUCTION;
            break;
    }
}

int arm_step(arm_core p) {
    int result;

    result = arm_execute_instruction(p);
    if (result)
        arm_exception(p, result);
    return result;
}
