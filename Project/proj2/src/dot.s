.globl dot

.text
# =======================================================
# FUNCTION: Dot product of 2 int vectors
# Arguments:
#   a0 (int*) is the pointer to the start of v0
#   a1 (int*) is the pointer to the start of v1
#   a2 (int)  is the length of the vectors
#   a3 (int)  is the stride of v0
#   a4 (int)  is the stride of v1
# Returns:
#   a0 (int)  is the dot product of v0 and v1
# Exceptions:
# - If the length of the vector is less than 1,
#   this function terminates the program with error code 75.
# - If the stride of either vector is less than 1,
#   this function terminates the program with error code 76.
# =======================================================
dot:

    # Prologue
    addi t0,x0,1
    blt a2,t0,error1
    blt a3,t0,error2
    blt a4,t0,error2
    addi t0,x0,4
    mul a3,a3,t0
    mul a4,a4,t0
    
    mv t1,x0
    mv t0,x0
    
    


loop_start:
    lw t2,0(a0)
    lw t3,0(a1)
    mul t4,t2,t3
    add t0,t0,t4
    
    add a0,a0,a3
    add a1,a1,a4
    
    addi t1,t1,1
    
    blt t1,a2,loop_start

loop_end:
    # Epilogue
    mv a0,t0
    ret
    
error1:
    addi a1,x0,75
    j exit2
error2:
    addi a1,x0,76
    j exit2
