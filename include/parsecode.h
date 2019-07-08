/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : parsecode.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef bnf_tamgu_h
#define bnf_tamgu_h
#include "x_node.h"


class bnf_tamgu {
public:


	size_t currentpos;
	int intoken;
	int lineerror;
	string labelerror;
	int errornumber;
	char gFail;
	int baseline;
	x_reading* fx;

	vector<string> labelerrors;
	vector<int> lineerrors;
	vector<int> errornumbers;

	bnf_tamgu(int l = 0) {
		baseline = l;
		fx = NULL;
		intoken = 0;
		currentpos = 0;
		Y_var_1 = ')';
		Y_var_0 = '(';
		VS_var_3 = "CLOSE";
		VS_var_2 = "OPEN";

	}

	void initialize(x_reading* xr) {
		labelerrors.clear();
		lineerrors.clear();
		errornumbers.clear();

		baseline = 0;
		fx = xr;
		gFail = 0;
		intoken = 0;
		currentpos = 0;
	}

	void setfail(char test) {
		if (test)
			gFail = 1;
	}

	void initfail(string lab, size_t pos, int error) {
		if (gFail == 1)
			return;
		if (pos >= fx->stackln.size()) {
			lineerror = -1;
			labelerror = lab;
			errornumber = error;
			return;
		}
		lineerror = fx->stackln[pos] + baseline;
		labelerror = lab;
		errornumber = error;
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


	inline void x_pop_node(x_node** tree, int addsubtree) {
		if (*tree != NULL) {
			if ((*tree)->init == 0) {
				delete *tree;
				*tree = NULL;
			}
			else
				(*tree)->pop(addsubtree);
		}
	}

	inline void x_init_node(string& lreturn, string& lret, x_node** tree, string name, long d, bool conc) {
		if (lret.size() > 0 || conc == false) {
			if (*tree == NULL)
				*tree = new x_node;
			(*tree)->start = d;
			(*tree)->end = d;
			(*tree)->token = name;
			(*tree)->init = 1;
			if (conc) {
				(*tree)->value = lret;
				lreturn += lret;
			}
		}
	}


	inline void x_init_tree(x_node** tree, x_node* subtree, int& addsubtree) {
		if (subtree == NULL)
			return;
		if (*tree == NULL)
			*tree = new x_node;
		if ((*tree)->append(subtree) == 0)
			delete subtree;
		else
			addsubtree++;
	}

	char x_test_string(string& lret, int sz, string sub);
	char x_test_pstring(string& lret, int sz, string& sub);

	void incrementintoken() {
		intoken++;
		if (intoken == fx->stack[currentpos].size()) {
			currentpos++;
			intoken = 0;
		}
	}

	void incrementpos() {
		currentpos++;
		intoken = 0;

	}


	inline char x_test_char(string& lret, unsigned char sub) {
		if (currentpos >= fx->stack.size())
			return(0);
		unsigned char x = fx->stack[currentpos][intoken];
		if (x == sub) {
			incrementintoken();
			lret += x;
			return(1);
		}
		return(0);
	}


	inline char x_test_in(string& lret, char* sub) {
		if (currentpos >= fx->stack.size())
			return(0);
		char x = fx->stack[currentpos][intoken];
		if (strchr(sub, x) != NULL) {
			lret += x;
			incrementintoken();
			return(1);
		}
		return(0);
	}


	inline char x_test_ord(string& lret, unsigned char b, unsigned char g) {
		if (currentpos >= fx->stack.size())
			return(0);
		unsigned char x = fx->stack[currentpos][intoken];
		if (x >= b && x <= g) {
			incrementintoken();
			lret += x;
			return(1);
		}
		return(0);
	}

	inline char x_test_any(string& lret) {
		if (currentpos >= fx->stack.size())
			return(0);
		char x = fx->stack[currentpos][intoken];
		incrementintoken();
		lret += x;
		return(1);
	}


	char m_EOF(string& lreturn, x_node** tree) {
		if (currentpos >= fx->stack.size())
			return(1);
		return(0);
	}

	long x_linenumber(size_t p) {
		if (p < fx->stackln.size())
			return (fx->stackln[p] + baseline);
		return -1;
	}

	string x_errormsg(int i);
	x_node* x_parsing(x_reading* xr, x_parsing_mode mode, bool display = true);



	char m_char(string& lreturn, x_node** tree);
	char m_path(string& lreturn, x_node** tree);
	char m_path_0_1(string& lreturn, x_node** tree);
	char m_shell(string& lreturn, x_node** tree);
	char m_shell_0_1(string& lreturn, x_node** tree);
	char m_signature(string& lreturn, x_node** tree);
	char m_label(string& lreturn, x_node** tree);
	char m_anydouble(string& lreturn, x_node** tree);
	char m_anydouble_0_1(string& lreturn, x_node** tree);
	char m_anydouble_0_2(string& lreturn, x_node** tree);
	char m_anysimple(string& lreturn, x_node** tree);
	char m_anycolon(string& lreturn, x_node** tree);
	char m_anycolon_0_1(string& lreturn, x_node** tree);
	char m_anycolon_0_2(string& lreturn, x_node** tree);
	char m_operator(string& lreturn, x_node** tree);
	char m_operator_0_1(string& lreturn, x_node** tree);
	char m_operator_0_2(string& lreturn, x_node** tree);
	char m_operator_0_3(string& lreturn, x_node** tree);
	char m_operator_0_4(string& lreturn, x_node** tree);
	char m_operator_0_5(string& lreturn, x_node** tree);
	char m_operatoraffectation(string& lreturn, x_node** tree);
	char m_operatoraffectation_0_1(string& lreturn, x_node** tree);
	char m_operatoraffectation_0_2(string& lreturn, x_node** tree);
	char m_operatoraffectation_0_3(string& lreturn, x_node** tree);
	char m_operatoraffectation_0_3_4(string& lreturn, x_node** tree);
	char m_operatoraffectation_0_5(string& lreturn, x_node** tree);
	char m_operatoraffectation_0_6(string& lreturn, x_node** tree);
	char m_operatoraffectation_0_7(string& lreturn, x_node** tree);
	char m_operatoraffectation_0_8(string& lreturn, x_node** tree);
	char m_operatoraffectation_0_9(string& lreturn, x_node** tree);
	char m_operatoraffectation_0_10(string& lreturn, x_node** tree);
	char m_operatoraffectation_0_11(string& lreturn, x_node** tree);
	char m_operatoraffectation_0_12(string& lreturn, x_node** tree);
	char m_operatoraffectation_0_13(string& lreturn, x_node** tree);
	char m_operatoraffectation_0_14(string& lreturn, x_node** tree);
	char m_operatoraffectation_0_15(string& lreturn, x_node** tree);
	char m_operatoraffectation_0_16(string& lreturn, x_node** tree);
	char m_orand(string& lreturn, x_node** tree);
	char m_orand_0_1(string& lreturn, x_node** tree);
	char m_orand_0_2(string& lreturn, x_node** tree);
	char m_comparator(string& lreturn, x_node** tree);
	char m_comparator_0_1(string& lreturn, x_node** tree);
	char m_comparator_0_2(string& lreturn, x_node** tree);
	char m_comparator_0_3(string& lreturn, x_node** tree);
	char m_comparator_0_4(string& lreturn, x_node** tree);
	char m_comparator_0_5(string& lreturn, x_node** tree);
	char m_operation(string& lreturn, x_node** tree);
	char m_comparison(string& lreturn, x_node** tree);
	char m_private(string& lreturn, x_node** tree);
	char m_common(string& lreturn, x_node** tree);
	char m_const(string& lreturn, x_node** tree);
	char m_feature(string& lreturn, x_node** tree);
	char m_feature_0_1(string& lreturn, x_node** tree);
	char m_feature_0_1_2(string& lreturn, x_node** tree);
	char m_feature_0_3(string& lreturn, x_node** tree);
	char m_type(string& lreturn, x_node** tree);
	char m_type_0_1(string& lreturn, x_node** tree);
	char m_depend(string& lreturn, x_node** tree);
	char m_declaration(string& lreturn, x_node** tree);
	char m_declaration_0_1(string& lreturn, x_node** tree);
	char m_declaration_0_1_2(string& lreturn, x_node** tree);
	char m_declaration_0_3(string& lreturn, x_node** tree);
	char m_declaration_0_4(string& lreturn, x_node** tree);
	char m_declaration_0_4_5(string& lreturn, x_node** tree);
	char m_declarationlist(string& lreturn, x_node** tree);
	char m_declarationlist_0_4(string& lreturn, x_node** tree);
	char m_declarationlist_0_4_5(string& lreturn, x_node** tree);
	char m_declarationlist_0_6(string& lreturn, x_node** tree);
	char m_declarationlist_0_6_7(string& lreturn, x_node** tree);
	char m_multideclaration(string& lreturn, x_node** tree);
	char m_declarationending(string& lreturn, x_node** tree);
	char m_frame(string& lreturn, x_node** tree);
	char m_frame_0_1(string& lreturn, x_node** tree);
	char m_frame_0_3(string& lreturn, x_node** tree);
	char m_extension(string& lreturn, x_node** tree);
	char m_instruction(string& lreturn, x_node** tree);
	char m_instruction_0_1(string& lreturn, x_node** tree);
	char m_nonlimited(string& lreturn, x_node** tree);
	char m_arguments(string& lreturn, x_node** tree);
	char m_arguments_0_1(string& lreturn, x_node** tree);
	char m_arguments_0_1_2(string& lreturn, x_node** tree);
	char m_arguments_0_1_2_3(string& lreturn, x_node** tree);
	char m_strict(string& lreturn, x_node** tree);
	char m_join(string& lreturn, x_node** tree);
	char m_protecclusive(string& lreturn, x_node** tree);
	char m_functionsort(string& lreturn, x_node** tree);
	char m_typefunction(string& lreturn, x_node** tree);
	char m_typefunction_0_1(string& lreturn, x_node** tree);
	char m_typefunction_0_2(string& lreturn, x_node** tree);
	char m_typefunction_0_3(string& lreturn, x_node** tree);
	char m_typefunction_0_4(string& lreturn, x_node** tree);
	char m_indexname(string& lreturn, x_node** tree);
	char m_intervalname(string& lreturn, x_node** tree);
	char m_function(string& lreturn, x_node** tree);
	char m_function_0_1(string& lreturn, x_node** tree);
	char m_function_0_2(string& lreturn, x_node** tree);
	char m_lambda(string& lreturn, x_node** tree);
	char m_evallambda(string& lreturn, x_node** tree);
	char m_subfunc(string& lreturn, x_node** tree);
	char m_subfuncbis(string& lreturn, x_node** tree);
	char m_subvar(string& lreturn, x_node** tree);
	char m_framesep(string& lreturn, x_node** tree);
	char m_frameup(string& lreturn, x_node** tree);
	char m_functioncall(string& lreturn, x_node** tree);
	char m_functioncall_0_1(string& lreturn, x_node** tree);
	char m_regularcall(string& lreturn, x_node** tree);
	char m_regularcall_0_1(string& lreturn, x_node** tree);
	char m_regularcall_0_2(string& lreturn, x_node** tree);
	char m_regularcall_0_2_3(string& lreturn, x_node** tree);
	char m_purecall(string& lreturn, x_node** tree);
	char m_purecall_0_2(string& lreturn, x_node** tree);
	char m_purecall_0_2_3(string& lreturn, x_node** tree);
	char m_returncall(string& lreturn, x_node** tree);
	char m_breakcall(string& lreturn, x_node** tree);
	char m_continuecall(string& lreturn, x_node** tree);
	char m_call(string& lreturn, x_node** tree);
	char m_sep(string& lreturn, x_node** tree);
	char m_parameters(string& lreturn, x_node** tree);
	char m_parameters_0_1(string& lreturn, x_node** tree);
	char m_parameters_0_1_2(string& lreturn, x_node** tree);
	char m_sousbloc(string& lreturn, x_node** tree);
	char m_bloc(string& lreturn, x_node** tree);
	char m_bloc_0_1(string& lreturn, x_node** tree);
	char m_bloc_0_2(string& lreturn, x_node** tree);
	char m_bloc_0_3(string& lreturn, x_node** tree);
	char m_complexbloc(string& lreturn, x_node** tree);
	char m_blocs(string& lreturn, x_node** tree);
	char m_blocs_0_1(string& lreturn, x_node** tree);
	char m_analyse(string& lreturn, x_node** tree);
	char m_analyse_0_1(string& lreturn, x_node** tree);
	char m_analyse_0_2(string& lreturn, x_node** tree);
	char m_analyse_0_3(string& lreturn, x_node** tree);
	char m_analyse_0_3_4(string& lreturn, x_node** tree);
	char m_analyse_0_3_4_5(string& lreturn, x_node** tree);
	char m_variable(string& lreturn, x_node** tree);
	char m_variable_0_1(string& lreturn, x_node** tree);
	char m_variable_0_1_2(string& lreturn, x_node** tree);
	char m_allvariable(string& lreturn, x_node** tree);
	char m_purevariable(string& lreturn, x_node** tree);
	char m_purevariable_0_1(string& lreturn, x_node** tree);
	char m_purevariable_0_1_2(string& lreturn, x_node** tree);
	char m_predvariable(string& lreturn, x_node** tree);
	char m_plusplus(string& lreturn, x_node** tree);
	char m_plusplus_0_1(string& lreturn, x_node** tree);
	char m_plusplus_0_2(string& lreturn, x_node** tree);
	char m_affectation(string& lreturn, x_node** tree);
	char m_affectation_0_1(string& lreturn, x_node** tree);
	char m_affectation_0_2(string& lreturn, x_node** tree);
	char m_affectation_0_2_3(string& lreturn, x_node** tree);
	char m_increment(string& lreturn, x_node** tree);
	char m_notin(string& lreturn, x_node** tree);
	char m_notin_0_1(string& lreturn, x_node** tree);
	char m_notin_0_2(string& lreturn, x_node** tree);
	char m_in(string& lreturn, x_node** tree);
	char m_on(string& lreturn, x_node** tree);
	char m_operatorin(string& lreturn, x_node** tree);
	char m_operationin(string& lreturn, x_node** tree);
	char m_operationin_0_1(string& lreturn, x_node** tree);
	char m_operationin_0_1_2(string& lreturn, x_node** tree);
	char m_operationon(string& lreturn, x_node** tree);
	char m_negation(string& lreturn, x_node** tree);
	char m_minus(string& lreturn, x_node** tree);
	char m_minus_0_1(string& lreturn, x_node** tree);
	char m_indexes(string& lreturn, x_node** tree);
	char m_indexes_0_1(string& lreturn, x_node** tree);
	char m_indexes_0_2(string& lreturn, x_node** tree);
	char m_indexes_0_2_3(string& lreturn, x_node** tree);
	char m_interval(string& lreturn, x_node** tree);
	char m_interval_0_1(string& lreturn, x_node** tree);
	char m_valtuple(string& lreturn, x_node** tree);
	char m_valtuple_0_1(string& lreturn, x_node** tree);
	char m_valtuple_0_1_2(string& lreturn, x_node** tree);
	char m_valtuple_0_1_2_3(string& lreturn, x_node** tree);
	char m_valtuple_0_1_2_3_4(string& lreturn, x_node** tree);
	char m_valvector(string& lreturn, x_node** tree);
	char m_valtail(string& lreturn, x_node** tree);
	char m_valvectortail(string& lreturn, x_node** tree);
	char m_valvectortail_0_1(string& lreturn, x_node** tree);
	char m_valvectortail_0_1_2(string& lreturn, x_node** tree);
	char m_valvectortail_0_1_2_3(string& lreturn, x_node** tree);
	char m_valvectortail_0_1_2_3_4(string& lreturn, x_node** tree);
	char m_valvectortail_0_1_2_5(string& lreturn, x_node** tree);
	char m_maptail(string& lreturn, x_node** tree);
	char m_dicoval(string& lreturn, x_node** tree);
	char m_valmaptail(string& lreturn, x_node** tree);
	char m_valmaptail_0_1(string& lreturn, x_node** tree);
	char m_valmaptail_0_1_2(string& lreturn, x_node** tree);
	char m_valmaptail_0_1_2_3(string& lreturn, x_node** tree);
	char m_valmaptail_0_1_2_3_4(string& lreturn, x_node** tree);
	char m_valmaptail_0_1_2_5(string& lreturn, x_node** tree);
	char m_intentionsep(string& lreturn, x_node** tree);
	char m_step(string& lreturn, x_node** tree);
	char m_intentionvector(string& lreturn, x_node** tree);
	char m_intentionvector_0_1(string& lreturn, x_node** tree);
	char m_intentionvector_0_1_2(string& lreturn, x_node** tree);
	char m_intentionvector_0_1_3(string& lreturn, x_node** tree);
	char m_intentionvector_0_1_3_4(string& lreturn, x_node** tree);
	char m_intentionvector_0_1_3_5(string& lreturn, x_node** tree);
	char m_intentionwithdouble(string& lreturn, x_node** tree);
	char m_intentionwithdouble_0_1(string& lreturn, x_node** tree);
	char m_intentionwithdouble_0_1_2(string& lreturn, x_node** tree);
	char m_intentionwithdouble_0_1_3(string& lreturn, x_node** tree);
	char m_intentionvect(string& lreturn, x_node** tree);
	char m_dico(string& lreturn, x_node** tree);
	char m_valmap(string& lreturn, x_node** tree);
	char m_valmap_0_1(string& lreturn, x_node** tree);
	char m_valmap_0_1_2(string& lreturn, x_node** tree);
	char m_valmap_0_1_2_3(string& lreturn, x_node** tree);
	char m_valmap_0_1_2_3_4(string& lreturn, x_node** tree);
	char m_jvector(string& lreturn, x_node** tree);
	char m_jvector_0_1(string& lreturn, x_node** tree);
	char m_jvector_0_1_2(string& lreturn, x_node** tree);
	char m_jvector_0_1_2_3(string& lreturn, x_node** tree);
	char m_jvector_0_1_2_3_4(string& lreturn, x_node** tree);
	char m_jdico(string& lreturn, x_node** tree);
	char m_jdico_0_1(string& lreturn, x_node** tree);
	char m_jmap(string& lreturn, x_node** tree);
	char m_jmap_0_1(string& lreturn, x_node** tree);
	char m_jmap_0_1_2(string& lreturn, x_node** tree);
	char m_jmap_0_1_2_3(string& lreturn, x_node** tree);
	char m_jmap_0_1_2_3_4(string& lreturn, x_node** tree);
	char m_jexpression(string& lreturn, x_node** tree);
	char m_cut(string& lreturn, x_node** tree);
	char m_fail(string& lreturn, x_node** tree);
	char m_predicate(string& lreturn, x_node** tree);
	char m_predicate_0_1(string& lreturn, x_node** tree);
	char m_predicate_0_1_2(string& lreturn, x_node** tree);
	char m_predicate_0_3(string& lreturn, x_node** tree);
	char m_predicatecall(string& lreturn, x_node** tree);
	char m_probaexpression(string& lreturn, x_node** tree);
	char m_retractcommand(string& lreturn, x_node** tree);
	char m_assertcommandbefore(string& lreturn, x_node** tree);
	char m_assertcommandafter(string& lreturn, x_node** tree);
	char m_assertweight(string& lreturn, x_node** tree);
	char m_assertpredicate(string& lreturn, x_node** tree);
	char m_assertpredicate_0_1(string& lreturn, x_node** tree);
	char m_assertpredicate_0_2(string& lreturn, x_node** tree);
	char m_predicatevariable(string& lreturn, x_node** tree);
	char m_term(string& lreturn, x_node** tree);
	char m_term_0_1(string& lreturn, x_node** tree);
	char m_tuple(string& lreturn, x_node** tree);
	char m_affectexpression(string& lreturn, x_node** tree);
	char m_affectexpression_0_1(string& lreturn, x_node** tree);
	char m_affectexpression_0_2(string& lreturn, x_node** tree);
	char m_affectexpression_0_2_3(string& lreturn, x_node** tree);
	char m_affectationpredicate(string& lreturn, x_node** tree);
	char m_affectationpredicate_0_1(string& lreturn, x_node** tree);
	char m_affectationpredicate_0_1_2(string& lreturn, x_node** tree);
	char m_comparepredicate(string& lreturn, x_node** tree);
	char m_comparepredicate_0_1(string& lreturn, x_node** tree);
	char m_pintentionvector(string& lreturn, x_node** tree);
	char m_pintentionvector_0_1(string& lreturn, x_node** tree);
	char m_pintentionvector_0_1_2(string& lreturn, x_node** tree);
	char m_pintentionvector_0_1_3(string& lreturn, x_node** tree);
	char m_pintentionvector_0_1_3_4(string& lreturn, x_node** tree);
	char m_pintentionwithdouble(string& lreturn, x_node** tree);
	char m_pintentionwithdouble_0_1(string& lreturn, x_node** tree);
	char m_pintentionwithdouble_0_1_2(string& lreturn, x_node** tree);
	char m_pintentionwithdouble_0_1_3(string& lreturn, x_node** tree);
	char m_pintentionvect(string& lreturn, x_node** tree);
	char m_merging(string& lreturn, x_node** tree);
	char m_apredicatelist(string& lreturn, x_node** tree);
	char m_apredicatelist_0_1(string& lreturn, x_node** tree);
	char m_apredicatelist_0_1_2(string& lreturn, x_node** tree);
	char m_apredicatelist_0_1_2_3(string& lreturn, x_node** tree);
	char m_apredicatelist_0_1_2_4(string& lreturn, x_node** tree);
	char m_apredicatelist_0_1_2_4_5(string& lreturn, x_node** tree);
	char m_valpredicatevector(string& lreturn, x_node** tree);
	char m_valpredicatevector_0_1(string& lreturn, x_node** tree);
	char m_pexpression(string& lreturn, x_node** tree);
	char m_predicateparameters(string& lreturn, x_node** tree);
	char m_predicateparameters_0_1(string& lreturn, x_node** tree);
	char m_predicateparameters_0_1_2(string& lreturn, x_node** tree);
	char m_mapmerging(string& lreturn, x_node** tree);
	char m_mappredicatelist(string& lreturn, x_node** tree);
	char m_mappredicatelist_0_1(string& lreturn, x_node** tree);
	char m_mappredicatelist_0_1_2(string& lreturn, x_node** tree);
	char m_mappredicatelist_0_1_2_3(string& lreturn, x_node** tree);
	char m_mappredicatelist_0_1_2_4(string& lreturn, x_node** tree);
	char m_valmappredicate(string& lreturn, x_node** tree);
	char m_valmappredicate_0_1(string& lreturn, x_node** tree);
	char m_predicatedico(string& lreturn, x_node** tree);
	char m_predicateoperator(string& lreturn, x_node** tree);
	char m_dboperation(string& lreturn, x_node** tree);
	char m_predicatedatabase(string& lreturn, x_node** tree);
	char m_predicateexpression(string& lreturn, x_node** tree);
	char m_predicateexpression_0_1(string& lreturn, x_node** tree);
	char m_predicateexpression_0_1_2(string& lreturn, x_node** tree);
	char m_predicateexpression_0_1_2_3(string& lreturn, x_node** tree);
	char m_predicateexpression_0_4(string& lreturn, x_node** tree);
	char m_predicateexpression_0_4_5(string& lreturn, x_node** tree);
	char m_predicatedefinition(string& lreturn, x_node** tree);
	char m_predicatedefinition_0_1(string& lreturn, x_node** tree);
	char m_predicatefact(string& lreturn, x_node** tree);
	char m_dcgword(string& lreturn, x_node** tree);
	char m_finaltoken(string& lreturn, x_node** tree);
	char m_finaltoken_0_1(string& lreturn, x_node** tree);
	char m_finaltoken_0_1_2(string& lreturn, x_node** tree);
	char m_finaltoken_0_1_2_3(string& lreturn, x_node** tree);
	char m_finaltoken_0_1_2_4(string& lreturn, x_node** tree);
	char m_finaltoken_0_1_2_4_5(string& lreturn, x_node** tree);
	char m_finaldcg(string& lreturn, x_node** tree);
	char m_dcg(string& lreturn, x_node** tree);
	char m_dcg_0_1(string& lreturn, x_node** tree);
	char m_dcg_0_1_2(string& lreturn, x_node** tree);
	char m_dcg_0_1_3(string& lreturn, x_node** tree);
	char m_dcg_0_1_3_4(string& lreturn, x_node** tree);
	char m_dcg_0_5(string& lreturn, x_node** tree);
	char m_dcg_0_5_6(string& lreturn, x_node** tree);
	char m_haskelltail(string& lreturn, x_node** tree);
	char m_haskelltail_0_1(string& lreturn, x_node** tree);
	char m_haskellexpression(string& lreturn, x_node** tree);
	char m_haskellvector(string& lreturn, x_node** tree);
	char m_haskellvector_0_1(string& lreturn, x_node** tree);
	char m_haskellvector_0_1_2(string& lreturn, x_node** tree);
	char m_haskellvector_0_1_2_3(string& lreturn, x_node** tree);
	char m_haskellvector_0_1_2_3_4(string& lreturn, x_node** tree);
	char m_haskellvector_0_1_2_5(string& lreturn, x_node** tree);
	char m_haskellmaptail(string& lreturn, x_node** tree);
	char m_haskellkeymap(string& lreturn, x_node** tree);
	char m_haskelldico(string& lreturn, x_node** tree);
	char m_haskellmap(string& lreturn, x_node** tree);
	char m_haskellmap_0_1(string& lreturn, x_node** tree);
	char m_haskellmap_0_1_2(string& lreturn, x_node** tree);
	char m_haskellmap_0_1_2_3(string& lreturn, x_node** tree);
	char m_haskellmap_0_1_2_3_4(string& lreturn, x_node** tree);
	char m_haskellmap_0_1_2_5(string& lreturn, x_node** tree);
	char m_letkeyword(string& lreturn, x_node** tree);
	char m_let(string& lreturn, x_node** tree);
	char m_let_0_2(string& lreturn, x_node** tree);
	char m_letmin(string& lreturn, x_node** tree);
	char m_letmin_0_1(string& lreturn, x_node** tree);
	char m_range(string& lreturn, x_node** tree);
	char m_range_0_1(string& lreturn, x_node** tree);
	char m_haskell(string& lreturn, x_node** tree);
	char m_haskell_0_1(string& lreturn, x_node** tree);
	char m_haskell_0_2(string& lreturn, x_node** tree);
	char m_haskell_0_2_3(string& lreturn, x_node** tree);
	char m_haskell_0_5(string& lreturn, x_node** tree);
	char m_otherwise(string& lreturn, x_node** tree);
	char m_guard(string& lreturn, x_node** tree);
	char m_guard_0_1(string& lreturn, x_node** tree);
	char m_hlambda(string& lreturn, x_node** tree);
	char m_hlambda_0_1(string& lreturn, x_node** tree);
	char m_hlambda_0_2(string& lreturn, x_node** tree);
	char m_hboollambda(string& lreturn, x_node** tree);
	char m_hfunction(string& lreturn, x_node** tree);
	char m_hfunction_0_1(string& lreturn, x_node** tree);
	char m_lfold(string& lreturn, x_node** tree);
	char m_rfold(string& lreturn, x_node** tree);
	char m_lscan(string& lreturn, x_node** tree);
	char m_rscan(string& lreturn, x_node** tree);
	char m_fold(string& lreturn, x_node** tree);
	char m_folding(string& lreturn, x_node** tree);
	char m_folding_0_1(string& lreturn, x_node** tree);
	char m_folding_0_2(string& lreturn, x_node** tree);
	char m_haskellbooloperator(string& lreturn, x_node** tree);
	char m_lfold1(string& lreturn, x_node** tree);
	char m_rfold1(string& lreturn, x_node** tree);
	char m_lscan1(string& lreturn, x_node** tree);
	char m_rscan1(string& lreturn, x_node** tree);
	char m_fold1(string& lreturn, x_node** tree);
	char m_folding1(string& lreturn, x_node** tree);
	char m_filterkeyword(string& lreturn, x_node** tree);
	char m_takekeyword(string& lreturn, x_node** tree);
	char m_inverted(string& lreturn, x_node** tree);
	char m_mapping(string& lreturn, x_node** tree);
	char m_mapping_0_1(string& lreturn, x_node** tree);
	char m_filtering(string& lreturn, x_node** tree);
	char m_filtering_0_1(string& lreturn, x_node** tree);
	char m_haskellbooling(string& lreturn, x_node** tree);
	char m_taking(string& lreturn, x_node** tree);
	char m_zipping(string& lreturn, x_node** tree);
	char m_zipping_0_1(string& lreturn, x_node** tree);
	char m_zipping_0_2(string& lreturn, x_node** tree);
	char m_pairing(string& lreturn, x_node** tree);
	char m_cycle(string& lreturn, x_node** tree);
	char m_cycling(string& lreturn, x_node** tree);
	char m_repeating(string& lreturn, x_node** tree);
	char m_flipping(string& lreturn, x_node** tree);
	char m_haskellcaseotherwise(string& lreturn, x_node** tree);
	char m_haskellresult(string& lreturn, x_node** tree);
	char m_haskellresult_0_1(string& lreturn, x_node** tree);
	char m_haskellcase(string& lreturn, x_node** tree);
	char m_haskellcase_0_1(string& lreturn, x_node** tree);
	char m_haskellcase_0_1_2(string& lreturn, x_node** tree);
	char m_haskellcase_0_3(string& lreturn, x_node** tree);
	char m_hfunctioncall(string& lreturn, x_node** tree);
	char m_hfunctioncall_0_1(string& lreturn, x_node** tree);
	char m_hfunctioncall_0_2(string& lreturn, x_node** tree);
	char m_hoperator(string& lreturn, x_node** tree);
	char m_hmetafunctions(string& lreturn, x_node** tree);
	char m_hcompose(string& lreturn, x_node** tree);
	char m_hcompose_0_1(string& lreturn, x_node** tree);
	char m_hcomposecall(string& lreturn, x_node** tree);
	char m_hoper(string& lreturn, x_node** tree);
	char m_hoperation(string& lreturn, x_node** tree);
	char m_hexpression(string& lreturn, x_node** tree);
	char m_hexpressions(string& lreturn, x_node** tree);
	char m_hexpressions_0_1(string& lreturn, x_node** tree);
	char m_whereexpression(string& lreturn, x_node** tree);
	char m_whereexpression_0_1(string& lreturn, x_node** tree);
	char m_whereexpression_0_1_2(string& lreturn, x_node** tree);
	char m_whereexpression_0_1_2_3(string& lreturn, x_node** tree);
	char m_whereexpression_0_1_2_3_4(string& lreturn, x_node** tree);
	char m_hinexpression(string& lreturn, x_node** tree);
	char m_telque(string& lreturn, x_node** tree);
	char m_telque_0_1(string& lreturn, x_node** tree);
	char m_telque_0_2(string& lreturn, x_node** tree);
	char m_telque_0_2_3(string& lreturn, x_node** tree);
	char m_telque_0_2_4(string& lreturn, x_node** tree);
	char m_telque_0_2_4_5(string& lreturn, x_node** tree);
	char m_telque_0_2_4_5_6(string& lreturn, x_node** tree);
	char m_telque_0_2_4_5_6_7(string& lreturn, x_node** tree);
	char m_telque_0_2_4_5_6_7_8(string& lreturn, x_node** tree);
	char m_telque_0_2_4_5_9(string& lreturn, x_node** tree);
	char m_telque_0_2_4_5_9_10(string& lreturn, x_node** tree);
	char m_telque_0_2_4_11(string& lreturn, x_node** tree);
	char m_telque_0_2_4_11_12(string& lreturn, x_node** tree);
	char m_telque_0_2_4_11_13(string& lreturn, x_node** tree);
	char m_telque_0_2_4_11_13_14(string& lreturn, x_node** tree);
	char m_telque_0_2_4_11_13_14_15(string& lreturn, x_node** tree);
	char m_telque_0_2_4_11_13_14_16(string& lreturn, x_node** tree);
	char m_telque_0_2_4_11_13_14_16_17(string& lreturn, x_node** tree);
	char m_telque_0_2_4_11_13_14_16_17_18(string& lreturn, x_node** tree);
	char m_telque_0_2_4_11_13_14_16_17_18_19(string& lreturn, x_node** tree);
	char m_telque_0_2_4_11_13_14_16_17_20(string& lreturn, x_node** tree);
	char m_telque_0_2_4_11_13_14_16_17_20_21(string& lreturn, x_node** tree);
	char m_telque_0_2_4_11_13_14_16_17_20_21_22(string& lreturn, x_node** tree);
	char m_telque_0_2_4_11_13_14_16_17_20_21_22_23(string& lreturn, x_node** tree);
	char m_optional(string& lreturn, x_node** tree);
	char m_expression(string& lreturn, x_node** tree);
	char m_expression_0_1(string& lreturn, x_node** tree);
	char m_expression_0_2(string& lreturn, x_node** tree);
	char m_wnexpressions(string& lreturn, x_node** tree);
	char m_wnexpressions_0_1(string& lreturn, x_node** tree);
	char m_wnexpressions_0_1_2(string& lreturn, x_node** tree);
	char m_nocomparisonexpressions(string& lreturn, x_node** tree);
	char m_nocomparisonexpressions_0_1(string& lreturn, x_node** tree);
	char m_nocomparisonexpressions_0_1_3(string& lreturn, x_node** tree);
	char m_nocomparisonexpressions_0_1_3_4(string& lreturn, x_node** tree);
	char m_nocomparisonnorinexpressions(string& lreturn, x_node** tree);
	char m_nocomparisonnorinexpressions_0_1(string& lreturn, x_node** tree);
	char m_nocomparisonnorinexpressions_0_1_3(string& lreturn, x_node** tree);
	char m_expressions(string& lreturn, x_node** tree);
	char m_expressions_0_1(string& lreturn, x_node** tree);
	char m_numbers(string& lreturn, x_node** tree);
	char m_numbers_0_1(string& lreturn, x_node** tree);
	char m_negated(string& lreturn, x_node** tree);
	char m_xnumber(string& lreturn, x_node** tree);
	char m_xnumber_0_1(string& lreturn, x_node** tree);
	char m_xnumber_0_1_2(string& lreturn, x_node** tree);
	char m_abool(string& lreturn, x_node** tree);
	char m_blocfor(string& lreturn, x_node** tree);
	char m_blocfor_0_1(string& lreturn, x_node** tree);
	char m_blocfor_0_2(string& lreturn, x_node** tree);
	char m_blocfor_0_2_3(string& lreturn, x_node** tree);
	char m_localif(string& lreturn, x_node** tree);
	char m_localif_0_1(string& lreturn, x_node** tree);
	char m_localif_0_1_2(string& lreturn, x_node** tree);
	char m_declarationfor(string& lreturn, x_node** tree);
	char m_declarationfor_0_1(string& lreturn, x_node** tree);
	char m_declarationfor_0_2(string& lreturn, x_node** tree);
	char m_optionalboolean(string& lreturn, x_node** tree);
	char m_booleanexpression(string& lreturn, x_node** tree);
	char m_booleanexpression_0_1(string& lreturn, x_node** tree);
	char m_booleanexpression_0_2(string& lreturn, x_node** tree);
	char m_booleanexpression_0_2_3(string& lreturn, x_node** tree);
	char m_booleanexpression_0_2_3_4(string& lreturn, x_node** tree);
	char m_arange(string& lreturn, x_node** tree);
	char m_test(string& lreturn, x_node** tree);
	char m_test_0_1(string& lreturn, x_node** tree);
	char m_test_0_2(string& lreturn, x_node** tree);
	char m_test_0_3(string& lreturn, x_node** tree);
	char m_test_0_3_4(string& lreturn, x_node** tree);
	char m_testelif(string& lreturn, x_node** tree);
	char m_loop(string& lreturn, x_node** tree);
	char m_doloop(string& lreturn, x_node** tree);
	char m_for(string& lreturn, x_node** tree);
	char m_for_0_1(string& lreturn, x_node** tree);
	char m_forin(string& lreturn, x_node** tree);
	char m_forin_0_1(string& lreturn, x_node** tree);
	char m_forin_0_1_2(string& lreturn, x_node** tree);
	char m_forin_0_1_2_3(string& lreturn, x_node** tree);
	char m_forin_0_4(string& lreturn, x_node** tree);
	char m_switch(string& lreturn, x_node** tree);
	char m_switch_0_2(string& lreturn, x_node** tree);
	char m_trycatch(string& lreturn, x_node** tree);
	char m_trycatch_0_1(string& lreturn, x_node** tree);
	char m_trycatch_0_1_2(string& lreturn, x_node** tree);
	char m_trycatch_0_1_2_3(string& lreturn, x_node** tree);
	char m_trycatch_0_4(string& lreturn, x_node** tree);
	char m_trycatch_0_4_5(string& lreturn, x_node** tree);
	char m_testswitch(string& lreturn, x_node** tree);
	char m_testswitch_0_1(string& lreturn, x_node** tree);
	char m_default(string& lreturn, x_node** tree);
	char m_lispexpression(string& lreturn, x_node** tree);
	char m_lispexpressions(string& lreturn, x_node** tree);
	char m_onelisp(string& lreturn, x_node** tree);
	char m_onelisp_0_1(string& lreturn, x_node** tree);
	char m_lisp(string& lreturn, x_node** tree);
	char m_lisp_0_1(string& lreturn, x_node** tree);
	char m_tagexpression(string& lreturn, x_node** tree);
	char m_tagexpressions(string& lreturn, x_node** tree);
	char m_onetag(string& lreturn, x_node** tree);
	char m_onetag_0_1(string& lreturn, x_node** tree);
	char m_tag(string& lreturn, x_node** tree);
	char m_tag_0_1(string& lreturn, x_node** tree);
	char m_ameta(string& lreturn, x_node** tree);
	char m_ameta_0_1(string& lreturn, x_node** tree);
	char m_ameta_0_1_2(string& lreturn, x_node** tree);
	char m_ameta_0_1_2_3(string& lreturn, x_node** tree);
	char m_ameta_0_4(string& lreturn, x_node** tree);
	char m_amulti(string& lreturn, x_node** tree);
	char m_asimple(string& lreturn, x_node** tree);
	char m_atoken(string& lreturn, x_node** tree);
	char m_atoken_0_2(string& lreturn, x_node** tree);
	char m_anitem(string& lreturn, x_node** tree);
	char m_dj(string& lreturn, x_node** tree);
	char m_aoptional(string& lreturn, x_node** tree);
	char m_aoptitem(string& lreturn, x_node** tree);
	char m_akleene(string& lreturn, x_node** tree);
	char m_anelement(string& lreturn, x_node** tree);
	char m_anelement_0_1(string& lreturn, x_node** tree);
	char m_anelement_0_1_2(string& lreturn, x_node** tree);
	char m_anitemelement(string& lreturn, x_node** tree);
	char m_anitemelement_0_1(string& lreturn, x_node** tree);
	char m_body(string& lreturn, x_node** tree);
	char m_body_0_1(string& lreturn, x_node** tree);
	char m_body_0_1_2(string& lreturn, x_node** tree);
	char m_body_0_1_2_3(string& lreturn, x_node** tree);
	char m_rule(string& lreturn, x_node** tree);
	char m_rule_0_1(string& lreturn, x_node** tree);
	char m_rule_0_1_2(string& lreturn, x_node** tree);
	char m_subgram(string& lreturn, x_node** tree);
	char m_subgram_0_1(string& lreturn, x_node** tree);
	char m_subgram_0_1_2(string& lreturn, x_node** tree);
	char m_subgram_0_1_2_3(string& lreturn, x_node** tree);
	char m_rules(string& lreturn, x_node** tree);
	char m_word(string& lreturn, x_node** tree);
	char m_punct(string& lreturn, x_node** tree);
	char m_typename(string& lreturn, x_node** tree);
	char m_astringdouble(string& lreturn, x_node** tree);
	char m_astringsimple(string& lreturn, x_node** tree);
	char m_afullstring(string& lreturn, x_node** tree);
	char m_anumber(string& lreturn, x_node** tree);
	char m_astring(string& lreturn, x_node** tree);
	char Y_var_1;
	char Y_var_0;
	string VS_var_3;
	string VS_var_2;
};

class bnf_tamgu_pop {
public:
	bnf_tamgu* pointer;

	bnf_tamgu_pop(bnf_tamgu* b) {
		pointer = b;
	}

	~bnf_tamgu_pop() {
		pointer->poperrors();
	}
};


#endif



