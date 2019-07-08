/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguboost.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamguboost_h
#define tamguboost_h

#ifdef Tamgu_REGEX
#if defined(MAVERICK) || defined(REGEXCPP) || defined(WIN32)
#include <regex>
using std::regex;
using std::sregex_token_iterator;
using std::smatch;
using std::match_results;
using std::wregex;
using std::wsregex_token_iterator;
using std::wsmatch;
using std::sregex_iterator;
using std::wsregex_iterator;
#else
#include <boost/regex.hpp>
using boost::regex;
using boost::sregex_token_iterator;
using boost::smatch;
using boost::match_results;
using boost::wregex;
using boost::wsregex_token_iterator;
using boost::wsmatch;
using boost::sregex_iterator;
using boost::wsregex_iterator;
#endif
#endif

#if defined(WIN32) || defined(MAVERICK) || defined(REGEXCPP)
#include <unordered_map>
#define hmap std::unordered_map
#else
using namespace boost;
#include <tr1/unordered_map>
#define hmap std::tr1::unordered_map
#endif

#endif
