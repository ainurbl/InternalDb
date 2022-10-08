%define api.value.type {Query}
%parse-param {Query *ret}


%code top {
    #include <stdio.h>
    #include "query.h"
    #include <string>

    extern int yylex(void);

    static void yyerror(Query *ret, const char* s) {
        printf("%s\n", s);
    }
}

%token SEMICOLON
%token IDENTIFIER
%token INTEGER
%token STRING
%token REAL
%token KW_CREATE
%token KW_TABLE
%token KW_INSERT
%token KW_INTO
%token KW_VALUES
%token KW_DELETE
%token KW_FROM
%token KW_WHERE
%token KW_TRUE KW_FALSE KW_OR KW_AND KW_NOT
%token LPAREN RPAREN
%token PLUS MINUS MULT DIV
%token KW_INT KW_REAL KW_TEXT KW_CHAR KW_VARCHAR KW_BOOLEAN
%token EQUALS NOT_EQUALS ASSIGN
%token COMMA
%token KW_SET KW_UPDATE
%token KW_SELECT

%type table_name
%type field_declarations field_declaration field_name datatype
%type KW_INT KW_REAL KW_TEXT KW_CHAR KW_VARCHAR KW_BOOLEAN
%type integer

%right ASSIGN EQUALS NOT_EQUALS
%left KW_AND KW_OR
%left PLUS MINUS MULT DIV
%%

start_internal: start  { *ret = $1; }

start: create_table
	| insert_into
	| delete_from
	| update
	| select

create_table: KW_CREATE KW_TABLE table_name LPAREN field_declarations RPAREN SEMICOLON {
	$$.operation = $1.operation;
	$$.tableName = $3.tableName;
	$$.createNameToType = $5.createNameToType;
}

insert_into: KW_INSERT KW_INTO table_name KW_VALUES LPAREN values RPAREN SEMICOLON {
	$$.operation = $1.operation;
	$$.tableName = $3.tableName;
	$$.insertValueToType = $6.insertValueToType;
}

update: KW_UPDATE table_name KW_SET fields_assigns KW_WHERE logical_expr SEMICOLON {
	$$.updateLogExpr = $6.expr;
	$$.updateFieldToExpr = $4.updateFieldToExpr;
	$$.operation = $1.operation;
	$$.tableName = $2.tableName;
}

delete_from: KW_DELETE KW_FROM table_name SEMICOLON { $$.operation = $1.operation; $$.tableName = $3.tableName; }
	| KW_DELETE KW_FROM table_name KW_WHERE logical_expr SEMICOLON {
	$$.deleteLogExpr = $5.expr;
	$$.operation = $1.operation;
	$$.tableName = $3.tableName;
}

select: KW_SELECT exprs KW_FROM table_name KW_WHERE logical_expr SEMICOLON {
	$$.selectExprs = $2.selectExprs;
	$$.selectLogExpr = $6.expr;
	$$.operation = $1.operation;
	$$.tableName = $4.tableName;
}
	| KW_SELECT MULT KW_FROM table_name KW_WHERE logical_expr SEMICOLON {
	$$.selectLogExpr = $6.expr;
	$$.operation = $1.operation;
	$$.tableName = $4.tableName;
}

table_name: IDENTIFIER { $$.tableName = $1.sval; }

field_name: IDENTIFIER

datatype: KW_INT
	| KW_REAL
	| KW_TEXT
	| KW_CHAR LPAREN integer RPAREN { $$.sval = $$.sval + "#" + std::to_string($3.ival);  }
	| KW_VARCHAR LPAREN integer RPAREN { $$.sval = $$.sval + "#" + std::to_string($3.ival);  }
	| KW_BOOLEAN

value: integer { $$.insertValueToType = {std::make_pair(Value($1.ival), Type("INT"))}; }
	| real { $$.insertValueToType = {std::make_pair(Value($1.fval), Type("REAL"))}; }
	| string { $$.insertValueToType = {std::make_pair(Value($1.sval), Type("TEXT"))}; }

values: value
	| value COMMA values { $$ = $3; $$.insertValueToType.push_back($1.insertValueToType[0]); }

field_assign: field_name ASSIGN expr { $$.updateFieldToExpr = {{$1.sval, $3.expr}}; }

fields_assigns: field_assign
	| field_assign COMMA fields_assigns {
	$$.updateFieldToExpr = $3.updateFieldToExpr;
	$$.updateFieldToExpr.insert($1.updateFieldToExpr.begin(), $1.updateFieldToExpr.end());
}

field_declaration: field_name datatype { $$.createNameToType = {make_pair($1.sval, Type($2.sval))}; }

field_declarations: field_declaration
	| field_declaration COMMA field_declarations { $$ = $3; $$.createNameToType.push_back($1.createNameToType[0]); }

integer: INTEGER

real: REAL

string: STRING

logical_expr: KW_TRUE { $$.expr = new Expr{new ExprEntity{new Value(true), nullptr}, OperationEnum::NONE, nullptr, nullptr}; }
	| KW_FALSE { $$.expr = new Expr{new ExprEntity{new Value(false), nullptr}, OperationEnum::NONE, nullptr, nullptr}; }
	| LPAREN logical_expr RPAREN { $$.expr = $2.expr; }
	| expr EQUALS expr { $$.expr = new Expr{nullptr, OperationEnum::EQUALS, $1.expr, $3.expr}; }
	| expr NOT_EQUALS expr { $$.expr = new Expr{nullptr, OperationEnum::EQUALS, $1.expr, $3.expr}; }
	| logical_expr KW_OR logical_expr { $$.expr = new Expr{nullptr, OperationEnum::KW_OR, $1.expr, $3.expr}; }
	| logical_expr KW_AND logical_expr { $$.expr = new Expr{nullptr, OperationEnum::KW_AND, $1.expr, $3.expr}; }
	| KW_NOT logical_expr { $$.expr = new Expr{nullptr, OperationEnum::KW_NOT, $2.expr, nullptr}; }

expr: integer { $$.expr = new Expr{new ExprEntity{new Value($1.ival), nullptr}, OperationEnum::NONE, nullptr, nullptr}; }
	| real { $$.expr = new Expr{new ExprEntity{new Value($1.fval), nullptr}, OperationEnum::NONE, nullptr, nullptr}; }
	| string { $$.expr = new Expr{new ExprEntity{new Value($1.sval), nullptr}, OperationEnum::NONE, nullptr, nullptr}; }
	| field_name { $$.expr = new Expr{new ExprEntity{nullptr, new std::string{$1.sval}}, OperationEnum::NONE, nullptr, nullptr}; }
	| logical_expr
	| LPAREN expr RPAREN { $$.expr = $2.expr; }
	| expr PLUS expr { $$.expr = new Expr{nullptr, OperationEnum::PLUS, $1.expr, $3.expr}; }
	| expr MINUS expr { $$.expr = new Expr{nullptr, OperationEnum::MINUS, $1.expr, $3.expr}; }
	| expr MULT expr { $$.expr = new Expr{nullptr, OperationEnum::MULT, $1.expr, $3.expr}; }
	| expr DIV expr { $$.expr = new Expr{nullptr, OperationEnum::DIV, $1.expr, $3.expr}; }

exprs: expr { $$.selectExprs = {$1.expr}; }
	| expr COMMA exprs { $$.selectExprs = $3.selectExprs; $$.selectExprs.push_back($1.expr); }




