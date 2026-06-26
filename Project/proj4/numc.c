#include "numc.h"
#include <structmember.h>

PyTypeObject Matrix61cType;

/* Helper functions for initalization of matrices and vectors */

/*
 * Return a tuple given rows and cols
 */
PyObject *get_shape(int rows, int cols) {
  if (rows == 1 || cols == 1) {
    return PyTuple_Pack(1, PyLong_FromLong(rows * cols));
  } else {
    return PyTuple_Pack(2, PyLong_FromLong(rows), PyLong_FromLong(cols));
  }
}
/*
 * Matrix(rows, cols, low, high). Fill a matrix random double values
 */
int init_rand(PyObject *self, int rows, int cols, unsigned int seed, double low,
              double high) {
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed) return alloc_failed;
    rand_matrix(new_mat, seed, low, high);
    ((Matrix61c *)self)->mat = new_mat;
    ((Matrix61c *)self)->shape = get_shape(new_mat->rows, new_mat->cols);
    return 0;
}

/*
 * Matrix(rows, cols, val). Fill a matrix of dimension rows * cols with val
 */
int init_fill(PyObject *self, int rows, int cols, double val) {
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed)
        return alloc_failed;
    else {
        fill_matrix(new_mat, val);
        ((Matrix61c *)self)->mat = new_mat;
        ((Matrix61c *)self)->shape = get_shape(new_mat->rows, new_mat->cols);
    }
    return 0;
}

/*
 * Matrix(rows, cols, 1d_list). Fill a matrix with dimension rows * cols with 1d_list values
 */
int init_1d(PyObject *self, int rows, int cols, PyObject *lst) {
    if (rows * cols != PyList_Size(lst)) {
        PyErr_SetString(PyExc_ValueError, "Incorrect number of elements in list");
        return -1;
    }
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed) return alloc_failed;
    int count = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            set(new_mat, i, j, PyFloat_AsDouble(PyList_GetItem(lst, count)));
            count++;
        }
    }
    ((Matrix61c *)self)->mat = new_mat;
    ((Matrix61c *)self)->shape = get_shape(new_mat->rows, new_mat->cols);
    return 0;
}

/*
 * Matrix(2d_list). Fill a matrix with dimension len(2d_list) * len(2d_list[0])
 */
int init_2d(PyObject *self, PyObject *lst) {
    int rows = PyList_Size(lst);
    if (rows == 0) {
        PyErr_SetString(PyExc_ValueError,
                        "Cannot initialize numc.Matrix with an empty list");
        return -1;
    }
    int cols;
    if (!PyList_Check(PyList_GetItem(lst, 0))) {
        PyErr_SetString(PyExc_ValueError, "List values not valid");
        return -1;
    } else {
        cols = PyList_Size(PyList_GetItem(lst, 0));
    }
    for (int i = 0; i < rows; i++) {
        if (!PyList_Check(PyList_GetItem(lst, i)) ||
                PyList_Size(PyList_GetItem(lst, i)) != cols) {
            PyErr_SetString(PyExc_ValueError, "List values not valid");
            return -1;
        }
    }
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed) return alloc_failed;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            set(new_mat, i, j,
                PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(lst, i), j)));
        }
    }
    ((Matrix61c *)self)->mat = new_mat;
    ((Matrix61c *)self)->shape = get_shape(new_mat->rows, new_mat->cols);
    return 0;
}

/*
 * This deallocation function is called when reference count is 0
 */
void Matrix61c_dealloc(Matrix61c *self) {
    deallocate_matrix(self->mat);
    Py_TYPE(self)->tp_free(self);
}

/* For immutable types all initializations should take place in tp_new */
PyObject *Matrix61c_new(PyTypeObject *type, PyObject *args,
                        PyObject *kwds) {
    /* size of allocated memory is tp_basicsize + nitems*tp_itemsize*/
    Matrix61c *self = (Matrix61c *)type->tp_alloc(type, 0);
    return (PyObject *)self;
}

/*
 * This matrix61c type is mutable, so needs init function. Return 0 on success otherwise -1
 */
int Matrix61c_init(PyObject *self, PyObject *args, PyObject *kwds) {
    /* Generate random matrices */
    if (kwds != NULL) {
        PyObject *rand = PyDict_GetItemString(kwds, "rand");
        if (!rand) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
        if (!PyBool_Check(rand)) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
        if (rand != Py_True) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }

        PyObject *low = PyDict_GetItemString(kwds, "low");
        PyObject *high = PyDict_GetItemString(kwds, "high");
        PyObject *seed = PyDict_GetItemString(kwds, "seed");
        double double_low = 0;
        double double_high = 1;
        unsigned int unsigned_seed = 0;

        if (low) {
            if (PyFloat_Check(low)) {
                double_low = PyFloat_AsDouble(low);
            } else if (PyLong_Check(low)) {
                double_low = PyLong_AsLong(low);
            }
        }

        if (high) {
            if (PyFloat_Check(high)) {
                double_high = PyFloat_AsDouble(high);
            } else if (PyLong_Check(high)) {
                double_high = PyLong_AsLong(high);
            }
        }

        if (double_low >= double_high) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }

        // Set seed if argument exists
        if (seed) {
            if (PyLong_Check(seed)) {
                unsigned_seed = PyLong_AsUnsignedLong(seed);
            }
        }

        PyObject *rows = NULL;
        PyObject *cols = NULL;
        if (PyArg_UnpackTuple(args, "args", 2, 2, &rows, &cols)) {
            if (rows && cols && PyLong_Check(rows) && PyLong_Check(cols)) {
                return init_rand(self, PyLong_AsLong(rows), PyLong_AsLong(cols), unsigned_seed, double_low,
                                 double_high);
            }
        } else {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
    }
    PyObject *arg1 = NULL;
    PyObject *arg2 = NULL;
    PyObject *arg3 = NULL;
    if (PyArg_UnpackTuple(args, "args", 1, 3, &arg1, &arg2, &arg3)) {
        /* arguments are (rows, cols, val) */
        if (arg1 && arg2 && arg3 && PyLong_Check(arg1) && PyLong_Check(arg2) && (PyLong_Check(arg3)
                || PyFloat_Check(arg3))) {
            if (PyLong_Check(arg3)) {
                return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), PyLong_AsLong(arg3));
            } else
                return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), PyFloat_AsDouble(arg3));
        } else if (arg1 && arg2 && arg3 && PyLong_Check(arg1) && PyLong_Check(arg2) && PyList_Check(arg3)) {
            /* Matrix(rows, cols, 1D list) */
            return init_1d(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), arg3);
        } else if (arg1 && PyList_Check(arg1) && arg2 == NULL && arg3 == NULL) {
            /* Matrix(rows, cols, 1D list) */
            return init_2d(self, arg1);
        } else if (arg1 && arg2 && PyLong_Check(arg1) && PyLong_Check(arg2) && arg3 == NULL) {
            /* Matrix(rows, cols, 1D list) */
            return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), 0);
        } else {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
    } else {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return -1;
    }
}

/*
 * List of lists representations for matrices
 */
PyObject *Matrix61c_to_list(Matrix61c *self) {
    int rows = self->mat->rows;
    int cols = self->mat->cols;
    PyObject *py_lst = NULL;
    if (self->mat->is_1d) {  // If 1D matrix, print as a single list
        py_lst = PyList_New(rows * cols);
        int count = 0;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                PyList_SetItem(py_lst, count, PyFloat_FromDouble(get(self->mat, i, j)));
                count++;
            }
        }
    } else {  // if 2D, print as nested list
        py_lst = PyList_New(rows);
        for (int i = 0; i < rows; i++) {
            PyList_SetItem(py_lst, i, PyList_New(cols));
            PyObject *curr_row = PyList_GetItem(py_lst, i);
            for (int j = 0; j < cols; j++) {
                PyList_SetItem(curr_row, j, PyFloat_FromDouble(get(self->mat, i, j)));
            }
        }
    }
    return py_lst;
}

PyObject *Matrix61c_class_to_list(Matrix61c *self, PyObject *args) {
    PyObject *mat = NULL;
    if (PyArg_UnpackTuple(args, "args", 1, 1, &mat)) {
        if (!PyObject_TypeCheck(mat, &Matrix61cType)) {
            PyErr_SetString(PyExc_TypeError, "Argument must of type numc.Matrix!");
            return NULL;
        }
        Matrix61c* mat61c = (Matrix61c*)mat;
        return Matrix61c_to_list(mat61c);
    } else {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return NULL;
    }
}

/*
 * Add class methods
 */
PyMethodDef Matrix61c_class_methods[] = {
    {"to_list", (PyCFunction)Matrix61c_class_to_list, METH_VARARGS, "Returns a list representation of numc.Matrix"},
    {NULL, NULL, 0, NULL}
};

/*
 * Matrix61c string representation. For printing purposes.
 */
PyObject *Matrix61c_repr(PyObject *self) {
    PyObject *py_lst = Matrix61c_to_list((Matrix61c *)self);
    return PyObject_Repr(py_lst);
}

/* NUMBER METHODS */

/*
 * Add the second numc.Matrix (Matrix61c) object to the first one. The first operand is
 * self, and the second operand can be obtained by casting `args`.
 */
PyObject *Matrix61c_add(Matrix61c* self, PyObject* args) {
    /* TODO: YOUR CODE HERE */
    if (!PyObject_TypeCheck(args, &Matrix61cType)) {
        PyErr_SetString(PyExc_TypeError, "Argument must of type numc.Matrix!");
        return NULL;
    }

    Matrix61c* other = (Matrix61c*)args;

    // make a new numc.Matrix object to hold the result 
    Matrix61c* result = (Matrix61c*)Matrix61c_new(&Matrix61cType, NULL, NULL);

    allocate_matrix(&(result->mat), self->mat->rows, self->mat->cols);

    add_matrix(result->mat, self->mat, other->mat);
    result->shape = get_shape(self->mat->rows, self->mat->cols);

    return (PyObject*)result;

}

/*
 * Substract the second numc.Matrix (Matrix61c) object from the first one. The first operand is
 * self, and the second operand can be obtained by casting `args`.
 */
PyObject *Matrix61c_sub(Matrix61c* self, PyObject* args) {
    /* TODO: YOUR CODE HERE */

    if (!PyObject_TypeCheck(args, &Matrix61cType)) {
        PyErr_SetString(PyExc_TypeError, "Argument must of type numc.Matrix!");
        return NULL;
    }

    Matrix61c* other = (Matrix61c*)args;

    // make a new numc.Matrix object to hold the result
    Matrix61c* result = (Matrix61c*)Matrix61c_new(&Matrix61cType, NULL, NULL);

    allocate_matrix(&(result->mat), self->mat->rows, self->mat->cols);

    sub_matrix(result->mat, self->mat, other->mat);
    result->shape = get_shape(self->mat->rows, self->mat->cols);
    return (PyObject*)result;
}

/*
 * NOT element-wise multiplication. The first operand is self, and the second operand
 * can be obtained by casting `args`.
 */
PyObject *Matrix61c_multiply(Matrix61c* self, PyObject *args) {
    /* TODO: YOUR CODE HERE */

    if (!PyObject_TypeCheck(args, &Matrix61cType)) {
        PyErr_SetString(PyExc_TypeError, "Argument must of type numc.Matrix!");
        return NULL;
    }

    Matrix61c* other = (Matrix61c*)args;

    Matrix61c* result = (Matrix61c*)Matrix61c_new(&Matrix61cType, NULL, NULL);

    allocate_matrix(&(result->mat), self->mat->rows, other->mat->cols);

    mul_matrix(result->mat, self->mat, other->mat);
    result->shape = get_shape(self->mat->rows, other->mat->cols);
    return (PyObject*)result;
}

/*
 * Negates the given numc.Matrix.
 */
PyObject *Matrix61c_neg(Matrix61c* self) {
    /* TODO: YOUR CODE HERE */

    if (self == NULL) {
        PyErr_SetString(PyExc_TypeError, "Argument must of type numc.Matrix!");
        return NULL;
    }

    Matrix61c* result = (Matrix61c*)Matrix61c_new(&Matrix61cType, NULL, NULL);

    allocate_matrix(&(result->mat), self->mat->rows, self->mat->cols);

    neg_matrix(result->mat, self->mat);
    result->shape = get_shape(self->mat->rows, self->mat->cols);
    return (PyObject*)result;
}

/*
 * Take the element-wise absolute value of this numc.Matrix.
 */
PyObject *Matrix61c_abs(Matrix61c *self) {
    /* TODO: YOUR CODE HERE */

    if (self == NULL) {
        PyErr_SetString(PyExc_TypeError, "Argument must of type numc.Matrix!");
        return NULL;
    }

    Matrix61c* result = (Matrix61c*)Matrix61c_new(&Matrix61cType, NULL, NULL);

    allocate_matrix(&(result->mat), self->mat->rows, self->mat->cols);

    abs_matrix(result->mat, self->mat);
    result->shape = get_shape(self->mat->rows, self->mat->cols);
    return (PyObject*)result;
}

/*
 * Raise numc.Matrix (Matrix61c) to the `pow`th power. You can ignore the argument `optional`.
 */
PyObject *Matrix61c_pow(Matrix61c *self, PyObject *pow, PyObject *optional) {
    /* TODO: YOUR CODE HERE */

    if (!PyLong_Check(pow)) {
        PyErr_SetString(PyExc_TypeError, "Argument must be an integer!");
        return NULL;
    }

    Matrix61c* result = (Matrix61c*)Matrix61c_new(&Matrix61cType, NULL, NULL);

    allocate_matrix(&(result->mat), self->mat->rows, self->mat->cols);

    pow_matrix(result->mat, self->mat, PyLong_AsLong(pow));
    result->shape = get_shape(self->mat->rows, self->mat->cols);
    return (PyObject*)result;
}

/*
 * Create a PyNumberMethods struct for overloading operators with all the number methods you have
 * define. You might find this link helpful: https://docs.python.org/3.6/c-api/typeobj.html
 */
PyNumberMethods Matrix61c_as_number = {
    /* TODO: YOUR CODE HERE */
    .nb_add = (binaryfunc)Matrix61c_add,
    .nb_subtract = (binaryfunc)Matrix61c_sub,
    .nb_multiply = (binaryfunc)Matrix61c_multiply,
    .nb_negative = (unaryfunc)Matrix61c_neg,
    .nb_power = (ternaryfunc)Matrix61c_pow,
    .nb_absolute = (unaryfunc)Matrix61c_abs
};


/* INSTANCE METHODS */

/*
 * Given a numc.Matrix self, parse `args` to (int) row, (int) col, and (double/int) val.
 * Return None in Python (this is different from returning null).
 */
PyObject *Matrix61c_set_value(Matrix61c *self, PyObject* args) {
    /* TODO: YOUR CODE HERE */
    PyObject *row, *col, *val;

    // throw TypeError if args is not a tuple of length 3 , if i and j are not integers, or if val is not a float or int 
    if (!PyArg_UnpackTuple(args, "set", 3, 3, &row, &col, &val)){
        PyErr_SetString(PyExc_TypeError, "The num of argments is not 3");
        return NULL;
    }

    if (!PyLong_Check(row) || !PyLong_Check(col)) {
        PyErr_SetString(PyExc_TypeError, "Row and column indices must be integers.");
        return NULL;
    }

    if (!PyFloat_Check(val) && !PyLong_Check(val)) {
        PyErr_SetString(PyExc_TypeError, "Value must be a float or int.");
        return NULL;
    }

    int i = PyLong_AsLong(row);
    int j = PyLong_AsLong(col);
    if (PyFloat_Check(val)) {
        set(self->mat, i, j, PyFloat_AsDouble(val));
    } else {
        set(self->mat, i, j, PyLong_AsLong(val));
    }
    Py_RETURN_NONE;
}

/*
 * Given a numc.Matrix `self`, parse `args` to (int) row and (int) col.
 * Return the value at the `row`th row and `col`th column, which is a Python
 * float/int.
 */
PyObject *Matrix61c_get_value(Matrix61c *self, PyObject* args) {
    /* TODO: YOUR CODE HERE */

    PyObject *row, *col;

    if (!PyArg_UnpackTuple(args, "get", 2, 2, &row, &col)) {
        PyErr_SetString(PyExc_TypeError, "The number of arguments is not 2");
        return NULL;
    }

    if (!PyLong_Check(row) || !PyLong_Check(col)) {
        PyErr_SetString(PyExc_TypeError, "Row and column indices must be integers.");
        return NULL;
    }

    int i = PyLong_AsLong(row);
    int j = PyLong_AsLong(col);

    double value = get(self->mat, i, j);
    return PyFloat_FromDouble(value);
}

/*
 * Create an array of PyMethodDef structs to hold the instance methods.
 * Name the python function corresponding to Matrix61c_get_value as "get" and Matrix61c_set_value
 * as "set"
 * You might find this link helpful: https://docs.python.org/3.6/c-api/structures.html
 */
PyMethodDef Matrix61c_methods[] = {
    /* TODO: YOUR CODE HERE */
    {"get", (PyCFunction)Matrix61c_get_value, METH_VARARGS, "Get the value at the specified row and column."},
    {"set", (PyCFunction)Matrix61c_set_value, METH_VARARGS, "Set the value at the specified row and column."},
    {NULL, NULL, 0, NULL}  /* Sentinel */
};

/* INDEXING */

/*
 * Given a numc.Matrix `self`, index into it with `key`. Return the indexed result.
 */
PyObject *Matrix61c_subscript(Matrix61c* self, PyObject* key) {
    /* TODO: YOUR CODE HERE */

    // key maybe an integer, a slice, or a tuple of two integers, a tuple of two slices, or a tuple of an integer and a slice (in either order)

    // self->mat maybe be 1D or 2D 

    if (self->mat->is_1d) {
        // 1D matrix 
        
        if (PyLong_Check(key)) {
            // key is a integer
            int index = PyLong_AsLong(key);

            // return a value 
            if (self->mat->rows == 1) {
                // 1D row vector 
                if (index < 0 || index >= self->mat->cols) {
                    PyErr_SetString(PyExc_IndexError, "Index out of range for 1D row vector.");
                    return NULL;
                }
                return PyFloat_FromDouble(get(self->mat, 0, index));
            } else {
                // 1D column vector 
                if (index < 0 || index >= self->mat->rows) {
                    PyErr_SetString(PyExc_IndexError, "Index out of range for 1D column vector.");
                    return NULL;
                }
                return PyFloat_FromDouble(get(self->mat, index, 0));
            }
        }

        else if (PySlice_Check(key)) {
            // key is a slice 

            if (self->mat->rows == 1) {
                // 1D row vector
                Py_ssize_t start, stop, step, slicelength;
                if (PySlice_GetIndicesEx(key, self->mat->cols, &start, &stop, &step, &slicelength) < 0) {
                    PyErr_SetString(PyExc_ValueError, "Invalid slice for 1D row vector.");
                    return NULL; // Error in slice indices
                }

                if (slicelength < 1 || step != 1) {
                    PyErr_SetString(PyExc_ValueError, "Invalid slice for 1D row vector.");
                    return NULL;
                }

                if (start < 0 || start >= self->mat->cols || stop < 0 || stop > self->mat->cols) {
                    PyErr_SetString(PyExc_IndexError, "Slice indices out of range for 1D row vector.");
                    return NULL;
                }

                if (slicelength == 1) {
                    return PyFloat_FromDouble(get(self->mat, 0, start));
                }

                // Create a new Matrix61c object for the result from self->mat
                Matrix61c* result = (Matrix61c*)Matrix61c_new(&Matrix61cType, NULL, NULL);
                allocate_matrix_ref(&(result->mat),self->mat, 0, start, 1, slicelength);

                result->shape = get_shape(1, slicelength);
                return (PyObject*)result;
            } else {
                // 1D column vector
                Py_ssize_t start, stop, step, slicelength;
                if (PySlice_GetIndicesEx(key, self->mat->rows, &start, &stop, &step, &slicelength) < 0) {
                    PyErr_SetString(PyExc_ValueError, "Invalid slice for 1D column vector.");
                    return NULL; // Error in slice indices
                }

                if (slicelength < 1 || step != 1) {
                    PyErr_SetString(PyExc_ValueError, "Invalid slice for 1D column vector.");
                    return NULL;
                }

                if (start < 0 || start >= self->mat->rows || stop < 0 || stop > self->mat->rows) {
                    PyErr_SetString(PyExc_IndexError, "Slice indices out of range for 1D column vector.");
                    return NULL;
                }

                // if the size is 1x1, just return the value
                if (slicelength == 1) {
                    return PyFloat_FromDouble(get(self->mat, start, 0));
                }

                // Create a new Matrix61c object for the result
                Matrix61c* result = (Matrix61c*)Matrix61c_new(&Matrix61cType, NULL, NULL);
                allocate_matrix_ref(&(result->mat),self->mat, start, 0, slicelength, 1);

                result->shape = get_shape(slicelength, 1);
                return (PyObject*)result;
            }

        }

        else {
            PyErr_SetString(PyExc_TypeError, "Invalid key type for 1D matrix.");
            return NULL;
        }
    } else {
        // 2D matrix

        if (PyLong_Check(key)) {
            // key is a integer 

            int index = PyLong_AsLong(key);

            if (index < 0 || index >= self->mat->rows) {
                PyErr_SetString(PyExc_IndexError, "Index out of range for 2D matrix.");
                return NULL;
            }

            // return a row vector 
            Matrix61c* result = (Matrix61c*)Matrix61c_new(&Matrix61cType, NULL, NULL);

            allocate_matrix_ref(&(result->mat),self->mat, index, 0, 1, self->mat->cols);

            result->shape = get_shape(1, self->mat->cols);
            return (PyObject*)result;
        } 
        else if (PySlice_Check(key)) {
            // key is a slice 

            Py_ssize_t start, stop, step, slicelength;
            if (PySlice_GetIndicesEx(key, self->mat->rows, &start, &stop, &step, &slicelength) < 0) {
                PyErr_SetString(PyExc_ValueError, "Invalid slice for 2D matrix.");
                return NULL; // Error in slice indices
            }

            if (slicelength < 1 || step != 1) {
                PyErr_SetString(PyExc_ValueError, "Invalid slice for 2D matrix.");
                return NULL;
            }

            if (start < 0 || start >= self->mat->rows || stop < 0 || stop > self->mat->rows) {
                PyErr_SetString(PyExc_IndexError, "Slice indices out of range for 2D matrix.");
                return NULL;
            }


            // Create a new Matrix61c object for the result from self->mat
            Matrix61c* result = (Matrix61c*)Matrix61c_new(&Matrix61cType, NULL, NULL);
            allocate_matrix_ref(&(result->mat),self->mat, start, 0, slicelength, self->mat->cols);


            result->shape = get_shape(slicelength, self->mat->cols);
            return (PyObject*)result;
        }
        else if (PyTuple_Check(key)) {
            // key is a tuple of two integers or slices or an integer and a slice (in either order)
            if (PyTuple_Size(key) != 2) {
                PyErr_SetString(PyExc_TypeError, "Tuple key must have exactly two elements.");
                return NULL;
            }

            PyObject *first = PyTuple_GetItem(key, 0);
            PyObject *second = PyTuple_GetItem(key, 1);

            // Handle the case where both are integers
            if (PyLong_Check(first) && PyLong_Check(second)) {
                int row = PyLong_AsLong(first);
                int col = PyLong_AsLong(second);
                
                if (row < 0 || row >= self->mat->rows || col < 0 || col >= self->mat->cols) {
                    PyErr_SetString(PyExc_IndexError, "Index out of range for 2D matrix.");
                    return NULL;
                }

                return PyFloat_FromDouble(get(self->mat, row, col));
            }

            // Handle the case where both are slices
            else if (PySlice_Check(first) && PySlice_Check(second)) {
                Py_ssize_t row_start, row_stop, row_step, row_length;
                Py_ssize_t col_start, col_stop, col_step, col_length;

                if (PySlice_GetIndicesEx(first, self->mat->rows, &row_start, &row_stop, &row_step, &row_length) < 0 ||
                    PySlice_GetIndicesEx(second, self->mat->cols, &col_start, &col_stop, &col_step, &col_length) < 0) {
                    PyErr_SetString(PyExc_ValueError, "Invalid slice for 2D matrix.");
                    return NULL; // Error in slice indices
                }

                if (row_length < 1 || row_step != 1 || col_length < 1 || col_step != 1) {
                    PyErr_SetString(PyExc_ValueError, "Invalid slice for 2D matrix.");
                    return NULL;
                }

                if (row_start < 0 || row_start >= self->mat->rows || row_stop < 0 || row_stop > self->mat->rows ||
                    col_start < 0 || col_start >= self->mat->cols || col_stop < 0 || col_stop > self->mat->cols) {
                    PyErr_SetString(PyExc_IndexError, "Slice indices out of range for 2D matrix.");
                    return NULL;
                }

                // if the size if 1x1, just return the value 
                if (row_length == 1 && col_length == 1) {
                    return PyFloat_FromDouble(get(self->mat, row_start, col_start));
                }

                // Create a new Matrix61c object for the result from self->mat
                Matrix61c* result = (Matrix61c*)Matrix61c_new(&Matrix61cType, NULL, NULL);
                allocate_matrix_ref(&(result->mat),self->mat, row_start, col_start, row_length, col_length);

                result->shape = get_shape(row_length, col_length);
                return (PyObject*)result;

            } 
            else if (PySlice_Check(first) && PyLong_Check(second)) {
                // first is a slice, second is an integer
                Py_ssize_t row_start, row_stop, row_step, row_length;
                int col = PyLong_AsLong(second);

                if (PySlice_GetIndicesEx(first, self->mat->rows, &row_start, &row_stop, &row_step, &row_length) < 0) {
                    PyErr_SetString(PyExc_ValueError, "Invalid slice for 2D matrix.");
                    return NULL; // Error in slice indices
                }

                if (row_length < 1 || row_step != 1) {
                    PyErr_SetString(PyExc_ValueError, "Invalid slice for 2D matrix.");
                    return NULL;
                }

                if (row_start < 0 || row_start >= self->mat->rows || row_stop < 0 || row_stop > self->mat->rows) {
                    PyErr_SetString(PyExc_IndexError, "Slice indices out of range for 2D matrix.");
                    return NULL;
                }

                if (col < 0 || col >= self->mat->cols) {
                    PyErr_SetString(PyExc_IndexError, "Index out of range for 2D matrix.");
                    return NULL;
                }
                
                if (row_length == 1) {
                    return PyFloat_FromDouble(get(self->mat, row_start, PyLong_AsLong(second)));
                }


                // Create a new Matrix61c object for the result from self->mat
                Matrix61c* result = (Matrix61c*)Matrix61c_new(&Matrix61cType, NULL, NULL);
                allocate_matrix_ref(&(result->mat),self->mat, row_start, col, row_length, 1);

                result->shape = get_shape(row_length, 1);
                return (PyObject*)result;

            } 
            else if (PyLong_Check(first) && PySlice_Check(second)) {
                // first is an integer, second is a slice
                int row = PyLong_AsLong(first);
                Py_ssize_t col_start, col_stop, col_step, col_length;
                if (PySlice_GetIndicesEx(second, self->mat->cols, &col_start, &col_stop, &col_step, &col_length) < 0) {
                    PyErr_SetString(PyExc_ValueError, "Invalid slice for 2D matrix.");
                    return NULL; // Error in slice indices
                }

                if (col_length < 1 || col_step != 1) {
                    PyErr_SetString(PyExc_ValueError, "Invalid slice for 2D matrix.");
                    return NULL;
                }

                if (row < 0 || row >= self->mat->rows) {
                    PyErr_SetString(PyExc_IndexError, "Index out of range for 2D matrix.");
                    return NULL;
                }

                if (col_start < 0 || col_start >= self->mat->cols || col_stop < 0 || col_stop > self->mat->cols) {
                    PyErr_SetString(PyExc_IndexError, "Slice indices out of range for 2D matrix.");
                    return NULL;
                }

                if (col_length == 1) {
                    return PyFloat_FromDouble(get(self->mat, row, col_start));
                }

                // Create a new Matrix61c object for the result from self->mat
                Matrix61c* result = (Matrix61c*)Matrix61c_new(&Matrix61cType, NULL, NULL);
                allocate_matrix_ref(&(result->mat),self->mat, row, col_start, 1, col_length);

                result->shape = get_shape(1, col_length);
                return (PyObject*)result;
            }
            
            else {
                PyErr_SetString(PyExc_TypeError, "Invalid key for 2D matrix.");
                return NULL;
            }
        }
        else {
            PyErr_SetString(PyExc_TypeError, "Invalid key type for 2D matrix.");
            return NULL;
        }
    }
}

/*
 * Given a numc.Matrix `self`, index into it with `key`, and set the indexed result to `v`.
 */
int Matrix61c_set_subscript(Matrix61c* self, PyObject *key, PyObject *v) {
    /* TODO: YOUR CODE HERE */

    if (self->mat->is_1d) {
        // 1D matrix 

        if (PyLong_Check(key)) {
            // key is a integer 

            int index = PyLong_AsLong(key);

        

            // if v is not a float or int, throw TypeError 
            if (!PyFloat_Check(v) && !PyLong_Check(v)) {
                PyErr_SetString(PyExc_TypeError, "Value must be a float or int.");
                return -1;
            }

            double value;

            if (PyFloat_Check(v)) {
                value = PyFloat_AsDouble(v);
            } else {
                value = PyLong_AsLong(v);
            }

            if (self->mat->rows == 1) {
                // 1D row vector 
                if (index < 0 || index >= self->mat->cols) {
                    PyErr_SetString(PyExc_IndexError, "Index out of range for 1D row vector.");
                    return -1;
                }
                set(self->mat, 0, index, value);
            }  else {
                // 1D column vector 
                if (index < 0 || index >= self->mat->rows) {
                    PyErr_SetString(PyExc_IndexError, "Index out of range for 1D column vector.");
                    return -1;
                }
                set(self->mat, index, 0, value);
            }

            return 0;
        } 
        else if (PySlice_Check(key)) {
            // key is a slice 

            Py_ssize_t start, stop, step, slicelength;

            if (self->mat->rows == 1) {
                // 1D row vector 
                if (PySlice_GetIndicesEx(key, self->mat->cols, &start, &stop, &step, &slicelength) < 0) {
                    PyErr_SetString(PyExc_ValueError, "Invalid slice for 1D row vector.");
                    return -1; // Error in slice indices
                }

                if (slicelength < 1 || step != 1) {
                    PyErr_SetString(PyExc_ValueError, "Invalid slice for 1D row vector.");
                    return -1;
                }

                if (start < 0 || start >= self->mat->cols || stop < 0 || stop > self->mat->cols) {
                    PyErr_SetString(PyExc_IndexError, "Slice indices out of range for 1D row vector.");
                    return -1;
                }

                if (!PyList_Check(v)) {
                    PyErr_SetString(PyExc_TypeError, "Value must be a list.");
                    return -1;
                }

                if (PyList_Size(v) != slicelength) {
                    PyErr_SetString(PyExc_ValueError, "Value must be a list of the correct length.");
                    return -1;
                }

                for (int i = 0; i < slicelength; i++) {
                    PyObject *item = PyList_GetItem(v, i);
                    double value;
                    if (PyFloat_Check(item)) {
                        value = PyFloat_AsDouble(item);
                    } else if (PyLong_Check(item)) {
                        value = PyLong_AsLong(item);
                    } else {
                        PyErr_SetString(PyExc_ValueError, "List items must be float or int.");
                        return -1;
                    }
                    set(self->mat, 0, start + i, value);
                }

                return 0;
            } else {
                // 1D column vector 
                if (PySlice_GetIndicesEx(key, self->mat->rows, &start, &stop, &step, &slicelength) < 0) {
                    PyErr_SetString(PyExc_ValueError, "Invalid slice for 1D column vector.");
                    return -1; // Error in slice indices
                }

                if (slicelength < 1 || step != 1) {
                    PyErr_SetString(PyExc_ValueError, "Invalid slice for 1D column vector.");
                    return -1;
                }

                if (start < 0 || start >= self->mat->rows || stop < 0 || stop > self->mat->rows) {
                    PyErr_SetString(PyExc_IndexError, "Slice indices out of range for 1D column vector.");
                    return -1;
                }

                if (!PyList_Check(v)) {
                    PyErr_SetString(PyExc_TypeError, "Value must be a list of the correct length.");
                    return -1;
                }

                if (PyList_Size(v) != slicelength) {
                    PyErr_SetString(PyExc_ValueError, "Value must be a list of the correct length.");
                    return -1;
                }

                for (int i = 0; i < slicelength; i++) {
                    PyObject *item = PyList_GetItem(v, i);
                    double value;
                    if (PyFloat_Check(item)) {
                        value = PyFloat_AsDouble(item);
                    } else if (PyLong_Check(item)) {
                        value = PyLong_AsLong(item);
                    } else {
                        PyErr_SetString(PyExc_ValueError, "List items must be float or int.");
                        return -1;
                    }
                    set(self->mat, start + i, 0, value);
                }

                return 0;
            }
        }
           
        else {
            PyErr_SetString(PyExc_TypeError, "Invalid key type for 1D matrix.");
            return -1;
        }
    } else {
        // 2D matrix 

        if (PyLong_Check(key)) {
            // key is a integer 

            int index = PyLong_AsLong(key);

            if (index < 0 || index >= self->mat->rows) {
                PyErr_SetString(PyExc_IndexError, "Index out of range for 2D matrix.");
                return -1;
            }

            if (!PyList_Check(v)) {
                PyErr_SetString(PyExc_TypeError, "Value must be a list.");
                return -1;
            }

            if (PyList_Size(v) != self->mat->cols) {
                PyErr_SetString(PyExc_ValueError, "Value must be a list of the correct length.");
                return -1;
            }

            for (int j = 0; j < self->mat->cols; j++) {
                PyObject *item = PyList_GetItem(v, j);
                double value;
                if (PyFloat_Check(item)) {
                    value = PyFloat_AsDouble(item);
                } else if (PyLong_Check(item)) {
                    value = PyLong_AsLong(item);
                } else {
                    PyErr_SetString(PyExc_ValueError, "List items must be float or int.");
                    return -1;
                }
                set(self->mat, index, j, value);
            }
            return 0;

        } 
        else if (PySlice_Check(key)) {
            // key is a slice 

            Py_ssize_t start, stop, step, slicelength;

            if (PySlice_GetIndicesEx(key, self->mat->rows, &start, &stop, &step, &slicelength) < 0) {
                PyErr_SetString(PyExc_ValueError, "Invalid slice for 2D matrix.");
                return -1; // Error in slice indices
            }

            if (slicelength < 1 || step != 1) {
                PyErr_SetString(PyExc_ValueError, "Invalid slice for 2D matrix.");
                return -1;
            }

            if (start < 0 || start >= self->mat->rows || stop < 0 || stop > self->mat->rows) {
                PyErr_SetString(PyExc_IndexError, "Slice indices out of range for 2D matrix.");
                return -1;
            }

            if (!PyList_Check(v)) {
                PyErr_SetString(PyExc_TypeError, "Value must be a list of lists.");
                return -1;
            }

            if (PyList_Size(v) != slicelength) {
                PyErr_SetString(PyExc_ValueError, "Value must be a list of the correct length.");
                return -1;
            }

            for (int i = 0; i < slicelength; i++) {
                PyObject *row_list = PyList_GetItem(v, i);
                if (!PyList_Check(row_list)) {
                    PyErr_SetString(PyExc_TypeError, "Value must be a list of lists.");
                    return -1;
                }
                if (PyList_Size(row_list) != self->mat->cols) {
                    PyErr_SetString(PyExc_ValueError, "Each row must have the correct number of columns.");
                    return -1;
                }
                for (int j = 0; j < self->mat->cols; j++) {
                    PyObject *item = PyList_GetItem(row_list, j);
                    double value;
                    if (PyFloat_Check(item)) {
                        value = PyFloat_AsDouble(item);
                    } else if (PyLong_Check(item)) {
                        value = PyLong_AsLong(item);
                    } else {
                        PyErr_SetString(PyExc_ValueError, "List items must be float or int.");
                        return -1;
                    }
                    set(self->mat, start + i, j, value);
                }
            }
            return 0;
        } 
        else if (PyTuple_Check(key)) {
            // key is a tuple of two integers or slices or an integer and a slice (in either order)
            if (PyTuple_Size(key) != 2) {
                PyErr_SetString(PyExc_TypeError, "Tuple key must have exactly two elements.");
                return -1;
            }

            PyObject *first = PyTuple_GetItem(key, 0);
            PyObject *second = PyTuple_GetItem(key, 1);

            // Handle the case where both are integers
            if (PyLong_Check(first) && PyLong_Check(second)) {
                int row = PyLong_AsLong(first);
                int col = PyLong_AsLong(second);

                if (row < 0 || row >= self->mat->rows || col < 0 || col >= self->mat->cols) {
                    PyErr_SetString(PyExc_IndexError, "Index out of range for 2D matrix.");
                    return -1;
                }

                if (!PyFloat_Check(v) && !PyLong_Check(v)) {
                    PyErr_SetString(PyExc_TypeError, "Value must be a float or int.");
                    return -1;
                }

                double value;
                if (PyFloat_Check(v)) {
                    value = PyFloat_AsDouble(v);
                } else {
                    value = PyLong_AsLong(v);
                }

                set(self->mat, row, col, value);
                return 0;
            } 
            else if (PySlice_Check(first) && PySlice_Check(second)) {

                Py_ssize_t row_start, row_stop, row_step, row_length;
                Py_ssize_t col_start, col_stop, col_step, col_length;

                if (PySlice_GetIndicesEx(first, self->mat->rows, &row_start, &row_stop, &row_step, &row_length) < 0 ||
                    PySlice_GetIndicesEx(second, self->mat->cols, &col_start, &col_stop, &col_step, &col_length) < 0) {
                    PyErr_SetString(PyExc_ValueError, "Invalid slice for 2D matrix.");
                    return -1; // Error in slice indices
                }

                if (row_length < 1 || row_step != 1 || col_length < 1 || col_step != 1) {
                    PyErr_SetString(PyExc_ValueError, "Invalid slice for 2D matrix.");
                    return -1;
                }

                if (row_start < 0 || row_start >= self->mat->rows || row_stop < 0 || row_stop > self->mat->rows ||
                    col_start < 0 || col_start >= self->mat->cols || col_stop < 0 || col_stop > self->mat->cols) {
                    PyErr_SetString(PyExc_IndexError, "Slice indices out of range for 2D matrix.");
                    return -1;
                }

                // if the size is 1x1, v is a value 
                if (row_length == 1 && col_length == 1) {
                    if (!PyFloat_Check(v) && !PyLong_Check(v)) {
                        PyErr_SetString(PyExc_TypeError, "Value must be a float or int.");
                        return -1;
                    }

                    double value;
                    if (PyFloat_Check(v)) {
                        value = PyFloat_AsDouble(v);
                    } else {
                        value = PyLong_AsLong(v);
                    }

                    set(self->mat, row_start, col_start, value);
                    return 0;
                }

                if (row_length == 1) {
                    // a row vector 
                    if (!PyList_Check(v)) {
                        PyErr_SetString(PyExc_TypeError, "Value must be a list.");
                        return -1;
                    }

                    if (PyList_Size(v) != col_length) {
                        PyErr_SetString(PyExc_ValueError, "Value must be a list of the correct length.");
                        return -1;
                    }

                    for (int j = 0; j < col_length; j++) {
                        PyObject *item = PyList_GetItem(v, j);
                        double value;
                        if (PyFloat_Check(item)) {
                            value = PyFloat_AsDouble(item);
                        } else if (PyLong_Check(item)) {
                            value = PyLong_AsLong(item);
                        } else {
                            PyErr_SetString(PyExc_ValueError, "List items must be float or int.");
                            return -1;
                        }
                        set(self->mat, row_start, col_start + j, value);
                    }
                    return 0;
                }

                if (col_length == 1) {
                    // a column vector 
                    if (!PyList_Check(v)) {
                        PyErr_SetString(PyExc_TypeError, "Value must be a list.");
                        return -1;
                    }

                    if (PyList_Size(v) != row_length) {
                        PyErr_SetString(PyExc_ValueError, "Value must be a list of the correct length.");
                        return -1;
                    }

                    for (int i = 0; i < row_length; i++) {
                        PyObject *item = PyList_GetItem(v, i);
                        double value;
                        if (PyFloat_Check(item)) {
                            value = PyFloat_AsDouble(item);
                        } else if (PyLong_Check(item)) {
                            value = PyLong_AsLong(item);
                        } else {
                            PyErr_SetString(PyExc_ValueError, "List items must be float or int.");
                            return -1;
                        }
                        set(self->mat, row_start + i, col_start, value);
                    }
                    return 0;
                }

                // if both row_length and col_length are greater than 1, v must be a list of lists
                if (!PyList_Check(v)) {
                    PyErr_SetString(PyExc_TypeError, "Value must be a list of lists.");
                    return -1;
                }

                if (PyList_Size(v) != row_length) {
                    PyErr_SetString(PyExc_ValueError, "Value must be a list of the correct length.");
                    return -1;
                }

                for (int i = 0; i < row_length; i++) {
                    PyObject *row_list = PyList_GetItem(v, i);
                    if (!PyList_Check(row_list)) {
                        PyErr_SetString(PyExc_TypeError, "Value must be a list of lists.");
                        return -1;
                    }
                    if (PyList_Size(row_list) != col_length) {
                        PyErr_SetString(PyExc_ValueError, "Each row must have the correct number of columns.");
                        return -1;
                    }
                    for (int j = 0; j < col_length; j++) {
                        PyObject *item = PyList_GetItem(row_list, j);
                        double value;
                        if (PyFloat_Check(item)) {
                            value = PyFloat_AsDouble(item);
                        } else if (PyLong_Check(item)) {
                            value = PyLong_AsLong(item);
                        } else {
                            PyErr_SetString(PyExc_ValueError, "List items must be float or int.");
                            return -1;
                        }
                        set(self->mat, row_start + i, col_start + j, value);
                    }
                }
                return 0;
            }
            else if (PySlice_Check(first) && PyLong_Check(second)) {
                // first is a slice, second is an integer
                Py_ssize_t row_start, row_stop, row_step, row_length;
                if (PySlice_GetIndicesEx(first, self->mat->rows, &row_start, &row_stop, &row_step, &row_length) < 0) {
                    PyErr_SetString(PyExc_ValueError, "Invalid slice for 2D matrix.");
                    return -1; // Error in slice indices
                }

                if (row_length < 1 || row_step != 1) {
                    PyErr_SetString(PyExc_ValueError, "Invalid slice for 2D matrix.");
                    return -1;
                }

                int col = PyLong_AsLong(second);

                if (row_start < 0 || row_start >= self->mat->rows || row_stop < 0 || row_stop > self->mat->rows) {
                    PyErr_SetString(PyExc_IndexError, "Slice indices out of range for 2D matrix.");
                    return -1;
                }

                if (col < 0 || col >= self->mat->cols) {
                    PyErr_SetString(PyExc_IndexError, "Index out of range for 2D matrix.");
                    return -1;
                }

                if (row_length == 1) {
                    if (!PyFloat_Check(v) && !PyLong_Check(v)) {
                        PyErr_SetString(PyExc_TypeError, "Value must be a float or int.");
                        return -1;
                    }

                    double value;
                    if (PyFloat_Check(v)) {
                        value = PyFloat_AsDouble(v);
                    } else {
                        value = PyLong_AsLong(v);
                    }

                    set(self->mat, row_start, col, value);
                    return 0;
                }

                if (!PyList_Check(v)) {
                    PyErr_SetString(PyExc_TypeError, "Value must be a list.");
                    return -1;
                }

                if (PyList_Size(v) != row_length) {
                    PyErr_SetString(PyExc_ValueError, "Value must be a list of the correct length.");
                    return -1;
                }

                for (int i = 0; i < row_length; i++) {
                    PyObject *item = PyList_GetItem(v, i);
                    double value;
                    if (PyFloat_Check(item)) {
                        value = PyFloat_AsDouble(item);
                    } else if (PyLong_Check(item)) {
                        value = PyLong_AsLong(item);
                    } else {
                        PyErr_SetString(PyExc_ValueError, "List items must be float or int.");
                        return -1;
                    }
                    set(self->mat, row_start + i, col, value);
                }
                return 0;
            } 
            else if (PyLong_Check(first) && PySlice_Check(second)) {
                // first is an integer, second is a slice
                int row = PyLong_AsLong(first);

                Py_ssize_t col_start, col_stop, col_step, col_length;
                if (PySlice_GetIndicesEx(second, self->mat->cols, &col_start, &col_stop, &col_step, &col_length) < 0) {
                    PyErr_SetString(PyExc_ValueError, "Invalid slice for 2D matrix.");
                    return -1; // Error in slice indices
                }

                if (col_length < 1 || col_step != 1) {
                    PyErr_SetString(PyExc_ValueError, "Invalid slice for 2D matrix.");
                    return -1;
                }

                if (row < 0 || row >= self->mat->rows) {
                    PyErr_SetString(PyExc_IndexError, "Index out of range for 2D matrix.");
                    return -1;
                }

                if (col_start < 0 || col_start >= self->mat->cols || col_stop < 0 || col_stop > self->mat->cols) { 
                    PyErr_SetString(PyExc_IndexError, "Slice indices out of range for 2D matrix.");
                    return -1;
                }

                if (col_length == 1) {
                    if (!PyFloat_Check(v) && !PyLong_Check(v)) {
                        PyErr_SetString(PyExc_TypeError, "Value must be a float or int.");
                        return -1;
                    }

                    double value;
                    if (PyFloat_Check(v)) {
                        value = PyFloat_AsDouble(v);
                    } else {
                        value = PyLong_AsLong(v);
                    }

                    set(self->mat, row, col_start, value);
                    return 0;
                }

                if (!PyList_Check(v)) {
                    PyErr_SetString(PyExc_TypeError, "Value must be a list.");
                    return -1;
                }

                if (PyList_Size(v) != col_length) {
                    PyErr_SetString(PyExc_ValueError, "Value must be a list of the correct length.");
                    return -1;
                }

                for (int j = 0; j < col_length; j++) {
                    PyObject *item = PyList_GetItem(v, j);
                    double value;
                    if (PyFloat_Check(item)) {
                        value = PyFloat_AsDouble(item);
                    } else if (PyLong_Check(item)) {
                        value = PyLong_AsLong(item);
                    } else {
                        PyErr_SetString(PyExc_ValueError, "List items must be float or int.");
                        return -1;
                    }
                    set(self->mat, row, col_start + j, value);
                }
                return 0;
            }
            else {
                PyErr_SetString(PyExc_TypeError, "Invalid key type for 2D matrix.");
                return -1;
            }

        } else {
            PyErr_SetString(PyExc_TypeError, "Invalid key type for 2D matrix.");
            return -1;
        }

    }
}


PyMappingMethods Matrix61c_mapping = {
    NULL,
    (binaryfunc) Matrix61c_subscript,
    (objobjargproc) Matrix61c_set_subscript,
};

/* INSTANCE ATTRIBUTES*/
PyMemberDef Matrix61c_members[] = {
    {
        "shape", T_OBJECT_EX, offsetof(Matrix61c, shape), 0,
        "(rows, cols)"
    },
    {NULL}  /* Sentinel */
};

PyTypeObject Matrix61cType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "numc.Matrix",
    .tp_basicsize = sizeof(Matrix61c),
    .tp_dealloc = (destructor)Matrix61c_dealloc,
    .tp_repr = (reprfunc)Matrix61c_repr,
    .tp_as_number = &Matrix61c_as_number,
    .tp_flags = Py_TPFLAGS_DEFAULT |
    Py_TPFLAGS_BASETYPE,
    .tp_doc = "numc.Matrix objects",
    .tp_methods = Matrix61c_methods,
    .tp_members = Matrix61c_members,
    .tp_as_mapping = &Matrix61c_mapping,
    .tp_init = (initproc)Matrix61c_init,
    .tp_new = Matrix61c_new
};


struct PyModuleDef numcmodule = {
    PyModuleDef_HEAD_INIT,
    "numc",
    "Numc matrix operations",
    -1,
    Matrix61c_class_methods
};

/* Initialize the numc module */
PyMODINIT_FUNC PyInit_numc(void) {
    PyObject* m;

    if (PyType_Ready(&Matrix61cType) < 0)
        return NULL;

    m = PyModule_Create(&numcmodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&Matrix61cType);
    PyModule_AddObject(m, "Matrix", (PyObject *)&Matrix61cType);
    printf("CS61C Fall 2020 Project 4: numc imported!\n");
    fflush(stdout);
    return m;
}