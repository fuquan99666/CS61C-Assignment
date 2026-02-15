.globl read_matrix

.text
# ==============================================================================
# FUNCTION: Allocates memory and reads in a binary file as a matrix of integers
#
# FILE FORMAT:
#   The first 8 bytes are two 4 byte ints representing the # of rows and columns
#   in the matrix. Every 4 bytes afterwards is an element of the matrix in
#   row-major order.
# Arguments:
#   a0 (char*) is the pointer to string representing the filename
#   a1 (int*)  is a pointer to an integer, we will set it to the number of rows
#   a2 (int*)  is a pointer to an integer, we will set it to the number of columns
# Returns:
#   a0 (int*)  is the pointer to the matrix in memory
# Exceptions:
# - If malloc returns an error,
#   this function terminates the program with error code 88.
# - If you receive an fopen error or eof, 
#   this function terminates the program with error code 90.
# - If you receive an fread error or eof,
#   this function terminates the program with error code 91.
# - If you receive an fclose error or eof,
#   this function terminates the program with error code 92.
# ==============================================================================
read_matrix:

    # Prologue

    addi sp, sp, -28
    sw ra, 0(sp) # because we will call other functions in this function
    sw s0, 4(sp)
    sw s1, 8(sp)    
    sw s2, 12(sp)
    sw s3, 16(sp)
    sw s4, 20(sp)
    sw s5, 24(sp)

    mv s4, a1
    mv s5, a2

    # we should first malloc a memory to store the matrix , use the malloc function that has 
    # written in the utils.s

    # before that , we should make sense how many bytes we need to malloc, we should use 
    # fopen and read the first 16 bytes to get the number of rows and columns 
    # then malloc those 

    # first , read the file 
    # use fopen to get the file discriptor (a int) 
    # because we just need to read the file, so we set the a2 mode to 0 (read only)
    mv a2, x0
    mv a1, a0 # filename in a1

    call fopen 

    # check if fopen is successful 
    blt a0, x0, error2
    # we use s2 to store the file discriptor for later use
    mv s2, a0

    # use malloc to allocate a memory to store the matrix
    # first malloc 16 bytes to store the number of rows and columns
    li a0, 16
    call malloc

    # check if malloc is successful
    # how to check ? a0 is return value of pointer to the allocated memory
    # the malloc is fail when a0 is less than 0(-1)

    beq a0, x0, error1
    mv s3, a0 # use s3 to store the pointer to the allocated memory for rows and columns 

    # read the first 8 bytes to get the numbers of rows and columns    
    mv a1, s2 # file discriptor in a1
    mv a2, a0 # buffer to store the row and column number in a2
    li a3, 8 # number of bytes to read
    call fread

    # check if fread is successful
    li t0, 8
    bne a0, t0, error3

    lw s0, 0(s3) # row
    lw s1, 4(s3) # column 


    # before malloc the total memory for the matrix
    # we should first free the first 16 bytes that we used to store the row and column number 
    mv a0, s3
    call free

    # compute the total number of bytes we need to malloc for the matrix
    mul t0, s0, s1 # row * column
    slli t0, t0, 2

    mv a0, t0
    call malloc 

    # check if malloc is successful 
    beq a0, x0, error1

    # use s3 to store the pointer to the allocated memory for the all matrix elements
    mv s3, a0
    mv a1, s2 # file discriptor in a1
    mv a2, s3 # buffer to store the matrix elements in a2
    mv a3, t0 # number of bytes to read in a3 , actually we should save it because we have called malloc but 
               

    call fread
    # check if read
    bne a0, t0, error3

    # now the all matrix elements should store in the memory that s3 point to

    # close the file 
    mv a1, s2
    call fclose

    # check if close
    blt a0, x0 , error4

    mv a0 , s3
    sw s0, 0(s4) # notice that here s4 and s5 is a pointer to integer
    sw s1, 0(s5)

    # Epilogue
    lw s5, 24(sp)
    lw s4, 20(sp)
    lw s3, 16(sp)
    lw s2, 12(sp)
    lw s1, 8(sp)
    lw s0, 4(sp)
    lw ra, 0(sp)
    addi sp, sp, 28

    ret

error1:
    li a1, 88
    call exit2


error2:
    li a1, 90
    call exit2


error3:
    li a1, 91
    call exit2

error4:
    li a1, 92
    call exit2
