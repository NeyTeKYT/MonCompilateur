#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <set>												
#include <map> 													// Utilisation d'une 'map' pour la gestion des types afin d'attribuer une variable à son type.
#include <FlexLexer.h>
#include "tokeniser.h"
using namespace std;

enum OPREL {EQU, DIFF, INF, SUP, INFE, SUPE, WTFR}; 			// Opérateurs relationnels
enum OPADD {ADD, SUB, OR, WTFA}; 								// Opérateurs additifs
enum OPMUL {MUL, DIV, MOD, AND , WTFM}; 						// Opérateurs multiplicatifs
enum TYPES {INTEGER, BOOLEAN, DOUBLE, CHAR, UNKNOWN_TYPE}; 		// TP4 : Gestion des types | TP7 : Ajout des types DOUBLE et CHAR

TOKEN current;													// Current token

FlexLexer* lexer = new yyFlexLexer; 							// This is the flex tokeniser
																// tokens can be read using lexer->yylex()
																// lexer->yylex() returns the type of the lexicon entry (see enum TOKEN in tokeniser.h)
																// and lexer->YYText() returns the lexicon entry as a string

set<string> CurrentVariables;									// Data structure to store only the declared variables without knowing their type.
map<string, TYPES> DeclaredVariables;							// Data structure to store every declared variables with its type.

unsigned long TagNumber = 0;
unsigned long long CasesTagNumber = 0;

/** 
 * @brief Vérifie qu'une variable est déclarée.
 *
 * @param variable La variable a vérifiée.
 *
 * @return "true" si la variable est déclarée, "false" sinon.
*/
bool IsDeclared(const char * variable) {
	return DeclaredVariables.find(string(variable)) != DeclaredVariables.end();
}

/**
 * @brief Arrête le programme en cours d'exécution à cause d'une erreur.
 *
 * @details Je voulais essayé de faire un compilateur qui affiche les erreurs dans le style de g++.
 *
 * @param message Le message qui permet d'identifier précisement l'erreur.
*/
void Error(string message) {
	cerr << "Ligne n°" << lexer->lineno() << ", lu : '" << lexer->YYText() << "'(" << current << "), mais ";
	cerr << message << endl;
	exit(-1);
}	

/**
 * @brief Retourne le type d'une variable déclarée.
 *
 * @param variable La variable, pour connaître son type.
 *
 * @return Le type de la variable (si elle est bien déclarée).
*/
TYPES getTypeFromVariable(const char * variable) {
	if(!IsDeclared(variable)) Error(* variable + " n'est pas une variable déclarée du programme lu par le compilateur !");
	string string_variable(variable);
	return DeclaredVariables[string_variable];
}

/**
 * @brief Compare le token sur lequel on se trouve avec une chaine de caractère attendue.
 *
 * @param ch La chaine de caractère attendue.
 *
 * @return "true" s'il s'agit de la chaine de caractère attendue, "false" sinon.
*/
bool isToken(const char * ch) {
	return strcmp(lexer->YYText(), ch) == 0;
}

/**
 * @brief Retourne le type d'un token.
 *
 * @param type Le type à comparer.
 *
 * @return Le type identifié.
*/
TYPES getTypeFromVAR(const char * type) {
	TYPES found_type;
	TYPES integer = INTEGER, boolean = BOOLEAN, double_type = DOUBLE, char_type = CHAR, unknwon = UNKNOWN_TYPE;

	if(isToken("INTEGER")) found_type = integer;
	else if(isToken("BOOLEAN")) found_type = boolean;
	else if(isToken("DOUBLE")) found_type = double_type;
	else if(isToken("CHAR")) found_type = char_type;
	else found_type = unknwon;

	return found_type;
}

/**
 * @brief Transforme un type en un string.
 *
 * @param type Le type a transformé en string.
 *
 * @return Le type sous forme de string.
*/
string typeToString(TYPES type) {
	switch(type) {
		case INTEGER: 	return "INTEGER";
		case BOOLEAN: 	return "BOOLEAN";
		case DOUBLE:	return "DOUBLE";
		case CHAR:		return "CHAR";	
		default: 		return "UNKNOWN_TYPE";
	}
}

/**
 * @brief Lit un token pour passer au suivant.
 *
 * @details Permet dans le code des fonctions d'avoir un simple appel à cette fonction, qui est plus facile à comprendre et donc plus intuitive.
*/
void readCurrent(void) {
	current = (TOKEN) lexer->yylex();
}

/**
 * @brief Génère le code assembleur pour la déclaration de variables du même type dans un bloc VAR.
 *
 * @details Pour rendre le code plus lisble, on fait un appel à cette fonction qui produit le code assembleur correspondant.
 *
 * @param type Le type des variables déclarées.
 * @param CurrentVariables Structure de donnée (set) avec toutes les variables dont il faut produire le code assembleur (elles ne sont pas encore officiellement déclarées).
*/
void producingASMCodeForDeclaringVariables(TYPES type, set<string> & CurrentVariables) {

	// Vérifie que le type n'est pas inconnu (type invalide).
	if(type == UNKNOWN_TYPE) Error("le type de(s) variable(s) déclarée(s) est inconnu !");

	for(auto variable = CurrentVariables.begin(); variable != CurrentVariables.end(); variable++) {

		switch(type) {

			case INTEGER:
				DeclaredVariables.insert({* variable, INTEGER});
				cout << * variable << ":\t.quad 0" << endl;
				break;

			case BOOLEAN:
				DeclaredVariables.insert({* variable, BOOLEAN});
				cout << * variable << ":\t.quad 0" << endl; 
				break;

			case DOUBLE:
				DeclaredVariables.insert({* variable, DOUBLE});
				cout << * variable << ":\t.double 0.0" << endl;
				break;

			case CHAR:
				DeclaredVariables.insert({* variable, CHAR});
				cout << * variable << ":\t.byte 0" << endl;
				break;

			default:
				Error("le type est inconnu !");

		}

	}

}
	
TYPES Identifier(void) {
	TYPES type;
	type = getTypeFromVariable(lexer->YYText()); 
	cout << "\tpush " << lexer->YYText() << endl;
	readCurrent(); 
	return type;
}

// Number := Digit{Digit}
TYPES Number(void) {
	TYPES type;
	string number = lexer->YYText();

	// Cas où il s'agit d'un nombre flottant (ex : 18.2)
	if(number.find('.') != string::npos) {
		double float_number = atof(number.c_str());
		unsigned int * i = (unsigned int *) & float_number;
		cout << "\tsubq $8,%rsp\t\t\t# allocate 8 bytes on stack's top" << endl;
		cout << "\tmovl	$" << * i << ", (%rsp)\t# Conversion of " << float_number << " (32 bit high part)" << endl;
		cout << "\tmovl	$" << * (i+1) << ", 4(%rsp)\t# Conversion of " << float_number << " (32 bit low part)" << endl;
		readCurrent(); // Lecture du nombre flottant
		type = DOUBLE;
	}

	// Cas d'un nombre entier
	else {
		cout << "\tpush $" << atoi(lexer->YYText()) << endl;
		readCurrent(); // Lecture du nombre entier
		type = INTEGER;
	}

	return type;
}

TYPES Char(void) {

	cout << "\tmovq $0, %rax" << endl;
	cout << "\tmovb $" << lexer->YYText() << ",%al" << endl;
	cout << "\tpush %rax\t# push a 64-bit version of " << lexer->YYText() << endl;
	readCurrent(); // Lecture de la chaine de caractère
	TYPES type = CHAR;
	return type;

}

TYPES Expression(void);											// Called by Term() and calls Term()

// Factor := Number | Letter | "(" Expression ")"| "!" Factor
TYPES Factor(void) {

	TYPES type;

	switch(current) {

		case RPARENT: 
			readCurrent(); 											// Lecture de '('
			type = Expression();
			if(current != LPARENT) Error("')' était attendu");		// ")" expected
			readCurrent(); 											// Lecture de ')'
			break;
		
		case NUMBER: 
			type = Number();
			break;

		case ID:
			type = Identifier();
			break;

		case CHARCONST:
			type = Char();
			break;

		default:
			Error("'(' ou constante ou variable attendue !");

	}

	return type;

}

// MultiplicativeOperator := "*" | "/" | "%" | "&&"
OPMUL MultiplicativeOperator(void) {

	OPMUL opmul;

	if(isToken("*")) opmul = MUL;
	else if(isToken("/")) opmul = DIV;
	else if(isToken("%")) opmul = MOD; 
	else if(isToken("&&")) opmul = AND;	
	else opmul = WTFM;

	readCurrent(); // Lecture de l'opérateur multiplicatif
	return opmul;

}

// Term := Factor {MultiplicativeOperator Factor}
TYPES Term(void) {

	OPMUL mulop;
	TYPES type, type_to_compare;
	type = Factor();

	while(current == MULOP) {

		mulop = MultiplicativeOperator();				// Save operator in local variable

		type_to_compare = Factor();
		if(type != type_to_compare) Error("tous les appels à Factor doivent renvoyer le même type (ou un type compatible dans le futur) !");

		cout << "\tpop %rbx" << endl;					// get first operand
		cout << "\tpop %rax" << endl;					// get second operand

		switch(mulop) {

			case AND:
				cout << "\tmulq	%rbx"<<endl;			// a * b -> %rdx:%rax
				cout << "\tpush %rax\t# AND"<<endl;		// store result
				break;

			case MUL:
				cout << "\tmulq	%rbx"<<endl;			// a * b -> %rdx:%rax
				cout << "\tpush %rax\t# MUL"<<endl;		// store result
				break;

			case DIV:
				cout << "\tmovq $0, %rdx"<<endl; 		// Higher part of numerator  
				cout << "\tdiv %rbx"<<endl;				// quotient goes to %rax
				cout << "\tpush %rax\t# DIV"<<endl;		// store result
				break;

			case MOD:
				cout << "\tmovq $0, %rdx"<<endl; 		// Higher part of numerator  
				cout << "\tdiv %rbx"<<endl;				// remainder goes to %rdx
				cout << "\tpush %rdx\t# MOD"<<endl;		// store result
				break;

			default:
				Error("Opérateur multiplicatif attendu !");
		}
	}

	return type;

}

// AdditiveOperator := "+" | "-" | "||"
OPADD AdditiveOperator(void) {

	OPADD opadd;

	if(isToken("+")) opadd = ADD;
	else if(isToken("-")) opadd = SUB;
	else if(isToken("||")) opadd = OR;
	else opadd = WTFA;

	readCurrent(); // Lecture de l'opérateur additif
	return opadd;

}

// SimpleExpression := Term {AdditiveOperator Term}
TYPES SimpleExpression(void) {

	OPADD adop;
	TYPES type, type_to_compare;
	type = Term();

	while(current == ADDOP) {

		adop = AdditiveOperator();							// Save operator in local variable
		
		type_to_compare = Term();
		if(type != type_to_compare) Error("tous les appels à Term doivent renvoyer le même type (ou un type compatible dans le futur) !");

		if(type == INTEGER) {

			cout << "\tpop %rbx" << endl;						// get first operand
			cout << "\tpop %rax" << endl;						// get second operand

			switch(adop) {

				case OR:
					cout << "\torq %rbx, %rax\t# OR" << endl;	// operand1 OR operand2
					break;	

				case ADD:
					cout << "\taddq	%rbx, %rax\t# ADD" << endl;	// add both operands
					break;

				case SUB:	
					cout << "\tsubq	%rbx, %rax\t# SUB" << endl;	// substract both operands
					break;

				default:
					Error("opérateur additif inconnu");

			}

			cout << "\tpush %rax" << endl;						// store result

		}

		else if(type == DOUBLE) {

			cout << "\tfldl 8(%rsp)\t" << endl;
            cout << "\tfldl (%rsp)\t" << endl;
            cout << "\taddq $16, %rsp\t" << endl;

			switch(adop) {

				case OR:
                    Error("l'opérateur 'OR' n'est pas supporté pour le type 'DOUBLE' !");
                    break;

				case ADD:
                    cout << "\tfaddp %st(0), %st(1)\t# op1 + op2" << endl;
                    break;

				case SUB:
                    cout << "\tfsubp %st(1), %st(0)\t# op1 - op2" << endl;
                    break;

				 default:
                    Error("l'opérateur additif est inconnu !");

			}

			cout << "\tsubq $8, %rsp\t" << endl;
            cout << "\tfstpl (%rsp)\t" << endl; 				// store result

		}	
		
		else Error("le type doit être soit INTEGER soit DOUBLE !");

	}

	return type;

}

void VarDeclaration(void);										// Called by VarDeclarationPart()

// VarDeclarationPart := "VAR" VarDeclaration {";" VarDeclaration} "."
void VarDeclarationPart(void) {

	if(!isToken("VAR")) Error("la déclaration d'une variable doit commencer par le mot clé 'VAR' !");
	readCurrent(); // Lecture du 'VAR'

	VarDeclaration(); 

	while(isToken(";")) {
		readCurrent(); // Lecture du ";"
		VarDeclaration();
	}

	if(!isToken(".")) Error("il y a un '.' manquant à la fin du bloc 'VAR' !");
	readCurrent(); // Lecture du "."

}

// VarDeclaration := Ident {"," Ident} ":" Type
void VarDeclaration(void) {
	
	if(current != ID) Error("un identificateur est attendu pour définir une variable !");

	CurrentVariables.insert(lexer->YYText()); 						// Utilisation du <set> car on ne connaît pas encore le type
	readCurrent(); 													// Lecture du nom de la variable déclarée.

	while(isToken(",")) {
		readCurrent(); 												// Lecture de ','

		if(current != ID) Error("un identificateur est attendu pour définir une variable !");

		CurrentVariables.insert(lexer->YYText()); 					// Utilisation du <set> car on ne connaît pas encore le type
		readCurrent(); 												// Lecture du nom de la variable déclarée.
	}

	if(!isToken(":")) Error("le caractère ':' est attendu après la déclaration d'une ou plusieurs variable(s) pour spécifier son type !");
	readCurrent(); 													// Lecture de ':'

	TYPES type = getTypeFromVAR(lexer->YYText());

	producingASMCodeForDeclaringVariables(type, CurrentVariables); 	// Production du code assembleur en fonction du type de(s) variable(s)

	readCurrent(); 													// Lecture du type de(s) variable(s)

}

// RelationalOperator := "==" | "!=" | "<" | ">" | "<=" | ">="  
OPREL RelationalOperator(void) {

	OPREL oprel;

	if(isToken("==")) oprel = EQU;
	else if(isToken("!=")) oprel = DIFF;
	else if(isToken("<")) oprel = INF;
	else if(isToken(">")) oprel = SUP;
	else if(isToken("<=")) oprel = INFE;
	else if(isToken(">=")) oprel = SUPE;
	else oprel = WTFR;

	readCurrent(); // Lecture de l'opérateur relationnel

	return oprel;

}

// Expression := SimpleExpression [RelationalOperator SimpleExpression]
TYPES Expression(void) {

	unsigned long long tag;
	OPREL oprel;

	TYPES leftExpressionType, rightExpressionType;
	leftExpressionType = SimpleExpression();

	if(current == RELOP) {

		tag = ++TagNumber;
		oprel = RelationalOperator();

		rightExpressionType = SimpleExpression();

		if(leftExpressionType != rightExpressionType) Error("tous les appels à SimpleExpression doivent renvoyer le même type (ou un type compatible dans le futur) !");

		if(leftExpressionType == INTEGER) {
			cout << "\tpop %rax" << endl;
			cout << "\tpop %rbx" << endl;
			cout << "\tcmpq %rax, %rbx" << endl;
		}

		else if(leftExpressionType == DOUBLE) {
			cout << "\tfldl	(%rsp)\t" << endl;
			cout << "\tfldl	8(%rsp)\t# first operand -> %st(0) ; second operand -> %st(1)" << endl;
			cout << "\t addq $16, %rsp\t# 2x pop nothing" << endl;
			cout << "\tfcomip %st(1)\t\t# compare op1 and op2 -> %RFLAGS and pop" << endl;
			cout << "\tfaddp %st(1)\t# pop nothing" << endl;

		}

		else Error("le type doit être soit un INTEGER soit un DOUBLE !");

		switch(oprel) {

			case EQU:
				cout << "\tje Vrai" << tag << "\t# If equal" << endl;
				break;

			case DIFF:
				cout << "\tjne Vrai" << tag << "\t# If different" << endl;
				break;

			case SUPE:
				cout << "\tjae Vrai" << tag << "\t# If above or equal" << endl;
				break;

			case INFE:
				cout << "\tjbe Vrai" << tag << "\t# If below or equal" << endl;
				break;

			case INF:
				cout << "\tjb Vrai" << tag << "\t# If below" << endl;
				break;

			case SUP:
				cout << "\tja Vrai" << tag << "\t# If above" << endl;
				break;

			default:
				Error("Opérateur de comparaison inconnu !");

		}

		cout << "\tpush $0\t\t# False" << endl;
		cout << "\tjmp Suite"<< TagNumber << endl;
		cout << "Vrai" << TagNumber<<":\tpush $0xFFFFFFFFFFFFFFFF\t\t# True" << endl;	
		cout << "Suite" << TagNumber << ":" << endl;
		return BOOLEAN;

	}

	return leftExpressionType;

}

// AssignementStatement := Identifier ":=" Expression
void AssignementStatement(void) {

	TYPES expression_type, variable_type;

	string variable;

	if(current != ID) Error("un identificateur est attendu !");

	if(!IsDeclared(lexer->YYText())) {
		cerr << "Erreur : Variable '" << lexer->YYText() << "' non déclarée" << endl;
		exit(-1);
	}

	variable = lexer->YYText();
	variable_type = getTypeFromVariable(lexer->YYText());

	readCurrent();

	if(current != ASSIGN) Error("caractères ':=' attendus");
	readCurrent();

	expression_type = Expression();

	if(expression_type != variable_type) Error("la variable '" + variable + "' de type " + typeToString(variable_type) + " ne peut pas recevoir une expression de type " + typeToString(expression_type) + " !");

	if(variable_type == CHAR) {
		cout << "\tpop %rax" << endl;
		cout << "\tmovb %al," << variable << endl;
	}

	else cout << "\tpop " << variable << endl;
	
}

void Statement(void);

// IfStatement := "IF" Expression "THEN" Statement [ "ELSE" Statement ]
void IfStatement(void) {

	unsigned long long tag = TagNumber++;

	if((current != KEYWORD) || (!isToken("IF"))) Error("le mot clé 'IF' est manquant pour déclarer une condition !");
	readCurrent(); // Lecture du "IF"

	if(Expression() != BOOLEAN) Error("L'expression d'une condition 'IF' doit être de type BOOLEAN !");

	cout << "\tpop %rax\t# Get the result of expression" << endl;
	cout << "\tcmpq $0, %rax" << endl;
	cout << "\tje Else" << tag << "\t# if FALSE, jump to Else" << tag << endl;

	if((current != KEYWORD) ||(!isToken("THEN"))) Error("le mot clé 'THEN' est manquant pour déclarer le code à exécuter dans la condition 'IF' !");
	readCurrent(); // Lecture du "THEN"

	Statement();

	cout << "\tjmp Next" << tag <<"\t# Do not execute the else statement" << endl;
	cout << "Else" << tag << ":" << endl; // Might be the same effective adress than Next:

	if((current == KEYWORD) && (isToken("ELSE"))) {
		readCurrent(); // Lecture du "ELSE"
		Statement();
	}

	cout << "Next" << tag << ":" << endl;

}

// WhileStatement := "WHILE" Expression "DO" Statement
void WhileStatement(void) {

	unsigned long long tag = TagNumber++;

	if(!isToken("WHILE")) Error("le mot clé 'WHILE' est manquant pour déclarer une boucle 'WHILE' !");
	cout << "While" << tag << ":" << endl;
	readCurrent(); // Lecture du "WHILE"

	if(Expression() != BOOLEAN) Error("L'expression d'une boucle 'WHILE' doit être de type BOOLEAN !");

	cout << "\tpop %rax\t# Get the result of expression" << endl;
	cout << "\tcmpq $0, %rax" << endl;
	cout << "\tje EndWhile" << tag << "\t# if FALSE, jump out of the loop" << tag << endl;


	if(!isToken("DO")) Error("le mot clé 'DO' est manquant pour terminer la boucle 'WHILE' !");
	readCurrent(); // Lecture du "DO"

	Statement();

	cout << "\tjmp While" << tag << endl;
	cout << "EndWhile" << tag << ":" << endl;

}

// ForStatement := "FOR" AssignementStatement "To" Expression "DO" Statement
void ForStatement(void) {

	unsigned long long tag = TagNumber++;

	if(!isToken("FOR")) Error("le mot clé 'FOR' est manquant dans la déclaration d'une boucle 'FOR' !");
	cout << "For" << tag << ":" << endl;
	readCurrent(); // Lecture du "FOR"

	AssignementStatement();

	if(!isToken("TO")) Error("'TO' manquant dans la boucle 'FOR' !");
	readCurrent(); // Lecture du "TO"

	if(Expression() != INTEGER) Error("l'expression dans TO doit être de type INTEGER !");

	cout << "\tpop %rsi\t# La borne TO" << endl; 
	cout << "\tpop %rax\t# La valeur initiale du compteur pour aller à TO" << endl;

	if(!isToken("DO")) Error("'DO' manquant dans la boucle 'FOR' !");
	readCurrent(); // Lecture du "DO"

	// Production du code Assembleur à partir de la page 34/116 du cours.

	// PourInit:
	cout << "ForInit" << tag << ":" << endl;

	// PourTest:
	cout << "ForTest" << tag << ":" << endl;
    cout << "\tcmpq %rsi, %rax\t" << endl;
    cout << "\tja EndFor" << tag << endl;

	Statement();

	cout << "\taddq $1, %rax\t# i++" << endl;
	cout << "\tjmp ForTest" << tag << endl;

	cout << "EndFor" << tag << ":" << endl;

}

// BlockStatement := "BEGIN" Statement { ";" Statement } "END"
void BlockStatement(void) {

	if(!isToken("BEGIN")) Error("le mot clé 'BEGIN' est manquant dans le BlockStatement !");
	readCurrent(); // Lecture du "BEGIN"

	Statement();

	while(isToken(";")) {
		readCurrent(); // Lecture du ";"
		if(isToken("END")) break;
		Statement();
	}

	if(!isToken("END")) Error("le mot clé 'END' est manquant pour terminer le BlockStatement !");
	readCurrent(); // Lecture du "END"

}

// DisplayStatement := "DISPLAY" Expression
void DisplayStatement(void) {

	if(!isToken("DISPLAY")) Error("pour afficher une expression, il faut commencer par le mot clé 'DISPLAY' suivi de l'expression à afficher !");
	readCurrent(); // Lecture du "DISPLAY"

	unsigned long long tag = ++TagNumber;

	switch(Expression()) {

		case INTEGER:
			cout << "\tpop %rsi\t# Valeur entière à afficher" << endl;
			cout << "\tmovq $FormatString1, %rdi\t# \"%llu\\n\"" << endl;
			cout << "\tmovl $0, %eax" << endl;
			cout << "\tcall printf@PLT" << endl;
			break;

		case BOOLEAN:
			cout << "\tpop %rdx\t# Valeur booléenne (0 = FALSE, non-0 = TRUE)" << endl;
			cout << "\tcmpq $0, %rdx" << endl;
			cout << "\tje False" << tag << endl;
			cout << "\tmovq $TrueString, %rdi\t# \"TRUE\\n\"" << endl;
			cout << "\tjmp Next" << tag << endl;
			cout << "False" << tag << ":" << endl;
			cout << "\tmovq $FalseString, %rdi\t# \"FALSE\\n\"" << endl;
			cout << "Next" << tag << ":" << endl;
			cout << "\tcall puts@PLT" << endl;
			break;

		case DOUBLE:
			cout << "\tmovsd (%rsp), %xmm0\t# Charger le double à afficher depuis le sommet de la pile" << endl;
			cout << "\tsubq $16, %rsp\t\t# Allouer de l'espace (alignement)" << endl;
			cout << "\tmovsd %xmm0, 8(%rsp)\t# Sauvegarder le double pour l'appel printf" << endl;
			cout << "\tmovq $FormatString2, %rdi\t# \"%f\\n\"" << endl;
			cout << "\tmovq $1, %rax\t\t# Indique à printf qu'on utilise les registres xmm" << endl;
			cout << "\tcall printf@PLT" << endl;
			cout << "\taddq $24, %rsp\t\t# Réajuster la pile après appel" << endl;
			break;

		case CHAR:
			cout << "\tpop %rsi\t\t\t# Récupère le caractère (8 bits dans %sil)" << endl;
			cout << "\tmovq $FormatString3, %rdi\t# \"%c\\n\"" << endl;
			cout << "\tmovl $0, %eax" << endl;
			cout << "\tcall printf@PLT" << endl;
			break;

		default:
			Error("l'expression à afficher par 'DISPLAY' doit être soit de type INTEGER, BOOLEAN, DOUBLE, ou CHAR !");

	}

}

void ConditionalStatement(void);

// Statement := AssignementStatement | IfStatement | WhileStatement | ForStatement | BlockStatement
void Statement(void) {

	if(current == KEYWORD) {

		if((isToken("IF") )|| (isToken("CASE"))) ConditionalStatement(); // TP8 : Adaptation pour considérer le mot clé "CASE"
		else if(isToken("WHILE")) WhileStatement();
		else if(isToken("FOR")) ForStatement();
		else if(isToken("BEGIN")) BlockStatement();
		else if(isToken("DISPLAY")) DisplayStatement();
		else Error("le mot clé est inconnu !");

	}

	else {

		if(current == ID) AssignementStatement();
		else Error("une instruction est attendue !");

	}

}

// StatementPart := Statement {";" Statement} "."
void StatementPart(void) {

	cout << "\t.align 8" << endl;	// Alignement on addresses that are a multiple of 8 (64 bits = 8 bytes)
	cout << "\t.text\t\t# The following lines contain the program" << endl;
	cout << "\t.globl main\t# The main function must be visible from outside" << endl;
	cout << "main:\t\t\t# The main function body :" << endl;
	cout << "\tmovq %rsp, %rbp\t# Save the position of the stack's top" << endl;

	Statement();

	while(isToken(";")) {
		readCurrent(); // Lecture de ';'
		Statement();
	}

	if(!isToken(".")) Error("le caractère '.' est manquant pour terminer la déclaration !");
	readCurrent(); // Lecture de '.'

}

void CaseStatement(void);

// ConditionalStatement ::= IfStatement | CaseStatement
void ConditionalStatement(void) {
	if(isToken("IF")) IfStatement();
	else if(isToken("CASE")) CaseStatement();
	else Error("une condition doit commencer soit par le mot clé 'IF' soit par le mot clé 'CASE' !");
}

void CaseListElement(void);

// CaseStatement ::= "CASE" Expression "OF" CaseListElement { ";" CaseListElement } "END"
void CaseStatement(void) {
	unsigned long long tag = TagNumber++;
	CasesTagNumber = 0; // Rénitialise à 0 dans le cas où il y aurait plusieurs cases dans le programme lu par le compilateur.

	if(!isToken("CASE")) Error("il manque le mot clé 'CASE' pour déclarer une condition !");
	cout << "Case" << tag << ":" << endl;
	readCurrent(); // Lecture du mot clé "CASE"

	TYPES type = Expression();
	if((type != INTEGER) && (type != CHAR)) Error("L'expression du CASE doit être soit de type INTEGER soit de type CHAR !");

	cout << "\tpop %rax\t# Get the result of expression" << endl;

	if(!isToken("OF")) Error("il manque le mot clé 'OF' dans la signature de la condition CASE !");
	readCurrent();

	CaseListElement();

	while(isToken(";")) {
		readCurrent(); // Lecture de ";"
		if(isToken("END")) break;
		CaseListElement();
	}

	if(!isToken("END")) Error("il manque le mot clé 'END' pour la condition CASE !");
	readCurrent(); // Lecture du mot clé "END"

	cout << "\tjmp EndCase" << tag << endl;
	cout << "EndCase" << tag << ":" << endl;
}

void CaseLabelList(void);

// CaseListElement ::= CaseLabelList ":" Statement
void CaseListElement(void) {
	CasesTagNumber++;
	cout << "Cas" << CasesTagNumber << ":" << endl; // Cas1, Cas2, Cas3, ...

	CaseLabelList();
	if(!isToken(":")) Error("il manque un ':' !");
	readCurrent(); // Lecture de ":"
	Statement();
}

void CaseLabel(void);

// CaseLabelList ::= CaseLabel { "," CaseLabel }
void CaseLabelList(void) {
	CaseLabel();
	while(isToken(",")) {
		readCurrent();
		CaseLabel();
	}
}

void Constant(void);

// CaseLabel ::= Constant
void CaseLabel(void) {
	Constant();
}

OPADD Sign(void);

// Constant ::= Number | Sign Number | Identifier | Sign Identifier | Char
void Constant(void) {
	switch(current) {

		case NUMBER: 
			Number();
			break;

		case ID:
			Identifier();
			break;

		case ADDOP:
			Sign();
			if(current == NUMBER) Number();
			else if(current == ID) Identifier();
			else Error("après un signe '+' ou '-' il faut soit un nombre soit un ID !");
			break;

		case CHARCONST:
			Char();
			break;

		default:
			Error("'(' ou constante ou variable attendue !");
	}
}

// Sign ::= "+" | "-"
OPADD Sign(void) {
	if(isToken("+")) {
		readCurrent();
		return ADD;
	}
	else if(isToken("-")) {
		readCurrent();
		return SUB;
	}
	else {
		Error("le signe doit être soit '+' soit '-' !");
		return WTFA;
	}

}

// Program := [DeclarationPart] StatementPart
void Program(void) {

	if(isToken("VAR")) VarDeclarationPart();
	StatementPart();

}

// First version : Source code on standard input and assembly code on standard output
int main(void) {
	
	// Header for gcc assembler / linker
	cout << ".data" << endl;
	cout << "FormatString1:\t.string \"%llu\"\t# used by printf to display 64-bit unsigned integers" << endl; 
	cout << "FormatString2:\t.string \"%lf\"\t# used by printf to display 64-bit floating point numbers" << endl; 
	cout << "FormatString3:\t.string \"%c\"\t# used by printf to display a 8-bit single character" << endl; 
	cout << "TrueString:\t.string \"TRUE\"\t# used by printf to display the boolean value TRUE" << endl; 
	cout << "FalseString:\t.string \"FALSE\"\t# used by printf to display the boolean value FALSE" << endl; 

	// Let's proceed to the analysis and code production
	readCurrent();
	Program();

	// Trailer for the gcc assembler / linker
	cout << "\tmovq %rbp, %rsp\t\t# Restore the position of the stack's top" << endl;
	cout << "\tret\t\t\t# Return from main function" << endl;

	if(current != FEOF) {
		cerr << "Caractères en trop à la fin du programme : [" << current << "]";
		Error("."); // unexpected characters at the end of program
	}

}