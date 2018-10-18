/*Daniel Ben-Ami
ID 204084370  */

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
