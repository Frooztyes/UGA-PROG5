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



int arm_load_store(arm_core p, uint32_t ins) {

    uint8_t Rn, Rd, shift_amount, shift_codeOp, Rm, P, U, I, B, W, L, cond, is_half;
    uint16_t immediate;


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

    is_half = !get_bit(ins, 26);
    
    // si bit 26 == 1 alors word / unsigned byte sinon half or doubleword
    
    // if P=0 : post-indexed addressing
    //  -W = 0 : LDR , LDRB , STR or STRB 
    //  -W = 1 : LDRBT , LDRT , STRBT or STRT 
    // if P=1 : offset / pre-index addressing
    //  -W = 0 : the base register is not updated 
    //  -W = 1 : the base register is updated 
    immediate = get_bits(ins,11,0); //immediate offset
    shift_amount = get_bits(ins,11,7); // shift amount for register offset
	shift_codeOp = get_bits(ins,6,5); // shift op code for register offset
	Rm = get_bits(ins,3,0); // offset register

   
    uint32_t value;
    int result;

    //a modif
    // if P=0 : post-indexed addressing
    // if P=1 : offset / pre-index addressing
    uint32_t address = arm_read_register(p, Rn);
	if (P) address+= addressing_mode(p, ins);

    
    if(cond_verifier(p,cond)) {
        //Load operation
        if(L){
                
            if (is_half) arm_read_half(p, address, (uint16_t *) &value); //(half)
            else if (B) arm_read_byte(p, address, (uint8_t *) &value); //(byte)
            else  arm_read_word(p, address, &value); //(word)
                
            if (Rd==PC) //PC = 15
                value&=0xFFFFFFFE;

            // if P=0 : post-indexed addressing
            //  -W = 0 : LDR , LDRB , STR or STRB 
            //  -W = 1 : LDRBT , LDRT , STRBT or STRT
            if (!P && W ) 
                result = arm_write_usr_register(p,Rd,value);	
            else 
                result = arm_write_register(p,Rd,value);
            
        //Store operation
        }else { 

            // if P=0 : post-indexed addressing
            //  -W = 0 : LDR , LDRB , STR or STRB 
            //  -W = 1 : LDRBT , LDRT , STRBT or STRT
            if (!P && W ) 
                value = arm_read_usr_register(p, Rd);	
            else 
                value = arm_read_register(p, Rd);

            
            if (is_half)  result = arm_write_half(p, address, (uint16_t) value); //(half)
            else if (B) result = arm_write_byte(p, address, (uint8_t) value);  //(byte)
            else result = arm_write_word(p, address, value); //(word)

        }   





        return result;
    }
    
    return UNDEFINED_INSTRUCTION;
}

int arm_load_store_multiple(arm_core p, uint32_t ins) {
    return UNDEFINED_INSTRUCTION;
}

int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
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


    

    

    

    

    




    