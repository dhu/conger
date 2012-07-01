grammar CongerCQL;

options
{
    //language=C;
    //ASTLabelType=pANTLR3_BASE_TREE;
    //ASTLabelType=CommonTree;
    output=AST;
    backtrack=false;
    //k=3;
}

tokens
{
	TOK_QUERY;
	TOK_BINARY;
	TOK_SELECT;
	TOK_ARG_LIST;
    TOK_SFW;
    TOK_WHERE;
    TOK_PROJTERM;
    TOK_PROJTERM_LIST;
    TOK_HAVING;
    TOK_GROUP_BY;
    TOK_FROM;
    TOK_JOIN;
    TOK_COND;
    TOK_UNARY_COND;
    TOK_BETWEEN;
    TOK_COMPARE;
    TOK_WINDOW;
    TOK_RELATION_VARIABLE;
    TOK_ATTR_LIST; 
    TOK_AGGR; 
	TOK_COND_LIST;
	TOK_RELATION_LIST;
	TOK_USING;
	TOK_FUNC;
	TOK_ARITH_EXPR;
	TOK_AGGR_EXPR;
}

// starting rule
statement
	: query EOF
	;

query
	: sfw_block
	| binary
	;

binary
	: view1=Identifier	binary_operator view2=Identifier
	    -> ^(TOK_BINARY $view1 $view2 binary_operator)
	;

binary_operator
	: KW_UNION 
	;

sfw_block
	: select_clause from_clause 
	(opt_group_by_clause? opt_having_clause?
    | opt_where_clause opt_group_by_clause? opt_having_clause?
	)
        -> ^(TOK_SFW select_clause from_clause  
        opt_where_clause? opt_group_by_clause? opt_having_clause?)
	;
	
select_clause
	: KW_SELECT (STAR | non_mt_projterm_list)
        -> ^(TOK_SELECT STAR? non_mt_projterm_list?)
	;

non_mt_projterm_list
	: projterm (options{greedy=true;}: COMMA projterm)*
        -> ^(TOK_PROJTERM_LIST projterm+)
	;

// AS 对于 borealis 来说并没有用处
projterm
	: arith_expr (KW_AS alias=Identifier)?
        -> ^(TOK_PROJTERM arith_expr ^(KW_AS $alias)?)
	;
	
arith_expr
	: arith_expr_main (arith_expr_operator^ arith_expr_main)?
	;

arith_expr_operator
    : PLUS | MINUS | STAR | DIVIDE | CONCATENATE
    ;

arith_expr_main
	: attr
	| const_value
	| aggr_expr
	| func_expr
    // 不支持正负, 这个歧义如何消除?
	// | (PLUS | MINUS) arith_expr
	| LPAREN arith_expr RPAREN
		// 这里主要就是把括号不要放到 AST
		-> arith_expr
	;

opt_where_clause
	: KW_WHERE non_mt_cond_list
        -> ^(TOK_WHERE non_mt_cond_list)
	;

opt_having_clause
	: KW_HAVING non_mt_cond_list
        -> ^(TOK_HAVING non_mt_cond_list)
	;

opt_group_by_clause
	: KW_GROUP KW_OVER LSQUARE window_type RSQUARE KW_BY non_mt_attr_list
        -> ^(TOK_GROUP_BY window_type ^(TOK_ATTR_LIST non_mt_attr_list))
	;

// 在语法的定义上， FROM 后面支持通过 comma 分隔多个relation_variable，但是在实现的时候并不支持这个。
from_clause
	: KW_FROM non_mt_relation_list 
	(KW_JOIN relation_variable KW_ON non_mt_cond_list (KW_TIMEOUT timeout=Integer unit=time_unit?)?)? 
        -> ^(TOK_FROM non_mt_relation_list 
            ^(TOK_JOIN relation_variable non_mt_cond_list ^(KW_TIMEOUT $timeout $unit?)? )?
            )
	;

non_mt_cond_list
	: non_mt_or_cond
        -> ^(TOK_COND_LIST non_mt_or_cond)
	;
	
non_mt_or_cond
	: non_mt_and_cond ( KW_OR^ non_mt_and_cond)*
	;
	
non_mt_and_cond
	: non_mt_not_cond (KW_AND^ non_mt_not_cond)*
	;
	
non_mt_not_cond
	: (KW_NOT^)* unary_condition
	;
	
// 为啥这个叫  unary？根本就不是一元的啊
unary_condition
	: arith_expr (unary_condition_operator^ arith_expr |
		KW_BETWEEN^ arith_expr KW_AND! arith_expr)
	;
	
between_condition_right
	: KW_BETWEEN^ arith_expr KW_AND! arith_expr
	;

compare_condition_right
	: unary_condition_operator arith_expr
	;

unary_condition_operator
    : LESSTHAN | LESSTHANOREQUALTO | GREATERTHAN | GREATERTHANOREQUALTO | EQUAL | NOTEQUAL
    ;
	
const_string
	: StringLiteral
	;

non_mt_arg_list
	: arith_expr (options{greedy=true;}: COMMA non_mt_arg_list)*
		-> ^(TOK_ARG_LIST arith_expr+)
	;
	
non_mt_relation_list
	: relation_variable (options{greedy=true;}: COMMA non_mt_relation_list)*
        -> ^(TOK_RELATION_LIST relation_variable+)
	;

relation_variable
	: variableName=Identifier (LSQUARE window_type RSQUARE)? ( KW_AS alias=Identifier)?
        -> ^(TOK_RELATION_VARIABLE $variableName window_type? $alias?)
	;

window_type
	: KW_RANGE range=time_spec ( KW_SLIDE slidetime=time_spec)? (KW_ON arith_expr)? (KW_TIMEOUT timeout=Integer unit=time_unit?)?
	    -> ^(TOK_WINDOW ^(KW_RANGE $range) ^(KW_SLIDE $slidetime)? ^(KW_TIMEOUT $timeout $unit?)?)
	| KW_ROWS row=Integer ( KW_SLIDE slide=Integer)?  (KW_TIMEOUT timeout=Integer unit=time_unit?)?
        -> ^(TOK_WINDOW ^(KW_ROWS $row) ^(KW_SLIDE $slide)?  ^(KW_TIMEOUT $timeout $unit?)?)
	;

non_mt_attr_list
	: arith_expr (options{greedy=true;}: COMMA non_mt_attr_list)*
        ->  arith_expr non_mt_attr_list*
	;

const_value
	: const_string
	| KW_NULL
	| const_int
	| const_float
	;

time_spec
	: Integer time_unit
	;

time_unit
	: KW_DAY
	| KW_DAYS
	| KW_HOUR
	| KW_HOURS
	| KW_MINUTE
	| KW_MINUTES
	| KW_SECOND
	| KW_SECONDS
	| KW_MILLISECOND
	| KW_MILLISECONDS
	| KW_MICROSECOND
	| KW_MICROSECONDS
	| KW_NANOSECOND
	| KW_NANOSECONDS
	;

aggr_expr
	: KW_COUNT LPAREN ( arith_expr_main | STAR) RPAREN
        -> ^(TOK_AGGR KW_COUNT arith_expr_main? STAR?)
	| ( (KW_SUM | KW_AVG) LPAREN arith_expr_main
		| ( KW_MAX | KW_MIN) LPAREN arith_expr_main
	) RPAREN
        -> ^(TOK_AGGR KW_SUM? KW_AVG? KW_MAX? KW_MIN? arith_expr_main)
	;

func_expr
	: func_name LPAREN ( non_mt_arg_list | KW_DISTINCT arith_expr) RPAREN
        -> ^(TOK_FUNC func_name non_mt_arg_list? KW_DISTINCT? arith_expr?)
	;

func_name
	: Identifier
	;

attr
	: Identifier ( DOT^ Identifier | DOT^ pseudo_column)?
	| pseudo_column
	;

pseudo_column
	: element_time
	;

element_time
	: KW_ELEMENT_TIME
	;

const_int
	: Integer
	;
	
const_float
	: Number
	;

// Keywords

KW_TRUE : 'TRUE';
KW_FALSE : 'FALSE';
KW_ALL : 'ALL';
KW_AND : 'AND';
KW_OR : 'OR';
KW_NOT : 'NOT' | '!';
KW_LIKE : 'LIKE';

KW_IF : 'IF';
KW_EXISTS : 'EXISTS';

KW_ASC : 'ASC';
KW_DESC : 'DESC';
KW_ORDER : 'ORDER';
KW_GROUP : 'GROUP';
KW_BY : 'BY';
KW_HAVING : 'HAVING';
KW_WHERE : 'WHERE';
KW_FROM : 'FROM';
KW_AS : 'AS';
KW_SELECT : 'SELECT';
KW_DISTINCT : 'DISTINCT';
KW_OUTER : 'OUTER';
KW_JOIN : 'JOIN';
KW_LEFT : 'LEFT';
KW_RIGHT : 'RIGHT';
KW_FULL : 'FULL';
KW_ON : 'ON';
KW_PARTITION : 'PARTITION';
KW_USING: 'USING';
KW_UNION: 'UNION';
KW_IS: 'IS';
KW_NULL: 'NULL';
KW_CREATE: 'CREATE';
KW_COLUMN: 'COLUMN';
KW_FIRST: 'FIRST';
KW_AFTER: 'AFTER';
KW_DESCRIBE: 'DESCRIBE';
KW_TO: 'TO';
KW_COMMENT: 'COMMENT';
KW_BOOLEAN: 'BOOLEAN';
KW_INT: 'INT';
KW_BIGINT: 'BIGINT';
KW_FLOAT: 'FLOAT';
KW_DOUBLE: 'DOUBLE';
KW_DATE: 'DATE';
KW_DATETIME: 'DATETIME';
KW_INTO: 'INTO';
KW_ROW: 'ROW';
KW_OUT: 'OUT';
KW_OF: 'OF';
KW_WITH: 'WITH';
KW_LIMIT: 'LIMIT';
KW_SET: 'SET';
KW_LONG: 'LONG';
KW_PLUS: 'PLUS';
KW_MINUS: 'MINUS';
KW_VIEW: 'VIEW';
KW_IN: 'IN';
KW_BEFORE: 'BEFORE';
KW_BETWEEN: 'BETWEEN';
KW_BOTH: 'BOTH';
KW_BINARY: 'BINARY';
KW_CROSS: 'CROSS';
KW_USE: 'USE';
KW_UPDATE: 'UPDATE';

KW_SLIDE: 'SLIDE';
KW_UNBOUNDED: 'UNBOUNDED'	;
KW_NOW: 'NOW';
KW_ROWS	: 'ROWS';
KW_EXCEPT: 'EXCEPT';
KW_LAST	: 'LAST';
KW_DSTREAM: 'DSTREAM';
KW_ISTREAM: 'ISTREAM';
KW_RSTREAM: 'RSTREAM';
KW_DIFFERENCE: 'DIFFERENCE';
KW_PASSING: 'PASSING';
KW_VALUE: 'VALUE';
KW_DEFAULT: 'DEFAULT';
KW_XOR: 'XOR';
KW_DAY: 'DAY';
KW_DAYS: 'DAYS';
KW_SECOND: 'SECOND';
KW_SECONDS: 'SECONDS';
KW_HOUR: 'HOUR';	
KW_HOURS: 'HOURS';	
KW_MINUTE: 'MINUTE';	
KW_MINUTES: 'MINUTES';	
KW_MILLISECOND: 'MILLISECOND';	
KW_MILLISECONDS: 'MILLISECONDS';	
KW_MICROSECOND: 'MICROSECOND';	
KW_MICROSECONDS: 'MICROSECONDS';	
KW_NANOSECOND: 'NANOSECOND';	
KW_NANOSECONDS: 'NANOSECONDS';	
KW_DECODE: 'DECODE';
KW_NULLS: 'NULLS';
KW_INTERVAL: 'INTERVAL';
KW_ELEMENT_TIME: 'ELEMENT_TIME';
KW_COUNT: 'COUNT'; 
KW_SUM: 'SUM';
KW_AVG: 'AVG';
KW_MAX: 'MAX';
KW_MIN: 'MIN';
KW_INTERSECT: 'INTERSECT';
KW_RANGE: 'RANGE';
KW_TIMEOUT: 'TIMEOUT';
KW_OVER	: 'OVER';

// Operators
// NOTE: if you add a new function/operator, add it to sysFuncNames so that describe function _FUNC_ will work.

DOT : '.'; // generated as a part of Number rule
COLON : ':' ;
COMMA : ',' ;
SEMICOLON : ';' ;

LPAREN : '(' ;
RPAREN : ')' ;
LSQUARE : '[' ;
RSQUARE : ']' ;
LCURLY : '{';
RCURLY : '}';

EQUAL : '=' | '==';
NOTEQUAL : '<>' | '!=';
LESSTHANOREQUALTO : '<=';
LESSTHAN : '<';
GREATERTHANOREQUALTO : '>=';
GREATERTHAN : '>';

DIVIDE : '/';
PLUS : '+';
MINUS : '-';
STAR : '*';
MOD : '%';
DIV : 'DIV';

AMPERSAND : '&';
TILDE : '~';
BITWISEOR : '|';
BITWISEXOR : '^';
QUESTION : '?';
DOLLAR : '$';

CONCATENATE: '||';	

// LITERALS
fragment
Letter
    : 'a'..'z' | 'A'..'Z'
    ;

fragment
HexDigit
    : 'a'..'f' | 'A'..'F'
    ;

fragment
Digit
    :
    '0'..'9'
    ;

fragment
Exponent
    :
    'e' ( PLUS|MINUS )? (Digit)+
    ;

fragment
RegexComponent
    : 'a'..'z' | 'A'..'Z' | '0'..'9' | '_'
    | PLUS | STAR | QUESTION | MINUS | DOT
    | LPAREN | RPAREN | LSQUARE | RSQUARE | LCURLY | RCURLY
    | BITWISEXOR | BITWISEOR | DOLLAR
    ;

StringLiteral
    :
    ( '\'' ( ~('\''|'\\') | ('\\' .) )* '\''
    | '\"' ( ~('\"'|'\\') | ('\\' .) )* '\"'
    )+
    ;

CharSetLiteral
    :
    StringLiteral
    | '0' 'X' (HexDigit|Digit)+
    ;

Integer
	: 
	(Digit)+
	;
	
Number
    :
    (Digit)+ ( DOT (Digit)* (Exponent)? | Exponent)?
    ;

Identifier
    :
    (Letter | Digit) (Letter | Digit | '_')*
    | '`' RegexComponent+ '`'
    ;

CharSetName
    :
    '_' (Letter | Digit | '_' | '-' | '.' | ':' )+
    ;

WS  :  (' '|'\r'|'\t'|'\n') {$channel=HIDDEN;}
    ;

COMMENT
  : '--' (~('\n'|'\r'))*
    { $channel=HIDDEN; }
  ;

