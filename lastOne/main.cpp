/*
<program> ::= <statement> <program’>

<program’> ::= ; <statement> <program’> | ε


<statement> ::= <id> = <expr>
              | if ( <condition> ) then <statement> <if_tail>
              | while ( <condition> ) do <statement>
              | for <id> = <expr> to <expr> do <statement>
              | repeat <statement> until ( <condition> )
              | do <statement> while ( <condition> )
              | begin <statement_list> end
              | print ( <expr> )
              | read ( <id> )


<if_tail> ::= else <statement> | ε


<statement_list> ::= <statement> <statement_list’>

<statement_list’> ::= ; <statement> <statement_list’> | ε


--------------------------------------------------


<expr> ::= <term> <expr’>

<expr’> ::= + <term> <expr’>
          | - <term> <expr’>
          | or <term> <expr’>
          | ε


<term> ::= <factor> <term’>

<term’> ::= * <factor> <term’>
          | / <factor> <term’>
          | mod <factor> <term’>
          | div <factor> <term’>
          | and <factor> <term’>
          | ε


<factor> ::= <base> <factor’>

<factor’> ::= ^ <base> <factor’> | ε


<base> ::= ( <expr> )
         | - <base>
         | not <base>
         | <num>
         | <id>


--------------------------------------------------


<condition> ::= <expr> <relop> <expr>

<relop> ::= == | != | < | > | <= | >=


--------------------------------------------------


<id> ::= <letter> <id’>

<id’> ::= <letter> <id’> | <digit> <id’> | ε


<num> ::= <digit> <num’>

<num’> ::= <digit> <num’> | ε


<digit> ::= 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9

<letter> ::= a | b | c | ... | z | A | B | C | ... | Z*/

#include <iostream>
#include <cstring>
#include <cctype>
#include <cstdio>
#include <cstdlib>
using namespace std;

/* ================= TOKENS ================= */

#define NONE -1
#define EOS '\0'
#define NUM 256
#define DIV 257
#define MOD 258
#define ID 259
#define DONE 260
#define IF 261
#define THEN 262
#define ELSE 263
#define WHILE 264
#define DO 265
#define FOR 266
#define TO 267
#define PRINT 268
#define READ 269
#define REPEAT 270
#define UNTIL 271
#define EQ 272
#define NEQ 273
#define LE 274
#define GE 275
#define AND 276
#define OR 277

/* ================= GLOBAL ================= */

int lookahead;
int tokenval;
int lineno = 1;

void init();
void emit(int t, int tval);
void match(int);
void error();
int lexan();
int lookup(char s[]);
int insert(char s[], int tok);

void expr();
void term();
void factor();
void statement();
void base();
void program();
void condition();

/* ================= SYMBOL TABLE ================= */

char lexbuf[100];
char lexemes[100];
int lastchar = -1;

struct entry
{
    char *lexptr;
    int token;
};

struct entry symtable[100];
int lastentry = 0;

/* ================= KEYWORDS ================= */

struct entry keywords[] = {
    "div", DIV,
    "mod", MOD,
    "if", IF,
    "then", THEN,
    "else", ELSE,
    "while", WHILE,
    "do", DO,
    "for", FOR,
    "to", TO,
    "print", PRINT,
    "read", READ,
    "repeat", REPEAT,
    "until", UNTIL,
    "and", AND,
    "or", OR,
    0, 0};

/* ================= FUNCTIONS ================= */

int lookup(char s[])
{
    for (int p = lastentry; p > 0; p--)
        if (strcmp(symtable[p].lexptr, s) == 0)
            return p;
    return 0;
}

int insert(char s[], int tok)
{
    int len = strlen(s);
    lastentry++;
    symtable[lastentry].token = tok;
    symtable[lastentry].lexptr = &lexemes[lastchar + 1];
    lastchar += len + 1;
    strcpy(symtable[lastentry].lexptr, s);
    return lastentry;
}

/* ================= LEXER ================= */

int lexan()
{
    int t;
    while (1)
    {
        t = getchar();

        if (t == ' ' || t == '\t')
            continue;
        if (t == '\n')
        {
            lineno++;
            continue;
        }

        if (isdigit(t))
        {
            ungetc(t, stdin);
            cin >> tokenval;
            return NUM;
        }

        if (isalpha(t))
        {
            int b = 0;
            while (isalnum(t))
            {
                lexbuf[b++] = t;
                t = getchar();
            }
            lexbuf[b] = EOS;
            if (t != EOF)
                ungetc(t, stdin);

            int p = lookup(lexbuf);
            if (p == 0)
                p = insert(lexbuf, ID);

            tokenval = p;
            return symtable[p].token;
        }

        if (t == '=')
        {
            if ((t = getchar()) == '=')
                return EQ;
            ungetc(t, stdin);
            return '=';
        }

        if (t == '!')
        {
            if ((t = getchar()) == '=')
                return NEQ;
        }

        if (t == '<')
        {
            if ((t = getchar()) == '=')
                return LE;
            ungetc(t, stdin);
            return '<';
        }

        if (t == '>')
        {
            if ((t = getchar()) == '=')
                return GE;
            ungetc(t, stdin);
            return '>';
        }

        if (t == EOF)
            return DONE;

        return t;
    }
}

/* ================= EMIT ================= */

void emit(int t, int tval)
{
    switch (t)
    {
    case '+':
        cout << "pop r1\npop r2\nadd r2,r1\npush r2\n";
        break;
    case '-':
        cout << "pop r1\npop r2\nsub r2,r1\npush r2\n";
        break;
    case '*':
        cout << "pop r1\npop r2\nmul r2,r1\npush r2\n";
        break;
    case '/':
        cout << "pop r1\npop r2\ndiv r2,r1\npush r2\n";
        break;
    case DIV:
        cout << "pop r1\npop r2\nDIV r2,r1\npush r2\n";
        break;
    case MOD:
        cout << "pop r1\npop r2\nmod r2,r1\npush r2\n";
        break;

    case NUM:
        cout << "push " << tval << "\n";
        break;

    case ID:
        cout << "push " << symtable[tval].lexptr << "\n";
        break;

    case EQ:
    case NEQ:
    case '<':
    case '>':
    case LE:
    case GE:
        cout << "pop r1\npop r2\ncmp r2,r1\npush r2\n";
        break;
    default:
        cout << t << tval << "\n";
    }
}

/* ================= PARSER ================= */

void match(int t)
{
    if (lookahead == t)
        lookahead = lexan();
    else
        cout << "syntax error\n", exit(1);
}

/* ===== expressions ===== */

// <base> ::= ( <expr> )| - <base>| <num>| <id>
void base()
{
    switch (lookahead)
    {
    case '(':
        match('(');
        expr();
        match(')');
        break;

    case '-':
        match('-');
        base();
        cout << "neg\n";
        break;

    case NUM:
        emit(NUM, tokenval);
        match(NUM);
        break;

    case ID:
        emit(ID, tokenval);
        match(ID);
        break;

    default:
        exit(1);
    }
}

/*
<factor> ::= <base> <factor>

<factor> ::= ^ <base> <factor> | ε
*/
void factor()
{
    base();

    while (1)
    {
        switch (lookahead)
        {
        case '^':
            match('^');
            base();
            cout << "power\n";
            break;

        default:
            return;
        }
    }
}

/*
<term> ::= <factor> <term>

<term> ::= * <factor> <term>
          | / <factor> <term>
          | mod <factor> <term>
          | div <factor> <term>
          | and <factor> <term>
          | ε
*/

void term()
{
    int t;
    factor();
    while (1)
    {
        switch (lookahead)
        {
        case '*':
        case '/':
        case MOD:
        case DIV:
        case AND:
            t = lookahead;
            match(t);
            factor();
            emit(t, NONE);
            continue;
        default:
            return;
        }
    }
}

/*
<expr> ::= <term> <expr>

<expr> ::= + <term> <expr>
          | - <term> <expr>
          | or <term> <expr>
          | ε
*/

void expr()
{
    int t;
    term();
    while (1)
    {
        switch (lookahead)
        {
        case '+':
        case '-':
        case OR:
            t = lookahead;
            match(t);
            term();
            emit(t, NONE);
            continue;
        default:
            return;
        }
    }
}

/* ===== condition ===== */

// <condition> ::= <expr> <relop> <expr>
void condition()
{
    expr();

    int t = lookahead;
    match(t);

    expr();

    emit(t, NONE);
}

/* ===== statement ===== */

/*
<statement> ::= <id> = <expr>
              | if ( <condition> ) then <statement> <if_tail>
              | while ( <condition> ) do <statement>
              | for <id> = <expr> to <expr> do <statement>
              | repeat <statement> until ( <condition> )
              | do <statement> while ( <condition> )
              | begin <statement_list> end
              | print ( <expr> )
              | read ( <id> )
*/
void statement()
{
    int tok = tokenval;

    switch (lookahead)
    {

    case ID:
        match(ID);
        match('=');
        expr();
        cout << "pop " << symtable[tok].lexptr << "\n";
        break;

    case IF:
        match(IF);
        match('(');
        condition();
        match(')');
        cout << "pop r2\ncmp r2,0\nbe else\n";
        match(THEN);
        statement();
        match(ELSE);
        cout << "else:\n";
        statement();
        break;

    case WHILE:
        match(WHILE);
        cout << "while:\n";
        match('(');
        condition();
        match(')');
        match(DO);
        cout << "pop r2\ncmp r2,0\nbe endwhile\n";
        statement();
        cout << "b while\nendwhile:\n";
        break;

    case FOR: // for <id> = <expr> to <expr> do <statement>
        match(FOR);
        cout << "for\n";
        match(ID);
        match('=');
        expr();
        cout << "pop r1\n";
        match(TO);
        expr();
        cout << "pop r2\n";
        cout << "sub r2,r1\n";
        cout << "cmp r2,0\n";
        cout << "be endfor\n";
        match(DO);
        statement();
        /* decrement loop counter and test before jumping back */
        cout << "sub r2,1\n";
        cout << "cmp r2,0\n";
        cout << "b for\n";
        cout << "endfor:\n";

        // case FOR : // for (id = expr ; expr ; expr) do stmt
        // int tok = tokenval;
        //     match(FOR);
        // 	match('(');

        // 	match(ID);
        // 	match('=');
        // 	expr();
        // 	cout << "pop " << symtable[tok].lexptr << "\n";
        // 	match(';');
        // 	cout << "for\n";
        // 	expr();
        // 	cout << "pop r2\n";
        // 	cout << "cmp r2, 0\n";
        // 	cout << "be endfor\n";
        // 	match(';');
        // 	expr();
        // 	match(')');
        // 	match(DO);
        // 	statement();
        // 	cout << "update\n";
        // 	cout << "b for\n";
        // 	cout << "endfor\n";
        // 	break;

    case REPEAT:
        match(REPEAT);
        cout << "repeat:\n";
        statement();
        match(UNTIL);
        match('(');
        condition();
        match(')');
        cout << "pop r2\ncmp r2,0\nbe repeat\n";
        break;

    case DO:
        match(DO);
        cout << "do:\n";
        statement();
        match(WHILE);
        match('(');
        condition();
        match(')');
        cout << "pop r2\ncmp r2,0\nbne do\n";
        break;

    case PRINT:
        match(PRINT);
        match('(');
        expr();
        match(')');
        cout << "print\n";
        break;

    case READ:
        match(READ);
        match('(');
        match(ID);
        match(')');
        cout << "read\n";
        break;

    default:
        error();
    }
}
void error()
{
    cout << "sytaxt error\n";
    exit(1);
}

/* ===== program ===== */

void program()
{
    statement();
    while (lookahead == ';')
    {
        match(';');
        statement();
    }
}

/* ================= MAIN ================= */

void init()
{
    for (entry *p = keywords; p->token; p++)
        insert((char *)p->lexptr, p->token);
}

int main()
{
    init();
    cout << "Enter program:\n";
    lookahead = lexan();
    program();
    return 0;
}