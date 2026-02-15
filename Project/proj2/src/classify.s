.globl classify

.text
# ==============================================================================
# FUNCTION: Performs inference using two linear layers with ReLU activation
# Arguments:
#   a0 (int)    argc
#   a1 (char**) argv
#   a2 (int)    print_classification (0 = print, else = don't print)
# Returns:
#   a0 (int)    Classification (argmax result)
# Exceptions:
#   - Incorrect number of args -> exit 89
#   - malloc fails -> exit 88
# ==============================================================================
classify:
    # =====================================
    # PROLOGUE & SAVE ARGUMENTS
    # =====================================
    # Allocate stack: 32(regs) + 24(dims) = 56 bytes
    addi sp, sp, -56
    
    # Save callee-saved registers
    sw ra, 0(sp)
    sw s0, 4(sp)
    sw s1, 8(sp)
    sw s2, 12(sp)
    sw s3, 16(sp)
    sw s4, 20(sp)
    sw s5, 24(sp)
    sw s6, 28(sp)
    
    # Save arguments to s registers
    mv s0, a0           # argc
    mv s1, a1           # argv
    mv s2, a2           # print_classification
    
    # =====================================
    # CHECK COMMAND LINE ARGUMENTS
    # =====================================
    li t0, 5
    bne s0, t0, error_89
    
    # =====================================
    # STORAGE FOR MATRIX DIMENSIONS
    # =====================================
    # sp+32: m0 rows
    # sp+36: m0 cols
    # sp+40: m1 rows
    # sp+44: m1 cols
    # sp+48: input rows
    # sp+52: input cols
    
    # =====================================
    # LOAD MATRIX m0
    # =====================================
    lw a0, 4(s1)                # argv[1] = M0_PATH
    addi a1, sp, 32             # store m0 rows at sp+32
    addi a2, sp, 36             # store m0 cols at sp+36
    call read_matrix
    mv s3, a0                   # s3 = m0 matrix pointer
    
    # =====================================
    # LOAD MATRIX m1
    # =====================================
    lw a0, 8(s1)                # argv[2] = M1_PATH
    addi a1, sp, 40             # store m1 rows at sp+40
    addi a2, sp, 44             # store m1 cols at sp+44
    call read_matrix
    mv s4, a0                   # s4 = m1 matrix pointer
    
    # =====================================
    # LOAD INPUT MATRIX
    # =====================================
    lw a0, 12(s1)               # argv[3] = INPUT_PATH
    addi a1, sp, 48             # store input rows at sp+48
    addi a2, sp, 52             # store input cols at sp+52
    call read_matrix
    mv s5, a0                   # s5 = input matrix pointer
    
    # =====================================
    # LOAD DIMENSIONS FROM STACK
    # =====================================
    lw t0, 32(sp)               # m0 rows
    lw t1, 36(sp)               # m0 cols
    lw t2, 40(sp)               # m1 rows
    lw t3, 44(sp)               # m1 cols
    lw t4, 48(sp)               # input rows
    lw t5, 52(sp)               # input cols
    
    # =====================================
    # FIRST LINEAR LAYER: output1 = m0 * input
    # Dimensions: (m0_rows × input_cols)
    # =====================================


    
    # malloc for output1: size = m0_rows * input_cols * 4
    mul t6, t0, t5              # m0_rows * input_cols
    slli a0, t6, 2              # *4 = bytes
    call malloc
    beq a0, x0, error_88
    mv a6, a0                   # a6 = output1 pointer

    lw t0, 32(sp)               # m0 rows
    lw t1, 36(sp)               # m0 cols
    lw t2, 40(sp)               # m1 rows
    lw t3, 44(sp)               # m1 cols
    lw t4, 48(sp)               # input rows
    lw t5, 52(sp)               # input cols
    
    # Call matmul: m0 * input
    mv a0, s3                   # m0 pointer
    mv a1, t0                   # m0 rows
    mv a2, t1                   # m0 cols
    mv a3, s5                   # input pointer
    mv a4, t4                   # input rows
    mv a5, t5                   # input cols
    # a6 already set to output1 pointer
    call matmul

    lw t0, 32(sp)               # m0 rows
    lw t1, 36(sp)               # m0 cols
    lw t2, 40(sp)               # m1 rows
    lw t3, 44(sp)               # m1 cols
    lw t4, 48(sp)               # input rows
    lw t5, 52(sp)               # input cols
    
    mv s6, a6                   # s6 = output1 pointer
    
    # =====================================
    # RELU ACTIVATION: output1 = ReLU(output1)
    # =====================================
    mv a0, s6                   # output1 pointer
    mul a1, t0, t5              # length = m0_rows * input_cols
    call relu

    lw t0, 32(sp)               # m0 rows
    lw t1, 36(sp)               # m0 cols
    lw t2, 40(sp)               # m1 rows
    lw t3, 44(sp)               # m1 cols
    lw t4, 48(sp)               # input rows
    lw t5, 52(sp)               # input cols
    
    # =====================================
    # SECOND LINEAR LAYER: output2 = m1 * output1
    # Dimensions: (m1_rows × input_cols)
    # =====================================
    
    # malloc for output2: size = m1_rows * input_cols * 4
    mul t6, t2, t5              # m1_rows * input_cols
    slli a0, t6, 2              # *4 = bytes
    call malloc
    beq a0, x0, error_88
    mv a6, a0                   # a6 = output2 pointer
    
    # Call matmul: m1 * output1
    mv a0, s4                   # m1 pointer
    mv a1, t2                   # m1 rows
    mv a2, t3                   # m1 cols
    mv a3, s6                   # output1 pointer
    mv a4, t0                   # output1 rows = m0_rows
    mv a5, t5                   # output1 cols = input_cols
    # a6 already set to output2 pointer
    call matmul

    lw t0, 32(sp)               # m0 rows
    lw t1, 36(sp)               # m0 cols
    lw t2, 40(sp)               # m1 rows
    lw t3, 44(sp)               # m1 cols
    lw t4, 48(sp)               # input rows
    lw t5, 52(sp)               # input cols
    
    # Free output1
    # notice that here we call the free function 
    # but free function will change the value of a6 
    # so we temporarily store the a6 to t6 
    # after free , we restore the a6 from t6 
    mv t6, a6
    mv a0, s6
    call free

    lw t0, 32(sp)               # m0 rows
    lw t1, 36(sp)               # m0 cols
    lw t2, 40(sp)               # m1 rows
    lw t3, 44(sp)               # m1 cols
    lw t4, 48(sp)               # input rows
    lw t5, 52(sp)               # input cols
    
    mv s6, t6                   # s6 = output2 pointer
    
    # =====================================
    # WRITE OUTPUT MATRIX TO FILE
    # =====================================
    lw a0, 16(s1)               # argv[4] = OUTPUT_PATH
    mv a1, s6                   # output2 pointer
    mv a2, t2                   # m1 rows (output rows)
    mv a3, t5                   # input cols (output cols)
    call write_matrix

    lw t0, 32(sp)               # m0 rows
    lw t1, 36(sp)               # m0 cols
    lw t2, 40(sp)               # m1 rows
    lw t3, 44(sp)               # m1 cols
    lw t4, 48(sp)               # input rows
    lw t5, 52(sp)               # input cols
    
    # =====================================
    # ARGMAX: Find classification
    # =====================================
    mv a0, s6                   # output2 pointer
    mul a1, t2, t5              # length = m1_rows * input_cols
    call argmax

    lw t0, 32(sp)               # m0 rows
    lw t1, 36(sp)               # m0 cols
    lw t2, 40(sp)               # m1 rows
    lw t3, 44(sp)               # m1 cols
    lw t4, 48(sp)               # input rows
    lw t5, 52(sp)               # input cols
    
    # Save classification result
    mv s0, a0                   # s0 = classification
    
    # =====================================
    # PRINT CLASSIFICATION (if requested)
    # =====================================
    bnez s2, skip_print         # if print_classification != 0, skip
    
    mv a1, s0
    call print_int
    li a1, 10                   # newline
    call print_char
    
skip_print:
    # =====================================
    # FREE OUTPUT MATRIX
    # =====================================
    mv a0, s6
    call free
    
    # =====================================
    # EPILOGUE & RETURN
    # =====================================
    mv a0, s0                   # return classification
    
    # Restore registers
    lw ra, 0(sp)
    lw s0, 4(sp)
    lw s1, 8(sp)
    lw s2, 12(sp)
    lw s3, 16(sp)
    lw s4, 20(sp)
    lw s5, 24(sp)
    lw s6, 28(sp)
    addi sp, sp, 56
    
    ret

# =====================================
# ERROR HANDLERS
# =====================================
error_88:
    li a1, 88
    call exit2

error_89:
    li a1, 89
    call exit2