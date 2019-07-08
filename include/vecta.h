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

template <class Z> class VECTA {
 public:
     
     //Un vecteur de Fonction
     Z* vecteur;
     
     //taille est la taille actuelle de la liste
     long taille;
     
     Z zero;
     
     //dernier element entre... Pour gerer les ajouts en fin de liste...
     long dernier;
     
     VECTA(long t=5,Z z=(Z)0) {
         zero=z;
         vecteur=NULL;
         if (t>0) {
             vecteur=new Z[t];
             for (dernier=0;dernier<t;dernier++)
                 vecteur[dernier]=zero;
         }
         taille=t;
         dernier=0;
     }
     
     ~VECTA() {
         delete[] vecteur;
     }
     
     
     inline void raz() {
         dernier=0;
         if (vecteur==NULL)
             return;
         memset(vecteur,0,taille*sizeof(Z));
         //for (long i=0;i<taille;i++)
         //    vecteur[i]=zero;
     }

	 inline void met(long z) {
         dernier=0;
         if (vecteur==NULL)
             return;
         memset(vecteur,z,taille*sizeof(Z));
         //for (long i=0;i<taille;i++)
         //    vecteur[i]=zero;
     }
     
     
     inline void nettoie() {
         if (vecteur==NULL)
             return;
         for (long i=0;i<taille;i++) {
             if (vecteur[i]!=zero)
                 delete vecteur[i];
             vecteur[i]=zero;
         }
         dernier=0;
     }
     
      inline void nettoietable() {
         if (vecteur==NULL)
             return;
         for (long i=0;i<taille;i++) {
             if (vecteur[i]!=zero)
                 delete[] vecteur[i];
             vecteur[i]=zero;
         }
         dernier=0;
     }

    void nulle(Z v) {
         zero=v;
     }
     
	 void reserve(long t) {
		 delete[] vecteur;
		 taille=t;
		 vecteur=new Z[t];
		 memset(vecteur,0,taille*sizeof(Z));
	 }

     void ajuste(long t) {
         Z* tfs;    
         
         if (t<=taille)
             return;
         //on realloue par bloc de t
         tfs=new Z[t];
         
         //for (i=0;i<dernier;i++)
         //  tfs[i]=vecteur[i];
         memcpy(tfs,vecteur,sizeof(Z)*dernier);
         
         //for (i=dernier;i<t;i++)
         //  tfs[i]=zero;
         
         memset(&tfs[dernier],0,sizeof(Z)*(t-dernier));
         
         delete[] vecteur;
         vecteur=tfs;
         taille=t;
     }
     
     void resize(long t) {
         Z* tfs;    
         
         if (t<=taille)
             return;
         //on realloue par bloc de t
         tfs=new Z[t];
         
         memcpy(tfs,vecteur,sizeof(Z)*dernier);         
         memset(&tfs[dernier],0,sizeof(Z)*(t-dernier));
         
         delete[] vecteur;
         vecteur=tfs;
         taille=t;
     }

     void aupluspres() {
         if (dernier==taille)
             return;
         Z* tfs;    
         
         //on realloue par bloc de t
         tfs=new Z[dernier];
         
         memcpy(tfs,vecteur,sizeof(Z)*dernier);
         
         delete[] vecteur;
         vecteur=tfs;
         taille=dernier;     
     }

     void reduit(long t) {        
         if (t>=taille)
             return;
         nettoie();
         delete[] vecteur;
         //on realloue par bloc de t
         vecteur=new Z[t];
         taille=t;
     }


     inline Z retire() {
         if (dernier==0)
             return zero;
         dernier--;
         Z v=vecteur[dernier];
         vecteur[dernier]=zero;
         return v;
     }
     
     inline void pop_back() {
         if (dernier==0)
             return;
         dernier--;         
         vecteur[dernier]=zero;
     }
     
     
     inline Z retireElement(long i=-1) {        
         if (dernier==0)
             return zero;
         
         long pos=i;
         
         if (i==-1)
             pos=dernier-1;
         
         Z v=vecteur[pos];
         vecteur[pos]=zero;
         
         //On deplace toutes les cases de 1...
         //if (i!=-1)
         //  memcpy(vecteur+i,vecteur+i+1,sizeof(Z)*(dernier-i-1));
         
         if (i!=-1) {
             for (long k=i;k<dernier-1;k++)
                 vecteur[k]=vecteur[k+1];
             if (dernier>0)
                 vecteur[dernier-1]=zero;
         }
         
         dernier--;
         return v;
     }
     
	 inline Z remove(long pos = -1) {
		 Z v;
		 if (pos < 0) {
			 if (dernier == 0)
				 return zero;
			 dernier--;
			 v = vecteur[dernier];
			 vecteur[dernier] = zero;
			 return v;
		 }

		 if (pos >= dernier)
			 return zero;
		 v = vecteur[pos];
		 //On deplace toutes les cases de 1...
		 dernier--;
		 for (; pos < dernier; pos++)
			 vecteur[pos] = vecteur[pos + 1];
		 vecteur[dernier] = zero;
		 return v;
	 }

     inline void erase(size_t pos) {        
         if (pos>=dernier)
             return;

         //On deplace toutes les cases de 1...
		 dernier--;
		 for (;pos<dernier;pos++)
			 vecteur[pos]=vecteur[pos+1];		 
		 vecteur[dernier]=zero;                  
     }

	 inline void insert(long pos,Z val) {
		 if (pos<0)
			 return;

		 if (dernier>=taille)
			 ajuste(taille+10);

		 //Dans ce cas, c'est un simple push
		 if (pos>=dernier) {
			 vecteur[pos]=val;
			 dernier++;
			 return;
		 }

		 //on ajoute alors l'element a sa place
		 //si la case est vide on le place a cet endroit
		 //sinon on effectue un deplacement de tous les elements vers la droite
		 if (vecteur[pos]!=NULL) {                          
			 //sinon, on deplace tous les elements d'une case vers la droite
			 for (long i=dernier-1;i>=pos;i--)
				 vecteur[i+1]=vecteur[i];
			 vecteur[pos]=val;
			 dernier++;
		 }
		 else
			 vecteur[pos]=val;
	 }

     inline Z fin() {
         if (dernier==0)
             return zero;
         return vecteur[dernier-1];
     }
     
     inline char detruit(long i) {   
         if (vecteur==NULL || i<0 || i >= dernier || vecteur[i]==zero)
             return 0;
         
         delete vecteur[i];
         vecteur[i]=zero;
         if (i==dernier-1)
             dernier--;
         return 1;
     }
     
     
     inline long ajoute(Z val,long inc=10) {
         
         if (dernier >= taille)
             ajuste(taille+inc);
         
         //sinon on ajoute l'element en queue...
         vecteur[dernier]=val;
         dernier++;
         return dernier-1;
     }

     inline void ajoute(VECTE<Z>& z) {
         long ta=z.dernier+dernier;
         if (ta > taille)
             ajuste(ta+3);

         for (long i=0;i<z.dernier;i++)
             ajoute(z.vecteur[i]);
     }
     
     
     inline long push_back(Z val,long inc=10) {
         
         if (dernier >= taille)
             ajuste(taille+inc);
         
         //sinon on ajoute l'element en queue...
         vecteur[dernier]=val;
         dernier++;
         return dernier-1;
     }
     
     
	 void operator =(VECTE<Z>& z) {
		 dernier=z.dernier;
		 if (dernier>=taille) {
			 delete[] vecteur;
			 taille=dernier+3;
			 vecteur=new Z[taille];
		 }

		 memset(vecteur,0,taille);
		 memcpy(vecteur,z.vecteur,sizeof(Z)*dernier);		 
	 }

     inline Z operator [](long i) {
         if (vecteur==NULL || i<0 || i >= dernier)
             return zero;
         
         return vecteur[i];
     } 

     inline void affecte(VECTE<Z>& z) {
         dernier=0;         
         ajoute(z);
     }

	 inline void affecte(VECTE<Z>* z) {
		 if (z==NULL)
			 return;
         dernier=0;         
         ajoute(*z);
     }
     
     
     inline Z cell(long pos) {
         if (vecteur==NULL || pos<0 || pos>=taille)
             return zero;
         
         return vecteur[pos];
     }
     
     inline Z cell(unsigned long pos) {
         if (vecteur==NULL || pos<0 || pos>=taille)
             return zero;
         
         return vecteur[pos];
     }
     
     inline Z affecte(long pos,Z val) {
         if (pos<0)
             return zero;
         
         if (pos >=taille)
             ajuste(pos+10);
         
         //on ajoute alors l'element a sa place
         vecteur[pos]=val;
         if (pos >=dernier)
             dernier=pos+1;
         return vecteur[pos];
     }
     
	 inline void annule(long pos) {
         if (pos<0)
             return;
         
         if (pos >=dernier)
			 return;
		 
		 vecteur[pos]=zero;
		 
		 if (pos==dernier-1)
			 dernier=pos;
	 }

     inline Z insere(long pos,Z val,long inc=10) {
         if (pos<0)
             return zero;
         
         if (pos>=taille)
             ajuste(pos+inc);

         //on ajoute alors l'element a sa place
         //si la case est vide on le place a cet endroit
         //sinon on effectue un deplacement de tous les elements vers la droite
         if (vecteur[pos]!=NULL) {
             
             if (dernier >=taille)
                 ajuste(taille+10);
             
             //sinon, on deplace tous les elements d'une case vers la droite
             for (long i=dernier-1;i>=pos;i--)
                 vecteur[i+1]=vecteur[i];
             dernier++;
         }
         
         vecteur[pos]=val;
         if (pos==dernier)
             dernier++;         
         return vecteur[pos];
     }

     //Autre insertion, forcement en pos avec deplacement vers la droite des valeurs
     inline Z insertion(long pos,Z val) {
         if (pos<0)
             return zero;
         
         //si l'element est en queue, on l'ajoute a sa position
         if (pos>=dernier) {
             affecte(pos,val);
             return vecteur[pos];
         }

         //on ajoute alors l'element a sa place         
         //sinon on effectue un deplacement de tous les elements vers la droite

         if (dernier >=taille)
             ajuste(taille+10);
         
         //sinon, on deplace tous les elements d'une case vers la droite
         for (long i=dernier-1;i>=pos;i--)
             vecteur[i+1]=vecteur[i];
         
         vecteur[pos]=val;
         dernier++;

         return vecteur[pos];
     }

     long size() {
         return dernier;
     } 
     
     void Clear() {
         nettoie();
     }
     
	 void clear() {
		 dernier = 0;
		 if (vecteur == NULL)
			 return;
		 memset(vecteur, 0, taille*sizeof(Z));
	 }

     void asseche() {
         if (vecteur!=NULL)
             delete[] vecteur;
         vecteur=NULL;
         dernier=0;
         taille=0;
     }

     inline long cherche(Z v) {
         for (long i=0;i<dernier;i++)
             if (vecteur[i]==v)
                 return i;
        return -1;
     }
};


#endif







