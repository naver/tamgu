# -*- coding: utf-8 -*-

import sys

#La grammaire d'analyse du generateur a base de regle

sys.setrecursionlimit(100000)
debugmode=False
stopexecution=False
thelastpos=-1
def m_mot_0_1(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'mot_0_1',fx.lastchar
 label="mot"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Or
 subtree={}
 if  x_test_one(fx,lret,'.') or x_test_ord(fx,lret,'A','Z') or x_test_ord(fx,lret,'a','z') or x_test_sup(fx,lret) or x_test_one(fx,lret,'_'):
    if x_test_tree(subtree):
        tree['%NODES'].append(subtree)
 else:
   tree.pop('%NODES')
   fx.seek(pos)
   x_setfail(fx,label,pos,exitonfail)
   return False
 lreturn.extend(lret)
 return True


def m_mot_0_2_3(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'mot_0_2_3',fx.lastchar
 label="mot"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Or
 subtree={}
 if x_test_one(fx,lret,'.') or x_test_ord(fx,lret,'A','Z') or x_test_ord(fx,lret,'a','z') or x_test_ord(fx,lret,'0','9')  or x_test_sup(fx,lret) or x_test_one(fx,lret,'_'):
   if x_test_tree(subtree):
     tree['%NODES'].append(subtree)
 else:
   tree.pop('%NODES')
   fx.seek(pos)
   x_setfail(fx,label,pos,exitonfail)
   return False
 lreturn.extend(lret)
 return True


def m_mot_0_2(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'mot_0_2',fx.lastchar
 label="mot"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 while not fx.eof():
   subtree={}
   if m_mot_0_2_3(fx,lret,subtree,nb):
      if x_test_tree(subtree):
         tree['%NODES'].append(subtree)
   else:
      break
 lreturn.extend(lret)
 return True


def m_mot_0(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'mot_0',fx.lastchar
 label="mot"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if m_mot_0_1(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if m_mot_0_2(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 lreturn.extend(lret)
 return True


def m_mot(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'mot',fx.lastchar
 label="mot"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 tree['%NAME']='mot'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_mot_0(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_string_4(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'string_4',fx.lastchar
 label="string"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if x_test_one(fx,lret,'$'):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if m_mot_0_1(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if m_mot_0_2(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 lreturn.extend(lret)
 return True


def m_string(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'string',fx.lastchar
 label="string"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 tree['%NAME']='string'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_string_4(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True

def m_vstring_4(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'vstring_4',fx.lastchar
 label="vstring"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if x_test_one(fx,lret,':'):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if m_mot_0_1(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if m_mot_0_2(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 lreturn.extend(lret)
 return True


def m_vstring(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'vstring',fx.lastchar
 label="vstring"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 tree['%NAME']='vstring'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_vstring_4(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True

def m_digits_8(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'digits_8',fx.lastchar
 label="digits"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Loop
 subtree={}
 if x_test_ord(fx,lret,'0','9'):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 while not fx.eof():
   subtree={}
   if x_test_ord(fx,lret,'0','9'):
      if x_test_tree(subtree):
         tree['%NODES'].append(subtree)
   else:
      break
 lreturn.extend(lret)
 return True


def m_digits(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'digits',fx.lastchar
 label="digits"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 tree['%NAME']='digits'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_digits_8(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_chiffre_9(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'chiffre_9',fx.lastchar
 label="chiffre"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if x_test_one(fx,lret,'\\'):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if m_digits(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 lreturn.extend(lret)
 return True


def m_chiffre(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'chiffre',fx.lastchar
 label="chiffre"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 tree['%NAME']='chiffre'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_chiffre_9(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_unevariable_10(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'm_unevariable_10',fx.lastchar
 label="unevariable"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if x_test_one(fx,lret,'@'):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if x_test_any(fx,lret):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 lreturn.extend(lret)
 return True


def m_unevariable(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'm_unevariable',fx.lastchar
 label="unevariable"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 tree['%NAME']='unevariable'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_unevariable_10(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_symbole_10(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'symbole_10',fx.lastchar
 label="symbole"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if x_test_one(fx,lret,'%'):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if x_test_any(fx,lret):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 lreturn.extend(lret)
 return True


def m_symbole(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'symbole',fx.lastchar
 label="symbole"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 tree['%NAME']='symbole'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_symbole_10(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_point(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'point',fx.lastchar
 label="point"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 tree['%NAME']='point'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if x_test_one(fx,lret,'.'):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_interval_11(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'interval_11',fx.lastchar
 label="interval"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if m_mot_0_2_3(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if x_test_one(fx,lret,'-'):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if m_mot_0_2_3(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 lreturn.extend(lret)
 return True


def m_interval(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'interval',fx.lastchar
 label="interval"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 tree['%NAME']='interval'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_interval_11(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_token_14(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'token_14',fx.lastchar
 label="token"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Or
 subtree={}
 if m_negation(fx,lret,subtree,nb+1) or m_string(fx,lret,subtree,nb+1) or m_vstring(fx,lret,subtree,nb+1)  or m_point(fx,lret,subtree,nb+1) or m_interval(fx,lret,subtree,nb+1) or m_mot(fx,lret,subtree,nb+1) or m_chiffre(fx,lret,subtree,nb+1) or m_symbole(fx,lret,subtree,nb+1) or m_unevariable(fx,lret,subtree,nb+1):
   if x_test_tree(subtree):
     tree['%NODES'].append(subtree)
 else:
   tree.pop('%NODES')
   fx.seek(pos)
   x_setfail(fx,label,pos,exitonfail)
   return False
 lreturn.extend(lret)
 return True


def m_token(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'token',fx.lastchar
 label="token"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 tree['%NAME']='token'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_token_14(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_negation_15_16(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'negation_15_16',fx.lastchar
 label="negation"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Optional
 subtree={}
 if m_blanc(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
   lret=[]
 lreturn.extend(lret)
 return True


def m_negation_15_17(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'negation_15_17',fx.lastchar
 label="negation"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Or
 subtree={}
 if m_crochet(fx,lret,subtree,nb+1) or m_optionel(fx,lret,subtree,nb+1) or m_contexte(fx,lret,subtree,nb+1) or m_token(fx,lret,subtree,nb+1):
   if x_test_tree(subtree):
     tree['%NODES'].append(subtree)
 else:
   tree.pop('%NODES')
   fx.seek(pos)
   x_setfail(fx,label,pos,exitonfail)
   return False
 lreturn.extend(lret)
 return True


def m_negation_15(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'negation_15',fx.lastchar
 label="negation"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if x_test_one(fx,lret,'~'):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if m_negation_15_16(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if m_negation_15_17(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 lreturn.extend(lret)
 return True


def m_negation(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'negation',fx.lastchar
 label="negation"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 tree['%NAME']='negation'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_negation_15(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_kleene_18_19(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'kleene_18_19',fx.lastchar
 label="kleene"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Or
 subtree={}
 if x_test_in(fx,lret,['*','+']):
   if x_test_tree(subtree):
     tree['%NODES'].append(subtree)
 else:
   tree.pop('%NODES')
   fx.seek(pos)
   x_setfail(fx,label,pos,exitonfail)
   return False
 lreturn.extend(lret)
 return True


def m_kleene_18_20_21(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'kleene_18_20_21',fx.lastchar
 label="kleene"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if m_blanc(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if m_elementobligatoire(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 lreturn.extend(lret)
 return True


def m_kleene_18_20(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'kleene_18_20',fx.lastchar
 label="kleene"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Optional
 subtree={}
 if m_kleene_18_20_21(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
   lret=[]
 lreturn.extend(lret)
 return True


def m_kleene_18(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'kleene_18',fx.lastchar
 label="kleene"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if m_kleene_18_19(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if m_kleene_18_20(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 lreturn.extend(lret)
 return True


def m_kleene(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'kleene',fx.lastchar
 label="kleene"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 tree['%NAME']='kleene'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_kleene_18(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_unblanc_22(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'unblanc_22',fx.lastchar
 label="unblanc"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Loop
 subtree={}
 if x_test_one(fx,lret,' '):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 while not fx.eof():
   subtree={}
   if x_test_one(fx,lret,' '):
      if x_test_tree(subtree):
         tree['%NODES'].append(subtree)
   else:
      break
 lreturn.extend(lret)
 return True


def m_unblanc(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'unblanc',fx.lastchar
 label="unblanc"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 tree['%NAME']='unblanc'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_unblanc_22(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_optionel_23(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'optionel_23',fx.lastchar
 label="optionel"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if x_test_one(fx,lret,'('):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if m_liste(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if x_test_one(fx,lret,')'):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 lreturn.extend(lret)
 return True


def m_optionel(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'optionel',fx.lastchar
 label="optionel"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 tree['%NAME']='optionel'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_optionel_23(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_crochet_24(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'crochet_24',fx.lastchar
 label="crochet"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if x_test_one(fx,lret,'['):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if m_liste(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if x_test_one(fx,lret,']'):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 lreturn.extend(lret)
 return True


def m_crochet(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'crochet',fx.lastchar
 label="crochet"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 tree['%NAME']='crochet'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_crochet_24(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_contexte_25(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'contexte_25',fx.lastchar
 label="contexte"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if x_test_one(fx,lret,'{'):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if m_liste(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if x_test_one(fx,lret,'}'):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 lreturn.extend(lret)
 return True


def m_contexte(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'contexte',fx.lastchar
 label="contexte"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 tree['%NAME']='contexte'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_contexte_25(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_elementobligatoire_26(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'elementobligatoire_26',fx.lastchar
 label="elementobligatoire"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Or
 subtree={}
 if m_crochet(fx,lret,subtree,nb+1) or m_token(fx,lret,subtree,nb+1):
   if x_test_tree(subtree):
     tree['%NODES'].append(subtree)
 else:
   tree.pop('%NODES')
   fx.seek(pos)
   x_setfail(fx,label,pos,exitonfail)
   return False
 lreturn.extend(lret)
 return True


def m_elementobligatoire(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'elementobligatoire',fx.lastchar
 label="elementobligatoire"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 tree['%NAME']='elementobligatoire'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_elementobligatoire_26(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_elementbase(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'elementbase',fx.lastchar
 label="elementbase"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 tree['%NAME']='elementbase'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_negation_15_17(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_element_28_29(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'element_28_29',fx.lastchar
 label="element"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Optional
 subtree={}
 if m_kleene(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
   lret=[]
 lreturn.extend(lret)
 return True


def m_element_28(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'element_28',fx.lastchar
 label="element"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if m_elementbase(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if m_element_28_29(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 lreturn.extend(lret)
 return True


def m_element(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'element',fx.lastchar
 label="element"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 tree['%NAME']='element'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_element_28(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_coupe_30(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'coupe_30',fx.lastchar
 label="coupe"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if x_test_one(fx,lret,';'):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if m_digits(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 lreturn.extend(lret)
 return True


def m_coupe(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'coupe',fx.lastchar
 label="coupe"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 tree['%NAME']='coupe'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_coupe_30(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_blanc_31_32(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'blanc_31_32',fx.lastchar
 label="blanc"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Or
 subtree={}
 if x_test_one(fx,lret,' ') or m_coupe(fx,lret,subtree,nb+1):
   if x_test_tree(subtree):
     tree['%NODES'].append(subtree)
 else:
   tree.pop('%NODES')
   fx.seek(pos)
   x_setfail(fx,label,pos,exitonfail)
   return False
 lreturn.extend(lret)
 return True


def m_blanc_31(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'blanc_31',fx.lastchar
 label="blanc"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Loop
 subtree={}
 if m_blanc_31_32(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 while not fx.eof():
   subtree={}
   if m_blanc_31_32(fx,lret,subtree,nb):
      if x_test_tree(subtree):
         tree['%NODES'].append(subtree)
   else:
      break
 lreturn.extend(lret)
 return True


def m_blanc(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'blanc',fx.lastchar
 label="blanc"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 tree['%NAME']='blanc'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_blanc_31(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_exclusif_33(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'exclusif_33',fx.lastchar
 label="exclusif"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if x_test_one(fx,lret,'^'):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if m_negation_15_16(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if m_liste(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 lreturn.extend(lret)
 return True


def m_exclusif(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'exclusif',fx.lastchar
 label="exclusif"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 tree['%NAME']='exclusif'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_exclusif_33(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_union_35(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'union_35',fx.lastchar
 label="union"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if x_test_one(fx,lret,'|'):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if m_negation_15_16(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if m_liste(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 lreturn.extend(lret)
 return True


def m_union(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'union',fx.lastchar
 label="union"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 tree['%NAME']='union'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_union_35(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_coupechoix_37(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'coupechoix_37',fx.lastchar
 label="coupechoix"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if m_coupe(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if m_negation_15_16(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if m_liste(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 lreturn.extend(lret)
 return True


def m_coupechoix(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'coupechoix',fx.lastchar
 label="coupechoix"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 tree['%NAME']='coupechoix'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_coupechoix_37(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_sequence_39(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'sequence_39',fx.lastchar
 label="sequence"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if m_blanc(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if m_liste(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 lreturn.extend(lret)
 return True


def m_sequence(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'sequence',fx.lastchar
 label="sequence"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 tree['%NAME']='sequence'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_sequence_39(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_suivant_40(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'suivant_40',fx.lastchar
 label="suivant"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Or
 subtree={}
 if m_exclusif(fx,lret,subtree,nb+1) or m_union(fx,lret,subtree,nb+1) or m_sequence(fx,lret,subtree,nb+1):
   if x_test_tree(subtree):
     tree['%NODES'].append(subtree)
 else:
   tree.pop('%NODES')
   fx.seek(pos)
   x_setfail(fx,label,pos,exitonfail)
   return False
 lreturn.extend(lret)
 return True


def m_suivant(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'suivant',fx.lastchar
 label="suivant"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 tree['%NAME']='suivant'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_suivant_40(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_liste_41_42(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'liste_41_42',fx.lastchar
 label="liste"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Optional
 subtree={}
 if m_suivant(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
   lret=[]
 lreturn.extend(lret)
 return True


def m_liste_41(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'liste_41',fx.lastchar
 label="liste"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if m_element(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if m_liste_41_42(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 lreturn.extend(lret)
 return True


def m_liste(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'liste',fx.lastchar
 label="liste"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 tree['%NAME']='liste'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_liste_41(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True


def m_regle_43(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'regle_43',fx.lastchar
 label="regle"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 if '%NODES' not in tree:
    tree['%NODES']=[]
 # Sequence
 subtree={}
 if m_mot(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if m_negation_15_16(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if x_test_one(fx,lret,':'):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if x_test_one(fx,lret,'='):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if m_negation_15_16(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 # Sequence
 subtree={}
 if m_liste(fx,lret,subtree,nb+1):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    fx.seek(pos)
    x_setfail(fx,label,pos,exitonfail)
    return False
 lreturn.extend(lret)
 return True


def m_regle(fx,lreturn,tree,nb):
 if stopexecution:
    return False
 if debugmode:
    print 'regle',fx.lastchar
 label="regle"
 lret=[]
 exitonfail=False
 pos=fx.tell()
 tree['%NAME']='regle'
 tree['%NODES']=[]
 subtree={}
 nb+=1
 if m_regle_43(fx,lret,subtree,nb):
    if x_test_tree(subtree):
       tree['%NODES'].append(subtree)
 else:
    tree.pop('%NODES')
    tree.pop('%NAME')
    fx.seek(pos)
    return False
 tree['%VALUE']="".join(lret)
 tree['%OFFSETS']=[pos,fx.tell()]
 lreturn.extend(lret)
 return True



def x_comparelen(xl,yl):
    x=xl[0]
    y=yl[0]
    if x>y:
        return -1
    if x==y:
        return 0
    return 1


        
def x_test_string(fx,lret,sz,sub):
  pos=fx.tell()
  [lst,lstu]=fx.read(sz)
  if len(lst)<sz:
      fx.seek(pos)
      return False
  if lst==sub or lstu==sub:
      lret.append(lst)
      return True
  fx.seek(pos)
  return False

def x_test_in(fx,lret,sub):
  pos=fx.tell()
  [lst,lstu]=fx.read(1)
  if lst=="":
      fx.seek(pos)
      return False
  if lst in sub or lstu in sub:
      lret.append(lst)
      return True
  fx.seek(pos)
  return False

def x_test_one(fx,lret,onechar):
  pos=fx.tell()
  [lst,lstu]=fx.read(1)
  if lst=="":
      fx.seek(pos)
      return False
  if lst==onechar or lstu==onechar:      
      lret.append(lst)      
      return True
  fx.seek(pos)
  return False

def x_setfail(fx,label,pos,test):
    if test:
        stopexecution=True
        fx.lastposerror=pos
        fx.lastlabel=label



def x_test_ord(fx,lret,b,g):
  pos=fx.tell()
  [lst,lstu]=fx.read(1)  
  if lst=="":
     fx.seek(pos)
     return False
  if ord(lst[0])>=ord(b) and ord(lst[0])<=ord(g):
     lret.append(lst)
     return True
  fx.seek(pos)
  return False

def x_test_sup(fx,lret):
  pos=fx.tell()
  [lst,lstu]=fx.read(1)
  if lst=="":
     fx.seek(pos)
     return False
  #print "x_test_ord",lst[0]
  if ord(lst[0])>=128:
     lret.append(lst)
     return True
  fx.seek(pos)
  return False



def x_test_any(fx,lret):
  pos=fx.tell()
  [lst,lstu]=fx.read(1)
  if lst=="":
     fx.seek(pos)
     return False
  lret.append(lst)
  return True



def x_test_tree(sub):
    if sub=={}:
       return False
    if "%NODES" in sub and sub["%NODES"]==[]:
        if "%NAME" not in sub:
            return False
        else:
            sub.pop("%NODES")
    return True



def x_displayanalysis(tree,nb):
    space="".zfill(nb).replace("0"," ")
    if "%NAME" in tree:
        name=tree["%NAME"]
        value=""
        off=[]
        if "%VALUE" in tree:
            value=tree["%VALUE"]
            off=tree["%OFFSETS"]
        print space,name,off,'"'+value+'"'
    if "%NODES" in tree:
        lst=tree["%NODES"]
        for l in lst:
            x_displayanalysis(l,nb+1)

def x_maxoffset(tree,nb):
    if "%NAME" in tree:
        if "%OFFSETS" in tree:
            off=tree["%OFFSETS"]
            if off[1]>nb[0]:
                nb[0]=off[1]
    if "%NODES" in tree:
        lst=tree["%NODES"]
        for l in lst:
            x_maxoffset(l,nb)




class readstring:
    def __init__(self,s):
        self.parcours=s
        self.pos=0
        self.length=len(s)
        self.lastchar=""
        self.lastposerror=0
        self.lastlabel=""

    def seek(self,p):
        self.pos=p

    def tell(self):
        return self.pos

    def peek(self):
        return s[self.pos]

    def eof(self):
        if self.pos>=self.length:
            return True
        return False
        
    def read(self,sz):
        borne=self.pos+sz
        if borne>=self.length:
            borne=self.length
        if self.eof():
            return ["",""]
        s=self.parcours[self.pos:borne]
        self.lastchar=s
        self.pos=borne
        try:
            sutf8=unicode(s).encode("utf8")
            return [s,sutf8]
        except:
            return [s,s]

class readfile:
    def __init__(self,s):
        self.name=s
        self.fichier=open(s,"rb")
        #Jump to the end of the file
        self.fichier.seek(0,2)
        self.size=self.fichier.tell()
        #Return to the beginning of the file
        self.fichier.seek(0,0)
        self.pos=0
        self.lastchar=""
        self.lastposerror=0
        self.lastlabel=""

    def seek(self,p):
        self.pos=p
        self.fichier.seek(p)

    def tell(self):
        return self.pos

    def peek(self):
        p=self.tell()
        c=self.fichier.read(1)
        self.seek(p)
        return c

    def eof(self):
        if self.pos>=self.size:
            return True
        return False
        
    def read(self,sz):
        borne=self.pos+sz
        if self.eof():
            return ["",""]
        s=self.fichier.read(sz)
        self.lastchar=s
        self.pos=borne
        try:
            sutf8=unicode(s).encode("utf8")
            return [s,sutf8]
        except:
            return [s,s]

    def close(self):
        self.fichier.close()

def x_parse_string(x,f,full=True):
    stopexecution=False
    fx=readstring(x)
    fx.lastposerror=0
    lret=[]
    tree={}
    res=f(fx,lret,tree,0)
    if not fx.eof() and full:
        if fx.lastposerror==0:
            maxoff=[0]
            x_maxoffset(tree,maxoff)
            fx.seek(maxoff[0])
            print "Error:"
            print fx.read(300)[0]
        else:
            print x[:fx.lastposerror],"Error on:",fx.lastlabel
        return False
    if res==True:
       x_displayanalysis(tree,0)
       return True
    return False


def x_parse_file(x,f,full=True):
    stopexecution=False
    fx=readfile(x)
    fx.lastposerror=0
    lret=[]
    tree={}
    res=f(fx,lret,tree,0)
    fx.close()
    if not fx.eof() and full:
        if fx.lastposerror==0:
            maxoff=[0]
            x_maxoffset(tree,maxoff)
            fx.seek(maxoff[0])
            print "Error:"
            print fx.read(300)[0]
        else:
            fx.seek(fx.lastposerror)
            fx.read(100)[0],"Error on:",fx.lastlabel
            
        return False
    if res==True:
       x_displayanalysis(tree,0)
       return True
    return False

def applyrule(r):
    tree={}
    lreturn=[]
    s=readstring(r)
    r=m_regle(s,lreturn,tree,0)
    print tree
    return r



