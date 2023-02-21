#include "Parser.h"

#include <sstream>

using std::string;
using std::unordered_map;
using std::unordered_set;

typedef Token::Type T;
typedef const string cs;

const unordered_map<string, unordered_set<string>> Parser::first_set_map = {
	{"START", {"class", "function"}},
{"ARRAYSIZETAIL", {"intnum", "rsqbr"}},
{"ASSIGNOP", {"assign"}},
{"CLASSDECL", {"class"}},
{"EXPRTAIL", {"eq", "neq", "lt", "gt", "leq", "geq"}},
{"FACTORTAIL", {"lpar", "lsqbr"}},
{"FACTORIDTAIL", {"lsqbr", "id", "lpar", "dot"}},
{"CONTINUATION", {"id", "dot"}},
{"FPARAMSTAIL", {"comma"}},
{"REPTFPARAMS4", {"comma"}},
{"FUNCDEF", {"function"}},
{"FUNCBODY", {"lcurbr"}},
{"FUNCHEAD", {"function"}},
{"FUNCHEADTAIL", {"id"}},
{"FUNCHEADTAILTAIL", {"sr", "lpar"}},
{"FUNCHEADTAILTAILTAIL", {"id", "constructor"}},
{"LOCALVARDECLTAIL", {"semi", "lsqbr", "lpar"}},
{"LOCALVARDECL", {"localvar"}},
{"MEMBERFUNCDECL", {"function", "constructor"}},
{"FPARAMS", {"id"}},
{"MEMBERVARDECL", {"attribute"}},
{"OPTCLASSDECL2", {"isa"}},
{"APARAMS", {"id", "intnum", "floatnum", "lpar", "not", "plus", "minus"}},
{"ARITHEXPR", {"id", "intnum", "floatnum", "lpar", "not", "plus", "minus"}},
{"RELOP", {"eq", "neq", "lt", "gt", "leq", "geq"}},
{"APARAMSTAIL", {"comma"}},
{"REPTAPARAMS1", {"comma"}},
{"MEMBERDECL", {"function", "constructor", "attribute"}},
{"REPTCLASSDECL4", {"public", "private", "function", "constructor", "attribute"}},
{"ARRAYSIZE", {"lsqbr"}},
{"REPTARRAYSIZE", {"lsqbr"}},
{"LOCALVARDECLORSTMT", {"localvar", "id", "if", "while", "read", "write", "return"}},
{"REPTFUNCBODY1", {"localvar", "id", "if", "while", "read", "write", "return"}},
{"REPTIDNEST", {"dot"}},
{"REPTOPTCLASSDECL22", {"comma"}},
{"CLASSDECLORFUNCDEF", {"class", "function"}},
{"REPTSTART0", {"class", "function"}},
{"RETURNTYPE", {"void", "integer", "float", "id"}},
{"ADDOP", {"plus", "minus", "or"}},
{"RIGHTRECARITHEXPR", {"plus", "minus", "or"}},
{"MULTOP", {"mult", "div", "and"}},
{"SIGN", {"plus", "minus"}},
{"REPTSTATBLOCK1", {"id", "if", "while", "read", "write", "return"}},
{"STATEMENT", {"id", "if", "while", "read", "write", "return"}},
{"RELEXPR", {"id", "intnum", "floatnum", "lpar", "not", "plus", "minus"}},
{"STATBLOCK", {"lcurbr", "id", "if", "while", "read", "write", "return"}},
{"EXPR", {"id", "intnum", "floatnum", "lpar", "not", "plus", "minus"}},
{"STATEMENTTAIL", {"lsqbr", "dot", "lpar", "assign"}},
{"STATEMENTPARENT", {"semi", "dot"}},
{"STATEMENTBRACK", {"dot", "assign"}},
{"ASSIGNEND", {"assign"}},
{"STATEMENTPOINTNEST", {"lsqbr", "assign", "lpar"}},
{"ENDPARAMS", {"lpar"}},
{"VARIABLEBRACK", {"dot"}},
{"REPTIDNESTR", {"id"}},
{"REPTIDNESTRTAIL", {"lsqbr", "dot", "lpar"}},
{"POINTNESTR", {"dot"}},
{"IDNESTTAIL", {"dot", "lsqbr", "lpar"}},
{"INDICE", {"lsqbr"}},
{"REPTINDICE", {"lsqbr"}},
{"PARAMS", {"lpar"}},
{"TERM", {"id", "intnum", "floatnum", "lpar", "not", "plus", "minus"}},
{"FACTOR", {"id", "intnum", "floatnum", "lpar", "not", "plus", "minus"}},
{"RIGHTRECTERM", {"mult", "div", "and"}},
{"TYPE", {"integer", "float", "id"}},
{"VARIABLE", {"id"}},
{"VARIABLETAIL", {"lsqbr", "dot", "lpar"}},
{"VISIBILITY", {"public", "private"}},
};

const unordered_map<string, unordered_set<string>> Parser::follow_set_map = {
{"ARRAYSIZETAIL", {"lsqbr", "rpar", "comma", "semi"}},
{"ASSIGNOP", {"id", "intnum", "floatnum", "lpar", "not", "plus", "minus"}},
{"CLASSDECL", {"class", "function"}},
{"EXPRTAIL", {"comma", "semi", "rpar"}},
{"FACTORTAIL", {"rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "mult", "div", "and", "plus", "minus", "or", "comma", "semi", "rpar"}},
{"FACTORIDTAIL", {"rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "mult", "div", "and", "plus", "minus", "or", "comma", "semi", "rpar"}},
{"CONTINUATION", {"rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "mult", "div", "and", "plus", "minus", "or", "comma", "semi", "rpar"}},
{"FPARAMSTAIL", {"rpar", "comma"}},
{"REPTFPARAMS4", {"rpar"}},
{"FUNCDEF", {"class", "function"}},
{"FUNCBODY", {"class", "function"}},
{"FUNCHEAD", {"lcurbr"}},
{"FUNCHEADTAIL", {"lcurbr"}},
{"FUNCHEADTAILTAIL", {"lcurbr"}},
{"FUNCHEADTAILTAILTAIL", {"lcurbr"}},
{"LOCALVARDECLTAIL", {"localvar", "id", "if", "while", "read", "write", "return", "rcurbr"}},
{"LOCALVARDECL", {"localvar", "id", "if", "while", "read", "write", "return", "rcurbr"}},
{"MEMBERFUNCDECL", {"public", "private", "function", "constructor", "attribute", "rcurbr"}},
{"FPARAMS", {"rpar"}},
{"MEMBERVARDECL", {"public", "private", "function", "constructor", "attribute", "rcurbr"}},
{"OPTCLASSDECL2", {"lcurbr"}},
{"APARAMS", {"rpar"}},
{"ARITHEXPR", {"rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "comma", "semi", "rpar"}},
{"RELOP", {"id", "intnum", "floatnum", "lpar", "not", "plus", "minus"}},
{"APARAMSTAIL", {"comma", "rpar"}},
{"REPTAPARAMS1", {"rpar"}},
{"MEMBERDECL", {"public", "private", "function", "constructor", "attribute", "rcurbr"}},
{"REPTCLASSDECL4", {"rcurbr"}},
{"ARRAYSIZE", {"lsqbr", "rpar", "comma", "semi"}},
{"REPTARRAYSIZE", {"rpar", "comma", "semi"}},
{"LOCALVARDECLORSTMT", {"localvar", "id", "if", "while", "read", "write", "return", "rcurbr"}},
{"REPTFUNCBODY1", {"rcurbr"}},
{"REPTIDNEST", {"id"}},
{"REPTOPTCLASSDECL22", {"lcurbr"}},
{"CLASSDECLORFUNCDEF", {"class", "function"}},
{"RETURNTYPE", {"semi", "lcurbr"}},
{"ADDOP", {"id", "intnum", "floatnum", "lpar", "not", "plus", "minus"}},
{"RIGHTRECARITHEXPR", {"rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "comma", "semi", "rpar"}},
{"MULTOP", {"id", "intnum", "floatnum", "lpar", "not", "plus", "minus"}},
{"SIGN", {"id", "intnum", "floatnum", "lpar", "not", "plus", "minus"}},
{"REPTSTATBLOCK1", {"rcurbr"}},
{"STATEMENT", {"else", "semi", "localvar", "id", "if", "while", "read", "write", "return", "rcurbr"}},
{"RELEXPR", {"rpar"}},
{"STATBLOCK", {"else", "semi"}},
{"EXPR", {"comma", "semi", "rpar"}},
{"STATEMENTTAIL", {"else", "semi", "localvar", "id", "if", "while", "read", "write", "return", "rcurbr"}},
{"STATEMENTPARENT", {"else", "semi", "localvar", "id", "if", "while", "read", "write", "return", "rcurbr"}},
{"STATEMENTBRACK", {"else", "semi", "localvar", "id", "if", "while", "read", "write", "return", "rcurbr"}},
{"ASSIGNEND", {"else", "semi", "localvar", "id", "if", "while", "read", "write", "return", "rcurbr"}},
{"STATEMENTPOINTNEST", {"else", "semi", "localvar", "id", "if", "while", "read", "write", "return", "rcurbr"}},
{"ENDPARAMS", {"else", "semi", "localvar", "id", "if", "while", "read", "write", "return", "rcurbr"}},
{"VARIABLEBRACK", {"rpar"}},
{"REPTIDNESTR", {"id"}},
{"REPTIDNESTRTAIL", {"id"}},
{"POINTNESTR", {"id", "assign", "lpar", "lsqbr", "rpar"}},
{"IDNESTTAIL", {"dot", "id"}},
{"INDICE", {"id", "assign", "lsqbr", "dot", "rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "mult", "div", "and", "plus", "minus", "or", "comma", "semi", "rpar"}},
{"REPTINDICE", {"id", "assign", "dot", "rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "mult", "div", "and", "plus", "minus", "or", "comma", "semi", "rpar"}},
{"PARAMS", {"id", "dot", "rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "mult", "div", "and", "plus", "minus", "or", "comma", "semi", "rpar"}},
{"TERM", {"rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "plus", "minus", "or", "comma", "semi", "rpar"}},
{"FACTOR", {"rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "mult", "div", "and", "plus", "minus", "or", "comma", "semi", "rpar"}},
{"RIGHTRECTERM", {"rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "plus", "minus", "or", "comma", "semi", "rpar"}},
{"TYPE", {"rpar", "lcurbr", "comma", "lpar", "lsqbr", "semi"}},
{"VARIABLE", {"rpar"}},
{"VARIABLETAIL", {"rpar"}},
{"VISIBILITY", {"function", "constructor", "attribute"}},
};

const unordered_set<string> Parser::nullable_set = {
"EXPRTAIL",
"FACTORTAIL",
"FACTORIDTAIL",
"REPTFPARAMS4",
"FPARAMS",
"OPTCLASSDECL2",
"APARAMS",
"REPTAPARAMS1",
"REPTCLASSDECL4",
"REPTARRAYSIZE",
"REPTFUNCBODY1",
"REPTIDNEST",
"REPTOPTCLASSDECL22",
"REPTSTART0",
"RIGHTRECARITHEXPR",
"REPTSTATBLOCK1",
"STATBLOCK",
"VARIABLEBRACK",
"REPTIDNESTRTAIL",
"REPTINDICE",
"RIGHTRECTERM",
"VARIABLETAIL",
"VISIBILITY",
};

const unordered_map<string, unordered_map<string, string>> Parser::parsing_map = initialize_parsing_map();

unordered_map<string, unordered_map<string, string>> Parser::initialize_parsing_map() {
	cs r1 = "REPTSTART0";
	cs r2 = "minus";
	cs r3 = "plus";
	cs r4 = "or";
	cs r5 = "EXPR REPTAPARAMS1";
	cs r6 = "&epsilon";
	cs r7 = "comma EXPR";
	cs r8 = "TERM RIGHTRECARITHEXPR";
	cs r9 = "lsqbr ARRAYSIZETAIL";
	cs r10 = "rsqbr";
	cs r11 = "intnum rsqbr";
	cs r12 = "assign";
	cs r13 = "ASSIGNOP EXPR semi";
	cs r14 = "class id OPTCLASSDECL2 lcurbr REPTCLASSDECL4 rcurbr semi";
	cs r15 = "FUNCDEF";
	cs r16 = "CLASSDECL";
	cs r17 = "REPTIDNEST id FACTORTAIL";
	cs r18 = "FACTOR RIGHTRECTERM RIGHTRECARITHEXPR EXPRTAIL";
	cs r19 = "RELOP ARITHEXPR";
	cs r20 = "id FACTORIDTAIL";
	cs r21 = "lpar ARITHEXPR rpar";
	cs r22 = "SIGN FACTOR";
	cs r23 = "not FACTOR";
	cs r24 = "floatnum";
	cs r25 = "intnum";
	cs r26 = "REPTINDICE";
	cs r27 = "PARAMS";
	cs r28 = "CONTINUATION";
	cs r29 = "PARAMS CONTINUATION";
	cs r30 = "INDICE REPTINDICE CONTINUATION";
	cs r31 = "id colon TYPE REPTARRAYSIZE REPTFPARAMS4";
	cs r32 = "comma id colon TYPE REPTARRAYSIZE";
	cs r33 = "FPARAMSTAIL REPTFPARAMS4";
	cs r34 = "lcurbr REPTFUNCBODY1 rcurbr";
	cs r35 = "FUNCHEAD FUNCBODY";
	cs r36 = "function FUNCHEADTAIL";
	cs r37 = "id FUNCHEADTAILTAIL";
	cs r38 = "lpar FPARAMS rpar arrow RETURNTYPE";
	cs r39 = "sr FUNCHEADTAILTAILTAIL";
	cs r40 = "id lpar FPARAMS rpar arrow RETURNTYPE";
	cs r41 = "constructor lpar FPARAMS rpar";
	cs r42 = "lsqbr ARITHEXPR rsqbr";
	cs r43 = "localvar id colon TYPE LOCALVARDECLTAIL";
	cs r44 = "REPTARRAYSIZE semi";
	cs r45 = "ENDPARAMS";
	cs r46 = "STATEMENT";
	cs r47 = "LOCALVARDECL";
	cs r48 = "MEMBERVARDECL";
	cs r49 = "MEMBERFUNCDECL";
	cs r50 = "constructor colon lpar FPARAMS rpar semi";
	cs r51 = "function id colon lpar FPARAMS rpar arrow RETURNTYPE semi";
	cs r52 = "attribute id colon TYPE REPTARRAYSIZE semi";
	cs r53 = "and";
	cs r54 = "div";
	cs r55 = "mult";
	cs r56 = "isa id REPTOPTCLASSDECL22";
	cs r57 = "lpar APARAMS rpar";
	cs r58 = "ARITHEXPR RELOP ARITHEXPR";
	cs r59 = "geq";
	cs r60 = "leq";
	cs r61 = "gt";
	cs r62 = "lt";
	cs r63 = "neq";
	cs r64 = "eq";
	cs r65 = "APARAMSTAIL REPTAPARAMS1";
	cs r66 = "VISIBILITY MEMBERDECL REPTCLASSDECL4";
	cs r67 = "ARRAYSIZE REPTARRAYSIZE";
	cs r68 = "LOCALVARDECLORSTMT REPTFUNCBODY1";
	cs r69 = "dot id IDNESTTAIL REPTIDNEST";
	cs r70 = "comma id REPTOPTCLASSDECL22";
	cs r71 = "CLASSDECLORFUNCDEF REPTSTART0";
	cs r72 = "STATEMENT REPTSTATBLOCK1";
	cs r73 = "TYPE";
	cs r74 = "void";
	cs r75 = "ADDOP TERM RIGHTRECARITHEXPR";
	cs r76 = "MULTOP FACTOR RIGHTRECTERM";
	cs r77 = "lcurbr REPTSTATBLOCK1 rcurbr";
	cs r78 = "id STATEMENTTAIL";
	cs r79 = "return lpar EXPR rpar semi";
	cs r80 = "write lpar EXPR rpar semi";
	cs r81 = "read lpar VARIABLE rpar semi";
	cs r82 = "while lpar RELEXPR rpar STATBLOCK semi";
	cs r83 = "if lpar RELEXPR rpar then STATBLOCK else STATBLOCK semi";
	cs r84 = "POINTNESTR STATEMENTPOINTNEST";
	cs r85 = "PARAMS STATEMENTPARENT";
	cs r86 = "INDICE REPTINDICE STATEMENTBRACK";
	cs r87 = "ASSIGNEND";
	cs r88 = "semi";
	cs r89 = "REPTINDICE ASSIGNEND";
	cs r90 = "PARAMS semi";
	cs r91 = "POINTNESTR REPTINDICE";
	cs r92 = "PARAMS POINTNESTR REPTINDICE";
	cs r93 = "INDICE REPTINDICE VARIABLEBRACK";
	cs r94 = "INDICE REPTINDICE";
	cs r95 = "dot REPTIDNESTR id";
	cs r96 = "id REPTIDNESTRTAIL";
	cs r97 = "POINTNESTR";
	cs r98 = "PARAMS POINTNESTR";
	cs r99 = "INDICE REPTINDICE POINTNESTR";
	cs r100 = "dot";
	cs r101 = "PARAMS dot";
	cs r102 = "INDICE REPTINDICE dot";
	cs r103 = "FACTOR RIGHTRECTERM";
	cs r104 = "id";
	cs r105 = "float";
	cs r106 = "integer";
	cs r107 = "id VARIABLETAIL";
	cs r108 = "private";
	cs r109 = "public";

	cs t_ = "";
	cs t_$ = "$";
	cs t_private = "private";
	cs t_public = "public";
	cs t_id = "id";
	cs t_float = "float";
	cs t_integer = "integer";
	cs t_dot = "dot";
	cs t_semi = "semi";
	cs t_rpar = "rpar";
	cs t_lpar = "lpar";
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
	cs t_constructor = "constructor";
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

	unordered_map<string, unordered_map<string, string>> parsing_map = {
			{"START", {{t_$, r1},{t_function, r1},{t_class, r1},}},
{"ADDOP", {{t_minus, r2},{t_plus, r3},{t_or, r4},}},
{"APARAMS", {{t_id, r5},{t_rpar, r6},{t_lpar, r5},{t_minus, r5},{t_plus, r5},{t_not, r5},{t_floatnum, r5},{t_intnum, r5},}},
{"APARAMSTAIL", {{t_comma, r7},}},
{"ARITHEXPR", {{t_id, r8},{t_lpar, r8},{t_minus, r8},{t_plus, r8},{t_not, r8},{t_floatnum, r8},{t_intnum, r8},}},
{"ARRAYSIZE", {{t_lsqbr, r9},}},
{"ARRAYSIZETAIL", {{t_rsqbr, r10},{t_intnum, r11},}},
{"ASSIGNOP", {{t_assign, r12},}},
{"ASSIGNEND", {{t_assign, r13},}},
{"CLASSDECL", {{t_class, r14},}},
{"CLASSDECLORFUNCDEF", {{t_function, r15},{t_class, r16},}},
{"CONTINUATION", {{t_id, r17},{t_dot, r17},}},
{"EXPR", {{t_id, r18},{t_lpar, r18},{t_minus, r18},{t_plus, r18},{t_not, r18},{t_floatnum, r18},{t_intnum, r18},}},
{"EXPRTAIL", {{t_semi, r6},{t_rpar, r6},{t_comma, r6},{t_geq, r19},{t_leq, r19},{t_gt, r19},{t_lt, r19},{t_neq, r19},{t_eq, r19},}},
{"FACTOR", {{t_id, r20},{t_lpar, r21},{t_minus, r22},{t_plus, r22},{t_not, r23},{t_floatnum, r24},{t_intnum, r25},}},
{"FACTORTAIL", {{t_semi, r26},{t_rpar, r26},{t_lpar, r27},{t_minus, r26},{t_plus, r26},{t_comma, r26},{t_geq, r26},{t_leq, r26},{t_gt, r26},{t_lt, r26},{t_neq, r26},{t_eq, r26},{t_and, r26},{t_div, r26},{t_mult, r26},{t_rsqbr, r26},{t_lsqbr, r26},{t_or, r26},}},
{"FACTORIDTAIL", {{t_id, r28},{t_dot, r28},{t_semi, r6},{t_rpar, r6},{t_lpar, r29},{t_minus, r6},{t_plus, r6},{t_comma, r6},{t_geq, r6},{t_leq, r6},{t_gt, r6},{t_lt, r6},{t_neq, r6},{t_eq, r6},{t_and, r6},{t_div, r6},{t_mult, r6},{t_rsqbr, r6},{t_lsqbr, r30},{t_or, r6},}},
{"FPARAMS", {{t_id, r31},{t_rpar, r6},}},
{"FPARAMSTAIL", {{t_comma, r32},}},
{"REPTFPARAMS4", {{t_rpar, r6},{t_comma, r33},}},
{"FUNCBODY", {{t_lcurbr, r34},}},
{"FUNCDEF", {{t_function, r35},}},
{"FUNCHEAD", {{t_function, r36},}},
{"FUNCHEADTAIL", {{t_id, r37},}},
{"FUNCHEADTAILTAIL", {{t_lpar, r38},{t_sr, r39},}},
{"FUNCHEADTAILTAILTAIL", {{t_id, r40},{t_constructor, r41},}},
{"INDICE", {{t_lsqbr, r42},}},
{"LOCALVARDECL", {{t_localvar, r43},}},
{"LOCALVARDECLTAIL", {{t_semi, r44},{t_lpar, r45},{t_lsqbr, r44},}},
{"LOCALVARDECLORSTMT", {{t_id, r46},{t_return, r46},{t_write, r46},{t_read, r46},{t_while, r46},{t_if, r46},{t_localvar, r47},}},
{"MEMBERDECL", {{t_attribute, r48},{t_constructor, r49},{t_function, r49},}},
{"MEMBERFUNCDECL", {{t_constructor, r50},{t_function, r51},}},
{"MEMBERVARDECL", {{t_attribute, r52},}},
{"MULTOP", {{t_and, r53},{t_div, r54},{t_mult, r55},}},
{"OPTCLASSDECL2", {{t_lcurbr, r6},{t_isa, r56},}},
{"PARAMS", {{t_lpar, r57},}},
{"RELEXPR", {{t_id, r58},{t_lpar, r58},{t_minus, r58},{t_plus, r58},{t_not, r58},{t_floatnum, r58},{t_intnum, r58},}},
{"RELOP", {{t_geq, r59},{t_leq, r60},{t_gt, r61},{t_lt, r62},{t_neq, r63},{t_eq, r64},}},
{"REPTAPARAMS1", {{t_rpar, r6},{t_comma, r65},}},
{"REPTCLASSDECL4", {{t_private, r66},{t_public, r66},{t_rcurbr, r6},{t_attribute, r66},{t_constructor, r66},{t_function, r66},}},
{"REPTARRAYSIZE", {{t_semi, r6},{t_rpar, r6},{t_comma, r6},{t_lsqbr, r67},}},
{"REPTFUNCBODY1", {{t_id, r68},{t_return, r68},{t_write, r68},{t_read, r68},{t_while, r68},{t_if, r68},{t_rcurbr, r6},{t_localvar, r68},}},
{"REPTIDNEST", {{t_id, r6},{t_dot, r69},}},
{"REPTOPTCLASSDECL22", {{t_lcurbr, r6},{t_comma, r70},}},
{"REPTSTART0", {{t_$, r6},{t_function, r71},{t_class, r71},}},
{"REPTSTATBLOCK1", {{t_id, r72},{t_return, r72},{t_write, r72},{t_read, r72},{t_while, r72},{t_if, r72},{t_rcurbr, r6},}},
{"RETURNTYPE", {{t_id, r73},{t_float, r73},{t_integer, r73},{t_void, r74},}},
{"RIGHTRECARITHEXPR", {{t_semi, r6},{t_rpar, r6},{t_minus, r75},{t_plus, r75},{t_comma, r6},{t_geq, r6},{t_leq, r6},{t_gt, r6},{t_lt, r6},{t_neq, r6},{t_eq, r6},{t_rsqbr, r6},{t_or, r75},}},
{"RIGHTRECTERM", {{t_semi, r6},{t_rpar, r6},{t_minus, r6},{t_plus, r6},{t_comma, r6},{t_geq, r6},{t_leq, r6},{t_gt, r6},{t_lt, r6},{t_neq, r6},{t_eq, r6},{t_and, r76},{t_div, r76},{t_mult, r76},{t_rsqbr, r6},{t_or, r6},}},
{"SIGN", {{t_minus, r2},{t_plus, r3},}},
{"STATBLOCK", {{t_id, r46},{t_semi, r6},{t_return, r46},{t_write, r46},{t_read, r46},{t_while, r46},{t_else, r6},{t_if, r46},{t_lcurbr, r77},}},
{"STATEMENT", {{t_id, r78},{t_return, r79},{t_write, r80},{t_read, r81},{t_while, r82},{t_if, r83},}},
{"STATEMENTTAIL", {{t_dot, r84},{t_lpar, r85},{t_lsqbr, r86},{t_assign, r87},}},
{"STATEMENTPARENT", {{t_dot, r84},{t_semi, r88},}},
{"STATEMENTPOINTNEST", {{t_lpar, r45},{t_lsqbr, r89},{t_assign, r89},}},
{"STATEMENTBRACK", {{t_dot, r84},{t_assign, r87},}},
{"ENDPARAMS", {{t_lpar, r90},}},
{"VARIABLEBRACK", {{t_dot, r91},{t_rpar, r6},}},
{"VARIABLETAIL", {{t_dot, r91},{t_rpar, r6},{t_lpar, r92},{t_lsqbr, r93},}},
{"REPTINDICE", {{t_id, r6},{t_dot, r6},{t_semi, r6},{t_rpar, r6},{t_minus, r6},{t_plus, r6},{t_comma, r6},{t_geq, r6},{t_leq, r6},{t_gt, r6},{t_lt, r6},{t_neq, r6},{t_eq, r6},{t_and, r6},{t_div, r6},{t_mult, r6},{t_rsqbr, r6},{t_lsqbr, r94},{t_assign, r6},{t_or, r6},}},
{"POINTNESTR", {{t_dot, r95},}},
{"REPTIDNESTR", {{t_id, r96},}},
{"REPTIDNESTRTAIL", {{t_id, r6},{t_dot, r97},{t_lpar, r98},{t_lsqbr, r99},}},
{"IDNESTTAIL", {{t_dot, r100},{t_lpar, r101},{t_lsqbr, r102},}},
{"TERM", {{t_id, r103},{t_lpar, r103},{t_minus, r103},{t_plus, r103},{t_not, r103},{t_floatnum, r103},{t_intnum, r103},}},
{"TYPE", {{t_id, r104},{t_float, r105},{t_integer, r106},}},
{"VARIABLE", {{t_id, r107},}},
{"VISIBILITY", {{t_private, r108},{t_public, r109},{t_attribute, r6},{t_constructor, r6},{t_function, r6},}},
	};

	return parsing_map;
}


Parser::Parser(std::string filename) : la(LexicalAnalyzer(filename)), error(false) {
	derivation_file.open("output/" + filename + ".outderivation");
	error_file.open("output/" + filename + ".outsyntaxerrors");
}

void Parser::print_stack() {
	derivation_file << left_side;
	std::stack<string> copy(stack);
	while (!copy.empty()) {
		derivation_file << copy.top() + " ";
		copy.pop();
	}

	derivation_file << std::endl;
}

bool Parser::parse() {
	stack.push("$");
	stack.push("START");

	Token current_token = get_next_token();

	while (stack.top() != "$" && current_token.token_type != Token::Type::EndOfFile) {
		string top = stack.top();

		string current_token_string_type = Token::get_string(current_token.token_type);

		std::cout << current_token_string_type << std::endl;

		if (Token::is_token_type(top)) {
			if (top == current_token_string_type) {
				stack.pop();
				left_side += current_token_string_type + " ";
				print_stack();
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
	if (s == "") return;
	string tmp;
	std::stringstream ss(s);
	std::vector<string> words;

	while (getline(ss, tmp, ' ')) {
		words.push_back(tmp);
		
	}

	for (int i = words.size() - 1; i >= 0; i--) {
		stack.push(words[i]);
	}
}

void Parser::pop() {
	stack.pop();
}

string Parser::top() {
	return stack.top();
}

void Parser::skip_errors(Token& current_token) {
	if (prev.token_type == current_token.token_type) {
		current_token = get_next_token();
		return;
	}
	prev = current_token;
	std::cout << "syntax error at " << current_token.line_location << " from " << current_token.lexeme << std::endl;
	error_file << "syntax error at " << current_token.line_location << " from " << current_token.lexeme << std::endl;
	T token_type = current_token.token_type;
	if (token_type == T::EndOfFile || in_follow(top(), token_type)) {
		pop();
	}
	else {
		while (in_first(top(), current_token.token_type) || (is_nullable(top()) && in_follow(top(), current_token.token_type)))
			current_token = get_next_token();
	}
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