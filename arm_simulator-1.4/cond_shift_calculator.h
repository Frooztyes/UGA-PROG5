#include <stdint.h>
#include "arm_core.h"

/* Vérifie les valeurs des différents flags Z, N, V et C
en fonction des bits cond (31 à 28) dans l'instruction.
*/
int cond_verifier(arm_core p, uint32_t cond);

/* Récupère la valeur du registre Rs au bit ind
*/
uint32_t get_rm_bit(arm_core p, uint32_t rm, uint8_t ind);

/* Récupère le flag N dans le registre CPSR au bit 31
*/
int get_N_flag(arm_core p);

/* Récupère le flag Z dans le registre CPSR au bit 30
*/
int get_Z_flag(arm_core p);

/* Récupère le flag C dans le registre CPSR au bit 29
*/
int get_C_flag(arm_core p);

/* Récupère le flag V dans le registre CPSR au bit 28
*/
int get_V_flag(arm_core p);

/* Modifie la valeur de shift_value et de shifter_carry_out grâce aux différentes valeurs de l'instruction
*/
void shifter_value_calculator(uint32_t *shift_value, uint32_t *shifter_carry_out, arm_core p, uint8_t I, uint8_t shift_op, uint8_t Rs, uint8_t Rm, uint32_t shift_imm);
