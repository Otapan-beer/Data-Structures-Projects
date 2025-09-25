#include "Matrix.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <cassert>
#include <stdexcept>

Matrix::Matrix() : rows(0), cols(0), numbers(nullptr) {}
Matrix::Matrix(unsigned int crows, unsigned int ccols, double value) {
    rows = crows;
    cols = ccols;

    //alocating memory - I admit, I had to use AI to help me here with setting up the pointers as I still do not really understand the syntax
    numbers = new double*[rows];
    for (unsigned int i = 0; i < rows; i++) { //I knew I have to do two for loops (row and col)
        numbers[i] = new double[cols];  //I kept putting a star here becaause I though it is a "double pointer" and not a "single" one
        for (unsigned int j = 0; j < cols; j++) {
            numbers[i][j] = value;  //set value to the row/col
        }
    }
}

//copy constructor - AI also helped with this one for the similar reason
Matrix::Matrix(const Matrix& other) : rows(other.rows), cols(other.cols) {
    numbers = new double*[rows];
    for (unsigned int i = 0; i < rows; i++) {
        numbers[i] = new double[cols];
        for (unsigned int j = 0; j < cols; j++) {
            numbers[i][j] = other.numbers[i][j];
        }
    }
}

//destructor - after changing the star in cnstructor I had to change the destructor to just one for loop in rows
Matrix::~Matrix() {
    for (unsigned int i = 0; i < rows; i++) {
        delete[] numbers[i];
    }
    delete[] numbers;
}

//getters 

//rows
unsigned int Matrix::num_rows() const{
    return rows;
}

//cols
unsigned int Matrix::num_cols() const{
    return cols;
}

//get value at (row, col)
bool Matrix::get(unsigned int row, unsigned int col, double& value) const{
    if (row >= rows || col >= cols) {
        return false;
    }
    value = numbers[row][col];
    return true;
}



//setters

//set value at (row, col)
bool Matrix::set(unsigned int row, unsigned int col, double value) {
    if (row >= rows || col >= cols) {
        return false;
    }
    numbers[row][col] = value;
    return true;
}


//functions

//get_row
double* Matrix::get_row(unsigned int row) const {
    if (row >= rows) {
        return nullptr;
    }
    double* row_copy = new double[cols];
    for (unsigned int j = 0; j < cols; j++) {
        row_copy[j] = numbers[row][j];
    }
    return row_copy;
}

//get_col
double* Matrix::get_col(unsigned int col) const {
    if (col >= cols) {
        return nullptr;
    }
    double* col_copy = new double[rows];
    for (unsigned int i = 0; i < rows; i++) {
        col_copy[i] = numbers[i][col];
    }
    return col_copy;
}


//multiply by a coefficient
void Matrix::multiply_by_coefficient(double coefficient) {
    for (unsigned int i = 0; i < rows; i++) {
        for (unsigned int j = 0; j < cols; j++) {
            numbers[i][j] *= coefficient;
        }
    }
}

//swap two rows
void Matrix::swap_row(unsigned int first, unsigned int second) {
    if (first >= rows || second >= rows) {
        return;
    }
    std::swap(numbers[first], numbers[second]);
}

//add another matrix to this matrix
bool Matrix::add(const Matrix& other) {
    if (rows != other.rows || cols != other.cols) {
        return false;
    }
    for (unsigned int i = 0; i < rows; i++) {
        for (unsigned int j = 0; j < cols; j++) {
            numbers[i][j] += other.numbers[i][j];
        }
    }
    return true;
}


Matrix Matrix::transpose() const {
    // Create a new matrix with swapped rows and columns
    Matrix transposed(cols, rows, 0.0);
    
    for (unsigned int i = 0; i < rows; i++) {
        for (unsigned int j = 0; j < cols; j++) {
            transposed.numbers[j][i] = numbers[i][j]; //from what I undersatnd, you just swap the numbers
        }
    }
    
    return transposed;
}

//I am in a hurry so this is just mostly AI:
Matrix& Matrix::operator=(const Matrix& other) {
    if (this == &other) {
        return *this; // Self-assignment check
    }

    // Free existing memory
    for (unsigned int i = 0; i < rows; i++) {
        delete[] numbers[i];
    }
    delete[] numbers;

    // Copy new size
    rows = other.rows;
    cols = other.cols;

    // Allocate new memory and copy data
    numbers = new double*[rows];
    for (unsigned int i = 0; i < rows; i++) {
        numbers[i] = new double[cols];
        for (unsigned int j = 0; j < cols; j++) {
            numbers[i][j] = other.numbers[i][j];
        }
    }

    return *this;
}

Matrix Matrix::operator+(const Matrix& other) const {
    if (rows != other.rows || cols != other.cols) {
        throw std::invalid_argument("Matrix dimensions must match for addition.");
    }

    Matrix result(rows, cols, 0.0);
    for (unsigned int i = 0; i < rows; i++) {
        for (unsigned int j = 0; j < cols; j++) {
            result.numbers[i][j] = numbers[i][j] + other.numbers[i][j];
        }
    }
    return result;
}

Matrix Matrix::operator-(const Matrix& other) const {
    if (rows != other.rows || cols != other.cols) {
        throw std::invalid_argument("Matrix dimensions must match for subtraction.");
    }

    Matrix result(rows, cols, 0.0);
    for (unsigned int i = 0; i < rows; i++) {
        for (unsigned int j = 0; j < cols; j++) {
            result.numbers[i][j] = numbers[i][j] - other.numbers[i][j];
        }
    }
    return result;
}

Matrix Matrix::operator*(double scalar) const {
    Matrix result(rows, cols, 0.0);
    for (unsigned int i = 0; i < rows; i++) {
        for (unsigned int j = 0; j < cols; j++) {
            result.numbers[i][j] = numbers[i][j] * scalar;
        }
    }
    return result;
}

Matrix Matrix::operator*(const Matrix& other) const {
    if (cols != other.rows) {
        throw std::invalid_argument("Matrix dimensions must align for multiplication.");
    }

    Matrix result(rows, other.cols, 0.0);
    for (unsigned int i = 0; i < rows; i++) {
        for (unsigned int j = 0; j < other.cols; j++) {
            for (unsigned int k = 0; k < cols; k++) {
                result.numbers[i][j] += numbers[i][k] * other.numbers[k][j];
            }
        }
    }
    return result;
}

bool Matrix::operator==(const Matrix& other) const {
    if (rows != other.rows || cols != other.cols) {
        return false;
    }

    for (unsigned int i = 0; i < rows; i++) {
        for (unsigned int j = 0; j < cols; j++) {
            if (std::fabs(numbers[i][j] - other.numbers[i][j]) > __EPSILON) {
                return false;
            }
        }
    }
    return true;
}

bool Matrix::operator!=(const Matrix& other) const {
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& out, const Matrix& m) {
    for (int i = 0; i < m.rows; ++i) {
        for (int j = 0; j < m.cols; ++j) {
            out << m.numbers[i][j] << " ";
        }
        out << std::endl;
    }
    return out;
}

std::istream& operator>>(std::istream& in, Matrix& m) {
    for (unsigned int i = 0; i < m.rows; i++) {
        for (unsigned int j = 0; j < m.cols; j++) {
            in >> m.numbers[i][j];
        }
    }
    return in;
}

Matrix* Matrix::quarter() const {
    int subRows = (rows + 1) / 2;  //I was using % before... because I though it can not overlap
    int subCols = (cols + 1) / 2;

    Matrix* result = new Matrix[4]{
        Matrix(subRows, subCols), //UL
        Matrix(subRows, subCols), //UR
        Matrix(subRows, subCols), //LL
        Matrix(subRows, subCols)  //LR
    };

    for (int i = 0; i < subRows; ++i) {
        for (int j = 0; j < subCols; ++j) {
            double value; //create new double value each time

            // UL quadrant
            if (get(i, j, value)) {
                result[0].set(i, j, value);
            }
            
            // UR quadrant
            if (get(i, j + subCols - (cols % 2), value)) {
                result[1].set(i, j, value);
            }

            // LL quadrant
            if (get(i + subRows - (rows % 2), j, value)) {
                result[2].set(i, j, value);
            }

            // LR quadrant
            if (get(i + subRows - (rows % 2), j + subCols - (cols % 2), value)) {
                result[3].set(i, j, value);
            }
        }
    }
    return result;
}
