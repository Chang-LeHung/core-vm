


    .global loadi 
    .type loadi, %function

loadi:
    // x0, x1, x2, x3, x7 are active
    // X0-X7 are parameters
    // X0 pc base
    // X1 bc_entry array base
    // X2 var_local_table_arr base
    // using X3 as cvm stack pointer
    // X7 as stack top val
    str w7, [x3], #8
    mov x9, #16
    add x0, x0, x9
    ldrsh w5, [x0]
    mov x4, #8
    mul w4, w5, w4
    add x4, x2, x4
    ldr w7, [x4]
    // jump to next instruction
    mov x9, #16
    add x0, x0, x9
    ldrsh w5, [x0]
    mov x4, #8
    mul w4, w5, w4
    add x4, x1, x4
    ldr x4, [x4]
    br x4
