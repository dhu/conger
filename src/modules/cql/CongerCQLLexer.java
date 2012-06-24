// $ANTLR 3.4 CongerCQL.g 2012-06-23 22:12:56

import org.antlr.runtime.*;
import java.util.Stack;
import java.util.List;
import java.util.ArrayList;

@SuppressWarnings({"all", "warnings", "unchecked"})
public class CongerCQLLexer extends Lexer {
    public static final int EOF=-1;
    public static final int AMPERSAND=4;
    public static final int BITWISEOR=5;
    public static final int BITWISEXOR=6;
    public static final int COLON=7;
    public static final int COMMA=8;
    public static final int COMMENT=9;
    public static final int CONCATENATE=10;
    public static final int CharSetLiteral=11;
    public static final int CharSetName=12;
    public static final int DIV=13;
    public static final int DIVIDE=14;
    public static final int DOLLAR=15;
    public static final int DOT=16;
    public static final int Digit=17;
    public static final int EQUAL=18;
    public static final int Exponent=19;
    public static final int GREATERTHAN=20;
    public static final int GREATERTHANOREQUALTO=21;
    public static final int HexDigit=22;
    public static final int Identifier=23;
    public static final int Integer=24;
    public static final int KW_AFTER=25;
    public static final int KW_ALL=26;
    public static final int KW_AND=27;
    public static final int KW_AS=28;
    public static final int KW_ASC=29;
    public static final int KW_AVG=30;
    public static final int KW_BEFORE=31;
    public static final int KW_BETWEEN=32;
    public static final int KW_BIGINT=33;
    public static final int KW_BINARY=34;
    public static final int KW_BOOLEAN=35;
    public static final int KW_BOTH=36;
    public static final int KW_BY=37;
    public static final int KW_COLUMN=38;
    public static final int KW_COMMENT=39;
    public static final int KW_COUNT=40;
    public static final int KW_CREATE=41;
    public static final int KW_CROSS=42;
    public static final int KW_DATE=43;
    public static final int KW_DATETIME=44;
    public static final int KW_DAY=45;
    public static final int KW_DAYS=46;
    public static final int KW_DECODE=47;
    public static final int KW_DEFAULT=48;
    public static final int KW_DESC=49;
    public static final int KW_DESCRIBE=50;
    public static final int KW_DIFFERENCE=51;
    public static final int KW_DISTINCT=52;
    public static final int KW_DOUBLE=53;
    public static final int KW_DSTREAM=54;
    public static final int KW_ELEMENT_TIME=55;
    public static final int KW_EXCEPT=56;
    public static final int KW_EXISTS=57;
    public static final int KW_FALSE=58;
    public static final int KW_FIRST=59;
    public static final int KW_FLOAT=60;
    public static final int KW_FROM=61;
    public static final int KW_FULL=62;
    public static final int KW_GROUP=63;
    public static final int KW_HAVING=64;
    public static final int KW_HOUR=65;
    public static final int KW_HOURS=66;
    public static final int KW_IF=67;
    public static final int KW_IN=68;
    public static final int KW_INT=69;
    public static final int KW_INTERSECT=70;
    public static final int KW_INTERVAL=71;
    public static final int KW_INTO=72;
    public static final int KW_IS=73;
    public static final int KW_ISTREAM=74;
    public static final int KW_JOIN=75;
    public static final int KW_LAST=76;
    public static final int KW_LEFT=77;
    public static final int KW_LIKE=78;
    public static final int KW_LIMIT=79;
    public static final int KW_LONG=80;
    public static final int KW_MAX=81;
    public static final int KW_MICROSECOND=82;
    public static final int KW_MICROSECONDS=83;
    public static final int KW_MILLISECOND=84;
    public static final int KW_MILLISECONDS=85;
    public static final int KW_MIN=86;
    public static final int KW_MINUS=87;
    public static final int KW_MINUTE=88;
    public static final int KW_MINUTES=89;
    public static final int KW_NANOSECOND=90;
    public static final int KW_NANOSECONDS=91;
    public static final int KW_NOT=92;
    public static final int KW_NOW=93;
    public static final int KW_NULL=94;
    public static final int KW_NULLS=95;
    public static final int KW_OF=96;
    public static final int KW_ON=97;
    public static final int KW_OR=98;
    public static final int KW_ORDER=99;
    public static final int KW_OUT=100;
    public static final int KW_OUTER=101;
    public static final int KW_OVER=102;
    public static final int KW_PARTITION=103;
    public static final int KW_PASSING=104;
    public static final int KW_PLUS=105;
    public static final int KW_RANGE=106;
    public static final int KW_RIGHT=107;
    public static final int KW_ROW=108;
    public static final int KW_ROWS=109;
    public static final int KW_RSTREAM=110;
    public static final int KW_SECOND=111;
    public static final int KW_SECONDS=112;
    public static final int KW_SELECT=113;
    public static final int KW_SET=114;
    public static final int KW_SLIDE=115;
    public static final int KW_SUM=116;
    public static final int KW_TIMEOUT=117;
    public static final int KW_TO=118;
    public static final int KW_TRUE=119;
    public static final int KW_UNBOUNDED=120;
    public static final int KW_UNION=121;
    public static final int KW_UPDATE=122;
    public static final int KW_USE=123;
    public static final int KW_USING=124;
    public static final int KW_VALUE=125;
    public static final int KW_VIEW=126;
    public static final int KW_WHERE=127;
    public static final int KW_WITH=128;
    public static final int KW_XOR=129;
    public static final int LCURLY=130;
    public static final int LESSTHAN=131;
    public static final int LESSTHANOREQUALTO=132;
    public static final int LPAREN=133;
    public static final int LSQUARE=134;
    public static final int Letter=135;
    public static final int MINUS=136;
    public static final int MOD=137;
    public static final int NOTEQUAL=138;
    public static final int Number=139;
    public static final int PLUS=140;
    public static final int QUESTION=141;
    public static final int RCURLY=142;
    public static final int RPAREN=143;
    public static final int RSQUARE=144;
    public static final int RegexComponent=145;
    public static final int SEMICOLON=146;
    public static final int STAR=147;
    public static final int StringLiteral=148;
    public static final int TILDE=149;
    public static final int TOK_AGGR=150;
    public static final int TOK_AGGR_EXPR=151;
    public static final int TOK_ARG_LIST=152;
    public static final int TOK_ARITH_EXPR=153;
    public static final int TOK_ATTR_LIST=154;
    public static final int TOK_BETWEEN=155;
    public static final int TOK_BINARY=156;
    public static final int TOK_COMPARE=157;
    public static final int TOK_COND=158;
    public static final int TOK_COND_LIST=159;
    public static final int TOK_FROM=160;
    public static final int TOK_FUNC=161;
    public static final int TOK_GROUP_BY=162;
    public static final int TOK_HAVING=163;
    public static final int TOK_JOIN=164;
    public static final int TOK_PROJTERM=165;
    public static final int TOK_PROJTERM_LIST=166;
    public static final int TOK_QUERY=167;
    public static final int TOK_RELATION_LIST=168;
    public static final int TOK_RELATION_VARIABLE=169;
    public static final int TOK_SELECT=170;
    public static final int TOK_SFW=171;
    public static final int TOK_UNARY_COND=172;
    public static final int TOK_USING=173;
    public static final int TOK_WHERE=174;
    public static final int TOK_WINDOW=175;
    public static final int WS=176;

    // delegates
    // delegators
    public Lexer[] getDelegates() {
        return new Lexer[] {};
    }

    public CongerCQLLexer() {} 
    public CongerCQLLexer(CharStream input) {
        this(input, new RecognizerSharedState());
    }
    public CongerCQLLexer(CharStream input, RecognizerSharedState state) {
        super(input,state);
    }
    public String getGrammarFileName() { return "CongerCQL.g"; }

    // $ANTLR start "KW_TRUE"
    public final void mKW_TRUE() throws RecognitionException {
        try {
            int _type = KW_TRUE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:265:9: ( 'TRUE' )
            // CongerCQL.g:265:11: 'TRUE'
            {
            match("TRUE"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_TRUE"

    // $ANTLR start "KW_FALSE"
    public final void mKW_FALSE() throws RecognitionException {
        try {
            int _type = KW_FALSE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:266:10: ( 'FALSE' )
            // CongerCQL.g:266:12: 'FALSE'
            {
            match("FALSE"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_FALSE"

    // $ANTLR start "KW_ALL"
    public final void mKW_ALL() throws RecognitionException {
        try {
            int _type = KW_ALL;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:267:8: ( 'ALL' )
            // CongerCQL.g:267:10: 'ALL'
            {
            match("ALL"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_ALL"

    // $ANTLR start "KW_AND"
    public final void mKW_AND() throws RecognitionException {
        try {
            int _type = KW_AND;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:268:8: ( 'AND' )
            // CongerCQL.g:268:10: 'AND'
            {
            match("AND"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_AND"

    // $ANTLR start "KW_OR"
    public final void mKW_OR() throws RecognitionException {
        try {
            int _type = KW_OR;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:269:7: ( 'OR' )
            // CongerCQL.g:269:9: 'OR'
            {
            match("OR"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_OR"

    // $ANTLR start "KW_NOT"
    public final void mKW_NOT() throws RecognitionException {
        try {
            int _type = KW_NOT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:270:8: ( 'NOT' | '!' )
            int alt1=2;
            int LA1_0 = input.LA(1);

            if ( (LA1_0=='N') ) {
                alt1=1;
            }
            else if ( (LA1_0=='!') ) {
                alt1=2;
            }
            else {
                NoViableAltException nvae =
                    new NoViableAltException("", 1, 0, input);

                throw nvae;

            }
            switch (alt1) {
                case 1 :
                    // CongerCQL.g:270:10: 'NOT'
                    {
                    match("NOT"); 



                    }
                    break;
                case 2 :
                    // CongerCQL.g:270:18: '!'
                    {
                    match('!'); 

                    }
                    break;

            }
            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_NOT"

    // $ANTLR start "KW_LIKE"
    public final void mKW_LIKE() throws RecognitionException {
        try {
            int _type = KW_LIKE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:271:9: ( 'LIKE' )
            // CongerCQL.g:271:11: 'LIKE'
            {
            match("LIKE"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_LIKE"

    // $ANTLR start "KW_IF"
    public final void mKW_IF() throws RecognitionException {
        try {
            int _type = KW_IF;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:273:7: ( 'IF' )
            // CongerCQL.g:273:9: 'IF'
            {
            match("IF"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_IF"

    // $ANTLR start "KW_EXISTS"
    public final void mKW_EXISTS() throws RecognitionException {
        try {
            int _type = KW_EXISTS;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:274:11: ( 'EXISTS' )
            // CongerCQL.g:274:13: 'EXISTS'
            {
            match("EXISTS"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_EXISTS"

    // $ANTLR start "KW_ASC"
    public final void mKW_ASC() throws RecognitionException {
        try {
            int _type = KW_ASC;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:276:8: ( 'ASC' )
            // CongerCQL.g:276:10: 'ASC'
            {
            match("ASC"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_ASC"

    // $ANTLR start "KW_DESC"
    public final void mKW_DESC() throws RecognitionException {
        try {
            int _type = KW_DESC;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:277:9: ( 'DESC' )
            // CongerCQL.g:277:11: 'DESC'
            {
            match("DESC"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_DESC"

    // $ANTLR start "KW_ORDER"
    public final void mKW_ORDER() throws RecognitionException {
        try {
            int _type = KW_ORDER;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:278:10: ( 'ORDER' )
            // CongerCQL.g:278:12: 'ORDER'
            {
            match("ORDER"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_ORDER"

    // $ANTLR start "KW_GROUP"
    public final void mKW_GROUP() throws RecognitionException {
        try {
            int _type = KW_GROUP;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:279:10: ( 'GROUP' )
            // CongerCQL.g:279:12: 'GROUP'
            {
            match("GROUP"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_GROUP"

    // $ANTLR start "KW_BY"
    public final void mKW_BY() throws RecognitionException {
        try {
            int _type = KW_BY;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:280:7: ( 'BY' )
            // CongerCQL.g:280:9: 'BY'
            {
            match("BY"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_BY"

    // $ANTLR start "KW_HAVING"
    public final void mKW_HAVING() throws RecognitionException {
        try {
            int _type = KW_HAVING;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:281:11: ( 'HAVING' )
            // CongerCQL.g:281:13: 'HAVING'
            {
            match("HAVING"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_HAVING"

    // $ANTLR start "KW_WHERE"
    public final void mKW_WHERE() throws RecognitionException {
        try {
            int _type = KW_WHERE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:282:10: ( 'WHERE' )
            // CongerCQL.g:282:12: 'WHERE'
            {
            match("WHERE"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_WHERE"

    // $ANTLR start "KW_FROM"
    public final void mKW_FROM() throws RecognitionException {
        try {
            int _type = KW_FROM;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:283:9: ( 'FROM' )
            // CongerCQL.g:283:11: 'FROM'
            {
            match("FROM"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_FROM"

    // $ANTLR start "KW_AS"
    public final void mKW_AS() throws RecognitionException {
        try {
            int _type = KW_AS;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:284:7: ( 'AS' )
            // CongerCQL.g:284:9: 'AS'
            {
            match("AS"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_AS"

    // $ANTLR start "KW_SELECT"
    public final void mKW_SELECT() throws RecognitionException {
        try {
            int _type = KW_SELECT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:285:11: ( 'SELECT' )
            // CongerCQL.g:285:13: 'SELECT'
            {
            match("SELECT"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_SELECT"

    // $ANTLR start "KW_DISTINCT"
    public final void mKW_DISTINCT() throws RecognitionException {
        try {
            int _type = KW_DISTINCT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:286:13: ( 'DISTINCT' )
            // CongerCQL.g:286:15: 'DISTINCT'
            {
            match("DISTINCT"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_DISTINCT"

    // $ANTLR start "KW_OUTER"
    public final void mKW_OUTER() throws RecognitionException {
        try {
            int _type = KW_OUTER;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:287:10: ( 'OUTER' )
            // CongerCQL.g:287:12: 'OUTER'
            {
            match("OUTER"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_OUTER"

    // $ANTLR start "KW_JOIN"
    public final void mKW_JOIN() throws RecognitionException {
        try {
            int _type = KW_JOIN;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:288:9: ( 'JOIN' )
            // CongerCQL.g:288:11: 'JOIN'
            {
            match("JOIN"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_JOIN"

    // $ANTLR start "KW_LEFT"
    public final void mKW_LEFT() throws RecognitionException {
        try {
            int _type = KW_LEFT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:289:9: ( 'LEFT' )
            // CongerCQL.g:289:11: 'LEFT'
            {
            match("LEFT"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_LEFT"

    // $ANTLR start "KW_RIGHT"
    public final void mKW_RIGHT() throws RecognitionException {
        try {
            int _type = KW_RIGHT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:290:10: ( 'RIGHT' )
            // CongerCQL.g:290:12: 'RIGHT'
            {
            match("RIGHT"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_RIGHT"

    // $ANTLR start "KW_FULL"
    public final void mKW_FULL() throws RecognitionException {
        try {
            int _type = KW_FULL;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:291:9: ( 'FULL' )
            // CongerCQL.g:291:11: 'FULL'
            {
            match("FULL"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_FULL"

    // $ANTLR start "KW_ON"
    public final void mKW_ON() throws RecognitionException {
        try {
            int _type = KW_ON;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:292:7: ( 'ON' )
            // CongerCQL.g:292:9: 'ON'
            {
            match("ON"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_ON"

    // $ANTLR start "KW_PARTITION"
    public final void mKW_PARTITION() throws RecognitionException {
        try {
            int _type = KW_PARTITION;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:293:14: ( 'PARTITION' )
            // CongerCQL.g:293:16: 'PARTITION'
            {
            match("PARTITION"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_PARTITION"

    // $ANTLR start "KW_USING"
    public final void mKW_USING() throws RecognitionException {
        try {
            int _type = KW_USING;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:294:9: ( 'USING' )
            // CongerCQL.g:294:11: 'USING'
            {
            match("USING"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_USING"

    // $ANTLR start "KW_UNION"
    public final void mKW_UNION() throws RecognitionException {
        try {
            int _type = KW_UNION;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:295:9: ( 'UNION' )
            // CongerCQL.g:295:11: 'UNION'
            {
            match("UNION"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_UNION"

    // $ANTLR start "KW_IS"
    public final void mKW_IS() throws RecognitionException {
        try {
            int _type = KW_IS;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:296:6: ( 'IS' )
            // CongerCQL.g:296:8: 'IS'
            {
            match("IS"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_IS"

    // $ANTLR start "KW_NULL"
    public final void mKW_NULL() throws RecognitionException {
        try {
            int _type = KW_NULL;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:297:8: ( 'NULL' )
            // CongerCQL.g:297:10: 'NULL'
            {
            match("NULL"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_NULL"

    // $ANTLR start "KW_CREATE"
    public final void mKW_CREATE() throws RecognitionException {
        try {
            int _type = KW_CREATE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:298:10: ( 'CREATE' )
            // CongerCQL.g:298:12: 'CREATE'
            {
            match("CREATE"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_CREATE"

    // $ANTLR start "KW_COLUMN"
    public final void mKW_COLUMN() throws RecognitionException {
        try {
            int _type = KW_COLUMN;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:299:10: ( 'COLUMN' )
            // CongerCQL.g:299:12: 'COLUMN'
            {
            match("COLUMN"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_COLUMN"

    // $ANTLR start "KW_FIRST"
    public final void mKW_FIRST() throws RecognitionException {
        try {
            int _type = KW_FIRST;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:300:9: ( 'FIRST' )
            // CongerCQL.g:300:11: 'FIRST'
            {
            match("FIRST"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_FIRST"

    // $ANTLR start "KW_AFTER"
    public final void mKW_AFTER() throws RecognitionException {
        try {
            int _type = KW_AFTER;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:301:9: ( 'AFTER' )
            // CongerCQL.g:301:11: 'AFTER'
            {
            match("AFTER"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_AFTER"

    // $ANTLR start "KW_DESCRIBE"
    public final void mKW_DESCRIBE() throws RecognitionException {
        try {
            int _type = KW_DESCRIBE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:302:12: ( 'DESCRIBE' )
            // CongerCQL.g:302:14: 'DESCRIBE'
            {
            match("DESCRIBE"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_DESCRIBE"

    // $ANTLR start "KW_TO"
    public final void mKW_TO() throws RecognitionException {
        try {
            int _type = KW_TO;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:303:6: ( 'TO' )
            // CongerCQL.g:303:8: 'TO'
            {
            match("TO"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_TO"

    // $ANTLR start "KW_COMMENT"
    public final void mKW_COMMENT() throws RecognitionException {
        try {
            int _type = KW_COMMENT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:304:11: ( 'COMMENT' )
            // CongerCQL.g:304:13: 'COMMENT'
            {
            match("COMMENT"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_COMMENT"

    // $ANTLR start "KW_BOOLEAN"
    public final void mKW_BOOLEAN() throws RecognitionException {
        try {
            int _type = KW_BOOLEAN;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:305:11: ( 'BOOLEAN' )
            // CongerCQL.g:305:13: 'BOOLEAN'
            {
            match("BOOLEAN"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_BOOLEAN"

    // $ANTLR start "KW_INT"
    public final void mKW_INT() throws RecognitionException {
        try {
            int _type = KW_INT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:306:7: ( 'INT' )
            // CongerCQL.g:306:9: 'INT'
            {
            match("INT"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_INT"

    // $ANTLR start "KW_BIGINT"
    public final void mKW_BIGINT() throws RecognitionException {
        try {
            int _type = KW_BIGINT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:307:10: ( 'BIGINT' )
            // CongerCQL.g:307:12: 'BIGINT'
            {
            match("BIGINT"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_BIGINT"

    // $ANTLR start "KW_FLOAT"
    public final void mKW_FLOAT() throws RecognitionException {
        try {
            int _type = KW_FLOAT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:308:9: ( 'FLOAT' )
            // CongerCQL.g:308:11: 'FLOAT'
            {
            match("FLOAT"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_FLOAT"

    // $ANTLR start "KW_DOUBLE"
    public final void mKW_DOUBLE() throws RecognitionException {
        try {
            int _type = KW_DOUBLE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:309:10: ( 'DOUBLE' )
            // CongerCQL.g:309:12: 'DOUBLE'
            {
            match("DOUBLE"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_DOUBLE"

    // $ANTLR start "KW_DATE"
    public final void mKW_DATE() throws RecognitionException {
        try {
            int _type = KW_DATE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:310:8: ( 'DATE' )
            // CongerCQL.g:310:10: 'DATE'
            {
            match("DATE"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_DATE"

    // $ANTLR start "KW_DATETIME"
    public final void mKW_DATETIME() throws RecognitionException {
        try {
            int _type = KW_DATETIME;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:311:12: ( 'DATETIME' )
            // CongerCQL.g:311:14: 'DATETIME'
            {
            match("DATETIME"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_DATETIME"

    // $ANTLR start "KW_INTO"
    public final void mKW_INTO() throws RecognitionException {
        try {
            int _type = KW_INTO;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:312:8: ( 'INTO' )
            // CongerCQL.g:312:10: 'INTO'
            {
            match("INTO"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_INTO"

    // $ANTLR start "KW_ROW"
    public final void mKW_ROW() throws RecognitionException {
        try {
            int _type = KW_ROW;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:313:7: ( 'ROW' )
            // CongerCQL.g:313:9: 'ROW'
            {
            match("ROW"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_ROW"

    // $ANTLR start "KW_OUT"
    public final void mKW_OUT() throws RecognitionException {
        try {
            int _type = KW_OUT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:314:7: ( 'OUT' )
            // CongerCQL.g:314:9: 'OUT'
            {
            match("OUT"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_OUT"

    // $ANTLR start "KW_OF"
    public final void mKW_OF() throws RecognitionException {
        try {
            int _type = KW_OF;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:315:6: ( 'OF' )
            // CongerCQL.g:315:8: 'OF'
            {
            match("OF"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_OF"

    // $ANTLR start "KW_WITH"
    public final void mKW_WITH() throws RecognitionException {
        try {
            int _type = KW_WITH;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:316:8: ( 'WITH' )
            // CongerCQL.g:316:10: 'WITH'
            {
            match("WITH"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_WITH"

    // $ANTLR start "KW_LIMIT"
    public final void mKW_LIMIT() throws RecognitionException {
        try {
            int _type = KW_LIMIT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:317:9: ( 'LIMIT' )
            // CongerCQL.g:317:11: 'LIMIT'
            {
            match("LIMIT"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_LIMIT"

    // $ANTLR start "KW_SET"
    public final void mKW_SET() throws RecognitionException {
        try {
            int _type = KW_SET;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:318:7: ( 'SET' )
            // CongerCQL.g:318:9: 'SET'
            {
            match("SET"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_SET"

    // $ANTLR start "KW_LONG"
    public final void mKW_LONG() throws RecognitionException {
        try {
            int _type = KW_LONG;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:319:8: ( 'LONG' )
            // CongerCQL.g:319:10: 'LONG'
            {
            match("LONG"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_LONG"

    // $ANTLR start "KW_PLUS"
    public final void mKW_PLUS() throws RecognitionException {
        try {
            int _type = KW_PLUS;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:320:8: ( 'PLUS' )
            // CongerCQL.g:320:10: 'PLUS'
            {
            match("PLUS"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_PLUS"

    // $ANTLR start "KW_MINUS"
    public final void mKW_MINUS() throws RecognitionException {
        try {
            int _type = KW_MINUS;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:321:9: ( 'MINUS' )
            // CongerCQL.g:321:11: 'MINUS'
            {
            match("MINUS"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_MINUS"

    // $ANTLR start "KW_VIEW"
    public final void mKW_VIEW() throws RecognitionException {
        try {
            int _type = KW_VIEW;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:322:8: ( 'VIEW' )
            // CongerCQL.g:322:10: 'VIEW'
            {
            match("VIEW"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_VIEW"

    // $ANTLR start "KW_IN"
    public final void mKW_IN() throws RecognitionException {
        try {
            int _type = KW_IN;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:323:6: ( 'IN' )
            // CongerCQL.g:323:8: 'IN'
            {
            match("IN"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_IN"

    // $ANTLR start "KW_BEFORE"
    public final void mKW_BEFORE() throws RecognitionException {
        try {
            int _type = KW_BEFORE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:324:10: ( 'BEFORE' )
            // CongerCQL.g:324:12: 'BEFORE'
            {
            match("BEFORE"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_BEFORE"

    // $ANTLR start "KW_BETWEEN"
    public final void mKW_BETWEEN() throws RecognitionException {
        try {
            int _type = KW_BETWEEN;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:325:11: ( 'BETWEEN' )
            // CongerCQL.g:325:13: 'BETWEEN'
            {
            match("BETWEEN"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_BETWEEN"

    // $ANTLR start "KW_BOTH"
    public final void mKW_BOTH() throws RecognitionException {
        try {
            int _type = KW_BOTH;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:326:8: ( 'BOTH' )
            // CongerCQL.g:326:10: 'BOTH'
            {
            match("BOTH"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_BOTH"

    // $ANTLR start "KW_BINARY"
    public final void mKW_BINARY() throws RecognitionException {
        try {
            int _type = KW_BINARY;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:327:10: ( 'BINARY' )
            // CongerCQL.g:327:12: 'BINARY'
            {
            match("BINARY"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_BINARY"

    // $ANTLR start "KW_CROSS"
    public final void mKW_CROSS() throws RecognitionException {
        try {
            int _type = KW_CROSS;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:328:9: ( 'CROSS' )
            // CongerCQL.g:328:11: 'CROSS'
            {
            match("CROSS"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_CROSS"

    // $ANTLR start "KW_USE"
    public final void mKW_USE() throws RecognitionException {
        try {
            int _type = KW_USE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:329:7: ( 'USE' )
            // CongerCQL.g:329:9: 'USE'
            {
            match("USE"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_USE"

    // $ANTLR start "KW_UPDATE"
    public final void mKW_UPDATE() throws RecognitionException {
        try {
            int _type = KW_UPDATE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:330:10: ( 'UPDATE' )
            // CongerCQL.g:330:12: 'UPDATE'
            {
            match("UPDATE"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_UPDATE"

    // $ANTLR start "KW_SLIDE"
    public final void mKW_SLIDE() throws RecognitionException {
        try {
            int _type = KW_SLIDE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:332:9: ( 'SLIDE' )
            // CongerCQL.g:332:11: 'SLIDE'
            {
            match("SLIDE"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_SLIDE"

    // $ANTLR start "KW_UNBOUNDED"
    public final void mKW_UNBOUNDED() throws RecognitionException {
        try {
            int _type = KW_UNBOUNDED;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:333:13: ( 'UNBOUNDED' )
            // CongerCQL.g:333:15: 'UNBOUNDED'
            {
            match("UNBOUNDED"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_UNBOUNDED"

    // $ANTLR start "KW_NOW"
    public final void mKW_NOW() throws RecognitionException {
        try {
            int _type = KW_NOW;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:334:7: ( 'NOW' )
            // CongerCQL.g:334:9: 'NOW'
            {
            match("NOW"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_NOW"

    // $ANTLR start "KW_ROWS"
    public final void mKW_ROWS() throws RecognitionException {
        try {
            int _type = KW_ROWS;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:335:9: ( 'ROWS' )
            // CongerCQL.g:335:11: 'ROWS'
            {
            match("ROWS"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_ROWS"

    // $ANTLR start "KW_EXCEPT"
    public final void mKW_EXCEPT() throws RecognitionException {
        try {
            int _type = KW_EXCEPT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:336:10: ( 'EXCEPT' )
            // CongerCQL.g:336:12: 'EXCEPT'
            {
            match("EXCEPT"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_EXCEPT"

    // $ANTLR start "KW_LAST"
    public final void mKW_LAST() throws RecognitionException {
        try {
            int _type = KW_LAST;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:337:9: ( 'LAST' )
            // CongerCQL.g:337:11: 'LAST'
            {
            match("LAST"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_LAST"

    // $ANTLR start "KW_DSTREAM"
    public final void mKW_DSTREAM() throws RecognitionException {
        try {
            int _type = KW_DSTREAM;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:338:11: ( 'DSTREAM' )
            // CongerCQL.g:338:13: 'DSTREAM'
            {
            match("DSTREAM"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_DSTREAM"

    // $ANTLR start "KW_ISTREAM"
    public final void mKW_ISTREAM() throws RecognitionException {
        try {
            int _type = KW_ISTREAM;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:339:11: ( 'ISTREAM' )
            // CongerCQL.g:339:13: 'ISTREAM'
            {
            match("ISTREAM"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_ISTREAM"

    // $ANTLR start "KW_RSTREAM"
    public final void mKW_RSTREAM() throws RecognitionException {
        try {
            int _type = KW_RSTREAM;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:340:11: ( 'RSTREAM' )
            // CongerCQL.g:340:13: 'RSTREAM'
            {
            match("RSTREAM"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_RSTREAM"

    // $ANTLR start "KW_DIFFERENCE"
    public final void mKW_DIFFERENCE() throws RecognitionException {
        try {
            int _type = KW_DIFFERENCE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:341:14: ( 'DIFFERENCE' )
            // CongerCQL.g:341:16: 'DIFFERENCE'
            {
            match("DIFFERENCE"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_DIFFERENCE"

    // $ANTLR start "KW_PASSING"
    public final void mKW_PASSING() throws RecognitionException {
        try {
            int _type = KW_PASSING;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:342:11: ( 'PASSING' )
            // CongerCQL.g:342:13: 'PASSING'
            {
            match("PASSING"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_PASSING"

    // $ANTLR start "KW_VALUE"
    public final void mKW_VALUE() throws RecognitionException {
        try {
            int _type = KW_VALUE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:343:9: ( 'VALUE' )
            // CongerCQL.g:343:11: 'VALUE'
            {
            match("VALUE"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_VALUE"

    // $ANTLR start "KW_DEFAULT"
    public final void mKW_DEFAULT() throws RecognitionException {
        try {
            int _type = KW_DEFAULT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:344:11: ( 'DEFAULT' )
            // CongerCQL.g:344:13: 'DEFAULT'
            {
            match("DEFAULT"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_DEFAULT"

    // $ANTLR start "KW_XOR"
    public final void mKW_XOR() throws RecognitionException {
        try {
            int _type = KW_XOR;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:345:7: ( 'XOR' )
            // CongerCQL.g:345:9: 'XOR'
            {
            match("XOR"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_XOR"

    // $ANTLR start "KW_DAY"
    public final void mKW_DAY() throws RecognitionException {
        try {
            int _type = KW_DAY;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:346:7: ( 'DAY' )
            // CongerCQL.g:346:9: 'DAY'
            {
            match("DAY"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_DAY"

    // $ANTLR start "KW_DAYS"
    public final void mKW_DAYS() throws RecognitionException {
        try {
            int _type = KW_DAYS;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:347:8: ( 'DAYS' )
            // CongerCQL.g:347:10: 'DAYS'
            {
            match("DAYS"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_DAYS"

    // $ANTLR start "KW_SECOND"
    public final void mKW_SECOND() throws RecognitionException {
        try {
            int _type = KW_SECOND;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:348:10: ( 'SECOND' )
            // CongerCQL.g:348:12: 'SECOND'
            {
            match("SECOND"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_SECOND"

    // $ANTLR start "KW_SECONDS"
    public final void mKW_SECONDS() throws RecognitionException {
        try {
            int _type = KW_SECONDS;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:349:11: ( 'SECONDS' )
            // CongerCQL.g:349:13: 'SECONDS'
            {
            match("SECONDS"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_SECONDS"

    // $ANTLR start "KW_HOUR"
    public final void mKW_HOUR() throws RecognitionException {
        try {
            int _type = KW_HOUR;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:350:8: ( 'HOUR' )
            // CongerCQL.g:350:10: 'HOUR'
            {
            match("HOUR"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_HOUR"

    // $ANTLR start "KW_HOURS"
    public final void mKW_HOURS() throws RecognitionException {
        try {
            int _type = KW_HOURS;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:351:9: ( 'HOURS' )
            // CongerCQL.g:351:11: 'HOURS'
            {
            match("HOURS"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_HOURS"

    // $ANTLR start "KW_MINUTE"
    public final void mKW_MINUTE() throws RecognitionException {
        try {
            int _type = KW_MINUTE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:352:10: ( 'MINUTE' )
            // CongerCQL.g:352:12: 'MINUTE'
            {
            match("MINUTE"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_MINUTE"

    // $ANTLR start "KW_MINUTES"
    public final void mKW_MINUTES() throws RecognitionException {
        try {
            int _type = KW_MINUTES;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:353:11: ( 'MINUTES' )
            // CongerCQL.g:353:13: 'MINUTES'
            {
            match("MINUTES"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_MINUTES"

    // $ANTLR start "KW_MILLISECOND"
    public final void mKW_MILLISECOND() throws RecognitionException {
        try {
            int _type = KW_MILLISECOND;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:354:15: ( 'MILLISECOND' )
            // CongerCQL.g:354:17: 'MILLISECOND'
            {
            match("MILLISECOND"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_MILLISECOND"

    // $ANTLR start "KW_MILLISECONDS"
    public final void mKW_MILLISECONDS() throws RecognitionException {
        try {
            int _type = KW_MILLISECONDS;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:355:16: ( 'MILLISECONDS' )
            // CongerCQL.g:355:18: 'MILLISECONDS'
            {
            match("MILLISECONDS"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_MILLISECONDS"

    // $ANTLR start "KW_MICROSECOND"
    public final void mKW_MICROSECOND() throws RecognitionException {
        try {
            int _type = KW_MICROSECOND;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:356:15: ( 'MICROSECOND' )
            // CongerCQL.g:356:17: 'MICROSECOND'
            {
            match("MICROSECOND"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_MICROSECOND"

    // $ANTLR start "KW_MICROSECONDS"
    public final void mKW_MICROSECONDS() throws RecognitionException {
        try {
            int _type = KW_MICROSECONDS;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:357:16: ( 'MICROSECONDS' )
            // CongerCQL.g:357:18: 'MICROSECONDS'
            {
            match("MICROSECONDS"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_MICROSECONDS"

    // $ANTLR start "KW_NANOSECOND"
    public final void mKW_NANOSECOND() throws RecognitionException {
        try {
            int _type = KW_NANOSECOND;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:358:14: ( 'NANOSECOND' )
            // CongerCQL.g:358:16: 'NANOSECOND'
            {
            match("NANOSECOND"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_NANOSECOND"

    // $ANTLR start "KW_NANOSECONDS"
    public final void mKW_NANOSECONDS() throws RecognitionException {
        try {
            int _type = KW_NANOSECONDS;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:359:15: ( 'NANOSECONDS' )
            // CongerCQL.g:359:17: 'NANOSECONDS'
            {
            match("NANOSECONDS"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_NANOSECONDS"

    // $ANTLR start "KW_DECODE"
    public final void mKW_DECODE() throws RecognitionException {
        try {
            int _type = KW_DECODE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:360:10: ( 'DECODE' )
            // CongerCQL.g:360:12: 'DECODE'
            {
            match("DECODE"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_DECODE"

    // $ANTLR start "KW_NULLS"
    public final void mKW_NULLS() throws RecognitionException {
        try {
            int _type = KW_NULLS;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:361:9: ( 'NULLS' )
            // CongerCQL.g:361:11: 'NULLS'
            {
            match("NULLS"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_NULLS"

    // $ANTLR start "KW_INTERVAL"
    public final void mKW_INTERVAL() throws RecognitionException {
        try {
            int _type = KW_INTERVAL;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:362:12: ( 'INTERVAL' )
            // CongerCQL.g:362:14: 'INTERVAL'
            {
            match("INTERVAL"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_INTERVAL"

    // $ANTLR start "KW_ELEMENT_TIME"
    public final void mKW_ELEMENT_TIME() throws RecognitionException {
        try {
            int _type = KW_ELEMENT_TIME;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:363:16: ( 'ELEMENT_TIME' )
            // CongerCQL.g:363:18: 'ELEMENT_TIME'
            {
            match("ELEMENT_TIME"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_ELEMENT_TIME"

    // $ANTLR start "KW_COUNT"
    public final void mKW_COUNT() throws RecognitionException {
        try {
            int _type = KW_COUNT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:364:9: ( 'COUNT' )
            // CongerCQL.g:364:11: 'COUNT'
            {
            match("COUNT"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_COUNT"

    // $ANTLR start "KW_SUM"
    public final void mKW_SUM() throws RecognitionException {
        try {
            int _type = KW_SUM;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:365:7: ( 'SUM' )
            // CongerCQL.g:365:9: 'SUM'
            {
            match("SUM"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_SUM"

    // $ANTLR start "KW_AVG"
    public final void mKW_AVG() throws RecognitionException {
        try {
            int _type = KW_AVG;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:366:7: ( 'AVG' )
            // CongerCQL.g:366:9: 'AVG'
            {
            match("AVG"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_AVG"

    // $ANTLR start "KW_MAX"
    public final void mKW_MAX() throws RecognitionException {
        try {
            int _type = KW_MAX;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:367:7: ( 'MAX' )
            // CongerCQL.g:367:9: 'MAX'
            {
            match("MAX"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_MAX"

    // $ANTLR start "KW_MIN"
    public final void mKW_MIN() throws RecognitionException {
        try {
            int _type = KW_MIN;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:368:7: ( 'MIN' )
            // CongerCQL.g:368:9: 'MIN'
            {
            match("MIN"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_MIN"

    // $ANTLR start "KW_INTERSECT"
    public final void mKW_INTERSECT() throws RecognitionException {
        try {
            int _type = KW_INTERSECT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:369:13: ( 'INTERSECT' )
            // CongerCQL.g:369:15: 'INTERSECT'
            {
            match("INTERSECT"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_INTERSECT"

    // $ANTLR start "KW_RANGE"
    public final void mKW_RANGE() throws RecognitionException {
        try {
            int _type = KW_RANGE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:370:9: ( 'RANGE' )
            // CongerCQL.g:370:11: 'RANGE'
            {
            match("RANGE"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_RANGE"

    // $ANTLR start "KW_TIMEOUT"
    public final void mKW_TIMEOUT() throws RecognitionException {
        try {
            int _type = KW_TIMEOUT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:371:11: ( 'TIMEOUT' )
            // CongerCQL.g:371:13: 'TIMEOUT'
            {
            match("TIMEOUT"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_TIMEOUT"

    // $ANTLR start "KW_OVER"
    public final void mKW_OVER() throws RecognitionException {
        try {
            int _type = KW_OVER;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:372:9: ( 'OVER' )
            // CongerCQL.g:372:11: 'OVER'
            {
            match("OVER"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "KW_OVER"

    // $ANTLR start "DOT"
    public final void mDOT() throws RecognitionException {
        try {
            int _type = DOT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:377:5: ( '.' )
            // CongerCQL.g:377:7: '.'
            {
            match('.'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "DOT"

    // $ANTLR start "COLON"
    public final void mCOLON() throws RecognitionException {
        try {
            int _type = COLON;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:378:7: ( ':' )
            // CongerCQL.g:378:9: ':'
            {
            match(':'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "COLON"

    // $ANTLR start "COMMA"
    public final void mCOMMA() throws RecognitionException {
        try {
            int _type = COMMA;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:379:7: ( ',' )
            // CongerCQL.g:379:9: ','
            {
            match(','); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "COMMA"

    // $ANTLR start "SEMICOLON"
    public final void mSEMICOLON() throws RecognitionException {
        try {
            int _type = SEMICOLON;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:380:11: ( ';' )
            // CongerCQL.g:380:13: ';'
            {
            match(';'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "SEMICOLON"

    // $ANTLR start "LPAREN"
    public final void mLPAREN() throws RecognitionException {
        try {
            int _type = LPAREN;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:382:8: ( '(' )
            // CongerCQL.g:382:10: '('
            {
            match('('); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "LPAREN"

    // $ANTLR start "RPAREN"
    public final void mRPAREN() throws RecognitionException {
        try {
            int _type = RPAREN;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:383:8: ( ')' )
            // CongerCQL.g:383:10: ')'
            {
            match(')'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "RPAREN"

    // $ANTLR start "LSQUARE"
    public final void mLSQUARE() throws RecognitionException {
        try {
            int _type = LSQUARE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:384:9: ( '[' )
            // CongerCQL.g:384:11: '['
            {
            match('['); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "LSQUARE"

    // $ANTLR start "RSQUARE"
    public final void mRSQUARE() throws RecognitionException {
        try {
            int _type = RSQUARE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:385:9: ( ']' )
            // CongerCQL.g:385:11: ']'
            {
            match(']'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "RSQUARE"

    // $ANTLR start "LCURLY"
    public final void mLCURLY() throws RecognitionException {
        try {
            int _type = LCURLY;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:386:8: ( '{' )
            // CongerCQL.g:386:10: '{'
            {
            match('{'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "LCURLY"

    // $ANTLR start "RCURLY"
    public final void mRCURLY() throws RecognitionException {
        try {
            int _type = RCURLY;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:387:8: ( '}' )
            // CongerCQL.g:387:10: '}'
            {
            match('}'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "RCURLY"

    // $ANTLR start "EQUAL"
    public final void mEQUAL() throws RecognitionException {
        try {
            int _type = EQUAL;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:389:7: ( '=' | '==' )
            int alt2=2;
            int LA2_0 = input.LA(1);

            if ( (LA2_0=='=') ) {
                int LA2_1 = input.LA(2);

                if ( (LA2_1=='=') ) {
                    alt2=2;
                }
                else {
                    alt2=1;
                }
            }
            else {
                NoViableAltException nvae =
                    new NoViableAltException("", 2, 0, input);

                throw nvae;

            }
            switch (alt2) {
                case 1 :
                    // CongerCQL.g:389:9: '='
                    {
                    match('='); 

                    }
                    break;
                case 2 :
                    // CongerCQL.g:389:15: '=='
                    {
                    match("=="); 



                    }
                    break;

            }
            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "EQUAL"

    // $ANTLR start "NOTEQUAL"
    public final void mNOTEQUAL() throws RecognitionException {
        try {
            int _type = NOTEQUAL;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:390:10: ( '<>' | '!=' )
            int alt3=2;
            int LA3_0 = input.LA(1);

            if ( (LA3_0=='<') ) {
                alt3=1;
            }
            else if ( (LA3_0=='!') ) {
                alt3=2;
            }
            else {
                NoViableAltException nvae =
                    new NoViableAltException("", 3, 0, input);

                throw nvae;

            }
            switch (alt3) {
                case 1 :
                    // CongerCQL.g:390:12: '<>'
                    {
                    match("<>"); 



                    }
                    break;
                case 2 :
                    // CongerCQL.g:390:19: '!='
                    {
                    match("!="); 



                    }
                    break;

            }
            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "NOTEQUAL"

    // $ANTLR start "LESSTHANOREQUALTO"
    public final void mLESSTHANOREQUALTO() throws RecognitionException {
        try {
            int _type = LESSTHANOREQUALTO;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:391:19: ( '<=' )
            // CongerCQL.g:391:21: '<='
            {
            match("<="); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "LESSTHANOREQUALTO"

    // $ANTLR start "LESSTHAN"
    public final void mLESSTHAN() throws RecognitionException {
        try {
            int _type = LESSTHAN;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:392:10: ( '<' )
            // CongerCQL.g:392:12: '<'
            {
            match('<'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "LESSTHAN"

    // $ANTLR start "GREATERTHANOREQUALTO"
    public final void mGREATERTHANOREQUALTO() throws RecognitionException {
        try {
            int _type = GREATERTHANOREQUALTO;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:393:22: ( '>=' )
            // CongerCQL.g:393:24: '>='
            {
            match(">="); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "GREATERTHANOREQUALTO"

    // $ANTLR start "GREATERTHAN"
    public final void mGREATERTHAN() throws RecognitionException {
        try {
            int _type = GREATERTHAN;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:394:13: ( '>' )
            // CongerCQL.g:394:15: '>'
            {
            match('>'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "GREATERTHAN"

    // $ANTLR start "DIVIDE"
    public final void mDIVIDE() throws RecognitionException {
        try {
            int _type = DIVIDE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:396:8: ( '/' )
            // CongerCQL.g:396:10: '/'
            {
            match('/'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "DIVIDE"

    // $ANTLR start "PLUS"
    public final void mPLUS() throws RecognitionException {
        try {
            int _type = PLUS;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:397:6: ( '+' )
            // CongerCQL.g:397:8: '+'
            {
            match('+'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "PLUS"

    // $ANTLR start "MINUS"
    public final void mMINUS() throws RecognitionException {
        try {
            int _type = MINUS;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:398:7: ( '-' )
            // CongerCQL.g:398:9: '-'
            {
            match('-'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "MINUS"

    // $ANTLR start "STAR"
    public final void mSTAR() throws RecognitionException {
        try {
            int _type = STAR;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:399:6: ( '*' )
            // CongerCQL.g:399:8: '*'
            {
            match('*'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "STAR"

    // $ANTLR start "MOD"
    public final void mMOD() throws RecognitionException {
        try {
            int _type = MOD;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:400:5: ( '%' )
            // CongerCQL.g:400:7: '%'
            {
            match('%'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "MOD"

    // $ANTLR start "DIV"
    public final void mDIV() throws RecognitionException {
        try {
            int _type = DIV;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:401:5: ( 'DIV' )
            // CongerCQL.g:401:7: 'DIV'
            {
            match("DIV"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "DIV"

    // $ANTLR start "AMPERSAND"
    public final void mAMPERSAND() throws RecognitionException {
        try {
            int _type = AMPERSAND;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:403:11: ( '&' )
            // CongerCQL.g:403:13: '&'
            {
            match('&'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "AMPERSAND"

    // $ANTLR start "TILDE"
    public final void mTILDE() throws RecognitionException {
        try {
            int _type = TILDE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:404:7: ( '~' )
            // CongerCQL.g:404:9: '~'
            {
            match('~'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "TILDE"

    // $ANTLR start "BITWISEOR"
    public final void mBITWISEOR() throws RecognitionException {
        try {
            int _type = BITWISEOR;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:405:11: ( '|' )
            // CongerCQL.g:405:13: '|'
            {
            match('|'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "BITWISEOR"

    // $ANTLR start "BITWISEXOR"
    public final void mBITWISEXOR() throws RecognitionException {
        try {
            int _type = BITWISEXOR;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:406:12: ( '^' )
            // CongerCQL.g:406:14: '^'
            {
            match('^'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "BITWISEXOR"

    // $ANTLR start "QUESTION"
    public final void mQUESTION() throws RecognitionException {
        try {
            int _type = QUESTION;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:407:10: ( '?' )
            // CongerCQL.g:407:12: '?'
            {
            match('?'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "QUESTION"

    // $ANTLR start "DOLLAR"
    public final void mDOLLAR() throws RecognitionException {
        try {
            int _type = DOLLAR;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:408:8: ( '$' )
            // CongerCQL.g:408:10: '$'
            {
            match('$'); 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "DOLLAR"

    // $ANTLR start "CONCATENATE"
    public final void mCONCATENATE() throws RecognitionException {
        try {
            int _type = CONCATENATE;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:410:12: ( '||' )
            // CongerCQL.g:410:14: '||'
            {
            match("||"); 



            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "CONCATENATE"

    // $ANTLR start "Letter"
    public final void mLetter() throws RecognitionException {
        try {
            // CongerCQL.g:416:5: ( 'a' .. 'z' | 'A' .. 'Z' )
            // CongerCQL.g:
            {
            if ( (input.LA(1) >= 'A' && input.LA(1) <= 'Z')||(input.LA(1) >= 'a' && input.LA(1) <= 'z') ) {
                input.consume();
            }
            else {
                MismatchedSetException mse = new MismatchedSetException(null,input);
                recover(mse);
                throw mse;
            }


            }


        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "Letter"

    // $ANTLR start "HexDigit"
    public final void mHexDigit() throws RecognitionException {
        try {
            // CongerCQL.g:421:5: ( 'a' .. 'f' | 'A' .. 'F' )
            // CongerCQL.g:
            {
            if ( (input.LA(1) >= 'A' && input.LA(1) <= 'F')||(input.LA(1) >= 'a' && input.LA(1) <= 'f') ) {
                input.consume();
            }
            else {
                MismatchedSetException mse = new MismatchedSetException(null,input);
                recover(mse);
                throw mse;
            }


            }


        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "HexDigit"

    // $ANTLR start "Digit"
    public final void mDigit() throws RecognitionException {
        try {
            // CongerCQL.g:426:5: ( '0' .. '9' )
            // CongerCQL.g:
            {
            if ( (input.LA(1) >= '0' && input.LA(1) <= '9') ) {
                input.consume();
            }
            else {
                MismatchedSetException mse = new MismatchedSetException(null,input);
                recover(mse);
                throw mse;
            }


            }


        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "Digit"

    // $ANTLR start "Exponent"
    public final void mExponent() throws RecognitionException {
        try {
            // CongerCQL.g:432:5: ( 'e' ( PLUS | MINUS )? ( Digit )+ )
            // CongerCQL.g:433:5: 'e' ( PLUS | MINUS )? ( Digit )+
            {
            match('e'); 

            // CongerCQL.g:433:9: ( PLUS | MINUS )?
            int alt4=2;
            int LA4_0 = input.LA(1);

            if ( (LA4_0=='+'||LA4_0=='-') ) {
                alt4=1;
            }
            switch (alt4) {
                case 1 :
                    // CongerCQL.g:
                    {
                    if ( input.LA(1)=='+'||input.LA(1)=='-' ) {
                        input.consume();
                    }
                    else {
                        MismatchedSetException mse = new MismatchedSetException(null,input);
                        recover(mse);
                        throw mse;
                    }


                    }
                    break;

            }


            // CongerCQL.g:433:25: ( Digit )+
            int cnt5=0;
            loop5:
            do {
                int alt5=2;
                int LA5_0 = input.LA(1);

                if ( ((LA5_0 >= '0' && LA5_0 <= '9')) ) {
                    alt5=1;
                }


                switch (alt5) {
            	case 1 :
            	    // CongerCQL.g:
            	    {
            	    if ( (input.LA(1) >= '0' && input.LA(1) <= '9') ) {
            	        input.consume();
            	    }
            	    else {
            	        MismatchedSetException mse = new MismatchedSetException(null,input);
            	        recover(mse);
            	        throw mse;
            	    }


            	    }
            	    break;

            	default :
            	    if ( cnt5 >= 1 ) break loop5;
                        EarlyExitException eee =
                            new EarlyExitException(5, input);
                        throw eee;
                }
                cnt5++;
            } while (true);


            }


        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "Exponent"

    // $ANTLR start "RegexComponent"
    public final void mRegexComponent() throws RecognitionException {
        try {
            // CongerCQL.g:438:5: ( 'a' .. 'z' | 'A' .. 'Z' | '0' .. '9' | '_' | PLUS | STAR | QUESTION | MINUS | DOT | LPAREN | RPAREN | LSQUARE | RSQUARE | LCURLY | RCURLY | BITWISEXOR | BITWISEOR | DOLLAR )
            // CongerCQL.g:
            {
            if ( input.LA(1)=='$'||(input.LA(1) >= '(' && input.LA(1) <= '+')||(input.LA(1) >= '-' && input.LA(1) <= '.')||(input.LA(1) >= '0' && input.LA(1) <= '9')||input.LA(1)=='?'||(input.LA(1) >= 'A' && input.LA(1) <= '[')||(input.LA(1) >= ']' && input.LA(1) <= '_')||(input.LA(1) >= 'a' && input.LA(1) <= '}') ) {
                input.consume();
            }
            else {
                MismatchedSetException mse = new MismatchedSetException(null,input);
                recover(mse);
                throw mse;
            }


            }


        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "RegexComponent"

    // $ANTLR start "StringLiteral"
    public final void mStringLiteral() throws RecognitionException {
        try {
            int _type = StringLiteral;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:444:5: ( ( '\\'' (~ ( '\\'' | '\\\\' ) | ( '\\\\' . ) )* '\\'' | '\\\"' (~ ( '\\\"' | '\\\\' ) | ( '\\\\' . ) )* '\\\"' )+ )
            // CongerCQL.g:445:5: ( '\\'' (~ ( '\\'' | '\\\\' ) | ( '\\\\' . ) )* '\\'' | '\\\"' (~ ( '\\\"' | '\\\\' ) | ( '\\\\' . ) )* '\\\"' )+
            {
            // CongerCQL.g:445:5: ( '\\'' (~ ( '\\'' | '\\\\' ) | ( '\\\\' . ) )* '\\'' | '\\\"' (~ ( '\\\"' | '\\\\' ) | ( '\\\\' . ) )* '\\\"' )+
            int cnt8=0;
            loop8:
            do {
                int alt8=3;
                int LA8_0 = input.LA(1);

                if ( (LA8_0=='\'') ) {
                    alt8=1;
                }
                else if ( (LA8_0=='\"') ) {
                    alt8=2;
                }


                switch (alt8) {
            	case 1 :
            	    // CongerCQL.g:445:7: '\\'' (~ ( '\\'' | '\\\\' ) | ( '\\\\' . ) )* '\\''
            	    {
            	    match('\''); 

            	    // CongerCQL.g:445:12: (~ ( '\\'' | '\\\\' ) | ( '\\\\' . ) )*
            	    loop6:
            	    do {
            	        int alt6=3;
            	        int LA6_0 = input.LA(1);

            	        if ( ((LA6_0 >= '\u0000' && LA6_0 <= '&')||(LA6_0 >= '(' && LA6_0 <= '[')||(LA6_0 >= ']' && LA6_0 <= '\uFFFF')) ) {
            	            alt6=1;
            	        }
            	        else if ( (LA6_0=='\\') ) {
            	            alt6=2;
            	        }


            	        switch (alt6) {
            	    	case 1 :
            	    	    // CongerCQL.g:445:14: ~ ( '\\'' | '\\\\' )
            	    	    {
            	    	    if ( (input.LA(1) >= '\u0000' && input.LA(1) <= '&')||(input.LA(1) >= '(' && input.LA(1) <= '[')||(input.LA(1) >= ']' && input.LA(1) <= '\uFFFF') ) {
            	    	        input.consume();
            	    	    }
            	    	    else {
            	    	        MismatchedSetException mse = new MismatchedSetException(null,input);
            	    	        recover(mse);
            	    	        throw mse;
            	    	    }


            	    	    }
            	    	    break;
            	    	case 2 :
            	    	    // CongerCQL.g:445:29: ( '\\\\' . )
            	    	    {
            	    	    // CongerCQL.g:445:29: ( '\\\\' . )
            	    	    // CongerCQL.g:445:30: '\\\\' .
            	    	    {
            	    	    match('\\'); 

            	    	    matchAny(); 

            	    	    }


            	    	    }
            	    	    break;

            	    	default :
            	    	    break loop6;
            	        }
            	    } while (true);


            	    match('\''); 

            	    }
            	    break;
            	case 2 :
            	    // CongerCQL.g:446:7: '\\\"' (~ ( '\\\"' | '\\\\' ) | ( '\\\\' . ) )* '\\\"'
            	    {
            	    match('\"'); 

            	    // CongerCQL.g:446:12: (~ ( '\\\"' | '\\\\' ) | ( '\\\\' . ) )*
            	    loop7:
            	    do {
            	        int alt7=3;
            	        int LA7_0 = input.LA(1);

            	        if ( ((LA7_0 >= '\u0000' && LA7_0 <= '!')||(LA7_0 >= '#' && LA7_0 <= '[')||(LA7_0 >= ']' && LA7_0 <= '\uFFFF')) ) {
            	            alt7=1;
            	        }
            	        else if ( (LA7_0=='\\') ) {
            	            alt7=2;
            	        }


            	        switch (alt7) {
            	    	case 1 :
            	    	    // CongerCQL.g:446:14: ~ ( '\\\"' | '\\\\' )
            	    	    {
            	    	    if ( (input.LA(1) >= '\u0000' && input.LA(1) <= '!')||(input.LA(1) >= '#' && input.LA(1) <= '[')||(input.LA(1) >= ']' && input.LA(1) <= '\uFFFF') ) {
            	    	        input.consume();
            	    	    }
            	    	    else {
            	    	        MismatchedSetException mse = new MismatchedSetException(null,input);
            	    	        recover(mse);
            	    	        throw mse;
            	    	    }


            	    	    }
            	    	    break;
            	    	case 2 :
            	    	    // CongerCQL.g:446:29: ( '\\\\' . )
            	    	    {
            	    	    // CongerCQL.g:446:29: ( '\\\\' . )
            	    	    // CongerCQL.g:446:30: '\\\\' .
            	    	    {
            	    	    match('\\'); 

            	    	    matchAny(); 

            	    	    }


            	    	    }
            	    	    break;

            	    	default :
            	    	    break loop7;
            	        }
            	    } while (true);


            	    match('\"'); 

            	    }
            	    break;

            	default :
            	    if ( cnt8 >= 1 ) break loop8;
                        EarlyExitException eee =
                            new EarlyExitException(8, input);
                        throw eee;
                }
                cnt8++;
            } while (true);


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "StringLiteral"

    // $ANTLR start "CharSetLiteral"
    public final void mCharSetLiteral() throws RecognitionException {
        try {
            int _type = CharSetLiteral;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:451:5: ( StringLiteral | '0' 'X' ( HexDigit | Digit )+ )
            int alt10=2;
            int LA10_0 = input.LA(1);

            if ( (LA10_0=='\"'||LA10_0=='\'') ) {
                alt10=1;
            }
            else if ( (LA10_0=='0') ) {
                alt10=2;
            }
            else {
                NoViableAltException nvae =
                    new NoViableAltException("", 10, 0, input);

                throw nvae;

            }
            switch (alt10) {
                case 1 :
                    // CongerCQL.g:452:5: StringLiteral
                    {
                    mStringLiteral(); 


                    }
                    break;
                case 2 :
                    // CongerCQL.g:453:7: '0' 'X' ( HexDigit | Digit )+
                    {
                    match('0'); 

                    match('X'); 

                    // CongerCQL.g:453:15: ( HexDigit | Digit )+
                    int cnt9=0;
                    loop9:
                    do {
                        int alt9=2;
                        int LA9_0 = input.LA(1);

                        if ( ((LA9_0 >= '0' && LA9_0 <= '9')||(LA9_0 >= 'A' && LA9_0 <= 'F')||(LA9_0 >= 'a' && LA9_0 <= 'f')) ) {
                            alt9=1;
                        }


                        switch (alt9) {
                    	case 1 :
                    	    // CongerCQL.g:
                    	    {
                    	    if ( (input.LA(1) >= '0' && input.LA(1) <= '9')||(input.LA(1) >= 'A' && input.LA(1) <= 'F')||(input.LA(1) >= 'a' && input.LA(1) <= 'f') ) {
                    	        input.consume();
                    	    }
                    	    else {
                    	        MismatchedSetException mse = new MismatchedSetException(null,input);
                    	        recover(mse);
                    	        throw mse;
                    	    }


                    	    }
                    	    break;

                    	default :
                    	    if ( cnt9 >= 1 ) break loop9;
                                EarlyExitException eee =
                                    new EarlyExitException(9, input);
                                throw eee;
                        }
                        cnt9++;
                    } while (true);


                    }
                    break;

            }
            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "CharSetLiteral"

    // $ANTLR start "Integer"
    public final void mInteger() throws RecognitionException {
        try {
            int _type = Integer;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:457:2: ( ( Digit )+ )
            // CongerCQL.g:458:2: ( Digit )+
            {
            // CongerCQL.g:458:2: ( Digit )+
            int cnt11=0;
            loop11:
            do {
                int alt11=2;
                int LA11_0 = input.LA(1);

                if ( ((LA11_0 >= '0' && LA11_0 <= '9')) ) {
                    alt11=1;
                }


                switch (alt11) {
            	case 1 :
            	    // CongerCQL.g:
            	    {
            	    if ( (input.LA(1) >= '0' && input.LA(1) <= '9') ) {
            	        input.consume();
            	    }
            	    else {
            	        MismatchedSetException mse = new MismatchedSetException(null,input);
            	        recover(mse);
            	        throw mse;
            	    }


            	    }
            	    break;

            	default :
            	    if ( cnt11 >= 1 ) break loop11;
                        EarlyExitException eee =
                            new EarlyExitException(11, input);
                        throw eee;
                }
                cnt11++;
            } while (true);


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "Integer"

    // $ANTLR start "Number"
    public final void mNumber() throws RecognitionException {
        try {
            int _type = Number;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:462:5: ( ( Digit )+ ( DOT ( Digit )* ( Exponent )? | Exponent )? )
            // CongerCQL.g:463:5: ( Digit )+ ( DOT ( Digit )* ( Exponent )? | Exponent )?
            {
            // CongerCQL.g:463:5: ( Digit )+
            int cnt12=0;
            loop12:
            do {
                int alt12=2;
                int LA12_0 = input.LA(1);

                if ( ((LA12_0 >= '0' && LA12_0 <= '9')) ) {
                    alt12=1;
                }


                switch (alt12) {
            	case 1 :
            	    // CongerCQL.g:
            	    {
            	    if ( (input.LA(1) >= '0' && input.LA(1) <= '9') ) {
            	        input.consume();
            	    }
            	    else {
            	        MismatchedSetException mse = new MismatchedSetException(null,input);
            	        recover(mse);
            	        throw mse;
            	    }


            	    }
            	    break;

            	default :
            	    if ( cnt12 >= 1 ) break loop12;
                        EarlyExitException eee =
                            new EarlyExitException(12, input);
                        throw eee;
                }
                cnt12++;
            } while (true);


            // CongerCQL.g:463:14: ( DOT ( Digit )* ( Exponent )? | Exponent )?
            int alt15=3;
            int LA15_0 = input.LA(1);

            if ( (LA15_0=='.') ) {
                alt15=1;
            }
            else if ( (LA15_0=='e') ) {
                alt15=2;
            }
            switch (alt15) {
                case 1 :
                    // CongerCQL.g:463:16: DOT ( Digit )* ( Exponent )?
                    {
                    mDOT(); 


                    // CongerCQL.g:463:20: ( Digit )*
                    loop13:
                    do {
                        int alt13=2;
                        int LA13_0 = input.LA(1);

                        if ( ((LA13_0 >= '0' && LA13_0 <= '9')) ) {
                            alt13=1;
                        }


                        switch (alt13) {
                    	case 1 :
                    	    // CongerCQL.g:
                    	    {
                    	    if ( (input.LA(1) >= '0' && input.LA(1) <= '9') ) {
                    	        input.consume();
                    	    }
                    	    else {
                    	        MismatchedSetException mse = new MismatchedSetException(null,input);
                    	        recover(mse);
                    	        throw mse;
                    	    }


                    	    }
                    	    break;

                    	default :
                    	    break loop13;
                        }
                    } while (true);


                    // CongerCQL.g:463:29: ( Exponent )?
                    int alt14=2;
                    int LA14_0 = input.LA(1);

                    if ( (LA14_0=='e') ) {
                        alt14=1;
                    }
                    switch (alt14) {
                        case 1 :
                            // CongerCQL.g:463:30: Exponent
                            {
                            mExponent(); 


                            }
                            break;

                    }


                    }
                    break;
                case 2 :
                    // CongerCQL.g:463:43: Exponent
                    {
                    mExponent(); 


                    }
                    break;

            }


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "Number"

    // $ANTLR start "Identifier"
    public final void mIdentifier() throws RecognitionException {
        try {
            int _type = Identifier;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:467:5: ( ( Letter | Digit ) ( Letter | Digit | '_' )* | '`' ( RegexComponent )+ '`' )
            int alt18=2;
            int LA18_0 = input.LA(1);

            if ( ((LA18_0 >= '0' && LA18_0 <= '9')||(LA18_0 >= 'A' && LA18_0 <= 'Z')||(LA18_0 >= 'a' && LA18_0 <= 'z')) ) {
                alt18=1;
            }
            else if ( (LA18_0=='`') ) {
                alt18=2;
            }
            else {
                NoViableAltException nvae =
                    new NoViableAltException("", 18, 0, input);

                throw nvae;

            }
            switch (alt18) {
                case 1 :
                    // CongerCQL.g:468:5: ( Letter | Digit ) ( Letter | Digit | '_' )*
                    {
                    if ( (input.LA(1) >= '0' && input.LA(1) <= '9')||(input.LA(1) >= 'A' && input.LA(1) <= 'Z')||(input.LA(1) >= 'a' && input.LA(1) <= 'z') ) {
                        input.consume();
                    }
                    else {
                        MismatchedSetException mse = new MismatchedSetException(null,input);
                        recover(mse);
                        throw mse;
                    }


                    // CongerCQL.g:468:22: ( Letter | Digit | '_' )*
                    loop16:
                    do {
                        int alt16=2;
                        int LA16_0 = input.LA(1);

                        if ( ((LA16_0 >= '0' && LA16_0 <= '9')||(LA16_0 >= 'A' && LA16_0 <= 'Z')||LA16_0=='_'||(LA16_0 >= 'a' && LA16_0 <= 'z')) ) {
                            alt16=1;
                        }


                        switch (alt16) {
                    	case 1 :
                    	    // CongerCQL.g:
                    	    {
                    	    if ( (input.LA(1) >= '0' && input.LA(1) <= '9')||(input.LA(1) >= 'A' && input.LA(1) <= 'Z')||input.LA(1)=='_'||(input.LA(1) >= 'a' && input.LA(1) <= 'z') ) {
                    	        input.consume();
                    	    }
                    	    else {
                    	        MismatchedSetException mse = new MismatchedSetException(null,input);
                    	        recover(mse);
                    	        throw mse;
                    	    }


                    	    }
                    	    break;

                    	default :
                    	    break loop16;
                        }
                    } while (true);


                    }
                    break;
                case 2 :
                    // CongerCQL.g:469:7: '`' ( RegexComponent )+ '`'
                    {
                    match('`'); 

                    // CongerCQL.g:469:11: ( RegexComponent )+
                    int cnt17=0;
                    loop17:
                    do {
                        int alt17=2;
                        int LA17_0 = input.LA(1);

                        if ( (LA17_0=='$'||(LA17_0 >= '(' && LA17_0 <= '+')||(LA17_0 >= '-' && LA17_0 <= '.')||(LA17_0 >= '0' && LA17_0 <= '9')||LA17_0=='?'||(LA17_0 >= 'A' && LA17_0 <= '[')||(LA17_0 >= ']' && LA17_0 <= '_')||(LA17_0 >= 'a' && LA17_0 <= '}')) ) {
                            alt17=1;
                        }


                        switch (alt17) {
                    	case 1 :
                    	    // CongerCQL.g:
                    	    {
                    	    if ( input.LA(1)=='$'||(input.LA(1) >= '(' && input.LA(1) <= '+')||(input.LA(1) >= '-' && input.LA(1) <= '.')||(input.LA(1) >= '0' && input.LA(1) <= '9')||input.LA(1)=='?'||(input.LA(1) >= 'A' && input.LA(1) <= '[')||(input.LA(1) >= ']' && input.LA(1) <= '_')||(input.LA(1) >= 'a' && input.LA(1) <= '}') ) {
                    	        input.consume();
                    	    }
                    	    else {
                    	        MismatchedSetException mse = new MismatchedSetException(null,input);
                    	        recover(mse);
                    	        throw mse;
                    	    }


                    	    }
                    	    break;

                    	default :
                    	    if ( cnt17 >= 1 ) break loop17;
                                EarlyExitException eee =
                                    new EarlyExitException(17, input);
                                throw eee;
                        }
                        cnt17++;
                    } while (true);


                    match('`'); 

                    }
                    break;

            }
            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "Identifier"

    // $ANTLR start "CharSetName"
    public final void mCharSetName() throws RecognitionException {
        try {
            int _type = CharSetName;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:473:5: ( '_' ( Letter | Digit | '_' | '-' | '.' | ':' )+ )
            // CongerCQL.g:474:5: '_' ( Letter | Digit | '_' | '-' | '.' | ':' )+
            {
            match('_'); 

            // CongerCQL.g:474:9: ( Letter | Digit | '_' | '-' | '.' | ':' )+
            int cnt19=0;
            loop19:
            do {
                int alt19=2;
                int LA19_0 = input.LA(1);

                if ( ((LA19_0 >= '-' && LA19_0 <= '.')||(LA19_0 >= '0' && LA19_0 <= ':')||(LA19_0 >= 'A' && LA19_0 <= 'Z')||LA19_0=='_'||(LA19_0 >= 'a' && LA19_0 <= 'z')) ) {
                    alt19=1;
                }


                switch (alt19) {
            	case 1 :
            	    // CongerCQL.g:
            	    {
            	    if ( (input.LA(1) >= '-' && input.LA(1) <= '.')||(input.LA(1) >= '0' && input.LA(1) <= ':')||(input.LA(1) >= 'A' && input.LA(1) <= 'Z')||input.LA(1)=='_'||(input.LA(1) >= 'a' && input.LA(1) <= 'z') ) {
            	        input.consume();
            	    }
            	    else {
            	        MismatchedSetException mse = new MismatchedSetException(null,input);
            	        recover(mse);
            	        throw mse;
            	    }


            	    }
            	    break;

            	default :
            	    if ( cnt19 >= 1 ) break loop19;
                        EarlyExitException eee =
                            new EarlyExitException(19, input);
                        throw eee;
                }
                cnt19++;
            } while (true);


            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "CharSetName"

    // $ANTLR start "WS"
    public final void mWS() throws RecognitionException {
        try {
            int _type = WS;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:477:5: ( ( ' ' | '\\r' | '\\t' | '\\n' ) )
            // CongerCQL.g:477:8: ( ' ' | '\\r' | '\\t' | '\\n' )
            {
            if ( (input.LA(1) >= '\t' && input.LA(1) <= '\n')||input.LA(1)=='\r'||input.LA(1)==' ' ) {
                input.consume();
            }
            else {
                MismatchedSetException mse = new MismatchedSetException(null,input);
                recover(mse);
                throw mse;
            }


            _channel=HIDDEN;

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "WS"

    // $ANTLR start "COMMENT"
    public final void mCOMMENT() throws RecognitionException {
        try {
            int _type = COMMENT;
            int _channel = DEFAULT_TOKEN_CHANNEL;
            // CongerCQL.g:481:3: ( '--' (~ ( '\\n' | '\\r' ) )* )
            // CongerCQL.g:481:5: '--' (~ ( '\\n' | '\\r' ) )*
            {
            match("--"); 



            // CongerCQL.g:481:10: (~ ( '\\n' | '\\r' ) )*
            loop20:
            do {
                int alt20=2;
                int LA20_0 = input.LA(1);

                if ( ((LA20_0 >= '\u0000' && LA20_0 <= '\t')||(LA20_0 >= '\u000B' && LA20_0 <= '\f')||(LA20_0 >= '\u000E' && LA20_0 <= '\uFFFF')) ) {
                    alt20=1;
                }


                switch (alt20) {
            	case 1 :
            	    // CongerCQL.g:
            	    {
            	    if ( (input.LA(1) >= '\u0000' && input.LA(1) <= '\t')||(input.LA(1) >= '\u000B' && input.LA(1) <= '\f')||(input.LA(1) >= '\u000E' && input.LA(1) <= '\uFFFF') ) {
            	        input.consume();
            	    }
            	    else {
            	        MismatchedSetException mse = new MismatchedSetException(null,input);
            	        recover(mse);
            	        throw mse;
            	    }


            	    }
            	    break;

            	default :
            	    break loop20;
                }
            } while (true);


             _channel=HIDDEN; 

            }

            state.type = _type;
            state.channel = _channel;
        }
        finally {
        	// do for sure before leaving
        }
    }
    // $ANTLR end "COMMENT"

    public void mTokens() throws RecognitionException {
        // CongerCQL.g:1:8: ( KW_TRUE | KW_FALSE | KW_ALL | KW_AND | KW_OR | KW_NOT | KW_LIKE | KW_IF | KW_EXISTS | KW_ASC | KW_DESC | KW_ORDER | KW_GROUP | KW_BY | KW_HAVING | KW_WHERE | KW_FROM | KW_AS | KW_SELECT | KW_DISTINCT | KW_OUTER | KW_JOIN | KW_LEFT | KW_RIGHT | KW_FULL | KW_ON | KW_PARTITION | KW_USING | KW_UNION | KW_IS | KW_NULL | KW_CREATE | KW_COLUMN | KW_FIRST | KW_AFTER | KW_DESCRIBE | KW_TO | KW_COMMENT | KW_BOOLEAN | KW_INT | KW_BIGINT | KW_FLOAT | KW_DOUBLE | KW_DATE | KW_DATETIME | KW_INTO | KW_ROW | KW_OUT | KW_OF | KW_WITH | KW_LIMIT | KW_SET | KW_LONG | KW_PLUS | KW_MINUS | KW_VIEW | KW_IN | KW_BEFORE | KW_BETWEEN | KW_BOTH | KW_BINARY | KW_CROSS | KW_USE | KW_UPDATE | KW_SLIDE | KW_UNBOUNDED | KW_NOW | KW_ROWS | KW_EXCEPT | KW_LAST | KW_DSTREAM | KW_ISTREAM | KW_RSTREAM | KW_DIFFERENCE | KW_PASSING | KW_VALUE | KW_DEFAULT | KW_XOR | KW_DAY | KW_DAYS | KW_SECOND | KW_SECONDS | KW_HOUR | KW_HOURS | KW_MINUTE | KW_MINUTES | KW_MILLISECOND | KW_MILLISECONDS | KW_MICROSECOND | KW_MICROSECONDS | KW_NANOSECOND | KW_NANOSECONDS | KW_DECODE | KW_NULLS | KW_INTERVAL | KW_ELEMENT_TIME | KW_COUNT | KW_SUM | KW_AVG | KW_MAX | KW_MIN | KW_INTERSECT | KW_RANGE | KW_TIMEOUT | KW_OVER | DOT | COLON | COMMA | SEMICOLON | LPAREN | RPAREN | LSQUARE | RSQUARE | LCURLY | RCURLY | EQUAL | NOTEQUAL | LESSTHANOREQUALTO | LESSTHAN | GREATERTHANOREQUALTO | GREATERTHAN | DIVIDE | PLUS | MINUS | STAR | MOD | DIV | AMPERSAND | TILDE | BITWISEOR | BITWISEXOR | QUESTION | DOLLAR | CONCATENATE | StringLiteral | CharSetLiteral | Integer | Number | Identifier | CharSetName | WS | COMMENT )
        int alt21=142;
        alt21 = dfa21.predict(input);
        switch (alt21) {
            case 1 :
                // CongerCQL.g:1:10: KW_TRUE
                {
                mKW_TRUE(); 


                }
                break;
            case 2 :
                // CongerCQL.g:1:18: KW_FALSE
                {
                mKW_FALSE(); 


                }
                break;
            case 3 :
                // CongerCQL.g:1:27: KW_ALL
                {
                mKW_ALL(); 


                }
                break;
            case 4 :
                // CongerCQL.g:1:34: KW_AND
                {
                mKW_AND(); 


                }
                break;
            case 5 :
                // CongerCQL.g:1:41: KW_OR
                {
                mKW_OR(); 


                }
                break;
            case 6 :
                // CongerCQL.g:1:47: KW_NOT
                {
                mKW_NOT(); 


                }
                break;
            case 7 :
                // CongerCQL.g:1:54: KW_LIKE
                {
                mKW_LIKE(); 


                }
                break;
            case 8 :
                // CongerCQL.g:1:62: KW_IF
                {
                mKW_IF(); 


                }
                break;
            case 9 :
                // CongerCQL.g:1:68: KW_EXISTS
                {
                mKW_EXISTS(); 


                }
                break;
            case 10 :
                // CongerCQL.g:1:78: KW_ASC
                {
                mKW_ASC(); 


                }
                break;
            case 11 :
                // CongerCQL.g:1:85: KW_DESC
                {
                mKW_DESC(); 


                }
                break;
            case 12 :
                // CongerCQL.g:1:93: KW_ORDER
                {
                mKW_ORDER(); 


                }
                break;
            case 13 :
                // CongerCQL.g:1:102: KW_GROUP
                {
                mKW_GROUP(); 


                }
                break;
            case 14 :
                // CongerCQL.g:1:111: KW_BY
                {
                mKW_BY(); 


                }
                break;
            case 15 :
                // CongerCQL.g:1:117: KW_HAVING
                {
                mKW_HAVING(); 


                }
                break;
            case 16 :
                // CongerCQL.g:1:127: KW_WHERE
                {
                mKW_WHERE(); 


                }
                break;
            case 17 :
                // CongerCQL.g:1:136: KW_FROM
                {
                mKW_FROM(); 


                }
                break;
            case 18 :
                // CongerCQL.g:1:144: KW_AS
                {
                mKW_AS(); 


                }
                break;
            case 19 :
                // CongerCQL.g:1:150: KW_SELECT
                {
                mKW_SELECT(); 


                }
                break;
            case 20 :
                // CongerCQL.g:1:160: KW_DISTINCT
                {
                mKW_DISTINCT(); 


                }
                break;
            case 21 :
                // CongerCQL.g:1:172: KW_OUTER
                {
                mKW_OUTER(); 


                }
                break;
            case 22 :
                // CongerCQL.g:1:181: KW_JOIN
                {
                mKW_JOIN(); 


                }
                break;
            case 23 :
                // CongerCQL.g:1:189: KW_LEFT
                {
                mKW_LEFT(); 


                }
                break;
            case 24 :
                // CongerCQL.g:1:197: KW_RIGHT
                {
                mKW_RIGHT(); 


                }
                break;
            case 25 :
                // CongerCQL.g:1:206: KW_FULL
                {
                mKW_FULL(); 


                }
                break;
            case 26 :
                // CongerCQL.g:1:214: KW_ON
                {
                mKW_ON(); 


                }
                break;
            case 27 :
                // CongerCQL.g:1:220: KW_PARTITION
                {
                mKW_PARTITION(); 


                }
                break;
            case 28 :
                // CongerCQL.g:1:233: KW_USING
                {
                mKW_USING(); 


                }
                break;
            case 29 :
                // CongerCQL.g:1:242: KW_UNION
                {
                mKW_UNION(); 


                }
                break;
            case 30 :
                // CongerCQL.g:1:251: KW_IS
                {
                mKW_IS(); 


                }
                break;
            case 31 :
                // CongerCQL.g:1:257: KW_NULL
                {
                mKW_NULL(); 


                }
                break;
            case 32 :
                // CongerCQL.g:1:265: KW_CREATE
                {
                mKW_CREATE(); 


                }
                break;
            case 33 :
                // CongerCQL.g:1:275: KW_COLUMN
                {
                mKW_COLUMN(); 


                }
                break;
            case 34 :
                // CongerCQL.g:1:285: KW_FIRST
                {
                mKW_FIRST(); 


                }
                break;
            case 35 :
                // CongerCQL.g:1:294: KW_AFTER
                {
                mKW_AFTER(); 


                }
                break;
            case 36 :
                // CongerCQL.g:1:303: KW_DESCRIBE
                {
                mKW_DESCRIBE(); 


                }
                break;
            case 37 :
                // CongerCQL.g:1:315: KW_TO
                {
                mKW_TO(); 


                }
                break;
            case 38 :
                // CongerCQL.g:1:321: KW_COMMENT
                {
                mKW_COMMENT(); 


                }
                break;
            case 39 :
                // CongerCQL.g:1:332: KW_BOOLEAN
                {
                mKW_BOOLEAN(); 


                }
                break;
            case 40 :
                // CongerCQL.g:1:343: KW_INT
                {
                mKW_INT(); 


                }
                break;
            case 41 :
                // CongerCQL.g:1:350: KW_BIGINT
                {
                mKW_BIGINT(); 


                }
                break;
            case 42 :
                // CongerCQL.g:1:360: KW_FLOAT
                {
                mKW_FLOAT(); 


                }
                break;
            case 43 :
                // CongerCQL.g:1:369: KW_DOUBLE
                {
                mKW_DOUBLE(); 


                }
                break;
            case 44 :
                // CongerCQL.g:1:379: KW_DATE
                {
                mKW_DATE(); 


                }
                break;
            case 45 :
                // CongerCQL.g:1:387: KW_DATETIME
                {
                mKW_DATETIME(); 


                }
                break;
            case 46 :
                // CongerCQL.g:1:399: KW_INTO
                {
                mKW_INTO(); 


                }
                break;
            case 47 :
                // CongerCQL.g:1:407: KW_ROW
                {
                mKW_ROW(); 


                }
                break;
            case 48 :
                // CongerCQL.g:1:414: KW_OUT
                {
                mKW_OUT(); 


                }
                break;
            case 49 :
                // CongerCQL.g:1:421: KW_OF
                {
                mKW_OF(); 


                }
                break;
            case 50 :
                // CongerCQL.g:1:427: KW_WITH
                {
                mKW_WITH(); 


                }
                break;
            case 51 :
                // CongerCQL.g:1:435: KW_LIMIT
                {
                mKW_LIMIT(); 


                }
                break;
            case 52 :
                // CongerCQL.g:1:444: KW_SET
                {
                mKW_SET(); 


                }
                break;
            case 53 :
                // CongerCQL.g:1:451: KW_LONG
                {
                mKW_LONG(); 


                }
                break;
            case 54 :
                // CongerCQL.g:1:459: KW_PLUS
                {
                mKW_PLUS(); 


                }
                break;
            case 55 :
                // CongerCQL.g:1:467: KW_MINUS
                {
                mKW_MINUS(); 


                }
                break;
            case 56 :
                // CongerCQL.g:1:476: KW_VIEW
                {
                mKW_VIEW(); 


                }
                break;
            case 57 :
                // CongerCQL.g:1:484: KW_IN
                {
                mKW_IN(); 


                }
                break;
            case 58 :
                // CongerCQL.g:1:490: KW_BEFORE
                {
                mKW_BEFORE(); 


                }
                break;
            case 59 :
                // CongerCQL.g:1:500: KW_BETWEEN
                {
                mKW_BETWEEN(); 


                }
                break;
            case 60 :
                // CongerCQL.g:1:511: KW_BOTH
                {
                mKW_BOTH(); 


                }
                break;
            case 61 :
                // CongerCQL.g:1:519: KW_BINARY
                {
                mKW_BINARY(); 


                }
                break;
            case 62 :
                // CongerCQL.g:1:529: KW_CROSS
                {
                mKW_CROSS(); 


                }
                break;
            case 63 :
                // CongerCQL.g:1:538: KW_USE
                {
                mKW_USE(); 


                }
                break;
            case 64 :
                // CongerCQL.g:1:545: KW_UPDATE
                {
                mKW_UPDATE(); 


                }
                break;
            case 65 :
                // CongerCQL.g:1:555: KW_SLIDE
                {
                mKW_SLIDE(); 


                }
                break;
            case 66 :
                // CongerCQL.g:1:564: KW_UNBOUNDED
                {
                mKW_UNBOUNDED(); 


                }
                break;
            case 67 :
                // CongerCQL.g:1:577: KW_NOW
                {
                mKW_NOW(); 


                }
                break;
            case 68 :
                // CongerCQL.g:1:584: KW_ROWS
                {
                mKW_ROWS(); 


                }
                break;
            case 69 :
                // CongerCQL.g:1:592: KW_EXCEPT
                {
                mKW_EXCEPT(); 


                }
                break;
            case 70 :
                // CongerCQL.g:1:602: KW_LAST
                {
                mKW_LAST(); 


                }
                break;
            case 71 :
                // CongerCQL.g:1:610: KW_DSTREAM
                {
                mKW_DSTREAM(); 


                }
                break;
            case 72 :
                // CongerCQL.g:1:621: KW_ISTREAM
                {
                mKW_ISTREAM(); 


                }
                break;
            case 73 :
                // CongerCQL.g:1:632: KW_RSTREAM
                {
                mKW_RSTREAM(); 


                }
                break;
            case 74 :
                // CongerCQL.g:1:643: KW_DIFFERENCE
                {
                mKW_DIFFERENCE(); 


                }
                break;
            case 75 :
                // CongerCQL.g:1:657: KW_PASSING
                {
                mKW_PASSING(); 


                }
                break;
            case 76 :
                // CongerCQL.g:1:668: KW_VALUE
                {
                mKW_VALUE(); 


                }
                break;
            case 77 :
                // CongerCQL.g:1:677: KW_DEFAULT
                {
                mKW_DEFAULT(); 


                }
                break;
            case 78 :
                // CongerCQL.g:1:688: KW_XOR
                {
                mKW_XOR(); 


                }
                break;
            case 79 :
                // CongerCQL.g:1:695: KW_DAY
                {
                mKW_DAY(); 


                }
                break;
            case 80 :
                // CongerCQL.g:1:702: KW_DAYS
                {
                mKW_DAYS(); 


                }
                break;
            case 81 :
                // CongerCQL.g:1:710: KW_SECOND
                {
                mKW_SECOND(); 


                }
                break;
            case 82 :
                // CongerCQL.g:1:720: KW_SECONDS
                {
                mKW_SECONDS(); 


                }
                break;
            case 83 :
                // CongerCQL.g:1:731: KW_HOUR
                {
                mKW_HOUR(); 


                }
                break;
            case 84 :
                // CongerCQL.g:1:739: KW_HOURS
                {
                mKW_HOURS(); 


                }
                break;
            case 85 :
                // CongerCQL.g:1:748: KW_MINUTE
                {
                mKW_MINUTE(); 


                }
                break;
            case 86 :
                // CongerCQL.g:1:758: KW_MINUTES
                {
                mKW_MINUTES(); 


                }
                break;
            case 87 :
                // CongerCQL.g:1:769: KW_MILLISECOND
                {
                mKW_MILLISECOND(); 


                }
                break;
            case 88 :
                // CongerCQL.g:1:784: KW_MILLISECONDS
                {
                mKW_MILLISECONDS(); 


                }
                break;
            case 89 :
                // CongerCQL.g:1:800: KW_MICROSECOND
                {
                mKW_MICROSECOND(); 


                }
                break;
            case 90 :
                // CongerCQL.g:1:815: KW_MICROSECONDS
                {
                mKW_MICROSECONDS(); 


                }
                break;
            case 91 :
                // CongerCQL.g:1:831: KW_NANOSECOND
                {
                mKW_NANOSECOND(); 


                }
                break;
            case 92 :
                // CongerCQL.g:1:845: KW_NANOSECONDS
                {
                mKW_NANOSECONDS(); 


                }
                break;
            case 93 :
                // CongerCQL.g:1:860: KW_DECODE
                {
                mKW_DECODE(); 


                }
                break;
            case 94 :
                // CongerCQL.g:1:870: KW_NULLS
                {
                mKW_NULLS(); 


                }
                break;
            case 95 :
                // CongerCQL.g:1:879: KW_INTERVAL
                {
                mKW_INTERVAL(); 


                }
                break;
            case 96 :
                // CongerCQL.g:1:891: KW_ELEMENT_TIME
                {
                mKW_ELEMENT_TIME(); 


                }
                break;
            case 97 :
                // CongerCQL.g:1:907: KW_COUNT
                {
                mKW_COUNT(); 


                }
                break;
            case 98 :
                // CongerCQL.g:1:916: KW_SUM
                {
                mKW_SUM(); 


                }
                break;
            case 99 :
                // CongerCQL.g:1:923: KW_AVG
                {
                mKW_AVG(); 


                }
                break;
            case 100 :
                // CongerCQL.g:1:930: KW_MAX
                {
                mKW_MAX(); 


                }
                break;
            case 101 :
                // CongerCQL.g:1:937: KW_MIN
                {
                mKW_MIN(); 


                }
                break;
            case 102 :
                // CongerCQL.g:1:944: KW_INTERSECT
                {
                mKW_INTERSECT(); 


                }
                break;
            case 103 :
                // CongerCQL.g:1:957: KW_RANGE
                {
                mKW_RANGE(); 


                }
                break;
            case 104 :
                // CongerCQL.g:1:966: KW_TIMEOUT
                {
                mKW_TIMEOUT(); 


                }
                break;
            case 105 :
                // CongerCQL.g:1:977: KW_OVER
                {
                mKW_OVER(); 


                }
                break;
            case 106 :
                // CongerCQL.g:1:985: DOT
                {
                mDOT(); 


                }
                break;
            case 107 :
                // CongerCQL.g:1:989: COLON
                {
                mCOLON(); 


                }
                break;
            case 108 :
                // CongerCQL.g:1:995: COMMA
                {
                mCOMMA(); 


                }
                break;
            case 109 :
                // CongerCQL.g:1:1001: SEMICOLON
                {
                mSEMICOLON(); 


                }
                break;
            case 110 :
                // CongerCQL.g:1:1011: LPAREN
                {
                mLPAREN(); 


                }
                break;
            case 111 :
                // CongerCQL.g:1:1018: RPAREN
                {
                mRPAREN(); 


                }
                break;
            case 112 :
                // CongerCQL.g:1:1025: LSQUARE
                {
                mLSQUARE(); 


                }
                break;
            case 113 :
                // CongerCQL.g:1:1033: RSQUARE
                {
                mRSQUARE(); 


                }
                break;
            case 114 :
                // CongerCQL.g:1:1041: LCURLY
                {
                mLCURLY(); 


                }
                break;
            case 115 :
                // CongerCQL.g:1:1048: RCURLY
                {
                mRCURLY(); 


                }
                break;
            case 116 :
                // CongerCQL.g:1:1055: EQUAL
                {
                mEQUAL(); 


                }
                break;
            case 117 :
                // CongerCQL.g:1:1061: NOTEQUAL
                {
                mNOTEQUAL(); 


                }
                break;
            case 118 :
                // CongerCQL.g:1:1070: LESSTHANOREQUALTO
                {
                mLESSTHANOREQUALTO(); 


                }
                break;
            case 119 :
                // CongerCQL.g:1:1088: LESSTHAN
                {
                mLESSTHAN(); 


                }
                break;
            case 120 :
                // CongerCQL.g:1:1097: GREATERTHANOREQUALTO
                {
                mGREATERTHANOREQUALTO(); 


                }
                break;
            case 121 :
                // CongerCQL.g:1:1118: GREATERTHAN
                {
                mGREATERTHAN(); 


                }
                break;
            case 122 :
                // CongerCQL.g:1:1130: DIVIDE
                {
                mDIVIDE(); 


                }
                break;
            case 123 :
                // CongerCQL.g:1:1137: PLUS
                {
                mPLUS(); 


                }
                break;
            case 124 :
                // CongerCQL.g:1:1142: MINUS
                {
                mMINUS(); 


                }
                break;
            case 125 :
                // CongerCQL.g:1:1148: STAR
                {
                mSTAR(); 


                }
                break;
            case 126 :
                // CongerCQL.g:1:1153: MOD
                {
                mMOD(); 


                }
                break;
            case 127 :
                // CongerCQL.g:1:1157: DIV
                {
                mDIV(); 


                }
                break;
            case 128 :
                // CongerCQL.g:1:1161: AMPERSAND
                {
                mAMPERSAND(); 


                }
                break;
            case 129 :
                // CongerCQL.g:1:1171: TILDE
                {
                mTILDE(); 


                }
                break;
            case 130 :
                // CongerCQL.g:1:1177: BITWISEOR
                {
                mBITWISEOR(); 


                }
                break;
            case 131 :
                // CongerCQL.g:1:1187: BITWISEXOR
                {
                mBITWISEXOR(); 


                }
                break;
            case 132 :
                // CongerCQL.g:1:1198: QUESTION
                {
                mQUESTION(); 


                }
                break;
            case 133 :
                // CongerCQL.g:1:1207: DOLLAR
                {
                mDOLLAR(); 


                }
                break;
            case 134 :
                // CongerCQL.g:1:1214: CONCATENATE
                {
                mCONCATENATE(); 


                }
                break;
            case 135 :
                // CongerCQL.g:1:1226: StringLiteral
                {
                mStringLiteral(); 


                }
                break;
            case 136 :
                // CongerCQL.g:1:1240: CharSetLiteral
                {
                mCharSetLiteral(); 


                }
                break;
            case 137 :
                // CongerCQL.g:1:1255: Integer
                {
                mInteger(); 


                }
                break;
            case 138 :
                // CongerCQL.g:1:1263: Number
                {
                mNumber(); 


                }
                break;
            case 139 :
                // CongerCQL.g:1:1270: Identifier
                {
                mIdentifier(); 


                }
                break;
            case 140 :
                // CongerCQL.g:1:1281: CharSetName
                {
                mCharSetName(); 


                }
                break;
            case 141 :
                // CongerCQL.g:1:1293: WS
                {
                mWS(); 


                }
                break;
            case 142 :
                // CongerCQL.g:1:1296: COMMENT
                {
                mCOMMENT(); 


                }
                break;

        }

    }


    protected DFA21 dfa21 = new DFA21(this);
    static final String DFA21_eotS =
        "\1\uffff\5\64\1\115\21\64\13\uffff\1\172\1\174\2\uffff\1\176\4\uffff"+
        "\1\u0080\5\uffff\2\u0088\3\uffff\1\64\1\u008d\10\64\1\u0097\2\64"+
        "\1\u009b\1\64\1\u009d\1\u009e\4\64\2\uffff\4\64\1\u00a9\1\u00ab"+
        "\1\u00ad\10\64\1\u00bc\33\64\12\uffff\1\u00e6\2\uffff\1\u00e6\1"+
        "\64\1\uffff\1\u0088\1\uffff\2\64\1\uffff\6\64\1\u00f1\1\u00f2\1"+
        "\u00f3\1\uffff\1\64\1\u00f5\1\64\1\uffff\1\u00f8\2\uffff\1\64\1"+
        "\115\1\u00fa\7\64\1\uffff\1\64\1\uffff\1\u0105\1\uffff\10\64\1\u010e"+
        "\2\64\1\u0112\2\64\1\uffff\13\64\1\u0120\2\64\1\u0123\2\64\1\u0127"+
        "\6\64\1\u012e\10\64\1\u0138\2\64\1\u013b\2\64\1\u013e\3\uffff\1"+
        "\u013f\1\u008a\1\u0140\2\64\1\u0143\1\u0144\2\64\3\uffff\1\64\1"+
        "\uffff\2\64\1\uffff\1\u014a\1\uffff\1\u014c\1\64\1\u014e\1\64\1"+
        "\u0150\1\u0151\1\u0152\1\64\1\u0154\1\64\1\uffff\3\64\1\u015a\4"+
        "\64\1\uffff\1\64\1\u0161\1\u0162\1\uffff\3\64\1\u0166\5\64\1\u016d"+
        "\1\64\1\u016f\1\64\1\uffff\2\64\1\uffff\1\u0173\1\64\1\u0175\1\uffff"+
        "\4\64\1\u017a\1\64\1\uffff\11\64\1\uffff\2\64\1\uffff\1\u0188\1"+
        "\64\3\uffff\1\64\1\u018b\2\uffff\1\u018c\1\u018d\1\u018e\1\u018f"+
        "\1\u0190\1\uffff\1\u0191\1\uffff\1\64\1\uffff\1\u0193\3\uffff\1"+
        "\64\1\uffff\5\64\1\uffff\6\64\2\uffff\1\64\1\u01a2\1\64\1\uffff"+
        "\5\64\1\u01a9\1\uffff\1\u01aa\1\uffff\2\64\1\u01ad\1\uffff\1\u01ae"+
        "\1\uffff\1\64\1\u01b0\2\64\1\uffff\1\u01b3\1\u01b4\3\64\1\u01b8"+
        "\2\64\1\u01bb\1\u01bc\3\64\1\uffff\1\u01c0\1\64\7\uffff\1\64\1\uffff"+
        "\3\64\1\u01c6\1\u01c7\3\64\1\u01cb\2\64\1\u01ce\2\64\1\uffff\1\64"+
        "\1\u01d2\1\u01d3\1\u01d4\1\64\1\u01d6\2\uffff\1\u01d7\1\u01d9\2"+
        "\uffff\1\64\1\uffff\2\64\2\uffff\1\64\1\u01de\1\u01df\1\uffff\1"+
        "\u01e0\1\64\2\uffff\1\u01e3\2\64\1\uffff\1\u01e6\1\64\1\u01e8\2"+
        "\64\2\uffff\2\64\1\u01ed\1\uffff\2\64\1\uffff\1\64\1\u01f1\1\u01f2"+
        "\3\uffff\1\u01f3\2\uffff\1\u01f4\1\uffff\1\u01f5\1\64\1\u01f7\1"+
        "\64\3\uffff\1\u01f9\1\u01fa\1\uffff\2\64\1\uffff\1\64\1\uffff\1"+
        "\u01fe\2\64\1\u0201\1\uffff\1\u0202\1\64\1\u0204\5\uffff\1\64\1"+
        "\uffff\1\64\2\uffff\3\64\1\uffff\1\u020a\1\64\2\uffff\1\64\1\uffff"+
        "\1\u020d\1\u020e\2\64\1\u0212\1\uffff\1\64\1\u0214\2\uffff\2\64"+
        "\1\u0217\1\uffff\1\64\1\uffff\1\u021a\1\u021c\1\uffff\1\u021d\1"+
        "\u021e\1\uffff\1\u021f\4\uffff";
    static final String DFA21_eofS =
        "\u0220\uffff";
    static final String DFA21_minS =
        "\1\11\1\111\1\101\2\106\1\101\1\75\1\101\1\106\1\114\1\101\1\122"+
        "\1\105\1\101\1\110\1\105\1\117\2\101\1\116\1\117\2\101\1\117\13"+
        "\uffff\2\75\2\uffff\1\55\4\uffff\1\174\3\uffff\2\0\2\56\3\uffff"+
        "\1\125\1\60\1\115\1\114\1\117\1\114\1\122\1\117\1\114\1\104\1\60"+
        "\1\124\1\107\1\60\1\124\2\60\1\105\1\124\1\114\1\116\2\uffff\1\113"+
        "\1\106\1\116\1\123\3\60\1\103\1\105\1\103\1\106\1\125\2\124\1\117"+
        "\1\60\1\117\1\107\1\106\1\126\1\125\1\105\1\124\1\103\1\111\1\115"+
        "\1\111\1\107\1\127\1\124\1\116\1\122\1\125\1\105\1\102\1\104\1\105"+
        "\1\114\1\103\1\130\1\105\1\114\1\122\10\uffff\2\0\1\42\2\0\1\42"+
        "\1\60\1\uffff\1\56\1\uffff\1\53\1\105\1\uffff\1\105\1\123\1\115"+
        "\1\114\1\123\1\101\3\60\1\uffff\1\105\1\60\1\105\1\uffff\1\60\2"+
        "\uffff\1\122\2\60\1\114\1\117\1\105\1\111\1\124\1\107\1\124\1\uffff"+
        "\1\122\1\uffff\1\60\1\uffff\1\123\1\105\1\115\1\103\1\101\1\117"+
        "\1\124\1\106\1\60\1\102\1\105\1\60\1\122\1\125\1\uffff\1\114\1\110"+
        "\1\111\1\101\1\117\1\127\1\111\2\122\1\110\1\105\1\60\1\117\1\104"+
        "\1\60\1\116\1\110\1\60\1\122\1\107\1\124\2\123\1\116\1\60\2\117"+
        "\2\101\1\123\1\125\1\115\1\116\1\60\1\114\1\122\1\60\1\127\1\125"+
        "\1\60\1\0\1\uffff\1\0\3\60\1\117\1\105\2\60\2\124\3\uffff\1\122"+
        "\1\uffff\2\122\1\uffff\1\60\1\uffff\1\60\1\123\1\60\1\124\3\60\1"+
        "\105\1\60\1\122\1\uffff\1\124\1\120\1\105\1\60\1\125\1\104\1\111"+
        "\1\105\1\uffff\1\114\2\60\1\uffff\1\105\1\120\1\105\1\60\1\116\2"+
        "\122\1\105\1\116\1\60\1\105\1\60\1\103\1\uffff\1\116\1\105\1\uffff"+
        "\1\60\1\124\1\60\1\uffff\2\105\2\111\1\60\1\107\1\uffff\1\116\1"+
        "\125\2\124\1\123\1\115\1\105\1\124\1\123\1\uffff\1\111\1\117\1\uffff"+
        "\1\60\1\105\3\uffff\1\125\1\60\2\uffff\5\60\1\uffff\1\60\1\uffff"+
        "\1\105\1\uffff\1\60\3\uffff\1\101\1\uffff\2\123\1\124\1\116\1\111"+
        "\1\uffff\1\114\1\105\1\116\1\122\1\105\1\111\2\uffff\1\101\1\60"+
        "\1\101\1\uffff\1\124\1\131\2\105\1\107\1\60\1\uffff\1\60\1\uffff"+
        "\1\124\1\104\1\60\1\uffff\1\60\1\uffff\1\101\1\60\1\124\1\116\1"+
        "\uffff\2\60\1\116\2\105\1\60\2\116\2\60\1\105\2\123\1\uffff\1\60"+
        "\1\124\7\uffff\1\103\1\uffff\1\115\1\101\1\105\2\60\1\124\1\102"+
        "\1\124\1\60\1\103\1\105\1\60\2\115\1\uffff\1\116\3\60\1\116\1\60"+
        "\2\uffff\2\60\2\uffff\1\115\1\uffff\1\111\1\107\2\uffff\1\104\2"+
        "\60\1\uffff\1\60\1\124\2\uffff\1\60\2\105\1\uffff\1\60\1\117\1\60"+
        "\1\114\1\103\2\uffff\1\137\1\105\1\60\1\uffff\1\124\1\116\1\uffff"+
        "\1\105\2\60\3\uffff\1\60\2\uffff\1\60\1\uffff\1\60\1\117\1\60\1"+
        "\105\3\uffff\2\60\1\uffff\2\103\1\uffff\1\116\1\uffff\1\60\2\124"+
        "\1\60\1\uffff\1\60\1\103\1\60\5\uffff\1\116\1\uffff\1\104\2\uffff"+
        "\2\117\1\104\1\uffff\1\60\1\111\2\uffff\1\105\1\uffff\2\60\2\116"+
        "\1\60\1\uffff\1\115\1\60\2\uffff\2\104\1\60\1\uffff\1\105\1\uffff"+
        "\2\60\1\uffff\2\60\1\uffff\1\60\4\uffff";
    static final String DFA21_maxS =
        "\1\176\1\122\1\125\2\126\1\125\1\75\1\117\1\123\1\130\1\123\1\122"+
        "\1\131\1\117\1\111\1\125\1\117\1\123\1\114\1\123\1\122\2\111\1\117"+
        "\13\uffff\1\76\1\75\2\uffff\1\55\4\uffff\1\174\3\uffff\2\uffff\2"+
        "\172\3\uffff\1\125\1\172\1\115\1\114\1\117\1\114\1\122\1\117\1\114"+
        "\1\104\1\172\1\124\1\107\1\172\1\124\2\172\1\105\1\127\1\114\1\116"+
        "\2\uffff\1\115\1\106\1\116\1\123\3\172\1\111\1\105\1\123\1\126\1"+
        "\125\1\131\1\124\1\117\1\172\1\124\1\116\1\124\1\126\1\125\1\105"+
        "\2\124\1\111\1\115\1\111\1\107\1\127\1\124\1\116\1\123\1\125\2\111"+
        "\1\104\1\117\1\125\1\116\1\130\1\105\1\114\1\122\10\uffff\2\uffff"+
        "\1\47\2\uffff\1\47\1\146\1\uffff\1\172\1\uffff\1\71\1\105\1\uffff"+
        "\1\105\1\123\1\115\1\114\1\123\1\101\3\172\1\uffff\1\105\1\172\1"+
        "\105\1\uffff\1\172\2\uffff\1\122\2\172\1\114\1\117\1\105\1\111\1"+
        "\124\1\107\1\124\1\uffff\1\122\1\uffff\1\172\1\uffff\1\123\1\105"+
        "\1\115\1\103\1\101\1\117\1\124\1\106\1\172\1\102\1\105\1\172\1\122"+
        "\1\125\1\uffff\1\114\1\110\1\111\1\101\1\117\1\127\1\111\2\122\1"+
        "\110\1\105\1\172\1\117\1\104\1\172\1\116\1\110\1\172\1\122\1\107"+
        "\1\124\2\123\1\116\1\172\2\117\2\101\1\123\1\125\1\115\1\116\1\172"+
        "\1\114\1\122\1\172\1\127\1\125\1\172\1\uffff\1\uffff\1\uffff\3\172"+
        "\1\117\1\105\2\172\2\124\3\uffff\1\122\1\uffff\2\122\1\uffff\1\172"+
        "\1\uffff\1\172\1\123\1\172\1\124\3\172\1\105\1\172\1\122\1\uffff"+
        "\1\124\1\120\1\105\1\172\1\125\1\104\1\111\1\105\1\uffff\1\114\2"+
        "\172\1\uffff\1\105\1\120\1\105\1\172\1\116\2\122\1\105\1\116\1\172"+
        "\1\105\1\172\1\103\1\uffff\1\116\1\105\1\uffff\1\172\1\124\1\172"+
        "\1\uffff\2\105\2\111\1\172\1\107\1\uffff\1\116\1\125\2\124\1\123"+
        "\1\115\1\105\2\124\1\uffff\1\111\1\117\1\uffff\1\172\1\105\3\uffff"+
        "\1\125\1\172\2\uffff\5\172\1\uffff\1\172\1\uffff\1\105\1\uffff\1"+
        "\172\3\uffff\1\101\1\uffff\1\126\1\123\1\124\1\116\1\111\1\uffff"+
        "\1\114\1\105\1\116\1\122\1\105\1\111\2\uffff\1\101\1\172\1\101\1"+
        "\uffff\1\124\1\131\2\105\1\107\1\172\1\uffff\1\172\1\uffff\1\124"+
        "\1\104\1\172\1\uffff\1\172\1\uffff\1\101\1\172\1\124\1\116\1\uffff"+
        "\2\172\1\116\2\105\1\172\2\116\2\172\1\105\2\123\1\uffff\1\172\1"+
        "\124\7\uffff\1\103\1\uffff\1\115\1\101\1\105\2\172\1\124\1\102\1"+
        "\124\1\172\1\103\1\105\1\172\2\115\1\uffff\1\116\3\172\1\116\1\172"+
        "\2\uffff\2\172\2\uffff\1\115\1\uffff\1\111\1\107\2\uffff\1\104\2"+
        "\172\1\uffff\1\172\1\124\2\uffff\1\172\2\105\1\uffff\1\172\1\117"+
        "\1\172\1\114\1\103\2\uffff\1\137\1\105\1\172\1\uffff\1\124\1\116"+
        "\1\uffff\1\105\2\172\3\uffff\1\172\2\uffff\1\172\1\uffff\1\172\1"+
        "\117\1\172\1\105\3\uffff\2\172\1\uffff\2\103\1\uffff\1\116\1\uffff"+
        "\1\172\2\124\1\172\1\uffff\1\172\1\103\1\172\5\uffff\1\116\1\uffff"+
        "\1\104\2\uffff\2\117\1\104\1\uffff\1\172\1\111\2\uffff\1\105\1\uffff"+
        "\2\172\2\116\1\172\1\uffff\1\115\1\172\2\uffff\2\104\1\172\1\uffff"+
        "\1\105\1\uffff\2\172\1\uffff\2\172\1\uffff\1\172\4\uffff";
    static final String DFA21_acceptS =
        "\30\uffff\1\152\1\153\1\154\1\155\1\156\1\157\1\160\1\161\1\162"+
        "\1\163\1\164\2\uffff\1\172\1\173\1\uffff\1\175\1\176\1\u0080\1\u0081"+
        "\1\uffff\1\u0083\1\u0084\1\u0085\4\uffff\1\u008b\1\u008c\1\u008d"+
        "\25\uffff\1\165\1\6\53\uffff\1\166\1\167\1\170\1\171\1\u008e\1\174"+
        "\1\u0086\1\u0082\7\uffff\1\u0089\1\uffff\1\u008a\2\uffff\1\45\11"+
        "\uffff\1\22\3\uffff\1\5\1\uffff\1\32\1\61\12\uffff\1\10\1\uffff"+
        "\1\36\1\uffff\1\71\16\uffff\1\16\51\uffff\1\u0087\12\uffff\1\3\1"+
        "\4\1\12\1\uffff\1\143\2\uffff\1\60\1\uffff\1\103\12\uffff\1\50\10"+
        "\uffff\1\177\3\uffff\1\117\15\uffff\1\64\2\uffff\1\142\3\uffff\1"+
        "\57\6\uffff\1\77\11\uffff\1\145\2\uffff\1\144\2\uffff\1\116\1\u0088"+
        "\1\1\2\uffff\1\21\1\31\5\uffff\1\151\1\uffff\1\37\1\uffff\1\7\1"+
        "\uffff\1\27\1\65\1\106\1\uffff\1\56\5\uffff\1\13\6\uffff\1\54\1"+
        "\120\3\uffff\1\74\6\uffff\1\123\1\uffff\1\62\3\uffff\1\26\1\uffff"+
        "\1\104\4\uffff\1\66\15\uffff\1\70\2\uffff\1\2\1\42\1\52\1\43\1\14"+
        "\1\25\1\136\1\uffff\1\63\16\uffff\1\15\6\uffff\1\124\1\20\2\uffff"+
        "\1\101\1\30\1\uffff\1\147\2\uffff\1\34\1\35\3\uffff\1\76\2\uffff"+
        "\1\141\1\67\3\uffff\1\114\5\uffff\1\11\1\105\3\uffff\1\135\2\uffff"+
        "\1\53\3\uffff\1\51\1\75\1\72\1\uffff\1\17\1\23\1\uffff\1\121\4\uffff"+
        "\1\100\1\40\1\41\2\uffff\1\125\2\uffff\1\150\1\uffff\1\110\4\uffff"+
        "\1\115\3\uffff\1\107\1\47\1\73\1\122\1\111\1\uffff\1\113\1\uffff"+
        "\1\46\1\126\3\uffff\1\137\2\uffff\1\44\1\24\1\uffff\1\55\5\uffff"+
        "\1\146\2\uffff\1\33\1\102\3\uffff\1\133\1\uffff\1\112\2\uffff\1"+
        "\134\2\uffff\1\127\1\uffff\1\131\1\140\1\130\1\132";
    static final String DFA21_specialS =
        "\60\uffff\1\5\1\0\117\uffff\1\1\1\7\1\uffff\1\3\1\4\137\uffff\1"+
        "\2\1\uffff\1\6\u0138\uffff}>";
    static final String[] DFA21_transitionS = {
            "\2\66\2\uffff\1\66\22\uffff\1\66\1\6\1\61\1\uffff\1\57\1\51"+
            "\1\52\1\60\1\34\1\35\1\50\1\46\1\32\1\47\1\30\1\45\1\62\11\63"+
            "\1\31\1\33\1\43\1\42\1\44\1\56\1\uffff\1\3\1\14\1\24\1\12\1"+
            "\11\1\2\1\13\1\15\1\10\1\20\1\64\1\7\1\25\1\5\1\4\1\22\1\64"+
            "\1\21\1\17\1\1\1\23\1\26\1\16\1\27\2\64\1\36\1\uffff\1\37\1"+
            "\55\1\65\33\64\1\40\1\54\1\41\1\53",
            "\1\71\5\uffff\1\70\2\uffff\1\67",
            "\1\72\7\uffff\1\75\2\uffff\1\76\5\uffff\1\73\2\uffff\1\74",
            "\1\102\5\uffff\1\77\1\uffff\1\100\4\uffff\1\101\2\uffff\1\103",
            "\1\107\7\uffff\1\106\3\uffff\1\104\2\uffff\1\105\1\110",
            "\1\113\15\uffff\1\111\5\uffff\1\112",
            "\1\114",
            "\1\121\3\uffff\1\117\3\uffff\1\116\5\uffff\1\120",
            "\1\122\7\uffff\1\124\4\uffff\1\123",
            "\1\126\13\uffff\1\125",
            "\1\132\3\uffff\1\127\3\uffff\1\130\5\uffff\1\131\3\uffff\1"+
            "\133",
            "\1\134",
            "\1\140\3\uffff\1\137\5\uffff\1\136\11\uffff\1\135",
            "\1\141\15\uffff\1\142",
            "\1\143\1\144",
            "\1\145\6\uffff\1\146\10\uffff\1\147",
            "\1\150",
            "\1\154\7\uffff\1\151\5\uffff\1\152\3\uffff\1\153",
            "\1\155\12\uffff\1\156",
            "\1\160\1\uffff\1\161\2\uffff\1\157",
            "\1\163\2\uffff\1\162",
            "\1\165\7\uffff\1\164",
            "\1\167\7\uffff\1\166",
            "\1\170",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "\1\171\1\114",
            "\1\173",
            "",
            "",
            "\1\175",
            "",
            "",
            "",
            "",
            "\1\177",
            "",
            "",
            "",
            "\47\u0081\1\u0083\64\u0081\1\u0082\uffa3\u0081",
            "\42\u0084\1\u0086\71\u0084\1\u0085\uffa3\u0084",
            "\1\u008a\1\uffff\12\u0089\7\uffff\27\64\1\u0087\2\64\4\uffff"+
            "\1\64\1\uffff\4\64\1\u008b\25\64",
            "\1\u008a\1\uffff\12\u0089\7\uffff\32\64\4\uffff\1\64\1\uffff"+
            "\4\64\1\u008b\25\64",
            "",
            "",
            "",
            "\1\u008c",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u008e",
            "\1\u008f",
            "\1\u0090",
            "\1\u0091",
            "\1\u0092",
            "\1\u0093",
            "\1\u0094",
            "\1\u0095",
            "\12\64\7\uffff\2\64\1\u0096\27\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u0098",
            "\1\u0099",
            "\12\64\7\uffff\3\64\1\u009a\26\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u009c",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u009f",
            "\1\u00a0\2\uffff\1\u00a1",
            "\1\u00a2",
            "\1\u00a3",
            "",
            "",
            "\1\u00a4\1\uffff\1\u00a5",
            "\1\u00a6",
            "\1\u00a7",
            "\1\u00a8",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\12\64\7\uffff\23\64\1\u00aa\6\64\4\uffff\1\64\1\uffff\32\64",
            "\12\64\7\uffff\23\64\1\u00ac\6\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u00af\5\uffff\1\u00ae",
            "\1\u00b0",
            "\1\u00b3\2\uffff\1\u00b2\14\uffff\1\u00b1",
            "\1\u00b5\14\uffff\1\u00b4\2\uffff\1\u00b6",
            "\1\u00b7",
            "\1\u00b8\4\uffff\1\u00b9",
            "\1\u00ba",
            "\1\u00bb",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u00bd\4\uffff\1\u00be",
            "\1\u00bf\6\uffff\1\u00c0",
            "\1\u00c1\15\uffff\1\u00c2",
            "\1\u00c3",
            "\1\u00c4",
            "\1\u00c5",
            "\1\u00c6",
            "\1\u00c9\10\uffff\1\u00c7\7\uffff\1\u00c8",
            "\1\u00ca",
            "\1\u00cb",
            "\1\u00cc",
            "\1\u00cd",
            "\1\u00ce",
            "\1\u00cf",
            "\1\u00d0",
            "\1\u00d1\1\u00d2",
            "\1\u00d3",
            "\1\u00d5\3\uffff\1\u00d4",
            "\1\u00d7\6\uffff\1\u00d6",
            "\1\u00d8",
            "\1\u00d9\11\uffff\1\u00da",
            "\1\u00db\1\u00dc\7\uffff\1\u00dd",
            "\1\u00e0\10\uffff\1\u00df\1\uffff\1\u00de",
            "\1\u00e1",
            "\1\u00e2",
            "\1\u00e3",
            "\1\u00e4",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "\47\u0081\1\u0083\64\u0081\1\u0082\uffa3\u0081",
            "\0\u00e5",
            "\1\61\4\uffff\1\60",
            "\42\u0084\1\u0086\71\u0084\1\u0085\uffa3\u0084",
            "\0\u00e7",
            "\1\61\4\uffff\1\60",
            "\12\u00e8\7\uffff\6\u00e8\32\uffff\6\u00e8",
            "",
            "\1\u008a\1\uffff\12\u0089\7\uffff\32\64\4\uffff\1\64\1\uffff"+
            "\4\64\1\u008b\25\64",
            "",
            "\1\u008a\1\uffff\1\u008a\2\uffff\12\u00e9",
            "\1\u00ea",
            "",
            "\1\u00eb",
            "\1\u00ec",
            "\1\u00ed",
            "\1\u00ee",
            "\1\u00ef",
            "\1\u00f0",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "\1\u00f4",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u00f6",
            "",
            "\12\64\7\uffff\4\64\1\u00f7\25\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "",
            "\1\u00f9",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u00fb",
            "\1\u00fc",
            "\1\u00fd",
            "\1\u00fe",
            "\1\u00ff",
            "\1\u0100",
            "\1\u0101",
            "",
            "\1\u0102",
            "",
            "\12\64\7\uffff\4\64\1\u0104\11\64\1\u0103\13\64\4\uffff\1\64"+
            "\1\uffff\32\64",
            "",
            "\1\u0106",
            "\1\u0107",
            "\1\u0108",
            "\1\u0109",
            "\1\u010a",
            "\1\u010b",
            "\1\u010c",
            "\1\u010d",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u010f",
            "\1\u0110",
            "\12\64\7\uffff\22\64\1\u0111\7\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u0113",
            "\1\u0114",
            "",
            "\1\u0115",
            "\1\u0116",
            "\1\u0117",
            "\1\u0118",
            "\1\u0119",
            "\1\u011a",
            "\1\u011b",
            "\1\u011c",
            "\1\u011d",
            "\1\u011e",
            "\1\u011f",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u0121",
            "\1\u0122",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u0124",
            "\1\u0125",
            "\12\64\7\uffff\22\64\1\u0126\7\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u0128",
            "\1\u0129",
            "\1\u012a",
            "\1\u012b",
            "\1\u012c",
            "\1\u012d",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u012f",
            "\1\u0130",
            "\1\u0131",
            "\1\u0132",
            "\1\u0133",
            "\1\u0134",
            "\1\u0135",
            "\1\u0136",
            "\12\64\7\uffff\24\64\1\u0137\5\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u0139",
            "\1\u013a",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u013c",
            "\1\u013d",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\47\u0081\1\u0083\64\u0081\1\u0082\uffa3\u0081",
            "",
            "\42\u0084\1\u0086\71\u0084\1\u0085\uffa3\u0084",
            "\12\u00e8\7\uffff\6\u00e8\24\64\4\uffff\1\64\1\uffff\6\u00e8"+
            "\24\64",
            "\12\u00e9\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u0141",
            "\1\u0142",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u0145",
            "\1\u0146",
            "",
            "",
            "",
            "\1\u0147",
            "",
            "\1\u0148",
            "\1\u0149",
            "",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "\12\64\7\uffff\22\64\1\u014b\7\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u014d",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u014f",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u0153",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u0155",
            "",
            "\1\u0156",
            "\1\u0157",
            "\1\u0158",
            "\12\64\7\uffff\21\64\1\u0159\10\64\4\uffff\1\64\1\uffff\32"+
            "\64",
            "\1\u015b",
            "\1\u015c",
            "\1\u015d",
            "\1\u015e",
            "",
            "\1\u015f",
            "\12\64\7\uffff\23\64\1\u0160\6\64\4\uffff\1\64\1\uffff\32\64",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "\1\u0163",
            "\1\u0164",
            "\1\u0165",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u0167",
            "\1\u0168",
            "\1\u0169",
            "\1\u016a",
            "\1\u016b",
            "\12\64\7\uffff\22\64\1\u016c\7\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u016e",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u0170",
            "",
            "\1\u0171",
            "\1\u0172",
            "",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u0174",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "\1\u0176",
            "\1\u0177",
            "\1\u0178",
            "\1\u0179",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u017b",
            "",
            "\1\u017c",
            "\1\u017d",
            "\1\u017e",
            "\1\u017f",
            "\1\u0180",
            "\1\u0181",
            "\1\u0182",
            "\1\u0183",
            "\1\u0184\1\u0185",
            "",
            "\1\u0186",
            "\1\u0187",
            "",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u0189",
            "",
            "",
            "",
            "\1\u018a",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "\1\u0192",
            "",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "",
            "",
            "\1\u0194",
            "",
            "\1\u0196\2\uffff\1\u0195",
            "\1\u0197",
            "\1\u0198",
            "\1\u0199",
            "\1\u019a",
            "",
            "\1\u019b",
            "\1\u019c",
            "\1\u019d",
            "\1\u019e",
            "\1\u019f",
            "\1\u01a0",
            "",
            "",
            "\1\u01a1",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u01a3",
            "",
            "\1\u01a4",
            "\1\u01a5",
            "\1\u01a6",
            "\1\u01a7",
            "\1\u01a8",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "\1\u01ab",
            "\1\u01ac",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "\1\u01af",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u01b1",
            "\1\u01b2",
            "",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u01b5",
            "\1\u01b6",
            "\1\u01b7",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u01b9",
            "\1\u01ba",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u01bd",
            "\1\u01be",
            "\1\u01bf",
            "",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u01c1",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "\1\u01c2",
            "",
            "\1\u01c3",
            "\1\u01c4",
            "\1\u01c5",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u01c8",
            "\1\u01c9",
            "\1\u01ca",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u01cc",
            "\1\u01cd",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u01cf",
            "\1\u01d0",
            "",
            "\1\u01d1",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u01d5",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\12\64\7\uffff\22\64\1\u01d8\7\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "",
            "\1\u01da",
            "",
            "\1\u01db",
            "\1\u01dc",
            "",
            "",
            "\1\u01dd",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u01e1",
            "",
            "",
            "\12\64\7\uffff\22\64\1\u01e2\7\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u01e4",
            "\1\u01e5",
            "",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u01e7",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u01e9",
            "\1\u01ea",
            "",
            "",
            "\1\u01eb",
            "\1\u01ec",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "\1\u01ee",
            "\1\u01ef",
            "",
            "\1\u01f0",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "",
            "",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u01f6",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u01f8",
            "",
            "",
            "",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "\1\u01fb",
            "\1\u01fc",
            "",
            "\1\u01fd",
            "",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u01ff",
            "\1\u0200",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u0203",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "",
            "",
            "",
            "",
            "\1\u0205",
            "",
            "\1\u0206",
            "",
            "",
            "\1\u0207",
            "\1\u0208",
            "\1\u0209",
            "",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u020b",
            "",
            "",
            "\1\u020c",
            "",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\1\u020f",
            "\1\u0210",
            "\12\64\7\uffff\22\64\1\u0211\7\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "\1\u0213",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "",
            "\1\u0215",
            "\1\u0216",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "\1\u0218",
            "",
            "\12\64\7\uffff\22\64\1\u0219\7\64\4\uffff\1\64\1\uffff\32\64",
            "\12\64\7\uffff\22\64\1\u021b\7\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "\12\64\7\uffff\32\64\4\uffff\1\64\1\uffff\32\64",
            "",
            "",
            "",
            ""
    };

    static final short[] DFA21_eot = DFA.unpackEncodedString(DFA21_eotS);
    static final short[] DFA21_eof = DFA.unpackEncodedString(DFA21_eofS);
    static final char[] DFA21_min = DFA.unpackEncodedStringToUnsignedChars(DFA21_minS);
    static final char[] DFA21_max = DFA.unpackEncodedStringToUnsignedChars(DFA21_maxS);
    static final short[] DFA21_accept = DFA.unpackEncodedString(DFA21_acceptS);
    static final short[] DFA21_special = DFA.unpackEncodedString(DFA21_specialS);
    static final short[][] DFA21_transition;

    static {
        int numStates = DFA21_transitionS.length;
        DFA21_transition = new short[numStates][];
        for (int i=0; i<numStates; i++) {
            DFA21_transition[i] = DFA.unpackEncodedString(DFA21_transitionS[i]);
        }
    }

    class DFA21 extends DFA {

        public DFA21(BaseRecognizer recognizer) {
            this.recognizer = recognizer;
            this.decisionNumber = 21;
            this.eot = DFA21_eot;
            this.eof = DFA21_eof;
            this.min = DFA21_min;
            this.max = DFA21_max;
            this.accept = DFA21_accept;
            this.special = DFA21_special;
            this.transition = DFA21_transition;
        }
        public String getDescription() {
            return "1:1: Tokens : ( KW_TRUE | KW_FALSE | KW_ALL | KW_AND | KW_OR | KW_NOT | KW_LIKE | KW_IF | KW_EXISTS | KW_ASC | KW_DESC | KW_ORDER | KW_GROUP | KW_BY | KW_HAVING | KW_WHERE | KW_FROM | KW_AS | KW_SELECT | KW_DISTINCT | KW_OUTER | KW_JOIN | KW_LEFT | KW_RIGHT | KW_FULL | KW_ON | KW_PARTITION | KW_USING | KW_UNION | KW_IS | KW_NULL | KW_CREATE | KW_COLUMN | KW_FIRST | KW_AFTER | KW_DESCRIBE | KW_TO | KW_COMMENT | KW_BOOLEAN | KW_INT | KW_BIGINT | KW_FLOAT | KW_DOUBLE | KW_DATE | KW_DATETIME | KW_INTO | KW_ROW | KW_OUT | KW_OF | KW_WITH | KW_LIMIT | KW_SET | KW_LONG | KW_PLUS | KW_MINUS | KW_VIEW | KW_IN | KW_BEFORE | KW_BETWEEN | KW_BOTH | KW_BINARY | KW_CROSS | KW_USE | KW_UPDATE | KW_SLIDE | KW_UNBOUNDED | KW_NOW | KW_ROWS | KW_EXCEPT | KW_LAST | KW_DSTREAM | KW_ISTREAM | KW_RSTREAM | KW_DIFFERENCE | KW_PASSING | KW_VALUE | KW_DEFAULT | KW_XOR | KW_DAY | KW_DAYS | KW_SECOND | KW_SECONDS | KW_HOUR | KW_HOURS | KW_MINUTE | KW_MINUTES | KW_MILLISECOND | KW_MILLISECONDS | KW_MICROSECOND | KW_MICROSECONDS | KW_NANOSECOND | KW_NANOSECONDS | KW_DECODE | KW_NULLS | KW_INTERVAL | KW_ELEMENT_TIME | KW_COUNT | KW_SUM | KW_AVG | KW_MAX | KW_MIN | KW_INTERSECT | KW_RANGE | KW_TIMEOUT | KW_OVER | DOT | COLON | COMMA | SEMICOLON | LPAREN | RPAREN | LSQUARE | RSQUARE | LCURLY | RCURLY | EQUAL | NOTEQUAL | LESSTHANOREQUALTO | LESSTHAN | GREATERTHANOREQUALTO | GREATERTHAN | DIVIDE | PLUS | MINUS | STAR | MOD | DIV | AMPERSAND | TILDE | BITWISEOR | BITWISEXOR | QUESTION | DOLLAR | CONCATENATE | StringLiteral | CharSetLiteral | Integer | Number | Identifier | CharSetName | WS | COMMENT );";
        }
        public int specialStateTransition(int s, IntStream _input) throws NoViableAltException {
            IntStream input = _input;
        	int _s = s;
            switch ( s ) {
                    case 0 : 
                        int LA21_49 = input.LA(1);

                        s = -1;
                        if ( ((LA21_49 >= '\u0000' && LA21_49 <= '!')||(LA21_49 >= '#' && LA21_49 <= '[')||(LA21_49 >= ']' && LA21_49 <= '\uFFFF')) ) {s = 132;}

                        else if ( (LA21_49=='\\') ) {s = 133;}

                        else if ( (LA21_49=='\"') ) {s = 134;}

                        if ( s>=0 ) return s;
                        break;

                    case 1 : 
                        int LA21_129 = input.LA(1);

                        s = -1;
                        if ( (LA21_129=='\'') ) {s = 131;}

                        else if ( ((LA21_129 >= '\u0000' && LA21_129 <= '&')||(LA21_129 >= '(' && LA21_129 <= '[')||(LA21_129 >= ']' && LA21_129 <= '\uFFFF')) ) {s = 129;}

                        else if ( (LA21_129=='\\') ) {s = 130;}

                        if ( s>=0 ) return s;
                        break;

                    case 2 : 
                        int LA21_229 = input.LA(1);

                        s = -1;
                        if ( (LA21_229=='\'') ) {s = 131;}

                        else if ( ((LA21_229 >= '\u0000' && LA21_229 <= '&')||(LA21_229 >= '(' && LA21_229 <= '[')||(LA21_229 >= ']' && LA21_229 <= '\uFFFF')) ) {s = 129;}

                        else if ( (LA21_229=='\\') ) {s = 130;}

                        if ( s>=0 ) return s;
                        break;

                    case 3 : 
                        int LA21_132 = input.LA(1);

                        s = -1;
                        if ( (LA21_132=='\"') ) {s = 134;}

                        else if ( ((LA21_132 >= '\u0000' && LA21_132 <= '!')||(LA21_132 >= '#' && LA21_132 <= '[')||(LA21_132 >= ']' && LA21_132 <= '\uFFFF')) ) {s = 132;}

                        else if ( (LA21_132=='\\') ) {s = 133;}

                        if ( s>=0 ) return s;
                        break;

                    case 4 : 
                        int LA21_133 = input.LA(1);

                        s = -1;
                        if ( ((LA21_133 >= '\u0000' && LA21_133 <= '\uFFFF')) ) {s = 231;}

                        if ( s>=0 ) return s;
                        break;

                    case 5 : 
                        int LA21_48 = input.LA(1);

                        s = -1;
                        if ( ((LA21_48 >= '\u0000' && LA21_48 <= '&')||(LA21_48 >= '(' && LA21_48 <= '[')||(LA21_48 >= ']' && LA21_48 <= '\uFFFF')) ) {s = 129;}

                        else if ( (LA21_48=='\\') ) {s = 130;}

                        else if ( (LA21_48=='\'') ) {s = 131;}

                        if ( s>=0 ) return s;
                        break;

                    case 6 : 
                        int LA21_231 = input.LA(1);

                        s = -1;
                        if ( (LA21_231=='\"') ) {s = 134;}

                        else if ( ((LA21_231 >= '\u0000' && LA21_231 <= '!')||(LA21_231 >= '#' && LA21_231 <= '[')||(LA21_231 >= ']' && LA21_231 <= '\uFFFF')) ) {s = 132;}

                        else if ( (LA21_231=='\\') ) {s = 133;}

                        if ( s>=0 ) return s;
                        break;

                    case 7 : 
                        int LA21_130 = input.LA(1);

                        s = -1;
                        if ( ((LA21_130 >= '\u0000' && LA21_130 <= '\uFFFF')) ) {s = 229;}

                        if ( s>=0 ) return s;
                        break;
            }
            NoViableAltException nvae =
                new NoViableAltException(getDescription(), 21, _s, input);
            error(nvae);
            throw nvae;
        }

    }
 

}