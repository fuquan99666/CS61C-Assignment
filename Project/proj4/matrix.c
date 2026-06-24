#include "matrix.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <immintrin.h>
#include <x86intrin.h>
#endif

/* Below are some intel intrinsics that might be useful
 * void _mm256_storeu_pd (double * mem_addr, __m256d a)
 * __m256d _mm256_set1_pd (double a)
 * __m256d _mm256_set_pd (double e3, double e2, double e1, double e0)
 * __m256d _mm256_loadu_pd (double const * mem_addr)
 * __m256d _mm256_add_pd (__m256d a, __m256d b)
 * __m256d _mm256_sub_pd (__m256d a, __m256d b)
 * __m256d _mm256_fmadd_pd (__m256d a, __m256d b, __m256d c)
 * __m256d _mm256_mul_pd (__m256d a, __m256d b)
 * __m256d _mm256_cmp_pd (__m256d a, __m256d b, const int imm8)
 * __m256d _mm256_and_pd (__m256d a, __m256d b)
 * __m256d _mm256_max_pd (__m256d a, __m256d b)
*/

/*
 * Generates a random double between `low` and `high`.
 */
double rand_double(double low, double high) {
    double range = (high - low);
    double div = RAND_MAX / range;
    return low + (rand() / div);
}

/*
 * Generates a random matrix with `seed`.
 */
void rand_matrix(matrix *result, unsigned int seed, double low, double high) {
    srand(seed);
    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < result->cols; j++) {
            set(result, i, j, rand_double(low, high));
        }
    }
}

/*
 * Allocate space for a matrix struct pointed to by the double pointer mat with
 * `rows` rows and `cols` columns. You should also allocate memory for the data array
 * and initialize all entries to be zeros. Remember to set all fieds of the matrix struct.
 * `parent` should be set to NULL to indicate that this matrix is not a slice.
 * You should return -1 if either `rows` or `cols` or both have invalid values, or if any
 * call to allocate memory in this function fails. If you don't set python error messages here upon
 * failure, then remember to set it in numc.c.
 * Return 0 upon success and non-zero upon failure.
 */
int allocate_matrix(matrix **mat, int rows, int cols) {
    /* TODO: YOUR CODE HERE */
    if (rows <= 0 || cols <= 0) {
        PyErr_SetString(PyExc_ValueError, "Matrix dimensions must be positive.");
        return -1; // Invalid dimensions 
    }

    double **data = (double **)malloc(rows * sizeof(double *));
    if (data == NULL) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for matrix data.");
        return -1; // Memory allocation failed
    }

    double *block = (double *)calloc(rows * cols, sizeof(double));
    if (block == NULL) {
        free(data);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for matrix block.");
        return -1; // Memory allocation failed
    }

    for (int i = 0; i < rows; i++) {
        data[i] = block + i * cols; // Point each row to the correct position in the block
    }

    *mat = (matrix *)malloc(sizeof(matrix));
    if (*mat == NULL) {
        free(data);
        free(block);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for matrix struct.");
        return -1; // Memory allocation failed
    }

    (*mat)->data = data;
    (*mat)->rows = rows;
    (*mat)->cols = cols;
    (*mat)->parent = NULL;
    (*mat)->is_1d = (rows == 1 || cols == 1) ? 1 : 0;
    (*mat)->ref_cnt = (int *)malloc(sizeof(int));
    (*mat)->stride = cols;
    (*mat)->block = block;

    if ((*mat)->ref_cnt == NULL) {
        // Free allocated data and matrix struct before returning
        free(data);
        free(block);
        free(*mat);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for reference count.");
        return -1; // Memory allocation failed
    }

    (*mat)->ref_cnt[0] = 1; // Initialize reference count to 1

    return 0; // Success
}

/*
 * Allocate space for a matrix struct pointed to by `mat` with `rows` rows and `cols` columns.
 * This is equivalent to setting the new matrix to be
 * from[row_offset:row_offset + rows, col_offset:col_offset + cols]
 * If you don't set python error messages here upon failure, then remember to set it in numc.c.
 * Return 0 upon success and non-zero upon failure.
 */
int allocate_matrix_ref(matrix **mat, matrix *from, int row_offset, int col_offset,
                        int rows, int cols) {
    /* TODO: YOUR CODE HERE */
    // The new matrix is a slice of the original matrix.
    
    if (rows <= 0 || cols <= 0 || row_offset < 0 || col_offset < 0 ||
        row_offset + rows > from->rows || col_offset + cols > from->cols) {
        PyErr_SetString(PyExc_ValueError, "Invalid dimensions or offsets for matrix slice.");
        return -1; // Invalid dimensions or offsets
    }

    *mat = (matrix *)malloc(sizeof(matrix));
    if (*mat == NULL) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for matrix struct.");
        return -1; // Memory allocation failed
    }

    double **data = (double **)malloc(rows * sizeof(double *));
    if (data == NULL) {
        free(*mat);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for matrix data.");
        return -1; // Memory allocation failed
    }

    for (int i = 0; i < rows; i++) {
        data[i] = from->data[0] + (row_offset + i) * from->stride + col_offset; // Point to the correct row and column
    }
    (*mat)->block = from->block; // Share the same block of data
    (*mat)->data = data;
    (*mat)->rows = rows;
    (*mat)->cols = cols;
    (*mat)->stride = from->stride; // Maintain the same stride as the parent matrix
    (*mat)->parent = from;
    (*mat)->is_1d = (rows == 1 || cols == 1) ? 1 : 0;
    from->ref_cnt[0]++; // Increment reference count of the all ref matrix
    (*mat)->ref_cnt = from->ref_cnt; // Initialize reference count pointer to the same as the parent matrix

    return 0; // Success
}

/*
 * This function will be called automatically by Python when a numc matrix loses all of its
 * reference pointers.
 * You need to make sure that you only free `mat->data` if no other existing matrices are also
 * referring this data array.
 * See the spec for more information.
 */
void deallocate_matrix(matrix *mat) {
    /* TODO: YOUR CODE HERE */
    if (mat == NULL) {
        return ; // Nothing to deallocate
    }

    mat->ref_cnt[0]--; // Decrement reference count
    free(mat->data);

    if (mat->ref_cnt[0] == 0) {
        // No other matrices are referring to this data array, free it 
        free(mat->block);
        free(mat->ref_cnt);
    }

    free(mat); // Free the matrix struct itself
}

/*
 * Return the double value of the matrix at the given row and column.
 * You may assume `row` and `col` are valid.
 */
double get(matrix *mat, int row, int col) {
    /* TODO: YOUR CODE HERE */
    return mat->data[row][col];
}

/*
 * Set the value at the given row and column to val. You may assume `row` and
 * `col` are valid
 */
void set(matrix *mat, int row, int col, double val) {
    /* TODO: YOUR CODE HERE */
    mat->data[row][col] = val;
}

/*
 * Set all entries in mat to val
 */
void fill_matrix(matrix *mat, double val) {
    /* TODO: YOUR CODE HERE */
    for (int i = 0; i < mat->rows; i++) {
        for (int j = 0; j < mat->cols; j++) {
            mat->data[i][j] = val;
        }
    }
}

/*
 * Store the result of adding mat1 and mat2 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int add_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    /* TODO: YOUR CODE HERE */
    if (mat1->rows != mat2->rows || mat1->cols != mat2->cols) {
        PyErr_SetString(PyExc_ValueError, "Matrix dimensions must match for addition.");
        return -1; // Dimension mismatch
    }

    if (result->rows != mat1->rows || result->cols != mat1->cols) {
        PyErr_SetString(PyExc_ValueError, "Result matrix dimensions must match input matrices.");
        return -1; // Result matrix dimension mismatch
    }

    for (int i = 0; i < mat1->rows; i++) {
        for (int j = 0; j < mat1->cols; j++) {
            result->data[i][j] = mat1->data[i][j] + mat2->data[i][j];
        }
    }
    return 0; // Success
}

/*
 * Store the result of subtracting mat2 from mat1 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int sub_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    /* TODO: YOUR CODE HERE */
    if (mat1->rows != mat2->rows || mat1->cols != mat2->cols) {
        PyErr_SetString(PyExc_ValueError, "Matrix dimensions must match for subtraction.");
        return -1; // Dimension mismatch
    }

    if (result->rows != mat1->rows || result->cols != mat1->cols) {
        PyErr_SetString(PyExc_ValueError, "Result matrix dimensions must match input matrices.");
        return -1; // Result matrix dimension mismatch
    }


    for (int i = 0; i < mat1->rows; i++) {
        for (int j = 0; j < mat1->cols; j++) {
            result->data[i][j] = mat1->data[i][j] - mat2->data[i][j];
        }
    }
    return 0; // Success
}

/*
 * Store the result of multiplying mat1 and mat2 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 * Remember that matrix multiplication is not the same as multiplying individual elements.
 */
int mul_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    /* TODO: YOUR CODE HERE */
    if (mat1->cols != mat2->rows) {
        PyErr_SetString(PyExc_ValueError, "Matrix dimensions are not compatible for multiplication.");
        return -1; // Dimension mismatch
    }

    if (result->rows != mat1->rows || result->cols != mat2->cols) {
        PyErr_SetString(PyExc_ValueError, "Result matrix dimensions must match the product of input matrices.");
        return -1; // Result matrix dimension mismatch
    }

    for(int i = 0; i < mat1->rows; i++) {
        for (int j = 0; j < mat2->cols; j++) {
            result->data[i][j] = 0; 
            for (int k = 0; k < mat1->cols; k++) {
                result->data[i][j] += mat1->data[i][k] * mat2->data[k][j];
            }
        }
    }
    return 0; // Success
}

/*
 * Store the result of raising mat to the (pow)th power to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 * Remember that pow is defined with matrix multiplication, not element-wise multiplication.
 */
int pow_matrix(matrix *result, matrix *mat, int pow) {
    /* TODO: YOUR CODE HERE */

    if (mat->rows != mat->cols) {
        PyErr_SetString(PyExc_ValueError, "Matrix must be square for exponentiation.");
        return -1; // Not a square matrix
    }

    if (pow < 0) {
        PyErr_SetString(PyExc_ValueError, "Exponent must be non-negative.");
        return -1; // Negative exponent not supported
    }

    if (result->rows != mat->rows || result->cols != mat->cols) {
        PyErr_SetString(PyExc_ValueError, "Result matrix dimensions must match input matrix.");
        return -1; // Result matrix dimension mismatch
    }

    matrix *tmp; 

    int flag = allocate_matrix(&tmp, mat->rows, mat->cols);
    if (flag != 0) {
        return flag; // Allocation failed
    }

    // convert mat to I matrix
    for (int i = 0; i < mat->rows; i++) {
        for (int j = 0; j < mat->cols; j++) {
            if (i == j) {
                result->data[i][j] = 1.0;
            } else {
                result->data[i][j] = 0.0;
            }
        }
    }



    for (int i = 0; i < pow; i++) {
        flag = mul_matrix(tmp, result, mat);
        if (flag != 0) {
            deallocate_matrix(tmp);
            return -1; 
        }

        // Update result to the new product
        for (int r = 0; r < mat->rows; r++) {
            for (int c = 0; c < mat->cols; c++) {
                result->data[r][c] = tmp->data[r][c];
            }
        }
    }
    deallocate_matrix(tmp);
    return 0;
}

/*
 * Store the result of element-wise negating mat's entries to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int neg_matrix(matrix *result, matrix *mat) {
    /* TODO: YOUR CODE HERE */

    if (result->rows != mat->rows || result->cols != mat->cols) {
        PyErr_SetString(PyExc_ValueError, "Result matrix dimensions must match input matrix.");
        return -1; // Result matrix dimension mismatch
    }

    for (int i = 0; i < mat->rows; i++) {
        for (int j = 0; j < mat->cols; j++) {
            result->data[i][j] = -mat->data[i][j];
        }
    }
    return 0; // Success

}

/*
 * Store the result of taking the absolute value element-wise to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int abs_matrix(matrix *result, matrix *mat) {
    /* TODO: YOUR CODE HERE */

    if (result->rows != mat->rows || result->cols != mat->cols) {
        PyErr_SetString(PyExc_ValueError, "Result matrix dimensions must match input matrix.");
        return -1; // Result matrix dimension mismatch
    }

    for (int i = 0; i < mat->rows; i++) {
        for (int j = 0; j < mat->cols; j++) {
            if (mat->data[i][j] < 0) {
                result->data[i][j] = -mat->data[i][j];
            } else {
                result->data[i][j] = mat->data[i][j];
            }
        }
    }
    return 0; // Success
}

