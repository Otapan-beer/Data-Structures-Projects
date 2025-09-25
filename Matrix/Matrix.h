#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <fstream>
#include <cmath>
#include <cassert>

const double __EPSILON = 1e-9; //to define __Epsilon

class Matrix {
private:
//variables
    unsigned int rows, cols; //can't have them negative anyway
    double** numbers;


public:
//constructor
    //default
    Matrix();
        
    //initializing constructor
    Matrix(unsigned int rows, unsigned int cols, double value = 0.0);

    //copy constructor
    Matrix(const Matrix& other);

    //Destructor
    ~Matrix();

//getters
    bool get(unsigned int row, unsigned int col, double& value) const;
    unsigned int num_rows() const;
    unsigned int num_cols() const;

//setters
    bool set(unsigned int row, unsigned int col, double value);

//functions
    double* get_row(unsigned int row) const;
    double* get_col(unsigned int col) const; //yes, these two were defined as functions in the guide
    void multiply_by_coefficient(double coefficient);
    void swap_row(unsigned int first, unsigned int second);
    bool add(const Matrix& other);
    Matrix* quarter() const;
    Matrix transpose() const;


//add this line:

    //assignment operator
    Matrix& operator=(const Matrix& other); // Assignment operator

    //overload arithmetic operators
    Matrix operator+(const Matrix& other) const; // Matrix addition
    Matrix operator-(const Matrix& other) const; // Matrix subtraction
    Matrix operator*(double scalar) const;       // Scalar multiplication
    Matrix operator*(const Matrix& other) const; // Matrix multiplication

    //overload comparison operators
    bool operator==(const Matrix& other) const;
    bool operator!=(const Matrix& other) const;

    //overload stream operators
    friend std::ostream& operator<<(std::ostream& out, const Matrix& m); 
    //I have learned that by making it a friend function it can acces private stuff
    friend std::istream& operator>>(std::istream& in, Matrix& m);



};
#endif