/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : codeparse.h
 Date       : 2017/09/01
 Purpose    : This file parses a Tamgu program... 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#ifndef bnf_tamgu_h
#define bnf_tamgu_h
#include "x_node.h"


class bnf_tamgu {
    public:


    long currentpos;
    int intoken;
    long lineerror;
    string labelerror;
    int errornumber;
    char gFail;
    long baseline;
    x_reading* fx;

    vector<string> labelerrors;
    vector<int> lineerrors;
    vector<int> errornumbers;

    bnf_tamgu(int l=0) {
        baseline=l;
        fx=NULL;
        intoken=0;
        currentpos=0;
        Y_var_1=')';
        Y_var_0='(';
        VS_var_3="CLOSE";
        VS_var_2="OPEN";

    }

    void initialize(x_reading* xr) {
        labelerrors.clear();
        lineerrors.clear();
        errornumbers.clear();

        baseline=0;
        fx=xr;
        gFail=0;
        intoken=0;
        currentpos=0;
    }

    void setfail(char test) {
        if (test)
            gFail=1;
    }

    void initfail(string lab,long pos,int error) {
        if (gFail==1)
            return;
        if (pos>=fx->stackln.size()) {
            if (fx->stackln.size() > 0)
                lineerror = fx->stackln.back()+baseline;
            else
                lineerror=-1;
            labelerror=lab;
            errornumber=error;
            return;
        }
        lineerror=fx->stackln[pos]+baseline;
        labelerror=lab;
        errornumber=error;
        labelerrors.push_back(labelerror);
        lineerrors.push_back(lineerror);
        errornumbers.push_back(errornumber);
    }

    void poperrors() {
        if (!gFail) {
            labelerrors.pop_back();
            lineerrors.pop_back();
            errornumbers.pop_back();
        }
    }


    inline void x_pop_node(x_node** tree,int addsubtree) {
        if (*tree!=NULL) {
            if ((*tree)->init==0) {
                delete *tree;
                *tree=NULL;
            }
            else
                (*tree)->pop(addsubtree);
        }
    }

    inline void x_init_node(string& lreturn,string& lret,x_node** tree,string name,long d,bool conc) {
        if (lret.size()>0 || conc==false) {
            if (*tree==NULL)
                *tree=new x_node;
            (*tree)->start=d;
            (*tree)->end=d;
            (*tree)->token=name;
            (*tree)->init=1;
            if (conc) {
                (*tree)->value=lret;
                lreturn+=lret;
            }
        }
    }


    inline void x_init_tree(x_node** tree,x_node* subtree,int& addsubtree) {
        if (subtree==NULL)
            return;
        if (*tree==NULL)
            *tree=new x_node;
        if ((*tree)->append(subtree)==0)
            delete subtree;
        else
            addsubtree++;
    }

    char x_test_dot(string& lret);
    char x_test_string(string& lret,string sub);

    void incrementintoken() {
        intoken++;
        if (intoken==fx->stack[currentpos].size()) {
            currentpos++;
            intoken=0;
        }
    }

    void incrementpos() {
        currentpos++;
        intoken=0;

    }


    inline char x_test_char(string& lret,unsigned char sub) {
        if (currentpos>=fx->stack.size())
            return(0);
        unsigned char x=fx->stack[currentpos][intoken];
        if (x==sub) {
            incrementintoken();
            lret+=x;
            return(1);
        }
        return(0);
    }


    inline char x_test_in(string& lret,char* sub) {
        if (currentpos>=fx->stack.size())
            return(0);
        char x=fx->stack[currentpos][intoken];
        if (strchr(sub,x)!=NULL) {
            lret+=x;
            incrementintoken();
            return(1);
        }
        return(0);
    }


    inline char x_test_ord(string& lret,unsigned char b,unsigned char g) {
        if (currentpos>=fx->stack.size())
            return(0);
        unsigned char x=fx->stack[currentpos][intoken];
        if (x>=b && x<=g) {
            incrementintoken();
            lret+=x;
            return(1);
        }
        return(0);
    }

    inline char x_test_any(string& lret) {
        if (currentpos>=fx->stack.size())
            return(0);
        char x=fx->stack[currentpos][intoken];
        incrementintoken();
        lret+=x;
        return(1);
    }


    char m_EOF(string& lreturn,x_node** tree) {
        if (currentpos>=fx->stack.size())
            return(1);
        return(0);
    }

    long x_linenumber(long p) {
        if (p<fx->stackln.size())
            return (fx->stackln[p]+baseline);
        return -1;
    }

    string x_errormsg(long i);
    x_node* x_parsing(x_reading* xr,x_parsing_mode mode,bool display=true);



    char m_path(string& lreturn,x_node** tree);
    char m_path_0_1(string& lreturn,x_node** tree);
    char m_shell(string& lreturn,x_node** tree);
    char m_shell_0_1(string& lreturn,x_node** tree);
    char m_signature(string& lreturn,x_node** tree);
    char m_operator(string& lreturn,x_node** tree);
    char m_operator_0_1(string& lreturn,x_node** tree);
    char m_operator_0_2(string& lreturn,x_node** tree);
    char m_operator_0_3(string& lreturn,x_node** tree);
    char m_operator_0_4(string& lreturn,x_node** tree);
    char m_operator_0_5(string& lreturn,x_node** tree);
    char m_operator_0_6(string& lreturn,x_node** tree);
    char m_operator_0_7(string& lreturn,x_node** tree);
    char m_operatoraffectation(string& lreturn,x_node** tree);
    char m_operatoraffectation_0_1(string& lreturn,x_node** tree);
    char m_operatoraffectation_0_2(string& lreturn,x_node** tree);
    char m_operatoraffectation_0_3(string& lreturn,x_node** tree);
    char m_operatoraffectation_0_4(string& lreturn,x_node** tree);
    char m_operatoraffectation_0_5(string& lreturn,x_node** tree);
    char m_operatoraffectation_0_6(string& lreturn,x_node** tree);
    char m_operatoraffectation_0_7(string& lreturn,x_node** tree);
    char m_operatoraffectation_0_7_8(string& lreturn,x_node** tree);
    char m_operatoraffectation_0_9(string& lreturn,x_node** tree);
    char m_operatoraffectation_0_10(string& lreturn,x_node** tree);
    char m_operatoraffectation_0_11(string& lreturn,x_node** tree);
    char m_operatoraffectation_0_12(string& lreturn,x_node** tree);
    char m_operatoraffectation_0_13(string& lreturn,x_node** tree);
    char m_operatoraffectation_0_14(string& lreturn,x_node** tree);
    char m_operatoraffectation_0_15(string& lreturn,x_node** tree);
    char m_operatoraffectation_0_16(string& lreturn,x_node** tree);
    char m_operatoraffectation_0_17(string& lreturn,x_node** tree);
    char m_operatoraffectation_0_18(string& lreturn,x_node** tree);
    char m_operatoraffectation_0_19(string& lreturn,x_node** tree);
    char m_operatoraffectation_0_20(string& lreturn,x_node** tree);
    char m_operatoraffectation_0_21(string& lreturn,x_node** tree);
    char m_orand(string& lreturn,x_node** tree);
    char m_orand_0_1(string& lreturn,x_node** tree);
    char m_orand_0_2(string& lreturn,x_node** tree);
    char m_comparator(string& lreturn,x_node** tree);
    char m_comparator_0_1(string& lreturn,x_node** tree);
    char m_comparator_0_2(string& lreturn,x_node** tree);
    char m_comparator_0_3(string& lreturn,x_node** tree);
    char m_comparator_0_4(string& lreturn,x_node** tree);
    char m_comparator_0_5(string& lreturn,x_node** tree);
    char m_comparator_0_6(string& lreturn,x_node** tree);
    char m_fulloperation(string& lreturn,x_node** tree);
    char m_operation(string& lreturn,x_node** tree);
    char m_multiply(string& lreturn,x_node** tree);
    char m_multiply_0_1(string& lreturn,x_node** tree);
    char m_multiply_0_2(string& lreturn,x_node** tree);
    char m_multiply_0_2_3(string& lreturn,x_node** tree);
    char m_comparison(string& lreturn,x_node** tree);
    char m_private(string& lreturn,x_node** tree);
    char m_common(string& lreturn,x_node** tree);
    char m_const(string& lreturn,x_node** tree);
    char m_feature(string& lreturn,x_node** tree);
    char m_feature_0_1(string& lreturn,x_node** tree);
    char m_feature_0_1_2(string& lreturn,x_node** tree);
    char m_feature_0_3(string& lreturn,x_node** tree);
    char m_notatype(string& lreturn,x_node** tree);
    char m_type(string& lreturn,x_node** tree);
    char m_type_0_1(string& lreturn,x_node** tree);
    char m_depend(string& lreturn,x_node** tree);
    char m_declaration(string& lreturn,x_node** tree);
    char m_declaration_0_1(string& lreturn,x_node** tree);
    char m_declaration_0_1_2(string& lreturn,x_node** tree);
    char m_declaration_0_3(string& lreturn,x_node** tree);
    char m_declaration_0_4(string& lreturn,x_node** tree);
    char m_declaration_0_4_5(string& lreturn,x_node** tree);
    char m_declarationlist(string& lreturn,x_node** tree);
    char m_declarationlist_0_4(string& lreturn,x_node** tree);
    char m_declarationlist_0_4_5(string& lreturn,x_node** tree);
    char m_declarationlist_0_6(string& lreturn,x_node** tree);
    char m_declarationlist_0_6_7(string& lreturn,x_node** tree);
    char m_multideclaration(string& lreturn,x_node** tree);
    char m_multideclaration_0_1(string& lreturn,x_node** tree);
    char m_multideclaration_0_1_2(string& lreturn,x_node** tree);
    char m_multideclaration_0_1_2_3(string& lreturn,x_node** tree);
    char m_declarationending(string& lreturn,x_node** tree);
    char m_frame(string& lreturn,x_node** tree);
    char m_frame_0_1(string& lreturn,x_node** tree);
    char m_frame_0_3(string& lreturn,x_node** tree);
    char m_extension(string& lreturn,x_node** tree);
    char m_instruction(string& lreturn,x_node** tree);
    char m_instruction_0_1(string& lreturn,x_node** tree);
    char m_nonlimited(string& lreturn,x_node** tree);
    char m_arguments(string& lreturn,x_node** tree);
    char m_arguments_0_1(string& lreturn,x_node** tree);
    char m_arguments_0_1_2(string& lreturn,x_node** tree);
    char m_arguments_0_1_2_3(string& lreturn,x_node** tree);
    char m_strict(string& lreturn,x_node** tree);
    char m_join(string& lreturn,x_node** tree);
    char m_protecclusive(string& lreturn,x_node** tree);
    char m_functionlabel(string& lreturn,x_node** tree);
    char m_typefunction(string& lreturn,x_node** tree);
    char m_typefunction_0_1(string& lreturn,x_node** tree);
    char m_typefunction_0_2(string& lreturn,x_node** tree);
    char m_typefunction_0_3(string& lreturn,x_node** tree);
    char m_typefunction_0_4(string& lreturn,x_node** tree);
    char m_indexname(string& lreturn,x_node** tree);
    char m_intervalname(string& lreturn,x_node** tree);
    char m_returntype(string& lreturn,x_node** tree);
    char m_function(string& lreturn,x_node** tree);
    char m_function_0_1(string& lreturn,x_node** tree);
    char m_function_0_2(string& lreturn,x_node** tree);
    char m_function_0_3(string& lreturn,x_node** tree);
    char m_subfunc(string& lreturn,x_node** tree);
    char m_subfuncbis(string& lreturn,x_node** tree);
    char m_subvar(string& lreturn,x_node** tree);
    char m_notafunction(string& lreturn,x_node** tree);
    char m_framesep(string& lreturn,x_node** tree);
    char m_frameup(string& lreturn,x_node** tree);
    char m_functioncall(string& lreturn,x_node** tree);
    char m_functioncall_0_1(string& lreturn,x_node** tree);
    char m_regularcall(string& lreturn,x_node** tree);
    char m_regularcall_0_1(string& lreturn,x_node** tree);
    char m_regularcall_0_1_2(string& lreturn,x_node** tree);
    char m_regularcall_0_3(string& lreturn,x_node** tree);
    char m_regularcall_0_3_4(string& lreturn,x_node** tree);
    char m_purecall(string& lreturn,x_node** tree);
    char m_purecall_0_2(string& lreturn,x_node** tree);
    char m_purecall_0_2_3(string& lreturn,x_node** tree);
    char m_returncall(string& lreturn,x_node** tree);
    char m_returncall_0_1(string& lreturn,x_node** tree);
    char m_returncall_0_1_2(string& lreturn,x_node** tree);
    char m_breakcall(string& lreturn,x_node** tree);
    char m_continuecall(string& lreturn,x_node** tree);
    char m_call(string& lreturn,x_node** tree);
    char m_sep(string& lreturn,x_node** tree);
    char m_parameters(string& lreturn,x_node** tree);
    char m_parameters_0_1(string& lreturn,x_node** tree);
    char m_parameters_0_1_2(string& lreturn,x_node** tree);
    char m_blocs(string& lreturn,x_node** tree);
    char m_blocs_0_1(string& lreturn,x_node** tree);
    char m_bloc(string& lreturn,x_node** tree);
    char m_bloc_0_1(string& lreturn,x_node** tree);
    char m_bloc_0_2(string& lreturn,x_node** tree);
    char m_bloc_0_2_3(string& lreturn,x_node** tree);
    char m_sousbloc(string& lreturn,x_node** tree);
    char m_sousbloc_0_1(string& lreturn,x_node** tree);
    char m_sousbloc_0_1_2(string& lreturn,x_node** tree);
    char m_sousbloc_0_1_3(string& lreturn,x_node** tree);
    char m_sousbloc_0_4(string& lreturn,x_node** tree);
    char m_sousbloc_0_4_5(string& lreturn,x_node** tree);
    char m_complexbloc(string& lreturn,x_node** tree);
    char m_analyse(string& lreturn,x_node** tree);
    char m_analyse_0_1(string& lreturn,x_node** tree);
    char m_analyse_0_2(string& lreturn,x_node** tree);
    char m_wrong(string& lreturn,x_node** tree);
    char m_variable(string& lreturn,x_node** tree);
    char m_variable_0_1(string& lreturn,x_node** tree);
    char m_variable_0_1_2(string& lreturn,x_node** tree);
    char m_allvariable(string& lreturn,x_node** tree);
    char m_purevariable(string& lreturn,x_node** tree);
    char m_purevariable_0_1(string& lreturn,x_node** tree);
    char m_purevariable_0_1_2(string& lreturn,x_node** tree);
    char m_predvariable(string& lreturn,x_node** tree);
    char m_predvariable_0_1(string& lreturn,x_node** tree);
    char m_plusplus(string& lreturn,x_node** tree);
    char m_plusplus_0_1(string& lreturn,x_node** tree);
    char m_plusplus_0_2(string& lreturn,x_node** tree);
    char m_power(string& lreturn,x_node** tree);
    char m_affectation(string& lreturn,x_node** tree);
    char m_affectation_0_1(string& lreturn,x_node** tree);
    char m_affectation_0_2(string& lreturn,x_node** tree);
    char m_increment(string& lreturn,x_node** tree);
    char m_increment_0_1(string& lreturn,x_node** tree);
    char m_notin(string& lreturn,x_node** tree);
    char m_notin_0_1(string& lreturn,x_node** tree);
    char m_notin_0_2(string& lreturn,x_node** tree);
    char m_in(string& lreturn,x_node** tree);
    char m_operatorin(string& lreturn,x_node** tree);
    char m_operationin(string& lreturn,x_node** tree);
    char m_negation(string& lreturn,x_node** tree);
    char m_minus(string& lreturn,x_node** tree);
    char m_minus_0_1(string& lreturn,x_node** tree);
    char m_indexes(string& lreturn,x_node** tree);
    char m_indexes_0_1(string& lreturn,x_node** tree);
    char m_indexes_0_2(string& lreturn,x_node** tree);
    char m_indexes_0_2_3(string& lreturn,x_node** tree);
    char m_interval(string& lreturn,x_node** tree);
    char m_interval_0_1(string& lreturn,x_node** tree);
    char m_valtuple(string& lreturn,x_node** tree);
    char m_valtuple_0_1(string& lreturn,x_node** tree);
    char m_valtuple_0_1_2(string& lreturn,x_node** tree);
    char m_valtuple_0_1_2_3(string& lreturn,x_node** tree);
    char m_valtuple_0_1_2_3_4(string& lreturn,x_node** tree);
    char m_valvector(string& lreturn,x_node** tree);
    char m_valtail(string& lreturn,x_node** tree);
    char m_valvectortail(string& lreturn,x_node** tree);
    char m_valvectortail_0_1(string& lreturn,x_node** tree);
    char m_valvectortail_0_1_2(string& lreturn,x_node** tree);
    char m_valvectortail_0_1_2_3(string& lreturn,x_node** tree);
    char m_valvectortail_0_1_2_3_4(string& lreturn,x_node** tree);
    char m_valvectortail_0_1_2_5(string& lreturn,x_node** tree);
    char m_maptail(string& lreturn,x_node** tree);
    char m_dicoval(string& lreturn,x_node** tree);
    char m_valmaptail(string& lreturn,x_node** tree);
    char m_valmaptail_0_1(string& lreturn,x_node** tree);
    char m_valmaptail_0_1_2(string& lreturn,x_node** tree);
    char m_valmaptail_0_1_2_3(string& lreturn,x_node** tree);
    char m_valmaptail_0_1_2_3_4(string& lreturn,x_node** tree);
    char m_valmaptail_0_1_2_5(string& lreturn,x_node** tree);
    char m_intentionsep(string& lreturn,x_node** tree);
    char m_step(string& lreturn,x_node** tree);
    char m_intentionvector(string& lreturn,x_node** tree);
    char m_intentionvector_0_1(string& lreturn,x_node** tree);
    char m_intentionvector_0_1_2(string& lreturn,x_node** tree);
    char m_intentionvector_0_1_3(string& lreturn,x_node** tree);
    char m_intentionvector_0_1_3_4(string& lreturn,x_node** tree);
    char m_intentionvector_0_1_3_5(string& lreturn,x_node** tree);
    char m_intentionwithdouble(string& lreturn,x_node** tree);
    char m_intentionwithdouble_0_1(string& lreturn,x_node** tree);
    char m_intentionwithdouble_0_1_2(string& lreturn,x_node** tree);
    char m_intentionwithdouble_0_1_3(string& lreturn,x_node** tree);
    char m_intentionvect(string& lreturn,x_node** tree);
    char m_dico(string& lreturn,x_node** tree);
    char m_valmap(string& lreturn,x_node** tree);
    char m_valmap_0_1(string& lreturn,x_node** tree);
    char m_valmap_0_1_2(string& lreturn,x_node** tree);
    char m_valmap_0_1_2_3(string& lreturn,x_node** tree);
    char m_valmap_0_1_2_3_4(string& lreturn,x_node** tree);
    char m_jvector(string& lreturn,x_node** tree);
    char m_jvector_0_1(string& lreturn,x_node** tree);
    char m_jvector_0_1_2(string& lreturn,x_node** tree);
    char m_jvector_0_1_2_3(string& lreturn,x_node** tree);
    char m_jvector_0_1_2_3_4(string& lreturn,x_node** tree);
    char m_jdico(string& lreturn,x_node** tree);
    char m_jdico_0_1(string& lreturn,x_node** tree);
    char m_jmap(string& lreturn,x_node** tree);
    char m_jmap_0_1(string& lreturn,x_node** tree);
    char m_jmap_0_1_2(string& lreturn,x_node** tree);
    char m_jmap_0_1_2_3(string& lreturn,x_node** tree);
    char m_jmap_0_1_2_3_4(string& lreturn,x_node** tree);
    char m_jexpression(string& lreturn,x_node** tree);
    char m_aconstant(string& lreturn,x_node** tree);
    char m_cut(string& lreturn,x_node** tree);
    char m_fail(string& lreturn,x_node** tree);
    char m_stop(string& lreturn,x_node** tree);
    char m_not(string& lreturn,x_node** tree);
    char m_notapredicate(string& lreturn,x_node** tree);
    char m_predicate(string& lreturn,x_node** tree);
    char m_predicate_0_1(string& lreturn,x_node** tree);
    char m_predicatecall(string& lreturn,x_node** tree);
    char m_retractcommand(string& lreturn,x_node** tree);
    char m_assertcommandbefore(string& lreturn,x_node** tree);
    char m_assertcommandafter(string& lreturn,x_node** tree);
    char m_assertpredicate(string& lreturn,x_node** tree);
    char m_assertpredicate_0_1(string& lreturn,x_node** tree);
    char m_assertpredicate_0_2(string& lreturn,x_node** tree);
    char m_assertpredicate_0_3(string& lreturn,x_node** tree);
    char m_compfeat(string& lreturn,x_node** tree);
    char m_compfeat_0_1(string& lreturn,x_node** tree);
    char m_compfeat_0_2(string& lreturn,x_node** tree);
    char m_compfeat_0_3(string& lreturn,x_node** tree);
    char m_valplus(string& lreturn,x_node** tree);
    char m_feat(string& lreturn,x_node** tree);
    char m_feat_0_1(string& lreturn,x_node** tree);
    char m_feat_0_2(string& lreturn,x_node** tree);
    char m_feat_0_2_3(string& lreturn,x_node** tree);
    char m_feat_0_2_3_4(string& lreturn,x_node** tree);
    char m_features(string& lreturn,x_node** tree);
    char m_features_0_1(string& lreturn,x_node** tree);
    char m_features_0_1_2(string& lreturn,x_node** tree);
    char m_features_0_1_2_3(string& lreturn,x_node** tree);
    char m_features_0_1_2_3_4(string& lreturn,x_node** tree);
    char m_modifcall(string& lreturn,x_node** tree);
    char m_dependencyparameters(string& lreturn,x_node** tree);
    char m_dependencyparameters_0_1(string& lreturn,x_node** tree);
    char m_dependencyparameters_0_1_2(string& lreturn,x_node** tree);
    char m_dependencyfact(string& lreturn,x_node** tree);
    char m_dependency(string& lreturn,x_node** tree);
    char m_dependency_0_1(string& lreturn,x_node** tree);
    char m_dependency_0_1_2(string& lreturn,x_node** tree);
    char m_dependency_0_3(string& lreturn,x_node** tree);
    char m_dependency_0_4(string& lreturn,x_node** tree);
    char m_dependance(string& lreturn,x_node** tree);
    char m_dependencyresult(string& lreturn,x_node** tree);
    char m_dependencyresult_0_1(string& lreturn,x_node** tree);
    char m_dependencyresult_0_4(string& lreturn,x_node** tree);
    char m_dependencyresult_0_4_5(string& lreturn,x_node** tree);
    char m_dependencyresult_0_4_5_6(string& lreturn,x_node** tree);
    char m_dependencyrule(string& lreturn,x_node** tree);
    char m_dependencyrule_0_1(string& lreturn,x_node** tree);
    char m_dependencyrule_0_2(string& lreturn,x_node** tree);
    char m_dependencyrule_0_2_3(string& lreturn,x_node** tree);
    char m_predicatevariable(string& lreturn,x_node** tree);
    char m_predicatevariable_0_1(string& lreturn,x_node** tree);
    char m_predicatevariable_0_2(string& lreturn,x_node** tree);
    char m_predicatevariable_0_2_3(string& lreturn,x_node** tree);
    char m_predicatevariable_0_2_4(string& lreturn,x_node** tree);
    char m_term(string& lreturn,x_node** tree);
    char m_term_0_1(string& lreturn,x_node** tree);
    char m_tuple(string& lreturn,x_node** tree);
    char m_affectexpression(string& lreturn,x_node** tree);
    char m_affectexpression_0_1(string& lreturn,x_node** tree);
    char m_affectexpression_0_2(string& lreturn,x_node** tree);
    char m_affectexpression_0_2_3(string& lreturn,x_node** tree);
    char m_comparepredicate(string& lreturn,x_node** tree);
    char m_comparepredicate_0_1(string& lreturn,x_node** tree);
    char m_affectationpredicate(string& lreturn,x_node** tree);
    char m_affectationpredicate_0_1(string& lreturn,x_node** tree);
    char m_predplusplus(string& lreturn,x_node** tree);
    char m_pintentionvector(string& lreturn,x_node** tree);
    char m_pintentionvector_0_1(string& lreturn,x_node** tree);
    char m_pintentionvector_0_1_2(string& lreturn,x_node** tree);
    char m_pintentionvector_0_1_3(string& lreturn,x_node** tree);
    char m_pintentionvector_0_1_3_4(string& lreturn,x_node** tree);
    char m_pintentionwithdouble(string& lreturn,x_node** tree);
    char m_pintentionwithdouble_0_1(string& lreturn,x_node** tree);
    char m_pintentionwithdouble_0_1_2(string& lreturn,x_node** tree);
    char m_pintentionwithdouble_0_1_3(string& lreturn,x_node** tree);
    char m_pintentionvect(string& lreturn,x_node** tree);
    char m_merging(string& lreturn,x_node** tree);
    char m_apredicatelist(string& lreturn,x_node** tree);
    char m_apredicatelist_0_1(string& lreturn,x_node** tree);
    char m_apredicatelist_0_1_2(string& lreturn,x_node** tree);
    char m_apredicatelist_0_1_2_3(string& lreturn,x_node** tree);
    char m_apredicatelist_0_1_2_4(string& lreturn,x_node** tree);
    char m_apredicatelist_0_1_2_4_5(string& lreturn,x_node** tree);
    char m_valpredicatevector(string& lreturn,x_node** tree);
    char m_valpredicatevector_0_1(string& lreturn,x_node** tree);
    char m_pexpression(string& lreturn,x_node** tree);
    char m_predicateparameters(string& lreturn,x_node** tree);
    char m_predicateparameters_0_1(string& lreturn,x_node** tree);
    char m_predicateparameters_0_1_2(string& lreturn,x_node** tree);
    char m_mapmerging(string& lreturn,x_node** tree);
    char m_mappredicatelist(string& lreturn,x_node** tree);
    char m_mappredicatelist_0_1(string& lreturn,x_node** tree);
    char m_mappredicatelist_0_1_2(string& lreturn,x_node** tree);
    char m_mappredicatelist_0_1_2_3(string& lreturn,x_node** tree);
    char m_mappredicatelist_0_1_2_4(string& lreturn,x_node** tree);
    char m_valmappredicate(string& lreturn,x_node** tree);
    char m_valmappredicate_0_1(string& lreturn,x_node** tree);
    char m_predicatedico(string& lreturn,x_node** tree);
    char m_predicateoperator(string& lreturn,x_node** tree);
    char m_predicateexpression(string& lreturn,x_node** tree);
    char m_predicateexpression_0_1(string& lreturn,x_node** tree);
    char m_predicateexpression_0_1_2(string& lreturn,x_node** tree);
    char m_predicateexpression_0_1_2_3(string& lreturn,x_node** tree);
    char m_predicateexpression_0_4(string& lreturn,x_node** tree);
    char m_predicateexpression_0_4_5(string& lreturn,x_node** tree);
    char m_predicatedefinition(string& lreturn,x_node** tree);
    char m_predicatedefinition_0_1(string& lreturn,x_node** tree);
    char m_predicatefact(string& lreturn,x_node** tree);
    char m_dcgword(string& lreturn,x_node** tree);
    char m_finaltoken(string& lreturn,x_node** tree);
    char m_finaltoken_0_1(string& lreturn,x_node** tree);
    char m_finaltoken_0_1_2(string& lreturn,x_node** tree);
    char m_finaltoken_0_1_2_3(string& lreturn,x_node** tree);
    char m_finaltoken_0_1_2_4(string& lreturn,x_node** tree);
    char m_finaltoken_0_1_2_4_5(string& lreturn,x_node** tree);
    char m_finaldcg(string& lreturn,x_node** tree);
    char m_dcg(string& lreturn,x_node** tree);
    char m_dcg_0_1(string& lreturn,x_node** tree);
    char m_dcg_0_1_2(string& lreturn,x_node** tree);
    char m_dcg_0_1_3(string& lreturn,x_node** tree);
    char m_dcg_0_1_3_4(string& lreturn,x_node** tree);
    char m_dcg_0_5(string& lreturn,x_node** tree);
    char m_dcg_0_5_6(string& lreturn,x_node** tree);
    char m_taskelltail(string& lreturn,x_node** tree);
    char m_taskelltail_0_1(string& lreturn,x_node** tree);
    char m_taskellexpression(string& lreturn,x_node** tree);
    char m_taskellvector(string& lreturn,x_node** tree);
    char m_taskellvector_0_1(string& lreturn,x_node** tree);
    char m_taskellvector_0_1_2(string& lreturn,x_node** tree);
    char m_taskellvector_0_1_2_3(string& lreturn,x_node** tree);
    char m_taskellvector_0_1_2_3_4(string& lreturn,x_node** tree);
    char m_taskellvector_0_1_2_5(string& lreturn,x_node** tree);
    char m_taskellmaptail(string& lreturn,x_node** tree);
    char m_taskellmaptail_0_1(string& lreturn,x_node** tree);
    char m_taskellkeymap(string& lreturn,x_node** tree);
    char m_taskelldico(string& lreturn,x_node** tree);
    char m_taskellmap(string& lreturn,x_node** tree);
    char m_taskellmap_0_1(string& lreturn,x_node** tree);
    char m_taskellmap_0_1_2(string& lreturn,x_node** tree);
    char m_taskellmap_0_1_2_3(string& lreturn,x_node** tree);
    char m_taskellmap_0_1_2_3_4(string& lreturn,x_node** tree);
    char m_taskellmap_0_1_2_5(string& lreturn,x_node** tree);
    char m_static(string& lreturn,x_node** tree);
    char m_letkeyword(string& lreturn,x_node** tree);
    char m_letkeyword_0_1(string& lreturn,x_node** tree);
    char m_let(string& lreturn,x_node** tree);
    char m_let_0_1(string& lreturn,x_node** tree);
    char m_let_0_2(string& lreturn,x_node** tree);
    char m_letmin(string& lreturn,x_node** tree);
    char m_letmin_0_1(string& lreturn,x_node** tree);
    char m_hvalmaptail(string& lreturn,x_node** tree);
    char m_range(string& lreturn,x_node** tree);
    char m_range_0_1(string& lreturn,x_node** tree);
    char m_range_0_1_2(string& lreturn,x_node** tree);
    char m_range_0_1_3(string& lreturn,x_node** tree);
    char m_range_0_1_3_4(string& lreturn,x_node** tree);
    char m_hdeclaration(string& lreturn,x_node** tree);
    char m_hdeclaration_0_1(string& lreturn,x_node** tree);
    char m_hdeclaration_0_1_2(string& lreturn,x_node** tree);
    char m_hdeclaration_0_1_2_3(string& lreturn,x_node** tree);
    char m_subtaskelldeclaration(string& lreturn,x_node** tree);
    char m_maybe(string& lreturn,x_node** tree);
    char m_declarationtaskell(string& lreturn,x_node** tree);
    char m_declarationtaskell_0_1(string& lreturn,x_node** tree);
    char m_declarationtaskell_0_2(string& lreturn,x_node** tree);
    char m_declarationtaskell_0_2_3(string& lreturn,x_node** tree);
    char m_declarationtaskell_0_2_3_4(string& lreturn,x_node** tree);
    char m_declarationtaskell_0_2_3_4_5(string& lreturn,x_node** tree);
    char m_declarationtaskell_0_2_3_6(string& lreturn,x_node** tree);
    char m_taskelldeclaration(string& lreturn,x_node** tree);
    char m_taskell(string& lreturn,x_node** tree);
    char m_taskell_0_1(string& lreturn,x_node** tree);
    char m_taskell_0_1_2(string& lreturn,x_node** tree);
    char m_taskell_0_1_2_3(string& lreturn,x_node** tree);
    char m_taskell_0_1_2_4(string& lreturn,x_node** tree);
    char m_taskell_0_1_2_4_5(string& lreturn,x_node** tree);
    char m_taskell_0_7(string& lreturn,x_node** tree);
    char m_otherwise(string& lreturn,x_node** tree);
    char m_guard(string& lreturn,x_node** tree);
    char m_guard_0_1(string& lreturn,x_node** tree);
    char m_hlambda(string& lreturn,x_node** tree);
    char m_hlambda_0_1(string& lreturn,x_node** tree);
    char m_hlambda_0_2(string& lreturn,x_node** tree);
    char m_hlambda_0_2_4(string& lreturn,x_node** tree);
    char m_hlambda_0_2_4_5(string& lreturn,x_node** tree);
    char m_hboollambda(string& lreturn,x_node** tree);
    char m_composefunction(string& lreturn,x_node** tree);
    char m_composefunction_0_1(string& lreturn,x_node** tree);
    char m_composefunction_0_2(string& lreturn,x_node** tree);
    char m_composefunction_0_2_3(string& lreturn,x_node** tree);
    char m_hfunction(string& lreturn,x_node** tree);
    char m_hfunction_0_1(string& lreturn,x_node** tree);
    char m_lfold(string& lreturn,x_node** tree);
    char m_rfold(string& lreturn,x_node** tree);
    char m_lscan(string& lreturn,x_node** tree);
    char m_rscan(string& lreturn,x_node** tree);
    char m_fold(string& lreturn,x_node** tree);
    char m_folding(string& lreturn,x_node** tree);
    char m_folding_0_1(string& lreturn,x_node** tree);
    char m_folding_0_2(string& lreturn,x_node** tree);
    char m_taskellalloperator(string& lreturn,x_node** tree);
    char m_taskellboolcheck(string& lreturn,x_node** tree);
    char m_lfold1(string& lreturn,x_node** tree);
    char m_rfold1(string& lreturn,x_node** tree);
    char m_lscan1(string& lreturn,x_node** tree);
    char m_rscan1(string& lreturn,x_node** tree);
    char m_fold1(string& lreturn,x_node** tree);
    char m_folding1(string& lreturn,x_node** tree);
    char m_filterkeyword(string& lreturn,x_node** tree);
    char m_takekeyword(string& lreturn,x_node** tree);
    char m_inverted(string& lreturn,x_node** tree);
    char m_taskcomparison(string& lreturn,x_node** tree);
    char m_mapping(string& lreturn,x_node** tree);
    char m_mapping_0_1(string& lreturn,x_node** tree);
    char m_filtering(string& lreturn,x_node** tree);
    char m_filtering_0_1(string& lreturn,x_node** tree);
    char m_taskellalltrue(string& lreturn,x_node** tree);
    char m_taskellboolchecking(string& lreturn,x_node** tree);
    char m_taking(string& lreturn,x_node** tree);
    char m_zipping(string& lreturn,x_node** tree);
    char m_zipping_0_1(string& lreturn,x_node** tree);
    char m_zipping_0_2(string& lreturn,x_node** tree);
    char m_pairing(string& lreturn,x_node** tree);
    char m_cycle(string& lreturn,x_node** tree);
    char m_cycling(string& lreturn,x_node** tree);
    char m_repeating(string& lreturn,x_node** tree);
    char m_flipping(string& lreturn,x_node** tree);
    char m_taskellcaseotherwise(string& lreturn,x_node** tree);
    char m_taskellcaseotherwise_0_1(string& lreturn,x_node** tree);
    char m_taskellresult(string& lreturn,x_node** tree);
    char m_taskellresult_0_1(string& lreturn,x_node** tree);
    char m_taskellcase(string& lreturn,x_node** tree);
    char m_taskellcase_0_1(string& lreturn,x_node** tree);
    char m_taskellcase_0_1_2(string& lreturn,x_node** tree);
    char m_taskellcase_0_3(string& lreturn,x_node** tree);
    char m_curryingleft(string& lreturn,x_node** tree);
    char m_curryingleft_0_1(string& lreturn,x_node** tree);
    char m_curryingright(string& lreturn,x_node** tree);
    char m_currying(string& lreturn,x_node** tree);
    char m_currying_0_1(string& lreturn,x_node** tree);
    char m_hfunctionoperation(string& lreturn,x_node** tree);
    char m_hfunctionoperation_0_1(string& lreturn,x_node** tree);
    char m_hfunctionoperation_0_2(string& lreturn,x_node** tree);
    char m_hfunctioncall(string& lreturn,x_node** tree);
    char m_hfunctioncall_0_1(string& lreturn,x_node** tree);
    char m_hfunctioncall_0_2(string& lreturn,x_node** tree);
    char m_hfunctioncall_0_2_3(string& lreturn,x_node** tree);
    char m_hoperator(string& lreturn,x_node** tree);
    char m_hmetafunctions(string& lreturn,x_node** tree);
    char m_hcompose(string& lreturn,x_node** tree);
    char m_hcompose_0_1(string& lreturn,x_node** tree);
    char m_hcomposecall(string& lreturn,x_node** tree);
    char m_hoper(string& lreturn,x_node** tree);
    char m_hoperation(string& lreturn,x_node** tree);
    char m_negcall(string& lreturn,x_node** tree);
    char m_hitem(string& lreturn,x_node** tree);
    char m_hnegated(string& lreturn,x_node** tree);
    char m_hmultiply(string& lreturn,x_node** tree);
    char m_hmultiply_0_2(string& lreturn,x_node** tree);
    char m_hoperations(string& lreturn,x_node** tree);
    char m_hoperations_0_1(string& lreturn,x_node** tree);
    char m_hoperations_0_1_2(string& lreturn,x_node** tree);
    char m_hoperations_0_5(string& lreturn,x_node** tree);
    char m_hcomparator(string& lreturn,x_node** tree);
    char m_hcomparison(string& lreturn,x_node** tree);
    char m_hcompare(string& lreturn,x_node** tree);
    char m_hcompare_0_1(string& lreturn,x_node** tree);
    char m_hcompare_0_2(string& lreturn,x_node** tree);
    char m_hcompare_0_2_3(string& lreturn,x_node** tree);
    char m_hforcecompare(string& lreturn,x_node** tree);
    char m_hforcecompare_0_1(string& lreturn,x_node** tree);
    char m_hoptionalboolean(string& lreturn,x_node** tree);
    char m_hoptionalboolean_0_1(string& lreturn,x_node** tree);
    char m_hbooleanexpression(string& lreturn,x_node** tree);
    char m_hbooleanexpression_0_1(string& lreturn,x_node** tree);
    char m_hbooleanexpression_0_2(string& lreturn,x_node** tree);
    char m_hbooleanexpression_0_2_3(string& lreturn,x_node** tree);
    char m_hbooleanexpression_0_2_3_4(string& lreturn,x_node** tree);
    char m_hforcebooleanexpression(string& lreturn,x_node** tree);
    char m_hbloc(string& lreturn,x_node** tree);
    char m_whereexpression(string& lreturn,x_node** tree);
    char m_whereexpression_0_1(string& lreturn,x_node** tree);
    char m_whereexpression_0_1_2(string& lreturn,x_node** tree);
    char m_whereexpression_0_1_2_3(string& lreturn,x_node** tree);
    char m_whereexpression_0_1_2_3_4(string& lreturn,x_node** tree);
    char m_whereexpression_0_1_2_3_4_5(string& lreturn,x_node** tree);
    char m_whereexpression_0_6(string& lreturn,x_node** tree);
    char m_whereexpression_0_6_7(string& lreturn,x_node** tree);
    char m_whereexpression_0_6_7_8(string& lreturn,x_node** tree);
    char m_whereexpression_0_6_7_8_9(string& lreturn,x_node** tree);
    char m_hinexpression(string& lreturn,x_node** tree);
    char m_notadeclaration(string& lreturn,x_node** tree);
    char m_deriving(string& lreturn,x_node** tree);
    char m_deriving_0_1(string& lreturn,x_node** tree);
    char m_deriving_0_1_2(string& lreturn,x_node** tree);
    char m_subdeclaration(string& lreturn,x_node** tree);
    char m_subdata(string& lreturn,x_node** tree);
    char m_subdata_0_1(string& lreturn,x_node** tree);
    char m_subdata_0_1_2(string& lreturn,x_node** tree);
    char m_hdatadeclaration(string& lreturn,x_node** tree);
    char m_hdatadeclaration_0_1(string& lreturn,x_node** tree);
    char m_hdatadeclaration_0_1_2(string& lreturn,x_node** tree);
    char m_hdatadeclaration_0_1_2_3(string& lreturn,x_node** tree);
    char m_hdata(string& lreturn,x_node** tree);
    char m_hdata_0_1(string& lreturn,x_node** tree);
    char m_hdata_0_1_2(string& lreturn,x_node** tree);
    char m_hdata_0_1_2_3(string& lreturn,x_node** tree);
    char m_hdata_0_1_2_3_4(string& lreturn,x_node** tree);
    char m_hdata_0_1_2_5(string& lreturn,x_node** tree);
    char m_hontology(string& lreturn,x_node** tree);
    char m_hontology_0_1(string& lreturn,x_node** tree);
    char m_hontology_0_1_2(string& lreturn,x_node** tree);
    char m_hontology_0_1_2_3(string& lreturn,x_node** tree);
    char m_hconcept(string& lreturn,x_node** tree);
    char m_conceptfunction(string& lreturn,x_node** tree);
    char m_returntaskelldeclaration(string& lreturn,x_node** tree);
    char m_assignfield(string& lreturn,x_node** tree);
    char m_dataassignment(string& lreturn,x_node** tree);
    char m_dataassignment_0_1(string& lreturn,x_node** tree);
    char m_dataassignment_0_1_2(string& lreturn,x_node** tree);
    char m_hrange(string& lreturn,x_node** tree);
    char m_hrange_0_1(string& lreturn,x_node** tree);
    char m_hrange_0_1_2(string& lreturn,x_node** tree);
    char m_hrange_0_3(string& lreturn,x_node** tree);
    char m_hrange_0_3_4(string& lreturn,x_node** tree);
    char m_hrange_0_3_4_5(string& lreturn,x_node** tree);
    char m_hrange_0_3_4_5_6(string& lreturn,x_node** tree);
    char m_hrange_0_3_4_5_6_7(string& lreturn,x_node** tree);
    char m_hrange_0_3_4_5_6_7_8(string& lreturn,x_node** tree);
    char m_telque(string& lreturn,x_node** tree);
    char m_telque_0_1(string& lreturn,x_node** tree);
    char m_telque_0_1_2(string& lreturn,x_node** tree);
    char m_telque_0_1_2_3(string& lreturn,x_node** tree);
    char m_telque_0_1_2_4(string& lreturn,x_node** tree);
    char m_telque_0_1_2_4_5(string& lreturn,x_node** tree);
    char m_telque_0_1_2_4_5_6(string& lreturn,x_node** tree);
    char m_telque_0_1_2_7(string& lreturn,x_node** tree);
    char m_telque_0_1_2_7_8(string& lreturn,x_node** tree);
    char m_telque_0_1_2_7_8_9(string& lreturn,x_node** tree);
    char m_telque_0_1_2_7_8_9_10(string& lreturn,x_node** tree);
    char m_telque_0_1_2_7_8_9_10_11(string& lreturn,x_node** tree);
    char m_telque_0_1_2_7_8_12(string& lreturn,x_node** tree);
    char m_telque_0_1_2_7_8_12_13(string& lreturn,x_node** tree);
    char m_telque_0_1_2_7_14(string& lreturn,x_node** tree);
    char m_telque_0_1_2_7_14_15(string& lreturn,x_node** tree);
    char m_telque_0_1_2_7_14_15_16(string& lreturn,x_node** tree);
    char m_telque_0_1_2_7_14_17(string& lreturn,x_node** tree);
    char m_telque_0_1_2_7_14_17_18(string& lreturn,x_node** tree);
    char m_telque_0_1_2_7_14_17_18_19(string& lreturn,x_node** tree);
    char m_telque_0_1_2_7_14_17_18_20(string& lreturn,x_node** tree);
    char m_telque_0_1_2_7_14_17_18_20_21(string& lreturn,x_node** tree);
    char m_telque_0_1_2_7_14_17_18_20_21_22(string& lreturn,x_node** tree);
    char m_telque_0_1_2_7_14_17_18_20_21_22_23(string& lreturn,x_node** tree);
    char m_telque_0_1_2_7_14_17_18_20_21_24(string& lreturn,x_node** tree);
    char m_telque_0_1_2_7_14_17_18_20_21_24_25(string& lreturn,x_node** tree);
    char m_telque_0_1_2_7_14_17_18_20_21_24_25_26(string& lreturn,x_node** tree);
    char m_telque_0_1_2_7_14_17_18_20_21_24_25_26_27(string& lreturn,x_node** tree);
    char m_subtelque(string& lreturn,x_node** tree);
    char m_subtelque_0_1(string& lreturn,x_node** tree);
    char m_subtelque_0_1_2(string& lreturn,x_node** tree);
    char m_parenthetic(string& lreturn,x_node** tree);
    char m_expression(string& lreturn,x_node** tree);
    char m_expression_0_1(string& lreturn,x_node** tree);
    char m_expression_0_2(string& lreturn,x_node** tree);
    char m_expressions(string& lreturn,x_node** tree);
    char m_expressions_0_2(string& lreturn,x_node** tree);
    char m_expressions_0_2_3(string& lreturn,x_node** tree);
    char m_comparing(string& lreturn,x_node** tree);
    char m_comparing_0_1(string& lreturn,x_node** tree);
    char m_wnexpressions(string& lreturn,x_node** tree);
    char m_wnexpressions_0_1(string& lreturn,x_node** tree);
    char m_wnexpressions_0_3(string& lreturn,x_node** tree);
    char m_wnexpressions_0_3_4(string& lreturn,x_node** tree);
    char m_nocomparisonexpressions(string& lreturn,x_node** tree);
    char m_nocomparisonnorinexpressions(string& lreturn,x_node** tree);
    char m_nocomparisonnorinexpressions_0_3(string& lreturn,x_node** tree);
    char m_numbers(string& lreturn,x_node** tree);
    char m_numbers_0_1(string& lreturn,x_node** tree);
    char m_negated(string& lreturn,x_node** tree);
    char m_abool(string& lreturn,x_node** tree);
    char m_blocfor(string& lreturn,x_node** tree);
    char m_blocfor_0_1(string& lreturn,x_node** tree);
    char m_blocfor_0_2(string& lreturn,x_node** tree);
    char m_blocfor_0_2_3(string& lreturn,x_node** tree);
    char m_localif(string& lreturn,x_node** tree);
    char m_localif_0_1(string& lreturn,x_node** tree);
    char m_localif_0_1_2(string& lreturn,x_node** tree);
    char m_declarationfor(string& lreturn,x_node** tree);
    char m_declarationfor_0_1(string& lreturn,x_node** tree);
    char m_declarationfor_0_2(string& lreturn,x_node** tree);
    char m_optionalboolean(string& lreturn,x_node** tree);
    char m_booleanexpression(string& lreturn,x_node** tree);
    char m_booleanexpression_0_1(string& lreturn,x_node** tree);
    char m_booleanexpression_0_2(string& lreturn,x_node** tree);
    char m_booleanexpression_0_2_3(string& lreturn,x_node** tree);
    char m_booleanexpression_0_2_3_4(string& lreturn,x_node** tree);
    char m_arange(string& lreturn,x_node** tree);
    char m_arange_0_1(string& lreturn,x_node** tree);
    char m_arange_0_1_2(string& lreturn,x_node** tree);
    char m_arange_0_1_2_3(string& lreturn,x_node** tree);
    char m_iftest(string& lreturn,x_node** tree);
    char m_iftest_0_1(string& lreturn,x_node** tree);
    char m_iftest_0_2(string& lreturn,x_node** tree);
    char m_iftest_0_3(string& lreturn,x_node** tree);
    char m_iftest_0_3_4(string& lreturn,x_node** tree);
    char m_testelif(string& lreturn,x_node** tree);
    char m_loop(string& lreturn,x_node** tree);
    char m_doloop(string& lreturn,x_node** tree);
    char m_for(string& lreturn,x_node** tree);
    char m_for_0_1(string& lreturn,x_node** tree);
    char m_forin(string& lreturn,x_node** tree);
    char m_forin_0_1(string& lreturn,x_node** tree);
    char m_forin_0_1_2(string& lreturn,x_node** tree);
    char m_forin_0_1_2_3(string& lreturn,x_node** tree);
    char m_forin_0_4(string& lreturn,x_node** tree);
    char m_switch(string& lreturn,x_node** tree);
    char m_switch_0_2(string& lreturn,x_node** tree);
    char m_trycatch(string& lreturn,x_node** tree);
    char m_trycatch_0_1(string& lreturn,x_node** tree);
    char m_trycatch_0_1_2(string& lreturn,x_node** tree);
    char m_trycatch_0_1_2_3(string& lreturn,x_node** tree);
    char m_trycatch_0_4(string& lreturn,x_node** tree);
    char m_trycatch_0_4_5(string& lreturn,x_node** tree);
    char m_testswitch(string& lreturn,x_node** tree);
    char m_testswitch_0_1(string& lreturn,x_node** tree);
    char m_default(string& lreturn,x_node** tree);
    char m_lispexpression(string& lreturn,x_node** tree);
    char m_lispexpressions(string& lreturn,x_node** tree);
    char m_onelisp(string& lreturn,x_node** tree);
    char m_onelisp_0_1(string& lreturn,x_node** tree);
    char m_lisp(string& lreturn,x_node** tree);
    char m_tagexpression(string& lreturn,x_node** tree);
    char m_tagexpressions(string& lreturn,x_node** tree);
    char m_onetag(string& lreturn,x_node** tree);
    char m_onetag_0_1(string& lreturn,x_node** tree);
    char m_tag(string& lreturn,x_node** tree);
    char m_ameta(string& lreturn,x_node** tree);
    char m_ameta_0_1(string& lreturn,x_node** tree);
    char m_ameta_0_1_2(string& lreturn,x_node** tree);
    char m_ameta_0_1_2_3(string& lreturn,x_node** tree);
    char m_ameta_0_4(string& lreturn,x_node** tree);
    char m_amulti(string& lreturn,x_node** tree);
    char m_asimple(string& lreturn,x_node** tree);
    char m_atoken(string& lreturn,x_node** tree);
    char m_atoken_0_2(string& lreturn,x_node** tree);
    char m_anitem(string& lreturn,x_node** tree);
    char m_dj(string& lreturn,x_node** tree);
    char m_aoptional(string& lreturn,x_node** tree);
    char m_aoptitem(string& lreturn,x_node** tree);
    char m_akleene(string& lreturn,x_node** tree);
    char m_anelement(string& lreturn,x_node** tree);
    char m_anelement_0_1(string& lreturn,x_node** tree);
    char m_anelement_0_1_2(string& lreturn,x_node** tree);
    char m_anitemelement(string& lreturn,x_node** tree);
    char m_anitemelement_0_1(string& lreturn,x_node** tree);
    char m_body(string& lreturn,x_node** tree);
    char m_body_0_1(string& lreturn,x_node** tree);
    char m_body_0_1_2(string& lreturn,x_node** tree);
    char m_body_0_1_2_3(string& lreturn,x_node** tree);
    char m_rule(string& lreturn,x_node** tree);
    char m_rule_0_1(string& lreturn,x_node** tree);
    char m_rule_0_1_2(string& lreturn,x_node** tree);
    char m_subgram(string& lreturn,x_node** tree);
    char m_subgram_0_1(string& lreturn,x_node** tree);
    char m_subgram_0_1_2(string& lreturn,x_node** tree);
    char m_subgram_0_1_2_3(string& lreturn,x_node** tree);
    char m_rules(string& lreturn,x_node** tree);
    char m_non(string& lreturn,x_node** tree);
    char m_kleene(string& lreturn,x_node** tree);
    char m_kleene_0_1(string& lreturn,x_node** tree);
    char m_kleene_0_2(string& lreturn,x_node** tree);
    char m_kleene_0_2_3(string& lreturn,x_node** tree);
    char m_any(string& lreturn,x_node** tree);
    char m_meta(string& lreturn,x_node** tree);
    char m_alabel(string& lreturn,x_node** tree);
    char m_alabel_0_1(string& lreturn,x_node** tree);
    char m_orlabels(string& lreturn,x_node** tree);
    char m_orlabels_0_1(string& lreturn,x_node** tree);
    char m_orlabels_0_1_2(string& lreturn,x_node** tree);
    char m_andlabels(string& lreturn,x_node** tree);
    char m_andlabels_0_1(string& lreturn,x_node** tree);
    char m_label(string& lreturn,x_node** tree);
    char m_lemma(string& lreturn,x_node** tree);
    char m_callmatch(string& lreturn,x_node** tree);
    char m_callmatch_0_1(string& lreturn,x_node** tree);
    char m_token(string& lreturn,x_node** tree);
    char m_token_0_1(string& lreturn,x_node** tree);
    char m_token_0_2(string& lreturn,x_node** tree);
    char m_listoftokens(string& lreturn,x_node** tree);
    char m_listoftokens_0_1(string& lreturn,x_node** tree);
    char m_listoftokens_0_1_2(string& lreturn,x_node** tree);
    char m_listoftokens_0_3(string& lreturn,x_node** tree);
    char m_sequenceoftokens(string& lreturn,x_node** tree);
    char m_sequenceoftokens_0_1(string& lreturn,x_node** tree);
    char m_optionaltokens(string& lreturn,x_node** tree);
    char m_optionaltokens_0_1(string& lreturn,x_node** tree);
    char m_removetokens(string& lreturn,x_node** tree);
    char m_removetokens_0_1(string& lreturn,x_node** tree);
    char m_annotation(string& lreturn,x_node** tree);
    char m_annotation_0_1(string& lreturn,x_node** tree);
    char m_annotation_0_1_2(string& lreturn,x_node** tree);
    char m_annotation_0_3(string& lreturn,x_node** tree);
    char m_ruletype(string& lreturn,x_node** tree);
    char m_annotationrule(string& lreturn,x_node** tree);
    char m_annotationrule_0_1(string& lreturn,x_node** tree);
    char m_annotationrule_0_2(string& lreturn,x_node** tree);
    char m_annotationrule_0_4(string& lreturn,x_node** tree);
    char m_annotationrule_0_4_6(string& lreturn,x_node** tree);
    char m_annotationrule_0_4_6_7(string& lreturn,x_node** tree);
    char m_punct(string& lreturn,x_node** tree);
    char m_astringdouble(string& lreturn,x_node** tree);
    char m_astringsimple(string& lreturn,x_node** tree);
    char m_afullstring(string& lreturn,x_node** tree);
    char m_anumber(string& lreturn,x_node** tree);
    char m_word(string& lreturn,x_node** tree);
    char m_typename(string& lreturn,x_node** tree);
    char m_astring(string& lreturn,x_node** tree);
    char m_atreg(string& lreturn,x_node** tree);
    char m_astreg(string& lreturn,x_node** tree);
    char m_apreg(string& lreturn,x_node** tree);
    char m_aspreg(string& lreturn,x_node** tree);
    char m_metas(string& lreturn,x_node** tree);
    char m_rgx(string& lreturn,x_node** tree);
    char Y_var_1;
    char Y_var_0;
    string VS_var_3;
    string VS_var_2;
};

class bnf_tamgu_pop {
    public:
    bnf_tamgu* pointer;

    bnf_tamgu_pop(bnf_tamgu* b) {
        pointer=b;
    }

    ~bnf_tamgu_pop() {
        pointer->poperrors();
    }
};


#endif
