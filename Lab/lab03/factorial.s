.globl factorial

.data
n: .word 8

.text
main:
    la t0, n
    lw a0, 0(t0)
    jal ra, factorial

    addi a1, a0, 0
    addi a0, x0, 1
    ecall # Print Result

    addi a1, x0, '\n'
    addi a0, x0, 11
    ecall # Print newline

    addi a0, x0, 10
    ecall # Exit

factorial:
    # YOUR CODE HERE
    addi sp , sp ,-4
    sw ra,0(sp)
    addi t0,x0,1
    beq a0,t0,base
    
    addi sp,sp ,-4
    sw a0,0(sp)
    addi a0,a0,-1
    jal ra , factorial
    lw t0,0(sp)
    addi sp,sp,4
    mul a0,a0,t0

exit:
    lw ra,0(sp)
    addi sp,sp,4
    jr ra
    
base:
    addi a0,x0,1
    j exit