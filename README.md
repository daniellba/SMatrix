# SMatrix
Choose between 3 different types of matrixs, insert numbers and do different operations.


<------------------- Header file ------------------->

/*Daniel Ben-Ami*/

#ifndef _SMATRIX
#define _SMATRIX
#include <string>
#include <iostream> 
using namespace std;

// A node of a sparse matrix.  
class MNode {
public:
	double _data;
	int _indexI, _indexJ; // the place of the node in the matrix 
						  // clarification: _nextRow is a pointer to the next columns in the row  
	MNode* _nextRow, *_nextCol;

	MNode(double data, int i, int j);
};


// A sparse matrix. a mathematical matrix object which is implemented
// in a way that saves memory if the matrix is sparse. The implementation
// consists of linked lists of elements which are not zero. 
class SMatrix {
private:
	MNode * * _rowHead, **_colHead; // _rowHead : array of head-pointers for the rows lists. 
	int _rowSize, _colSize; // _rowSize - number of rows
	int _elemNum; // number of entries not containing zero 
	string _matType;

	void setValue(int, int, double);
	void removeElement(int, int);
	void insertNode(MNode*);
	bool IsExist(int, int) const;

public:

	// construct a 'rows X cols' matrix. 
	SMatrix(int rows, int cols, string matType);

	SMatrix(SMatrix &other);

	// destroy this matrix. 
	~SMatrix();

	// set the (i,j) element to be 'data' 
	void setElement(int i, int j, double data);

	// return the (i,j) element    
	double getElement(int i, int j)const;

	// rowShift     
	void rowShift(const int shiftSize);

	// colShift     
	void colShift(const int shiftSize);

	// isA matType    
	bool isA(string matType);

	// SMatrix size in bytes
	void sizeInBytes();

	// print columns as linked lists
	void printColumnsIndexes();

	// print rows as linked lists
	void printRowsIndexes();

	// operator = for SMatrix
	SMatrix& operator=(SMatrix &other);

	// operator + for SMatrix
	SMatrix operator+(SMatrix &other);

	// operator << friend function for SMatrix
	friend ostream& operator<<(ostream& os, const SMatrix& mat);
};


#endif



<------------------- CPP file ------------------->

/*Daniel Ben-Ami*/

#include "SMatrix.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>;
using namespace std;

// MNode constructor
MNode::MNode(double data, int i, int j)
{
	_data = data;
	_indexI = i;
	_indexJ = j;
	_nextRow = NULL;
	_nextCol = NULL;
}

// SMatrix constructor
SMatrix::SMatrix(int rows, int cols, string type)
{
	if (type != "Arrowhead" && type != "Toeplitz" && type != "any") 
		//checks if the type is legal
	{
		cout << "NA" << endl;
		return;
	}
	if (rows < 0 || cols < 0)
	{
		cout << "NA" << endl;
		return;
	}
	_rowSize = rows;
	_colSize = cols;
	_elemNum = 0;
	_matType = type;
	_rowHead = new MNode*[rows];
	
	
	if (!_rowHead)
	{
		cout << "allocation error";
		exit(1);
	}
	_colHead = new MNode*[cols];
	if (!_colHead)
	{
		cout << "allocation error";
		exit(1);
	}
	for (int i = 0; i < rows; i++)
	{
		_rowHead[i] = NULL;
	}
	for (int i = 0; i < cols; i++)
	{
		_colHead[i] = NULL;
	}
}

// remove element with (i,j) index from row and column
// separate cases: first element in list or in the middle
void SMatrix::removeElement(int i, int j)
{
	MNode *prev=NULL;
	MNode *colPtr = _colHead[j];               // keep the column linked list
	MNode *rowPtr = _rowHead[i];               // keep the row linked lst

	if (_colHead[j]->_indexI == i)             // if element is the first in column
		_colHead[j] = _colHead[j]->_nextCol;

	else                                       // if element is not the first in column
	{
		while (colPtr->_indexI != i)           // find prev element on the column
		{
			prev = colPtr;
			colPtr = colPtr->_nextCol;
		}
		prev->_nextCol = colPtr->_nextCol;     // connect the prev with the next
	}

	if (_rowHead[i]->_indexJ == j)             // if element is the first in row
	{
		rowPtr = _rowHead[i];
		_rowHead[i] = _rowHead[i]->_nextRow;
	}

	else                                      // if element is not the first in row
	{
		while (rowPtr->_indexJ != j)          // find prev element on the row
		{
			prev = rowPtr;
			rowPtr = rowPtr->_nextRow;
		}
		prev->_nextRow = rowPtr->_nextRow;    // connect the prev with the next
	}

	delete rowPtr;                            // delete the element only once

}

//add new node to both row and column
// separate cases: list is empty or not empty
void SMatrix::insertNode(MNode* n)
{
	MNode* p = _rowHead[n->_indexI];  // the i row
	if (p != NULL)                    // there are elements in i row
	{
		if (p->_indexJ > n->_indexJ) // need to insert n at start, before existing elements
		{
			n->_nextRow = p;
			_rowHead[n->_indexI] = n;
		}
		else                          // need to inserst in the middle of the list or at end
		{
			while (p->_nextRow && p->_nextRow->_indexJ < n->_indexJ)
				p = p->_nextRow;
			n->_nextRow = p->_nextRow;
			p->_nextRow = n;
		}
	}
	else                              // row is empty
		_rowHead[n->_indexI] = n;

	p = _colHead[n->_indexJ];         // the j column
	if (p != NULL)                    // there are elements in j col
	{
		if (p->_indexI > n->_indexI)  // need to insert n at start, before existing elements
		{
			n->_nextCol = p;
			_colHead[n->_indexJ] = n;
		}
		else                          // need to inserst in the middle of the list or at end
		{
			while (p->_nextCol && p->_nextCol->_indexI < n->_indexI)
				p = p->_nextCol;
			n->_nextCol = p->_nextCol;
			p->_nextCol = n;
		}
	}
	else                            // column is empty
		_colHead[n->_indexJ] = n;
}

// set value to exist element (i,j)
void SMatrix::setValue(int i, int j, double data)
{
	MNode *ptr = _rowHead[i];

	while (ptr->_indexJ != j)          // find the element on the row
		ptr = ptr->_nextRow;
	ptr->_data = data;

}

// check if element (i,j) exists
bool SMatrix::IsExist(int i, int j) const
{
	MNode *ptr = _rowHead[i];

	if (ptr == NULL)
		return false;

	while (ptr && ptr->_indexJ < j)
		ptr = ptr->_nextRow;

	if (ptr && ptr->_indexJ == j)
		return true;

	return false;
}

// set the (i,j) element to be 'data' 
void SMatrix::setElement(int i, int j, double data)
{
	if (i<0 || i >= _rowSize || j<0 || j >= _colSize) {
		cout << "NA" << endl;
		return;
	}

	double temp = getElement(i, j);
	bool found = IsExist(i, j);
	if (data == 0 && found == true)
	{
		_elemNum--;
		removeElement(i, j);
		if (isA(this->_matType) == false)
		{
			setValue(i, j, temp);
			cout << "NA" << endl;
			return;
		}
	}
	if (data == 0 && found == false)
	{
		return;
	}
	if (data != 0 && found == true)
	{
		setValue(i, j, data);
		if (isA(this->_matType) == false)
		{
			setValue(i, j, temp);
			cout << "NA" << endl;
			return;
		}
	}
	if (data != 0 && found == false)
	{
		MNode *n = new MNode(data, i, j);
		_elemNum++;
		insertNode(n);

		if (isA(this->_matType) == false)
		{
			setValue(i, j, temp);
			cout << "NA" << endl;
			return;
		}
	}
}

// destroy this matrix. 
SMatrix::~SMatrix()
{
	if (_elemNum != 0)
	{
		//delete all nodes inside linked lists
		for (int i = 0; i < _rowSize; i++)
		{
			MNode* p = _rowHead[i], *p2;
			while (p)
			{
				p2 = p;
				p = p->_nextRow;
				delete p2;
			}

		}

	}
	//delete array of linked lists
	delete[] _colHead;
	delete[] _rowHead;
}

// print zero value 'num' times
void printZero(int num)
{
	for (int i = 0; i < num; i++)
		cout << "0,";

}

// print operator for SMatrix class
ostream& operator<<(ostream& os, const SMatrix& mat)
{
	for (int i = 0; i < mat._rowSize; i++)
	{
		for (int j = 0; j < mat._colSize; j++)
		{
			os << mat.getElement(i, j);
			if (j + 1 != mat._colSize)
				os << ",";
		}
		cout << endl;
	}
	return os;
}

//copy constractor 
SMatrix::SMatrix(SMatrix &other)
{
	this->_colSize = other._colSize;
	this->_rowSize = other._rowSize;
	this->_elemNum = 0;
	this->_matType = other._matType;

	_rowHead = new MNode *[_rowSize];
	if (!_rowHead)
	{
		cout << "allocation error";
		exit(1);
	}

	_colHead = new MNode*[_colSize];
	if (!_colHead)
	{
		cout << "allocation error";
		exit(1);
	}

	//inialize the matrix to nulls
	for (int i = 0; i < _rowSize; i++)
	{
		_rowHead[i] = NULL;
	}
	for (int i = 0; i < _colSize; i++)
	{
		_colHead[i] = NULL;
	}

	double temp;
	for (int i = 0; i < _rowSize; i++)
	{
		for (int j = 0; j < _colSize; j++)
		{
			temp = other.getElement(i, j);
			this->setElement(i, j, temp);
		}
	}
}

//gets a specific element with 2 indexs
double SMatrix::getElement(int i, int j) const 
{
	MNode *ptr = _rowHead[i];
	//in case the indexs out of bounds
	if (i<0 || i >= _rowSize || j<0 || j >= _colSize) {
		cout << "NA" << endl;
	}
	else
	{
		while (ptr != NULL && ptr->_indexJ < _colSize)
		{
			if (ptr->_indexJ==j)
			{
				return ptr->_data;
			}
			else
			{
				ptr = ptr->_nextRow;
			}
		}
		return 0;
	}
}

//switchs between rows
void SMatrix::rowShift(const int shiftSize)
{
	int counter = shiftSize;
	if (counter<0)//if it's negative
	{
		int temp;
		temp = counter * (-1);
		temp = counter % 10;
		temp = _rowSize - 1 - temp;
		counter = temp;
	}

	while (counter > 0)//the amount of the time I do the etartions
	{
		for (int i = 0; i < _rowSize; i++)
		{

			double ptrArr = getElement(i, 0); //holds cells data
			double ptr2Arr;				  //holds the next cell data
			for (int j = 1; j < _colSize; j++)
			{
				ptr2Arr = getElement(i,j); //switchs between the cells
				setElement(i, j, ptrArr);
				ptrArr = ptr2Arr;
			}
			setElement(i, 0, ptrArr); //inserts the data to the first cell
		}
		counter--;
	}

	if (isA(_matType) == false) //checks if the new matrix still stand the requiarments
	{
		int counter2 = _rowSize - shiftSize;
		rowShift(counter2); //in case it doesn't it makes the matrix back to it's original
	}
}

//switchs between cols
void SMatrix::colShift(const int shiftSize)
{
	int counter = shiftSize;
	if (counter<0)//if it's negative
	{
		int temp;
		temp = counter * (-1);
		temp = counter % 10;
		temp = _colSize - 1 - temp;
		counter = temp;
	}

	while (counter > 0)//the amount of the time I do the etartions
	{
		for (int j = 0; j < _colSize; j++)
		{

			double ptrArr = getElement(0, j); //holds cells data
			double ptr2Arr;			 //holds the next cell data
			for (int i = 1; i < _rowSize; i++)
			{
				ptr2Arr = getElement(i, j); //switchs between the cells
				setElement(i, j, ptrArr);
				ptrArr = ptr2Arr;
			}
			setElement(0, j, ptrArr); //inserts the data to the first cell
		}
		counter--;
	}

	if (isA(_matType) == false) //checks if the new matrix still stand the requiarments
	{
		int counter2 = _colSize - shiftSize;
		rowShift(counter2); //in case it doesn't it makes the matrix back to it's original
	}
}

//checks the type of the matrix 
bool SMatrix::isA(string matType)
{
	int i, j, k;
	double cellData;
	cout.setf(std::ios::boolalpha);

	if (matType == "Toeplitz")
	{
		//basicly here i save the cell data and compare it to the other 
		//cells data in the rows/cols.

		for (k = 0; k < _colSize; k++)
		{
			cellData = getElement(0, k);

			for (i = 0, j = 0 + k; i < _rowSize && j < _colSize; i++, j++)
			{
				if (cellData != getElement(i, j))
				{
					return false;
				}
			}
		}


		for (k = 0; k < _rowSize; k++)
		{
			cellData = getElement(k, 0);
			for (i = 0 + k, j = 0; i < _colSize && j < _rowSize; i++, j++)
			{
				if (cellData != getElement(i, j))
				{
					return false;
				}
			}
		}
		return true;

	}

	 else if (matType == "Arrowhead")
	 {
		 if (_rowSize == _colSize) //checks if it's a square matrix
		 {
			 for (i = 1; i < _rowSize; i++)
			 {
				 for (j = 1; j < _colSize; j++)
				 {
					 if (getElement(i,j) != 0 && i != j)
					 {
						 return false;
					 }
				 }
			 }
			 return true;
		 }
		 return false;
	 }

	 else if (matType == "any")
	 {
		 return true;
	 }
	
	 else { return false; }
}

//calculate the "weight" of the matrix
void SMatrix::sizeInBytes()
{
	int sum = (_colSize + _rowSize) * sizeof(MNode*);
	double sumOfElement = _elemNum * sizeof(MNode);
	cout << sum + sumOfElement + sizeof(SMatrix) << endl;
}

//prints by columns
void SMatrix::printColumnsIndexes()
{
	for (int i = 0; i < _colSize; i++)
	{
		cout << i << ": ";
		while (_colHead[i] != NULL && _colHead[i]->_indexI<_rowSize)
		{
			cout << "(" << _colHead[i]->_indexI << "," << _colHead[i]->_data << ")->";
			_colHead[i] = _colHead[i]->_nextCol;
		}
		cout << endl;
	}
}

//prints by rows
void SMatrix::printRowsIndexes()
{
	for (int i = 0; i < _rowSize ; i++)
	{
		cout << i << ": ";
		while (_rowHead[i] != NULL && _rowHead[i]->_indexJ<_colSize)
		{
			cout << "(" << _rowHead[i]->_indexJ << "," << _rowHead[i]->_data << ")->";
			_rowHead[i] = _rowHead[i]->_nextRow;
		}
		cout << endl;
	}
}

//ableing to add matrix with the operator "+"
SMatrix SMatrix:: operator+(SMatrix &other)
{
	if (this->_colSize == other._colSize && this->_rowSize == other._rowSize && 
		this->_matType == other._matType)
		//checks if the matrixs are the same size (MxN)
	{
		double cellSum = 0;
		SMatrix *result = new SMatrix (this->_rowSize, this->_colSize, this->_matType);
		for (int i = 0; i < this->_rowSize; i++)
		{
			for (int j = 0; j < this->_colSize; j++)
			{
				cellSum = this->getElement(i,j) + other.getElement(i,j);
				result->setElement(i, j, cellSum);
				cellSum = 0;
			}
		}
		return *result;
	}
	else { cout << "NA" << endl; exit(1); }
}

//ableing to place a matrix to other matrix and thanks to the method "+" you can 
//also do matrix + matrix = matrix
SMatrix& SMatrix :: operator=(SMatrix& other)
{
	if (this == &other) //if the matrix already same
		return *this;
	else
	{
		this->_colSize = other._colSize;
		this->_rowSize = other._rowSize;
		this->_elemNum = 0;
		this->_matType = other._matType;

		this->_rowHead = new MNode *[_rowSize];
		if (!_rowHead)
		{
			cout << "allocation error";
			exit(1);
		}

		this->_colHead = new MNode*[_colSize];
		if (!_colHead)
		{
			cout << "allocation error";
			exit(1);
		}

		for (int i = 0; i < _rowSize; i++)
		{
			_rowHead[i] = NULL;
		}
		for (int i = 0; i < _colSize; i++)
		{
			_colHead[i] = NULL;
		}

		double temp;
		for (int i = 0; i < _rowSize; i++)
		{
			for (int j = 0; j < _colSize; j++)
			{
				temp = other.getElement(i, j);
				this->setElement(i, j, temp);
			}
		}
	}return *this;
}

