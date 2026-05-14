# 矩阵乘法: C = A * B, 大小 N x N
# 三重循环:
# for i in 0..N-1:
#   for j in 0..N-1:
#     sum = 0
#     for k in 0..N-1:
#       sum += A[i*N + k] * B[k*N + j]
#     C[i*N + j] = sum

.equ N, 4
.equ A_BASE, 0x2000
.equ B_BASE, 0x2100
.equ C_BASE, 0x2200

_start:
    # li s0, N
    addi s0, x0, 4            # s0 = N = 4

    # li s1, A_BASE = 0x2000
    lui  s1, 0x2              # s1 = 0x2000
    addi s1, s1, 0            # 加上低12位（0）

    # li s2, B_BASE = 0x2100
    lui  s2, 0x2              # s2 = 0x2000
    addi s2, s2, 0x100        # 加上低12位（0x100）

    # li s3, C_BASE = 0x2200
    lui  s3, 0x2              # s3 = 0x2000
    addi s3, s3, 0x200        # 加上低12位（0x200）

    # li s4, 0 (i = 0)
    addi s4, x0, 0

loop_i:
    bge  s4, s0, end

    # li s5, 0 (j = 0)
    addi s5, x0, 0

loop_j:
    bge  s5, s0, next_i

    # li s6, 0 (sum = 0)
    addi s6, x0, 0

    # li s7, 0 (k = 0)
    addi s7, x0, 0

loop_k:
    bge  s7, s0, store_c

    # 计算 A[i][k] 地址
    mul  t0, s4, s0          # t0 = i * N
    add  t0, t0, s7          # t0 = i*N + k
    slli t0, t0, 2           # t0 = t0 * 4
    add  t0, s1, t0          # t0 = A[i][k] 地址
    lw   t1, 0(t0)           # t1 = A[i][k]

    # 计算 B[k][j] 地址
    mul  t2, s7, s0          # t2 = k * N
    add  t2, t2, s5          # t2 = k*N + j
    slli t2, t2, 2           # t2 = t2 * 4
    add  t2, s2, t2          # t2 = B[k][j] 地址
    lw   t3, 0(t2)           # t3 = B[k][j]

    mul  t4, t1, t3          # t4 = A[i][k] * B[k][j]
    add  s6, s6, t4          # sum = sum + t4

    addi s7, s7, 1           # k = k + 1
    jal  x0, loop_k

store_c:
    # 计算 C[i][j] 地址
    mul  t0, s4, s0          # t0 = i * N
    add  t0, t0, s5          # t0 = i*N + j
    slli t0, t0, 2           # t0 = t0 * 4
    add  t0, s3, t0          # t0 = C[i][j] 地址
    sw   s6, 0(t0)           # 存储 sum

    addi s5, s5, 1           # j = j + 1
    jal  x0, loop_j

next_i:
    addi s4, s4, 1           # i = i + 1
    jal  x0, loop_i

end:
    # 程序结束，死循环
    nop
