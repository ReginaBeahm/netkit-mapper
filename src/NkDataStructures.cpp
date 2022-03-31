#include "NkDataStructures.h"
#include <stdexcept>

template <class T>
Matrix2D<T>::Matrix2D(int rows, int columns) : 
    columns(columns),
    rows(rows)
{
    elements.reserve(rows * columns);   // Reserve to avoid reallocs
}

template <class T>
Matrix2D<T>::Matrix2D() :
    columns(0),
    rows(0)
{

}

template <class T>
Matrix2D<T>::~Matrix2D() 
{

}

template<class T>
int Matrix2D<T>::GetIndex(int row, int column) 
{
    int index = column + row * columns;

    if (index >= rows * columns) 
    {
        throw std::out_of_range("Index was out of range!");  // Throw exception if out-of-range index is given
    }

    return (column + row * columns);
}

template <class T>
T Matrix2D<T>::GetElement(int row, int column) 
{
    return elements[GetIndex(row, column)];
}

template <class T>
void Matrix2D<T>::InsertElement(T element, int row, int column) 
{
    elements[GetIndex(row, column)] = element;
}

template <class T>
void Matrix2D<T>::AddRows(int n) 
{
    rows += n;
    elements.reserve(rows * columns);   // Expand memory allocation
}

template <class T>
void Matrix2D<T>::AddColumns(int n) 
{
    columns += n;
    elements.reserve(rows * columns);
}

template class Matrix2D<int>;
template class Matrix2D<float>;    // Force compilation for int and float types to avoid linking errors