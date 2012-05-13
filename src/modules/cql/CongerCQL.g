grammar CongerCQL;

options
{
    //language=C;
    //ASTLabelType=pANTLR3_BASE_TREE;
    ASTLabelType=CommonTree;
    output=AST;
    backtrack=false;
    //k=3;
}

tokens
{
	TOK_CREATE_VIEW;
	TOK_QUERY;
	TOK_BINARY;
	TOK_SELECT;
	TOK_ARG_LIST;
	TOK_IDSTREAM;
    TOK_RSTREAM;
    TOK_SFW;
    TOK_WHERE;
    TOK_PROJTERM;
    TOK_PROJTERM_LIST;
    TOK_ORDER_BY;
    TOK_ORDER_EXPR;
    TOK_HAVING;
    TOK_GROUP_BY;
    TOK_FROM;
    TOK_JOIN;
    TOK_COND;
    TOK_UNARY_COND;
    TOK_BETWEEN;
    TOK_COMPARE;
    TOK_PARTITION;
    TOK_WINDOW;
    TOK_RELATION_VARIABLE;
    TOK_ATTR_LIST; 
    TOK_AGGR; 
    TOK_AGGR_DISTINCT;
	TOK_COND_LIST;
	TOK_RELATION_LIST;
	TOK_USING;
	TOK_FUNC;
	TOK_ARITH_EXPR;
}

// starting rule
statement
	: query EOF
	;

query
	: sfw_block
	| idstream_clause LPAREN sfw_block RPAREN using_clause
        -> ^(TOK_IDSTREAM sfw_block using_clause)
	| rstream LPAREN sfw_block RPAREN
        -> ^(TOK_RSTREAM sfw_block)
	| binary
	| idstream_clause LPAREN binary RPAREN using_clause
        -> ^(TOK_IDSTREAM binary using_clause)
	| rstream LPAREN binary RPAREN
        -> ^(TOK_RSTREAM binary)
	;

binary
	: view1=Identifier	binary_operator view2=Identifier
	    -> ^(TOK_BINARY $view1 $view2 binary_operator)
	;

binary_operator
	: KW_EXCEPT
	| KW_MINUS
	| KW_INTERSECT
	| KW_UNION KW_ALL?
	| KW_NOT? KW_IN
	;

sfw_block
	: select_clause from_clause 
	(opt_group_by_clause? opt_having_clause?
    | order_by_clause
    | opt_where_clause opt_group_by_clause? opt_having_clause?
	)
        -> ^(TOK_SFW select_clause from_clause order_by_clause? 
        opt_where_clause? opt_group_by_clause? opt_having_clause?)
	;
	
select_clause
	: KW_SELECT KW_DISTINCT? (STAR | non_mt_projterm_list)
        -> ^(TOK_SELECT KW_DISTINCT? STAR? non_mt_projterm_list?)
	;

non_mt_projterm_list
	: projterm (options{greedy=true;}: COMMA projterm)*
        -> ^(TOK_PROJTERM_LIST projterm+)
	;

projterm
	: arith_expr (KW_AS alias=Identifier)?
        -> ^(TOK_PROJTERM arith_expr KW_AS? $alias?)
	;
	
arith_expr
	: arith_expr_main (arith_expr_operator arith_expr_main)?
        // 这里的抽象树能不能后两个用一个括号括起，再加一个问号
        -> ^(TOK_ARITH_EXPR arith_expr_main arith_expr_operator? arith_expr_main?)
	;

arith_expr_operator
    : PLUS | MINUS | STAR | DIVIDE | CONCATENATE
    ;

arith_expr_main
	: attr
	| const_value
	| aggr_expr
	| aggr_distinct_expr
	| func_expr
    // 不支持正负, 这个歧义如何消除
	// | (PLUS | MINUS) arith_expr
	| LPAREN arith_expr RPAREN
	;
	
order_by_clause
	: KW_ORDER KW_BY order_by_list
        -> ^(TOK_ORDER_BY order_by_list)
	;

order_by_list
	: orderterm (options{greedy=true;}: COMMA order_by_list)*
        -> orderterm*
	;

orderterm
	: order_expr asc_desc? null_spec?
        -> ^(TOK_ORDER_EXPR order_expr asc_desc? null_spec?)
	;

asc_desc
	: KW_ASC
	| KW_DESC
	;

null_spec
	: KW_NULLS (KW_FIRST | KW_LAST)
	;

order_expr
	: attr
	| const_int
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
	: KW_GROUP KW_BY non_mt_attr_list
        -> ^(TOK_GROUP_BY non_mt_attr_list)
	;

from_clause
	: KW_FROM non_mt_relation_list 
	(((KW_LEFT | KW_RIGHT)? KW_OUTER)? KW_JOIN relation_variable KW_ON non_mt_cond_list)? 
	// XXX 只能连接两张表吗？
        -> ^(TOK_FROM non_mt_relation_list 
            ^(TOK_JOIN relation_variable KW_ON non_mt_cond_list KW_LEFT? KW_RIGHT? KW_OUTER?)?)
	;

non_mt_cond_list
	: non_mt_cond_list_main (cond_list_operator non_mt_cond_list_main)*
        -> ^(TOK_COND_LIST ^(TOK_COND cond_list_operator? non_mt_cond_list_main)+)
	;

cond_list_operator
    : KW_XOR | KW_OR | KW_AND
    ;
	
non_mt_cond_list_main
options {backtrack=true;}
	: KW_NOT non_mt_cond_list_main
	| unary_condition
	| LPAREN unary_condition RPAREN
	;

unary_condition
	: arith_expr (between_condition_right | compare_condition_right)
	;
	
between_condition_right
	: KW_BETWEEN arith_expr KW_AND arith_expr
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
        -> ^(TOK_RELATION_VARIABLE $variableName ^(TOK_WINDOW window_type)? $alias?)
	;

window_type
	: KW_RANGE 
		(time_spec ( KW_SLIDE time_spec)?
		| const_value KW_ON Identifier // XXX 这个语句是什么意思？
		| KW_UNBOUNDED
		)
	| KW_NOW
	| KW_ROWS Integer ( KW_SLIDE Integer)?
	| KW_PARTITION KW_BY non_mt_attr_list KW_ROWS	
		rows=Integer (KW_RANGE time_spec (KW_SLIDE time_spec)? )?
        -> ^(TOK_PARTITION non_mt_attr_list $rows (KW_RANGE time_spec (KW_SLIDE time_spec)?)?)
	;

non_mt_attr_list
	: attr (options{greedy=true;}: COMMA non_mt_attr_list)*
        -> ^(TOK_ATTR_LIST attr+)
	;

const_value
	: interval_value
	| const_string
	| KW_NULL
	| const_int
	| KW_FLOAT
	| KW_DOUBLE
	;

interval_value
	: KW_INTERVAL const_string (KW_DAY | KW_DAYS) KW_TO (KW_SECOND|KW_SECONDS)
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

aggr_distinct_expr
	: aggr_distinct_operator LPAREN KW_DISTINCT arith_expr RPAREN
        -> ^(TOK_AGGR_DISTINCT aggr_distinct_operator arith_expr)
	;
aggr_distinct_operator
	: KW_COUNT | KW_SUM | KW_AVG | KW_MAX | KW_MIN
	;
aggr_expr
	: KW_COUNT LPAREN ( arith_expr | STAR) RPAREN
        -> ^(TOK_AGGR KW_COUNT arith_expr? STAR?)
	| ( (KW_SUM | KW_AVG) LPAREN arith_expr
		| ( KW_MAX | KW_MIN) LPAREN arith_expr
	) RPAREN
        -> ^(TOK_AGGR KW_SUM? KW_AVG? KW_MAX KW_MIN? arith_expr)
	;

func_expr
	: func_name LPAREN ( non_mt_arg_list | KW_DISTINCT arith_expr) RPAREN
        -> ^(TOK_FUNC func_name non_mt_arg_list? KW_DISTINCT? arith_expr?)
	;

func_name
	: Identifier
	;

idstream_clause
	: KW_ISTREAM
	| KW_DSTREAM
	;

using_clause
	: KW_DIFFERENCE KW_USING LPAREN usinglist RPAREN
        -> ^(TOK_USING usinglist)
	;

usinglist
	: usingterm (options{greedy=true;}: COMMA usinglist)*
        -> usingterm+
	;

usingterm
	: usingexpr
	;

usingexpr
	: attr
	| const_int
	;

attr
	: Identifier ( DOT Identifier | DOT pseudo_column)?
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

rstream
	: KW_RSTREAM
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

