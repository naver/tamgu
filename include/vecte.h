/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : vecte.h
 Date       : 2017/09/01
 Purpose    : Template to handle vector of pointers. 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
   */

#ifndef vecte_h
#define vecte_h

class Tamgu;
extern Exchanging Tamgu* aNULL;

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
    
    VECTE(VECTE<Z>& v) {
        sz = v.sz;
        vecteur = (Z*)malloc(sizeof(Z)*sz);
        for (last = 0; last < v.last; last++)
            vecteur[last] = v.vecteur[last];
        
        for (;last < sz; last++)
            vecteur[last] = NULL;
        last = v.last;
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
            tfs[i] = (i < last) ? vecteur[i] : NULL;
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
            tfs[i] = (i < last) ? vecteur[i] : NULL;
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
            tfs[i] = (i < last) ? vecteur[i] : NULL;
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
            tfs[i] = (i < last) ? vecteur[i] : init;
        }
        
        free(vecteur);
        vecteur = tfs;
        sz = t;
    }    

    inline Z backpop() {
        return vecteur[--last];
    }
    
	inline void pop_back() {
		last--;
	}

    inline void move(long pos, long nb) {
        if ((last+nb) >= sz)
            taillor(last+nb+3);

        //Dans ce cas, c'est un simple push
        if (pos >= last) {
            last += nb;
            return;
        }
        
        
        for (long i = last - 1; i >= pos; i--)
            vecteur[i + nb] = vecteur[i];
        last += nb;
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

    inline bool push_size(Z val, long mx) {

        if (last >= sz)
            taillor(sz<<1);

        //sinon on ajoute l'element en queue...
        vecteur[last++] = val;
        return (last >= mx);
    }

    inline bool check() {
        return (last < sz);
    }
    
    inline void push(Z val) {
        //sinon on ajoute l'element en queue...
        vecteur[last++] = val;
    }

	inline Z operator [](long pos) {
		return vecteur[pos];
	}

    void eraseraw(long i) {
        for (; i < last; i++)
            vecteur[i] = vecteur[i+1];
        last--;
    }

    void erase(long i) {
        if (i >= 0 && i < last) {
            for (; i < last; i++)
                vecteur[i] = vecteur[i+1];
            last--;
        }
    }

	inline Z removeElement(long i = -1) {
        i = (i < 0)?last - 1 : i;
        
        if (i >= 0 && i < last) {
            Z v = vecteur[i];
            for (; i < last; i++)
                vecteur[i] = vecteur[i + 1];
            last--;
            return v;
        }
        return NULL;
	}

    inline Z remove(long i = -1) {
        i = (i < 0)?last - 1 : i;
        
        if (i >= 0 && i < last) {
            Z v = vecteur[i];
            for (; i < last; i++)
                vecteur[i] = vecteur[i + 1];
            last--;
            return v;
        }
        return NULL;
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
        bint i;
        for (i = 0; i< last && vecteur[i] != v; i++) {}
        if (i != last)
            return i;
        return -1;
	}

    inline bool removevalue(Z v) {
        long i;
        for (i = 0; i < last && vecteur[i] != v; i++) {}
        if (i != last) {
            eraseraw(i);
            return true;
        }
        return false;
    }

    inline bool check(Z v) {
        bint i;
        for (i = 0; i< last && vecteur[i] != v; i++) {}
        return (i != last);
    }

};

//-------------------------------------------------------
//atomic_vector
//-------------------------------------------------------

const long base_atomic_size = 8;
const long atomic_vector_size = 1 << base_atomic_size;

template <class Z> class atomic_vector_element {
public:
    std::atomic<Z> vecteur[atomic_vector_size];
    atomic_vector_element<Z>* next;

    atomic_vector_element(Z& z) {
        for (short i = 0; i < atomic_vector_size; i++)
            vecteur[i] = z;
        next = NULL;
    }
    
    ~atomic_vector_element() {
        if (next != NULL)
            delete next;
    }

    void clear(Z& z) {
        for (short i = 0; i < atomic_vector_size; i++)
            vecteur[i] = z;
        if (next != NULL)
            next->clear(z);
    }
    
    void clean(Z& z) {
        Z v;
        for (short i = 0; i < atomic_vector_size; i++) {
            v = vecteur[i].exchange(z);
            if (v != z)
                delete v;
        }
        if (next != NULL)
            next->clean(z);
    }
    
    atomic_vector_element<Z>* copy(atomic_vector_element<Z>* a, Z& z, long& nbelements) {
        Z v;
        for (short i = 0; i < atomic_vector_size; i++) {
            v = a->vecteur[i];
            vecteur[i] = v;
        }
        if (a->next != NULL) {
            if (next == NULL) {
                next = new atomic_vector_element<Z>(z);
                nbelements++;
            }
            return next->copy(a->next, z, nbelements);
        }
        return this;
    }
    
    void duplicate(atomic_vector_element<Z>* a) {
        Z v;
        for (short i = 0; i < atomic_vector_size; i++) {
            v = a->vecteur[i];
            vecteur[i] = v;
        }
    }
    
    Z replace(Z v, long i) {
        return vecteur[i].exchange(v);
    }
    
    void set(Z v, long i) {
        vecteur[i] = v;
    }
};


template <class Z> class atomic_vector {
public:

    atomic_vector_element<Z>* head;
    atomic_vector_element<Z>* end;
    std::recursive_mutex _lock;

    //Un vecteur de Fonction
    long nbelements;
    std::atomic<long> last;
    Z zero;

    atomic_vector<Z>(Z& z) {
        zero = z;
        head = new atomic_vector_element<Z>(zero);
        end = head;
        nbelements = 1;
        last = 0;
    }

    atomic_vector<Z>(Z z, bool p) {
        zero = z;
        head = new atomic_vector_element<Z>(zero);
        end = head;
        nbelements  = 1;
        last = 0;
    }
    
    ~atomic_vector<Z>() {
        delete head;
    }
    
    long size() {
        return last;
    }
    
    void clean() {
        _lock.lock();
        head->clean(zero);
        _lock.unlock();
        last = 0;
    }
    
    void clear() {
        _lock.lock();
        head->clear(zero);
        _lock.unlock();
        last = 0;
    }
    
    atomic_vector_element<Z>* findelement(long pos, long& i) {
        if (pos >= last)
            return NULL;
        
        if (pos < atomic_vector_size) {
            i = pos;
            return head;
        }
        
        long nb = pos >> base_atomic_size;
        i =  pos - (nb << base_atomic_size);

        atomic_vector_element<Z>* n = head;
        while (nb > 0) {
            n = n->next;
            nb--;
        }
        
        return n;
    }

    atomic_vector_element<Z>* findpos(long pos, long& i) {
        if (pos < atomic_vector_size) {
            i = pos;
            return head;
        }
        
        long nb = pos >> base_atomic_size;
        i =  pos - (nb << base_atomic_size);
        if (nb >= nbelements)
            return NULL;
        
        atomic_vector_element<Z>* n = head;
        while (nb > 0) {
            n = n->next;
            nb--;
        }
        
        return n;
    }
    

    void cleanfrom(long pos) {
        long i = 0;
        atomic_vector_element<Z>* n = findelement(pos, i);
        if (n == NULL)
            return;

        long lst = pos;
		Z v;
        while (n != NULL) {
            for (;i < base_atomic_size; i++) {
                if (pos == last) {
                    last = lst;
                    return;
                }
				v = n->replace(zero, i);
                if (v != zero)
					delete v;
                pos++;
            }
            i = 0;
            n = n->next;
        }
        last = lst;
    }
    
    inline void erase(long pos) {
        long i = 0;
        atomic_vector_element<Z>* n = findelement(pos, i);
        if (n == NULL)
            return;
        
        n->vecteur[i] = zero;
    }

    
    inline Z replace(long pos, Z nv) {
        long i = 0;
        atomic_vector_element<Z>* n = findpos(pos, i);
        if (n != NULL) {
            
            if (pos > last)
                last = pos + 1;

            return n->replace(nv, i);
        }
        
        long nb = pos >> base_atomic_size;
        i =  pos - (nb << base_atomic_size);
        n = head;
        
        while (nb > 0) {
            n = n->next;
            if (n == NULL) {
                _lock.lock();
                n = findpos(pos, i);
                //We check again to see if it has been created by another thread...
                if (n != NULL) {
                    Z v = n->replace(nv, i);
                    if (pos > last)
                        last = pos + 1;
                    _lock.unlock();
                    return v;
                }
                
                n = new atomic_vector_element<Z>(zero);
                ++nbelements;
                end->next = n;
                end = n;
                _lock.unlock();
            }
            nb--;
        }
        
        if (pos > last)
            last = pos + 1;

        return n->replace(nv, i);
    }

    inline Z remove(long pos) {
        long i = 0;
        atomic_vector_element<Z>* n = findelement(pos, i);
        if (n == NULL)
            return zero;

        Z v = n->replace(zero, i);
        
        if (last == pos+1)
            last--;
        else {
            _lock.lock();
            while (pos < last) {
                if (i == atomic_vector_size - 1) {
                    if (n->next == NULL)
                        break;
                    n->set(n->next->vecteur[0], i);
                    i = 0;
                    n = n->next;
                    pos++;
                    continue;
                }
                n->set(n->vecteur[i+1], i);
                i++;
                pos++;
            }
            last--;
            _lock.unlock();
        }

        return v;
    }
    
    inline void pop(long pos) {
        long i = 0;
        atomic_vector_element<Z>* n = findelement(pos, i);
        if (n == NULL)
            return;
        
        if (last == pos+1)
            last--;
        else {
            _lock.lock();
            while (pos < last) {
                if (i == atomic_vector_size - 1) {
                    if (n->next == NULL)
                        break;
                    n->set(n->next->vecteur[0], i);
                    i = 0;
                    n = n->next;
                    pos++;
                    continue;
                }
                n->set(n->vecteur[i+1], i);
                i++;
                pos++;
            }
            last--;
            _lock.unlock();
        }
    }

    inline Z backpop() {
        if (!last)
            return zero;

        long i = 0;
        long pos = --last;
        atomic_vector_element<Z>* n = findpos(pos, i);
        return n->vecteur[i];
    }

    Z back() {
        if (!last)
            return zero;
        
        long i = 0;
        long pos = last-1;
        atomic_vector_element<Z>* n = findpos(pos, i);
        return n->vecteur[i];
    }
    
    void pop_back() {
        if (!last)
            return;
        --last;
    }

    Z remove_back() {
        if (!last)
            return zero;
        long i = 0;
        long pos = --last;
        atomic_vector_element<Z>* n = findpos(pos, i);
        
        return n->replace(zero, i);
    }

    inline long push_back(Z val) {
        long pos = last++;

        if (pos < atomic_vector_size) {
            head->vecteur[pos] = val;
            return pos;
        }
        
        long i = 0;
        atomic_vector_element<Z>* n = findpos(pos, i);
        if (n == NULL) {
            //This is the only case when we need to protect our structure...
            _lock.lock();
            n = findpos(pos, i);
            if (n == NULL) {
                n = new atomic_vector_element<Z>(zero);
                end->next = n;
                end = n;
                ++nbelements;
            }
            _lock.unlock();
        }
        
        n->vecteur[i] = val;
        return pos;
    }
    
    inline long put(Z val, long pos) {
        //This is a case when we try to find a slot...
        if (pos >= last)
            return push_back(val);
        
        long i = 0;
        atomic_vector_element<Z>* n = findpos(pos, i);
        n->vecteur[i] = val;
        return pos;
    }

    inline long set(Z val, long pos) {
            //This is a case when we try to find a slot...
        long i = 0;
        atomic_vector_element<Z>* n = findelement(pos, i);
        if (n == NULL)
            return push_back(val);

        n->vecteur[i] = val;
        return pos;
    }
    
    inline void setback(Z val) {
            //This is a case when we try to find a slot...
        long i = 0;
        long pos = last-1;
        atomic_vector_element<Z>* n = findpos(pos, i);
        if (n == NULL)
            return;
        n->vecteur[i] = val;
    }
    
    inline Z operator [](long pos) {
        long i = 0;
        atomic_vector_element<Z>* n = findelement(pos, i);
        if (n == NULL)
            return zero;
        return n->vecteur[i];
    }
    
    void copy(atomic_vector<Z>* a) {
        end = head->copy(a->head, zero, nbelements);
        long pos = a->last;
        last = pos;
        zero = a->zero;
    }
    
    inline atomic_vector& operator=(atomic_vector<Z>& a) {
        copy(&a);
        return *this;
    }
    
    inline bool check(long pos, Z v) {
        long i = 0;
        atomic_vector_element<Z>* n = findelement(pos, i);
        if (n == NULL || n->vecteur[i] != v)
            return false;
        return true;
    }

    void insert(long pos, Z val) {
        long i = 0;
        atomic_vector_element<Z>* n = findpos(pos, i);
            //In this case we can use this slot...
        if (n->vecteur[i] == zero) {
            n->vecteur[i] = val;
            return;
        }
        
        _lock.lock();

        long lst = last++;
        long nb =  lst >> base_atomic_size;
        i =  lst - (nb << base_atomic_size);
        
        atomic_vector_element<Z>* prev;
        if (nb == nbelements) {
                //We need one more chunk to add stuff in...
            n = new atomic_vector_element<Z>(zero);
            end->next = n;
            end = n;
            ++nbelements;
            i = 0;
            lst--;
        }
        else
            n = findpos(lst, i);

        //We must shift all elements to the right...
        while (lst != pos) {
            if (i == 0) {
                prev = head;
                while (prev->next != n) prev=prev->next;
                i = atomic_vector_size-1;
                n->set(prev->vecteur[i], 0);
                n = prev;
            }
            else
                n->set(n->vecteur[i-1], i);
            i--;
            lst--;
        }
        n->vecteur[i] = val;
        _lock.unlock();
    }
};

template <class Z> class atomic_vector_iterator {
public:
    
    atomic_vector_element<Z>* av;
    Z second;

    long last;
    
    long first;
    long i, pos;
    bool ending;

    atomic_vector_iterator<Z>() {
        ending =  false;
        av =  NULL;
        i = 0;
        first = 0;
        pos = 0;
        last = 0;
    }

    atomic_vector_iterator<Z>(atomic_vector<Z>& v) {
        begin(v);
    }

    atomic_vector_iterator<Z>(atomic_vector<Z>& v, long init) {
        ending =  false;
        i = 0;
        first = 0;
        pos = init;
        last  = v.last;
        av = v.findelement(pos, i);
        if (av == NULL)
            ending = true;

        next();
    }

    void next() {
        if (pos == last) {
            ending = true;
            return;
        }
        
        if (i == atomic_vector_size) {
            i = 0;
            av = av->next;
            if (av == NULL) {
                ending = true;
                return;
            }
        }
        
        first = pos++;
        second = av->vecteur[i++];
        if (second ==  NULL)
            second = aNULL;
    }
    
    void replace(Z v) {
        av->vecteur[i-1] = v;
    }
    
    bool end() {
        return ending;
    }
    
    void begin(atomic_vector<Z>& v) {
        ending =  false;
        last = v.last;
        av =  v.head;
        i = 0;
        first = 0;
        pos = 0;
        next();
    }
};

//-------------------------------------------------------
//atomic_value_vector
//-------------------------------------------------------

template <class Z> class atomic_value_vector_element {
public:
    Z vecteur[atomic_vector_size];
    atomic_value_vector_element<Z>* next;

    atomic_value_vector_element(Z& z) {
        for (short i = 0; i < atomic_vector_size; i++)
            vecteur[i] = z;
        next = NULL;
    }
    
    ~atomic_value_vector_element() {
        if (next != NULL)
            delete next;
    }

    void clear(Z& z) {
        for (short i = 0; i < atomic_vector_size; i++)
            vecteur[i] = z;
        if (next != NULL)
            next->clear(z);
    }
    
    void clean(Z& z) {
        for (short i = 0; i < atomic_vector_size; i++) {
            if (vecteur[i] != z)
                delete vecteur[i];
            vecteur[i] = z;
        }
        if (next != NULL)
            next->clean(z);
    }
    
    atomic_value_vector_element<Z>* copy(atomic_value_vector_element<Z>* a, Z& z, long& nbelements) {
        for (short i = 0; i < atomic_vector_size; i++)
            vecteur[i] = a->vecteur[i];
        if (a->next != NULL) {
            if (next == NULL) {
                next = new atomic_value_vector_element<Z>(z);
                nbelements++;
            }
            return next->copy(a->next, z, nbelements);
        }
        return this;
    }
    
    void duplicate(atomic_value_vector_element<Z>* a) {
        for (short i = 0; i < atomic_vector_size; i++)
            vecteur[i] = a->vecteur[i];
    }
};


template <class Z> class atomic_value_vector {
public:

    atomic_value_vector_element<Z>* head;
    atomic_value_vector_element<Z>* end;
    std::recursive_mutex _lock;

    //Un vecteur de Fonction
    long nbelements;
    std::atomic<long> last;
    Z zero;

	atomic_value_vector<Z>(Z& z) {
		zero = z;
		head = new atomic_value_vector_element<Z>(zero);
		end = head;
		nbelements = 1;
		last = 0;
	}

    atomic_value_vector<Z>(Z z, bool p) {
        zero = z;
        head = new atomic_value_vector_element<Z>(zero);
        end = head;
        nbelements  = 1;
        last = 0;
    }
    
    ~atomic_value_vector<Z>() {
        delete head;
    }
    
    long size() {
        return last;
    }
    
    void clean() {
        clear();
    }
    
    void clear() {
        _lock.lock();
        head->clear(zero);
        _lock.unlock();
        last = 0;
    }
    
    atomic_value_vector_element<Z>* findelement(long pos, long& i) {
        if (pos >= last)
            return NULL;
        
        if (pos < atomic_vector_size) {
            i = pos;
            return head;
        }
        
        long nb = pos >> base_atomic_size;
        i =  pos - (nb << base_atomic_size);

        atomic_value_vector_element<Z>* n = head;
        while (nb > 0) {
            n = n->next;
            nb--;
        }
        
        return n;
    }

    atomic_value_vector_element<Z>* findpos(long pos, long& i) {
        if (pos < atomic_vector_size) {
            i = pos;
            return head;
        }
        
        long nb = pos >> base_atomic_size;
        i =  pos - (nb << base_atomic_size);
        if (nb >= nbelements)
            return NULL;
        
        atomic_value_vector_element<Z>* n = head;
        while (nb > 0) {
            n = n->next;
            nb--;
        }
        
        return n;
    }
    

    void cleanfrom(long pos) {
        long i = 0;
        atomic_value_vector_element<Z>* n = findelement(pos, i);
        if (n == NULL)
            return;

        long lst = pos;
        while (n != NULL) {
            for (;i < base_atomic_size; i++) {
                if (pos == last) {
                    last = lst;
                    return;
                }
                
                if (n->vecteur[i] != NULL) {
                    delete n->vecteur[i];
                    n->vecteur[i] = zero;
                }
                pos++;
            }
            i = 0;
            n = n->next;
        }
        last = lst;
    }
    
    inline void erase(long pos) {
        long i = 0;
        atomic_value_vector_element<Z>* n = findelement(pos, i);
        if (n == NULL)
            return;
        
        n->vecteur[i] = zero;
    }

    
    inline void replace(long pos, Z nv) {
        long i = 0;
        atomic_value_vector_element<Z>* n = findpos(pos, i);
        if (n != NULL) {
            
            n->vecteur[i] = nv;
            
            if (pos > last)
                last = pos + 1;
            return;
        }
        
        long nb = pos >> base_atomic_size;
        i =  pos - (nb << base_atomic_size);
        n = head;
        
        while (nb > 0) {
            n = n->next;
            if (n == NULL) {
                _lock.lock();
                n = findpos(pos, i);
                //We check again to see if it has been created by another thread...
                if (n != NULL) {
                    n->vecteur[i] = nv;
                    if (pos > last)
                        last = pos + 1;
                    _lock.unlock();
                    return;
                }
                
                n = new atomic_value_vector_element<Z>(zero);
                ++nbelements;
                end->next = n;
                end = n;
                _lock.unlock();
            }
            nb--;
        }
        
        if (pos > last)
            last = pos + 1;

        n->vecteur[i] = nv;
    }

    inline void remove(long pos) {
        long i = 0;
        atomic_value_vector_element<Z>* n = findelement(pos, i);
        if (n == NULL)
            return zero;

        n->vecteur[i] = zero;
        if (last == pos+1)
            last--;
        else {
            _lock.lock();
            while (pos < last) {
                if (i == atomic_vector_size - 1) {
                    if (n->next == NULL)
                        break;
                    n->vecteur[i] = n->next->vecteur[0];
                    i = 0;
                    n = n->next;
                    pos++;
                    continue;
                }
                n->vecteur[i] = n->vecteur[i+1];
                i++;
                pos++;
            }
            last--;
            _lock.unlock();
        }
    }
    
    inline void pop(long pos) {
        long i = 0;
        atomic_value_vector_element<Z>* n = findelement(pos, i);
        if (n == NULL)
            return;
        
        if (last == pos+1)
            last--;
        else {
            _lock.lock();
            while (pos < last) {
                if (i == atomic_vector_size - 1) {
                    if (n->next == NULL)
                        break;
                    n->vecteur[i] = n->next->vecteur[0];
                    i = 0;
                    n = n->next;
                    pos++;
                    continue;
                }
                n->vecteur[i] = n->vecteur[i+1];
                i++;
                pos++;
            }
            last--;
            _lock.unlock();
        }
    }

    Z back() {
        if (!last)
            return zero;
        
        long i = 0;
        long pos = last-1;
        atomic_value_vector_element<Z>* n = findpos(pos, i);
        return n->vecteur[i];
    }
    
    void pop_back() {
        if (!last)
            return;
        --last;
    }

    Z remove_back() {
        if (!last)
            return zero;
        --last;
        long i = 0;
        long pos = last;
        atomic_value_vector_element<Z>* n = findpos(pos, i);
        
        Z v = n->vecteur[i];
        n->vecteur[i] = zero;
        return v;
    }

    inline long push_back(Z val) {
        long pos = last++;

        if (pos < atomic_vector_size) {
            head->vecteur[pos] = val;
            return pos;
        }
        
        long i = 0;
        atomic_value_vector_element<Z>* n = findpos(pos, i);
        if (n == NULL) {
            //This is the only case when we need to protect our structure...
            _lock.lock();
            n = findpos(pos, i);
            if (n == NULL) {
                n = new atomic_value_vector_element<Z>(zero);
                end->next = n;
                end = n;
                ++nbelements;
            }
            _lock.unlock();
        }
        
        n->vecteur[i] = val;
        return pos;
    }
    
    inline long put(Z val, long pos) {
        //This is a case when we try to find a slot...
        if (pos >= last)
            return push_back(val);
        
        long i = 0;
        atomic_value_vector_element<Z>* n = findpos(pos, i);
        n->vecteur[i] = val;
        return pos;
    }

    inline long set(Z val, long pos) {
            //This is a case when we try to find a slot...
        long i = 0;
        atomic_value_vector_element<Z>* n = findelement(pos, i);
        if (n == NULL)
            return push_back(val);

        n->vecteur[i] = val;
        return pos;
    }
    
    inline void setback(Z val) {
            //This is a case when we try to find a slot...
        long i = 0;
        long pos = last-1;
        atomic_value_vector_element<Z>* n = findpos(pos, i);
        if (n == NULL)
            return;
        n->vecteur[i] = val;
    }
    
    inline Z operator [](long pos) {
        long i = 0;
        atomic_value_vector_element<Z>* n = findelement(pos, i);
        if (n == NULL)
            return zero;
        return n->vecteur[i];
    }
    
    void copy(atomic_value_vector<Z>* a) {
        end = head->copy(a->head, zero, nbelements);
        long pos = a->last;
        last = pos;
        zero = a->zero;
    }
    
    inline atomic_value_vector& operator=(atomic_value_vector<Z>& a) {
        copy(&a);
        return *this;
    }
    
    inline bool check(long pos, Z v) {
        long i = 0;
        atomic_value_vector_element<Z>* n = findelement(pos, i);
        if (n == NULL || n->vecteur[i] != v)
            return false;
        return true;
    }

    void insert(long pos, Z val) {
        long i = 0;
        atomic_value_vector_element<Z>* n = findpos(pos, i);

        _lock.lock();

        long lst = last++;
        long nb =  lst >> base_atomic_size;
        i =  lst - (nb << base_atomic_size);
        
        atomic_value_vector_element<Z>* prev;
        if (nb == nbelements) {
                //We need one more chunk to add stuff in...
            n = new atomic_value_vector_element<Z>(zero);
            end->next = n;
            end = n;
            ++nbelements;
            i = 0;
            lst--;
        }
        else
            n = findpos(lst, i);

        //We must shift all elements to the right...
        while (lst != pos) {
            if (i == 0) {
                prev = head;
                while (prev->next != n) prev=prev->next;
                i = atomic_vector_size-1;
                n->vecteur[0] = prev->vecteur[i];
                n = prev;
            }
            else
                n->vecteur[i] = n->vecteur[i-1];
            i--;
            lst--;
        }
        n->vecteur[i] = val;
        _lock.unlock();
    }
};

template <class Z> class atomic_value_vector_iterator {
public:
    
    atomic_value_vector_element<Z>* av;
    Z second;
    
    long last;
    
    long first;
    long i, pos;
    bool ending;
    
    atomic_value_vector_iterator<Z>() {
        ending =  false;
        av =  NULL;
        i = 0;
        first = 0;
        pos = 0;
        last = 0;
    }
    
    atomic_value_vector_iterator<Z>(atomic_value_vector<Z>& v) {
        begin(v);
    }
    
    atomic_value_vector_iterator<Z>(atomic_value_vector<Z>& v, long init) {
        ending =  false;
        i = 0;
        first = 0;
        pos = init;
        last  = v.last;
        av = v.findelement(pos, i);
        if (av == NULL)
            ending = true;
        
        next();
    }
    
    void next() {
        if (pos == last) {
            ending = true;
            return;
        }
        
        if (i == atomic_vector_size) {
            i = 0;
            av = av->next;
            if (av == NULL) {
                ending = true;
                return;
            }
        }
        
        first = pos++;
        second = av->vecteur[i++];
    }
    
    void replace(Z v) {
        av->vecteur[i-1] = v;
    }
    
    bool end() {
        return ending;
    }
    
    void begin(atomic_value_vector<Z>& v) {
        ending =  false;
        last = v.last;
        av =  v.head;
        i = 0;
        first = 0;
        pos = 0;
        next();
    }
};

//------------------------------------------------------------
//// RING
//------------------------------------------------------------
template <class Z> class atomic_ring_element {
public:
    std::atomic<Z> vecteur[atomic_vector_size];
    
    atomic_ring_element(Z& z) {
        for (short i = 0; i < atomic_vector_size; i++)
            vecteur[i] = z;
    }
    
    void clear(Z& z) {
        for (short i = 0; i < atomic_vector_size; i++)
            vecteur[i] = z;
    }
    
    void clean(Z& z) {
        Z v;
        for (short i = 0; i < atomic_vector_size; i++) {
            v =  vecteur[i].exchange(z);
            if (v != z)
                delete v;
        }
    }
    
    void duplicate(atomic_ring_element<Z>* a) {
        Z v;
        for (short i = 0; i < atomic_vector_size; i++) {
            v = a->vecteur[i];
            vecteur[i] = v;
        }
    }
    
    Z replace(Z v, uchar i) {
        return vecteur[i].exchange(v);
    }
    
    void set(Z v, uchar i) {
        vecteur[i] = v;
    }
};

const unsigned short atomic_ring_size = 65535;

template <class Z> class atomic_ring {
public:
    
    std::recursive_mutex _lock;
    atomic_ring_element<Z>* head[256];
    
        //Un vecteur de Fonction
    std::atomic<unsigned short> first;
    std::atomic<unsigned short> last;
    Z zero;
    
    atomic_ring(Z z, bool p = true) {
        zero = z;
        for (long i = 0; i < 256; i++)
            head[i] = NULL;
        
        head[0] = new atomic_ring_element<Z>(zero);
        head[255] = new atomic_ring_element<Z>(zero);
        last = 0;
        first = 0;
    }
    
    ~atomic_ring() {
        last = 0;
        first = 0;
        for (long i = 0; i < 256; i++) {
            if (head[i] != NULL) {
                delete head[i];
                head[i] = NULL;
            }
        }
    }
    
    inline void addnewelement(uchar nb) {
        if (head[nb] == NULL) {
            _lock.lock();
            if (head[nb] == NULL)
                head[nb] = new atomic_ring_element<Z>(zero);
            _lock.unlock();
        }
    }
    
    long size() {
        unsigned short sz = (last-first);
        return (long)sz;
    }
    
    inline unsigned short tally() {
        return (last-first);
    }

    inline bool tallyfirst(unsigned short& p, unsigned short f, unsigned short l) {
        unsigned sz = l-f;
        if (!sz)
            return false;
        p = first++;
        return true;
    }
    
    inline bool tallylast(unsigned short& p, unsigned short f, unsigned short l) {
        unsigned sz = l-f;
        if (!sz)
            return false;
        p = --last;
        return true;
    }
    
    void clean() {
        _lock.lock();
        for (long i = 0; i < 256; i++) {
            if (head[i] != NULL)
                 head[i]->clean(zero);
        }
        _lock.unlock();
        last = 0;
        first = 0;
    }
    
    void clear() {
        _lock.lock();
        for (long i = 0; i < 256; i++) {
            if (head[i] != NULL)
                head[i]->clear(zero);
        }
        _lock.unlock();
        last = 0;
        first = 0;
    }
    
    bool compute(unsigned short pos, uchar& nb, uchar& i) {
        if (pos >= tally())
            return false;
        
        pos += first;

        nb = pos >> base_atomic_size;
        if (head[nb] == NULL)
            return false;
        
        i =  pos - (nb << base_atomic_size);
        
        return true;
    }
    
    inline Z replace(unsigned short pos, Z nv) {
        pos += first;
        
        uchar nb = pos >> base_atomic_size;
        uchar i =  pos - (nb << base_atomic_size);

        addnewelement(nb);
        
        return head[nb]->replace(nv, i);
    }
    
    inline Z remove(unsigned short pos) {
        pos += first;
        
        uchar nb = pos >> base_atomic_size;
        uchar i =  pos - (nb << base_atomic_size);
        
        _lock.lock();
        unsigned short mx = --last;
        
        uchar pnb,pi;

        while (pos != mx) {
            pnb = nb;
            pi =  i;
            ++pos;
            nb = pos >> base_atomic_size;
            i =  pos - (nb << base_atomic_size);
            head[pnb]->set(head[nb]->vecteur[i], pi);
        }
        _lock.unlock();
        
        return head[nb]->replace(zero, i);
    }
    
    inline Z erase(unsigned short pos) {
        if (pos >= tally())
            return zero;
        
        pos += first;
        
        uchar nb = pos >> base_atomic_size;
        uchar i =  pos - (nb << base_atomic_size);
        
        return head[nb]->replace(zero, i);
    }
    
    Z front() {
        unsigned short pos = first;
        
        uchar nb = pos >> base_atomic_size;
        if (head[nb] == NULL)
            return zero;
        
        uchar i =  pos - (nb << base_atomic_size);
        return head[nb]->vecteur[i];
    }
    
    Z back() {
        unsigned short pos = last-1;
        
        uchar nb = pos >> base_atomic_size;
        if (head[nb] == NULL)
            return zero;

        uchar i =  pos - (nb << base_atomic_size);
        return head[nb]->vecteur[i];
    }

    Z remove_front() {
        unsigned short pos = 0;
        if (!tallyfirst(pos, first, last))
            return zero;
        
        uchar nb = pos >> base_atomic_size;
        uchar i =  pos - (nb << base_atomic_size);
        
        return head[nb]->replace(zero, i);
    }
    
    Z remove_back() {
        unsigned short pos = 0;
        if (!tallylast(pos, first, last))
            return zero;

        uchar nb = pos >> base_atomic_size;
        uchar i =  pos - (nb << base_atomic_size);

        return head[nb]->replace(zero, i);
    }
    
    inline Z push_back(Z val) {
        unsigned short pos = last++;
        uchar nb = pos >> base_atomic_size;
        uchar i =  pos - (nb << base_atomic_size);

        addnewelement(nb);

        return head[nb]->replace(val, i);
    }
    
    inline Z push_front(Z val) {
        unsigned short pos = --first;
        uchar nb = pos >> base_atomic_size;
        uchar i =  pos - (nb << base_atomic_size);
        
        addnewelement(nb);

        return head[nb]->replace(val, i);
    }

    inline Z operator [](unsigned short pos) {
        pos += first;
        
        uchar nb = pos >> base_atomic_size;
        uchar i =  pos - (nb << base_atomic_size);
        if (head[nb] == NULL)
            return zero;
        
        return head[nb]->vecteur[i];
    }
    
    void copy(atomic_ring<Z>* a) {
        for (short i = 0; i < 256; i++) {
            if (a->head[i] == NULL)
                continue;

            if (head[i] == NULL)
                head[i] = new atomic_ring_element<Z>(zero);

            head[i]->duplicate(a->head[i]);
        }
        unsigned short pos = a->first;
        first = pos;
        pos = a->last;
        last = pos;
        zero = a->zero;
    }
    
    inline atomic_ring& operator=(atomic_ring<Z>& a) {
        copy(&a);
        return *this;
    }
    
    inline bool check(unsigned short pos, Z v) {
        pos += first;
        
        uchar nb = pos >> base_atomic_size;
        uchar i =  pos - (nb << base_atomic_size);
        
        if (head[nb] == NULL || head[nb]->vecteur[i] != v)
            return false;
        
        return true;
    }
    
    bool insert(unsigned short pos, Z val) {
        unsigned short sz = tally();
        if (sz == atomic_ring_size)
            return false;
        
        if (!pos)
            return push_front(val);
        
        if (pos >= sz)
            return push_back(val);
        
        pos += first;
        uchar nb = pos >> base_atomic_size;
        uchar i =  pos - (nb << base_atomic_size);
        atomic_ring_element<Z>* n = head[nb];

        _lock.lock();
        unsigned short poslast;
        uchar lastnb,lasti,pnb,pi;

        if ((short)pos < 0) {
            poslast = --first;
            while (poslast < pos) {
                lastnb = poslast >> base_atomic_size;
                lasti =  poslast - (lastnb << base_atomic_size);
                poslast++;
                pnb = poslast >> base_atomic_size;
                pi =  poslast - (pnb << base_atomic_size);
                if (head[lastnb] == NULL)
                    head[lastnb] = new atomic_ring_element<Z>(zero);
                head[lastnb]->set(head[pnb]->vecteur[pi], lasti);
            }
        }
        else {
            poslast = last++;
            while (poslast > pos) {
                lastnb = poslast >> base_atomic_size;
                lasti =  poslast - (lastnb << base_atomic_size);
                poslast--;
                pnb = poslast >> base_atomic_size;
                pi =  poslast - (pnb << base_atomic_size);
                if (head[lastnb] == NULL)
                    head[lastnb] = new atomic_ring_element<Z>(zero);
                head[lastnb]->set(head[pnb]->vecteur[pi], lasti);
            }
        }
        _lock.unlock();

        n->vecteur[i] = val;
        return true;
    }
    
};

template <class Z> class atomic_ring_iterator {
public:
    
    atomic_ring<Z>* av;
    Z second;
    
    unsigned short pos, first;
    uchar nb, i;
    bool ending;
    
    atomic_ring_iterator<Z>() {
        ending =  false;
        av =  NULL;
        pos = 0;
        next();
    }
    
    atomic_ring_iterator<Z>(atomic_ring<Z>& v) {
        ending =  false;
        pos = 0;
        av = &v;
        next();
    }
    
    atomic_ring_iterator<Z>(atomic_ring<Z>& v, long init) {
        ending =  false;
        pos = init;
        ending = !v.compute(pos, nb, i);
        av = &v;
        next();
    }
    
    void next() {
        if (!av->compute(pos, nb, i)) {
            ending  = true;
            return;
        }
        
        first = pos++;
        second = av->head[nb]->vecteur[i];
    }

    Z replace(Z v) {
        av->compute(first, nb, i);
        return av->head[nb]->replace(v, i);
    }
    
    bool end() {
        return ending;
    }
    

    void begin(atomic_ring<Z>& v) {
        ending =  false;
        pos = 0;
        av = &v;
        next();
    }

};

#endif




