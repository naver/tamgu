import rulen
import sys

debugmode=False

def tl(x):
    return (type(x)==type([]))
def td(x):
    return (type(x)==type({}))
        

def applyrule(r,tree):
    lreturn=[]
    s=rulen.readstring(r)
    return rulen.m_regle(s,lreturn,tree,0)


def explore(tree,nb,sub):
    if "%NODES" in tree:
        lst=tree["%NODES"]
        for l in lst:
            treeanalysis(l,nb,sub)

def insertion(struct,sub,operator):
    struct.insert(0,operator)
    if sub[0][0] in ["&","|","()","{}","^",";"]:
        if sub[0][0]==operator:
            struct.extend(sub[0][1:])
        else:
            struct.extend(sub)
    else:
        struct.extend(sub[0])
    


def treeanalysis(tree,nb,struct):
    #space="".zfill(nb).replace("0"," ")
    #Nous nous occupons que des categories suivantes: CROCHET, UNION, STRING, CHIFFRE, MOT
    if "%NAME" in tree:
        name=tree["%NAME"]
        value=tree["%VALUE"]
        #print name,value
        if name in ["string","point","interval","mot","digits","symbole"]:
            element={name:value}
            struct.append(element)
        elif name=="negation":
            #Constraint on applying the ANY operator on the next element
            sub=[]
            explore(tree,nb+1,sub)
            struct.append(["~",sub])
        elif name=="kleene":
            sub=[]
            explore(tree,nb+1,sub)
            if sub==[]:
                struct[-1]=[value,struct[-1]]
            else:
                struct[-1]=[value[0],struct[-1],sub]
        elif name=="liste":
            sub=[]
            explore(tree,nb+1,sub)
            struct.append(sub)
        elif name=="contexte":
            sub=[]
            explore(tree,nb+1,sub)
            sub.insert(0,"{}")
            struct.append(sub)
        elif name=="optionel":
            sub=[]
            explore(tree,nb+1,sub)
            sub.insert(0,"()")
            struct.append(sub)
        elif name=="union":
            sub=[]
            explore(tree,nb+1,sub)
            insertion(struct,sub,"|")
        elif name=="exclusif":
            sub=[]
            explore(tree,nb+1,sub)
            insertion(struct,sub,"^")
        elif name=="sequence":
            sub=[]
            explore(tree,nb+1,sub)
            insertion(struct,sub,"&")
        elif name=="coupechoix":
            sub=[]
            explore(tree,nb+1,sub)
            insertion(struct,sub,";")
        else:
            explore(tree,nb+1,struct)
    else:
        explore(tree,nb+1,struct)


stringrule="""        
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
"""
intervalrule="""
def x_test_ord(fx,lret,b,g):
  pos=fx.tell()
  [lst,lstu]=fx.read(1)
  if lst=="":
     fx.seek(pos)
     return False
  #print "x_test_ord",lst[0]
  if ord(lst[0])>=ord(b) and ord(lst[0])<=ord(g):
     lret.append(lst)
     return True
  fx.seek(pos)
  return False
"""

anyrule="""
def x_test_any(fx,lret):
  pos=fx.tell()
  [lst,lstu]=fx.read(1)
  if lst=="":
     fx.seek(pos)
     return False
  lret.append(lst)
  return True
"""


testrule="""

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
"""


comparerule="""
def x_comparelen(xl,yl):
    x=xl[0]
    y=yl[0]
    if x>y:
        return -1
    if x==y:
        return 0
    return 1
"""
displayrule="""
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
"""

testsubtree="""
def x_test_tree(sub):
    if sub=={}:
       return False
    if "%NODES" in sub and sub["%NODES"]==[]:
        if "%NAME" not in sub:
            return False
        else:
            sub.pop("%NODES")
    return True
"""

def displayanalysis(tree,nb):
    space="".zfill(nb).replace("0"," ")
    if "%NAME" in tree:
        name=tree["%NAME"]
        value=tree["%VALUE"]
        print space,name,value
    if "%NODES" in tree:
        lst=tree["%NODES"]
        for l in lst:
            displayanalysis(l,nb+1)

def createrule(name,s,nb):
    pssubfunc=name.find("_")
    if pssubfunc==-1:
        basename=name
    else:
        basename=name[:pssubfunc]
    
    newname=name
    if nb!=None:
        newname+="_"+str(nb[0])
        nb[0]+=1
    s.append("def m_"+newname+"(fx,lreturn,tree,nb):")
    s.append(" if stopexecution:")
    s.append("    return False") 
    s.append(" if debugmode:")
    s.append("    print '"+newname+"',fx.lastchar")
    s.append(" label=\""+basename+"\"")
    s.append(" lret=[]")
    s.append(" exitonfail=False")
    s.append(" pos=fx.tell()")
    if nb==None:
         s.append(" tree['%NAME']='"+newname+"'")
         s.append(" tree['%NODES']=[]")
    else:
        s.append(" if '%NODES' not in tree:")
        s.append("    tree['%NODES']=[]")
    return newname

def concatenaterule(elements):
    rule=[]
    concat=[]
    for r in elements:
        if tl(r[0]):
            subrule=analyserule(r)
            if subrule!=[]:
                rule.append(subrule)
        else:
            if r[0][0:2]=="X_":
                concat.append(r[0][2:])
            else:
                if concat!=[]:
                    if len(concat)==1:
                        nr='x_test_one(fx,lret,'+"".join(concat)+')'
                    else:
                        nr='x_test_in(fx,lret,['+",".join(concat)+'])'
                    rule.append(nr)
                concat=[]
                rule.append(r)
    if concat!=[]:
        if len(concat)==1:
            nr='x_test_one(fx,lret,'+"".join(concat)+')'
        else:
            nr='x_test_in(fx,lret,['+",".join(concat)+'])'
        rule.append(nr)
    return rule    

def replacerule(elements,rule):
    for r in elements:
        if tl(r):
            rl=[]
            replacerule(r,rl)
            rule.append(rl)
        else:
            if r[0:2]=="X_":
                nr='x_test_one(fx,lret,'+r[2:]+')'
                rule.append(nr)
            else:
                rule.append(r)
        

def bodysequence(rule,subdef):
    if tl(rule):
        for r in rule:
            bodysequence(r,subdef)
    else:
        subdef.append(" # Sequence")
        subdef.append(" subtree={}")
        subdef.append(" if "+rule+":")
        subdef.append("    if x_test_tree(subtree):")
        subdef.append("       tree['%NODES'].append(subtree)")
        subdef.append(" else:")
        subdef.append("    tree.pop('%NODES')")
        subdef.append("    fx.seek(pos)")
        subdef.append("    x_setfail(fx,label,pos,exitonfail)")
        subdef.append("    return False")

def bodyoptional(rule,subdef):
    if tl(rule):
        for r in rule:
            bodyoptional(r,subdef)
    else:
        subdef.append(" # Optional")
        subdef.append(" subtree={}")
        subdef.append(" if "+rule+":")
        subdef.append("    if x_test_tree(subtree):")
        subdef.append("       tree['%NODES'].append(subtree)")
        subdef.append(" else:")
        subdef.append("   lret=[]")

def bodycontextual(rule,subdef):
    if tl(rule):
        for r in rule:
            bodycontextual(r,subdef)
    else:
        subdef.append(" # Contextual. Test next nodes, then return to current position")
        subdef.append(" poscontext=fx.tell()")
        ru=rule
        if ",subtree," in ru:
            subdef.append(" subtreebis={}")
            ru=ru.replace(",subtree,",",subtreebis,")
        subdef.append(" lretbis=[]")
        ru=ru.replace(",lret,",",lretbis,")
        subdef.append(" if "+ru+":")
        subdef.append("    fx.seek(poscontext)")
        subdef.append(" else:")
        subdef.append("    tree.pop('%NODES')")
        subdef.append("    fx.seek(pos)")
        subdef.append("    x_setfail(fx,label,pos,exitonfail)")
        subdef.append("    return False")
        
def bodynegation(rule,subdef):
    if tl(rule):
        for r in rule:
            bodyoptional(r,subdef)
    else:
        subdef.append(" # Negation")
        subdef.append(" subtree={}")
        subdef.append(" if not "+rule+":")
        subdef.append("    if x_test_tree(subtree):")
        subdef.append("       tree['%NODES'].append(subtree)")
        subdef.append(" else:")
        subdef.append("   tree.pop('%NODES')")
        subdef.append("   fx.seek(pos)")
        subdef.append("   x_setfail(fx,label,pos,exitonfail)")
        subdef.append("   return False")
        

def bodyor_old(rule,s,subdef,first):
    if tl(rule):
        for r in rule:
            bodyor_old(r,s,subdef,False)
    else:
        if s[0]=="":
            s[0]+=" if "+rule            
        else:
            s[0]+=" or "+rule
    if first:
        s[0]+=":"
        subdef.append(" # Or")
        subdef.append(" subtree={}")
        subdef.append(s[0])
        subdef.append("   if x_test_tree(subtree):")
        subdef.append("     tree['%NODES'].append(subtree)")
        subdef.append(" else:")
        subdef.append("   tree.pop('%NODES')")
        subdef.append("   fx.seek(pos)")
        subdef.append("   x_setfail(fx,label,pos,exitonfail)")
        subdef.append("   return False")

def bodyorend(subdef):
    subdef.append(" if lsts==[]:")
    subdef.append("   fx.seek(pos)")
    subdef.append("   x_setfail(fx,label,pos,exitonfail)")
    subdef.append("   return False")
    subdef.append(" lsts.sort(x_comparelen)")
    subdef.append(" if x_test_tree(subtree):")
    subdef.append("    tree['%NODES'].append(lsts[0][1])")
    subdef.append(" fx.seek(lsts[0][0])")
    subdef.append(" lret=lsts[0][2]")
    
def bodyorrec(rule,subdef,first):
    if tl(rule):
        for r in rule:
            bodyorrec(r,subdef,first)
    else:
        if first[0]==True:
            subdef.append(" lsts=[]")
        first[0]=False
        subdef.append(" subtree={}")
        subdef.append(" lret=[]")
        subdef.append(" fx.seek(pos)")
        subdef.append(" if "+rule+":")
        subdef.append("    lsts.append([fx.tell(),subtree,lret])")
        
def bodyevaluate(rule):
    for r in rule:
        if tl(r):
            rs=bodyevaluate(r)
            if rs==False:
                return rs
        elif r[0:7]!='x_test_':
            return False
    return True
    
def bodyor(rule,subdef,select):
    if bodyevaluate(rule) or select:
        s=[""]
        bodyor_old(rule,s,subdef,True)
    else:
        first=[True]
        bodyorrec(rule,subdef,first)
        bodyorend(subdef)

def bodywhile(rule,subdef,operator):
    if tl(rule):
        bodywhile(rule[0],subdef,operator)
        return
        
    if operator=="+":
        subdef.append(" # Loop")
        subdef.append(" subtree={}")
        subdef.append(" if "+rule+":")
        subdef.append("    if x_test_tree(subtree):")
        subdef.append("       tree['%NODES'].append(subtree)")
        subdef.append(" else:")
        subdef.append("    tree.pop('%NODES')")        
        subdef.append("    fx.seek(pos)")
        subdef.append("    x_setfail(fx,label,pos,exitonfail)")
        subdef.append("    return False")
    subdef.append(" while not fx.eof():")
    subdef.append("   subtree={}")
    subdef.append("   if "+rule+":")
    subdef.append("      if x_test_tree(subtree):")
    subdef.append("         tree['%NODES'].append(subtree)")
    subdef.append("   else:")
    subdef.append("      break")

def bodyconstraint(const,subdef,operator):
    rule=const[0][0]
    constraint=const[1][0]

    if operator=="+":
        subdef.append(" # LoopConstraint")
        subdef.append(" subtree={}")
        subdef.append(" if "+constraint+":")
        subdef.append("     tree.pop('%NODES')")
        subdef.append("     fx.seek(pos)")
        subdef.append("     x_setfail(fx,label,pos,exitonfail)")
        subdef.append("     return False")
        subdef.append(" lret=[]")
        subdef.append(" subtree={}")
        subdef.append(" if "+rule+":")
        subdef.append("    if x_test_tree(subtree):")
        subdef.append("       tree['%NODES'].append(subtree)")
        subdef.append(" else:")
        subdef.append("    tree.pop('%NODES')")        
        subdef.append("    fx.seek(pos)")
        subdef.append("    x_setfail(fx,label,pos,exitonfail)")
        subdef.append("    return False")
    subdef.append(" foundsolution=False")
    subdef.append(" while not fx.eof():")
    subdef.append("   subtree={}")
    subdef.append("   if "+constraint+":")
    subdef.append("      foundsolution=True")
    subdef.append("      if x_test_tree(subtree):")
    subdef.append("         tree['%NODES'].append(subtree)")
    subdef.append("      break")
    subdef.append("   subtree={}")
    subdef.append("   if "+rule+":")
    subdef.append("      if x_test_tree(subtree):")
    subdef.append("         tree['%NODES'].append(subtree)")
    subdef.append("   else:")
    subdef.append("      break")
    subdef.append("   subtree={}")
    subdef.append(" if not foundsolution:")
    subdef.append("    tree.pop('%NODES')")
    subdef.append("    fx.seek(pos)")
    subdef.append("    x_setfail(fx,label,pos,exitonfail)")
    subdef.append("    return False")

        
def generate(name,ruletree,result,functions,nb,current,dicorules):
    if tl(ruletree):
        operator=ruletree[0]
        if operator in ["&","|","()","{}","+","*","~","^",";"]:
            subdef=[]
            newname=createrule(name,subdef,nb)
            if operator=="~":
                result.append("not m_"+newname+"(fx,lret,subtree,nb)")
            else:
                result.append("m_"+newname+"(fx,lret,subtree,nb)")
            baselst=[]
            pointrule=None
            #Concatenation of element
            if operator=="@":
                #The first element is a test on POINT
                [therule]=ruletree[1:]
                pointrule=therule[0]
                nexts=therule[1:]
            else:
                nexts=ruletree[1:]
            for r in nexts:
                newdef=[]
                generate(newname,r,newdef,functions,nb,operator,dicorules)
                baselst.append(newdef)

            lst=[]
            if operator=="~":
                replacerule(baselst,lst)
                bodynegation(lst,subdef)
            if operator=="&":
                replacerule(baselst,lst)
                bodysequence(lst,subdef)
            if operator==";":
                replacerule(baselst,lst)
                bodysequence(lst,subdef)
                subdef.append(" exitonfail=True")
            elif operator=="|":
                lst=concatenaterule(baselst)
                bodyor(lst,subdef,False)                
            elif operator=="^":
                lst=concatenaterule(baselst)
                bodyor(lst,subdef,True)                
            elif operator=="()":
                replacerule(baselst,lst)
                bodyoptional(lst,subdef)
            elif operator=="{}":
                replacerule(baselst,lst)
                bodycontextual(lst,subdef)
            elif operator in ["+","*"]:
                replacerule(baselst,lst)
                if len(lst)==1:
                    bodywhile(lst,subdef,operator)
                else:
                    bodyconstraint(lst,subdef,operator)
                
            subdef.append(" lreturn.extend(lret)")
            subdef.append(" return True")
            keyfunc="".join(subdef[7:])
            try:
                myname=dicorules[keyfunc]
                result[-1]=result[-1].replace("_"+newname,"_"+myname)                
            except:                
                dicorules[keyfunc]=newname
                functions.append(subdef)                    
        else:
            for rule in ruletree:
                generate(name,rule,result,functions,nb,current,dicorules)
    else:
        if td(ruletree):            
            if "symbole" in ruletree:
                sym=ruletree["symbole"][1:]
                if sym=="\\":
                    sym+="\\"
                if sym=='"':
                    result.append("X_chr(34)")
                elif sym=="'":
                    result.append("X_chr(39)")
                else:
                    result.append("X_'"+sym+"'")
            elif "string" in ruletree:
                lx=len(ruletree["string"][1:])
                sb=ruletree["string"][1:]
                if lx==1:
                    if sb=='"':
                        result.append("X_chr(34)")
                    elif sb=="'":
                        result.append("X_chr(39)")
                    else:
                        result.append("X_'"+sb+"'")
                else:
                    result.append("x_test_string(fx,lret,"+str(lx)+",'"+ruletree["string"][1:]+"')")
            elif "point" in ruletree:
                result.append("x_test_any(fx,lret)")
            elif "digits" in ruletree:
                result.append("X_chr("+ruletree["digits"]+")")
            elif "mot" in ruletree:
                result.append("m_"+ruletree["mot"]+"(fx,lret,subtree,nb+1)")
            elif "interval" in ruletree:
                s=ruletree["interval"].split("-")
                result.append("x_test_ord(fx,lret,'"+s[0]+"','"+s[1]+"')")
        else:
            print
            print "Erreur",ruletree
            print
        
                
                              
def simplify(r,bodies):
    if tl(r):
        if r[0] in ["|","()","{}","&","+","*","^"]:
            b=[r[0]]
            for i in r[1:]:
                simplify(i,b)
            bodies.append(b)
        else:
            if tl(r) and len(r)==1:
                bodies.append(r[0])
            else:
                bodies.append(r)
    else:
        bodies.append(r)
            

def cleanrules(rules,simple):
    for r in rules:
        s=[r[0],r[1]]
        simplify(r[2],s)
        simple.append(s)
            
def getrecursivefirst(r,lst):
    if tl(r):
        if r[0] in ["|","()","{}","&","+","*","^"]:
            for i in r[1:]:
                getrecursivefirst(i,lst)
    else:
        if "mot" not in r:
            lst.append(r)

def invertedindex(bodies,idx):
    for b in bodies:
        name=b[1]["mot"]
        lst=[]
        getrecursivefirst(b[2],lst)
        print "I",lst
        
def compiling(filename):
    global stringrule
    f=open(filename,"r")
    erreur=[]
    rules=[]
    comments=[]
    kept=[]
    removed=[]
    for e in f:
        l=e.strip()
        if l=="":
            continue
        if l[0]=='#':
            comments.append(l)
        elif l[0]=="+":
            kpt=l[1:].strip().split(",")
            for k in kpt:
                kept.append(k.strip())
        elif l[0]=="!":
            kpt=l[1:].strip().split(",")
            for k in kpt:
                removed.append(k.strip())
        else:
            print l
            tree={}
            if not applyrule(l,tree):
                erreur.append(l)
            else:
                struct=[{"%":l}]
                treeanalysis(tree,0,struct)
                #displayanalysis(tree,0)
                rules.append(struct)

    for x in erreur:
        print "Error:",x

    bodies=[]
    cleanrules(rules,bodies)

    #idx={}
    #invertedindex(bodies,idx)    
    #print idx

    nb=[0]
    filenew=filename.replace(".txt","n.py")
    foutput=open(filenew,"w")
    foutput.write("# -*- coding: utf-8 -*-\n\n")
    foutput.write("import sys\n\n")
    for c in comments:
        foutput.write(c+"\n")
    foutput.write("\nsys.setrecursionlimit(100000)\n")    
    foutput.write("debugmode=False\n")
    foutput.write("stopexecution=False\n")
    foutput.write("thelastpos=-1\n")
    dicorules={}
    for r in bodies:
        #The first element is the rule name
        functions=[]
        s=[]
        name=r[1]["mot"]
        createrule(name,s,None)
        generate(name,r[2:],s,functions,nb,None,dicorules)
        xxs=s[-1]
        s=s[:-1]
        xbase=[]
        replacerule([xxs],xbase)
        xs=xbase[0]
        s.append(" subtree={}")
        s.append(" nb+=1")
        s.append(" if "+xs+":")
        s.append("    if x_test_tree(subtree):")
        s.append("       tree['%NODES'].append(subtree)")
        s.append(" else:")
        s.append("    tree.pop('%NODES')")
        s.append("    tree.pop('%NAME')")
        s.append("    fx.seek(pos)")
        s.append("    return False")
        if name not in removed:
            s.append(" tree['%VALUE']=\"\".join(lret)")
            s.append(" tree['%OFFSETS']=[pos,fx.tell()]")
            s.append(" lreturn.extend(lret)")
        else:
            s.append(" tree.pop('%NODES')")
            s.append(" tree.pop('%NAME')")
        s.append(" return True")
        functions.append(s)
        for fc in functions:
            for fct in fc:
                foutput.write(fct+"\n")
            foutput.write("\n\n")
    foutput.write(comparerule)
    foutput.write("\n\n")
    foutput.write(stringrule)
    foutput.write("\n\n")
    foutput.write(intervalrule)
    foutput.write("\n\n")
    foutput.write(anyrule)    
    foutput.write("\n\n")
    foutput.write(testsubtree)    
    foutput.write("\n\n")
    foutput.write(displayrule)
    foutput.write("\n\n")
    foutput.write(testrule)
    foutput.write("\n\n")
    foutput.close()
    f.close()

try:
    chemin=sys.argv[1]
except:
    chemin=r"C:\XIP\GrammairesBNF\xml.txt"
compiling(chemin)
