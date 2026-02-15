.globl write_matrix

.text
# ==============================================================================
# FUNCTION: Writes a matrix of integers into a binary file
# FILE FORMAT:
#   The first 8 bytes of the file will be two 4 byte ints representing the
#   numbers of rows and columns respectively. Every 4 bytes thereafter is an
#   element of the matrix in row-major order.
# Arguments:
#   a0 (char*) is the pointer to string representing the filename
#   a1 (int*)  is the pointer to the start of the matrix in memory
#   a2 (int)   is the number of rows in the matrix
#   a3 (int)   is the number of columns in the matrix
# Returns:
#   None
# Exceptions:
# - If you receive an fopen error or eof,
#   this function terminates the program with error code 93.
# - If you receive an fwrite error or eof,
#   this function terminates the program with error code 94.
# - If you receive an fclose error or eof,
#   this function terminates the program with error code 95.
# ==============================================================================
write_matrix:

    # Prologue

    addi sp, sp, -24
    sw ra, 0(sp)
    sw s0, 4(sp)
    sw s1, 8(sp)
    sw s2, 12(sp)
    sw s3, 16(sp)
    sw s4, 20(sp)

    # save the arguments in s registers for later use 
    mv s0, a0 # filename
    mv s1, a1 # pointer to the start of the matrix in memory
    mv s2, a2 # number of rows in the matrix
    mv s3, a3 # number of columns in the matrix

    # first , we should open the file to get discriptor
    mv a1, s0
    li a2, 1 # mode = 1 for write only 
    call fopen 

    # check if fopen
    blt a0, x0, error1
    mv s4, a0 # save the file discriptor for later use 

    # for fwrite, a2 is a buffer that contains the content we want to write 
    # question is that we just need to get a bin file, so directly write the matrix into the file ?
    # obiviously firstly should write the number of rows and columns into the file 

    # maybe we should create a buffer for the number of rows and columns ?
    # use malloc ?
    li a0, 8
    call malloc
    beq a0, x0, error4

    sw s2, 0(a0)
    sw s3, 4(a0)
    mv t3, a0

    mv a1, s4
    mv a2, a0
    li a3, 2
    li a4, 4
    call fwrite 

    # check if fwrite 
    bne a0, a3, error2

    # close the small buffer
    mv a0, t3
    call free

    # then write the matrix into the file
    mv a1, s4
    mv a2, s1
    mul t0, s2,s3
    mv a3, t0
    li a4, 4 
    call fwrite

    bne a0, a3, error2

    # close the file 
    mv a1, s4
    call fclose

    blt a0, x0, error3

    # Epilogue
    lw ra, 0(sp)
    lw s0, 4(sp)
    lw s1, 8(sp)
    lw s2, 12(sp)
    lw s3, 16(sp)
    lw s4, 20(sp)
    addi sp, sp, 24

    ret

error1:
    li a1, 93
    call exit2

error2:
    li a1, 94
    call exit2

error3:
    li a1, 95
    call exit2

error4:
    li a1, 96
    call exit2