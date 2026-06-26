from utils import *
from unittest import TestCase

"""
For each operation, you should write tests to test  on matrices of different sizes.
Hint: use dp_mc_matrix to generate dumbpy and numc matrices with the same data and use
      cmp_dp_nc_matrix to compare the results
"""
class TestAdd(TestCase):
    def test_small_add(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(2, 2, seed=0)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(2, 2, seed=5)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_medium_add(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(100, 100, seed=7)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(100, 100, seed=1)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_add(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(1000, 1000, seed=13)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(1000, 1000, seed=15)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_larger_add(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(3000, 3000, seed=13)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(3000, 3000, seed=15)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "add")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

class TestSub(TestCase):
    def test_small_sub(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(1000, 1000, seed=3)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(1000, 1000, seed=4)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "sub")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_medium_sub(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(5000, 5000, seed=7)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(5000, 5000, seed=8)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "sub")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_sub(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(10000, 10000, seed=33)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(10000, 10000, seed=42)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "sub")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

class TestAbs(TestCase):
    def test_small_abs(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(100, 150, seed=88)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "abs")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_medium_abs(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(3330, 4920, seed=98)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "abs")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_abs(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(12000, 13333, seed=10)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "abs")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

class TestNeg(TestCase):
    def test_small_neg(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "neg")
        self.assertTrue(is_correct)
        print_speedup(speed_up)
    def test_medium_neg(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(100, 100, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "neg")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_neg(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(1000, 1000, seed=0)
        is_correct, speed_up = compute([dp_mat], [nc_mat], "neg")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

class TestMul(TestCase):
    def test_small_mul(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(2, 2, seed=3)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(2, 2, seed=4)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "mul")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_medium_mul(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(100, 100, seed=4)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(100, 100, seed=5)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "mul")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_mul(self):
        # TODO: YOUR CODE HERE
        dp_mat1, nc_mat1 = rand_dp_nc_matrix(1000, 1000, seed=5)
        dp_mat2, nc_mat2 = rand_dp_nc_matrix(1000, 1000, seed=6)
        is_correct, speed_up = compute([dp_mat1, dp_mat2], [nc_mat1, nc_mat2], "mul")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

class TestPow(TestCase):
    def test_small_pow(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=0)
        is_correct, speed_up = compute([dp_mat, 3], [nc_mat, 3], "pow")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_medium_pow(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(100, 100, seed=4)
        is_correct, speed_up = compute([dp_mat, 5], [nc_mat, 5], "pow")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

    def test_large_pow(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(1000, 1000, seed=9)
        is_correct, speed_up = compute([dp_mat, 8], [nc_mat, 8], "pow")
        self.assertTrue(is_correct)
        print_speedup(speed_up)

class TestGet(TestCase):
    def test_get(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=333)
        rand_row = np.random.randint(dp_mat.shape[0])
        rand_col = np.random.randint(dp_mat.shape[1])
        self.assertEqual(round(dp_mat[rand_row][rand_col], decimal_places),
            round(nc_mat[rand_row][rand_col], decimal_places))
        
    def test_get_out_of_bounds(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=333)
        _ = nc_mat[3][3]
        _ = dp_mat[3][3]

class TestSet(TestCase):
    def test_set(self):
        # TODO: YOUR CODE HERE
        dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=0)
        rand_row = np.random.randint(dp_mat.shape[0])
        rand_col = np.random.randint(dp_mat.shape[1])
        rand_val = np.random.uniform(-100, 100)
        dp_mat[rand_row][rand_col] = rand_val
        nc_mat[rand_row][rand_col] = rand_val
        self.assertEqual(round(dp_mat[rand_row][rand_col], decimal_places),
            round(nc_mat[rand_row][rand_col], decimal_places))
    def test_set_middle(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(5,5,seed=33)
        row_start = np.random.randint(0, 3)
        row_end = np.random.randint(row_start+2, 5)
        col_start = np.random.randint(0, 3)
        col_end = np.random.randint(col_start+2, 5)


        dp_mat_1 = dp_mat[row_start:row_end, col_start:col_end]
        nc_mat_1 = nc_mat[row_start:row_end, col_start:col_end]


        val_mat = np.random.uniform(-100, 100, size=(row_end - row_start, col_end - col_start))
        val_mat_list = val_mat.tolist()
        
        dp_mat_1[0:row_end - row_start, 0:col_end - col_start] = val_mat_list
        nc_mat_1[0:row_end - row_start, 0:col_end - col_start] = val_mat_list

        self.assertTrue(cmp_dp_nc_matrix(dp_mat, nc_mat))

    def test_set_large(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(10, 10, seed=33)
        row_start = np.random.randint(0, 8)
        row_end = np.random.randint(row_start+2, 10)
        col_start = np.random.randint(0, 8)
        col_end = np.random.randint(col_start+2, 10)


        dp_mat_1 = dp_mat[row_start:row_end, col_start:col_end]
        nc_mat_1 = nc_mat[row_start:row_end, col_start:col_end]


        val_mat = np.random.uniform(-100, 100, size=(row_end - row_start, col_end - col_start))
        val_mat_list = val_mat.tolist()

        dp_mat_1[0:row_end - row_start, 0:col_end - col_start] = val_mat_list
        nc_mat_1[0:row_end - row_start, 0:col_end - col_start] = val_mat_list

        self.assertTrue(cmp_dp_nc_matrix(dp_mat, nc_mat))

    def test_set_out_of_bounds(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(5, 5, seed=0)
        nc_mat[6][6] = 10
        dp_mat[6][6] = 10
    

class TestShape(TestCase):
    def test_shape(self):
        dp_mat, nc_mat = rand_dp_nc_matrix(2, 2, seed=0)
        self.assertTrue(dp_mat.shape == nc_mat.shape)
