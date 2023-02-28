#include "Parser.h"
#include "../Semantic/SemanticAnalyzer.h"

#include <sstream>

using std::string;
using std::unordered_map;
using std::unordered_set;

typedef Token::Type T;
typedef const string cs;

const unordered_map<string, unordered_set<string>> Parser::first_set_map = {
{"START", {"eof", "class", "function"}},
{"ARRAYSIZE2", {"intnum", "rsqbr"}},
{"CLASSDECL", {"class"}},
{"EXPR2", {"eq", "neq", "lt", "gt", "leq", "geq"}},
{"FACTOR2", {"lpar", "lsqbr"}},
{"FUNCDEF", {"function"}},
{"FUNCBODY", {"lcurbr"}},
{"FUNCHEAD", {"function"}},
{"FUNCHEADTAIL", {"sr", "lpar"}},
{"FUNCHEADMEMBERTAIL", {"id", "constructorkeyword"}},
{"IDNEST2", {"lpar", "lsqbr"}},
{"ARRAYOROBJECT", {"lpar", "lsqbr"}},
{"LOCALVARDECL", {"localvar"}},
{"MEMBERFUNCDECL", {"function", "constructorkeyword"}},
{"MEMBERFUNCHEAD", {"function", "constructorkeyword"}},
{"FPARAMS", {"id"}},
{"MEMBERVARDECL", {"attribute"}},
{"OPTINHERITS", {"isa"}},
{"PROG", {"class", "function"}},
{"ARITHEXPR", {"id", "intnum", "floatnum", "lpar", "not", "plus", "minus"}},
{"RELOP", {"eq", "neq", "lt", "gt", "leq", "geq"}},
{"APARAMSTAIL", {"comma"}},
{"REPTAPARAMS1", {"comma"}},
{"ARRAYSIZE", {"lsqbr"}},
{"REPTARRAYSIZE", {"lsqbr"}},
{"FPARAMSTAIL", {"comma"}},
{"REPTFPARAMS4", {"comma"}},
{"REPTINHERITSLIST", {"comma"}},
{"LOCALVARORSTAT", {"localvar", "id", "if", "while", "read", "write", "return"}},
{"REPTLOCALVARORSTAT", {"localvar", "id", "if", "while", "read", "write", "return"}},
{"MEMBERDECL", {"attribute", "function", "constructorkeyword"}},
{"REPTMEMBERDECL", {"public", "private"}},
{"CLASSDECLORFUNCDEF", {"class", "function"}},
{"REPTPROG0", {"class", "function"}},
{"IDNEST", {"dot"}},
{"REPTVARIABLEORFUNCTIONCALL", {"dot"}},
{"RETURNTYPE", {"void", "integer", "float", "id"}},
{"ADDOP", {"plus", "minus", "or"}},
{"RIGHTRECARITHEXPR", {"plus", "minus", "or"}},
{"MULTOP", {"mult", "div", "and"}},
{"SIGN", {"plus", "minus"}},
{"REPTSTATBLOCK1", {"id", "if", "while", "read", "write", "return"}},
{"STATEMENT", {"id", "if", "while", "read", "write", "return"}},
{"RELEXPR", {"id", "intnum", "floatnum", "lpar", "not", "plus", "minus"}},
{"STATBLOCK", {"lcurbr", "id", "if", "while", "read", "write", "return"}},
{"INDICE", {"lsqbr"}},
{"STATEMENTIDNEST2", {"dot"}},
{"STATEMENTIDNEST3", {"dot", "assign"}},
{"ASSIGNOP", {"assign"}},
{"EXPR", {"id", "intnum", "floatnum", "lpar", "not", "plus", "minus"}},
{"STATEMENTIDNEST", {"dot", "lpar", "lsqbr", "assign"}},
{"TERM", {"id", "intnum", "floatnum", "lpar", "not", "plus", "minus"}},
{"FACTOR", {"id", "intnum", "floatnum", "lpar", "not", "plus", "minus"}},
{"RIGHTRECTERM", {"mult", "div", "and"}},
{"TYPE", {"integer", "float", "id"}},
{"VARIABLE", {"id"}},
{"VARIABLE2", {"lpar", "lsqbr", "dot"}},
{"REPTVARIABLE", {"dot"}},
{"VARIDNEST2", {"lpar", "lsqbr"}},
{"APARAMS", {"id", "intnum", "floatnum", "lpar", "not", "plus", "minus"}},
{"VARIDNEST", {"dot"}},
{"REPTIDNEST1", {"lsqbr"}},
{"VISIBILITY", {"public", "private"}},
};

const unordered_map<string, unordered_set<string>> Parser::follow_set_map = {
{"ARRAYSIZE2", {"lsqbr", "rpar", "comma", "semi"}},
{"CLASSDECL", {"class", "function", "eof"}},
{"EXPR2", {"semi", "comma", "rpar"}},
{"FACTOR2", {"semi", "mult", "div", "and", "dot", "rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "plus", "minus", "or", "comma", "rpar"}},
{"FUNCDEF", {"class", "function", "eof"}},
{"FUNCBODY", {"class", "function", "eof"}},
{"FUNCHEAD", {"lcurbr"}},
{"FUNCHEADTAIL", {"lcurbr"}},
{"FUNCHEADMEMBERTAIL", {"lcurbr"}},
{"IDNEST2", {"semi", "mult", "div", "and", "dot", "rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "plus", "minus", "or", "comma", "rpar"}},
{"ARRAYOROBJECT", {"semi"}},
{"LOCALVARDECL", {"localvar", "id", "if", "while", "read", "write", "return", "rcurbr"}},
{"MEMBERFUNCDECL", {"public", "private", "rcurbr"}},
{"MEMBERFUNCHEAD", {"semi"}},
{"FPARAMS", {"rpar"}},
{"MEMBERVARDECL", {"public", "private", "rcurbr"}},
{"OPTINHERITS", {"lcurbr"}},
{"PROG", {"eof"}},
{"ARITHEXPR", {"semi", "rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "comma", "rpar"}},
{"RELOP", {"id", "intnum", "floatnum", "lpar", "not", "plus", "minus"}},
{"APARAMSTAIL", {"comma", "rpar"}},
{"REPTAPARAMS1", {"rpar"}},
{"ARRAYSIZE", {"lsqbr", "rpar", "comma", "semi"}},
{"REPTARRAYSIZE", {"rpar", "comma", "semi"}},
{"FPARAMSTAIL", {"comma", "rpar"}},
{"REPTFPARAMS4", {"rpar"}},
{"REPTINHERITSLIST", {"lcurbr"}},
{"LOCALVARORSTAT", {"localvar", "id", "if", "while", "read", "write", "return", "rcurbr"}},
{"REPTLOCALVARORSTAT", {"rcurbr"}},
{"MEMBERDECL", {"public", "private", "rcurbr"}},
{"REPTMEMBERDECL", {"rcurbr"}},
{"CLASSDECLORFUNCDEF", {"class", "function", "eof"}},
{"REPTPROG0", {"eof"}},
{"IDNEST", {"semi", "mult", "div", "and", "dot", "rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "plus", "minus", "or", "comma", "rpar"}},
{"REPTVARIABLEORFUNCTIONCALL", {"semi", "mult", "div", "and", "rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "plus", "minus", "or", "comma", "rpar"}},
{"RETURNTYPE", {"semi", "lcurbr"}},
{"ADDOP", {"id", "intnum", "floatnum", "lpar", "not", "plus", "minus"}},
{"RIGHTRECARITHEXPR", {"semi", "rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "comma", "rpar"}},
{"MULTOP", {"id", "intnum", "floatnum", "lpar", "not", "plus", "minus"}},
{"SIGN", {"id", "intnum", "floatnum", "lpar", "not", "plus", "minus"}},
{"REPTSTATBLOCK1", {"rcurbr"}},
{"STATEMENT", {"else", "semi", "localvar", "id", "if", "while", "read", "write", "return", "rcurbr"}},
{"RELEXPR", {"rpar"}},
{"STATBLOCK", {"else", "semi"}},
{"INDICE", {"semi", "mult", "div", "and", "lsqbr", "assign", "dot", "rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "plus", "minus", "or", "comma", "rpar"}},
{"STATEMENTIDNEST2", {"semi"}},
{"STATEMENTIDNEST3", {"semi"}},
{"ASSIGNOP", {"id", "intnum", "floatnum", "lpar", "not", "plus", "minus"}},
{"EXPR", {"semi", "comma", "rpar"}},
{"STATEMENTIDNEST", {"semi"}},
{"TERM", {"semi", "rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "plus", "minus", "or", "comma", "rpar"}},
{"FACTOR", {"semi", "mult", "div", "and", "rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "plus", "minus", "or", "comma", "rpar"}},
{"RIGHTRECTERM", {"semi", "rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "plus", "minus", "or", "comma", "rpar"}},
{"TYPE", {"rpar", "lcurbr", "comma", "lpar", "lsqbr", "semi"}},
{"VARIABLE", {"rpar"}},
{"VARIABLE2", {"rpar"}},
{"REPTVARIABLE", {"rpar"}},
{"VARIDNEST2", {"rpar", "dot"}},
{"APARAMS", {"rpar"}},
{"VARIDNEST", {"rpar", "dot"}},
{"REPTIDNEST1", {"assign", "semi", "mult", "div", "and", "dot", "rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "plus", "minus", "or", "comma", "rpar"}},
{"VISIBILITY", {"attribute", "function", "constructorkeyword"}},
};

const unordered_set<string> Parser::nullable_set = {
"FACTOR2",
"IDNEST2",
"ARRAYOROBJECT",
"FPARAMS",
"OPTINHERITS",
"PROG",
"REPTAPARAMS1",
"REPTARRAYSIZE",
"REPTFPARAMS4",
"REPTINHERITSLIST",
"REPTLOCALVARORSTAT",
"REPTMEMBERDECL",
"REPTPROG0",
"REPTVARIABLEORFUNCTIONCALL",
"RIGHTRECARITHEXPR",
"REPTSTATBLOCK1",
"STATBLOCK",
"STATEMENTIDNEST2",
"RIGHTRECTERM",
"VARIABLE2",
"REPTVARIABLE",
"VARIDNEST2",
"APARAMS",
"REPTIDNEST1",
};

const unordered_map<string, unordered_map<string, string>> Parser::parsing_map = initialize_parsing_map();

unordered_map<string, unordered_map<string, string>> Parser::initialize_parsing_map() {
	cs r1 = "PROG eof";
	cs r2 = "minus";
	cs r3 = "plus";
	cs r4 = "or";
	cs r5 = "&epsilon";
	cs r6 = "EXPR REPTAPARAMS1";
	cs r7 = "comma EXPR";
	cs r8 = "TERM RIGHTRECARITHEXPR";
	cs r9 = "lpar APARAMS rpar";
	cs r10 = "REPTARRAYSIZE";
	cs r11 = "lsqbr ARRAYSIZE2";
	cs r12 = "rsqbr";
	cs r13 = "intnum rsqbr";
	cs r14 = "assign";
	cs r15 = "class id OPTINHERITS lcurbr REPTMEMBERDECL rcurbr semi";
	cs r16 = "FUNCDEF";
	cs r17 = "CLASSDECL";
	cs r18 = "ARITHEXPR EXPR2";
	cs r19 = "RELOP ARITHEXPR";
	cs r20 = "lpar ARITHEXPR rpar";
	cs r21 = "id FACTOR2 REPTVARIABLEORFUNCTIONCALL";
	cs r22 = "SIGN FACTOR";
	cs r23 = "not FACTOR";
	cs r24 = "floatnum";
	cs r25 = "intnum";
	cs r26 = "REPTIDNEST1";
	cs r27 = "id colon TYPE REPTARRAYSIZE REPTFPARAMS4";
	cs r28 = "comma id colon TYPE REPTARRAYSIZE";
	cs r29 = "lcurbr REPTLOCALVARORSTAT rcurbr";
	cs r30 = "FUNCHEAD FUNCBODY";
	cs r31 = "function id FUNCHEADTAIL";
	cs r32 = "id lpar FPARAMS rpar arrow RETURNTYPE";
	cs r33 = "constructorkeyword lpar FPARAMS rpar";
	cs r34 = "lpar FPARAMS rpar arrow RETURNTYPE";
	cs r35 = "sr FUNCHEADMEMBERTAIL";
	cs r36 = "dot id IDNEST2";
	cs r37 = "lsqbr ARITHEXPR rsqbr";
	cs r38 = "localvar id colon TYPE ARRAYOROBJECT semi";
	cs r39 = "STATEMENT";
	cs r40 = "LOCALVARDECL";
	cs r41 = "MEMBERVARDECL";
	cs r42 = "MEMBERFUNCDECL";
	cs r43 = "MEMBERFUNCHEAD semi";
	cs r44 = "constructorkeyword colon lpar FPARAMS rpar";
	cs r45 = "function id colon lpar FPARAMS rpar arrow RETURNTYPE";
	cs r46 = "attribute id colon TYPE REPTARRAYSIZE semi";
	cs r47 = "and";
	cs r48 = "div";
	cs r49 = "mult";
	cs r50 = "isa id REPTINHERITSLIST";
	cs r51 = "REPTPROG0";
	cs r52 = "ARITHEXPR RELOP ARITHEXPR";
	cs r53 = "geq";
	cs r54 = "leq";
	cs r55 = "gt";
	cs r56 = "lt";
	cs r57 = "neq";
	cs r58 = "eq";
	cs r59 = "APARAMSTAIL REPTAPARAMS1";
	cs r60 = "ARRAYSIZE REPTARRAYSIZE";
	cs r61 = "FPARAMSTAIL REPTFPARAMS4";
	cs r62 = "INDICE REPTIDNEST1";
	cs r63 = "comma id REPTINHERITSLIST";
	cs r64 = "LOCALVARORSTAT REPTLOCALVARORSTAT";
	cs r65 = "VISIBILITY MEMBERDECL REPTMEMBERDECL";
	cs r66 = "CLASSDECLORFUNCDEF REPTPROG0";
	cs r67 = "STATEMENT REPTSTATBLOCK1";
	cs r68 = "VARIDNEST REPTVARIABLE";
	cs r69 = "IDNEST REPTVARIABLEORFUNCTIONCALL";
	cs r70 = "TYPE";
	cs r71 = "void";
	cs r72 = "ADDOP TERM RIGHTRECARITHEXPR";
	cs r73 = "MULTOP FACTOR RIGHTRECTERM";
	cs r74 = "lcurbr REPTSTATBLOCK1 rcurbr";
	cs r75 = "id STATEMENTIDNEST semi";
	cs r76 = "return lpar EXPR rpar semi";
	cs r77 = "write lpar EXPR rpar semi";
	cs r78 = "read lpar VARIABLE rpar semi";
	cs r79 = "while lpar RELEXPR rpar STATBLOCK semi";
	cs r80 = "if lpar RELEXPR rpar then STATBLOCK else STATBLOCK semi";
	cs r81 = "lpar APARAMS rpar STATEMENTIDNEST2";
	cs r82 = "dot id STATEMENTIDNEST";
	cs r83 = "INDICE REPTIDNEST1 STATEMENTIDNEST3";
	cs r84 = "ASSIGNOP EXPR";
	cs r85 = "FACTOR RIGHTRECTERM";
	cs r86 = "id";
	cs r87 = "float";
	cs r88 = "integer";
	cs r89 = "id VARIABLE2";
	cs r90 = "REPTIDNEST1 REPTVARIABLE";
	cs r91 = "lpar APARAMS rpar VARIDNEST";
	cs r92 = "dot id VARIDNEST2";
	cs r93 = "private";
	cs r94 = "public";

	cs t_ = "";
	cs t_$ = "$";
	cs t_private = "private";
	cs t_public = "public";
	cs t_rpar = "rpar";
	cs t_lpar = "lpar";
	cs t_id = "id";
	cs t_dot = "dot";
	cs t_float = "float";
	cs t_integer = "integer";
	cs t_semi = "semi";
	cs t_return = "return";
	cs t_write = "write";
	cs t_read = "read";
	cs t_while = "while";
	cs t_else = "else";
	cs t_then = "then";
	cs t_if = "if";
	cs t_rcurbr = "rcurbr";
	cs t_lcurbr = "lcurbr";
	cs t_minus = "minus";
	cs t_plus = "plus";
	cs t_void = "void";
	cs t_comma = "comma";
	cs t_geq = "geq";
	cs t_leq = "leq";
	cs t_gt = "gt";
	cs t_lt = "lt";
	cs t_neq = "neq";
	cs t_eq = "eq";
	cs t_isa = "isa";
	cs t_and = "and";
	cs t_div = "div";
	cs t_mult = "mult";
	cs t_colon = "colon";
	cs t_attribute = "attribute";
	cs t_constructorkeyword = "constructorkeyword";
	cs t_arrow = "arrow";
	cs t_function = "function";
	cs t_localvar = "localvar";
	cs t_rsqbr = "rsqbr";
	cs t_lsqbr = "lsqbr";
	cs t_sr = "sr";
	cs t_not = "not";
	cs t_floatnum = "floatnum";
	cs t_intnum = "intnum";
	cs t_class = "class";
	cs t_assign = "assign";
	cs t_or = "or";
	cs t_eof = "eof";

	unordered_map<string, unordered_map<string, string>> parsing_map = {
			{"START", {{t_function, r1},{t_class, r1},{t_eof, r1},}},
{"ADDOP", {{t_minus, r2},{t_plus, r3},{t_or, r4},}},
{"APARAMS", {{t_rpar, r5},{t_lpar, r6},{t_id, r6},{t_minus, r6},{t_plus, r6},{t_not, r6},{t_floatnum, r6},{t_intnum, r6},}},
{"APARAMSTAIL", {{t_comma, r7},}},
{"ARITHEXPR", {{t_lpar, r8},{t_id, r8},{t_minus, r8},{t_plus, r8},{t_not, r8},{t_floatnum, r8},{t_intnum, r8},}},
{"ARRAYOROBJECT", {{t_lpar, r9},{t_semi, r10},{t_lsqbr, r10},}},
{"ARRAYSIZE", {{t_lsqbr, r11},}},
{"ARRAYSIZE2", {{t_rsqbr, r12},{t_intnum, r13},}},
{"ASSIGNOP", {{t_assign, r14},}},
{"CLASSDECL", {{t_class, r15},}},
{"CLASSDECLORFUNCDEF", {{t_function, r16},{t_class, r17},}},
{"EXPR", {{t_lpar, r18},{t_id, r18},{t_minus, r18},{t_plus, r18},{t_not, r18},{t_floatnum, r18},{t_intnum, r18},}},
{"EXPR2", {{t_rpar, r5},{t_semi, r5},{t_comma, r5},{t_geq, r19},{t_leq, r19},{t_gt, r19},{t_lt, r19},{t_neq, r19},{t_eq, r19},}},
{"FACTOR", {{t_lpar, r20},{t_id, r21},{t_minus, r22},{t_plus, r22},{t_not, r23},{t_floatnum, r24},{t_intnum, r25},}},
{"FACTOR2", {{t_rpar, r26},{t_lpar, r9},{t_dot, r26},{t_semi, r26},{t_minus, r26},{t_plus, r26},{t_comma, r26},{t_geq, r26},{t_leq, r26},{t_gt, r26},{t_lt, r26},{t_neq, r26},{t_eq, r26},{t_and, r26},{t_div, r26},{t_mult, r26},{t_rsqbr, r26},{t_lsqbr, r26},{t_or, r26},}},
{"FPARAMS", {{t_rpar, r5},{t_id, r27},}},
{"FPARAMSTAIL", {{t_comma, r28},}},
{"FUNCBODY", {{t_lcurbr, r29},}},
{"FUNCDEF", {{t_function, r30},}},
{"FUNCHEAD", {{t_function, r31},}},
{"FUNCHEADMEMBERTAIL", {{t_id, r32},{t_constructorkeyword, r33},}},
{"FUNCHEADTAIL", {{t_lpar, r34},{t_sr, r35},}},
{"IDNEST", {{t_dot, r36},}},
{"IDNEST2", {{t_rpar, r26},{t_lpar, r9},{t_dot, r26},{t_semi, r26},{t_minus, r26},{t_plus, r26},{t_comma, r26},{t_geq, r26},{t_leq, r26},{t_gt, r26},{t_lt, r26},{t_neq, r26},{t_eq, r26},{t_and, r26},{t_div, r26},{t_mult, r26},{t_rsqbr, r26},{t_lsqbr, r26},{t_or, r26},}},
{"INDICE", {{t_lsqbr, r37},}},
{"LOCALVARDECL", {{t_localvar, r38},}},
{"LOCALVARORSTAT", {{t_id, r39},{t_return, r39},{t_write, r39},{t_read, r39},{t_while, r39},{t_if, r39},{t_localvar, r40},}},
{"MEMBERDECL", {{t_attribute, r41},{t_constructorkeyword, r42},{t_function, r42},}},
{"MEMBERFUNCDECL", {{t_constructorkeyword, r43},{t_function, r43},}},
{"MEMBERFUNCHEAD", {{t_constructorkeyword, r44},{t_function, r45},}},
{"MEMBERVARDECL", {{t_attribute, r46},}},
{"MULTOP", {{t_and, r47},{t_div, r48},{t_mult, r49},}},
{"OPTINHERITS", {{t_lcurbr, r5},{t_isa, r50},}},
{"PROG", {{t_function, r51},{t_class, r51},{t_eof, r51},}},
{"RELEXPR", {{t_lpar, r52},{t_id, r52},{t_minus, r52},{t_plus, r52},{t_not, r52},{t_floatnum, r52},{t_intnum, r52},}},
{"RELOP", {{t_geq, r53},{t_leq, r54},{t_gt, r55},{t_lt, r56},{t_neq, r57},{t_eq, r58},}},
{"REPTAPARAMS1", {{t_rpar, r5},{t_comma, r59},}},
{"REPTARRAYSIZE", {{t_rpar, r5},{t_semi, r5},{t_comma, r5},{t_lsqbr, r60},}},
{"REPTFPARAMS4", {{t_rpar, r5},{t_comma, r61},}},
{"REPTIDNEST1", {{t_rpar, r5},{t_dot, r5},{t_semi, r5},{t_minus, r5},{t_plus, r5},{t_comma, r5},{t_geq, r5},{t_leq, r5},{t_gt, r5},{t_lt, r5},{t_neq, r5},{t_eq, r5},{t_and, r5},{t_div, r5},{t_mult, r5},{t_rsqbr, r5},{t_lsqbr, r62},{t_assign, r5},{t_or, r5},}},
{"REPTINHERITSLIST", {{t_lcurbr, r5},{t_comma, r63},}},
{"REPTLOCALVARORSTAT", {{t_id, r64},{t_return, r64},{t_write, r64},{t_read, r64},{t_while, r64},{t_if, r64},{t_rcurbr, r5},{t_localvar, r64},}},
{"REPTMEMBERDECL", {{t_private, r65},{t_public, r65},{t_rcurbr, r5},}},
{"REPTPROG0", {{t_function, r66},{t_class, r66},{t_eof, r5},}},
{"REPTSTATBLOCK1", {{t_id, r67},{t_return, r67},{t_write, r67},{t_read, r67},{t_while, r67},{t_if, r67},{t_rcurbr, r5},}},
{"REPTVARIABLE", {{t_rpar, r5},{t_dot, r68},}},
{"REPTVARIABLEORFUNCTIONCALL", {{t_rpar, r5},{t_dot, r69},{t_semi, r5},{t_minus, r5},{t_plus, r5},{t_comma, r5},{t_geq, r5},{t_leq, r5},{t_gt, r5},{t_lt, r5},{t_neq, r5},{t_eq, r5},{t_and, r5},{t_div, r5},{t_mult, r5},{t_rsqbr, r5},{t_or, r5},}},
{"RETURNTYPE", {{t_id, r70},{t_float, r70},{t_integer, r70},{t_void, r71},}},
{"RIGHTRECARITHEXPR", {{t_rpar, r5},{t_semi, r5},{t_minus, r72},{t_plus, r72},{t_comma, r5},{t_geq, r5},{t_leq, r5},{t_gt, r5},{t_lt, r5},{t_neq, r5},{t_eq, r5},{t_rsqbr, r5},{t_or, r72},}},
{"RIGHTRECTERM", {{t_rpar, r5},{t_semi, r5},{t_minus, r5},{t_plus, r5},{t_comma, r5},{t_geq, r5},{t_leq, r5},{t_gt, r5},{t_lt, r5},{t_neq, r5},{t_eq, r5},{t_and, r73},{t_div, r73},{t_mult, r73},{t_rsqbr, r5},{t_or, r5},}},
{"SIGN", {{t_minus, r2},{t_plus, r3},}},
{"STATBLOCK", {{t_id, r39},{t_semi, r5},{t_return, r39},{t_write, r39},{t_read, r39},{t_while, r39},{t_else, r5},{t_if, r39},{t_lcurbr, r74},}},
{"STATEMENT", {{t_id, r75},{t_return, r76},{t_write, r77},{t_read, r78},{t_while, r79},{t_if, r80},}},
{"STATEMENTIDNEST", {{t_lpar, r81},{t_dot, r82},{t_lsqbr, r83},{t_assign, r84},}},
{"STATEMENTIDNEST2", {{t_dot, r82},{t_semi, r5},}},
{"STATEMENTIDNEST3", {{t_dot, r82},{t_assign, r84},}},
{"TERM", {{t_lpar, r85},{t_id, r85},{t_minus, r85},{t_plus, r85},{t_not, r85},{t_floatnum, r85},{t_intnum, r85},}},
{"TYPE", {{t_id, r86},{t_float, r87},{t_integer, r88},}},
{"VARIABLE", {{t_id, r89},}},
{"VARIABLE2", {{t_rpar, r90},{t_lpar, r91},{t_dot, r90},{t_lsqbr, r90},}},
{"VARIDNEST", {{t_dot, r92},}},
{"VARIDNEST2", {{t_rpar, r26},{t_lpar, r91},{t_dot, r26},{t_lsqbr, r26},}},
{"VISIBILITY", {{t_private, r93},{t_public, r94},}},
	};

	return parsing_map;
}


Parser::Parser(std::string filename) : la(LexicalAnalyzer(filename)), error(false) {
	derivation_file.open("output/outsyntax/" + filename + ".outderivation");
	error_file.open("output/outsyntax/" + filename + ".outsyntaxerrors");
}

Parser::Parser(std::string filename, SemanticAnalyzer* s_a) : la(LexicalAnalyzer(filename)), error(false), semantic_analyzer(s_a) {
	derivation_file.open("output/outsyntax/" + filename + ".outderivation");
	error_file.open("output/outsyntax/" + filename + ".outsyntaxerrors");
}

void Parser::print_stack() {
	std::stringstream ss;
	ss << left_side;
	std::stack<string> copy(stack);
	while (!copy.empty()) {
		ss << copy.top() + " ";
		copy.pop();
	}

	ss << std::endl;
	std::cout << ss.str();
	derivation_file << ss.str();
}

bool Parser::parse() {
	stack.push("$");
	stack.push("START");

	Token current_token = get_next_token();
	Token previous_token = current_token;

	while (stack.top() != "$") { //&& current_token.token_type != Token::Type::EndOfFile) {
		string top = stack.top();

		string current_token_string_type = Token::get_string(current_token.token_type);

		if (semantic_analyzer && semantic_analyzer->perform_semantic_action(top, previous_token)) {
			stack.pop();
		}
		else if (Token::is_token_type(top)) {
			if (top == current_token_string_type) {
				stack.pop();
				left_side += current_token_string_type + " ";
				print_stack();
				previous_token = current_token;
				current_token = get_next_token();
			}
			else {
				skip_errors(current_token);
				error = true;
			}
		}
		else {
			const unordered_map<string, string>& non_terminal_map = parsing_map.at(top);
			if (non_terminal_map.find(current_token_string_type) != non_terminal_map.end()) {
				stack.pop();
				inverse_push(non_terminal_map.at(current_token_string_type));
			}
			else {
				skip_errors(current_token);
				error = true;
			}
		}
	}

	if (current_token.token_type != T::EndOfFile || error) {
		return false;
	}
	else {
		return true;
	}
	derivation_file.close();
	error_file.close();
}

Token Parser::get_next_token() {
	Token t = la.get_next_token();
	while (t.token_type == T::BlockComment || t.token_type == T::LineComment) t = la.get_next_token();
	return t;
}


void Parser::inverse_push(string s) {
	if (s == "" || s[0] == '&') return;
	string tmp;
	std::stringstream ss(s);
	std::vector<string> words;

	while (getline(ss, tmp, ' ')) {
		words.push_back(tmp);
		
	}

	for (int i = words.size() - 1; i >= 0; i--) {
		stack.push(words[i]);
	}
	print_stack();
}

void Parser::pop() {
	stack.pop();
}

string Parser::top() {
	return stack.top();
}

void Parser::print_errors(const Token& current_token) {
	std::stringstream ss;
	ss << "Expected ";

	string grammmar_def = top();

	if (Token::is_token_type(grammmar_def)) {
		ss << grammmar_def;
	}
	else {
		auto non_terminal_first_set = first_set_map.at(grammmar_def);
		unordered_set<string> possible_elements(non_terminal_first_set);

		if (is_nullable(grammmar_def)) {
			unordered_set<string> follow_set = follow_set_map.at(grammmar_def);
			possible_elements.insert(follow_set.begin(), follow_set.end());
		}

		bool first = true;
		for (string first_element : possible_elements) {
			if (!first) {
				ss << ", ";
			}
			else {
				first = false;
			}
			ss << first_element;
		}
	}

	ss << ". Received " << Token::get_string(current_token.token_type) << " instead on line " 
		<< current_token.line_location << ", character index " << current_token.index_start << "." << std::endl;

	error_file << ss.str();
}

void Parser::skip_errors(Token& current_token) {

	std::cout << "syntax error at " << current_token.line_location << " from " << current_token.lexeme << std::endl;
	//error_file << "syntax error at " << current_token.line_location << " from a " << Token::get_string(current_token.token_type) << " (\"" << current_token.lexeme << "\")" << std::endl;

	print_errors(current_token);

	T token_type = current_token.token_type;
	if (Token::is_token_type(top()) || token_type == T::EndOfFile || in_follow(top(), token_type)) {
		pop();
	}
	else {
		while (!(in_first(top(), current_token.token_type) || (is_nullable(top()) && in_follow(top(), current_token.token_type))))
			current_token = get_next_token();
	}
	token_type;
}

bool Parser::in_first(string s, T token_type) {
	string token_string = Token::get_string(token_type);
	if (s == token_string) return true;
	if (first_set_map.find(s) != first_set_map.end()) {
		const unordered_set<std::string>& set = first_set_map.at(s);
		return set.find(token_string) != set.end();
	}
	else return false;
}

bool Parser::in_follow(string s, T token_type) {
	string token_string = Token::get_string(token_type);
	if (follow_set_map.find(s) != follow_set_map.end()) {
		const unordered_set<std::string>& set = follow_set_map.at(s);
		return set.find(token_string) != set.end();
	}
	else return false;
}

bool Parser::is_nullable(string s) {
	return nullable_set.find(s) != nullable_set.end();
}