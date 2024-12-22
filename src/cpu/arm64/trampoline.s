

    .global trampoline 

trampoline:
    ; X0-X7 are parameters
    ; X0 pc base
    ; X1 bc_entry array base
    ; X2 var_local_table_arr base
    ; using X3 as cvm stack pointer
    stp x29, x30, [sp, #-16]!
    ; ldrh w0, [x0]
    ldrsh w5, [x0]
    mov x4, #8
    mul w4, w5, w4
    add x4, x1, x4
    ldr x4, [x4]
    br x4
    ; ldp x29, x30, [sp], #16
    ; ret
