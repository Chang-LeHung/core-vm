

.global printi
.extern printf
.type printi, %function

.data
sym: .asciz "%d"

.text
printi:
    // x0, x1, x2, x3, x7 are active
    // X0-X7 are parameters
    // X0 pc base
    // X1 bc_entry array base
    // X2 var_local_table_arr base
    // using X3 as cvm stack pointer
    // X7 as stack top val
    // save registers
    stp x0, x1, [sp, #-16]!
    stp x2, x3, [sp, #-16]!
    stp x3, x7, [sp, #-16]!

    ldr x0, =sym
    mov w1, w7
    bl printf

    // restore registers
    ldp x3, x7, [sp], #16
    ldp x2, x3, [sp], #16
    ldp x0, x1, [sp], #16

    str w7, [x3, #-8]!
    // jump to next instruction
    mov x9, #16
    add x0, x0, x9
    ldrsh w5, [x0]
    mov x4, #8
    mul w4, w5, w4
    add x4, x1, x4
    ldr x4, [x4]
    br x4
