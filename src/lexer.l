%option noyywrap

%top {
    #include "query.h"
    #include "parser.h"
    #include <string.h>

    static char* toUpper(char *src) {
        char* ret = src;
        while (*src) {
            *src = toupper(*src);
            src++;
        }
        return ret;
    }
}

%%

";"             { return SEMICOLON; }

"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return MULT; }
"/"             { return DIV; }

"("             { return LPAREN; }
")"             { return RPAREN; }
","             { return COMMA; }


(?i:TRUE)       { return KW_TRUE; }
(?i:FALSE)      { return KW_FALSE; }
(?i:OR)         { return KW_OR; }
(?i:AND)        { return KW_AND; }
(?i:NOT)        { return KW_NOT; }

"="             { return ASSIGN; }
"=="            { return EQUALS; }
"!="            { return NOT_EQUALS; }


(?i:CREATE)     { yylval.operation = "CREATE"; return KW_CREATE; }
(?i:TABLE)      { return KW_TABLE; }

(?i:INSERT)     { yylval.operation = "INSERT"; return KW_INSERT; }
(?i:INTO)       { return KW_INTO; }
(?i:VALUES)     { return KW_VALUES; }

(?i:DELETE)     { yylval.operation = "DELETE"; return KW_DELETE; }
(?i:FROM)       { return KW_FROM; }

(?i:WHERE)      { return KW_WHERE; }

(?i:SET)        { return KW_SET; }
(?i:UPDATE)     { yylval.operation = "UPDATE"; return KW_UPDATE; }

(?i:SELECT)     { yylval.operation = "SELECT"; return KW_SELECT; }

(?i:INT)        { yylval.sval = toUpper(strdup(yytext)); return KW_INT; }
(?i:REAL)       { yylval.sval = toUpper(strdup(yytext)); return KW_REAL; }
(?i:TEXT)       { yylval.sval = toUpper(strdup(yytext)); return KW_TEXT; }
(?i:CHAR)       { yylval.sval = toUpper(strdup(yytext)); return KW_CHAR; }
(?i:VARCHAR)    { yylval.sval = toUpper(strdup(yytext)); return KW_VARCHAR; }
(?i:BOOLEAN)    { yylval.sval = toUpper(strdup(yytext)); return KW_BOOLEAN; }

[a-zA-Z][a-zA-Z0-9]*    { yylval.sval = strdup(yytext); return IDENTIFIER; }
-?[0-9]+                { yylval.ival = atoi(yytext); return INTEGER; }
-?[0-9]+\.[0-9]+        { yylval.fval = atof(yytext); return REAL; }
\'[a-zA-Z0-9]*\'        { yylval.sval = strdup(yytext); return STRING; }
. ;
