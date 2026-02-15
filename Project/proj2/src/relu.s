.globl relu

.text
# ==============================================================================
# FUNCTION: Performs an inplace element-wise ReLU on an array of ints
# Arguments:
# 	a0 (int*) is the pointer to the array
#	a1 (int)  is the # of elements in the array
# Returns:
#	None
# Exceptions:
# - If the length of the vector is less than 1,
#   this function terminates the program with error code 78.
# ==============================================================================
relu:
    # Prologue
    addi sp,sp,-8
    sw s1,4(sp)
    sw s0,0(sp)
    
    addi s1,x0,1
    blt a1,s1,error
    
    
    mv s1,x0

loop_start:
    
    lw s0,0(a0)
    bge s0,x0,loop_continue
    
    mv s0,x0
    

loop_continue:
    sw s0,0(a0)
    addi a0,a0,4
    addi s1,s1,1
    
    blt s1,a1,loop_start

loop_end:


    # Epilogue
    lw s0,0(sp)
    lw s1,4(sp)
    addi sp,sp,8

	ret
    
error:
    addi a1,x0,78
    call exit2
