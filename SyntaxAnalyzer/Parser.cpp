#include "Parser.h"
#include "../Semantic/ASTGenerator.h"

#include <sstream>

using std::string;
using std::unordered_map;
using std::unordered_set;

typedef Token::Type T;
typedef const string cs;

const unordered_map<string, unordered_set<string>> Parser::first_set_map = {
{"START", {"eof", "class", "function"}},
{"ARRAYSIZE2", {"rsqbr", "intnum"}},
{"CLASSDECL", {"class"}},
{"EXPR2", {"eq", "neq", "lt", "gt", "leq", "geq"}},
{"INTNUM", {"intnum"}},
{"FACTOR2", {"lpar", "lsqbr"}},
{"FUNCDEF", {"function"}},
{"FUNCBODY", {"lcurbr"}},
{"FUNCHEAD", {"function"}},
{"FPARAMS", {"id"}},
{"FUNCHEADTAIL", {"sr", "lpar"}},
{"FUNCHEADMEMBERTAIL", {"constructorkeyword", "id"}},
{"IDNEST2", {"lpar", "lsqbr"}},
{"ARRAYOROBJECT", {"lpar", "lsqbr"}},
{"LOCALVARDECL", {"localvar"}},
{"MEMBERFUNCDECL", {"function", "constructorkeyword"}},
{"MEMBERFUNCHEAD", {"function", "constructorkeyword"}},
{"ARROWRETURNTYPE", {"arrow"}},
{"PARAMLIST", {"lpar"}},
{"MEMBERVARDECL", {"attribute"}},
{"VARDECL", {"id"}},
{"OPTINHERITS", {"isa"}},
{"PROG", {"class", "function"}},
{"ARITHEXPR", {"floatnum", "lpar", "not", "id", "intnum", "plus", "minus"}},
{"RELOPOP", {"eq", "neq", "lt", "gt", "leq", "geq"}},
{"RELOP", {"eq", "neq", "lt", "gt", "leq", "geq"}},
{"APARAMSTAIL", {"comma"}},
{"REPTAPARAMS1", {"comma"}},
{"ARRAYSIZE", {"lsqbr"}},
{"REPTARRAYSIZE", {"lsqbr"}},
{"FPARAMSTAIL", {"comma"}},
{"REPTFPARAMS4", {"comma"}},
{"REPTINHERITSLIST", {"comma"}},
{"LOCALVARORSTAT", {"localvar", "if", "while", "read", "write", "return", "id"}},
{"REPTLOCALVARORSTAT", {"localvar", "if", "while", "read", "write", "return", "id"}},
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
{"REPTSTATBLOCK1", {"if", "while", "read", "write", "return", "id"}},
{"STATBLOCK", {"lcurbr", "if", "while", "read", "write", "return", "id"}},
{"STATEMENT", {"if", "while", "read", "write", "return", "id"}},
{"STATEMENTBLOCK", {"lcurbr", "if", "while", "read", "write", "return", "id"}},
{"RELEXPRPAR", {"lpar"}},
{"RELEXPR", {"floatnum", "lpar", "not", "id", "intnum", "plus", "minus"}},
{"EXPRPAR", {"lpar"}},
{"INDICE", {"lsqbr"}},
{"STATEMENTIDNEST2", {"dot"}},
{"STATEMENTIDNEST3", {"assign", "dot"}},
{"ASSIGNOP", {"assign"}},
{"EXPR", {"floatnum", "lpar", "not", "id", "intnum", "plus", "minus"}},
{"DOTIDSTATEMENTIDNEST", {"dot"}},
{"STATEMENTIDNEST", {"dot", "lpar", "lsqbr", "assign"}},
{"TERM", {"floatnum", "lpar", "not", "id", "intnum", "plus", "minus"}},
{"FACTOR", {"floatnum", "lpar", "not", "id", "intnum", "plus", "minus"}},
{"RIGHTRECTERM", {"mult", "div", "and"}},
{"TYPE", {"integer", "float", "id"}},
{"VARIABLE", {"id"}},
{"VARIABLE2", {"lsqbr", "dot", "lpar"}},
{"REPTVARIABLE", {"dot"}},
{"ID", {"id"}},
{"APARAMS", {"floatnum", "lpar", "not", "id", "intnum", "plus", "minus"}},
{"APARAMSLIST", {"lpar"}},
{"VARIDNEST", {"dot"}},
{"VARIDNEST2", {"lpar", "lsqbr"}},
{"APARAMSLISTIDNEST", {"lpar"}},
{"REPTIDNEST1", {"lsqbr"}},
{"VISIBILITY", {"public", "private"}},
};

const unordered_map<string, unordered_set<string>> Parser::follow_set_map = {
{"ARRAYSIZE2", {"lsqbr", "rpar", "comma", "semi"}},
{"CLASSDECL", {"class", "function", "eof"}},
{"EXPR2", {"semi", "comma", "rpar"}},
{"INTNUM", {"semi", "mult", "div", "and", "eq", "neq", "lt", "gt", "leq", "geq", "rsqbr", "plus", "minus", "or", "comma", "rpar"}},
{"FACTOR2", {"semi", "mult", "div", "and", "dot", "rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "plus", "minus", "or", "comma", "rpar"}},
{"FUNCDEF", {"class", "function", "eof"}},
{"FUNCBODY", {"class", "function", "eof"}},
{"FUNCHEAD", {"lcurbr"}},
{"FPARAMS", {"rpar"}},
{"FUNCHEADTAIL", {"lcurbr"}},
{"FUNCHEADMEMBERTAIL", {"lcurbr"}},
{"IDNEST2", {"semi", "mult", "div", "and", "dot", "rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "plus", "minus", "or", "comma", "rpar"}},
{"ARRAYOROBJECT", {"semi"}},
{"LOCALVARDECL", {"localvar", "if", "while", "read", "write", "return", "id", "rcurbr"}},
{"MEMBERFUNCDECL", {"public", "private", "rcurbr"}},
{"MEMBERFUNCHEAD", {"semi"}},
{"ARROWRETURNTYPE", {"semi", "lcurbr"}},
{"PARAMLIST", {"semi", "arrow", "lcurbr"}},
{"MEMBERVARDECL", {"public", "private", "rcurbr"}},
{"VARDECL", {"rpar", "comma", "lpar", "lsqbr", "semi"}},
{"OPTINHERITS", {"lcurbr"}},
{"PROG", {"eof"}},
{"ARITHEXPR", {"semi", "rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "comma", "rpar"}},
{"RELOPOP", {"floatnum", "lpar", "not", "id", "intnum", "plus", "minus"}},
{"RELOP", {"floatnum", "lpar", "not", "id", "intnum", "plus", "minus"}},
{"APARAMSTAIL", {"rpar", "comma"}},
{"REPTAPARAMS1", {"rpar"}},
{"ARRAYSIZE", {"lsqbr", "rpar", "comma", "semi"}},
{"REPTARRAYSIZE", {"rpar", "comma", "semi"}},
{"FPARAMSTAIL", {"comma", "rpar"}},
{"REPTFPARAMS4", {"rpar"}},
{"REPTINHERITSLIST", {"lcurbr"}},
{"LOCALVARORSTAT", {"localvar", "if", "while", "read", "write", "return", "id", "rcurbr"}},
{"REPTLOCALVARORSTAT", {"rcurbr"}},
{"MEMBERDECL", {"public", "private", "rcurbr"}},
{"REPTMEMBERDECL", {"rcurbr"}},
{"CLASSDECLORFUNCDEF", {"class", "function", "eof"}},
{"REPTPROG0", {"eof"}},
{"IDNEST", {"semi", "mult", "div", "and", "dot", "rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "plus", "minus", "or", "comma", "rpar"}},
{"REPTVARIABLEORFUNCTIONCALL", {"semi", "mult", "div", "and", "rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "plus", "minus", "or", "comma", "rpar"}},
{"RETURNTYPE", {"semi", "lcurbr"}},
{"ADDOP", {"floatnum", "lpar", "not", "id", "intnum", "plus", "minus"}},
{"RIGHTRECARITHEXPR", {"semi", "rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "comma", "rpar"}},
{"MULTOP", {"floatnum", "lpar", "not", "id", "intnum", "plus", "minus"}},
{"SIGN", {"floatnum", "lpar", "not", "id", "intnum", "plus", "minus"}},
{"REPTSTATBLOCK1", {"rcurbr"}},
{"STATBLOCK", {"else", "semi"}},
{"STATEMENT", {"else", "semi", "localvar", "if", "while", "read", "write", "return", "id", "rcurbr"}},
{"STATEMENTBLOCK", {"else", "semi"}},
{"RELEXPRPAR", {"then", "lcurbr", "if", "while", "read", "write", "return", "id", "semi"}},
{"RELEXPR", {"rpar"}},
{"EXPRPAR", {"semi"}},
{"INDICE", {"semi", "mult", "div", "and", "lsqbr", "assign", "dot", "rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "plus", "minus", "or", "comma", "rpar"}},
{"STATEMENTIDNEST2", {"semi"}},
{"STATEMENTIDNEST3", {"semi"}},
{"ASSIGNOP", {"floatnum", "lpar", "not", "id", "intnum", "plus", "minus"}},
{"EXPR", {"semi", "comma", "rpar"}},
{"DOTIDSTATEMENTIDNEST", {"semi"}},
{"STATEMENTIDNEST", {"semi"}},
{"TERM", {"semi", "rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "plus", "minus", "or", "comma", "rpar"}},
{"FACTOR", {"semi", "mult", "div", "and", "rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "plus", "minus", "or", "comma", "rpar"}},
{"RIGHTRECTERM", {"semi", "rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "plus", "minus", "or", "comma", "rpar"}},
{"TYPE", {"rpar", "lcurbr", "comma", "lpar", "lsqbr", "semi"}},
{"VARIABLE", {"rpar"}},
{"VARIABLE2", {"rpar"}},
{"REPTVARIABLE", {"rpar"}},
{"ID", {"mult", "div", "and", "semi", "rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "isa", "lcurbr", "sr", "colon", "assign", "dot", "lpar", "lsqbr", "plus", "minus", "or", "comma", "rpar"}},
{"APARAMS", {"rpar"}},
{"APARAMSLIST", {"mult", "div", "and", "semi", "dot", "rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "plus", "minus", "or", "comma", "rpar"}},
{"VARIDNEST", {"rpar", "dot"}},
{"VARIDNEST2", {"rpar", "dot"}},
{"APARAMSLISTIDNEST", {"rpar", "dot"}},
{"REPTIDNEST1", {"assign", "semi", "mult", "div", "and", "dot", "rsqbr", "eq", "neq", "lt", "gt", "leq", "geq", "plus", "minus", "or", "comma", "rpar"}},
{"VISIBILITY", {"attribute", "function", "constructorkeyword"}},
};

const unordered_set<string> Parser::nullable_set = {
"FACTOR2",
"FPARAMS",
"IDNEST2",
"ARRAYOROBJECT",
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
"STATEMENTBLOCK",
"STATEMENTIDNEST2",
"RIGHTRECTERM",
"VARIABLE2",
"REPTVARIABLE",
"APARAMS",
"VARIDNEST2",
"REPTIDNEST1",
};

const unordered_map<string, unordered_map<string, string>> Parser::parsing_map = initialize_parsing_map();

unordered_map<string, unordered_map<string, string>> Parser::initialize_parsing_map() {
	cs r1 = "_stop PROG _Prog eof";
	cs r2 = "minus";
	cs r3 = "plus";
	cs r4 = "or";
	cs r5 = "";
	cs r6 = "EXPR REPTAPARAMS1";
	cs r7 = "comma EXPR";
	cs r8 = "TERM RIGHTRECARITHEXPR";
	cs r9 = "APARAMSLIST";
	cs r10 = "REPTARRAYSIZE _DimList";
	cs r11 = "lsqbr ARRAYSIZE2";
	cs r12 = "rsqbr";
	cs r13 = "INTNUM rsqbr";
	cs r14 = "intnum _IntNum";
	cs r15 = "assign";
	cs r16 = "class ID OPTINHERITS lcurbr REPTMEMBERDECL rcurbr semi";
	cs r17 = "FUNCDEF _FuncDef";
	cs r18 = "_stop CLASSDECL _Class";
	cs r19 = "ARITHEXPR EXPR2";
	cs r20 = "";
	cs r21 = "RELOPOP ARITHEXPR _Operation";
	cs r22 = "lpar ARITHEXPR rpar";
	cs r23 = "_stop SIGN _Sign FACTOR _Factor";
	cs r24 = "ID FACTOR2 REPTVARIABLEORFUNCTIONCALL _DotList";
	cs r25 = "_stop not _Sign FACTOR _Factor";

	cs r26 = "floatnum _FloatNum";
	cs r27 = "INTNUM";
	cs r28 = "REPTIDNEST1 _DimList _Variable";
	cs r29 = "APARAMSLIST _FuncCall";
	cs r30 = "";
	cs r31 = "_stop VARDECL REPTARRAYSIZE _DimList _VarDecl REPTFPARAMS4";
	cs r32 = "comma _stop VARDECL REPTARRAYSIZE _DimList _VarDecl";
	cs r33 = "lcurbr _stop REPTLOCALVARORSTAT _StatBlock rcurbr";
	cs r34 = "FUNCHEAD FUNCBODY";
	cs r35 = "function ID FUNCHEADTAIL";
	cs r36 = "lpar FPARAMS _ParamList rpar";
	cs r37 = "ID PARAMLIST ARROWRETURNTYPE";
	cs r38 = "constructorkeyword _Id PARAMLIST";
	cs r39 = "PARAMLIST ARROWRETURNTYPE";
	cs r40 = "sr _Scope FUNCHEADMEMBERTAIL";
	cs r41 = "arrow RETURNTYPE";
	cs r42 = "dot ID IDNEST2";
	cs r43 = "REPTIDNEST1 _DimList _Variable";
	cs r44 = "APARAMSLIST _FuncCall";
	cs r45 = "lsqbr _stop ARITHEXPR _Dim rsqbr";
	cs r46 = "localvar _stop VARDECL ARRAYOROBJECT _VarDecl semi";
	cs r47 = "STATEMENT";
	cs r48 = "LOCALVARDECL";
	cs r49 = "MEMBERVARDECL _VarDecl";
	cs r50 = "MEMBERFUNCDECL _FuncDecl";
	cs r51 = "MEMBERFUNCHEAD semi";
	cs r52 = "constructorkeyword _Id colon PARAMLIST";
	cs r53 = "function ID colon PARAMLIST ARROWRETURNTYPE";
	cs r54 = "attribute VARDECL REPTARRAYSIZE _DimList semi";
	cs r55 = "id _Id";
	cs r56 = "ID colon TYPE _Type";
	cs r57 = "and";
	cs r58 = "div";
	cs r59 = "mult";
	cs r60 = "";
	cs r61 = "isa _stop ID REPTINHERITSLIST _ISA";
	cs r62 = "REPTPROG0";
	cs r63 = "ARITHEXPR RELOPOP ARITHEXPR _Operation";
	cs r64 = "RELOP _OpType";
	cs r65 = "geq";
	cs r66 = "leq";
	cs r67 = "gt";
	cs r68 = "lt";
	cs r69 = "neq";
	cs r70 = "eq";
	cs r71 = "";
	cs r72 = "APARAMSTAIL REPTAPARAMS1";
	cs r73 = "";
	cs r74 = "_stop ARRAYSIZE _Dim REPTARRAYSIZE";
	cs r75 = "";
	cs r76 = "FPARAMSTAIL REPTFPARAMS4";
	cs r77 = "";
	cs r78 = "INDICE REPTIDNEST1";
	cs r79 = "";
	cs r80 = "comma ID REPTINHERITSLIST";
	cs r81 = "LOCALVARORSTAT REPTLOCALVARORSTAT";
	cs r82 = "";
	cs r83 = "VISIBILITY _stop _Visibility MEMBERDECL REPTMEMBERDECL";
	cs r84 = "";
	cs r85 = "CLASSDECLORFUNCDEF REPTPROG0";
	cs r86 = "";
	cs r87 = "STATEMENT REPTSTATBLOCK1";
	cs r88 = "";
	cs r89 = "";
	cs r90 = "VARIDNEST REPTVARIABLE";
	cs r91 = "";
	cs r92 = "IDNEST REPTVARIABLEORFUNCTIONCALL";
	cs r93 = "TYPE _Type";
	cs r94 = "void _Type";
	cs r95 = "";
	cs r96 = "ADDOP _OpType TERM _Operation RIGHTRECARITHEXPR";
	cs r97 = "";
	cs r98 = "MULTOP _OpType FACTOR _Operation RIGHTRECTERM";
	cs r99 = "minus";
	cs r100 = "plus";
	cs r101 = "";
	cs r102 = "STATEMENT";
	cs r103 = "lcurbr REPTSTATBLOCK1 rcurbr";
	cs r104 = "_stop STATBLOCK _StatBlock";
	cs r105 = "return _stop _StatType EXPRPAR semi _Statement";
	cs r106 = "write _stop _StatType EXPRPAR semi _Statement";
	cs r107 = "read _stop _StatType lpar VARIABLE rpar semi _Statement";
	cs r108 = "while _stop _StatType _stop RELEXPRPAR _Condition STATEMENTBLOCK semi _Statement";
	cs r109 = "if _stop _StatType _stop RELEXPRPAR _Condition then STATEMENTBLOCK else STATEMENTBLOCK semi _Statement";
	cs r110 = "ID STATEMENTIDNEST semi";
	cs r111 = "lpar RELEXPR rpar";
	cs r112 = "lpar EXPR rpar";
	cs r113 = "APARAMSLIST _FuncCall STATEMENTIDNEST2";
	cs r114 = "_Variable DOTIDSTATEMENTIDNEST";
	cs r115 = "INDICE REPTIDNEST1 _DimList _Variable STATEMENTIDNEST3";
	cs r116 = "_Variable _DotList ASSIGNOP EXPR _AssignStat";
	cs r117 = "DOTIDSTATEMENTIDNEST";
	cs r118 = "_DotList _FuncCallStat";
	cs r119 = "DOTIDSTATEMENTIDNEST";
	cs r120 = "_DotList ASSIGNOP EXPR _AssignStat";
	cs r121 = "dot ID STATEMENTIDNEST";
	cs r122 = "FACTOR RIGHTRECTERM";
	cs r123 = "float";
	cs r124 = "integer";
	cs r125 = "id";
	cs r126 = "ID VARIABLE2";
	cs r127 = "REPTIDNEST1 _DimList REPTVARIABLE";
	cs r128 = "APARAMSLISTIDNEST";
	cs r129 = "dot ID VARIDNEST2";
	cs r130 = "lpar _stop APARAMS _AParamList rpar";
	cs r131 = "APARAMSLIST VARIDNEST";
	cs r132 = "REPTIDNEST1";
	cs r133 = "APARAMSLISTIDNEST";
	cs r134 = "private";
	cs r135 = "public";

	cs t_ = "";
	cs t_$ = "$";
	cs t_private = "private";
	cs t_public = "public";
	cs t_rpar = "rpar";
	cs t_lpar = "lpar";
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
	cs t_id = "id";
	cs t_attribute = "attribute";
	cs t_constructorkeyword = "constructorkeyword";
	cs t_function = "function";
	cs t_localvar = "localvar";
	cs t_rsqbr = "rsqbr";
	cs t_lsqbr = "lsqbr";
	cs t_arrow = "arrow";
	cs t_sr = "sr";
	cs t_not = "not";
	cs t_floatnum = "floatnum";
	cs t_class = "class";
	cs t_assign = "assign";
	cs t_intnum = "intnum";
	cs t_or = "or";
	cs t_eof = "eof";

	unordered_map<string, unordered_map<string, string>> parsing_map = {
		{"START", {{t_function, r1},{t_class, r1},{t_eof, r1},}},
{"ADDOP", {{t_minus, r2},{t_plus, r3},{t_or, r4},}},
{"APARAMS", {{t_rpar, r5},{t_lpar, r6},{t_minus, r6},{t_plus, r6},{t_id, r6},{t_not, r6},{t_floatnum, r6},{t_intnum, r6},}},
{"APARAMSTAIL", {{t_comma, r7},}},
{"ARITHEXPR", {{t_lpar, r8},{t_minus, r8},{t_plus, r8},{t_id, r8},{t_not, r8},{t_floatnum, r8},{t_intnum, r8},}},
{"ARRAYOROBJECT", {{t_lpar, r9},{t_semi, r10},{t_lsqbr, r10},}},
{"ARRAYSIZE", {{t_lsqbr, r11},}},
{"ARRAYSIZE2", {{t_rsqbr, r12},{t_intnum, r13},}},
{"INTNUM", {{t_intnum, r14},}},
{"ASSIGNOP", {{t_assign, r15},}},
{"CLASSDECL", {{t_class, r16},}},
{"CLASSDECLORFUNCDEF", {{t_function, r17},{t_class, r18},}},
{"EXPR", {{t_lpar, r19},{t_minus, r19},{t_plus, r19},{t_id, r19},{t_not, r19},{t_floatnum, r19},{t_intnum, r19},}},
{"EXPR2", {{t_rpar, r20},{t_semi, r20},{t_comma, r20},{t_geq, r21},{t_leq, r21},{t_gt, r21},{t_lt, r21},{t_neq, r21},{t_eq, r21},}},
{"FACTOR", {{t_lpar, r22},{t_minus, r23},{t_plus, r23},{t_id, r24},{t_not, r25},{t_floatnum, r26},{t_intnum, r27},}},
{"FACTOR2", {{t_rpar, r28},{t_lpar, r29},{t_dot, r28},{t_semi, r28},{t_minus, r28},{t_plus, r28},{t_comma, r28},{t_geq, r28},{t_leq, r28},{t_gt, r28},{t_lt, r28},{t_neq, r28},{t_eq, r28},{t_and, r28},{t_div, r28},{t_mult, r28},{t_rsqbr, r28},{t_lsqbr, r28},{t_or, r28},}},
{"FPARAMS", {{t_rpar, r30},{t_id, r31},}},
{"FPARAMSTAIL", {{t_comma, r32},}},
{"FUNCBODY", {{t_lcurbr, r33},}},
{"FUNCDEF", {{t_function, r34},}},
{"FUNCHEAD", {{t_function, r35},}},
{"PARAMLIST", {{t_lpar, r36},}},
{"FUNCHEADMEMBERTAIL", {{t_id, r37},{t_constructorkeyword, r38},}},
{"FUNCHEADTAIL", {{t_lpar, r39},{t_sr, r40},}},
{"ARROWRETURNTYPE", {{t_arrow, r41},}},
{"IDNEST", {{t_dot, r42},}},
{"IDNEST2", {{t_rpar, r43},{t_lpar, r44},{t_dot, r43},{t_semi, r43},{t_minus, r43},{t_plus, r43},{t_comma, r43},{t_geq, r43},{t_leq, r43},{t_gt, r43},{t_lt, r43},{t_neq, r43},{t_eq, r43},{t_and, r43},{t_div, r43},{t_mult, r43},{t_rsqbr, r43},{t_lsqbr, r43},{t_or, r43},}},
{"INDICE", {{t_lsqbr, r45},}},
{"LOCALVARDECL", {{t_localvar, r46},}},
{"LOCALVARORSTAT", {{t_return, r47},{t_write, r47},{t_read, r47},{t_while, r47},{t_if, r47},{t_id, r47},{t_localvar, r48},}},
{"MEMBERDECL", {{t_attribute, r49},{t_constructorkeyword, r50},{t_function, r50},}},
{"MEMBERFUNCDECL", {{t_constructorkeyword, r51},{t_function, r51},}},
{"MEMBERFUNCHEAD", {{t_constructorkeyword, r52},{t_function, r53},}},
{"MEMBERVARDECL", {{t_attribute, r54},}},
{"ID", {{t_id, r55},}},
{"VARDECL", {{t_id, r56},}},
{"MULTOP", {{t_and, r57},{t_div, r58},{t_mult, r59},}},
{"OPTINHERITS", {{t_lcurbr, r60},{t_isa, r61},}},
{"PROG", {{t_function, r62},{t_class, r62},{t_eof, r62},}},
{"RELEXPR", {{t_lpar, r63},{t_minus, r63},{t_plus, r63},{t_id, r63},{t_not, r63},{t_floatnum, r63},{t_intnum, r63},}},
{"RELOPOP", {{t_geq, r64},{t_leq, r64},{t_gt, r64},{t_lt, r64},{t_neq, r64},{t_eq, r64},}},
{"RELOP", {{t_geq, r65},{t_leq, r66},{t_gt, r67},{t_lt, r68},{t_neq, r69},{t_eq, r70},}},
{"REPTAPARAMS1", {{t_rpar, r71},{t_comma, r72},}},
{"REPTARRAYSIZE", {{t_rpar, r73},{t_semi, r73},{t_comma, r73},{t_lsqbr, r74},}},
{"REPTFPARAMS4", {{t_rpar, r75},{t_comma, r76},}},
{"REPTIDNEST1", {{t_rpar, r77},{t_dot, r77},{t_semi, r77},{t_minus, r77},{t_plus, r77},{t_comma, r77},{t_geq, r77},{t_leq, r77},{t_gt, r77},{t_lt, r77},{t_neq, r77},{t_eq, r77},{t_and, r77},{t_div, r77},{t_mult, r77},{t_rsqbr, r77},{t_lsqbr, r78},{t_assign, r77},{t_or, r77},}},
{"REPTINHERITSLIST", {{t_lcurbr, r79},{t_comma, r80},}},
{"REPTLOCALVARORSTAT", {{t_return, r81},{t_write, r81},{t_read, r81},{t_while, r81},{t_if, r81},{t_rcurbr, r82},{t_id, r81},{t_localvar, r81},}},
{"REPTMEMBERDECL", {{t_private, r83},{t_public, r83},{t_rcurbr, r84},}},
{"REPTPROG0", {{t_function, r85},{t_class, r85},{t_eof, r86},}},
{"REPTSTATBLOCK1", {{t_return, r87},{t_write, r87},{t_read, r87},{t_while, r87},{t_if, r87},{t_rcurbr, r88},{t_id, r87},}},
{"REPTVARIABLE", {{t_rpar, r89},{t_dot, r90},}},
{"REPTVARIABLEORFUNCTIONCALL", {{t_rpar, r91},{t_dot, r92},{t_semi, r91},{t_minus, r91},{t_plus, r91},{t_comma, r91},{t_geq, r91},{t_leq, r91},{t_gt, r91},{t_lt, r91},{t_neq, r91},{t_eq, r91},{t_and, r91},{t_div, r91},{t_mult, r91},{t_rsqbr, r91},{t_or, r91},}},
{"RETURNTYPE", {{t_float, r93},{t_integer, r93},{t_void, r94},{t_id, r93},}},
{"RIGHTRECARITHEXPR", {{t_rpar, r95},{t_semi, r95},{t_minus, r96},{t_plus, r96},{t_comma, r95},{t_geq, r95},{t_leq, r95},{t_gt, r95},{t_lt, r95},{t_neq, r95},{t_eq, r95},{t_rsqbr, r95},{t_or, r96},}},
{"RIGHTRECTERM", {{t_rpar, r97},{t_semi, r97},{t_minus, r97},{t_plus, r97},{t_comma, r97},{t_geq, r97},{t_leq, r97},{t_gt, r97},{t_lt, r97},{t_neq, r97},{t_eq, r97},{t_and, r98},{t_div, r98},{t_mult, r98},{t_rsqbr, r97},{t_or, r97},}},
{"SIGN", {{t_minus, r99},{t_plus, r100},}},
{"STATBLOCK", {{t_semi, r101},{t_return, r102},{t_write, r102},{t_read, r102},{t_while, r102},{t_else, r101},{t_if, r102},{t_lcurbr, r103},{t_id, r102},}},
{"STATEMENTBLOCK", {{t_semi, r104},{t_return, r104},{t_write, r104},{t_read, r104},{t_while, r104},{t_else, r104},{t_if, r104},{t_lcurbr, r104},{t_id, r104},}},
{"STATEMENT", {{t_return, r105},{t_write, r106},{t_read, r107},{t_while, r108},{t_if, r109},{t_id, r110},}},
{"RELEXPRPAR", {{t_lpar, r111},}},
{"EXPRPAR", {{t_lpar, r112},}},
{"STATEMENTIDNEST", {{t_lpar, r113},{t_dot, r114},{t_lsqbr, r115},{t_assign, r116},}},
{"STATEMENTIDNEST2", {{t_dot, r117},{t_semi, r118},}},
{"STATEMENTIDNEST3", {{t_dot, r119},{t_assign, r120},}},
{"DOTIDSTATEMENTIDNEST", {{t_dot, r121},}},
{"TERM", {{t_lpar, r122},{t_minus, r122},{t_plus, r122},{t_id, r122},{t_not, r122},{t_floatnum, r122},{t_intnum, r122},}},
{"TYPE", {{t_float, r123},{t_integer, r124},{t_id, r125},}},
{"VARIABLE", {{t_id, r126},}},
{"VARIABLE2", {{t_rpar, r127},{t_lpar, r128},{t_dot, r127},{t_lsqbr, r127},}},
{"VARIDNEST", {{t_dot, r129},}},
{"APARAMSLIST", {{t_lpar, r130},}},
{"APARAMSLISTIDNEST", {{t_lpar, r131},}},
{"VARIDNEST2", {{t_rpar, r132},{t_lpar, r133},{t_dot, r132},{t_lsqbr, r132},}},
{"VISIBILITY", {{t_private, r134},{t_public, r135},}},
	};

	return parsing_map;
}


Parser::Parser(std::string filename) : la(LexicalAnalyzer(filename)), error(false) {
	derivation_file.open("output/outsyntax/" + filename + ".outderivation");
	error_file.open("output/outsyntax/" + filename + ".outsyntaxerrors");
}

Parser::Parser(std::string filename, ASTGenerator* s_a) : la(LexicalAnalyzer(filename)), error(false), semantic_analyzer(s_a) {
	derivation_file.open("output/outsyntax/" + filename + ".outderivation");
	error_file.open("output/outsyntax/" + filename + ".outsyntaxerrors");
}

void Parser::print_current_grammar() {
	std::ofstream grm_file ("output/attribute_grammar.grm");
	for (auto non_terminal : parsing_map) {
		unordered_set<string> u_set;
		for (auto transitions : non_terminal.second) {
			string rule = transitions.second;
			if (!u_set.count(rule)) {
				grm_file << non_terminal.first << " -> " << rule << std::endl;
				u_set.insert(rule);
			}
		}
		grm_file << std::endl;
	}
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
	derivation_file << ss.str();
}

bool Parser::parse() {
	print_current_grammar();
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

	//std::cout << "syntax error at " << current_token.line_location << " from " << current_token.lexeme << std::endl;
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