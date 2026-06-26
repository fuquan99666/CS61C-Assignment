"""
This file contains some helper functions for testing
Provided by CS 61C staff
"""

import numc as nc
import numpy as np
import hashlib, struct
from typing import Union, List
import operator
import time


class DumbpyLikeMatrix:
    """模拟 dumbpy.Matrix 的接口，底层用 NumPy 数组"""
    
    def __init__(self, *args, rand=False, seed=0, low=0.0, high=1.0):
        
        # 随机矩阵：Matrix(rows, cols, rand=True, seed=0)
        
        if rand:
            rows, cols = args[0], args[1]
            # ✅ 直接用 numc 生成随机矩阵
            nc_mat = nc.Matrix(rows, cols, rand=True, seed=seed, low=low, high=high)
            # 转换成 NumPy 数组
            self._data = np.array(nc.to_list(nc_mat))
        else:
            # 情况1：Matrix(rows, cols) → 全零矩阵
            if len(args) == 2 and isinstance(args[0], int):
                rows, cols = args
                self._data = np.zeros((rows, cols))
            # 情况2：Matrix(2d_list) → 从列表创建
            elif len(args) == 1 and isinstance(args[0], list):
                self._data = np.array(args[0], dtype=np.float64)
            # 情况3：Matrix(rows, cols, 1d_list) → 从一维列表填充
            elif len(args) == 3 and isinstance(args[0], int):
                rows, cols, lst = args
                self._data = np.array(lst, dtype=np.float64).reshape(rows, cols)
            else:
                raise ValueError("Unsupported constructor arguments")
    
    @property
    def shape(self):
        # 一维矩阵返回 (n,)，二维返回 (rows, cols)
        if self._data.ndim == 1:
            return (self._data.shape[0],)
        return self._data.shape
    
    def __getitem__(self, key):
        # 情况1：整数索引
        if isinstance(key, int):
            return self._data[key]
        
        # 情况2：切片索引
        if isinstance(key, slice):
            return DumbpyLikeMatrix.from_array(self._data[key])
        
        # 情况3：元组索引（支持整数、切片组合）
        if isinstance(key, tuple):
            if len(key) == 1:
                return self._data[key[0]]
            if len(key) == 2:
                row_key, col_key = key[0], key[1]
                
                # 两个都是整数
                if isinstance(row_key, int) and isinstance(col_key, int):
                    return self._data[row_key, col_key]
                
                # 行整数 + 列切片
                if isinstance(row_key, int) and isinstance(col_key, slice):
                    return DumbpyLikeMatrix.from_array(self._data[row_key, col_key])
                
                # 行切片 + 列整数
                if isinstance(row_key, slice) and isinstance(col_key, int):
                    return DumbpyLikeMatrix.from_array(self._data[row_key, col_key])
                
                # 两个都是切片
                if isinstance(row_key, slice) and isinstance(col_key, slice):
                    return DumbpyLikeMatrix.from_array(self._data[row_key, col_key])
        
        raise TypeError(f"Invalid key type: {type(key)}")
    
    def __setitem__(self, key, value):
        # 情况1：整数索引
        if isinstance(key, int):
            self._data[key] = value
            return
        
        # 情况2：切片索引（1D 赋值）
        if isinstance(key, slice):
            if isinstance(value, DumbpyLikeMatrix):
                self._data[key] = value._data
            else:
                self._data[key] = value
            return
        
        # 情况3：元组索引（支持整数、切片组合）
        if isinstance(key, tuple):
            if len(key) == 2:
                row_key, col_key = key[0], key[1]
                
                # 转换 value 为 NumPy 数组（如果是 DumbpyLikeMatrix）
                if isinstance(value, DumbpyLikeMatrix):
                    value = value._data
                
                # 两个都是整数
                if isinstance(row_key, int) and isinstance(col_key, int):
                    self._data[row_key, col_key] = value
                    return
                
                # 行整数 + 列切片
                if isinstance(row_key, int) and isinstance(col_key, slice):
                    self._data[row_key, col_key] = value
                    return
                
                # 行切片 + 列整数
                if isinstance(row_key, slice) and isinstance(col_key, int):
                    self._data[row_key, col_key] = value
                    return
                
                # 两个都是切片
                if isinstance(row_key, slice) and isinstance(col_key, slice):
                    self._data[row_key, col_key] = value
                    return
        
        raise TypeError(f"Invalid key type for assignment: {type(key)}")
        
    # 运算符重载
    def __add__(self, other):
        if isinstance(other, DumbpyLikeMatrix):
            return DumbpyLikeMatrix.from_array(self._data + other._data)
        return DumbpyLikeMatrix.from_array(self._data + other)
    
    def __sub__(self, other):
        if isinstance(other, DumbpyLikeMatrix):
            return DumbpyLikeMatrix.from_array(self._data - other._data)
        return DumbpyLikeMatrix.from_array(self._data - other)
    
    def __mul__(self, other):
        # 矩阵乘法（不是逐元素乘）
        if isinstance(other, DumbpyLikeMatrix):
            return DumbpyLikeMatrix.from_array(self._data @ other._data)
        return DumbpyLikeMatrix.from_array(self._data * other)
    
    def __neg__(self):
        return DumbpyLikeMatrix.from_array(-self._data)
    
    def __abs__(self):
        return DumbpyLikeMatrix.from_array(np.abs(self._data))
    
    def __pow__(self, power):
        # 矩阵幂
        result = np.eye(self._data.shape[0])
        base = self._data.copy()
        n = power
        while n > 0:
            if n & 1:
                result = result @ base
            base = base @ base
            n >>= 1
        return DumbpyLikeMatrix.from_array(result)
    
    def __repr__(self):
        return f"DumbpyLikeMatrix({self._data.tolist()})"
    
    @classmethod
    def from_array(cls, arr):
        """从 NumPy 数组创建实例"""
        obj = cls.__new__(cls)
        obj._data = arr
        return obj

dp = type('dp', (), {'Matrix': DumbpyLikeMatrix})()

# import dumbpy as dp


"""
Global vars
"""
num_samples = 1000
decimal_places = 6
func_mapping = {
    "add": operator.add,
    "sub": operator.sub,
    "mul": operator.mul,
    "neg": operator.neg,
    "abs": operator.abs,
    "pow": operator.pow
}
"""
Returns a dumbpy matrix and a numc matrix with the same data
"""
def dp_nc_matrix(*args, **kwargs):
    if len(kwargs) > 0:
        return dp.Matrix(*args, **kwargs), nc.Matrix(*args, **kwargs)
    else:
        return dp.Matrix(*args), nc.Matrix(*args)

"""
Returns a random dumbpy matrix and a random numc matrix with the same data
seed, low, and high are optional
"""
def rand_dp_nc_matrix(rows, cols, seed=0):
    return dp.Matrix(rows, cols, rand=True, seed=seed), nc.Matrix(rows, cols, rand=True, seed=seed)


"""
Returns whether the given dumbpy matrix dp_mat is equal to the numc matrix nc_mat
This function allows a reasonable margin of( floating point errors
"""
def cmp_dp_nc_matrix(dp_mat: dp.Matrix, nc_mat: nc.Matrix):
    return rand_md5(dp_mat) == rand_md5(nc_mat)

"""
Test if numc returns the correct result given an operation and some matrices.
If speed_up is set to True, returns the speedup as well
"""
def compute(dp_mat_lst: List[Union[dp.Matrix, int]],
    nc_mat_lst: List[Union[nc.Matrix, int]], op: str):
    f = func_mapping[op]
    nc_start, nc_end, dp_start, dp_end = None, None, None, None
    nc_result, dp_result = None, None
    assert(op in list(func_mapping.keys()))
    if op == "neg" or op == "abs":
        assert(len(dp_mat_lst) == 1)
        assert(len(nc_mat_lst) == 1)
        nc_start = time.time()
        nc_result = f(nc_mat_lst[0])
        nc_end = time.time()

        dp_start = time.time()
        dp_result = f(dp_mat_lst[0])
        dp_end = time.time()
    else:
        assert(len(dp_mat_lst) > 1)
        assert(len(nc_mat_lst) > 1)
        nc_start = time.time()
        nc_result = nc_mat_lst[0]
        for mat in nc_mat_lst[1:]:
            nc_result = f(nc_result, mat)
        nc_end = time.time()

        dp_start = time.time()
        dp_result = dp_mat_lst[0]
        for mat in dp_mat_lst[1:]:
            dp_result = f(dp_result, mat)
        dp_end = time.time()
    # Check for correctness
    is_correct = cmp_dp_nc_matrix(dp_result, nc_result)
    if (not is_correct):
        print("Results do not match!")
        print("Dumbpy result:", dp_result)
        print("Numc result:", nc_result)
    
    return is_correct, (dp_end - dp_start) / (nc_end - nc_start)

"""
Print speedup
"""
def print_speedup(speed_up):
    print("Speed up is:", speed_up)

"""
Generate a md5 hash by sampling random elements in nc_mat
"""
def rand_md5(mat: Union[dp.Matrix, nc.Matrix]):
    np.random.seed(1)
    m = hashlib.md5()
    if len(mat.shape) > 1:
        rows, cols = mat.shape
        total_cnt = mat.shape[0] * mat.shape[1]
        if total_cnt < num_samples:
            for i in range(rows):
                for j in range(cols):
                    m.update(struct.pack("f", round(mat[i][j], decimal_places)))
        else:
            for _ in range(num_samples):
                i = np.random.randint(rows)
                j = np.random.randint(cols)
                m.update(struct.pack("f", round(mat[i][j], decimal_places)))
    else:
        total_cnt = mat.shape[0]
        if total_cnt < num_samples:
            for i in range(total_cnt):
                m.update(struct.pack("f", round(mat[i], decimal_places)))
        else:
            for _ in range(num_samples):
                i = np.random.randint(total_cnt)
                m.update(struct.pack("f", round(mat[i], decimal_places)))
    return m.digest()
