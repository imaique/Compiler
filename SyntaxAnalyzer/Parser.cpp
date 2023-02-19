#include "Parser.h"
using std::string;
using std::unordered_map;

typedef Token::Type T;
typedef const string cs;

const unordered_map<string, unordered_map<string, string>> Parser::parsing_map = initialize_parsing_map();

unordered_map<string, unordered_map<string, string>> Parser::initialize_parsing_map() {
	cs epsilon = "";
	cs r1 = "REPTSTART0";
	cs r2 = "EXPR REPTAPARAMS1";
	cs r3 = "TERM RIGHTRECARITHEXPR";
	cs r4 = "REPTIDNEST id FACTORTAIL";
	cs r5 = "FACTOR RIGHTRECTERM RIGHTRECARITHEXPR EXPRTAIL";
	cs r6 = "id FACTORIDTAIL";
	cs r7 = "CONTINUATION";
	cs r8 = "TYPE id REPTARRAYSIZE REPTFPARAMS3";
	cs r9 = "REPTINDICE";
	cs r10 = "id FUNCHEADTAILTAIL";
	cs r11 = "id lpar FPARAMS rpar arrow RETURNTYPE";
	cs r12 = "lpar FPARAMS rpar arrow RETURNTYPE";
	cs r13 = "lpar ARITHEXPR rpar";
	cs r14 = "STATEMENT";
	cs r15 = "REPTARRAYSIZE semi";
	cs r16 = "ARITHEXPR RELOP ARITHEXPR";
	cs r17 = "VISIBILITY MEMBERDECL REPTCLASSDECL4";
	cs r18 = "LOCALVARDECLORSTMT REPTFUNCBODY1";
	cs r19 = "dot id IDNESTTAIL REPTIDNEST";
	cs r20 = "STATEMENT REPTSTATBLOCK1";
	cs r21 = "TYPE";
	cs r22 = "POINTNESTR STATEMENTPOINTNEST";
	cs r23 = "PARAMS STATEMENTPARENT";
	cs r24 = "POINTNESTR REPTINDICE";
	cs r25 = "PARAMS POINTNESTR REPTINDICE";
	cs r26 = "PARAMS POINTNESTR";
	cs r27 = "write lpar EXPR rpar semi";
	cs r28 = "return lpar EXPR rpar semi";
	cs r29 = "id REPTIDNESTRTAIL";
	cs r30 = "FACTOR RIGHTRECTERM";
	cs r31 = "dot REPTIDNESTR id";
	cs r32 = "ENDPARAMS";
	cs r33 = "PARAMS semi";
	cs r34 = "PARAMS dot";
	cs r35 = "id STATEMENTTAIL";
	cs r36 = "id VARIABLETAIL";
	cs r37 = "POINTNESTR";
	cs r38 = "read lpar VARIABLE rpar semi";
	cs r39 = "while lpar RELEXPR rpar STATBLOCK semi";
	cs r40 = "if lpar RELEXPR rpar then STATBLOCK else STATBLOCK semi";
	cs r41 = "lcurbr REPTSTATBLOCK1 rcurbr";
	cs r42 = "comma id REPTOPTCLASSDECL22";
	cs r43 = "FPARAMSTAIL REPTFPARAMS3";
	cs r44 = "ADDOP TERM RIGHTRECARITHEXPR";
	cs r45 = "APARAMSTAIL REPTAPARAMS1";
	cs r46 = "lpar APARAMS rpar";
	cs r47 = "MULTOP FACTOR RIGHTRECTERM";
	cs r48 = "isa id REPTOPTCLASSDECL22";
	cs r49 = "lcurbr REPTFUNCBODY1 rcurbr";
	cs r50 = "comma TYPE id REPTARRAYSIZE";
	cs r51 = "RELOP ARITHEXPR";
	cs r52 = "SIGN FACTOR";
	cs r53 = "MEMBERVARDECL";
	cs r54 = "MEMBERFUNCDECL";
	cs r55 = "FUNCHEAD FUNCBODY";
	cs r56 = "CLASSDECL";
	cs r57 = "FUNCDEF";
	cs r58 = "function id colon lpar FPARAMS rpar arrow RETURNTYPE semi";
	cs r59 = "constructor colon lpar FPARAMS rpar semi";
	cs r60 = "function FUNCHEADTAIL";
	cs r61 = "attribute id colon TYPE REPTARRAYSIZE semi";
	cs r62 = "LOCALVARDECL";
	cs r63 = "localvar id colon TYPE LOCALVARDECLTAIL";
	cs r64 = "INDICE REPTINDICE CONTINUATION";
	cs r65 = "not FACTOR";
	cs r66 = "class id OPTCLASSDECL2 lcurbr REPTCLASSDECL4 rcurbr semi";
	cs r67 = "constructor lpar FPARAMS rpar";
	cs r68 = "lsqbr ARRAYSIZETAIL";
	cs r69 = "rsqbr";
	cs r70 = "intnum rsqbr";
	cs r71 = "ASSIGNOP EXPR semi";
	cs r72 = "assign";
	cs r73 = "sr FUNCHEADTAILTAILTAIL";
	cs r74 = "lsqbr ARITHEXPR rsqbr";
	cs r75 = "comma EXPR";
	cs r76 = "PARAMS";
	cs r77 = "PARAMS CONTINUATION";
	cs r78 = "ARRAYSIZE REPTARRAYSIZE";
	cs r79 = "CLASSDECLORFUNCDEF r1";
	cs r80 = "INDICE r9 STATEMENTBRACK";
	cs r81 = "r9 ASSIGNEND";
	cs r82 = "INDICE r9 VARIABLEBRACK";
	cs r83 = "INDICE r9";
	cs r84 = "INDICE r9 POINTNESTR";
	cs r85 = "INDICE r9 dot";
	cs r86 = "ASSIGNEND";
	cs r87 = "";
	cs r88 = "";
	cs r89 = "";
	cs r90 = "";
	cs minus = "minus";
	cs plus = "plus";
	cs or = "or";

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
	cs t_floatlit = "floatlit";
	cs t_intlit = "intlit";
	cs t_class = "class";
	cs t_assign = "assign";
	cs t_intnum = "intnum";
	cs t_or = "or";

	unordered_map<string, unordered_map<string, string>> parsing_map = {
		{"START", {{T::EndOfFile, r1},{T::Function, r1},{T::Class, r1},}},
		{"ADDOP", {{T::Minus, minus},{T::Plus, plus},{T::OR, or},}},
		{"APARAMS", {{T::ID, r2},{t_rpar, epsilon},{t_lpar, r2},{T::ID, r2},{T::ID, r2},{T::ID, r2},}},
		{"APARAMSTAIL", {{T::EndOfFile, r1},}},
		{"ARITHEXPR", {{T::EndOfFile, r1},}},
		{"ARRAYSIZE", {{T::EndOfFile, r1},}},
		{"ARRAYSIZETAIL", {{T::EndOfFile, r1},}},
		{"ASSIGNOP", {{T::EndOfFile, r1},}},
		{"ASSIGNEND", {{T::EndOfFile, r1},}},
		{"CLASSDECL", {{T::EndOfFile, r1},}},
		{"CLASSDECLORFUNCDEF", {{T::EndOfFile, r1},}},
		{"CONTINUATION", {{T::EndOfFile, r1},}},
		{"EXPR", {{T::EndOfFile, r1},}},
		{"EXPRTAIL", {{T::EndOfFile, r1},}},
		{"FACTOR", {{T::EndOfFile, r1},}},
		{"FACTORTAIL", {{T::EndOfFile, r1},}},
		{"FACTORIDTAIL", {{T::EndOfFile, r1},}},
		{"FPARAMS", {{T::EndOfFile, r1},}},
		{"FPARAMSTAIL", {{T::EndOfFile, r1},}},
		{"FUNCBODY", {{T::EndOfFile, r1},}},
		{"FUNCDEF", {{T::EndOfFile, r1},}},
		{"FUNCHEAD", {{T::EndOfFile, r1},}},
		{"FUNCHEADTAIL", {{T::EndOfFile, r1},}},
		{"FUNCHEADTAILTAIL", {{T::EndOfFile, r1},}},
		{"FUNCHEADTAILTAILTAIL", {{T::EndOfFile, r1},}},
		{"INDICE", {{T::EndOfFile, r1},}},
		{"LOCALVARDECL", {{T::EndOfFile, r1},}},
		{"LOCALVARDECLTAIL", {{T::EndOfFile, r1},}},
		{"LOCALVARDECLORSTMT", {{T::EndOfFile, r1},}},
		{"MEMBERDECL", {{T::EndOfFile, r1},}},
		{"MEMBERFUNCDECL", {{T::EndOfFile, r1},}},
		{"MEMBERVARDECL", {{T::EndOfFile, r1},}},
		{"MULTOP", {{T::EndOfFile, r1},}},
		{"OPTCLASSDECL2", {{T::EndOfFile, r1},}},
		{"PARAMS", {{T::EndOfFile, r1},}},
		{"RELEXPR", {{T::EndOfFile, r1},}},
		{"RELOP", {{T::EndOfFile, r1},}},
		{"REPTAPARAMS1", {{T::EndOfFile, r1},}},
		{"REPTCLASSDECL4", {{T::EndOfFile, r1},}},
		{"REPTFPARAMS3", {{T::EndOfFile, r1},}},
		{"REPTARRAYSIZE", {{T::EndOfFile, r1},}},
		{"REPTFUNCBODY1", {{T::EndOfFile, r1},}},
		{"REPTIDNEST", {{T::EndOfFile, r1},}},
		{"REPTOPTCLASSDECL22", {{T::EndOfFile, r1},}},
		{"REPTSTART0", {{T::EndOfFile, r1},}},
		{"REPTSTATBLOCK1", {{T::EndOfFile, r1},}},
		{"RETURNTYPE", {{T::EndOfFile, r1},}},
		{"RIGHTRECARITHEXPR", {{T::EndOfFile, r1},}},
	};

	return parsing_map;
}


Parser::Parser(std::string filename) : la(LexicalAnalyzer(filename)) {}

void Parser::parse() {

}