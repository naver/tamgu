/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : vecta.h
 Date       : 2017/09/01
 Purpose    : Template to handle vector of pointers. 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef vecta_h
#define vecta_h

template <class Z> class vecte {
public:

	Z* vecteur;
	//sz is the vector size
	long sz;
	//last element appended... 
	long last;

	vecte(long t = 3) {
		last = 0;
		sz = t + 1;
		vecteur = (Z*)malloc(sizeof(Z)*sz);
		memset(vecteur, NULL, sizeof(Z)*sz);
	}

	vecte(long nb, Z val) {
		sz = nb;
		vecteur = (Z*)malloc(sizeof(Z)*sz);
		for (last = 0; last < nb; last++)
			vecteur[last] = val;
	}

	vecte(vecte<Z>& v) {
		sz = v.sz;
		last = v.last;
		vecteur = (Z*)malloc(sizeof(Z)*sz);
		memcpy(vecteur, v.vecteur, sizeof(Z)*sz);
	}

	~vecte() {
		if (sz)
			free(vecteur);
	}


	inline double get_(long pos) {
		return vecteur[pos];
	}

	inline void set_(long pos, double v) {
		vecteur[pos] = v;
	}

	inline void setlast(Z val) {
		vecteur[last - 1] = val;
	}

	inline long size() {
		return last;
	}

	inline void wipe() {
		while (last > 0) {
			if (vecteur[--last] != NULL) {
				delete vecteur[last];
				vecteur[last] = NULL;
			}
		}
	}

	inline void cleaning() {
		while (last > 0) {
			delete vecteur[--last];
		}
	}

	inline void clean() {
		for (last = 0; last < sz; last++) {
			if (vecteur[last] != NULL) {
				delete vecteur[last];
				vecteur[last] = NULL;
			}
		}
		last = 0;
	}

	void clear() {
		last = 0;
	}

	inline void reserve(long t) {
		if (t > sz) {
			sz = t;
			//We reallocate our vecteur
			vecteur = (Z*)realloc(vecteur, sizeof(Z)*sz);
			memset(vecteur + last, NULL, sizeof(Z)*(sz - last));
		}
	}

	inline void trim() {
		if (!last) {
			free(vecteur);
			vecteur = NULL;
			sz = 0;
			return;
		}
		if (last == sz)
			return;

		Z* trimmed = (Z*)malloc(sizeof(Z)*last);
		memcpy(trimmed, vecteur, sizeof(Z)*last);
		free(vecteur);
		vecteur = trimmed;
		sz = last;
	}

	inline void resize(long i) {
		if (i >= sz) {
			sz = i << 1;
			//We reallocate our vecteur
			vecteur = (Z*)realloc(vecteur, sizeof(Z)*sz);
			memset(vecteur + last, NULL, sizeof(Z)*(sz - last));
		}
	}


	inline Z remove(long pos = -1) {
		Z v;
		if (pos < 0) {
			if (last == 0)
				return NULL;
			last--;
			v = vecteur[last];
			vecteur[last] = NULL;
			return v;
		}

		if (pos >= last)
			return NULL;

		v = vecteur[pos];
		//We move all the boxes by 1...
		last--;
		for (; pos < last; pos++)
			vecteur[pos] = vecteur[pos + 1];
		vecteur[last] = NULL;
		return v;
	}

	inline void swap(long i, long j) {
		Z e = vecteur[i];
		vecteur[i] = vecteur[j];
		vecteur[j] = e;
	}

	inline Z backpop() {
		return vecteur[--last];
	}

	inline void pop_back() {
		last--;
	}

	inline void insert(long pos, Z val) {
		resize(last);

		//In this case, this is a simple push
		if (pos >= last) {
			vecteur[last++] = val;
			return;
		}

		// the element is then added in its place
		// if the box is empty we place it at this place
		//If not, all elements are moved to the right.
		//sinon, on deplace tous les elements d'une case vers la droite
		for (long i = last - 1; i >= pos; i--)
			vecteur[i + 1] = vecteur[i];
		vecteur[pos] = val;
		last++;
	}

	inline Z back() {
		return vecteur[last - 1];
	}

	inline Z preback() {
		return vecteur[last - 2];
	}

	inline void push_back(Z val) {
		resize(last);
		//sinon on ajoute l'element en queue...
		vecteur[last++] = val;
	}

	void padding(long nb) {
		if (sz < (nb + last)) {
			sz = (nb + last) << 1;
			//We reallocate our vecteur
			vecteur = (Z*)realloc(vecteur, sizeof(Z)*sz);
			memset(vecteur + last, NULL, sizeof(Z)*(sz - last));
		}
	}

	void padding(long nb, Z v) {
		if (sz < (nb + last)) {
			sz = (nb + last) << 1;
			//We reallocate our vecteur
			vecteur = (Z*)realloc(vecteur, sizeof(Z)*sz);
			for (nb = last; nb < sz; nb++)
				vecteur[nb] = v;
		}
	}

	inline Z operator [](long pos) {
		return vecteur[pos];
	}

	void operator =(vecte<Z>& t) {
		if (sz < t.sz) {
			sz = t.sz;
			vecteur = (Z*)realloc(vecteur, sizeof(Z)*sz);
		}
		last = t.last;
		memcpy(vecteur, t.vecteur, sizeof(Z)*last);
	}

	inline bool empty() {
		return !last;
	}

	inline Z get(long pos) {
		return (pos >= last) ? NULL : vecteur[pos];
	}

	void reverse() {
		Z e;
		long stop = last >> 1;
		for (long i = 0; i < stop; i++) {
			e = vecteur[i];
			vecteur[i] = vecteur[last - i];
			vecteur[last - i] = e;
		}
	}

	void erase(long i) {
		if (i == last) {
			vecteur[last--] = NULL;
			return;
		}

		if (i >= 0 && i < last) {
			if (last == sz) {
				last--;
				while (i < last) {
					vecteur[i] = vecteur[i + 1];
					i++;
				}
				vecteur[last] = NULL;
				return;
			}

			while (i < last) {
				vecteur[i] = vecteur[i + 1];
				i++;
			}
			last--;
		}
	}

	inline Z removeElement(long i = -1) {
		if (!last)
			return NULL;

		long pos = i;

		if (i == -1)
			pos = last - 1;

		Z v = vecteur[pos];
		vecteur[pos] = NULL;

		//On deplace toutes les cases de 1...

		if (i != -1) {
			for (long k = i; k < last - 1; k++)
				vecteur[k] = vecteur[k + 1];
			if (last > 0)
				vecteur[last - 1] = NULL;
		}

		last--;
		return v;
	}

	inline void atlast(Z val) {
		vecteur[last - 1] = val;
	}

	inline void beforelast(Z val) {
		if (last) {
			reserve(last);
			vecteur[last] = vecteur[last - 1];
			vecteur[last - 1] = val;
			last++;
		}
		else
			vecteur[last++] = val;
	}

	inline void at(long pos, Z val) {
		reserve(pos + 1);
		vecteur[pos] = val;
	}

	inline long search(Z v) {
		long i = 0;
		for (; i < last && v != vecteur[i]; i++) {}
		return (i == last) ? -1 : i;
	}

	inline bool check(Z v) {
		long i = 0;
		for (; i < last && v != vecteur[i]; i++) {}
		return (i != last);
	}

	inline bool checkanderase(Z v) {
		long i = 0;
		for (; i < last && v != vecteur[i]; i++) {}

		if (i != last) {
			last--;
			while (i < last) {
				vecteur[i] = vecteur[i + 1];
				i++;
			}
			return true;
		}
		return false;
	}

	inline bool operator ==(vecte<Z>& v) {
		if (last != v.last)
			return false;

		long i = 0;
		for (; i < last && v.vecteur[i] == vecteur[i]; i++) {}
		return (i == last);
	}

	inline void to_vector(vector<Z>& v) {
		for (long i = 0; i < last; i++)
			v.push_back(vecteur[i]);
	}

};

#endif







