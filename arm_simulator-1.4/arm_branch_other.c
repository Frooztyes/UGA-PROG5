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
#include "arm_branch_other.h"
#include "arm_constants.h"
#include "util.h"
#include <debug.h>
#include <stdlib.h>
#include "cond_shift_calculator.h"

int arm_branch(arm_core p, uint32_t ins) {
    printf("Instruction branch B/BL\n");
    if(cond_verifier(p, get_bits(ins, 31, 28))) {
        uint32_t pc = arm_read_register(p, 15);
        // sauvegarde de la valeur de PC dans le link register (BL)
        if(get_bit(ins, 24)) {
            arm_write_register(p, 14,  pc - 4);
        }
        // page 160 de la documentation
        // adresse de base en 23 bits
        uint32_t target_address = get_bits(ins, 23, 0);
        uint32_t extended_adr;

        /*
                                    11001000 <- 8-bit value of -56
                        11111111 11001000 <- extended to 16-bit value
        11111111 11111111 11111111 11001000 <- extended 32-bit value

                                        01001000 <- 8-bit value of 72
                        00000000 01001000 <- extended to 16-bit value
        00000000 00000000 00000000 01001000 <- extended 32-bit value
         */

        // je dois regarder le bit de poids fort pour savoir son signe

        if(get_bit(target_address, 23)) // bit de poids fort à 1, donc négatif
        {
            // 0011 1111 0000 0000 0000 0000 0000 0000
            //    3    F    0    0    0    0    0    0
            // mettre l'adresse sur 30 bits et conserver le signe ainsi que la valeur de l'adresse
            extended_adr =  0x3F000000 | target_address;
        }
        else // bit de poids fort à 0, donc positif
        { 
            // 0000 0000 0000 0000 0000 0000 0000 0000
            //    0    0    0    0    0    0    0    0
            // mettre l'addresse sur 30 bits et conserver le signe ainsi que la valeur de l'adresse
            extended_adr =  0x00000000 | target_address;
        }


        // décalage à gauche de 2 pour la multiplication par 4
        extended_adr = extended_adr << 2;

        // calcul du complément à 2 pour avoir un nombre signé
        extended_adr = (~extended_adr) + 1;
        
        // on ajoute la valeur de notre adresse à la valeur de pc
        if(get_bit(target_address, 23)) {
            arm_write_register(p, 15, pc - extended_adr);
        } else {
            arm_write_register(p, 15, pc + extended_adr);
        }

    }
    return 0;
}

int arm_coprocessor_others_swi(arm_core p, uint32_t ins) {
    if (get_bit(ins, 24)) {
        /* Here we implement the end of the simulation as swi 0x123456 */
        if ((ins & 0xFFFFFF) == 0x123456)
            exit(0);
        return SOFTWARE_INTERRUPT;
    } 
    return UNDEFINED_INSTRUCTION;
}

int arm_miscellaneous(arm_core p, uint32_t ins) {

    return UNDEFINED_INSTRUCTION;
}
