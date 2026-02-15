.globl argmax

.text
# =================================================================
# FUNCTION: Given a int vector, return the index of the largest
#	element. If there are multiple, return the one
#	with the smallest index.
# Arguments:
# 	a0 (int*) is the pointer to the start of the vector
#	a1 (int)  is the # of elements in the vector
# Returns:
#	a0 (int)  is the first index of the largest element
# Exceptions:
# - If the length of the vector is less than 1,
#   this function terminates the program with error code 77.
# =================================================================
argmax:

    # Prologue
    addi t0,x0,1
    blt a1,t0,error
    mv t0,x0
    lw t1,0(a0)
    mv t3,x0


loop_start:
    addi t0,t0,1
    addi a0,a0,4
    bge t0,a1,loop_end

loop_continue:
    lw t2,0(a0)
    bge t1,t2,loop_start
    mv t1,t2
    mv t3,t0
    j loop_start

loop_end:
    

    # Epilogue
    mv a0,t3
    ret
    
    
error:
    addi a1,x0,77
    j exit2
   
