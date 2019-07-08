/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : matrix.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef matrix_h
#define matrix_h


class TAMGUILINEINTEGER {
public:
	long* line;
	long size;

	TAMGUILINEINTEGER() {
		size = 0;
		line = NULL;
	}

	~TAMGUILINEINTEGER() {
		if (line != NULL)
			delete[] line;
	}

	void clear(long nb = 0) {
		if (line != NULL) {
			delete[] line;
			size = 0;
			line = NULL;
		}
	}

	void release() {
		line = NULL;
		size = 0;
	}

	void update(long r) {
		if (r >= size) {
			long* ln = new long[r + 1];
			memset(ln, 0, (r + 1)*sizeof(long));
			if (line != NULL) {
				memcpy(ln, line, size*sizeof(long));
				delete[] line;
			}
			size = r + 1;
			line = ln;
		}
	}

	inline long& operator [](long r) {
		if (r >= size) {
			long* ln = new long[r + 1];
			memset(ln, 0, (r + 1)*sizeof(long));
			if (line != NULL) {
				memcpy(ln, line, size*sizeof(long));
				delete[] line;
			}
			size = r + 1;
			line = ln;
		}
		return line[r];
	}

	void init(long r) {
		if (line == NULL) {
			size = r;
			line = new long[r];
			memset(line, 0, size*sizeof(long));
		}
	}

	bool check(long r) {
		if (r < 0 || r >= size || line == NULL)
			return false;
		return true;
	}
};

class TAMGUILINEFLOAT {
public:
	double* line;
	long size;

	TAMGUILINEFLOAT() {
		size = 0;
		line = NULL;
	}

	~TAMGUILINEFLOAT() {
		if (line != NULL)
			delete[] line;
	}

	void clear(long nb = 0) {
		if (line != NULL) {
			delete[] line;
			size = 0;
			line = NULL;
		}
	}

	void release() {
		line = NULL;
		size = 0;
	}

	void update(long r) {
		if (r >= size) {
			double* ln = new double[r + 1];
			memset(ln, 0, (r + 1)*sizeof(double));
			if (line != NULL) {
				memcpy(ln, line, size*sizeof(double));
				delete[] line;
			}
			size = r + 1;
			line = ln;
		}
	}

	inline double& operator [](long r) {
		if (r >= size) {
			double* ln = new double[r + 1];
			memset(ln, 0, (r + 1)*sizeof(double));
			if (line != NULL) {
				memcpy(ln, line, size*sizeof(double));
				delete[] line;
			}
			size = r + 1;
			line = ln;
		}
		return line[r];
	}

	void init(long r) {
		if (line == NULL) {
			size = r;
			line = new double[r];
			memset(line, 0, size*sizeof(double));
		}
	}

	bool check(long r) {
		if (r < 0 || r >= size || line == NULL)
			return false;
		return true;
	}
};

template<class Z> class TAMGUIMATRIX {
public:
	Z* matrix;
	long size;

	TAMGUIMATRIX() {
		matrix = NULL;
		size = 0;
	}

	TAMGUIMATRIX(long r) {
		matrix = NULL;
		size = 0;
		init(r);
	}

	void clear(long nb = 0) {
		if (matrix != NULL) {
			delete[] matrix;
			matrix = NULL;
			size = 0;
		}
	}

	~TAMGUIMATRIX() {
		clear();
	}

	void update(long r, long c) {
		if (r >= size) {
			Z* mat = new Z[r + 1];
			memset(mat, NULL, (r + 1)*sizeof(Z));
			if (matrix != NULL) {
				for (long i = 0; i < size; i++) {
					mat[i].line = matrix[i].line;
					mat[i].size = matrix[i].size;
					matrix[i].release();
				}
				delete[] matrix;
			}
			matrix = mat;
			size = r + 1;
		}

		for (long i = 0; i<size; i++)
			matrix[i].update(c);
	}

	void init(long r) {
		if (matrix == NULL && r > 0) {
			size = r;
			matrix = new Z[size];
		}
	}

	inline Z& operator[](long r) {
		if (r >= size) {
			Z* mat = new Z[r + 1];
			memset(mat, NULL, (r + 1)*sizeof(Z));
			if (matrix != NULL) {
				for (long i = 0; i < size; i++) {
					mat[i].line = matrix[i].line;
					mat[i].size = matrix[i].size;
					matrix[i].release();
				}
				delete[] matrix;
			}
			matrix = mat;
			size = r + 1;
		}
		return matrix[r];
	}

	bool check(long r) {
		if (r < 0 || r >= size || matrix == NULL || matrix[r].line == NULL)
			return false;
		return true;
	}
};


#endif


