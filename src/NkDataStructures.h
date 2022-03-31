#ifndef NK_DATA_STRUCTURES_H
#define NK_DATA_STRUCTURES_H

#include <vector>

// Creates a dynamic 2D array using only one dimension for performance gains
template <class T>
class Matrix2D 
{
public:
    Matrix2D(int rows, int columns);
    Matrix2D();   // Default constructor initialises rows and columns to 0
    ~Matrix2D();

    // Get element at given row and column
    T GetElement(int row, int column);

    /* Insert element at given index. Will overwrite if an element already
    exists at this index */
    void InsertElement(T element, int row, int column);

    // Expand matrix by n rows
    void AddRows(int n);

    // Expand matrix by n columns
    void AddColumns(int n);

private:
    std::vector<T> elements;   // Uses vector for easier memory management

    int columns;
    int rows;

    // Get 1D index from 2D index
    int GetIndex(int row, int column);
};

#endif