/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : jagvecte.h
 Date       : 2017/09/01
 Purpose    : Template to handle vector of pointers. 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
   */

#ifndef vecte_h
#define vecte_h

template <class Z> class VECTE {
public:

	//Un vecteur de Fonction
	Z* vecteur;
	//sz est la sz actuelle de la liste
	long sz;
	//last element entre... Pour gerer les ajouts en fin de liste...
	long last;

	VECTE(long t = 3) {
		vecteur = NULL;
		if (t > 0)
            vecteur = (Z*)malloc(sizeof(Z)*t);

		sz = t;
        for (bint i = 0; i< sz; i++)
            vecteur[i] = NULL;

		last = 0;
	}

	~VECTE() {
		free(vecteur);
	}

	long size() {
		return last;
	}

    void wipe() {
        while (last>0) {
            if (vecteur[--last] != NULL) {
                delete vecteur[last];
                vecteur[last] = NULL;
            }
        }
    }
    
    void clean() {
		if (vecteur == NULL)
			return;
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

	void reserve(long t) {
		if (t <= sz)
			return;

		Z* tfs;

		//on realloue par bloc de t
		tfs = (Z*)malloc(sizeof(Z)*t);

        for (bint i = 0; i< t; i++) {
            if (i < last)
                tfs[i] = vecteur[i];
            else
                tfs[i] = NULL;
        }

        free(vecteur);
		vecteur = tfs;
		sz = t;
	}

    void taillor(long t) {
        Z* tfs;

        if (t <= sz)
            return;
        //on realloue par bloc de t
        tfs = (Z*)malloc(sizeof(Z)*t);
        for (bint i = 0; i< t; i++) {
            if (i < last)
                tfs[i] = vecteur[i];
            else
                tfs[i] = NULL;
        }

        free(vecteur);
        vecteur = tfs;
        sz = t;
    }

    void resize(long t) {
        Z* tfs;

        if (t <= sz)
            return;
        //on realloue par bloc de t
        tfs = (Z*)malloc(sizeof(Z)*t);
        for (bint i = 0; i< t; i++) {
            if (i < last)
                tfs[i] = vecteur[i];
            else
                tfs[i] = NULL;
        }

        free(vecteur);
        vecteur = tfs;
        sz = t;
    }

    void resize(long t, Z init) {
        Z* tfs;

        if (t <= sz)
        return;
        //on realloue par bloc de t
        tfs = (Z*)malloc(sizeof(Z)*t);
        for (bint i = 0; i< t; i++) {
            if (i < last)
            tfs[i] = vecteur[i];
            else
            tfs[i] = init;
        }

        free(vecteur);
        vecteur = tfs;
        sz = t;
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
		//On deplace toutes les cases de 1...
		last--;
		for (; pos < last; pos++)
			vecteur[pos] = vecteur[pos + 1];
		vecteur[last] = NULL;
		return v;
	}

    inline Z backpop() {
        return vecteur[--last];
    }

	inline void pop_back() {
		last--;
	}

	inline void insert(long pos, Z val) {
        if (last >= sz)
			taillor(sz<<1);

		//Dans ce cas, c'est un simple push
		if (pos >= last) {
			vecteur[last++] = val;
			return;
		}

		//on ajoute alors l'element a sa place
		//si la case est vide on le place a cet endroit
		//sinon on effectue un deplacement de tous les elements vers la droite
		if (vecteur[pos] != NULL) {
			//sinon, on deplace tous les elements d'une case vers la droite
			for (long i = last - 1; i >= pos; i--)
				vecteur[i + 1] = vecteur[i];
			vecteur[pos] = val;
			last++;
		}
		else
			vecteur[pos] = val;
	}

	inline Z back() {
		return vecteur[last - 1];
	}

	inline void push_back(Z val) {

		if (last >= sz)
			taillor(sz<<1);

		//sinon on ajoute l'element en queue...
		vecteur[last++] = val;
	}


	inline Z operator [](long pos) {
		return vecteur[pos];
	}

	void erase(long i) {
		if (i < 0 || i >= last)
			return;

		if (last == sz) {
			last--;
			while (i < last) {
				vecteur[i] = vecteur[i+1];
                i++;
			}
			vecteur[last] = NULL;
			return;
		}

		while (i < last) {
			vecteur[i] = vecteur[i+1];
            i++;
		}
		last--;
	}

	inline Z removeElement(long i = -1) {
		if (last == 0)
			return NULL;

		long pos = i;

		if (i == -1)
			pos = last - 1;

		Z v = vecteur[pos];
		vecteur[pos] = NULL;

		//On deplace toutes les cases de 1...

		if (i != -1) {
			for (bint k = i; k<last - 1; k++)
				vecteur[k] = vecteur[k + 1];
			if (last>0)
				vecteur[last - 1] = NULL;
		}

		last--;
		return v;
	}

	void shaveoff() {
		if (last == sz)
			return;
		Z* tfs;

		//on realloue par bloc de t
		tfs = (Z*)malloc(sizeof(Z)*last);

        for (bint i = 0; i < last; i++)
            tfs[i] =  vecteur[i];

		free(vecteur);
		vecteur = tfs;
		sz = last;
	}

	void at(long pos, Z val) {
		if (pos >= sz)
            taillor(pos<<1);
		vecteur[pos] = val;
	}

    void pushat(long pos, Z val) {
        vecteur[pos] = val;
        if (pos >= last)
            last = pos+1;
    }


	void operator =(VECTE<Z>& z) {
		last = z.last;
		if (last >= sz) {
			free(vecteur);
            sz = last<<1;
			vecteur = (Z*)malloc(sizeof(Z)*sz);
		}

        for (bint i = 0; i < last; i++)
            vecteur[i] =  z.vecteur[i];
	}

	inline long search(Z v) {
        for (bint i = 0; i< last; i++) {
            if (vecteur[i] == v)
                return i;
        }
		return -1;
	}

    inline bool check(Z v) {
        for (bint i = 0; i< last; i++) {
            if (vecteur[i] == v)
                return true;
        }
        return false;
    }

};

#endif






