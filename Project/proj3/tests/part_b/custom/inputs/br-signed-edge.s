# 测试有符号比较的边界情况
_start:
    # 最大正数 0x7FFFFFFF  vs 最小负数 0x80000000
    lui  x1, 0x7FFFF
    addi x1, x1, 0x7FF        # x1 = 0x7FFFFFFF
    lui  x2, 0x80000
    addi x2, x2, 0x000        # x2 = 0x80000000

    # 7FFFFFFF < 80000000 ? 有符号比较应该是 false（正数不小于负数）
    blt  x1, x2, fail         # 不应该跳转，跳转则说明错了

    # 正确：继续执行
    addi x3, x0, 1

fail:
    addi x3, x0, 0x315# 错误码
