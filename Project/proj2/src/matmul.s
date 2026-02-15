.globl matmul

.text
# =======================================================
# FUNCTION: Matrix Multiplication of 2 integer matrices
#   d = matmul(m0, m1)
# Arguments:
#   a0 (int*) is pointer to start of m0
#   a1 (int) is number of rows of m0
#   a2 (int) is number of columns of m0
#   a3 (int*) is pointer to start of m1
#   a4 (int) is number of rows of m1
#   a5 (int) is number of columns of m1
#   a6 (int*) is pointer to start of d
# Returns:
#   None (void), sets d = matmul(m0, m1)
# Exceptions:
#   - If dimensions of m0 are invalid -> exit code 72
#   - If dimensions of m1 are invalid -> exit code 73
#   - If dimensions don't match -> exit code 74
# =======================================================
matmul:
    # Error checks
    addi t0, x0, 1
    blt a1, t0, error1
    blt a2, t0, error1
    blt a4, t0, error2
    blt a5, t0, error2
    bne a2, a4, error3

    # Prologue

    addi sp, sp, -32
    sw ra, 28(sp)
    sw s0, 24(sp)    # m0 base pointer (never modify)
    sw s1, 20(sp)    # m1 base pointer (never modify)
    sw s2, 16(sp)    # m0 columns
    sw s3, 12(sp)    # m0 rows
    sw s4, 8(sp)     # m1 columns
    sw s5, 4(sp)     # output base pointer
    sw s6, 0(sp)     # m0 row stride (bytes)
    
    mv s0, a0
    mv s1, a3
    mv s2, a2
    mv s3, a1
    mv s4, a5
    mv s5, a6
    
    # Precompute m0 row stride (bytes)
    slli s6, s2, 2
    
    # Outer loop: i = 0 to m0_rows-1
    li t0, 0

outer_loop:

    bge t0, s3, outer_loop_end
    
    # Inner loop: j = 0 to m1_cols-1
    li t1, 0

inner_loop:
    bge t1, s4, inner_loop_end
    
    # a0 = &m0[i][0] = s0 + (i * s2) * 4
    mul t2, t0, s6
    add a0, s0, t2

    
    # a1 = &m1[0][j] = s1 + j * 4
    slli t3, t1, 2
    add a1, s1, t3
    
    # Set dot parameters
    mv a2, s2        # vector length = m0 columns
    li a3, 1         # m0 stride = 1 element
    mv a4, s4        # m1 stride = m1 columns elements
    
    # Save temporary registers
    addi sp, sp, -8
    sw t0, 4(sp)
    sw t1, 0(sp)
    
    call dot
    
    # Restore temporary registers
    # this time we restore the base pointer into s0,s1
    # and we use t0,t1 to store position of output element 
    # remenber to store and restore the t0,t1


    lw t1, 0(sp)
    lw t0, 4(sp)
    addi sp, sp, 8
    
    # Output address = s5 + (i * s4 + j) * 4
    # we should notice that the column of C is not same as the A matrix 
    # so we can't use the s6 to calculate 

    mul t4, t0, s4
    add t4,t4,t1
    slli t4,t4,2
    add t4, s5,t4
    sw a0, 0(t4)
    
    addi t1, t1, 1
    j inner_loop
    
inner_loop_end:
    addi t0, t0, 1
    # Do NOT modify s0! Next row address is s0 + (i * s2) * 4
    j outer_loop
    
outer_loop_end:
    # Epilogue
    lw s6, 0(sp)
    lw s5, 4(sp)
    lw s4, 8(sp)
    lw s3, 12(sp)
    lw s2, 16(sp)
    lw s1, 20(sp)
    lw s0, 24(sp)
    lw ra, 28(sp)
    addi sp, sp, 32
    ret

error1:
    addi a1, x0, 72
    j exit2

error2:
    addi a1, x0, 73
    j exit2

error3:
    addi a1, x0, 74
    j exit2