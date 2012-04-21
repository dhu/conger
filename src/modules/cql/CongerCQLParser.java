// $ANTLR 3.4 CongerCQL.g 2012-04-21 21:15:20

import org.antlr.runtime.*;
import java.util.Stack;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;

import org.antlr.runtime.tree.*;


@SuppressWarnings({"all", "warnings", "unchecked"})
public class CongerCQLParser extends Parser {
    public static final String[] tokenNames = new String[] {
        "<invalid>", "<EOR>", "<DOWN>", "<UP>", "AMPERSAND", "BITWISEOR", "BITWISEXOR", "COLON", "COMMA", "COMMENT", "CONCATENATE", "CharSetLiteral", "CharSetName", "DIV", "DIVIDE", "DOLLAR", "DOT", "Digit", "EQUAL", "Exponent", "GREATERTHAN", "GREATERTHANOREQUALTO", "HexDigit", "Identifier", "Integer", "KW_AFTER", "KW_ALL", "KW_AND", "KW_AS", "KW_ASC", "KW_AVG", "KW_BEFORE", "KW_BETWEEN", "KW_BIGINT", "KW_BINARY", "KW_BOOLEAN", "KW_BOTH", "KW_BY", "KW_COLUMN", "KW_COMMENT", "KW_COUNT", "KW_CREATE", "KW_CROSS", "KW_DATE", "KW_DATETIME", "KW_DAY", "KW_DAYS", "KW_DECODE", "KW_DEFAULT", "KW_DESC", "KW_DESCRIBE", "KW_DIFFERENCE", "KW_DISTINCT", "KW_DOUBLE", "KW_DSTREAM", "KW_ELEMENT_TIME", "KW_EXCEPT", "KW_EXISTS", "KW_FALSE", "KW_FIRST", "KW_FLOAT", "KW_FROM", "KW_FULL", "KW_GROUP", "KW_HAVING", "KW_HOUR", "KW_HOURS", "KW_IF", "KW_IN", "KW_INT", "KW_INTERSECT", "KW_INTERVAL", "KW_INTO", "KW_IS", "KW_ISTREAM", "KW_JOIN", "KW_LAST", "KW_LEFT", "KW_LIKE", "KW_LIMIT", "KW_LONG", "KW_MAX", "KW_MICROSECOND", "KW_MICROSECONDS", "KW_MILLISECOND", "KW_MILLISECONDS", "KW_MIN", "KW_MINUS", "KW_MINUTE", "KW_MINUTES", "KW_NANOSECOND", "KW_NANOSECONDS", "KW_NOT", "KW_NOW", "KW_NULL", "KW_NULLS", "KW_OF", "KW_ON", "KW_OR", "KW_ORDER", "KW_OUT", "KW_OUTER", "KW_PARTITION", "KW_PASSING", "KW_PLUS", "KW_RANGE", "KW_RIGHT", "KW_ROW", "KW_ROWS", "KW_RSTREAM", "KW_SECOND", "KW_SECONDS", "KW_SELECT", "KW_SET", "KW_SLIDE", "KW_SUM", "KW_TO", "KW_TRUE", "KW_UNBOUNDED", "KW_UNION", "KW_UPDATE", "KW_USE", "KW_USING", "KW_VALUE", "KW_VIEW", "KW_WHERE", "KW_WITH", "KW_XOR", "LCURLY", "LESSTHAN", "LESSTHANOREQUALTO", "LPAREN", "LSQUARE", "Letter", "MINUS", "MOD", "NOTEQUAL", "Number", "PLUS", "QUESTION", "RCURLY", "RPAREN", "RSQUARE", "RegexComponent", "SEMICOLON", "STAR", "StringLiteral", "TILDE", "TOK_AGGR", "TOK_AGGR_DISTINCT", "TOK_ARG_LIST", "TOK_ARITH_EXPR", "TOK_ATTR_LIST", "TOK_BETWEEN", "TOK_BINARY", "TOK_COMPARE", "TOK_COND", "TOK_COND_LIST", "TOK_CREATE_VIEW", "TOK_FROM", "TOK_FUNC", "TOK_GROUP_BY", "TOK_HAVING", "TOK_IDSTREAM", "TOK_JOIN", "TOK_ORDER_BY", "TOK_ORDER_EXPR", "TOK_PARTITION", "TOK_PROJTERM", "TOK_PROJTERM_LIST", "TOK_QUERY", "TOK_RELATION_LIST", "TOK_RELATION_VARIABLE", "TOK_RSTREAM", "TOK_SELECT", "TOK_SFW", "TOK_UNARY_COND", "TOK_USING", "TOK_WHERE", "TOK_WINDOW", "WS"
    };

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
    public static final int KW_PARTITION=102;
    public static final int KW_PASSING=103;
    public static final int KW_PLUS=104;
    public static final int KW_RANGE=105;
    public static final int KW_RIGHT=106;
    public static final int KW_ROW=107;
    public static final int KW_ROWS=108;
    public static final int KW_RSTREAM=109;
    public static final int KW_SECOND=110;
    public static final int KW_SECONDS=111;
    public static final int KW_SELECT=112;
    public static final int KW_SET=113;
    public static final int KW_SLIDE=114;
    public static final int KW_SUM=115;
    public static final int KW_TO=116;
    public static final int KW_TRUE=117;
    public static final int KW_UNBOUNDED=118;
    public static final int KW_UNION=119;
    public static final int KW_UPDATE=120;
    public static final int KW_USE=121;
    public static final int KW_USING=122;
    public static final int KW_VALUE=123;
    public static final int KW_VIEW=124;
    public static final int KW_WHERE=125;
    public static final int KW_WITH=126;
    public static final int KW_XOR=127;
    public static final int LCURLY=128;
    public static final int LESSTHAN=129;
    public static final int LESSTHANOREQUALTO=130;
    public static final int LPAREN=131;
    public static final int LSQUARE=132;
    public static final int Letter=133;
    public static final int MINUS=134;
    public static final int MOD=135;
    public static final int NOTEQUAL=136;
    public static final int Number=137;
    public static final int PLUS=138;
    public static final int QUESTION=139;
    public static final int RCURLY=140;
    public static final int RPAREN=141;
    public static final int RSQUARE=142;
    public static final int RegexComponent=143;
    public static final int SEMICOLON=144;
    public static final int STAR=145;
    public static final int StringLiteral=146;
    public static final int TILDE=147;
    public static final int TOK_AGGR=148;
    public static final int TOK_AGGR_DISTINCT=149;
    public static final int TOK_ARG_LIST=150;
    public static final int TOK_ARITH_EXPR=151;
    public static final int TOK_ATTR_LIST=152;
    public static final int TOK_BETWEEN=153;
    public static final int TOK_BINARY=154;
    public static final int TOK_COMPARE=155;
    public static final int TOK_COND=156;
    public static final int TOK_COND_LIST=157;
    public static final int TOK_CREATE_VIEW=158;
    public static final int TOK_FROM=159;
    public static final int TOK_FUNC=160;
    public static final int TOK_GROUP_BY=161;
    public static final int TOK_HAVING=162;
    public static final int TOK_IDSTREAM=163;
    public static final int TOK_JOIN=164;
    public static final int TOK_ORDER_BY=165;
    public static final int TOK_ORDER_EXPR=166;
    public static final int TOK_PARTITION=167;
    public static final int TOK_PROJTERM=168;
    public static final int TOK_PROJTERM_LIST=169;
    public static final int TOK_QUERY=170;
    public static final int TOK_RELATION_LIST=171;
    public static final int TOK_RELATION_VARIABLE=172;
    public static final int TOK_RSTREAM=173;
    public static final int TOK_SELECT=174;
    public static final int TOK_SFW=175;
    public static final int TOK_UNARY_COND=176;
    public static final int TOK_USING=177;
    public static final int TOK_WHERE=178;
    public static final int TOK_WINDOW=179;
    public static final int WS=180;

    // delegates
    public Parser[] getDelegates() {
        return new Parser[] {};
    }

    // delegators


    public CongerCQLParser(TokenStream input) {
        this(input, new RecognizerSharedState());
    }
    public CongerCQLParser(TokenStream input, RecognizerSharedState state) {
        super(input, state);
    }

protected TreeAdaptor adaptor = new CommonTreeAdaptor();

public void setTreeAdaptor(TreeAdaptor adaptor) {
    this.adaptor = adaptor;
}
public TreeAdaptor getTreeAdaptor() {
    return adaptor;
}
    public String[] getTokenNames() { return CongerCQLParser.tokenNames; }
    public String getGrammarFileName() { return "CongerCQL.g"; }


    public static class statement_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "statement"
    // CongerCQL.g:50:1: statement : query EOF ;
    public final CongerCQLParser.statement_return statement() throws RecognitionException {
        CongerCQLParser.statement_return retval = new CongerCQLParser.statement_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token EOF2=null;
        CongerCQLParser.query_return query1 =null;


        CommonTree EOF2_tree=null;

        try {
            // CongerCQL.g:51:2: ( query EOF )
            // CongerCQL.g:51:4: query EOF
            {
            root_0 = (CommonTree)adaptor.nil();


            pushFollow(FOLLOW_query_in_statement257);
            query1=query();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) adaptor.addChild(root_0, query1.getTree());

            EOF2=(Token)match(input,EOF,FOLLOW_EOF_in_statement259); if (state.failed) return retval;
            if ( state.backtracking==0 ) {
            EOF2_tree = 
            (CommonTree)adaptor.create(EOF2)
            ;
            adaptor.addChild(root_0, EOF2_tree);
            }

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "statement"


    public static class query_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "query"
    // CongerCQL.g:54:1: query : ( sfw_block | idstream_clause LPAREN sfw_block RPAREN using_clause -> ^( TOK_IDSTREAM sfw_block using_clause ) | rstream LPAREN sfw_block RPAREN -> ^( TOK_RSTREAM sfw_block ) | binary | idstream_clause LPAREN binary RPAREN using_clause -> ^( TOK_IDSTREAM binary using_clause ) | rstream LPAREN binary RPAREN -> ^( TOK_RSTREAM binary ) );
    public final CongerCQLParser.query_return query() throws RecognitionException {
        CongerCQLParser.query_return retval = new CongerCQLParser.query_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token LPAREN5=null;
        Token RPAREN7=null;
        Token LPAREN10=null;
        Token RPAREN12=null;
        Token LPAREN15=null;
        Token RPAREN17=null;
        Token LPAREN20=null;
        Token RPAREN22=null;
        CongerCQLParser.sfw_block_return sfw_block3 =null;

        CongerCQLParser.idstream_clause_return idstream_clause4 =null;

        CongerCQLParser.sfw_block_return sfw_block6 =null;

        CongerCQLParser.using_clause_return using_clause8 =null;

        CongerCQLParser.rstream_return rstream9 =null;

        CongerCQLParser.sfw_block_return sfw_block11 =null;

        CongerCQLParser.binary_return binary13 =null;

        CongerCQLParser.idstream_clause_return idstream_clause14 =null;

        CongerCQLParser.binary_return binary16 =null;

        CongerCQLParser.using_clause_return using_clause18 =null;

        CongerCQLParser.rstream_return rstream19 =null;

        CongerCQLParser.binary_return binary21 =null;


        CommonTree LPAREN5_tree=null;
        CommonTree RPAREN7_tree=null;
        CommonTree LPAREN10_tree=null;
        CommonTree RPAREN12_tree=null;
        CommonTree LPAREN15_tree=null;
        CommonTree RPAREN17_tree=null;
        CommonTree LPAREN20_tree=null;
        CommonTree RPAREN22_tree=null;
        RewriteRuleTokenStream stream_RPAREN=new RewriteRuleTokenStream(adaptor,"token RPAREN");
        RewriteRuleTokenStream stream_LPAREN=new RewriteRuleTokenStream(adaptor,"token LPAREN");
        RewriteRuleSubtreeStream stream_binary=new RewriteRuleSubtreeStream(adaptor,"rule binary");
        RewriteRuleSubtreeStream stream_sfw_block=new RewriteRuleSubtreeStream(adaptor,"rule sfw_block");
        RewriteRuleSubtreeStream stream_idstream_clause=new RewriteRuleSubtreeStream(adaptor,"rule idstream_clause");
        RewriteRuleSubtreeStream stream_using_clause=new RewriteRuleSubtreeStream(adaptor,"rule using_clause");
        RewriteRuleSubtreeStream stream_rstream=new RewriteRuleSubtreeStream(adaptor,"rule rstream");
        try {
            // CongerCQL.g:55:2: ( sfw_block | idstream_clause LPAREN sfw_block RPAREN using_clause -> ^( TOK_IDSTREAM sfw_block using_clause ) | rstream LPAREN sfw_block RPAREN -> ^( TOK_RSTREAM sfw_block ) | binary | idstream_clause LPAREN binary RPAREN using_clause -> ^( TOK_IDSTREAM binary using_clause ) | rstream LPAREN binary RPAREN -> ^( TOK_RSTREAM binary ) )
            int alt1=6;
            switch ( input.LA(1) ) {
            case KW_SELECT:
                {
                alt1=1;
                }
                break;
            case KW_DSTREAM:
            case KW_ISTREAM:
                {
                int LA1_2 = input.LA(2);

                if ( (LA1_2==LPAREN) ) {
                    int LA1_5 = input.LA(3);

                    if ( (LA1_5==KW_SELECT) ) {
                        alt1=2;
                    }
                    else if ( (LA1_5==Identifier) ) {
                        alt1=5;
                    }
                    else {
                        if (state.backtracking>0) {state.failed=true; return retval;}
                        NoViableAltException nvae =
                            new NoViableAltException("", 1, 5, input);

                        throw nvae;

                    }
                }
                else {
                    if (state.backtracking>0) {state.failed=true; return retval;}
                    NoViableAltException nvae =
                        new NoViableAltException("", 1, 2, input);

                    throw nvae;

                }
                }
                break;
            case KW_RSTREAM:
                {
                int LA1_3 = input.LA(2);

                if ( (LA1_3==LPAREN) ) {
                    int LA1_6 = input.LA(3);

                    if ( (LA1_6==KW_SELECT) ) {
                        alt1=3;
                    }
                    else if ( (LA1_6==Identifier) ) {
                        alt1=6;
                    }
                    else {
                        if (state.backtracking>0) {state.failed=true; return retval;}
                        NoViableAltException nvae =
                            new NoViableAltException("", 1, 6, input);

                        throw nvae;

                    }
                }
                else {
                    if (state.backtracking>0) {state.failed=true; return retval;}
                    NoViableAltException nvae =
                        new NoViableAltException("", 1, 3, input);

                    throw nvae;

                }
                }
                break;
            case Identifier:
                {
                alt1=4;
                }
                break;
            default:
                if (state.backtracking>0) {state.failed=true; return retval;}
                NoViableAltException nvae =
                    new NoViableAltException("", 1, 0, input);

                throw nvae;

            }

            switch (alt1) {
                case 1 :
                    // CongerCQL.g:55:4: sfw_block
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    pushFollow(FOLLOW_sfw_block_in_query270);
                    sfw_block3=sfw_block();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, sfw_block3.getTree());

                    }
                    break;
                case 2 :
                    // CongerCQL.g:56:4: idstream_clause LPAREN sfw_block RPAREN using_clause
                    {
                    pushFollow(FOLLOW_idstream_clause_in_query275);
                    idstream_clause4=idstream_clause();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_idstream_clause.add(idstream_clause4.getTree());

                    LPAREN5=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_query277); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_LPAREN.add(LPAREN5);


                    pushFollow(FOLLOW_sfw_block_in_query279);
                    sfw_block6=sfw_block();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_sfw_block.add(sfw_block6.getTree());

                    RPAREN7=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_query281); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_RPAREN.add(RPAREN7);


                    pushFollow(FOLLOW_using_clause_in_query283);
                    using_clause8=using_clause();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_using_clause.add(using_clause8.getTree());

                    // AST REWRITE
                    // elements: using_clause, sfw_block
                    // token labels: 
                    // rule labels: retval
                    // token list labels: 
                    // rule list labels: 
                    // wildcard labels: 
                    if ( state.backtracking==0 ) {

                    retval.tree = root_0;
                    RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

                    root_0 = (CommonTree)adaptor.nil();
                    // 57:9: -> ^( TOK_IDSTREAM sfw_block using_clause )
                    {
                        // CongerCQL.g:57:12: ^( TOK_IDSTREAM sfw_block using_clause )
                        {
                        CommonTree root_1 = (CommonTree)adaptor.nil();
                        root_1 = (CommonTree)adaptor.becomeRoot(
                        (CommonTree)adaptor.create(TOK_IDSTREAM, "TOK_IDSTREAM")
                        , root_1);

                        adaptor.addChild(root_1, stream_sfw_block.nextTree());

                        adaptor.addChild(root_1, stream_using_clause.nextTree());

                        adaptor.addChild(root_0, root_1);
                        }

                    }


                    retval.tree = root_0;
                    }

                    }
                    break;
                case 3 :
                    // CongerCQL.g:58:4: rstream LPAREN sfw_block RPAREN
                    {
                    pushFollow(FOLLOW_rstream_in_query306);
                    rstream9=rstream();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_rstream.add(rstream9.getTree());

                    LPAREN10=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_query308); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_LPAREN.add(LPAREN10);


                    pushFollow(FOLLOW_sfw_block_in_query310);
                    sfw_block11=sfw_block();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_sfw_block.add(sfw_block11.getTree());

                    RPAREN12=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_query312); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_RPAREN.add(RPAREN12);


                    // AST REWRITE
                    // elements: sfw_block
                    // token labels: 
                    // rule labels: retval
                    // token list labels: 
                    // rule list labels: 
                    // wildcard labels: 
                    if ( state.backtracking==0 ) {

                    retval.tree = root_0;
                    RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

                    root_0 = (CommonTree)adaptor.nil();
                    // 59:9: -> ^( TOK_RSTREAM sfw_block )
                    {
                        // CongerCQL.g:59:12: ^( TOK_RSTREAM sfw_block )
                        {
                        CommonTree root_1 = (CommonTree)adaptor.nil();
                        root_1 = (CommonTree)adaptor.becomeRoot(
                        (CommonTree)adaptor.create(TOK_RSTREAM, "TOK_RSTREAM")
                        , root_1);

                        adaptor.addChild(root_1, stream_sfw_block.nextTree());

                        adaptor.addChild(root_0, root_1);
                        }

                    }


                    retval.tree = root_0;
                    }

                    }
                    break;
                case 4 :
                    // CongerCQL.g:60:4: binary
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    pushFollow(FOLLOW_binary_in_query333);
                    binary13=binary();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, binary13.getTree());

                    }
                    break;
                case 5 :
                    // CongerCQL.g:61:4: idstream_clause LPAREN binary RPAREN using_clause
                    {
                    pushFollow(FOLLOW_idstream_clause_in_query338);
                    idstream_clause14=idstream_clause();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_idstream_clause.add(idstream_clause14.getTree());

                    LPAREN15=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_query340); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_LPAREN.add(LPAREN15);


                    pushFollow(FOLLOW_binary_in_query342);
                    binary16=binary();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_binary.add(binary16.getTree());

                    RPAREN17=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_query344); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_RPAREN.add(RPAREN17);


                    pushFollow(FOLLOW_using_clause_in_query346);
                    using_clause18=using_clause();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_using_clause.add(using_clause18.getTree());

                    // AST REWRITE
                    // elements: using_clause, binary
                    // token labels: 
                    // rule labels: retval
                    // token list labels: 
                    // rule list labels: 
                    // wildcard labels: 
                    if ( state.backtracking==0 ) {

                    retval.tree = root_0;
                    RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

                    root_0 = (CommonTree)adaptor.nil();
                    // 62:9: -> ^( TOK_IDSTREAM binary using_clause )
                    {
                        // CongerCQL.g:62:12: ^( TOK_IDSTREAM binary using_clause )
                        {
                        CommonTree root_1 = (CommonTree)adaptor.nil();
                        root_1 = (CommonTree)adaptor.becomeRoot(
                        (CommonTree)adaptor.create(TOK_IDSTREAM, "TOK_IDSTREAM")
                        , root_1);

                        adaptor.addChild(root_1, stream_binary.nextTree());

                        adaptor.addChild(root_1, stream_using_clause.nextTree());

                        adaptor.addChild(root_0, root_1);
                        }

                    }


                    retval.tree = root_0;
                    }

                    }
                    break;
                case 6 :
                    // CongerCQL.g:63:4: rstream LPAREN binary RPAREN
                    {
                    pushFollow(FOLLOW_rstream_in_query369);
                    rstream19=rstream();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_rstream.add(rstream19.getTree());

                    LPAREN20=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_query371); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_LPAREN.add(LPAREN20);


                    pushFollow(FOLLOW_binary_in_query373);
                    binary21=binary();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_binary.add(binary21.getTree());

                    RPAREN22=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_query375); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_RPAREN.add(RPAREN22);


                    // AST REWRITE
                    // elements: binary
                    // token labels: 
                    // rule labels: retval
                    // token list labels: 
                    // rule list labels: 
                    // wildcard labels: 
                    if ( state.backtracking==0 ) {

                    retval.tree = root_0;
                    RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

                    root_0 = (CommonTree)adaptor.nil();
                    // 64:9: -> ^( TOK_RSTREAM binary )
                    {
                        // CongerCQL.g:64:12: ^( TOK_RSTREAM binary )
                        {
                        CommonTree root_1 = (CommonTree)adaptor.nil();
                        root_1 = (CommonTree)adaptor.becomeRoot(
                        (CommonTree)adaptor.create(TOK_RSTREAM, "TOK_RSTREAM")
                        , root_1);

                        adaptor.addChild(root_1, stream_binary.nextTree());

                        adaptor.addChild(root_0, root_1);
                        }

                    }


                    retval.tree = root_0;
                    }

                    }
                    break;

            }
            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "query"


    public static class binary_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "binary"
    // CongerCQL.g:67:1: binary : view1= Identifier binary_operator view2= Identifier -> ^( TOK_BINARY $view1 $view2 binary_operator ) ;
    public final CongerCQLParser.binary_return binary() throws RecognitionException {
        CongerCQLParser.binary_return retval = new CongerCQLParser.binary_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token view1=null;
        Token view2=null;
        CongerCQLParser.binary_operator_return binary_operator23 =null;


        CommonTree view1_tree=null;
        CommonTree view2_tree=null;
        RewriteRuleTokenStream stream_Identifier=new RewriteRuleTokenStream(adaptor,"token Identifier");
        RewriteRuleSubtreeStream stream_binary_operator=new RewriteRuleSubtreeStream(adaptor,"rule binary_operator");
        try {
            // CongerCQL.g:68:2: (view1= Identifier binary_operator view2= Identifier -> ^( TOK_BINARY $view1 $view2 binary_operator ) )
            // CongerCQL.g:68:4: view1= Identifier binary_operator view2= Identifier
            {
            view1=(Token)match(input,Identifier,FOLLOW_Identifier_in_binary404); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_Identifier.add(view1);


            pushFollow(FOLLOW_binary_operator_in_binary406);
            binary_operator23=binary_operator();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_binary_operator.add(binary_operator23.getTree());

            view2=(Token)match(input,Identifier,FOLLOW_Identifier_in_binary410); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_Identifier.add(view2);


            // AST REWRITE
            // elements: binary_operator, view1, view2
            // token labels: view1, view2
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleTokenStream stream_view1=new RewriteRuleTokenStream(adaptor,"token view1",view1);
            RewriteRuleTokenStream stream_view2=new RewriteRuleTokenStream(adaptor,"token view2",view2);
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 69:6: -> ^( TOK_BINARY $view1 $view2 binary_operator )
            {
                // CongerCQL.g:69:9: ^( TOK_BINARY $view1 $view2 binary_operator )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_BINARY, "TOK_BINARY")
                , root_1);

                adaptor.addChild(root_1, stream_view1.nextNode());

                adaptor.addChild(root_1, stream_view2.nextNode());

                adaptor.addChild(root_1, stream_binary_operator.nextTree());

                adaptor.addChild(root_0, root_1);
                }

            }


            retval.tree = root_0;
            }

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "binary"


    public static class binary_operator_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "binary_operator"
    // CongerCQL.g:72:1: binary_operator : ( KW_EXCEPT | KW_MINUS | KW_INTERSECT | KW_UNION ( KW_ALL )? | ( KW_NOT )? KW_IN );
    public final CongerCQLParser.binary_operator_return binary_operator() throws RecognitionException {
        CongerCQLParser.binary_operator_return retval = new CongerCQLParser.binary_operator_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token KW_EXCEPT24=null;
        Token KW_MINUS25=null;
        Token KW_INTERSECT26=null;
        Token KW_UNION27=null;
        Token KW_ALL28=null;
        Token KW_NOT29=null;
        Token KW_IN30=null;

        CommonTree KW_EXCEPT24_tree=null;
        CommonTree KW_MINUS25_tree=null;
        CommonTree KW_INTERSECT26_tree=null;
        CommonTree KW_UNION27_tree=null;
        CommonTree KW_ALL28_tree=null;
        CommonTree KW_NOT29_tree=null;
        CommonTree KW_IN30_tree=null;

        try {
            // CongerCQL.g:73:2: ( KW_EXCEPT | KW_MINUS | KW_INTERSECT | KW_UNION ( KW_ALL )? | ( KW_NOT )? KW_IN )
            int alt4=5;
            switch ( input.LA(1) ) {
            case KW_EXCEPT:
                {
                alt4=1;
                }
                break;
            case KW_MINUS:
                {
                alt4=2;
                }
                break;
            case KW_INTERSECT:
                {
                alt4=3;
                }
                break;
            case KW_UNION:
                {
                alt4=4;
                }
                break;
            case KW_IN:
            case KW_NOT:
                {
                alt4=5;
                }
                break;
            default:
                if (state.backtracking>0) {state.failed=true; return retval;}
                NoViableAltException nvae =
                    new NoViableAltException("", 4, 0, input);

                throw nvae;

            }

            switch (alt4) {
                case 1 :
                    // CongerCQL.g:73:4: KW_EXCEPT
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    KW_EXCEPT24=(Token)match(input,KW_EXCEPT,FOLLOW_KW_EXCEPT_in_binary_operator440); if (state.failed) return retval;
                    if ( state.backtracking==0 ) {
                    KW_EXCEPT24_tree = 
                    (CommonTree)adaptor.create(KW_EXCEPT24)
                    ;
                    adaptor.addChild(root_0, KW_EXCEPT24_tree);
                    }

                    }
                    break;
                case 2 :
                    // CongerCQL.g:74:4: KW_MINUS
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    KW_MINUS25=(Token)match(input,KW_MINUS,FOLLOW_KW_MINUS_in_binary_operator445); if (state.failed) return retval;
                    if ( state.backtracking==0 ) {
                    KW_MINUS25_tree = 
                    (CommonTree)adaptor.create(KW_MINUS25)
                    ;
                    adaptor.addChild(root_0, KW_MINUS25_tree);
                    }

                    }
                    break;
                case 3 :
                    // CongerCQL.g:75:4: KW_INTERSECT
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    KW_INTERSECT26=(Token)match(input,KW_INTERSECT,FOLLOW_KW_INTERSECT_in_binary_operator450); if (state.failed) return retval;
                    if ( state.backtracking==0 ) {
                    KW_INTERSECT26_tree = 
                    (CommonTree)adaptor.create(KW_INTERSECT26)
                    ;
                    adaptor.addChild(root_0, KW_INTERSECT26_tree);
                    }

                    }
                    break;
                case 4 :
                    // CongerCQL.g:76:4: KW_UNION ( KW_ALL )?
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    KW_UNION27=(Token)match(input,KW_UNION,FOLLOW_KW_UNION_in_binary_operator455); if (state.failed) return retval;
                    if ( state.backtracking==0 ) {
                    KW_UNION27_tree = 
                    (CommonTree)adaptor.create(KW_UNION27)
                    ;
                    adaptor.addChild(root_0, KW_UNION27_tree);
                    }

                    // CongerCQL.g:76:13: ( KW_ALL )?
                    int alt2=2;
                    int LA2_0 = input.LA(1);

                    if ( (LA2_0==KW_ALL) ) {
                        alt2=1;
                    }
                    switch (alt2) {
                        case 1 :
                            // CongerCQL.g:76:13: KW_ALL
                            {
                            KW_ALL28=(Token)match(input,KW_ALL,FOLLOW_KW_ALL_in_binary_operator457); if (state.failed) return retval;
                            if ( state.backtracking==0 ) {
                            KW_ALL28_tree = 
                            (CommonTree)adaptor.create(KW_ALL28)
                            ;
                            adaptor.addChild(root_0, KW_ALL28_tree);
                            }

                            }
                            break;

                    }


                    }
                    break;
                case 5 :
                    // CongerCQL.g:77:4: ( KW_NOT )? KW_IN
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    // CongerCQL.g:77:4: ( KW_NOT )?
                    int alt3=2;
                    int LA3_0 = input.LA(1);

                    if ( (LA3_0==KW_NOT) ) {
                        alt3=1;
                    }
                    switch (alt3) {
                        case 1 :
                            // CongerCQL.g:77:4: KW_NOT
                            {
                            KW_NOT29=(Token)match(input,KW_NOT,FOLLOW_KW_NOT_in_binary_operator463); if (state.failed) return retval;
                            if ( state.backtracking==0 ) {
                            KW_NOT29_tree = 
                            (CommonTree)adaptor.create(KW_NOT29)
                            ;
                            adaptor.addChild(root_0, KW_NOT29_tree);
                            }

                            }
                            break;

                    }


                    KW_IN30=(Token)match(input,KW_IN,FOLLOW_KW_IN_in_binary_operator466); if (state.failed) return retval;
                    if ( state.backtracking==0 ) {
                    KW_IN30_tree = 
                    (CommonTree)adaptor.create(KW_IN30)
                    ;
                    adaptor.addChild(root_0, KW_IN30_tree);
                    }

                    }
                    break;

            }
            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "binary_operator"


    public static class sfw_block_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "sfw_block"
    // CongerCQL.g:80:1: sfw_block : select_clause from_clause ( ( opt_group_by_clause )? ( opt_having_clause )? | order_by_clause | opt_where_clause ( opt_group_by_clause )? ( opt_having_clause )? ) -> ^( TOK_SFW select_clause from_clause ( order_by_clause )? ( opt_where_clause )? ( opt_group_by_clause )? ( opt_having_clause )? ) ;
    public final CongerCQLParser.sfw_block_return sfw_block() throws RecognitionException {
        CongerCQLParser.sfw_block_return retval = new CongerCQLParser.sfw_block_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        CongerCQLParser.select_clause_return select_clause31 =null;

        CongerCQLParser.from_clause_return from_clause32 =null;

        CongerCQLParser.opt_group_by_clause_return opt_group_by_clause33 =null;

        CongerCQLParser.opt_having_clause_return opt_having_clause34 =null;

        CongerCQLParser.order_by_clause_return order_by_clause35 =null;

        CongerCQLParser.opt_where_clause_return opt_where_clause36 =null;

        CongerCQLParser.opt_group_by_clause_return opt_group_by_clause37 =null;

        CongerCQLParser.opt_having_clause_return opt_having_clause38 =null;


        RewriteRuleSubtreeStream stream_select_clause=new RewriteRuleSubtreeStream(adaptor,"rule select_clause");
        RewriteRuleSubtreeStream stream_opt_group_by_clause=new RewriteRuleSubtreeStream(adaptor,"rule opt_group_by_clause");
        RewriteRuleSubtreeStream stream_from_clause=new RewriteRuleSubtreeStream(adaptor,"rule from_clause");
        RewriteRuleSubtreeStream stream_order_by_clause=new RewriteRuleSubtreeStream(adaptor,"rule order_by_clause");
        RewriteRuleSubtreeStream stream_opt_having_clause=new RewriteRuleSubtreeStream(adaptor,"rule opt_having_clause");
        RewriteRuleSubtreeStream stream_opt_where_clause=new RewriteRuleSubtreeStream(adaptor,"rule opt_where_clause");
        try {
            // CongerCQL.g:81:2: ( select_clause from_clause ( ( opt_group_by_clause )? ( opt_having_clause )? | order_by_clause | opt_where_clause ( opt_group_by_clause )? ( opt_having_clause )? ) -> ^( TOK_SFW select_clause from_clause ( order_by_clause )? ( opt_where_clause )? ( opt_group_by_clause )? ( opt_having_clause )? ) )
            // CongerCQL.g:81:4: select_clause from_clause ( ( opt_group_by_clause )? ( opt_having_clause )? | order_by_clause | opt_where_clause ( opt_group_by_clause )? ( opt_having_clause )? )
            {
            pushFollow(FOLLOW_select_clause_in_sfw_block477);
            select_clause31=select_clause();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_select_clause.add(select_clause31.getTree());

            pushFollow(FOLLOW_from_clause_in_sfw_block479);
            from_clause32=from_clause();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_from_clause.add(from_clause32.getTree());

            // CongerCQL.g:82:2: ( ( opt_group_by_clause )? ( opt_having_clause )? | order_by_clause | opt_where_clause ( opt_group_by_clause )? ( opt_having_clause )? )
            int alt9=3;
            switch ( input.LA(1) ) {
            case EOF:
            case KW_GROUP:
            case KW_HAVING:
            case RPAREN:
                {
                alt9=1;
                }
                break;
            case KW_ORDER:
                {
                alt9=2;
                }
                break;
            case KW_WHERE:
                {
                alt9=3;
                }
                break;
            default:
                if (state.backtracking>0) {state.failed=true; return retval;}
                NoViableAltException nvae =
                    new NoViableAltException("", 9, 0, input);

                throw nvae;

            }

            switch (alt9) {
                case 1 :
                    // CongerCQL.g:82:3: ( opt_group_by_clause )? ( opt_having_clause )?
                    {
                    // CongerCQL.g:82:3: ( opt_group_by_clause )?
                    int alt5=2;
                    int LA5_0 = input.LA(1);

                    if ( (LA5_0==KW_GROUP) ) {
                        alt5=1;
                    }
                    switch (alt5) {
                        case 1 :
                            // CongerCQL.g:82:3: opt_group_by_clause
                            {
                            pushFollow(FOLLOW_opt_group_by_clause_in_sfw_block484);
                            opt_group_by_clause33=opt_group_by_clause();

                            state._fsp--;
                            if (state.failed) return retval;
                            if ( state.backtracking==0 ) stream_opt_group_by_clause.add(opt_group_by_clause33.getTree());

                            }
                            break;

                    }


                    // CongerCQL.g:82:24: ( opt_having_clause )?
                    int alt6=2;
                    int LA6_0 = input.LA(1);

                    if ( (LA6_0==KW_HAVING) ) {
                        alt6=1;
                    }
                    switch (alt6) {
                        case 1 :
                            // CongerCQL.g:82:24: opt_having_clause
                            {
                            pushFollow(FOLLOW_opt_having_clause_in_sfw_block487);
                            opt_having_clause34=opt_having_clause();

                            state._fsp--;
                            if (state.failed) return retval;
                            if ( state.backtracking==0 ) stream_opt_having_clause.add(opt_having_clause34.getTree());

                            }
                            break;

                    }


                    }
                    break;
                case 2 :
                    // CongerCQL.g:83:7: order_by_clause
                    {
                    pushFollow(FOLLOW_order_by_clause_in_sfw_block496);
                    order_by_clause35=order_by_clause();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_order_by_clause.add(order_by_clause35.getTree());

                    }
                    break;
                case 3 :
                    // CongerCQL.g:84:7: opt_where_clause ( opt_group_by_clause )? ( opt_having_clause )?
                    {
                    pushFollow(FOLLOW_opt_where_clause_in_sfw_block504);
                    opt_where_clause36=opt_where_clause();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_opt_where_clause.add(opt_where_clause36.getTree());

                    // CongerCQL.g:84:24: ( opt_group_by_clause )?
                    int alt7=2;
                    int LA7_0 = input.LA(1);

                    if ( (LA7_0==KW_GROUP) ) {
                        alt7=1;
                    }
                    switch (alt7) {
                        case 1 :
                            // CongerCQL.g:84:24: opt_group_by_clause
                            {
                            pushFollow(FOLLOW_opt_group_by_clause_in_sfw_block506);
                            opt_group_by_clause37=opt_group_by_clause();

                            state._fsp--;
                            if (state.failed) return retval;
                            if ( state.backtracking==0 ) stream_opt_group_by_clause.add(opt_group_by_clause37.getTree());

                            }
                            break;

                    }


                    // CongerCQL.g:84:45: ( opt_having_clause )?
                    int alt8=2;
                    int LA8_0 = input.LA(1);

                    if ( (LA8_0==KW_HAVING) ) {
                        alt8=1;
                    }
                    switch (alt8) {
                        case 1 :
                            // CongerCQL.g:84:45: opt_having_clause
                            {
                            pushFollow(FOLLOW_opt_having_clause_in_sfw_block509);
                            opt_having_clause38=opt_having_clause();

                            state._fsp--;
                            if (state.failed) return retval;
                            if ( state.backtracking==0 ) stream_opt_having_clause.add(opt_having_clause38.getTree());

                            }
                            break;

                    }


                    }
                    break;

            }


            // AST REWRITE
            // elements: order_by_clause, opt_where_clause, opt_group_by_clause, select_clause, from_clause, opt_having_clause
            // token labels: 
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 86:9: -> ^( TOK_SFW select_clause from_clause ( order_by_clause )? ( opt_where_clause )? ( opt_group_by_clause )? ( opt_having_clause )? )
            {
                // CongerCQL.g:86:12: ^( TOK_SFW select_clause from_clause ( order_by_clause )? ( opt_where_clause )? ( opt_group_by_clause )? ( opt_having_clause )? )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_SFW, "TOK_SFW")
                , root_1);

                adaptor.addChild(root_1, stream_select_clause.nextTree());

                adaptor.addChild(root_1, stream_from_clause.nextTree());

                // CongerCQL.g:86:48: ( order_by_clause )?
                if ( stream_order_by_clause.hasNext() ) {
                    adaptor.addChild(root_1, stream_order_by_clause.nextTree());

                }
                stream_order_by_clause.reset();

                // CongerCQL.g:86:65: ( opt_where_clause )?
                if ( stream_opt_where_clause.hasNext() ) {
                    adaptor.addChild(root_1, stream_opt_where_clause.nextTree());

                }
                stream_opt_where_clause.reset();

                // CongerCQL.g:87:13: ( opt_group_by_clause )?
                if ( stream_opt_group_by_clause.hasNext() ) {
                    adaptor.addChild(root_1, stream_opt_group_by_clause.nextTree());

                }
                stream_opt_group_by_clause.reset();

                // CongerCQL.g:87:34: ( opt_having_clause )?
                if ( stream_opt_having_clause.hasNext() ) {
                    adaptor.addChild(root_1, stream_opt_having_clause.nextTree());

                }
                stream_opt_having_clause.reset();

                adaptor.addChild(root_0, root_1);
                }

            }


            retval.tree = root_0;
            }

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "sfw_block"


    public static class select_clause_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "select_clause"
    // CongerCQL.g:90:1: select_clause : KW_SELECT ( KW_DISTINCT )? ( STAR | non_mt_projterm_list ) -> ^( TOK_SELECT ( KW_DISTINCT )? ( STAR )? ( non_mt_projterm_list )? ) ;
    public final CongerCQLParser.select_clause_return select_clause() throws RecognitionException {
        CongerCQLParser.select_clause_return retval = new CongerCQLParser.select_clause_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token KW_SELECT39=null;
        Token KW_DISTINCT40=null;
        Token STAR41=null;
        CongerCQLParser.non_mt_projterm_list_return non_mt_projterm_list42 =null;


        CommonTree KW_SELECT39_tree=null;
        CommonTree KW_DISTINCT40_tree=null;
        CommonTree STAR41_tree=null;
        RewriteRuleTokenStream stream_KW_SELECT=new RewriteRuleTokenStream(adaptor,"token KW_SELECT");
        RewriteRuleTokenStream stream_STAR=new RewriteRuleTokenStream(adaptor,"token STAR");
        RewriteRuleTokenStream stream_KW_DISTINCT=new RewriteRuleTokenStream(adaptor,"token KW_DISTINCT");
        RewriteRuleSubtreeStream stream_non_mt_projterm_list=new RewriteRuleSubtreeStream(adaptor,"rule non_mt_projterm_list");
        try {
            // CongerCQL.g:91:2: ( KW_SELECT ( KW_DISTINCT )? ( STAR | non_mt_projterm_list ) -> ^( TOK_SELECT ( KW_DISTINCT )? ( STAR )? ( non_mt_projterm_list )? ) )
            // CongerCQL.g:91:4: KW_SELECT ( KW_DISTINCT )? ( STAR | non_mt_projterm_list )
            {
            KW_SELECT39=(Token)match(input,KW_SELECT,FOLLOW_KW_SELECT_in_select_clause568); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_KW_SELECT.add(KW_SELECT39);


            // CongerCQL.g:91:14: ( KW_DISTINCT )?
            int alt10=2;
            int LA10_0 = input.LA(1);

            if ( (LA10_0==KW_DISTINCT) ) {
                alt10=1;
            }
            switch (alt10) {
                case 1 :
                    // CongerCQL.g:91:14: KW_DISTINCT
                    {
                    KW_DISTINCT40=(Token)match(input,KW_DISTINCT,FOLLOW_KW_DISTINCT_in_select_clause570); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_KW_DISTINCT.add(KW_DISTINCT40);


                    }
                    break;

            }


            // CongerCQL.g:91:27: ( STAR | non_mt_projterm_list )
            int alt11=2;
            int LA11_0 = input.LA(1);

            if ( (LA11_0==STAR) ) {
                alt11=1;
            }
            else if ( ((LA11_0 >= Identifier && LA11_0 <= Integer)||LA11_0==KW_AVG||LA11_0==KW_COUNT||LA11_0==KW_DOUBLE||LA11_0==KW_ELEMENT_TIME||LA11_0==KW_FLOAT||LA11_0==KW_INTERVAL||LA11_0==KW_MAX||LA11_0==KW_MIN||LA11_0==KW_NULL||LA11_0==KW_SUM||LA11_0==LPAREN||LA11_0==StringLiteral) ) {
                alt11=2;
            }
            else {
                if (state.backtracking>0) {state.failed=true; return retval;}
                NoViableAltException nvae =
                    new NoViableAltException("", 11, 0, input);

                throw nvae;

            }
            switch (alt11) {
                case 1 :
                    // CongerCQL.g:91:28: STAR
                    {
                    STAR41=(Token)match(input,STAR,FOLLOW_STAR_in_select_clause574); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_STAR.add(STAR41);


                    }
                    break;
                case 2 :
                    // CongerCQL.g:91:35: non_mt_projterm_list
                    {
                    pushFollow(FOLLOW_non_mt_projterm_list_in_select_clause578);
                    non_mt_projterm_list42=non_mt_projterm_list();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_non_mt_projterm_list.add(non_mt_projterm_list42.getTree());

                    }
                    break;

            }


            // AST REWRITE
            // elements: KW_DISTINCT, non_mt_projterm_list, STAR
            // token labels: 
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 92:9: -> ^( TOK_SELECT ( KW_DISTINCT )? ( STAR )? ( non_mt_projterm_list )? )
            {
                // CongerCQL.g:92:12: ^( TOK_SELECT ( KW_DISTINCT )? ( STAR )? ( non_mt_projterm_list )? )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_SELECT, "TOK_SELECT")
                , root_1);

                // CongerCQL.g:92:25: ( KW_DISTINCT )?
                if ( stream_KW_DISTINCT.hasNext() ) {
                    adaptor.addChild(root_1, 
                    stream_KW_DISTINCT.nextNode()
                    );

                }
                stream_KW_DISTINCT.reset();

                // CongerCQL.g:92:38: ( STAR )?
                if ( stream_STAR.hasNext() ) {
                    adaptor.addChild(root_1, 
                    stream_STAR.nextNode()
                    );

                }
                stream_STAR.reset();

                // CongerCQL.g:92:44: ( non_mt_projterm_list )?
                if ( stream_non_mt_projterm_list.hasNext() ) {
                    adaptor.addChild(root_1, stream_non_mt_projterm_list.nextTree());

                }
                stream_non_mt_projterm_list.reset();

                adaptor.addChild(root_0, root_1);
                }

            }


            retval.tree = root_0;
            }

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "select_clause"


    public static class non_mt_projterm_list_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "non_mt_projterm_list"
    // CongerCQL.g:95:1: non_mt_projterm_list : projterm ( options {greedy=true; } : COMMA projterm )* -> ^( TOK_PROJTERM_LIST ( projterm )+ ) ;
    public final CongerCQLParser.non_mt_projterm_list_return non_mt_projterm_list() throws RecognitionException {
        CongerCQLParser.non_mt_projterm_list_return retval = new CongerCQLParser.non_mt_projterm_list_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token COMMA44=null;
        CongerCQLParser.projterm_return projterm43 =null;

        CongerCQLParser.projterm_return projterm45 =null;


        CommonTree COMMA44_tree=null;
        RewriteRuleTokenStream stream_COMMA=new RewriteRuleTokenStream(adaptor,"token COMMA");
        RewriteRuleSubtreeStream stream_projterm=new RewriteRuleSubtreeStream(adaptor,"rule projterm");
        try {
            // CongerCQL.g:96:2: ( projterm ( options {greedy=true; } : COMMA projterm )* -> ^( TOK_PROJTERM_LIST ( projterm )+ ) )
            // CongerCQL.g:96:4: projterm ( options {greedy=true; } : COMMA projterm )*
            {
            pushFollow(FOLLOW_projterm_in_non_mt_projterm_list613);
            projterm43=projterm();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_projterm.add(projterm43.getTree());

            // CongerCQL.g:96:13: ( options {greedy=true; } : COMMA projterm )*
            loop12:
            do {
                int alt12=2;
                int LA12_0 = input.LA(1);

                if ( (LA12_0==COMMA) ) {
                    alt12=1;
                }


                switch (alt12) {
            	case 1 :
            	    // CongerCQL.g:96:37: COMMA projterm
            	    {
            	    COMMA44=(Token)match(input,COMMA,FOLLOW_COMMA_in_non_mt_projterm_list624); if (state.failed) return retval; 
            	    if ( state.backtracking==0 ) stream_COMMA.add(COMMA44);


            	    pushFollow(FOLLOW_projterm_in_non_mt_projterm_list626);
            	    projterm45=projterm();

            	    state._fsp--;
            	    if (state.failed) return retval;
            	    if ( state.backtracking==0 ) stream_projterm.add(projterm45.getTree());

            	    }
            	    break;

            	default :
            	    break loop12;
                }
            } while (true);


            // AST REWRITE
            // elements: projterm
            // token labels: 
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 97:9: -> ^( TOK_PROJTERM_LIST ( projterm )+ )
            {
                // CongerCQL.g:97:12: ^( TOK_PROJTERM_LIST ( projterm )+ )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_PROJTERM_LIST, "TOK_PROJTERM_LIST")
                , root_1);

                if ( !(stream_projterm.hasNext()) ) {
                    throw new RewriteEarlyExitException();
                }
                while ( stream_projterm.hasNext() ) {
                    adaptor.addChild(root_1, stream_projterm.nextTree());

                }
                stream_projterm.reset();

                adaptor.addChild(root_0, root_1);
                }

            }


            retval.tree = root_0;
            }

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "non_mt_projterm_list"


    public static class projterm_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "projterm"
    // CongerCQL.g:100:1: projterm : arith_expr ( KW_AS alias= Identifier )? -> ^( TOK_PROJTERM arith_expr ( KW_AS )? ( $alias)? ) ;
    public final CongerCQLParser.projterm_return projterm() throws RecognitionException {
        CongerCQLParser.projterm_return retval = new CongerCQLParser.projterm_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token alias=null;
        Token KW_AS47=null;
        CongerCQLParser.arith_expr_return arith_expr46 =null;


        CommonTree alias_tree=null;
        CommonTree KW_AS47_tree=null;
        RewriteRuleTokenStream stream_KW_AS=new RewriteRuleTokenStream(adaptor,"token KW_AS");
        RewriteRuleTokenStream stream_Identifier=new RewriteRuleTokenStream(adaptor,"token Identifier");
        RewriteRuleSubtreeStream stream_arith_expr=new RewriteRuleSubtreeStream(adaptor,"rule arith_expr");
        try {
            // CongerCQL.g:101:2: ( arith_expr ( KW_AS alias= Identifier )? -> ^( TOK_PROJTERM arith_expr ( KW_AS )? ( $alias)? ) )
            // CongerCQL.g:101:4: arith_expr ( KW_AS alias= Identifier )?
            {
            pushFollow(FOLLOW_arith_expr_in_projterm656);
            arith_expr46=arith_expr();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_arith_expr.add(arith_expr46.getTree());

            // CongerCQL.g:101:15: ( KW_AS alias= Identifier )?
            int alt13=2;
            int LA13_0 = input.LA(1);

            if ( (LA13_0==KW_AS) ) {
                alt13=1;
            }
            switch (alt13) {
                case 1 :
                    // CongerCQL.g:101:16: KW_AS alias= Identifier
                    {
                    KW_AS47=(Token)match(input,KW_AS,FOLLOW_KW_AS_in_projterm659); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_KW_AS.add(KW_AS47);


                    alias=(Token)match(input,Identifier,FOLLOW_Identifier_in_projterm663); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_Identifier.add(alias);


                    }
                    break;

            }


            // AST REWRITE
            // elements: arith_expr, KW_AS, alias
            // token labels: alias
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleTokenStream stream_alias=new RewriteRuleTokenStream(adaptor,"token alias",alias);
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 102:9: -> ^( TOK_PROJTERM arith_expr ( KW_AS )? ( $alias)? )
            {
                // CongerCQL.g:102:12: ^( TOK_PROJTERM arith_expr ( KW_AS )? ( $alias)? )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_PROJTERM, "TOK_PROJTERM")
                , root_1);

                adaptor.addChild(root_1, stream_arith_expr.nextTree());

                // CongerCQL.g:102:38: ( KW_AS )?
                if ( stream_KW_AS.hasNext() ) {
                    adaptor.addChild(root_1, 
                    stream_KW_AS.nextNode()
                    );

                }
                stream_KW_AS.reset();

                // CongerCQL.g:102:46: ( $alias)?
                if ( stream_alias.hasNext() ) {
                    adaptor.addChild(root_1, stream_alias.nextNode());

                }
                stream_alias.reset();

                adaptor.addChild(root_0, root_1);
                }

            }


            retval.tree = root_0;
            }

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "projterm"


    public static class arith_expr_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "arith_expr"
    // CongerCQL.g:105:1: arith_expr : arith_expr_main ( arith_expr_operator arith_expr_main )? -> ^( TOK_ARITH_EXPR arith_expr_main ( arith_expr_operator )? ( arith_expr_main )? ) ;
    public final CongerCQLParser.arith_expr_return arith_expr() throws RecognitionException {
        CongerCQLParser.arith_expr_return retval = new CongerCQLParser.arith_expr_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        CongerCQLParser.arith_expr_main_return arith_expr_main48 =null;

        CongerCQLParser.arith_expr_operator_return arith_expr_operator49 =null;

        CongerCQLParser.arith_expr_main_return arith_expr_main50 =null;


        RewriteRuleSubtreeStream stream_arith_expr_operator=new RewriteRuleSubtreeStream(adaptor,"rule arith_expr_operator");
        RewriteRuleSubtreeStream stream_arith_expr_main=new RewriteRuleSubtreeStream(adaptor,"rule arith_expr_main");
        try {
            // CongerCQL.g:106:2: ( arith_expr_main ( arith_expr_operator arith_expr_main )? -> ^( TOK_ARITH_EXPR arith_expr_main ( arith_expr_operator )? ( arith_expr_main )? ) )
            // CongerCQL.g:106:4: arith_expr_main ( arith_expr_operator arith_expr_main )?
            {
            pushFollow(FOLLOW_arith_expr_main_in_arith_expr700);
            arith_expr_main48=arith_expr_main();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_arith_expr_main.add(arith_expr_main48.getTree());

            // CongerCQL.g:106:20: ( arith_expr_operator arith_expr_main )?
            int alt14=2;
            int LA14_0 = input.LA(1);

            if ( (LA14_0==CONCATENATE||LA14_0==DIVIDE||LA14_0==MINUS||LA14_0==PLUS||LA14_0==STAR) ) {
                alt14=1;
            }
            switch (alt14) {
                case 1 :
                    // CongerCQL.g:106:21: arith_expr_operator arith_expr_main
                    {
                    pushFollow(FOLLOW_arith_expr_operator_in_arith_expr703);
                    arith_expr_operator49=arith_expr_operator();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_arith_expr_operator.add(arith_expr_operator49.getTree());

                    pushFollow(FOLLOW_arith_expr_main_in_arith_expr705);
                    arith_expr_main50=arith_expr_main();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_arith_expr_main.add(arith_expr_main50.getTree());

                    }
                    break;

            }


            // AST REWRITE
            // elements: arith_expr_main, arith_expr_main, arith_expr_operator
            // token labels: 
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 108:9: -> ^( TOK_ARITH_EXPR arith_expr_main ( arith_expr_operator )? ( arith_expr_main )? )
            {
                // CongerCQL.g:108:12: ^( TOK_ARITH_EXPR arith_expr_main ( arith_expr_operator )? ( arith_expr_main )? )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_ARITH_EXPR, "TOK_ARITH_EXPR")
                , root_1);

                adaptor.addChild(root_1, stream_arith_expr_main.nextTree());

                // CongerCQL.g:108:45: ( arith_expr_operator )?
                if ( stream_arith_expr_operator.hasNext() ) {
                    adaptor.addChild(root_1, stream_arith_expr_operator.nextTree());

                }
                stream_arith_expr_operator.reset();

                // CongerCQL.g:108:66: ( arith_expr_main )?
                if ( stream_arith_expr_main.hasNext() ) {
                    adaptor.addChild(root_1, stream_arith_expr_main.nextTree());

                }
                stream_arith_expr_main.reset();

                adaptor.addChild(root_0, root_1);
                }

            }


            retval.tree = root_0;
            }

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "arith_expr"


    public static class arith_expr_operator_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "arith_expr_operator"
    // CongerCQL.g:111:1: arith_expr_operator : ( PLUS | MINUS | STAR | DIVIDE | CONCATENATE );
    public final CongerCQLParser.arith_expr_operator_return arith_expr_operator() throws RecognitionException {
        CongerCQLParser.arith_expr_operator_return retval = new CongerCQLParser.arith_expr_operator_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token set51=null;

        CommonTree set51_tree=null;

        try {
            // CongerCQL.g:112:5: ( PLUS | MINUS | STAR | DIVIDE | CONCATENATE )
            // CongerCQL.g:
            {
            root_0 = (CommonTree)adaptor.nil();


            set51=(Token)input.LT(1);

            if ( input.LA(1)==CONCATENATE||input.LA(1)==DIVIDE||input.LA(1)==MINUS||input.LA(1)==PLUS||input.LA(1)==STAR ) {
                input.consume();
                if ( state.backtracking==0 ) adaptor.addChild(root_0, 
                (CommonTree)adaptor.create(set51)
                );
                state.errorRecovery=false;
                state.failed=false;
            }
            else {
                if (state.backtracking>0) {state.failed=true; return retval;}
                MismatchedSetException mse = new MismatchedSetException(null,input);
                throw mse;
            }


            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "arith_expr_operator"


    public static class arith_expr_main_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "arith_expr_main"
    // CongerCQL.g:115:1: arith_expr_main : ( attr | const_value | aggr_expr | aggr_distinct_expr | func_expr | LPAREN arith_expr RPAREN );
    public final CongerCQLParser.arith_expr_main_return arith_expr_main() throws RecognitionException {
        CongerCQLParser.arith_expr_main_return retval = new CongerCQLParser.arith_expr_main_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token LPAREN57=null;
        Token RPAREN59=null;
        CongerCQLParser.attr_return attr52 =null;

        CongerCQLParser.const_value_return const_value53 =null;

        CongerCQLParser.aggr_expr_return aggr_expr54 =null;

        CongerCQLParser.aggr_distinct_expr_return aggr_distinct_expr55 =null;

        CongerCQLParser.func_expr_return func_expr56 =null;

        CongerCQLParser.arith_expr_return arith_expr58 =null;


        CommonTree LPAREN57_tree=null;
        CommonTree RPAREN59_tree=null;

        try {
            // CongerCQL.g:116:2: ( attr | const_value | aggr_expr | aggr_distinct_expr | func_expr | LPAREN arith_expr RPAREN )
            int alt15=6;
            switch ( input.LA(1) ) {
            case Identifier:
                {
                int LA15_1 = input.LA(2);

                if ( (LA15_1==EOF||LA15_1==COMMA||LA15_1==CONCATENATE||LA15_1==DIVIDE||LA15_1==DOT||LA15_1==EQUAL||(LA15_1 >= GREATERTHAN && LA15_1 <= GREATERTHANOREQUALTO)||(LA15_1 >= KW_AND && LA15_1 <= KW_AS)||LA15_1==KW_BETWEEN||LA15_1==KW_FROM||(LA15_1 >= KW_GROUP && LA15_1 <= KW_HAVING)||(LA15_1 >= KW_OR && LA15_1 <= KW_ORDER)||LA15_1==KW_WHERE||LA15_1==KW_XOR||(LA15_1 >= LESSTHAN && LA15_1 <= LESSTHANOREQUALTO)||LA15_1==MINUS||LA15_1==NOTEQUAL||LA15_1==PLUS||LA15_1==RPAREN||LA15_1==STAR) ) {
                    alt15=1;
                }
                else if ( (LA15_1==LPAREN) ) {
                    alt15=5;
                }
                else {
                    if (state.backtracking>0) {state.failed=true; return retval;}
                    NoViableAltException nvae =
                        new NoViableAltException("", 15, 1, input);

                    throw nvae;

                }
                }
                break;
            case KW_ELEMENT_TIME:
                {
                alt15=1;
                }
                break;
            case Integer:
            case KW_DOUBLE:
            case KW_FLOAT:
            case KW_INTERVAL:
            case KW_NULL:
            case StringLiteral:
                {
                alt15=2;
                }
                break;
            case KW_COUNT:
                {
                int LA15_4 = input.LA(2);

                if ( (LA15_4==LPAREN) ) {
                    int LA15_11 = input.LA(3);

                    if ( (LA15_11==KW_DISTINCT) ) {
                        alt15=4;
                    }
                    else if ( ((LA15_11 >= Identifier && LA15_11 <= Integer)||LA15_11==KW_AVG||LA15_11==KW_COUNT||LA15_11==KW_DOUBLE||LA15_11==KW_ELEMENT_TIME||LA15_11==KW_FLOAT||LA15_11==KW_INTERVAL||LA15_11==KW_MAX||LA15_11==KW_MIN||LA15_11==KW_NULL||LA15_11==KW_SUM||LA15_11==LPAREN||(LA15_11 >= STAR && LA15_11 <= StringLiteral)) ) {
                        alt15=3;
                    }
                    else {
                        if (state.backtracking>0) {state.failed=true; return retval;}
                        NoViableAltException nvae =
                            new NoViableAltException("", 15, 11, input);

                        throw nvae;

                    }
                }
                else {
                    if (state.backtracking>0) {state.failed=true; return retval;}
                    NoViableAltException nvae =
                        new NoViableAltException("", 15, 4, input);

                    throw nvae;

                }
                }
                break;
            case KW_SUM:
                {
                int LA15_5 = input.LA(2);

                if ( (LA15_5==LPAREN) ) {
                    int LA15_12 = input.LA(3);

                    if ( (LA15_12==KW_DISTINCT) ) {
                        alt15=4;
                    }
                    else if ( ((LA15_12 >= Identifier && LA15_12 <= Integer)||LA15_12==KW_AVG||LA15_12==KW_COUNT||LA15_12==KW_DOUBLE||LA15_12==KW_ELEMENT_TIME||LA15_12==KW_FLOAT||LA15_12==KW_INTERVAL||LA15_12==KW_MAX||LA15_12==KW_MIN||LA15_12==KW_NULL||LA15_12==KW_SUM||LA15_12==LPAREN||LA15_12==StringLiteral) ) {
                        alt15=3;
                    }
                    else {
                        if (state.backtracking>0) {state.failed=true; return retval;}
                        NoViableAltException nvae =
                            new NoViableAltException("", 15, 12, input);

                        throw nvae;

                    }
                }
                else {
                    if (state.backtracking>0) {state.failed=true; return retval;}
                    NoViableAltException nvae =
                        new NoViableAltException("", 15, 5, input);

                    throw nvae;

                }
                }
                break;
            case KW_AVG:
                {
                int LA15_6 = input.LA(2);

                if ( (LA15_6==LPAREN) ) {
                    int LA15_12 = input.LA(3);

                    if ( (LA15_12==KW_DISTINCT) ) {
                        alt15=4;
                    }
                    else if ( ((LA15_12 >= Identifier && LA15_12 <= Integer)||LA15_12==KW_AVG||LA15_12==KW_COUNT||LA15_12==KW_DOUBLE||LA15_12==KW_ELEMENT_TIME||LA15_12==KW_FLOAT||LA15_12==KW_INTERVAL||LA15_12==KW_MAX||LA15_12==KW_MIN||LA15_12==KW_NULL||LA15_12==KW_SUM||LA15_12==LPAREN||LA15_12==StringLiteral) ) {
                        alt15=3;
                    }
                    else {
                        if (state.backtracking>0) {state.failed=true; return retval;}
                        NoViableAltException nvae =
                            new NoViableAltException("", 15, 12, input);

                        throw nvae;

                    }
                }
                else {
                    if (state.backtracking>0) {state.failed=true; return retval;}
                    NoViableAltException nvae =
                        new NoViableAltException("", 15, 6, input);

                    throw nvae;

                }
                }
                break;
            case KW_MAX:
                {
                int LA15_7 = input.LA(2);

                if ( (LA15_7==LPAREN) ) {
                    int LA15_13 = input.LA(3);

                    if ( (LA15_13==KW_DISTINCT) ) {
                        alt15=4;
                    }
                    else if ( ((LA15_13 >= Identifier && LA15_13 <= Integer)||LA15_13==KW_AVG||LA15_13==KW_COUNT||LA15_13==KW_DOUBLE||LA15_13==KW_ELEMENT_TIME||LA15_13==KW_FLOAT||LA15_13==KW_INTERVAL||LA15_13==KW_MAX||LA15_13==KW_MIN||LA15_13==KW_NULL||LA15_13==KW_SUM||LA15_13==LPAREN||LA15_13==StringLiteral) ) {
                        alt15=3;
                    }
                    else {
                        if (state.backtracking>0) {state.failed=true; return retval;}
                        NoViableAltException nvae =
                            new NoViableAltException("", 15, 13, input);

                        throw nvae;

                    }
                }
                else {
                    if (state.backtracking>0) {state.failed=true; return retval;}
                    NoViableAltException nvae =
                        new NoViableAltException("", 15, 7, input);

                    throw nvae;

                }
                }
                break;
            case KW_MIN:
                {
                int LA15_8 = input.LA(2);

                if ( (LA15_8==LPAREN) ) {
                    int LA15_13 = input.LA(3);

                    if ( (LA15_13==KW_DISTINCT) ) {
                        alt15=4;
                    }
                    else if ( ((LA15_13 >= Identifier && LA15_13 <= Integer)||LA15_13==KW_AVG||LA15_13==KW_COUNT||LA15_13==KW_DOUBLE||LA15_13==KW_ELEMENT_TIME||LA15_13==KW_FLOAT||LA15_13==KW_INTERVAL||LA15_13==KW_MAX||LA15_13==KW_MIN||LA15_13==KW_NULL||LA15_13==KW_SUM||LA15_13==LPAREN||LA15_13==StringLiteral) ) {
                        alt15=3;
                    }
                    else {
                        if (state.backtracking>0) {state.failed=true; return retval;}
                        NoViableAltException nvae =
                            new NoViableAltException("", 15, 13, input);

                        throw nvae;

                    }
                }
                else {
                    if (state.backtracking>0) {state.failed=true; return retval;}
                    NoViableAltException nvae =
                        new NoViableAltException("", 15, 8, input);

                    throw nvae;

                }
                }
                break;
            case LPAREN:
                {
                alt15=6;
                }
                break;
            default:
                if (state.backtracking>0) {state.failed=true; return retval;}
                NoViableAltException nvae =
                    new NoViableAltException("", 15, 0, input);

                throw nvae;

            }

            switch (alt15) {
                case 1 :
                    // CongerCQL.g:116:4: attr
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    pushFollow(FOLLOW_attr_in_arith_expr_main782);
                    attr52=attr();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, attr52.getTree());

                    }
                    break;
                case 2 :
                    // CongerCQL.g:117:4: const_value
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    pushFollow(FOLLOW_const_value_in_arith_expr_main787);
                    const_value53=const_value();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, const_value53.getTree());

                    }
                    break;
                case 3 :
                    // CongerCQL.g:118:4: aggr_expr
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    pushFollow(FOLLOW_aggr_expr_in_arith_expr_main792);
                    aggr_expr54=aggr_expr();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, aggr_expr54.getTree());

                    }
                    break;
                case 4 :
                    // CongerCQL.g:119:4: aggr_distinct_expr
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    pushFollow(FOLLOW_aggr_distinct_expr_in_arith_expr_main797);
                    aggr_distinct_expr55=aggr_distinct_expr();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, aggr_distinct_expr55.getTree());

                    }
                    break;
                case 5 :
                    // CongerCQL.g:120:4: func_expr
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    pushFollow(FOLLOW_func_expr_in_arith_expr_main802);
                    func_expr56=func_expr();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, func_expr56.getTree());

                    }
                    break;
                case 6 :
                    // CongerCQL.g:123:4: LPAREN arith_expr RPAREN
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    LPAREN57=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_arith_expr_main814); if (state.failed) return retval;
                    if ( state.backtracking==0 ) {
                    LPAREN57_tree = 
                    (CommonTree)adaptor.create(LPAREN57)
                    ;
                    adaptor.addChild(root_0, LPAREN57_tree);
                    }

                    pushFollow(FOLLOW_arith_expr_in_arith_expr_main816);
                    arith_expr58=arith_expr();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, arith_expr58.getTree());

                    RPAREN59=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_arith_expr_main818); if (state.failed) return retval;
                    if ( state.backtracking==0 ) {
                    RPAREN59_tree = 
                    (CommonTree)adaptor.create(RPAREN59)
                    ;
                    adaptor.addChild(root_0, RPAREN59_tree);
                    }

                    }
                    break;

            }
            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "arith_expr_main"


    public static class order_by_clause_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "order_by_clause"
    // CongerCQL.g:126:1: order_by_clause : KW_ORDER KW_BY order_by_list -> ^( TOK_ORDER_BY order_by_list ) ;
    public final CongerCQLParser.order_by_clause_return order_by_clause() throws RecognitionException {
        CongerCQLParser.order_by_clause_return retval = new CongerCQLParser.order_by_clause_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token KW_ORDER60=null;
        Token KW_BY61=null;
        CongerCQLParser.order_by_list_return order_by_list62 =null;


        CommonTree KW_ORDER60_tree=null;
        CommonTree KW_BY61_tree=null;
        RewriteRuleTokenStream stream_KW_ORDER=new RewriteRuleTokenStream(adaptor,"token KW_ORDER");
        RewriteRuleTokenStream stream_KW_BY=new RewriteRuleTokenStream(adaptor,"token KW_BY");
        RewriteRuleSubtreeStream stream_order_by_list=new RewriteRuleSubtreeStream(adaptor,"rule order_by_list");
        try {
            // CongerCQL.g:127:2: ( KW_ORDER KW_BY order_by_list -> ^( TOK_ORDER_BY order_by_list ) )
            // CongerCQL.g:127:4: KW_ORDER KW_BY order_by_list
            {
            KW_ORDER60=(Token)match(input,KW_ORDER,FOLLOW_KW_ORDER_in_order_by_clause830); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_KW_ORDER.add(KW_ORDER60);


            KW_BY61=(Token)match(input,KW_BY,FOLLOW_KW_BY_in_order_by_clause832); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_KW_BY.add(KW_BY61);


            pushFollow(FOLLOW_order_by_list_in_order_by_clause834);
            order_by_list62=order_by_list();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_order_by_list.add(order_by_list62.getTree());

            // AST REWRITE
            // elements: order_by_list
            // token labels: 
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 128:9: -> ^( TOK_ORDER_BY order_by_list )
            {
                // CongerCQL.g:128:12: ^( TOK_ORDER_BY order_by_list )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_ORDER_BY, "TOK_ORDER_BY")
                , root_1);

                adaptor.addChild(root_1, stream_order_by_list.nextTree());

                adaptor.addChild(root_0, root_1);
                }

            }


            retval.tree = root_0;
            }

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "order_by_clause"


    public static class order_by_list_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "order_by_list"
    // CongerCQL.g:131:1: order_by_list : orderterm ( options {greedy=true; } : COMMA order_by_list )* -> ( orderterm )* ;
    public final CongerCQLParser.order_by_list_return order_by_list() throws RecognitionException {
        CongerCQLParser.order_by_list_return retval = new CongerCQLParser.order_by_list_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token COMMA64=null;
        CongerCQLParser.orderterm_return orderterm63 =null;

        CongerCQLParser.order_by_list_return order_by_list65 =null;


        CommonTree COMMA64_tree=null;
        RewriteRuleTokenStream stream_COMMA=new RewriteRuleTokenStream(adaptor,"token COMMA");
        RewriteRuleSubtreeStream stream_orderterm=new RewriteRuleSubtreeStream(adaptor,"rule orderterm");
        RewriteRuleSubtreeStream stream_order_by_list=new RewriteRuleSubtreeStream(adaptor,"rule order_by_list");
        try {
            // CongerCQL.g:132:2: ( orderterm ( options {greedy=true; } : COMMA order_by_list )* -> ( orderterm )* )
            // CongerCQL.g:132:4: orderterm ( options {greedy=true; } : COMMA order_by_list )*
            {
            pushFollow(FOLLOW_orderterm_in_order_by_list861);
            orderterm63=orderterm();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_orderterm.add(orderterm63.getTree());

            // CongerCQL.g:132:14: ( options {greedy=true; } : COMMA order_by_list )*
            loop16:
            do {
                int alt16=2;
                int LA16_0 = input.LA(1);

                if ( (LA16_0==COMMA) ) {
                    alt16=1;
                }


                switch (alt16) {
            	case 1 :
            	    // CongerCQL.g:132:38: COMMA order_by_list
            	    {
            	    COMMA64=(Token)match(input,COMMA,FOLLOW_COMMA_in_order_by_list872); if (state.failed) return retval; 
            	    if ( state.backtracking==0 ) stream_COMMA.add(COMMA64);


            	    pushFollow(FOLLOW_order_by_list_in_order_by_list874);
            	    order_by_list65=order_by_list();

            	    state._fsp--;
            	    if (state.failed) return retval;
            	    if ( state.backtracking==0 ) stream_order_by_list.add(order_by_list65.getTree());

            	    }
            	    break;

            	default :
            	    break loop16;
                }
            } while (true);


            // AST REWRITE
            // elements: orderterm
            // token labels: 
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 133:9: -> ( orderterm )*
            {
                // CongerCQL.g:133:12: ( orderterm )*
                while ( stream_orderterm.hasNext() ) {
                    adaptor.addChild(root_0, stream_orderterm.nextTree());

                }
                stream_orderterm.reset();

            }


            retval.tree = root_0;
            }

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "order_by_list"


    public static class orderterm_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "orderterm"
    // CongerCQL.g:136:1: orderterm : order_expr ( asc_desc )? ( null_spec )? -> ^( TOK_ORDER_EXPR order_expr ( asc_desc )? ( null_spec )? ) ;
    public final CongerCQLParser.orderterm_return orderterm() throws RecognitionException {
        CongerCQLParser.orderterm_return retval = new CongerCQLParser.orderterm_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        CongerCQLParser.order_expr_return order_expr66 =null;

        CongerCQLParser.asc_desc_return asc_desc67 =null;

        CongerCQLParser.null_spec_return null_spec68 =null;


        RewriteRuleSubtreeStream stream_order_expr=new RewriteRuleSubtreeStream(adaptor,"rule order_expr");
        RewriteRuleSubtreeStream stream_asc_desc=new RewriteRuleSubtreeStream(adaptor,"rule asc_desc");
        RewriteRuleSubtreeStream stream_null_spec=new RewriteRuleSubtreeStream(adaptor,"rule null_spec");
        try {
            // CongerCQL.g:137:2: ( order_expr ( asc_desc )? ( null_spec )? -> ^( TOK_ORDER_EXPR order_expr ( asc_desc )? ( null_spec )? ) )
            // CongerCQL.g:137:4: order_expr ( asc_desc )? ( null_spec )?
            {
            pushFollow(FOLLOW_order_expr_in_orderterm900);
            order_expr66=order_expr();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_order_expr.add(order_expr66.getTree());

            // CongerCQL.g:137:15: ( asc_desc )?
            int alt17=2;
            int LA17_0 = input.LA(1);

            if ( (LA17_0==KW_ASC||LA17_0==KW_DESC) ) {
                alt17=1;
            }
            switch (alt17) {
                case 1 :
                    // CongerCQL.g:137:15: asc_desc
                    {
                    pushFollow(FOLLOW_asc_desc_in_orderterm902);
                    asc_desc67=asc_desc();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_asc_desc.add(asc_desc67.getTree());

                    }
                    break;

            }


            // CongerCQL.g:137:25: ( null_spec )?
            int alt18=2;
            int LA18_0 = input.LA(1);

            if ( (LA18_0==KW_NULLS) ) {
                alt18=1;
            }
            switch (alt18) {
                case 1 :
                    // CongerCQL.g:137:25: null_spec
                    {
                    pushFollow(FOLLOW_null_spec_in_orderterm905);
                    null_spec68=null_spec();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_null_spec.add(null_spec68.getTree());

                    }
                    break;

            }


            // AST REWRITE
            // elements: null_spec, asc_desc, order_expr
            // token labels: 
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 138:9: -> ^( TOK_ORDER_EXPR order_expr ( asc_desc )? ( null_spec )? )
            {
                // CongerCQL.g:138:12: ^( TOK_ORDER_EXPR order_expr ( asc_desc )? ( null_spec )? )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_ORDER_EXPR, "TOK_ORDER_EXPR")
                , root_1);

                adaptor.addChild(root_1, stream_order_expr.nextTree());

                // CongerCQL.g:138:40: ( asc_desc )?
                if ( stream_asc_desc.hasNext() ) {
                    adaptor.addChild(root_1, stream_asc_desc.nextTree());

                }
                stream_asc_desc.reset();

                // CongerCQL.g:138:50: ( null_spec )?
                if ( stream_null_spec.hasNext() ) {
                    adaptor.addChild(root_1, stream_null_spec.nextTree());

                }
                stream_null_spec.reset();

                adaptor.addChild(root_0, root_1);
                }

            }


            retval.tree = root_0;
            }

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "orderterm"


    public static class asc_desc_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "asc_desc"
    // CongerCQL.g:141:1: asc_desc : ( KW_ASC | KW_DESC );
    public final CongerCQLParser.asc_desc_return asc_desc() throws RecognitionException {
        CongerCQLParser.asc_desc_return retval = new CongerCQLParser.asc_desc_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token set69=null;

        CommonTree set69_tree=null;

        try {
            // CongerCQL.g:142:2: ( KW_ASC | KW_DESC )
            // CongerCQL.g:
            {
            root_0 = (CommonTree)adaptor.nil();


            set69=(Token)input.LT(1);

            if ( input.LA(1)==KW_ASC||input.LA(1)==KW_DESC ) {
                input.consume();
                if ( state.backtracking==0 ) adaptor.addChild(root_0, 
                (CommonTree)adaptor.create(set69)
                );
                state.errorRecovery=false;
                state.failed=false;
            }
            else {
                if (state.backtracking>0) {state.failed=true; return retval;}
                MismatchedSetException mse = new MismatchedSetException(null,input);
                throw mse;
            }


            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "asc_desc"


    public static class null_spec_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "null_spec"
    // CongerCQL.g:146:1: null_spec : KW_NULLS ( KW_FIRST | KW_LAST ) ;
    public final CongerCQLParser.null_spec_return null_spec() throws RecognitionException {
        CongerCQLParser.null_spec_return retval = new CongerCQLParser.null_spec_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token KW_NULLS70=null;
        Token set71=null;

        CommonTree KW_NULLS70_tree=null;
        CommonTree set71_tree=null;

        try {
            // CongerCQL.g:147:2: ( KW_NULLS ( KW_FIRST | KW_LAST ) )
            // CongerCQL.g:147:4: KW_NULLS ( KW_FIRST | KW_LAST )
            {
            root_0 = (CommonTree)adaptor.nil();


            KW_NULLS70=(Token)match(input,KW_NULLS,FOLLOW_KW_NULLS_in_null_spec955); if (state.failed) return retval;
            if ( state.backtracking==0 ) {
            KW_NULLS70_tree = 
            (CommonTree)adaptor.create(KW_NULLS70)
            ;
            adaptor.addChild(root_0, KW_NULLS70_tree);
            }

            set71=(Token)input.LT(1);

            if ( input.LA(1)==KW_FIRST||input.LA(1)==KW_LAST ) {
                input.consume();
                if ( state.backtracking==0 ) adaptor.addChild(root_0, 
                (CommonTree)adaptor.create(set71)
                );
                state.errorRecovery=false;
                state.failed=false;
            }
            else {
                if (state.backtracking>0) {state.failed=true; return retval;}
                MismatchedSetException mse = new MismatchedSetException(null,input);
                throw mse;
            }


            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "null_spec"


    public static class order_expr_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "order_expr"
    // CongerCQL.g:150:1: order_expr : ( attr | const_int );
    public final CongerCQLParser.order_expr_return order_expr() throws RecognitionException {
        CongerCQLParser.order_expr_return retval = new CongerCQLParser.order_expr_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        CongerCQLParser.attr_return attr72 =null;

        CongerCQLParser.const_int_return const_int73 =null;



        try {
            // CongerCQL.g:151:2: ( attr | const_int )
            int alt19=2;
            int LA19_0 = input.LA(1);

            if ( (LA19_0==Identifier||LA19_0==KW_ELEMENT_TIME) ) {
                alt19=1;
            }
            else if ( (LA19_0==Integer) ) {
                alt19=2;
            }
            else {
                if (state.backtracking>0) {state.failed=true; return retval;}
                NoViableAltException nvae =
                    new NoViableAltException("", 19, 0, input);

                throw nvae;

            }
            switch (alt19) {
                case 1 :
                    // CongerCQL.g:151:4: attr
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    pushFollow(FOLLOW_attr_in_order_expr974);
                    attr72=attr();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, attr72.getTree());

                    }
                    break;
                case 2 :
                    // CongerCQL.g:152:4: const_int
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    pushFollow(FOLLOW_const_int_in_order_expr979);
                    const_int73=const_int();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, const_int73.getTree());

                    }
                    break;

            }
            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "order_expr"


    public static class opt_where_clause_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "opt_where_clause"
    // CongerCQL.g:155:1: opt_where_clause : KW_WHERE non_mt_cond_list -> ^( TOK_WHERE non_mt_cond_list ) ;
    public final CongerCQLParser.opt_where_clause_return opt_where_clause() throws RecognitionException {
        CongerCQLParser.opt_where_clause_return retval = new CongerCQLParser.opt_where_clause_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token KW_WHERE74=null;
        CongerCQLParser.non_mt_cond_list_return non_mt_cond_list75 =null;


        CommonTree KW_WHERE74_tree=null;
        RewriteRuleTokenStream stream_KW_WHERE=new RewriteRuleTokenStream(adaptor,"token KW_WHERE");
        RewriteRuleSubtreeStream stream_non_mt_cond_list=new RewriteRuleSubtreeStream(adaptor,"rule non_mt_cond_list");
        try {
            // CongerCQL.g:156:2: ( KW_WHERE non_mt_cond_list -> ^( TOK_WHERE non_mt_cond_list ) )
            // CongerCQL.g:156:4: KW_WHERE non_mt_cond_list
            {
            KW_WHERE74=(Token)match(input,KW_WHERE,FOLLOW_KW_WHERE_in_opt_where_clause990); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_KW_WHERE.add(KW_WHERE74);


            pushFollow(FOLLOW_non_mt_cond_list_in_opt_where_clause992);
            non_mt_cond_list75=non_mt_cond_list();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_non_mt_cond_list.add(non_mt_cond_list75.getTree());

            // AST REWRITE
            // elements: non_mt_cond_list
            // token labels: 
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 157:9: -> ^( TOK_WHERE non_mt_cond_list )
            {
                // CongerCQL.g:157:12: ^( TOK_WHERE non_mt_cond_list )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_WHERE, "TOK_WHERE")
                , root_1);

                adaptor.addChild(root_1, stream_non_mt_cond_list.nextTree());

                adaptor.addChild(root_0, root_1);
                }

            }


            retval.tree = root_0;
            }

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "opt_where_clause"


    public static class opt_having_clause_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "opt_having_clause"
    // CongerCQL.g:160:1: opt_having_clause : KW_HAVING non_mt_cond_list -> ^( TOK_HAVING non_mt_cond_list ) ;
    public final CongerCQLParser.opt_having_clause_return opt_having_clause() throws RecognitionException {
        CongerCQLParser.opt_having_clause_return retval = new CongerCQLParser.opt_having_clause_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token KW_HAVING76=null;
        CongerCQLParser.non_mt_cond_list_return non_mt_cond_list77 =null;


        CommonTree KW_HAVING76_tree=null;
        RewriteRuleTokenStream stream_KW_HAVING=new RewriteRuleTokenStream(adaptor,"token KW_HAVING");
        RewriteRuleSubtreeStream stream_non_mt_cond_list=new RewriteRuleSubtreeStream(adaptor,"rule non_mt_cond_list");
        try {
            // CongerCQL.g:161:2: ( KW_HAVING non_mt_cond_list -> ^( TOK_HAVING non_mt_cond_list ) )
            // CongerCQL.g:161:4: KW_HAVING non_mt_cond_list
            {
            KW_HAVING76=(Token)match(input,KW_HAVING,FOLLOW_KW_HAVING_in_opt_having_clause1019); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_KW_HAVING.add(KW_HAVING76);


            pushFollow(FOLLOW_non_mt_cond_list_in_opt_having_clause1021);
            non_mt_cond_list77=non_mt_cond_list();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_non_mt_cond_list.add(non_mt_cond_list77.getTree());

            // AST REWRITE
            // elements: non_mt_cond_list
            // token labels: 
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 162:9: -> ^( TOK_HAVING non_mt_cond_list )
            {
                // CongerCQL.g:162:12: ^( TOK_HAVING non_mt_cond_list )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_HAVING, "TOK_HAVING")
                , root_1);

                adaptor.addChild(root_1, stream_non_mt_cond_list.nextTree());

                adaptor.addChild(root_0, root_1);
                }

            }


            retval.tree = root_0;
            }

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "opt_having_clause"


    public static class opt_group_by_clause_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "opt_group_by_clause"
    // CongerCQL.g:165:1: opt_group_by_clause : KW_GROUP KW_BY non_mt_attr_list -> ^( TOK_GROUP_BY non_mt_attr_list ) ;
    public final CongerCQLParser.opt_group_by_clause_return opt_group_by_clause() throws RecognitionException {
        CongerCQLParser.opt_group_by_clause_return retval = new CongerCQLParser.opt_group_by_clause_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token KW_GROUP78=null;
        Token KW_BY79=null;
        CongerCQLParser.non_mt_attr_list_return non_mt_attr_list80 =null;


        CommonTree KW_GROUP78_tree=null;
        CommonTree KW_BY79_tree=null;
        RewriteRuleTokenStream stream_KW_GROUP=new RewriteRuleTokenStream(adaptor,"token KW_GROUP");
        RewriteRuleTokenStream stream_KW_BY=new RewriteRuleTokenStream(adaptor,"token KW_BY");
        RewriteRuleSubtreeStream stream_non_mt_attr_list=new RewriteRuleSubtreeStream(adaptor,"rule non_mt_attr_list");
        try {
            // CongerCQL.g:166:2: ( KW_GROUP KW_BY non_mt_attr_list -> ^( TOK_GROUP_BY non_mt_attr_list ) )
            // CongerCQL.g:166:4: KW_GROUP KW_BY non_mt_attr_list
            {
            KW_GROUP78=(Token)match(input,KW_GROUP,FOLLOW_KW_GROUP_in_opt_group_by_clause1048); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_KW_GROUP.add(KW_GROUP78);


            KW_BY79=(Token)match(input,KW_BY,FOLLOW_KW_BY_in_opt_group_by_clause1050); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_KW_BY.add(KW_BY79);


            pushFollow(FOLLOW_non_mt_attr_list_in_opt_group_by_clause1052);
            non_mt_attr_list80=non_mt_attr_list();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_non_mt_attr_list.add(non_mt_attr_list80.getTree());

            // AST REWRITE
            // elements: non_mt_attr_list
            // token labels: 
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 167:9: -> ^( TOK_GROUP_BY non_mt_attr_list )
            {
                // CongerCQL.g:167:12: ^( TOK_GROUP_BY non_mt_attr_list )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_GROUP_BY, "TOK_GROUP_BY")
                , root_1);

                adaptor.addChild(root_1, stream_non_mt_attr_list.nextTree());

                adaptor.addChild(root_0, root_1);
                }

            }


            retval.tree = root_0;
            }

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "opt_group_by_clause"


    public static class from_clause_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "from_clause"
    // CongerCQL.g:170:1: from_clause : KW_FROM non_mt_relation_list ( ( ( KW_LEFT | KW_RIGHT )? KW_OUTER )? KW_JOIN relation_variable KW_ON non_mt_cond_list )? -> ^( TOK_FROM non_mt_relation_list ( ^( TOK_JOIN relation_variable KW_ON non_mt_cond_list ( KW_LEFT )? ( KW_RIGHT )? ( KW_OUTER )? ) )? ) ;
    public final CongerCQLParser.from_clause_return from_clause() throws RecognitionException {
        CongerCQLParser.from_clause_return retval = new CongerCQLParser.from_clause_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token KW_FROM81=null;
        Token KW_LEFT83=null;
        Token KW_RIGHT84=null;
        Token KW_OUTER85=null;
        Token KW_JOIN86=null;
        Token KW_ON88=null;
        CongerCQLParser.non_mt_relation_list_return non_mt_relation_list82 =null;

        CongerCQLParser.relation_variable_return relation_variable87 =null;

        CongerCQLParser.non_mt_cond_list_return non_mt_cond_list89 =null;


        CommonTree KW_FROM81_tree=null;
        CommonTree KW_LEFT83_tree=null;
        CommonTree KW_RIGHT84_tree=null;
        CommonTree KW_OUTER85_tree=null;
        CommonTree KW_JOIN86_tree=null;
        CommonTree KW_ON88_tree=null;
        RewriteRuleTokenStream stream_KW_RIGHT=new RewriteRuleTokenStream(adaptor,"token KW_RIGHT");
        RewriteRuleTokenStream stream_KW_OUTER=new RewriteRuleTokenStream(adaptor,"token KW_OUTER");
        RewriteRuleTokenStream stream_KW_JOIN=new RewriteRuleTokenStream(adaptor,"token KW_JOIN");
        RewriteRuleTokenStream stream_KW_LEFT=new RewriteRuleTokenStream(adaptor,"token KW_LEFT");
        RewriteRuleTokenStream stream_KW_ON=new RewriteRuleTokenStream(adaptor,"token KW_ON");
        RewriteRuleTokenStream stream_KW_FROM=new RewriteRuleTokenStream(adaptor,"token KW_FROM");
        RewriteRuleSubtreeStream stream_relation_variable=new RewriteRuleSubtreeStream(adaptor,"rule relation_variable");
        RewriteRuleSubtreeStream stream_non_mt_cond_list=new RewriteRuleSubtreeStream(adaptor,"rule non_mt_cond_list");
        RewriteRuleSubtreeStream stream_non_mt_relation_list=new RewriteRuleSubtreeStream(adaptor,"rule non_mt_relation_list");
        try {
            // CongerCQL.g:171:2: ( KW_FROM non_mt_relation_list ( ( ( KW_LEFT | KW_RIGHT )? KW_OUTER )? KW_JOIN relation_variable KW_ON non_mt_cond_list )? -> ^( TOK_FROM non_mt_relation_list ( ^( TOK_JOIN relation_variable KW_ON non_mt_cond_list ( KW_LEFT )? ( KW_RIGHT )? ( KW_OUTER )? ) )? ) )
            // CongerCQL.g:171:4: KW_FROM non_mt_relation_list ( ( ( KW_LEFT | KW_RIGHT )? KW_OUTER )? KW_JOIN relation_variable KW_ON non_mt_cond_list )?
            {
            KW_FROM81=(Token)match(input,KW_FROM,FOLLOW_KW_FROM_in_from_clause1079); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_KW_FROM.add(KW_FROM81);


            pushFollow(FOLLOW_non_mt_relation_list_in_from_clause1081);
            non_mt_relation_list82=non_mt_relation_list();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_non_mt_relation_list.add(non_mt_relation_list82.getTree());

            // CongerCQL.g:172:2: ( ( ( KW_LEFT | KW_RIGHT )? KW_OUTER )? KW_JOIN relation_variable KW_ON non_mt_cond_list )?
            int alt22=2;
            int LA22_0 = input.LA(1);

            if ( (LA22_0==KW_JOIN||LA22_0==KW_LEFT||LA22_0==KW_OUTER||LA22_0==KW_RIGHT) ) {
                alt22=1;
            }
            switch (alt22) {
                case 1 :
                    // CongerCQL.g:172:3: ( ( KW_LEFT | KW_RIGHT )? KW_OUTER )? KW_JOIN relation_variable KW_ON non_mt_cond_list
                    {
                    // CongerCQL.g:172:3: ( ( KW_LEFT | KW_RIGHT )? KW_OUTER )?
                    int alt21=2;
                    int LA21_0 = input.LA(1);

                    if ( (LA21_0==KW_LEFT||LA21_0==KW_OUTER||LA21_0==KW_RIGHT) ) {
                        alt21=1;
                    }
                    switch (alt21) {
                        case 1 :
                            // CongerCQL.g:172:4: ( KW_LEFT | KW_RIGHT )? KW_OUTER
                            {
                            // CongerCQL.g:172:4: ( KW_LEFT | KW_RIGHT )?
                            int alt20=3;
                            int LA20_0 = input.LA(1);

                            if ( (LA20_0==KW_LEFT) ) {
                                alt20=1;
                            }
                            else if ( (LA20_0==KW_RIGHT) ) {
                                alt20=2;
                            }
                            switch (alt20) {
                                case 1 :
                                    // CongerCQL.g:172:5: KW_LEFT
                                    {
                                    KW_LEFT83=(Token)match(input,KW_LEFT,FOLLOW_KW_LEFT_in_from_clause1088); if (state.failed) return retval; 
                                    if ( state.backtracking==0 ) stream_KW_LEFT.add(KW_LEFT83);


                                    }
                                    break;
                                case 2 :
                                    // CongerCQL.g:172:15: KW_RIGHT
                                    {
                                    KW_RIGHT84=(Token)match(input,KW_RIGHT,FOLLOW_KW_RIGHT_in_from_clause1092); if (state.failed) return retval; 
                                    if ( state.backtracking==0 ) stream_KW_RIGHT.add(KW_RIGHT84);


                                    }
                                    break;

                            }


                            KW_OUTER85=(Token)match(input,KW_OUTER,FOLLOW_KW_OUTER_in_from_clause1096); if (state.failed) return retval; 
                            if ( state.backtracking==0 ) stream_KW_OUTER.add(KW_OUTER85);


                            }
                            break;

                    }


                    KW_JOIN86=(Token)match(input,KW_JOIN,FOLLOW_KW_JOIN_in_from_clause1100); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_KW_JOIN.add(KW_JOIN86);


                    pushFollow(FOLLOW_relation_variable_in_from_clause1102);
                    relation_variable87=relation_variable();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_relation_variable.add(relation_variable87.getTree());

                    KW_ON88=(Token)match(input,KW_ON,FOLLOW_KW_ON_in_from_clause1104); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_KW_ON.add(KW_ON88);


                    pushFollow(FOLLOW_non_mt_cond_list_in_from_clause1106);
                    non_mt_cond_list89=non_mt_cond_list();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_non_mt_cond_list.add(non_mt_cond_list89.getTree());

                    }
                    break;

            }


            // AST REWRITE
            // elements: KW_OUTER, non_mt_relation_list, non_mt_cond_list, relation_variable, KW_RIGHT, KW_ON, KW_LEFT
            // token labels: 
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 174:9: -> ^( TOK_FROM non_mt_relation_list ( ^( TOK_JOIN relation_variable KW_ON non_mt_cond_list ( KW_LEFT )? ( KW_RIGHT )? ( KW_OUTER )? ) )? )
            {
                // CongerCQL.g:174:12: ^( TOK_FROM non_mt_relation_list ( ^( TOK_JOIN relation_variable KW_ON non_mt_cond_list ( KW_LEFT )? ( KW_RIGHT )? ( KW_OUTER )? ) )? )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_FROM, "TOK_FROM")
                , root_1);

                adaptor.addChild(root_1, stream_non_mt_relation_list.nextTree());

                // CongerCQL.g:175:13: ( ^( TOK_JOIN relation_variable KW_ON non_mt_cond_list ( KW_LEFT )? ( KW_RIGHT )? ( KW_OUTER )? ) )?
                if ( stream_KW_OUTER.hasNext()||stream_non_mt_cond_list.hasNext()||stream_relation_variable.hasNext()||stream_KW_RIGHT.hasNext()||stream_KW_ON.hasNext()||stream_KW_LEFT.hasNext() ) {
                    // CongerCQL.g:175:13: ^( TOK_JOIN relation_variable KW_ON non_mt_cond_list ( KW_LEFT )? ( KW_RIGHT )? ( KW_OUTER )? )
                    {
                    CommonTree root_2 = (CommonTree)adaptor.nil();
                    root_2 = (CommonTree)adaptor.becomeRoot(
                    (CommonTree)adaptor.create(TOK_JOIN, "TOK_JOIN")
                    , root_2);

                    adaptor.addChild(root_2, stream_relation_variable.nextTree());

                    adaptor.addChild(root_2, 
                    stream_KW_ON.nextNode()
                    );

                    adaptor.addChild(root_2, stream_non_mt_cond_list.nextTree());

                    // CongerCQL.g:175:65: ( KW_LEFT )?
                    if ( stream_KW_LEFT.hasNext() ) {
                        adaptor.addChild(root_2, 
                        stream_KW_LEFT.nextNode()
                        );

                    }
                    stream_KW_LEFT.reset();

                    // CongerCQL.g:175:74: ( KW_RIGHT )?
                    if ( stream_KW_RIGHT.hasNext() ) {
                        adaptor.addChild(root_2, 
                        stream_KW_RIGHT.nextNode()
                        );

                    }
                    stream_KW_RIGHT.reset();

                    // CongerCQL.g:175:84: ( KW_OUTER )?
                    if ( stream_KW_OUTER.hasNext() ) {
                        adaptor.addChild(root_2, 
                        stream_KW_OUTER.nextNode()
                        );

                    }
                    stream_KW_OUTER.reset();

                    adaptor.addChild(root_1, root_2);
                    }

                }
                stream_KW_OUTER.reset();
                stream_non_mt_cond_list.reset();
                stream_relation_variable.reset();
                stream_KW_RIGHT.reset();
                stream_KW_ON.reset();
                stream_KW_LEFT.reset();

                adaptor.addChild(root_0, root_1);
                }

            }


            retval.tree = root_0;
            }

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "from_clause"


    public static class non_mt_cond_list_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "non_mt_cond_list"
    // CongerCQL.g:178:1: non_mt_cond_list : non_mt_cond_list_main ( cond_list_operator non_mt_cond_list_main )* -> ^( TOK_COND_LIST ( ^( TOK_COND ( cond_list_operator )? non_mt_cond_list_main ) )+ ) ;
    public final CongerCQLParser.non_mt_cond_list_return non_mt_cond_list() throws RecognitionException {
        CongerCQLParser.non_mt_cond_list_return retval = new CongerCQLParser.non_mt_cond_list_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        CongerCQLParser.non_mt_cond_list_main_return non_mt_cond_list_main90 =null;

        CongerCQLParser.cond_list_operator_return cond_list_operator91 =null;

        CongerCQLParser.non_mt_cond_list_main_return non_mt_cond_list_main92 =null;


        RewriteRuleSubtreeStream stream_cond_list_operator=new RewriteRuleSubtreeStream(adaptor,"rule cond_list_operator");
        RewriteRuleSubtreeStream stream_non_mt_cond_list_main=new RewriteRuleSubtreeStream(adaptor,"rule non_mt_cond_list_main");
        try {
            // CongerCQL.g:179:2: ( non_mt_cond_list_main ( cond_list_operator non_mt_cond_list_main )* -> ^( TOK_COND_LIST ( ^( TOK_COND ( cond_list_operator )? non_mt_cond_list_main ) )+ ) )
            // CongerCQL.g:179:4: non_mt_cond_list_main ( cond_list_operator non_mt_cond_list_main )*
            {
            pushFollow(FOLLOW_non_mt_cond_list_main_in_non_mt_cond_list1171);
            non_mt_cond_list_main90=non_mt_cond_list_main();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_non_mt_cond_list_main.add(non_mt_cond_list_main90.getTree());

            // CongerCQL.g:179:26: ( cond_list_operator non_mt_cond_list_main )*
            loop23:
            do {
                int alt23=2;
                int LA23_0 = input.LA(1);

                if ( (LA23_0==KW_AND||LA23_0==KW_OR||LA23_0==KW_XOR) ) {
                    alt23=1;
                }


                switch (alt23) {
            	case 1 :
            	    // CongerCQL.g:179:27: cond_list_operator non_mt_cond_list_main
            	    {
            	    pushFollow(FOLLOW_cond_list_operator_in_non_mt_cond_list1174);
            	    cond_list_operator91=cond_list_operator();

            	    state._fsp--;
            	    if (state.failed) return retval;
            	    if ( state.backtracking==0 ) stream_cond_list_operator.add(cond_list_operator91.getTree());

            	    pushFollow(FOLLOW_non_mt_cond_list_main_in_non_mt_cond_list1176);
            	    non_mt_cond_list_main92=non_mt_cond_list_main();

            	    state._fsp--;
            	    if (state.failed) return retval;
            	    if ( state.backtracking==0 ) stream_non_mt_cond_list_main.add(non_mt_cond_list_main92.getTree());

            	    }
            	    break;

            	default :
            	    break loop23;
                }
            } while (true);


            // AST REWRITE
            // elements: non_mt_cond_list_main, cond_list_operator
            // token labels: 
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 180:9: -> ^( TOK_COND_LIST ( ^( TOK_COND ( cond_list_operator )? non_mt_cond_list_main ) )+ )
            {
                // CongerCQL.g:180:12: ^( TOK_COND_LIST ( ^( TOK_COND ( cond_list_operator )? non_mt_cond_list_main ) )+ )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_COND_LIST, "TOK_COND_LIST")
                , root_1);

                if ( !(stream_non_mt_cond_list_main.hasNext()) ) {
                    throw new RewriteEarlyExitException();
                }
                while ( stream_non_mt_cond_list_main.hasNext() ) {
                    // CongerCQL.g:180:28: ^( TOK_COND ( cond_list_operator )? non_mt_cond_list_main )
                    {
                    CommonTree root_2 = (CommonTree)adaptor.nil();
                    root_2 = (CommonTree)adaptor.becomeRoot(
                    (CommonTree)adaptor.create(TOK_COND, "TOK_COND")
                    , root_2);

                    // CongerCQL.g:180:39: ( cond_list_operator )?
                    if ( stream_cond_list_operator.hasNext() ) {
                        adaptor.addChild(root_2, stream_cond_list_operator.nextTree());

                    }
                    stream_cond_list_operator.reset();

                    adaptor.addChild(root_2, stream_non_mt_cond_list_main.nextTree());

                    adaptor.addChild(root_1, root_2);
                    }

                }
                stream_non_mt_cond_list_main.reset();

                adaptor.addChild(root_0, root_1);
                }

            }


            retval.tree = root_0;
            }

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "non_mt_cond_list"


    public static class cond_list_operator_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "cond_list_operator"
    // CongerCQL.g:183:1: cond_list_operator : ( KW_XOR | KW_OR | KW_AND );
    public final CongerCQLParser.cond_list_operator_return cond_list_operator() throws RecognitionException {
        CongerCQLParser.cond_list_operator_return retval = new CongerCQLParser.cond_list_operator_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token set93=null;

        CommonTree set93_tree=null;

        try {
            // CongerCQL.g:184:5: ( KW_XOR | KW_OR | KW_AND )
            // CongerCQL.g:
            {
            root_0 = (CommonTree)adaptor.nil();


            set93=(Token)input.LT(1);

            if ( input.LA(1)==KW_AND||input.LA(1)==KW_OR||input.LA(1)==KW_XOR ) {
                input.consume();
                if ( state.backtracking==0 ) adaptor.addChild(root_0, 
                (CommonTree)adaptor.create(set93)
                );
                state.errorRecovery=false;
                state.failed=false;
            }
            else {
                if (state.backtracking>0) {state.failed=true; return retval;}
                MismatchedSetException mse = new MismatchedSetException(null,input);
                throw mse;
            }


            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "cond_list_operator"


    public static class non_mt_cond_list_main_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "non_mt_cond_list_main"
    // CongerCQL.g:187:1: non_mt_cond_list_main options {backtrack=true; } : ( KW_NOT non_mt_cond_list_main | unary_condition | LPAREN unary_condition RPAREN );
    public final CongerCQLParser.non_mt_cond_list_main_return non_mt_cond_list_main() throws RecognitionException {
        CongerCQLParser.non_mt_cond_list_main_return retval = new CongerCQLParser.non_mt_cond_list_main_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token KW_NOT94=null;
        Token LPAREN97=null;
        Token RPAREN99=null;
        CongerCQLParser.non_mt_cond_list_main_return non_mt_cond_list_main95 =null;

        CongerCQLParser.unary_condition_return unary_condition96 =null;

        CongerCQLParser.unary_condition_return unary_condition98 =null;


        CommonTree KW_NOT94_tree=null;
        CommonTree LPAREN97_tree=null;
        CommonTree RPAREN99_tree=null;

        try {
            // CongerCQL.g:189:2: ( KW_NOT non_mt_cond_list_main | unary_condition | LPAREN unary_condition RPAREN )
            int alt24=3;
            switch ( input.LA(1) ) {
            case KW_NOT:
                {
                alt24=1;
                }
                break;
            case Identifier:
            case Integer:
            case KW_AVG:
            case KW_COUNT:
            case KW_DOUBLE:
            case KW_ELEMENT_TIME:
            case KW_FLOAT:
            case KW_INTERVAL:
            case KW_MAX:
            case KW_MIN:
            case KW_NULL:
            case KW_SUM:
            case StringLiteral:
                {
                alt24=2;
                }
                break;
            case LPAREN:
                {
                int LA24_15 = input.LA(2);

                if ( (synpred2_CongerCQL()) ) {
                    alt24=2;
                }
                else if ( (true) ) {
                    alt24=3;
                }
                else {
                    if (state.backtracking>0) {state.failed=true; return retval;}
                    NoViableAltException nvae =
                        new NoViableAltException("", 24, 15, input);

                    throw nvae;

                }
                }
                break;
            default:
                if (state.backtracking>0) {state.failed=true; return retval;}
                NoViableAltException nvae =
                    new NoViableAltException("", 24, 0, input);

                throw nvae;

            }

            switch (alt24) {
                case 1 :
                    // CongerCQL.g:189:4: KW_NOT non_mt_cond_list_main
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    KW_NOT94=(Token)match(input,KW_NOT,FOLLOW_KW_NOT_in_non_mt_cond_list_main1246); if (state.failed) return retval;
                    if ( state.backtracking==0 ) {
                    KW_NOT94_tree = 
                    (CommonTree)adaptor.create(KW_NOT94)
                    ;
                    adaptor.addChild(root_0, KW_NOT94_tree);
                    }

                    pushFollow(FOLLOW_non_mt_cond_list_main_in_non_mt_cond_list_main1248);
                    non_mt_cond_list_main95=non_mt_cond_list_main();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, non_mt_cond_list_main95.getTree());

                    }
                    break;
                case 2 :
                    // CongerCQL.g:190:4: unary_condition
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    pushFollow(FOLLOW_unary_condition_in_non_mt_cond_list_main1253);
                    unary_condition96=unary_condition();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, unary_condition96.getTree());

                    }
                    break;
                case 3 :
                    // CongerCQL.g:191:4: LPAREN unary_condition RPAREN
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    LPAREN97=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_non_mt_cond_list_main1258); if (state.failed) return retval;
                    if ( state.backtracking==0 ) {
                    LPAREN97_tree = 
                    (CommonTree)adaptor.create(LPAREN97)
                    ;
                    adaptor.addChild(root_0, LPAREN97_tree);
                    }

                    pushFollow(FOLLOW_unary_condition_in_non_mt_cond_list_main1260);
                    unary_condition98=unary_condition();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, unary_condition98.getTree());

                    RPAREN99=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_non_mt_cond_list_main1262); if (state.failed) return retval;
                    if ( state.backtracking==0 ) {
                    RPAREN99_tree = 
                    (CommonTree)adaptor.create(RPAREN99)
                    ;
                    adaptor.addChild(root_0, RPAREN99_tree);
                    }

                    }
                    break;

            }
            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "non_mt_cond_list_main"


    public static class unary_condition_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "unary_condition"
    // CongerCQL.g:194:1: unary_condition : arith_expr ( between_condition_right | compare_condition_right ) ;
    public final CongerCQLParser.unary_condition_return unary_condition() throws RecognitionException {
        CongerCQLParser.unary_condition_return retval = new CongerCQLParser.unary_condition_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        CongerCQLParser.arith_expr_return arith_expr100 =null;

        CongerCQLParser.between_condition_right_return between_condition_right101 =null;

        CongerCQLParser.compare_condition_right_return compare_condition_right102 =null;



        try {
            // CongerCQL.g:195:2: ( arith_expr ( between_condition_right | compare_condition_right ) )
            // CongerCQL.g:195:4: arith_expr ( between_condition_right | compare_condition_right )
            {
            root_0 = (CommonTree)adaptor.nil();


            pushFollow(FOLLOW_arith_expr_in_unary_condition1273);
            arith_expr100=arith_expr();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) adaptor.addChild(root_0, arith_expr100.getTree());

            // CongerCQL.g:195:15: ( between_condition_right | compare_condition_right )
            int alt25=2;
            int LA25_0 = input.LA(1);

            if ( (LA25_0==KW_BETWEEN) ) {
                alt25=1;
            }
            else if ( (LA25_0==EQUAL||(LA25_0 >= GREATERTHAN && LA25_0 <= GREATERTHANOREQUALTO)||(LA25_0 >= LESSTHAN && LA25_0 <= LESSTHANOREQUALTO)||LA25_0==NOTEQUAL) ) {
                alt25=2;
            }
            else {
                if (state.backtracking>0) {state.failed=true; return retval;}
                NoViableAltException nvae =
                    new NoViableAltException("", 25, 0, input);

                throw nvae;

            }
            switch (alt25) {
                case 1 :
                    // CongerCQL.g:195:16: between_condition_right
                    {
                    pushFollow(FOLLOW_between_condition_right_in_unary_condition1276);
                    between_condition_right101=between_condition_right();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, between_condition_right101.getTree());

                    }
                    break;
                case 2 :
                    // CongerCQL.g:195:42: compare_condition_right
                    {
                    pushFollow(FOLLOW_compare_condition_right_in_unary_condition1280);
                    compare_condition_right102=compare_condition_right();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, compare_condition_right102.getTree());

                    }
                    break;

            }


            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "unary_condition"


    public static class between_condition_right_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "between_condition_right"
    // CongerCQL.g:198:1: between_condition_right : KW_BETWEEN arith_expr KW_AND arith_expr ;
    public final CongerCQLParser.between_condition_right_return between_condition_right() throws RecognitionException {
        CongerCQLParser.between_condition_right_return retval = new CongerCQLParser.between_condition_right_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token KW_BETWEEN103=null;
        Token KW_AND105=null;
        CongerCQLParser.arith_expr_return arith_expr104 =null;

        CongerCQLParser.arith_expr_return arith_expr106 =null;


        CommonTree KW_BETWEEN103_tree=null;
        CommonTree KW_AND105_tree=null;

        try {
            // CongerCQL.g:199:2: ( KW_BETWEEN arith_expr KW_AND arith_expr )
            // CongerCQL.g:199:4: KW_BETWEEN arith_expr KW_AND arith_expr
            {
            root_0 = (CommonTree)adaptor.nil();


            KW_BETWEEN103=(Token)match(input,KW_BETWEEN,FOLLOW_KW_BETWEEN_in_between_condition_right1293); if (state.failed) return retval;
            if ( state.backtracking==0 ) {
            KW_BETWEEN103_tree = 
            (CommonTree)adaptor.create(KW_BETWEEN103)
            ;
            adaptor.addChild(root_0, KW_BETWEEN103_tree);
            }

            pushFollow(FOLLOW_arith_expr_in_between_condition_right1295);
            arith_expr104=arith_expr();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) adaptor.addChild(root_0, arith_expr104.getTree());

            KW_AND105=(Token)match(input,KW_AND,FOLLOW_KW_AND_in_between_condition_right1297); if (state.failed) return retval;
            if ( state.backtracking==0 ) {
            KW_AND105_tree = 
            (CommonTree)adaptor.create(KW_AND105)
            ;
            adaptor.addChild(root_0, KW_AND105_tree);
            }

            pushFollow(FOLLOW_arith_expr_in_between_condition_right1299);
            arith_expr106=arith_expr();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) adaptor.addChild(root_0, arith_expr106.getTree());

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "between_condition_right"


    public static class compare_condition_right_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "compare_condition_right"
    // CongerCQL.g:202:1: compare_condition_right : unary_condition_operator arith_expr ;
    public final CongerCQLParser.compare_condition_right_return compare_condition_right() throws RecognitionException {
        CongerCQLParser.compare_condition_right_return retval = new CongerCQLParser.compare_condition_right_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        CongerCQLParser.unary_condition_operator_return unary_condition_operator107 =null;

        CongerCQLParser.arith_expr_return arith_expr108 =null;



        try {
            // CongerCQL.g:203:2: ( unary_condition_operator arith_expr )
            // CongerCQL.g:203:4: unary_condition_operator arith_expr
            {
            root_0 = (CommonTree)adaptor.nil();


            pushFollow(FOLLOW_unary_condition_operator_in_compare_condition_right1310);
            unary_condition_operator107=unary_condition_operator();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) adaptor.addChild(root_0, unary_condition_operator107.getTree());

            pushFollow(FOLLOW_arith_expr_in_compare_condition_right1312);
            arith_expr108=arith_expr();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) adaptor.addChild(root_0, arith_expr108.getTree());

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "compare_condition_right"


    public static class unary_condition_operator_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "unary_condition_operator"
    // CongerCQL.g:206:1: unary_condition_operator : ( LESSTHAN | LESSTHANOREQUALTO | GREATERTHAN | GREATERTHANOREQUALTO | EQUAL | NOTEQUAL );
    public final CongerCQLParser.unary_condition_operator_return unary_condition_operator() throws RecognitionException {
        CongerCQLParser.unary_condition_operator_return retval = new CongerCQLParser.unary_condition_operator_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token set109=null;

        CommonTree set109_tree=null;

        try {
            // CongerCQL.g:207:5: ( LESSTHAN | LESSTHANOREQUALTO | GREATERTHAN | GREATERTHANOREQUALTO | EQUAL | NOTEQUAL )
            // CongerCQL.g:
            {
            root_0 = (CommonTree)adaptor.nil();


            set109=(Token)input.LT(1);

            if ( input.LA(1)==EQUAL||(input.LA(1) >= GREATERTHAN && input.LA(1) <= GREATERTHANOREQUALTO)||(input.LA(1) >= LESSTHAN && input.LA(1) <= LESSTHANOREQUALTO)||input.LA(1)==NOTEQUAL ) {
                input.consume();
                if ( state.backtracking==0 ) adaptor.addChild(root_0, 
                (CommonTree)adaptor.create(set109)
                );
                state.errorRecovery=false;
                state.failed=false;
            }
            else {
                if (state.backtracking>0) {state.failed=true; return retval;}
                MismatchedSetException mse = new MismatchedSetException(null,input);
                throw mse;
            }


            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "unary_condition_operator"


    public static class const_string_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "const_string"
    // CongerCQL.g:210:1: const_string : StringLiteral ;
    public final CongerCQLParser.const_string_return const_string() throws RecognitionException {
        CongerCQLParser.const_string_return retval = new CongerCQLParser.const_string_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token StringLiteral110=null;

        CommonTree StringLiteral110_tree=null;

        try {
            // CongerCQL.g:211:2: ( StringLiteral )
            // CongerCQL.g:211:4: StringLiteral
            {
            root_0 = (CommonTree)adaptor.nil();


            StringLiteral110=(Token)match(input,StringLiteral,FOLLOW_StringLiteral_in_const_string1361); if (state.failed) return retval;
            if ( state.backtracking==0 ) {
            StringLiteral110_tree = 
            (CommonTree)adaptor.create(StringLiteral110)
            ;
            adaptor.addChild(root_0, StringLiteral110_tree);
            }

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "const_string"


    public static class non_mt_arg_list_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "non_mt_arg_list"
    // CongerCQL.g:214:1: non_mt_arg_list : arith_expr ( options {greedy=true; } : COMMA non_mt_arg_list )* -> ^( TOK_ARG_LIST ( arith_expr )+ ) ;
    public final CongerCQLParser.non_mt_arg_list_return non_mt_arg_list() throws RecognitionException {
        CongerCQLParser.non_mt_arg_list_return retval = new CongerCQLParser.non_mt_arg_list_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token COMMA112=null;
        CongerCQLParser.arith_expr_return arith_expr111 =null;

        CongerCQLParser.non_mt_arg_list_return non_mt_arg_list113 =null;


        CommonTree COMMA112_tree=null;
        RewriteRuleTokenStream stream_COMMA=new RewriteRuleTokenStream(adaptor,"token COMMA");
        RewriteRuleSubtreeStream stream_arith_expr=new RewriteRuleSubtreeStream(adaptor,"rule arith_expr");
        RewriteRuleSubtreeStream stream_non_mt_arg_list=new RewriteRuleSubtreeStream(adaptor,"rule non_mt_arg_list");
        try {
            // CongerCQL.g:215:2: ( arith_expr ( options {greedy=true; } : COMMA non_mt_arg_list )* -> ^( TOK_ARG_LIST ( arith_expr )+ ) )
            // CongerCQL.g:215:4: arith_expr ( options {greedy=true; } : COMMA non_mt_arg_list )*
            {
            pushFollow(FOLLOW_arith_expr_in_non_mt_arg_list1372);
            arith_expr111=arith_expr();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_arith_expr.add(arith_expr111.getTree());

            // CongerCQL.g:215:15: ( options {greedy=true; } : COMMA non_mt_arg_list )*
            loop26:
            do {
                int alt26=2;
                int LA26_0 = input.LA(1);

                if ( (LA26_0==COMMA) ) {
                    alt26=1;
                }


                switch (alt26) {
            	case 1 :
            	    // CongerCQL.g:215:39: COMMA non_mt_arg_list
            	    {
            	    COMMA112=(Token)match(input,COMMA,FOLLOW_COMMA_in_non_mt_arg_list1383); if (state.failed) return retval; 
            	    if ( state.backtracking==0 ) stream_COMMA.add(COMMA112);


            	    pushFollow(FOLLOW_non_mt_arg_list_in_non_mt_arg_list1385);
            	    non_mt_arg_list113=non_mt_arg_list();

            	    state._fsp--;
            	    if (state.failed) return retval;
            	    if ( state.backtracking==0 ) stream_non_mt_arg_list.add(non_mt_arg_list113.getTree());

            	    }
            	    break;

            	default :
            	    break loop26;
                }
            } while (true);


            // AST REWRITE
            // elements: arith_expr
            // token labels: 
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 216:3: -> ^( TOK_ARG_LIST ( arith_expr )+ )
            {
                // CongerCQL.g:216:6: ^( TOK_ARG_LIST ( arith_expr )+ )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_ARG_LIST, "TOK_ARG_LIST")
                , root_1);

                if ( !(stream_arith_expr.hasNext()) ) {
                    throw new RewriteEarlyExitException();
                }
                while ( stream_arith_expr.hasNext() ) {
                    adaptor.addChild(root_1, stream_arith_expr.nextTree());

                }
                stream_arith_expr.reset();

                adaptor.addChild(root_0, root_1);
                }

            }


            retval.tree = root_0;
            }

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "non_mt_arg_list"


    public static class non_mt_relation_list_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "non_mt_relation_list"
    // CongerCQL.g:219:1: non_mt_relation_list : relation_variable ( options {greedy=true; } : COMMA non_mt_relation_list )* -> ^( TOK_RELATION_LIST ( relation_variable )+ ) ;
    public final CongerCQLParser.non_mt_relation_list_return non_mt_relation_list() throws RecognitionException {
        CongerCQLParser.non_mt_relation_list_return retval = new CongerCQLParser.non_mt_relation_list_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token COMMA115=null;
        CongerCQLParser.relation_variable_return relation_variable114 =null;

        CongerCQLParser.non_mt_relation_list_return non_mt_relation_list116 =null;


        CommonTree COMMA115_tree=null;
        RewriteRuleTokenStream stream_COMMA=new RewriteRuleTokenStream(adaptor,"token COMMA");
        RewriteRuleSubtreeStream stream_relation_variable=new RewriteRuleSubtreeStream(adaptor,"rule relation_variable");
        RewriteRuleSubtreeStream stream_non_mt_relation_list=new RewriteRuleSubtreeStream(adaptor,"rule non_mt_relation_list");
        try {
            // CongerCQL.g:220:2: ( relation_variable ( options {greedy=true; } : COMMA non_mt_relation_list )* -> ^( TOK_RELATION_LIST ( relation_variable )+ ) )
            // CongerCQL.g:220:4: relation_variable ( options {greedy=true; } : COMMA non_mt_relation_list )*
            {
            pushFollow(FOLLOW_relation_variable_in_non_mt_relation_list1410);
            relation_variable114=relation_variable();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_relation_variable.add(relation_variable114.getTree());

            // CongerCQL.g:220:22: ( options {greedy=true; } : COMMA non_mt_relation_list )*
            loop27:
            do {
                int alt27=2;
                int LA27_0 = input.LA(1);

                if ( (LA27_0==COMMA) ) {
                    alt27=1;
                }


                switch (alt27) {
            	case 1 :
            	    // CongerCQL.g:220:46: COMMA non_mt_relation_list
            	    {
            	    COMMA115=(Token)match(input,COMMA,FOLLOW_COMMA_in_non_mt_relation_list1421); if (state.failed) return retval; 
            	    if ( state.backtracking==0 ) stream_COMMA.add(COMMA115);


            	    pushFollow(FOLLOW_non_mt_relation_list_in_non_mt_relation_list1423);
            	    non_mt_relation_list116=non_mt_relation_list();

            	    state._fsp--;
            	    if (state.failed) return retval;
            	    if ( state.backtracking==0 ) stream_non_mt_relation_list.add(non_mt_relation_list116.getTree());

            	    }
            	    break;

            	default :
            	    break loop27;
                }
            } while (true);


            // AST REWRITE
            // elements: relation_variable
            // token labels: 
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 221:9: -> ^( TOK_RELATION_LIST ( relation_variable )+ )
            {
                // CongerCQL.g:221:12: ^( TOK_RELATION_LIST ( relation_variable )+ )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_RELATION_LIST, "TOK_RELATION_LIST")
                , root_1);

                if ( !(stream_relation_variable.hasNext()) ) {
                    throw new RewriteEarlyExitException();
                }
                while ( stream_relation_variable.hasNext() ) {
                    adaptor.addChild(root_1, stream_relation_variable.nextTree());

                }
                stream_relation_variable.reset();

                adaptor.addChild(root_0, root_1);
                }

            }


            retval.tree = root_0;
            }

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "non_mt_relation_list"


    public static class relation_variable_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "relation_variable"
    // CongerCQL.g:224:1: relation_variable : variableName= Identifier ( LSQUARE window_type RSQUARE )? ( KW_AS alias= Identifier )? -> ^( TOK_RELATION_VARIABLE $variableName ( ^( TOK_WINDOW window_type ) )? ( $alias)? ) ;
    public final CongerCQLParser.relation_variable_return relation_variable() throws RecognitionException {
        CongerCQLParser.relation_variable_return retval = new CongerCQLParser.relation_variable_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token variableName=null;
        Token alias=null;
        Token LSQUARE117=null;
        Token RSQUARE119=null;
        Token KW_AS120=null;
        CongerCQLParser.window_type_return window_type118 =null;


        CommonTree variableName_tree=null;
        CommonTree alias_tree=null;
        CommonTree LSQUARE117_tree=null;
        CommonTree RSQUARE119_tree=null;
        CommonTree KW_AS120_tree=null;
        RewriteRuleTokenStream stream_KW_AS=new RewriteRuleTokenStream(adaptor,"token KW_AS");
        RewriteRuleTokenStream stream_LSQUARE=new RewriteRuleTokenStream(adaptor,"token LSQUARE");
        RewriteRuleTokenStream stream_RSQUARE=new RewriteRuleTokenStream(adaptor,"token RSQUARE");
        RewriteRuleTokenStream stream_Identifier=new RewriteRuleTokenStream(adaptor,"token Identifier");
        RewriteRuleSubtreeStream stream_window_type=new RewriteRuleSubtreeStream(adaptor,"rule window_type");
        try {
            // CongerCQL.g:225:2: (variableName= Identifier ( LSQUARE window_type RSQUARE )? ( KW_AS alias= Identifier )? -> ^( TOK_RELATION_VARIABLE $variableName ( ^( TOK_WINDOW window_type ) )? ( $alias)? ) )
            // CongerCQL.g:225:4: variableName= Identifier ( LSQUARE window_type RSQUARE )? ( KW_AS alias= Identifier )?
            {
            variableName=(Token)match(input,Identifier,FOLLOW_Identifier_in_relation_variable1455); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_Identifier.add(variableName);


            // CongerCQL.g:225:28: ( LSQUARE window_type RSQUARE )?
            int alt28=2;
            int LA28_0 = input.LA(1);

            if ( (LA28_0==LSQUARE) ) {
                alt28=1;
            }
            switch (alt28) {
                case 1 :
                    // CongerCQL.g:225:29: LSQUARE window_type RSQUARE
                    {
                    LSQUARE117=(Token)match(input,LSQUARE,FOLLOW_LSQUARE_in_relation_variable1458); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_LSQUARE.add(LSQUARE117);


                    pushFollow(FOLLOW_window_type_in_relation_variable1460);
                    window_type118=window_type();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_window_type.add(window_type118.getTree());

                    RSQUARE119=(Token)match(input,RSQUARE,FOLLOW_RSQUARE_in_relation_variable1462); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_RSQUARE.add(RSQUARE119);


                    }
                    break;

            }


            // CongerCQL.g:225:59: ( KW_AS alias= Identifier )?
            int alt29=2;
            int LA29_0 = input.LA(1);

            if ( (LA29_0==KW_AS) ) {
                alt29=1;
            }
            switch (alt29) {
                case 1 :
                    // CongerCQL.g:225:61: KW_AS alias= Identifier
                    {
                    KW_AS120=(Token)match(input,KW_AS,FOLLOW_KW_AS_in_relation_variable1468); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_KW_AS.add(KW_AS120);


                    alias=(Token)match(input,Identifier,FOLLOW_Identifier_in_relation_variable1472); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_Identifier.add(alias);


                    }
                    break;

            }


            // AST REWRITE
            // elements: window_type, alias, variableName
            // token labels: variableName, alias
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleTokenStream stream_variableName=new RewriteRuleTokenStream(adaptor,"token variableName",variableName);
            RewriteRuleTokenStream stream_alias=new RewriteRuleTokenStream(adaptor,"token alias",alias);
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 226:9: -> ^( TOK_RELATION_VARIABLE $variableName ( ^( TOK_WINDOW window_type ) )? ( $alias)? )
            {
                // CongerCQL.g:226:12: ^( TOK_RELATION_VARIABLE $variableName ( ^( TOK_WINDOW window_type ) )? ( $alias)? )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_RELATION_VARIABLE, "TOK_RELATION_VARIABLE")
                , root_1);

                adaptor.addChild(root_1, stream_variableName.nextNode());

                // CongerCQL.g:226:50: ( ^( TOK_WINDOW window_type ) )?
                if ( stream_window_type.hasNext() ) {
                    // CongerCQL.g:226:50: ^( TOK_WINDOW window_type )
                    {
                    CommonTree root_2 = (CommonTree)adaptor.nil();
                    root_2 = (CommonTree)adaptor.becomeRoot(
                    (CommonTree)adaptor.create(TOK_WINDOW, "TOK_WINDOW")
                    , root_2);

                    adaptor.addChild(root_2, stream_window_type.nextTree());

                    adaptor.addChild(root_1, root_2);
                    }

                }
                stream_window_type.reset();

                // CongerCQL.g:226:78: ( $alias)?
                if ( stream_alias.hasNext() ) {
                    adaptor.addChild(root_1, stream_alias.nextNode());

                }
                stream_alias.reset();

                adaptor.addChild(root_0, root_1);
                }

            }


            retval.tree = root_0;
            }

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "relation_variable"


    public static class window_type_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "window_type"
    // CongerCQL.g:229:1: window_type : ( KW_RANGE ( time_spec ( KW_SLIDE time_spec )? | const_value KW_ON Identifier | KW_UNBOUNDED ) | KW_NOW | KW_ROWS Integer ( KW_SLIDE Integer )? | KW_PARTITION KW_BY non_mt_attr_list KW_ROWS rows= Integer ( KW_RANGE time_spec ( KW_SLIDE time_spec )? )? -> ^( TOK_PARTITION non_mt_attr_list $rows ( KW_RANGE time_spec ( KW_SLIDE time_spec )? )? ) );
    public final CongerCQLParser.window_type_return window_type() throws RecognitionException {
        CongerCQLParser.window_type_return retval = new CongerCQLParser.window_type_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token rows=null;
        Token KW_RANGE121=null;
        Token KW_SLIDE123=null;
        Token KW_ON126=null;
        Token Identifier127=null;
        Token KW_UNBOUNDED128=null;
        Token KW_NOW129=null;
        Token KW_ROWS130=null;
        Token Integer131=null;
        Token KW_SLIDE132=null;
        Token Integer133=null;
        Token KW_PARTITION134=null;
        Token KW_BY135=null;
        Token KW_ROWS137=null;
        Token KW_RANGE138=null;
        Token KW_SLIDE140=null;
        CongerCQLParser.time_spec_return time_spec122 =null;

        CongerCQLParser.time_spec_return time_spec124 =null;

        CongerCQLParser.const_value_return const_value125 =null;

        CongerCQLParser.non_mt_attr_list_return non_mt_attr_list136 =null;

        CongerCQLParser.time_spec_return time_spec139 =null;

        CongerCQLParser.time_spec_return time_spec141 =null;


        CommonTree rows_tree=null;
        CommonTree KW_RANGE121_tree=null;
        CommonTree KW_SLIDE123_tree=null;
        CommonTree KW_ON126_tree=null;
        CommonTree Identifier127_tree=null;
        CommonTree KW_UNBOUNDED128_tree=null;
        CommonTree KW_NOW129_tree=null;
        CommonTree KW_ROWS130_tree=null;
        CommonTree Integer131_tree=null;
        CommonTree KW_SLIDE132_tree=null;
        CommonTree Integer133_tree=null;
        CommonTree KW_PARTITION134_tree=null;
        CommonTree KW_BY135_tree=null;
        CommonTree KW_ROWS137_tree=null;
        CommonTree KW_RANGE138_tree=null;
        CommonTree KW_SLIDE140_tree=null;
        RewriteRuleTokenStream stream_KW_ROWS=new RewriteRuleTokenStream(adaptor,"token KW_ROWS");
        RewriteRuleTokenStream stream_KW_SLIDE=new RewriteRuleTokenStream(adaptor,"token KW_SLIDE");
        RewriteRuleTokenStream stream_Integer=new RewriteRuleTokenStream(adaptor,"token Integer");
        RewriteRuleTokenStream stream_KW_PARTITION=new RewriteRuleTokenStream(adaptor,"token KW_PARTITION");
        RewriteRuleTokenStream stream_KW_RANGE=new RewriteRuleTokenStream(adaptor,"token KW_RANGE");
        RewriteRuleTokenStream stream_KW_BY=new RewriteRuleTokenStream(adaptor,"token KW_BY");
        RewriteRuleSubtreeStream stream_time_spec=new RewriteRuleSubtreeStream(adaptor,"rule time_spec");
        RewriteRuleSubtreeStream stream_non_mt_attr_list=new RewriteRuleSubtreeStream(adaptor,"rule non_mt_attr_list");
        try {
            // CongerCQL.g:230:2: ( KW_RANGE ( time_spec ( KW_SLIDE time_spec )? | const_value KW_ON Identifier | KW_UNBOUNDED ) | KW_NOW | KW_ROWS Integer ( KW_SLIDE Integer )? | KW_PARTITION KW_BY non_mt_attr_list KW_ROWS rows= Integer ( KW_RANGE time_spec ( KW_SLIDE time_spec )? )? -> ^( TOK_PARTITION non_mt_attr_list $rows ( KW_RANGE time_spec ( KW_SLIDE time_spec )? )? ) )
            int alt35=4;
            switch ( input.LA(1) ) {
            case KW_RANGE:
                {
                alt35=1;
                }
                break;
            case KW_NOW:
                {
                alt35=2;
                }
                break;
            case KW_ROWS:
                {
                alt35=3;
                }
                break;
            case KW_PARTITION:
                {
                alt35=4;
                }
                break;
            default:
                if (state.backtracking>0) {state.failed=true; return retval;}
                NoViableAltException nvae =
                    new NoViableAltException("", 35, 0, input);

                throw nvae;

            }

            switch (alt35) {
                case 1 :
                    // CongerCQL.g:230:4: KW_RANGE ( time_spec ( KW_SLIDE time_spec )? | const_value KW_ON Identifier | KW_UNBOUNDED )
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    KW_RANGE121=(Token)match(input,KW_RANGE,FOLLOW_KW_RANGE_in_window_type1513); if (state.failed) return retval;
                    if ( state.backtracking==0 ) {
                    KW_RANGE121_tree = 
                    (CommonTree)adaptor.create(KW_RANGE121)
                    ;
                    adaptor.addChild(root_0, KW_RANGE121_tree);
                    }

                    // CongerCQL.g:231:3: ( time_spec ( KW_SLIDE time_spec )? | const_value KW_ON Identifier | KW_UNBOUNDED )
                    int alt31=3;
                    switch ( input.LA(1) ) {
                    case Integer:
                        {
                        int LA31_1 = input.LA(2);

                        if ( ((LA31_1 >= KW_DAY && LA31_1 <= KW_DAYS)||(LA31_1 >= KW_HOUR && LA31_1 <= KW_HOURS)||(LA31_1 >= KW_MICROSECOND && LA31_1 <= KW_MILLISECONDS)||(LA31_1 >= KW_MINUTE && LA31_1 <= KW_NANOSECONDS)||(LA31_1 >= KW_SECOND && LA31_1 <= KW_SECONDS)) ) {
                            alt31=1;
                        }
                        else if ( (LA31_1==KW_ON) ) {
                            alt31=2;
                        }
                        else {
                            if (state.backtracking>0) {state.failed=true; return retval;}
                            NoViableAltException nvae =
                                new NoViableAltException("", 31, 1, input);

                            throw nvae;

                        }
                        }
                        break;
                    case KW_DOUBLE:
                    case KW_FLOAT:
                    case KW_INTERVAL:
                    case KW_NULL:
                    case StringLiteral:
                        {
                        alt31=2;
                        }
                        break;
                    case KW_UNBOUNDED:
                        {
                        alt31=3;
                        }
                        break;
                    default:
                        if (state.backtracking>0) {state.failed=true; return retval;}
                        NoViableAltException nvae =
                            new NoViableAltException("", 31, 0, input);

                        throw nvae;

                    }

                    switch (alt31) {
                        case 1 :
                            // CongerCQL.g:231:4: time_spec ( KW_SLIDE time_spec )?
                            {
                            pushFollow(FOLLOW_time_spec_in_window_type1519);
                            time_spec122=time_spec();

                            state._fsp--;
                            if (state.failed) return retval;
                            if ( state.backtracking==0 ) adaptor.addChild(root_0, time_spec122.getTree());

                            // CongerCQL.g:231:14: ( KW_SLIDE time_spec )?
                            int alt30=2;
                            int LA30_0 = input.LA(1);

                            if ( (LA30_0==KW_SLIDE) ) {
                                alt30=1;
                            }
                            switch (alt30) {
                                case 1 :
                                    // CongerCQL.g:231:16: KW_SLIDE time_spec
                                    {
                                    KW_SLIDE123=(Token)match(input,KW_SLIDE,FOLLOW_KW_SLIDE_in_window_type1523); if (state.failed) return retval;
                                    if ( state.backtracking==0 ) {
                                    KW_SLIDE123_tree = 
                                    (CommonTree)adaptor.create(KW_SLIDE123)
                                    ;
                                    adaptor.addChild(root_0, KW_SLIDE123_tree);
                                    }

                                    pushFollow(FOLLOW_time_spec_in_window_type1525);
                                    time_spec124=time_spec();

                                    state._fsp--;
                                    if (state.failed) return retval;
                                    if ( state.backtracking==0 ) adaptor.addChild(root_0, time_spec124.getTree());

                                    }
                                    break;

                            }


                            }
                            break;
                        case 2 :
                            // CongerCQL.g:232:5: const_value KW_ON Identifier
                            {
                            pushFollow(FOLLOW_const_value_in_window_type1533);
                            const_value125=const_value();

                            state._fsp--;
                            if (state.failed) return retval;
                            if ( state.backtracking==0 ) adaptor.addChild(root_0, const_value125.getTree());

                            KW_ON126=(Token)match(input,KW_ON,FOLLOW_KW_ON_in_window_type1535); if (state.failed) return retval;
                            if ( state.backtracking==0 ) {
                            KW_ON126_tree = 
                            (CommonTree)adaptor.create(KW_ON126)
                            ;
                            adaptor.addChild(root_0, KW_ON126_tree);
                            }

                            Identifier127=(Token)match(input,Identifier,FOLLOW_Identifier_in_window_type1537); if (state.failed) return retval;
                            if ( state.backtracking==0 ) {
                            Identifier127_tree = 
                            (CommonTree)adaptor.create(Identifier127)
                            ;
                            adaptor.addChild(root_0, Identifier127_tree);
                            }

                            }
                            break;
                        case 3 :
                            // CongerCQL.g:233:5: KW_UNBOUNDED
                            {
                            KW_UNBOUNDED128=(Token)match(input,KW_UNBOUNDED,FOLLOW_KW_UNBOUNDED_in_window_type1544); if (state.failed) return retval;
                            if ( state.backtracking==0 ) {
                            KW_UNBOUNDED128_tree = 
                            (CommonTree)adaptor.create(KW_UNBOUNDED128)
                            ;
                            adaptor.addChild(root_0, KW_UNBOUNDED128_tree);
                            }

                            }
                            break;

                    }


                    }
                    break;
                case 2 :
                    // CongerCQL.g:235:4: KW_NOW
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    KW_NOW129=(Token)match(input,KW_NOW,FOLLOW_KW_NOW_in_window_type1553); if (state.failed) return retval;
                    if ( state.backtracking==0 ) {
                    KW_NOW129_tree = 
                    (CommonTree)adaptor.create(KW_NOW129)
                    ;
                    adaptor.addChild(root_0, KW_NOW129_tree);
                    }

                    }
                    break;
                case 3 :
                    // CongerCQL.g:236:4: KW_ROWS Integer ( KW_SLIDE Integer )?
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    KW_ROWS130=(Token)match(input,KW_ROWS,FOLLOW_KW_ROWS_in_window_type1558); if (state.failed) return retval;
                    if ( state.backtracking==0 ) {
                    KW_ROWS130_tree = 
                    (CommonTree)adaptor.create(KW_ROWS130)
                    ;
                    adaptor.addChild(root_0, KW_ROWS130_tree);
                    }

                    Integer131=(Token)match(input,Integer,FOLLOW_Integer_in_window_type1560); if (state.failed) return retval;
                    if ( state.backtracking==0 ) {
                    Integer131_tree = 
                    (CommonTree)adaptor.create(Integer131)
                    ;
                    adaptor.addChild(root_0, Integer131_tree);
                    }

                    // CongerCQL.g:236:20: ( KW_SLIDE Integer )?
                    int alt32=2;
                    int LA32_0 = input.LA(1);

                    if ( (LA32_0==KW_SLIDE) ) {
                        alt32=1;
                    }
                    switch (alt32) {
                        case 1 :
                            // CongerCQL.g:236:22: KW_SLIDE Integer
                            {
                            KW_SLIDE132=(Token)match(input,KW_SLIDE,FOLLOW_KW_SLIDE_in_window_type1564); if (state.failed) return retval;
                            if ( state.backtracking==0 ) {
                            KW_SLIDE132_tree = 
                            (CommonTree)adaptor.create(KW_SLIDE132)
                            ;
                            adaptor.addChild(root_0, KW_SLIDE132_tree);
                            }

                            Integer133=(Token)match(input,Integer,FOLLOW_Integer_in_window_type1566); if (state.failed) return retval;
                            if ( state.backtracking==0 ) {
                            Integer133_tree = 
                            (CommonTree)adaptor.create(Integer133)
                            ;
                            adaptor.addChild(root_0, Integer133_tree);
                            }

                            }
                            break;

                    }


                    }
                    break;
                case 4 :
                    // CongerCQL.g:237:4: KW_PARTITION KW_BY non_mt_attr_list KW_ROWS rows= Integer ( KW_RANGE time_spec ( KW_SLIDE time_spec )? )?
                    {
                    KW_PARTITION134=(Token)match(input,KW_PARTITION,FOLLOW_KW_PARTITION_in_window_type1573); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_KW_PARTITION.add(KW_PARTITION134);


                    KW_BY135=(Token)match(input,KW_BY,FOLLOW_KW_BY_in_window_type1575); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_KW_BY.add(KW_BY135);


                    pushFollow(FOLLOW_non_mt_attr_list_in_window_type1577);
                    non_mt_attr_list136=non_mt_attr_list();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_non_mt_attr_list.add(non_mt_attr_list136.getTree());

                    KW_ROWS137=(Token)match(input,KW_ROWS,FOLLOW_KW_ROWS_in_window_type1579); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_KW_ROWS.add(KW_ROWS137);


                    rows=(Token)match(input,Integer,FOLLOW_Integer_in_window_type1586); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_Integer.add(rows);


                    // CongerCQL.g:238:16: ( KW_RANGE time_spec ( KW_SLIDE time_spec )? )?
                    int alt34=2;
                    int LA34_0 = input.LA(1);

                    if ( (LA34_0==KW_RANGE) ) {
                        alt34=1;
                    }
                    switch (alt34) {
                        case 1 :
                            // CongerCQL.g:238:17: KW_RANGE time_spec ( KW_SLIDE time_spec )?
                            {
                            KW_RANGE138=(Token)match(input,KW_RANGE,FOLLOW_KW_RANGE_in_window_type1589); if (state.failed) return retval; 
                            if ( state.backtracking==0 ) stream_KW_RANGE.add(KW_RANGE138);


                            pushFollow(FOLLOW_time_spec_in_window_type1591);
                            time_spec139=time_spec();

                            state._fsp--;
                            if (state.failed) return retval;
                            if ( state.backtracking==0 ) stream_time_spec.add(time_spec139.getTree());

                            // CongerCQL.g:238:36: ( KW_SLIDE time_spec )?
                            int alt33=2;
                            int LA33_0 = input.LA(1);

                            if ( (LA33_0==KW_SLIDE) ) {
                                alt33=1;
                            }
                            switch (alt33) {
                                case 1 :
                                    // CongerCQL.g:238:37: KW_SLIDE time_spec
                                    {
                                    KW_SLIDE140=(Token)match(input,KW_SLIDE,FOLLOW_KW_SLIDE_in_window_type1594); if (state.failed) return retval; 
                                    if ( state.backtracking==0 ) stream_KW_SLIDE.add(KW_SLIDE140);


                                    pushFollow(FOLLOW_time_spec_in_window_type1596);
                                    time_spec141=time_spec();

                                    state._fsp--;
                                    if (state.failed) return retval;
                                    if ( state.backtracking==0 ) stream_time_spec.add(time_spec141.getTree());

                                    }
                                    break;

                            }


                            }
                            break;

                    }


                    // AST REWRITE
                    // elements: time_spec, KW_RANGE, rows, time_spec, KW_SLIDE, non_mt_attr_list
                    // token labels: rows
                    // rule labels: retval
                    // token list labels: 
                    // rule list labels: 
                    // wildcard labels: 
                    if ( state.backtracking==0 ) {

                    retval.tree = root_0;
                    RewriteRuleTokenStream stream_rows=new RewriteRuleTokenStream(adaptor,"token rows",rows);
                    RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

                    root_0 = (CommonTree)adaptor.nil();
                    // 239:9: -> ^( TOK_PARTITION non_mt_attr_list $rows ( KW_RANGE time_spec ( KW_SLIDE time_spec )? )? )
                    {
                        // CongerCQL.g:239:12: ^( TOK_PARTITION non_mt_attr_list $rows ( KW_RANGE time_spec ( KW_SLIDE time_spec )? )? )
                        {
                        CommonTree root_1 = (CommonTree)adaptor.nil();
                        root_1 = (CommonTree)adaptor.becomeRoot(
                        (CommonTree)adaptor.create(TOK_PARTITION, "TOK_PARTITION")
                        , root_1);

                        adaptor.addChild(root_1, stream_non_mt_attr_list.nextTree());

                        adaptor.addChild(root_1, stream_rows.nextNode());

                        // CongerCQL.g:239:51: ( KW_RANGE time_spec ( KW_SLIDE time_spec )? )?
                        if ( stream_time_spec.hasNext()||stream_KW_RANGE.hasNext()||stream_KW_SLIDE.hasNext()||stream_time_spec.hasNext() ) {
                            adaptor.addChild(root_1, 
                            stream_KW_RANGE.nextNode()
                            );

                            adaptor.addChild(root_1, stream_time_spec.nextTree());

                            // CongerCQL.g:239:71: ( KW_SLIDE time_spec )?
                            if ( stream_time_spec.hasNext()||stream_KW_SLIDE.hasNext() ) {
                                adaptor.addChild(root_1, 
                                stream_KW_SLIDE.nextNode()
                                );

                                adaptor.addChild(root_1, stream_time_spec.nextTree());

                            }
                            stream_time_spec.reset();
                            stream_KW_SLIDE.reset();

                        }
                        stream_time_spec.reset();
                        stream_KW_RANGE.reset();
                        stream_KW_SLIDE.reset();
                        stream_time_spec.reset();

                        adaptor.addChild(root_0, root_1);
                        }

                    }


                    retval.tree = root_0;
                    }

                    }
                    break;

            }
            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "window_type"


    public static class non_mt_attr_list_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "non_mt_attr_list"
    // CongerCQL.g:242:1: non_mt_attr_list : attr ( options {greedy=true; } : COMMA non_mt_attr_list )* -> ^( TOK_ATTR_LIST ( attr )+ ) ;
    public final CongerCQLParser.non_mt_attr_list_return non_mt_attr_list() throws RecognitionException {
        CongerCQLParser.non_mt_attr_list_return retval = new CongerCQLParser.non_mt_attr_list_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token COMMA143=null;
        CongerCQLParser.attr_return attr142 =null;

        CongerCQLParser.non_mt_attr_list_return non_mt_attr_list144 =null;


        CommonTree COMMA143_tree=null;
        RewriteRuleTokenStream stream_COMMA=new RewriteRuleTokenStream(adaptor,"token COMMA");
        RewriteRuleSubtreeStream stream_non_mt_attr_list=new RewriteRuleSubtreeStream(adaptor,"rule non_mt_attr_list");
        RewriteRuleSubtreeStream stream_attr=new RewriteRuleSubtreeStream(adaptor,"rule attr");
        try {
            // CongerCQL.g:243:2: ( attr ( options {greedy=true; } : COMMA non_mt_attr_list )* -> ^( TOK_ATTR_LIST ( attr )+ ) )
            // CongerCQL.g:243:4: attr ( options {greedy=true; } : COMMA non_mt_attr_list )*
            {
            pushFollow(FOLLOW_attr_in_non_mt_attr_list1645);
            attr142=attr();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_attr.add(attr142.getTree());

            // CongerCQL.g:243:9: ( options {greedy=true; } : COMMA non_mt_attr_list )*
            loop36:
            do {
                int alt36=2;
                int LA36_0 = input.LA(1);

                if ( (LA36_0==COMMA) ) {
                    alt36=1;
                }


                switch (alt36) {
            	case 1 :
            	    // CongerCQL.g:243:33: COMMA non_mt_attr_list
            	    {
            	    COMMA143=(Token)match(input,COMMA,FOLLOW_COMMA_in_non_mt_attr_list1656); if (state.failed) return retval; 
            	    if ( state.backtracking==0 ) stream_COMMA.add(COMMA143);


            	    pushFollow(FOLLOW_non_mt_attr_list_in_non_mt_attr_list1658);
            	    non_mt_attr_list144=non_mt_attr_list();

            	    state._fsp--;
            	    if (state.failed) return retval;
            	    if ( state.backtracking==0 ) stream_non_mt_attr_list.add(non_mt_attr_list144.getTree());

            	    }
            	    break;

            	default :
            	    break loop36;
                }
            } while (true);


            // AST REWRITE
            // elements: attr
            // token labels: 
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 244:9: -> ^( TOK_ATTR_LIST ( attr )+ )
            {
                // CongerCQL.g:244:12: ^( TOK_ATTR_LIST ( attr )+ )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_ATTR_LIST, "TOK_ATTR_LIST")
                , root_1);

                if ( !(stream_attr.hasNext()) ) {
                    throw new RewriteEarlyExitException();
                }
                while ( stream_attr.hasNext() ) {
                    adaptor.addChild(root_1, stream_attr.nextTree());

                }
                stream_attr.reset();

                adaptor.addChild(root_0, root_1);
                }

            }


            retval.tree = root_0;
            }

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "non_mt_attr_list"


    public static class const_value_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "const_value"
    // CongerCQL.g:247:1: const_value : ( interval_value | const_string | KW_NULL | const_int | KW_FLOAT | KW_DOUBLE );
    public final CongerCQLParser.const_value_return const_value() throws RecognitionException {
        CongerCQLParser.const_value_return retval = new CongerCQLParser.const_value_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token KW_NULL147=null;
        Token KW_FLOAT149=null;
        Token KW_DOUBLE150=null;
        CongerCQLParser.interval_value_return interval_value145 =null;

        CongerCQLParser.const_string_return const_string146 =null;

        CongerCQLParser.const_int_return const_int148 =null;


        CommonTree KW_NULL147_tree=null;
        CommonTree KW_FLOAT149_tree=null;
        CommonTree KW_DOUBLE150_tree=null;

        try {
            // CongerCQL.g:248:2: ( interval_value | const_string | KW_NULL | const_int | KW_FLOAT | KW_DOUBLE )
            int alt37=6;
            switch ( input.LA(1) ) {
            case KW_INTERVAL:
                {
                alt37=1;
                }
                break;
            case StringLiteral:
                {
                alt37=2;
                }
                break;
            case KW_NULL:
                {
                alt37=3;
                }
                break;
            case Integer:
                {
                alt37=4;
                }
                break;
            case KW_FLOAT:
                {
                alt37=5;
                }
                break;
            case KW_DOUBLE:
                {
                alt37=6;
                }
                break;
            default:
                if (state.backtracking>0) {state.failed=true; return retval;}
                NoViableAltException nvae =
                    new NoViableAltException("", 37, 0, input);

                throw nvae;

            }

            switch (alt37) {
                case 1 :
                    // CongerCQL.g:248:4: interval_value
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    pushFollow(FOLLOW_interval_value_in_const_value1688);
                    interval_value145=interval_value();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, interval_value145.getTree());

                    }
                    break;
                case 2 :
                    // CongerCQL.g:249:4: const_string
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    pushFollow(FOLLOW_const_string_in_const_value1693);
                    const_string146=const_string();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, const_string146.getTree());

                    }
                    break;
                case 3 :
                    // CongerCQL.g:250:4: KW_NULL
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    KW_NULL147=(Token)match(input,KW_NULL,FOLLOW_KW_NULL_in_const_value1698); if (state.failed) return retval;
                    if ( state.backtracking==0 ) {
                    KW_NULL147_tree = 
                    (CommonTree)adaptor.create(KW_NULL147)
                    ;
                    adaptor.addChild(root_0, KW_NULL147_tree);
                    }

                    }
                    break;
                case 4 :
                    // CongerCQL.g:251:4: const_int
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    pushFollow(FOLLOW_const_int_in_const_value1703);
                    const_int148=const_int();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, const_int148.getTree());

                    }
                    break;
                case 5 :
                    // CongerCQL.g:252:4: KW_FLOAT
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    KW_FLOAT149=(Token)match(input,KW_FLOAT,FOLLOW_KW_FLOAT_in_const_value1708); if (state.failed) return retval;
                    if ( state.backtracking==0 ) {
                    KW_FLOAT149_tree = 
                    (CommonTree)adaptor.create(KW_FLOAT149)
                    ;
                    adaptor.addChild(root_0, KW_FLOAT149_tree);
                    }

                    }
                    break;
                case 6 :
                    // CongerCQL.g:253:4: KW_DOUBLE
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    KW_DOUBLE150=(Token)match(input,KW_DOUBLE,FOLLOW_KW_DOUBLE_in_const_value1713); if (state.failed) return retval;
                    if ( state.backtracking==0 ) {
                    KW_DOUBLE150_tree = 
                    (CommonTree)adaptor.create(KW_DOUBLE150)
                    ;
                    adaptor.addChild(root_0, KW_DOUBLE150_tree);
                    }

                    }
                    break;

            }
            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "const_value"


    public static class interval_value_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "interval_value"
    // CongerCQL.g:256:1: interval_value : KW_INTERVAL const_string ( KW_DAY | KW_DAYS ) KW_TO ( KW_SECOND | KW_SECONDS ) ;
    public final CongerCQLParser.interval_value_return interval_value() throws RecognitionException {
        CongerCQLParser.interval_value_return retval = new CongerCQLParser.interval_value_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token KW_INTERVAL151=null;
        Token set153=null;
        Token KW_TO154=null;
        Token set155=null;
        CongerCQLParser.const_string_return const_string152 =null;


        CommonTree KW_INTERVAL151_tree=null;
        CommonTree set153_tree=null;
        CommonTree KW_TO154_tree=null;
        CommonTree set155_tree=null;

        try {
            // CongerCQL.g:257:2: ( KW_INTERVAL const_string ( KW_DAY | KW_DAYS ) KW_TO ( KW_SECOND | KW_SECONDS ) )
            // CongerCQL.g:257:4: KW_INTERVAL const_string ( KW_DAY | KW_DAYS ) KW_TO ( KW_SECOND | KW_SECONDS )
            {
            root_0 = (CommonTree)adaptor.nil();


            KW_INTERVAL151=(Token)match(input,KW_INTERVAL,FOLLOW_KW_INTERVAL_in_interval_value1724); if (state.failed) return retval;
            if ( state.backtracking==0 ) {
            KW_INTERVAL151_tree = 
            (CommonTree)adaptor.create(KW_INTERVAL151)
            ;
            adaptor.addChild(root_0, KW_INTERVAL151_tree);
            }

            pushFollow(FOLLOW_const_string_in_interval_value1726);
            const_string152=const_string();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) adaptor.addChild(root_0, const_string152.getTree());

            set153=(Token)input.LT(1);

            if ( (input.LA(1) >= KW_DAY && input.LA(1) <= KW_DAYS) ) {
                input.consume();
                if ( state.backtracking==0 ) adaptor.addChild(root_0, 
                (CommonTree)adaptor.create(set153)
                );
                state.errorRecovery=false;
                state.failed=false;
            }
            else {
                if (state.backtracking>0) {state.failed=true; return retval;}
                MismatchedSetException mse = new MismatchedSetException(null,input);
                throw mse;
            }


            KW_TO154=(Token)match(input,KW_TO,FOLLOW_KW_TO_in_interval_value1736); if (state.failed) return retval;
            if ( state.backtracking==0 ) {
            KW_TO154_tree = 
            (CommonTree)adaptor.create(KW_TO154)
            ;
            adaptor.addChild(root_0, KW_TO154_tree);
            }

            set155=(Token)input.LT(1);

            if ( (input.LA(1) >= KW_SECOND && input.LA(1) <= KW_SECONDS) ) {
                input.consume();
                if ( state.backtracking==0 ) adaptor.addChild(root_0, 
                (CommonTree)adaptor.create(set155)
                );
                state.errorRecovery=false;
                state.failed=false;
            }
            else {
                if (state.backtracking>0) {state.failed=true; return retval;}
                MismatchedSetException mse = new MismatchedSetException(null,input);
                throw mse;
            }


            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "interval_value"


    public static class time_spec_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "time_spec"
    // CongerCQL.g:260:1: time_spec : Integer time_unit ;
    public final CongerCQLParser.time_spec_return time_spec() throws RecognitionException {
        CongerCQLParser.time_spec_return retval = new CongerCQLParser.time_spec_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token Integer156=null;
        CongerCQLParser.time_unit_return time_unit157 =null;


        CommonTree Integer156_tree=null;

        try {
            // CongerCQL.g:261:2: ( Integer time_unit )
            // CongerCQL.g:261:4: Integer time_unit
            {
            root_0 = (CommonTree)adaptor.nil();


            Integer156=(Token)match(input,Integer,FOLLOW_Integer_in_time_spec1753); if (state.failed) return retval;
            if ( state.backtracking==0 ) {
            Integer156_tree = 
            (CommonTree)adaptor.create(Integer156)
            ;
            adaptor.addChild(root_0, Integer156_tree);
            }

            pushFollow(FOLLOW_time_unit_in_time_spec1755);
            time_unit157=time_unit();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) adaptor.addChild(root_0, time_unit157.getTree());

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "time_spec"


    public static class time_unit_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "time_unit"
    // CongerCQL.g:264:1: time_unit : ( KW_DAY | KW_DAYS | KW_HOUR | KW_HOURS | KW_MINUTE | KW_MINUTES | KW_SECOND | KW_SECONDS | KW_MILLISECOND | KW_MILLISECONDS | KW_MICROSECOND | KW_MICROSECONDS | KW_NANOSECOND | KW_NANOSECONDS );
    public final CongerCQLParser.time_unit_return time_unit() throws RecognitionException {
        CongerCQLParser.time_unit_return retval = new CongerCQLParser.time_unit_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token set158=null;

        CommonTree set158_tree=null;

        try {
            // CongerCQL.g:265:2: ( KW_DAY | KW_DAYS | KW_HOUR | KW_HOURS | KW_MINUTE | KW_MINUTES | KW_SECOND | KW_SECONDS | KW_MILLISECOND | KW_MILLISECONDS | KW_MICROSECOND | KW_MICROSECONDS | KW_NANOSECOND | KW_NANOSECONDS )
            // CongerCQL.g:
            {
            root_0 = (CommonTree)adaptor.nil();


            set158=(Token)input.LT(1);

            if ( (input.LA(1) >= KW_DAY && input.LA(1) <= KW_DAYS)||(input.LA(1) >= KW_HOUR && input.LA(1) <= KW_HOURS)||(input.LA(1) >= KW_MICROSECOND && input.LA(1) <= KW_MILLISECONDS)||(input.LA(1) >= KW_MINUTE && input.LA(1) <= KW_NANOSECONDS)||(input.LA(1) >= KW_SECOND && input.LA(1) <= KW_SECONDS) ) {
                input.consume();
                if ( state.backtracking==0 ) adaptor.addChild(root_0, 
                (CommonTree)adaptor.create(set158)
                );
                state.errorRecovery=false;
                state.failed=false;
            }
            else {
                if (state.backtracking>0) {state.failed=true; return retval;}
                MismatchedSetException mse = new MismatchedSetException(null,input);
                throw mse;
            }


            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "time_unit"


    public static class aggr_distinct_expr_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "aggr_distinct_expr"
    // CongerCQL.g:281:1: aggr_distinct_expr : aggr_distinct_operator LPAREN KW_DISTINCT arith_expr RPAREN -> ^( TOK_AGGR_DISTINCT aggr_distinct_operator arith_expr ) ;
    public final CongerCQLParser.aggr_distinct_expr_return aggr_distinct_expr() throws RecognitionException {
        CongerCQLParser.aggr_distinct_expr_return retval = new CongerCQLParser.aggr_distinct_expr_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token LPAREN160=null;
        Token KW_DISTINCT161=null;
        Token RPAREN163=null;
        CongerCQLParser.aggr_distinct_operator_return aggr_distinct_operator159 =null;

        CongerCQLParser.arith_expr_return arith_expr162 =null;


        CommonTree LPAREN160_tree=null;
        CommonTree KW_DISTINCT161_tree=null;
        CommonTree RPAREN163_tree=null;
        RewriteRuleTokenStream stream_RPAREN=new RewriteRuleTokenStream(adaptor,"token RPAREN");
        RewriteRuleTokenStream stream_KW_DISTINCT=new RewriteRuleTokenStream(adaptor,"token KW_DISTINCT");
        RewriteRuleTokenStream stream_LPAREN=new RewriteRuleTokenStream(adaptor,"token LPAREN");
        RewriteRuleSubtreeStream stream_aggr_distinct_operator=new RewriteRuleSubtreeStream(adaptor,"rule aggr_distinct_operator");
        RewriteRuleSubtreeStream stream_arith_expr=new RewriteRuleSubtreeStream(adaptor,"rule arith_expr");
        try {
            // CongerCQL.g:282:2: ( aggr_distinct_operator LPAREN KW_DISTINCT arith_expr RPAREN -> ^( TOK_AGGR_DISTINCT aggr_distinct_operator arith_expr ) )
            // CongerCQL.g:282:4: aggr_distinct_operator LPAREN KW_DISTINCT arith_expr RPAREN
            {
            pushFollow(FOLLOW_aggr_distinct_operator_in_aggr_distinct_expr1842);
            aggr_distinct_operator159=aggr_distinct_operator();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_aggr_distinct_operator.add(aggr_distinct_operator159.getTree());

            LPAREN160=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_aggr_distinct_expr1844); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_LPAREN.add(LPAREN160);


            KW_DISTINCT161=(Token)match(input,KW_DISTINCT,FOLLOW_KW_DISTINCT_in_aggr_distinct_expr1846); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_KW_DISTINCT.add(KW_DISTINCT161);


            pushFollow(FOLLOW_arith_expr_in_aggr_distinct_expr1848);
            arith_expr162=arith_expr();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_arith_expr.add(arith_expr162.getTree());

            RPAREN163=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_aggr_distinct_expr1850); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_RPAREN.add(RPAREN163);


            // AST REWRITE
            // elements: arith_expr, aggr_distinct_operator
            // token labels: 
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 283:9: -> ^( TOK_AGGR_DISTINCT aggr_distinct_operator arith_expr )
            {
                // CongerCQL.g:283:12: ^( TOK_AGGR_DISTINCT aggr_distinct_operator arith_expr )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_AGGR_DISTINCT, "TOK_AGGR_DISTINCT")
                , root_1);

                adaptor.addChild(root_1, stream_aggr_distinct_operator.nextTree());

                adaptor.addChild(root_1, stream_arith_expr.nextTree());

                adaptor.addChild(root_0, root_1);
                }

            }


            retval.tree = root_0;
            }

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "aggr_distinct_expr"


    public static class aggr_distinct_operator_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "aggr_distinct_operator"
    // CongerCQL.g:285:1: aggr_distinct_operator : ( KW_COUNT | KW_SUM | KW_AVG | KW_MAX | KW_MIN );
    public final CongerCQLParser.aggr_distinct_operator_return aggr_distinct_operator() throws RecognitionException {
        CongerCQLParser.aggr_distinct_operator_return retval = new CongerCQLParser.aggr_distinct_operator_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token set164=null;

        CommonTree set164_tree=null;

        try {
            // CongerCQL.g:286:2: ( KW_COUNT | KW_SUM | KW_AVG | KW_MAX | KW_MIN )
            // CongerCQL.g:
            {
            root_0 = (CommonTree)adaptor.nil();


            set164=(Token)input.LT(1);

            if ( input.LA(1)==KW_AVG||input.LA(1)==KW_COUNT||input.LA(1)==KW_MAX||input.LA(1)==KW_MIN||input.LA(1)==KW_SUM ) {
                input.consume();
                if ( state.backtracking==0 ) adaptor.addChild(root_0, 
                (CommonTree)adaptor.create(set164)
                );
                state.errorRecovery=false;
                state.failed=false;
            }
            else {
                if (state.backtracking>0) {state.failed=true; return retval;}
                MismatchedSetException mse = new MismatchedSetException(null,input);
                throw mse;
            }


            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "aggr_distinct_operator"


    public static class aggr_expr_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "aggr_expr"
    // CongerCQL.g:288:1: aggr_expr : ( KW_COUNT LPAREN ( arith_expr | STAR ) RPAREN -> ^( TOK_AGGR KW_COUNT ( arith_expr )? ( STAR )? ) | ( ( KW_SUM | KW_AVG ) LPAREN arith_expr | ( KW_MAX | KW_MIN ) LPAREN arith_expr ) RPAREN -> ^( TOK_AGGR ( KW_SUM )? ( KW_AVG )? KW_MAX ( KW_MIN )? arith_expr ) );
    public final CongerCQLParser.aggr_expr_return aggr_expr() throws RecognitionException {
        CongerCQLParser.aggr_expr_return retval = new CongerCQLParser.aggr_expr_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token KW_COUNT165=null;
        Token LPAREN166=null;
        Token STAR168=null;
        Token RPAREN169=null;
        Token KW_SUM170=null;
        Token KW_AVG171=null;
        Token LPAREN172=null;
        Token KW_MAX174=null;
        Token KW_MIN175=null;
        Token LPAREN176=null;
        Token RPAREN178=null;
        CongerCQLParser.arith_expr_return arith_expr167 =null;

        CongerCQLParser.arith_expr_return arith_expr173 =null;

        CongerCQLParser.arith_expr_return arith_expr177 =null;


        CommonTree KW_COUNT165_tree=null;
        CommonTree LPAREN166_tree=null;
        CommonTree STAR168_tree=null;
        CommonTree RPAREN169_tree=null;
        CommonTree KW_SUM170_tree=null;
        CommonTree KW_AVG171_tree=null;
        CommonTree LPAREN172_tree=null;
        CommonTree KW_MAX174_tree=null;
        CommonTree KW_MIN175_tree=null;
        CommonTree LPAREN176_tree=null;
        CommonTree RPAREN178_tree=null;
        RewriteRuleTokenStream stream_KW_COUNT=new RewriteRuleTokenStream(adaptor,"token KW_COUNT");
        RewriteRuleTokenStream stream_KW_MAX=new RewriteRuleTokenStream(adaptor,"token KW_MAX");
        RewriteRuleTokenStream stream_RPAREN=new RewriteRuleTokenStream(adaptor,"token RPAREN");
        RewriteRuleTokenStream stream_KW_MIN=new RewriteRuleTokenStream(adaptor,"token KW_MIN");
        RewriteRuleTokenStream stream_STAR=new RewriteRuleTokenStream(adaptor,"token STAR");
        RewriteRuleTokenStream stream_KW_SUM=new RewriteRuleTokenStream(adaptor,"token KW_SUM");
        RewriteRuleTokenStream stream_KW_AVG=new RewriteRuleTokenStream(adaptor,"token KW_AVG");
        RewriteRuleTokenStream stream_LPAREN=new RewriteRuleTokenStream(adaptor,"token LPAREN");
        RewriteRuleSubtreeStream stream_arith_expr=new RewriteRuleSubtreeStream(adaptor,"rule arith_expr");
        try {
            // CongerCQL.g:289:2: ( KW_COUNT LPAREN ( arith_expr | STAR ) RPAREN -> ^( TOK_AGGR KW_COUNT ( arith_expr )? ( STAR )? ) | ( ( KW_SUM | KW_AVG ) LPAREN arith_expr | ( KW_MAX | KW_MIN ) LPAREN arith_expr ) RPAREN -> ^( TOK_AGGR ( KW_SUM )? ( KW_AVG )? KW_MAX ( KW_MIN )? arith_expr ) )
            int alt42=2;
            int LA42_0 = input.LA(1);

            if ( (LA42_0==KW_COUNT) ) {
                alt42=1;
            }
            else if ( (LA42_0==KW_AVG||LA42_0==KW_MAX||LA42_0==KW_MIN||LA42_0==KW_SUM) ) {
                alt42=2;
            }
            else {
                if (state.backtracking>0) {state.failed=true; return retval;}
                NoViableAltException nvae =
                    new NoViableAltException("", 42, 0, input);

                throw nvae;

            }
            switch (alt42) {
                case 1 :
                    // CongerCQL.g:289:4: KW_COUNT LPAREN ( arith_expr | STAR ) RPAREN
                    {
                    KW_COUNT165=(Token)match(input,KW_COUNT,FOLLOW_KW_COUNT_in_aggr_expr1904); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_KW_COUNT.add(KW_COUNT165);


                    LPAREN166=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_aggr_expr1906); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_LPAREN.add(LPAREN166);


                    // CongerCQL.g:289:20: ( arith_expr | STAR )
                    int alt38=2;
                    int LA38_0 = input.LA(1);

                    if ( ((LA38_0 >= Identifier && LA38_0 <= Integer)||LA38_0==KW_AVG||LA38_0==KW_COUNT||LA38_0==KW_DOUBLE||LA38_0==KW_ELEMENT_TIME||LA38_0==KW_FLOAT||LA38_0==KW_INTERVAL||LA38_0==KW_MAX||LA38_0==KW_MIN||LA38_0==KW_NULL||LA38_0==KW_SUM||LA38_0==LPAREN||LA38_0==StringLiteral) ) {
                        alt38=1;
                    }
                    else if ( (LA38_0==STAR) ) {
                        alt38=2;
                    }
                    else {
                        if (state.backtracking>0) {state.failed=true; return retval;}
                        NoViableAltException nvae =
                            new NoViableAltException("", 38, 0, input);

                        throw nvae;

                    }
                    switch (alt38) {
                        case 1 :
                            // CongerCQL.g:289:22: arith_expr
                            {
                            pushFollow(FOLLOW_arith_expr_in_aggr_expr1910);
                            arith_expr167=arith_expr();

                            state._fsp--;
                            if (state.failed) return retval;
                            if ( state.backtracking==0 ) stream_arith_expr.add(arith_expr167.getTree());

                            }
                            break;
                        case 2 :
                            // CongerCQL.g:289:35: STAR
                            {
                            STAR168=(Token)match(input,STAR,FOLLOW_STAR_in_aggr_expr1914); if (state.failed) return retval; 
                            if ( state.backtracking==0 ) stream_STAR.add(STAR168);


                            }
                            break;

                    }


                    RPAREN169=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_aggr_expr1917); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_RPAREN.add(RPAREN169);


                    // AST REWRITE
                    // elements: KW_COUNT, arith_expr, STAR
                    // token labels: 
                    // rule labels: retval
                    // token list labels: 
                    // rule list labels: 
                    // wildcard labels: 
                    if ( state.backtracking==0 ) {

                    retval.tree = root_0;
                    RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

                    root_0 = (CommonTree)adaptor.nil();
                    // 290:9: -> ^( TOK_AGGR KW_COUNT ( arith_expr )? ( STAR )? )
                    {
                        // CongerCQL.g:290:12: ^( TOK_AGGR KW_COUNT ( arith_expr )? ( STAR )? )
                        {
                        CommonTree root_1 = (CommonTree)adaptor.nil();
                        root_1 = (CommonTree)adaptor.becomeRoot(
                        (CommonTree)adaptor.create(TOK_AGGR, "TOK_AGGR")
                        , root_1);

                        adaptor.addChild(root_1, 
                        stream_KW_COUNT.nextNode()
                        );

                        // CongerCQL.g:290:32: ( arith_expr )?
                        if ( stream_arith_expr.hasNext() ) {
                            adaptor.addChild(root_1, stream_arith_expr.nextTree());

                        }
                        stream_arith_expr.reset();

                        // CongerCQL.g:290:44: ( STAR )?
                        if ( stream_STAR.hasNext() ) {
                            adaptor.addChild(root_1, 
                            stream_STAR.nextNode()
                            );

                        }
                        stream_STAR.reset();

                        adaptor.addChild(root_0, root_1);
                        }

                    }


                    retval.tree = root_0;
                    }

                    }
                    break;
                case 2 :
                    // CongerCQL.g:291:4: ( ( KW_SUM | KW_AVG ) LPAREN arith_expr | ( KW_MAX | KW_MIN ) LPAREN arith_expr ) RPAREN
                    {
                    // CongerCQL.g:291:4: ( ( KW_SUM | KW_AVG ) LPAREN arith_expr | ( KW_MAX | KW_MIN ) LPAREN arith_expr )
                    int alt41=2;
                    int LA41_0 = input.LA(1);

                    if ( (LA41_0==KW_AVG||LA41_0==KW_SUM) ) {
                        alt41=1;
                    }
                    else if ( (LA41_0==KW_MAX||LA41_0==KW_MIN) ) {
                        alt41=2;
                    }
                    else {
                        if (state.backtracking>0) {state.failed=true; return retval;}
                        NoViableAltException nvae =
                            new NoViableAltException("", 41, 0, input);

                        throw nvae;

                    }
                    switch (alt41) {
                        case 1 :
                            // CongerCQL.g:291:6: ( KW_SUM | KW_AVG ) LPAREN arith_expr
                            {
                            // CongerCQL.g:291:6: ( KW_SUM | KW_AVG )
                            int alt39=2;
                            int LA39_0 = input.LA(1);

                            if ( (LA39_0==KW_SUM) ) {
                                alt39=1;
                            }
                            else if ( (LA39_0==KW_AVG) ) {
                                alt39=2;
                            }
                            else {
                                if (state.backtracking>0) {state.failed=true; return retval;}
                                NoViableAltException nvae =
                                    new NoViableAltException("", 39, 0, input);

                                throw nvae;

                            }
                            switch (alt39) {
                                case 1 :
                                    // CongerCQL.g:291:7: KW_SUM
                                    {
                                    KW_SUM170=(Token)match(input,KW_SUM,FOLLOW_KW_SUM_in_aggr_expr1947); if (state.failed) return retval; 
                                    if ( state.backtracking==0 ) stream_KW_SUM.add(KW_SUM170);


                                    }
                                    break;
                                case 2 :
                                    // CongerCQL.g:291:16: KW_AVG
                                    {
                                    KW_AVG171=(Token)match(input,KW_AVG,FOLLOW_KW_AVG_in_aggr_expr1951); if (state.failed) return retval; 
                                    if ( state.backtracking==0 ) stream_KW_AVG.add(KW_AVG171);


                                    }
                                    break;

                            }


                            LPAREN172=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_aggr_expr1954); if (state.failed) return retval; 
                            if ( state.backtracking==0 ) stream_LPAREN.add(LPAREN172);


                            pushFollow(FOLLOW_arith_expr_in_aggr_expr1956);
                            arith_expr173=arith_expr();

                            state._fsp--;
                            if (state.failed) return retval;
                            if ( state.backtracking==0 ) stream_arith_expr.add(arith_expr173.getTree());

                            }
                            break;
                        case 2 :
                            // CongerCQL.g:292:5: ( KW_MAX | KW_MIN ) LPAREN arith_expr
                            {
                            // CongerCQL.g:292:5: ( KW_MAX | KW_MIN )
                            int alt40=2;
                            int LA40_0 = input.LA(1);

                            if ( (LA40_0==KW_MAX) ) {
                                alt40=1;
                            }
                            else if ( (LA40_0==KW_MIN) ) {
                                alt40=2;
                            }
                            else {
                                if (state.backtracking>0) {state.failed=true; return retval;}
                                NoViableAltException nvae =
                                    new NoViableAltException("", 40, 0, input);

                                throw nvae;

                            }
                            switch (alt40) {
                                case 1 :
                                    // CongerCQL.g:292:7: KW_MAX
                                    {
                                    KW_MAX174=(Token)match(input,KW_MAX,FOLLOW_KW_MAX_in_aggr_expr1964); if (state.failed) return retval; 
                                    if ( state.backtracking==0 ) stream_KW_MAX.add(KW_MAX174);


                                    }
                                    break;
                                case 2 :
                                    // CongerCQL.g:292:16: KW_MIN
                                    {
                                    KW_MIN175=(Token)match(input,KW_MIN,FOLLOW_KW_MIN_in_aggr_expr1968); if (state.failed) return retval; 
                                    if ( state.backtracking==0 ) stream_KW_MIN.add(KW_MIN175);


                                    }
                                    break;

                            }


                            LPAREN176=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_aggr_expr1971); if (state.failed) return retval; 
                            if ( state.backtracking==0 ) stream_LPAREN.add(LPAREN176);


                            pushFollow(FOLLOW_arith_expr_in_aggr_expr1973);
                            arith_expr177=arith_expr();

                            state._fsp--;
                            if (state.failed) return retval;
                            if ( state.backtracking==0 ) stream_arith_expr.add(arith_expr177.getTree());

                            }
                            break;

                    }


                    RPAREN178=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_aggr_expr1978); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_RPAREN.add(RPAREN178);


                    // AST REWRITE
                    // elements: KW_MIN, KW_SUM, KW_MAX, arith_expr, KW_AVG
                    // token labels: 
                    // rule labels: retval
                    // token list labels: 
                    // rule list labels: 
                    // wildcard labels: 
                    if ( state.backtracking==0 ) {

                    retval.tree = root_0;
                    RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

                    root_0 = (CommonTree)adaptor.nil();
                    // 294:9: -> ^( TOK_AGGR ( KW_SUM )? ( KW_AVG )? KW_MAX ( KW_MIN )? arith_expr )
                    {
                        // CongerCQL.g:294:12: ^( TOK_AGGR ( KW_SUM )? ( KW_AVG )? KW_MAX ( KW_MIN )? arith_expr )
                        {
                        CommonTree root_1 = (CommonTree)adaptor.nil();
                        root_1 = (CommonTree)adaptor.becomeRoot(
                        (CommonTree)adaptor.create(TOK_AGGR, "TOK_AGGR")
                        , root_1);

                        // CongerCQL.g:294:23: ( KW_SUM )?
                        if ( stream_KW_SUM.hasNext() ) {
                            adaptor.addChild(root_1, 
                            stream_KW_SUM.nextNode()
                            );

                        }
                        stream_KW_SUM.reset();

                        // CongerCQL.g:294:31: ( KW_AVG )?
                        if ( stream_KW_AVG.hasNext() ) {
                            adaptor.addChild(root_1, 
                            stream_KW_AVG.nextNode()
                            );

                        }
                        stream_KW_AVG.reset();

                        adaptor.addChild(root_1, 
                        stream_KW_MAX.nextNode()
                        );

                        // CongerCQL.g:294:46: ( KW_MIN )?
                        if ( stream_KW_MIN.hasNext() ) {
                            adaptor.addChild(root_1, 
                            stream_KW_MIN.nextNode()
                            );

                        }
                        stream_KW_MIN.reset();

                        adaptor.addChild(root_1, stream_arith_expr.nextTree());

                        adaptor.addChild(root_0, root_1);
                        }

                    }


                    retval.tree = root_0;
                    }

                    }
                    break;

            }
            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "aggr_expr"


    public static class func_expr_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "func_expr"
    // CongerCQL.g:297:1: func_expr : func_name LPAREN ( non_mt_arg_list | KW_DISTINCT arith_expr ) RPAREN -> ^( TOK_FUNC func_name ( non_mt_arg_list )? ( KW_DISTINCT )? ( arith_expr )? ) ;
    public final CongerCQLParser.func_expr_return func_expr() throws RecognitionException {
        CongerCQLParser.func_expr_return retval = new CongerCQLParser.func_expr_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token LPAREN180=null;
        Token KW_DISTINCT182=null;
        Token RPAREN184=null;
        CongerCQLParser.func_name_return func_name179 =null;

        CongerCQLParser.non_mt_arg_list_return non_mt_arg_list181 =null;

        CongerCQLParser.arith_expr_return arith_expr183 =null;


        CommonTree LPAREN180_tree=null;
        CommonTree KW_DISTINCT182_tree=null;
        CommonTree RPAREN184_tree=null;
        RewriteRuleTokenStream stream_RPAREN=new RewriteRuleTokenStream(adaptor,"token RPAREN");
        RewriteRuleTokenStream stream_KW_DISTINCT=new RewriteRuleTokenStream(adaptor,"token KW_DISTINCT");
        RewriteRuleTokenStream stream_LPAREN=new RewriteRuleTokenStream(adaptor,"token LPAREN");
        RewriteRuleSubtreeStream stream_arith_expr=new RewriteRuleSubtreeStream(adaptor,"rule arith_expr");
        RewriteRuleSubtreeStream stream_func_name=new RewriteRuleSubtreeStream(adaptor,"rule func_name");
        RewriteRuleSubtreeStream stream_non_mt_arg_list=new RewriteRuleSubtreeStream(adaptor,"rule non_mt_arg_list");
        try {
            // CongerCQL.g:298:2: ( func_name LPAREN ( non_mt_arg_list | KW_DISTINCT arith_expr ) RPAREN -> ^( TOK_FUNC func_name ( non_mt_arg_list )? ( KW_DISTINCT )? ( arith_expr )? ) )
            // CongerCQL.g:298:4: func_name LPAREN ( non_mt_arg_list | KW_DISTINCT arith_expr ) RPAREN
            {
            pushFollow(FOLLOW_func_name_in_func_expr2016);
            func_name179=func_name();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_func_name.add(func_name179.getTree());

            LPAREN180=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_func_expr2018); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_LPAREN.add(LPAREN180);


            // CongerCQL.g:298:21: ( non_mt_arg_list | KW_DISTINCT arith_expr )
            int alt43=2;
            int LA43_0 = input.LA(1);

            if ( ((LA43_0 >= Identifier && LA43_0 <= Integer)||LA43_0==KW_AVG||LA43_0==KW_COUNT||LA43_0==KW_DOUBLE||LA43_0==KW_ELEMENT_TIME||LA43_0==KW_FLOAT||LA43_0==KW_INTERVAL||LA43_0==KW_MAX||LA43_0==KW_MIN||LA43_0==KW_NULL||LA43_0==KW_SUM||LA43_0==LPAREN||LA43_0==StringLiteral) ) {
                alt43=1;
            }
            else if ( (LA43_0==KW_DISTINCT) ) {
                alt43=2;
            }
            else {
                if (state.backtracking>0) {state.failed=true; return retval;}
                NoViableAltException nvae =
                    new NoViableAltException("", 43, 0, input);

                throw nvae;

            }
            switch (alt43) {
                case 1 :
                    // CongerCQL.g:298:23: non_mt_arg_list
                    {
                    pushFollow(FOLLOW_non_mt_arg_list_in_func_expr2022);
                    non_mt_arg_list181=non_mt_arg_list();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_non_mt_arg_list.add(non_mt_arg_list181.getTree());

                    }
                    break;
                case 2 :
                    // CongerCQL.g:298:41: KW_DISTINCT arith_expr
                    {
                    KW_DISTINCT182=(Token)match(input,KW_DISTINCT,FOLLOW_KW_DISTINCT_in_func_expr2026); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_KW_DISTINCT.add(KW_DISTINCT182);


                    pushFollow(FOLLOW_arith_expr_in_func_expr2028);
                    arith_expr183=arith_expr();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_arith_expr.add(arith_expr183.getTree());

                    }
                    break;

            }


            RPAREN184=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_func_expr2031); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_RPAREN.add(RPAREN184);


            // AST REWRITE
            // elements: arith_expr, KW_DISTINCT, non_mt_arg_list, func_name
            // token labels: 
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 299:9: -> ^( TOK_FUNC func_name ( non_mt_arg_list )? ( KW_DISTINCT )? ( arith_expr )? )
            {
                // CongerCQL.g:299:12: ^( TOK_FUNC func_name ( non_mt_arg_list )? ( KW_DISTINCT )? ( arith_expr )? )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_FUNC, "TOK_FUNC")
                , root_1);

                adaptor.addChild(root_1, stream_func_name.nextTree());

                // CongerCQL.g:299:33: ( non_mt_arg_list )?
                if ( stream_non_mt_arg_list.hasNext() ) {
                    adaptor.addChild(root_1, stream_non_mt_arg_list.nextTree());

                }
                stream_non_mt_arg_list.reset();

                // CongerCQL.g:299:50: ( KW_DISTINCT )?
                if ( stream_KW_DISTINCT.hasNext() ) {
                    adaptor.addChild(root_1, 
                    stream_KW_DISTINCT.nextNode()
                    );

                }
                stream_KW_DISTINCT.reset();

                // CongerCQL.g:299:63: ( arith_expr )?
                if ( stream_arith_expr.hasNext() ) {
                    adaptor.addChild(root_1, stream_arith_expr.nextTree());

                }
                stream_arith_expr.reset();

                adaptor.addChild(root_0, root_1);
                }

            }


            retval.tree = root_0;
            }

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "func_expr"


    public static class func_name_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "func_name"
    // CongerCQL.g:302:1: func_name : Identifier ;
    public final CongerCQLParser.func_name_return func_name() throws RecognitionException {
        CongerCQLParser.func_name_return retval = new CongerCQLParser.func_name_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token Identifier185=null;

        CommonTree Identifier185_tree=null;

        try {
            // CongerCQL.g:303:2: ( Identifier )
            // CongerCQL.g:303:4: Identifier
            {
            root_0 = (CommonTree)adaptor.nil();


            Identifier185=(Token)match(input,Identifier,FOLLOW_Identifier_in_func_name2067); if (state.failed) return retval;
            if ( state.backtracking==0 ) {
            Identifier185_tree = 
            (CommonTree)adaptor.create(Identifier185)
            ;
            adaptor.addChild(root_0, Identifier185_tree);
            }

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "func_name"


    public static class idstream_clause_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "idstream_clause"
    // CongerCQL.g:306:1: idstream_clause : ( KW_ISTREAM | KW_DSTREAM );
    public final CongerCQLParser.idstream_clause_return idstream_clause() throws RecognitionException {
        CongerCQLParser.idstream_clause_return retval = new CongerCQLParser.idstream_clause_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token set186=null;

        CommonTree set186_tree=null;

        try {
            // CongerCQL.g:307:2: ( KW_ISTREAM | KW_DSTREAM )
            // CongerCQL.g:
            {
            root_0 = (CommonTree)adaptor.nil();


            set186=(Token)input.LT(1);

            if ( input.LA(1)==KW_DSTREAM||input.LA(1)==KW_ISTREAM ) {
                input.consume();
                if ( state.backtracking==0 ) adaptor.addChild(root_0, 
                (CommonTree)adaptor.create(set186)
                );
                state.errorRecovery=false;
                state.failed=false;
            }
            else {
                if (state.backtracking>0) {state.failed=true; return retval;}
                MismatchedSetException mse = new MismatchedSetException(null,input);
                throw mse;
            }


            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "idstream_clause"


    public static class using_clause_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "using_clause"
    // CongerCQL.g:311:1: using_clause : KW_DIFFERENCE KW_USING LPAREN usinglist RPAREN -> ^( TOK_USING usinglist ) ;
    public final CongerCQLParser.using_clause_return using_clause() throws RecognitionException {
        CongerCQLParser.using_clause_return retval = new CongerCQLParser.using_clause_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token KW_DIFFERENCE187=null;
        Token KW_USING188=null;
        Token LPAREN189=null;
        Token RPAREN191=null;
        CongerCQLParser.usinglist_return usinglist190 =null;


        CommonTree KW_DIFFERENCE187_tree=null;
        CommonTree KW_USING188_tree=null;
        CommonTree LPAREN189_tree=null;
        CommonTree RPAREN191_tree=null;
        RewriteRuleTokenStream stream_KW_DIFFERENCE=new RewriteRuleTokenStream(adaptor,"token KW_DIFFERENCE");
        RewriteRuleTokenStream stream_RPAREN=new RewriteRuleTokenStream(adaptor,"token RPAREN");
        RewriteRuleTokenStream stream_KW_USING=new RewriteRuleTokenStream(adaptor,"token KW_USING");
        RewriteRuleTokenStream stream_LPAREN=new RewriteRuleTokenStream(adaptor,"token LPAREN");
        RewriteRuleSubtreeStream stream_usinglist=new RewriteRuleSubtreeStream(adaptor,"rule usinglist");
        try {
            // CongerCQL.g:312:2: ( KW_DIFFERENCE KW_USING LPAREN usinglist RPAREN -> ^( TOK_USING usinglist ) )
            // CongerCQL.g:312:4: KW_DIFFERENCE KW_USING LPAREN usinglist RPAREN
            {
            KW_DIFFERENCE187=(Token)match(input,KW_DIFFERENCE,FOLLOW_KW_DIFFERENCE_in_using_clause2094); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_KW_DIFFERENCE.add(KW_DIFFERENCE187);


            KW_USING188=(Token)match(input,KW_USING,FOLLOW_KW_USING_in_using_clause2096); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_KW_USING.add(KW_USING188);


            LPAREN189=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_using_clause2098); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_LPAREN.add(LPAREN189);


            pushFollow(FOLLOW_usinglist_in_using_clause2100);
            usinglist190=usinglist();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_usinglist.add(usinglist190.getTree());

            RPAREN191=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_using_clause2102); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_RPAREN.add(RPAREN191);


            // AST REWRITE
            // elements: usinglist
            // token labels: 
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 313:9: -> ^( TOK_USING usinglist )
            {
                // CongerCQL.g:313:12: ^( TOK_USING usinglist )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_USING, "TOK_USING")
                , root_1);

                adaptor.addChild(root_1, stream_usinglist.nextTree());

                adaptor.addChild(root_0, root_1);
                }

            }


            retval.tree = root_0;
            }

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "using_clause"


    public static class usinglist_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "usinglist"
    // CongerCQL.g:316:1: usinglist : usingterm ( options {greedy=true; } : COMMA usinglist )* -> ( usingterm )+ ;
    public final CongerCQLParser.usinglist_return usinglist() throws RecognitionException {
        CongerCQLParser.usinglist_return retval = new CongerCQLParser.usinglist_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token COMMA193=null;
        CongerCQLParser.usingterm_return usingterm192 =null;

        CongerCQLParser.usinglist_return usinglist194 =null;


        CommonTree COMMA193_tree=null;
        RewriteRuleTokenStream stream_COMMA=new RewriteRuleTokenStream(adaptor,"token COMMA");
        RewriteRuleSubtreeStream stream_usinglist=new RewriteRuleSubtreeStream(adaptor,"rule usinglist");
        RewriteRuleSubtreeStream stream_usingterm=new RewriteRuleSubtreeStream(adaptor,"rule usingterm");
        try {
            // CongerCQL.g:317:2: ( usingterm ( options {greedy=true; } : COMMA usinglist )* -> ( usingterm )+ )
            // CongerCQL.g:317:4: usingterm ( options {greedy=true; } : COMMA usinglist )*
            {
            pushFollow(FOLLOW_usingterm_in_usinglist2129);
            usingterm192=usingterm();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_usingterm.add(usingterm192.getTree());

            // CongerCQL.g:317:14: ( options {greedy=true; } : COMMA usinglist )*
            loop44:
            do {
                int alt44=2;
                int LA44_0 = input.LA(1);

                if ( (LA44_0==COMMA) ) {
                    alt44=1;
                }


                switch (alt44) {
            	case 1 :
            	    // CongerCQL.g:317:38: COMMA usinglist
            	    {
            	    COMMA193=(Token)match(input,COMMA,FOLLOW_COMMA_in_usinglist2140); if (state.failed) return retval; 
            	    if ( state.backtracking==0 ) stream_COMMA.add(COMMA193);


            	    pushFollow(FOLLOW_usinglist_in_usinglist2142);
            	    usinglist194=usinglist();

            	    state._fsp--;
            	    if (state.failed) return retval;
            	    if ( state.backtracking==0 ) stream_usinglist.add(usinglist194.getTree());

            	    }
            	    break;

            	default :
            	    break loop44;
                }
            } while (true);


            // AST REWRITE
            // elements: usingterm
            // token labels: 
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 318:9: -> ( usingterm )+
            {
                if ( !(stream_usingterm.hasNext()) ) {
                    throw new RewriteEarlyExitException();
                }
                while ( stream_usingterm.hasNext() ) {
                    adaptor.addChild(root_0, stream_usingterm.nextTree());

                }
                stream_usingterm.reset();

            }


            retval.tree = root_0;
            }

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "usinglist"


    public static class usingterm_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "usingterm"
    // CongerCQL.g:321:1: usingterm : usingexpr ;
    public final CongerCQLParser.usingterm_return usingterm() throws RecognitionException {
        CongerCQLParser.usingterm_return retval = new CongerCQLParser.usingterm_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        CongerCQLParser.usingexpr_return usingexpr195 =null;



        try {
            // CongerCQL.g:322:2: ( usingexpr )
            // CongerCQL.g:322:4: usingexpr
            {
            root_0 = (CommonTree)adaptor.nil();


            pushFollow(FOLLOW_usingexpr_in_usingterm2168);
            usingexpr195=usingexpr();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) adaptor.addChild(root_0, usingexpr195.getTree());

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "usingterm"


    public static class usingexpr_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "usingexpr"
    // CongerCQL.g:325:1: usingexpr : ( attr | const_int );
    public final CongerCQLParser.usingexpr_return usingexpr() throws RecognitionException {
        CongerCQLParser.usingexpr_return retval = new CongerCQLParser.usingexpr_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        CongerCQLParser.attr_return attr196 =null;

        CongerCQLParser.const_int_return const_int197 =null;



        try {
            // CongerCQL.g:326:2: ( attr | const_int )
            int alt45=2;
            int LA45_0 = input.LA(1);

            if ( (LA45_0==Identifier||LA45_0==KW_ELEMENT_TIME) ) {
                alt45=1;
            }
            else if ( (LA45_0==Integer) ) {
                alt45=2;
            }
            else {
                if (state.backtracking>0) {state.failed=true; return retval;}
                NoViableAltException nvae =
                    new NoViableAltException("", 45, 0, input);

                throw nvae;

            }
            switch (alt45) {
                case 1 :
                    // CongerCQL.g:326:4: attr
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    pushFollow(FOLLOW_attr_in_usingexpr2179);
                    attr196=attr();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, attr196.getTree());

                    }
                    break;
                case 2 :
                    // CongerCQL.g:327:4: const_int
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    pushFollow(FOLLOW_const_int_in_usingexpr2184);
                    const_int197=const_int();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, const_int197.getTree());

                    }
                    break;

            }
            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "usingexpr"


    public static class attr_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "attr"
    // CongerCQL.g:330:1: attr : ( Identifier ( DOT Identifier | DOT pseudo_column )? | pseudo_column );
    public final CongerCQLParser.attr_return attr() throws RecognitionException {
        CongerCQLParser.attr_return retval = new CongerCQLParser.attr_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token Identifier198=null;
        Token DOT199=null;
        Token Identifier200=null;
        Token DOT201=null;
        CongerCQLParser.pseudo_column_return pseudo_column202 =null;

        CongerCQLParser.pseudo_column_return pseudo_column203 =null;


        CommonTree Identifier198_tree=null;
        CommonTree DOT199_tree=null;
        CommonTree Identifier200_tree=null;
        CommonTree DOT201_tree=null;

        try {
            // CongerCQL.g:331:2: ( Identifier ( DOT Identifier | DOT pseudo_column )? | pseudo_column )
            int alt47=2;
            int LA47_0 = input.LA(1);

            if ( (LA47_0==Identifier) ) {
                alt47=1;
            }
            else if ( (LA47_0==KW_ELEMENT_TIME) ) {
                alt47=2;
            }
            else {
                if (state.backtracking>0) {state.failed=true; return retval;}
                NoViableAltException nvae =
                    new NoViableAltException("", 47, 0, input);

                throw nvae;

            }
            switch (alt47) {
                case 1 :
                    // CongerCQL.g:331:4: Identifier ( DOT Identifier | DOT pseudo_column )?
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    Identifier198=(Token)match(input,Identifier,FOLLOW_Identifier_in_attr2195); if (state.failed) return retval;
                    if ( state.backtracking==0 ) {
                    Identifier198_tree = 
                    (CommonTree)adaptor.create(Identifier198)
                    ;
                    adaptor.addChild(root_0, Identifier198_tree);
                    }

                    // CongerCQL.g:331:15: ( DOT Identifier | DOT pseudo_column )?
                    int alt46=3;
                    int LA46_0 = input.LA(1);

                    if ( (LA46_0==DOT) ) {
                        int LA46_1 = input.LA(2);

                        if ( (LA46_1==Identifier) ) {
                            alt46=1;
                        }
                        else if ( (LA46_1==KW_ELEMENT_TIME) ) {
                            alt46=2;
                        }
                    }
                    switch (alt46) {
                        case 1 :
                            // CongerCQL.g:331:17: DOT Identifier
                            {
                            DOT199=(Token)match(input,DOT,FOLLOW_DOT_in_attr2199); if (state.failed) return retval;
                            if ( state.backtracking==0 ) {
                            DOT199_tree = 
                            (CommonTree)adaptor.create(DOT199)
                            ;
                            adaptor.addChild(root_0, DOT199_tree);
                            }

                            Identifier200=(Token)match(input,Identifier,FOLLOW_Identifier_in_attr2201); if (state.failed) return retval;
                            if ( state.backtracking==0 ) {
                            Identifier200_tree = 
                            (CommonTree)adaptor.create(Identifier200)
                            ;
                            adaptor.addChild(root_0, Identifier200_tree);
                            }

                            }
                            break;
                        case 2 :
                            // CongerCQL.g:331:34: DOT pseudo_column
                            {
                            DOT201=(Token)match(input,DOT,FOLLOW_DOT_in_attr2205); if (state.failed) return retval;
                            if ( state.backtracking==0 ) {
                            DOT201_tree = 
                            (CommonTree)adaptor.create(DOT201)
                            ;
                            adaptor.addChild(root_0, DOT201_tree);
                            }

                            pushFollow(FOLLOW_pseudo_column_in_attr2207);
                            pseudo_column202=pseudo_column();

                            state._fsp--;
                            if (state.failed) return retval;
                            if ( state.backtracking==0 ) adaptor.addChild(root_0, pseudo_column202.getTree());

                            }
                            break;

                    }


                    }
                    break;
                case 2 :
                    // CongerCQL.g:332:4: pseudo_column
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    pushFollow(FOLLOW_pseudo_column_in_attr2214);
                    pseudo_column203=pseudo_column();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, pseudo_column203.getTree());

                    }
                    break;

            }
            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "attr"


    public static class pseudo_column_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "pseudo_column"
    // CongerCQL.g:335:1: pseudo_column : element_time ;
    public final CongerCQLParser.pseudo_column_return pseudo_column() throws RecognitionException {
        CongerCQLParser.pseudo_column_return retval = new CongerCQLParser.pseudo_column_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        CongerCQLParser.element_time_return element_time204 =null;



        try {
            // CongerCQL.g:336:2: ( element_time )
            // CongerCQL.g:336:4: element_time
            {
            root_0 = (CommonTree)adaptor.nil();


            pushFollow(FOLLOW_element_time_in_pseudo_column2225);
            element_time204=element_time();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) adaptor.addChild(root_0, element_time204.getTree());

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "pseudo_column"


    public static class element_time_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "element_time"
    // CongerCQL.g:339:1: element_time : KW_ELEMENT_TIME ;
    public final CongerCQLParser.element_time_return element_time() throws RecognitionException {
        CongerCQLParser.element_time_return retval = new CongerCQLParser.element_time_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token KW_ELEMENT_TIME205=null;

        CommonTree KW_ELEMENT_TIME205_tree=null;

        try {
            // CongerCQL.g:340:2: ( KW_ELEMENT_TIME )
            // CongerCQL.g:340:4: KW_ELEMENT_TIME
            {
            root_0 = (CommonTree)adaptor.nil();


            KW_ELEMENT_TIME205=(Token)match(input,KW_ELEMENT_TIME,FOLLOW_KW_ELEMENT_TIME_in_element_time2236); if (state.failed) return retval;
            if ( state.backtracking==0 ) {
            KW_ELEMENT_TIME205_tree = 
            (CommonTree)adaptor.create(KW_ELEMENT_TIME205)
            ;
            adaptor.addChild(root_0, KW_ELEMENT_TIME205_tree);
            }

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "element_time"


    public static class const_int_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "const_int"
    // CongerCQL.g:343:1: const_int : Integer ;
    public final CongerCQLParser.const_int_return const_int() throws RecognitionException {
        CongerCQLParser.const_int_return retval = new CongerCQLParser.const_int_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token Integer206=null;

        CommonTree Integer206_tree=null;

        try {
            // CongerCQL.g:344:2: ( Integer )
            // CongerCQL.g:344:4: Integer
            {
            root_0 = (CommonTree)adaptor.nil();


            Integer206=(Token)match(input,Integer,FOLLOW_Integer_in_const_int2247); if (state.failed) return retval;
            if ( state.backtracking==0 ) {
            Integer206_tree = 
            (CommonTree)adaptor.create(Integer206)
            ;
            adaptor.addChild(root_0, Integer206_tree);
            }

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "const_int"


    public static class rstream_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "rstream"
    // CongerCQL.g:347:1: rstream : KW_RSTREAM ;
    public final CongerCQLParser.rstream_return rstream() throws RecognitionException {
        CongerCQLParser.rstream_return retval = new CongerCQLParser.rstream_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token KW_RSTREAM207=null;

        CommonTree KW_RSTREAM207_tree=null;

        try {
            // CongerCQL.g:348:2: ( KW_RSTREAM )
            // CongerCQL.g:348:4: KW_RSTREAM
            {
            root_0 = (CommonTree)adaptor.nil();


            KW_RSTREAM207=(Token)match(input,KW_RSTREAM,FOLLOW_KW_RSTREAM_in_rstream2258); if (state.failed) return retval;
            if ( state.backtracking==0 ) {
            KW_RSTREAM207_tree = 
            (CommonTree)adaptor.create(KW_RSTREAM207)
            ;
            adaptor.addChild(root_0, KW_RSTREAM207_tree);
            }

            }

            retval.stop = input.LT(-1);


            if ( state.backtracking==0 ) {

            retval.tree = (CommonTree)adaptor.rulePostProcessing(root_0);
            adaptor.setTokenBoundaries(retval.tree, retval.start, retval.stop);
            }
        }
        catch (RecognitionException re) {
            reportError(re);
            recover(input,re);
    	retval.tree = (CommonTree)adaptor.errorNode(input, retval.start, input.LT(-1), re);

        }

        finally {
        	// do for sure before leaving
        }
        return retval;
    }
    // $ANTLR end "rstream"

    // $ANTLR start synpred2_CongerCQL
    public final void synpred2_CongerCQL_fragment() throws RecognitionException {
        // CongerCQL.g:190:4: ( unary_condition )
        // CongerCQL.g:190:4: unary_condition
        {
        pushFollow(FOLLOW_unary_condition_in_synpred2_CongerCQL1253);
        unary_condition();

        state._fsp--;
        if (state.failed) return ;

        }

    }
    // $ANTLR end synpred2_CongerCQL

    // Delegated rules

    public final boolean synpred2_CongerCQL() {
        state.backtracking++;
        int start = input.mark();
        try {
            synpred2_CongerCQL_fragment(); // can never throw exception
        } catch (RecognitionException re) {
            System.err.println("impossible: "+re);
        }
        boolean success = !state.failed;
        input.rewind(start);
        state.backtracking--;
        state.failed=false;
        return success;
    }


 

    public static final BitSet FOLLOW_query_in_statement257 = new BitSet(new long[]{0x0000000000000000L});
    public static final BitSet FOLLOW_EOF_in_statement259 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_sfw_block_in_query270 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_idstream_clause_in_query275 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000008L});
    public static final BitSet FOLLOW_LPAREN_in_query277 = new BitSet(new long[]{0x0000000000000000L,0x0001000000000000L});
    public static final BitSet FOLLOW_sfw_block_in_query279 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000002000L});
    public static final BitSet FOLLOW_RPAREN_in_query281 = new BitSet(new long[]{0x0008000000000000L});
    public static final BitSet FOLLOW_using_clause_in_query283 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_rstream_in_query306 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000008L});
    public static final BitSet FOLLOW_LPAREN_in_query308 = new BitSet(new long[]{0x0000000000000000L,0x0001000000000000L});
    public static final BitSet FOLLOW_sfw_block_in_query310 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000002000L});
    public static final BitSet FOLLOW_RPAREN_in_query312 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_binary_in_query333 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_idstream_clause_in_query338 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000008L});
    public static final BitSet FOLLOW_LPAREN_in_query340 = new BitSet(new long[]{0x0000000000800000L});
    public static final BitSet FOLLOW_binary_in_query342 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000002000L});
    public static final BitSet FOLLOW_RPAREN_in_query344 = new BitSet(new long[]{0x0008000000000000L});
    public static final BitSet FOLLOW_using_clause_in_query346 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_rstream_in_query369 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000008L});
    public static final BitSet FOLLOW_LPAREN_in_query371 = new BitSet(new long[]{0x0000000000800000L});
    public static final BitSet FOLLOW_binary_in_query373 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000002000L});
    public static final BitSet FOLLOW_RPAREN_in_query375 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_Identifier_in_binary404 = new BitSet(new long[]{0x0100000000000000L,0x0080000010800050L});
    public static final BitSet FOLLOW_binary_operator_in_binary406 = new BitSet(new long[]{0x0000000000800000L});
    public static final BitSet FOLLOW_Identifier_in_binary410 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_EXCEPT_in_binary_operator440 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_MINUS_in_binary_operator445 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_INTERSECT_in_binary_operator450 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_UNION_in_binary_operator455 = new BitSet(new long[]{0x0000000004000002L});
    public static final BitSet FOLLOW_KW_ALL_in_binary_operator457 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_NOT_in_binary_operator463 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000010L});
    public static final BitSet FOLLOW_KW_IN_in_binary_operator466 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_select_clause_in_sfw_block477 = new BitSet(new long[]{0x2000000000000000L});
    public static final BitSet FOLLOW_from_clause_in_sfw_block479 = new BitSet(new long[]{0x8000000000000002L,0x2000000800000001L});
    public static final BitSet FOLLOW_opt_group_by_clause_in_sfw_block484 = new BitSet(new long[]{0x0000000000000002L,0x0000000000000001L});
    public static final BitSet FOLLOW_opt_having_clause_in_sfw_block487 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_order_by_clause_in_sfw_block496 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_opt_where_clause_in_sfw_block504 = new BitSet(new long[]{0x8000000000000002L,0x0000000000000001L});
    public static final BitSet FOLLOW_opt_group_by_clause_in_sfw_block506 = new BitSet(new long[]{0x0000000000000002L,0x0000000000000001L});
    public static final BitSet FOLLOW_opt_having_clause_in_sfw_block509 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_SELECT_in_select_clause568 = new BitSet(new long[]{0x10B0010041800000L,0x0008000040420080L,0x0000000000060008L});
    public static final BitSet FOLLOW_KW_DISTINCT_in_select_clause570 = new BitSet(new long[]{0x10A0010041800000L,0x0008000040420080L,0x0000000000060008L});
    public static final BitSet FOLLOW_STAR_in_select_clause574 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_non_mt_projterm_list_in_select_clause578 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_projterm_in_non_mt_projterm_list613 = new BitSet(new long[]{0x0000000000000102L});
    public static final BitSet FOLLOW_COMMA_in_non_mt_projterm_list624 = new BitSet(new long[]{0x10A0010041800000L,0x0008000040420080L,0x0000000000040008L});
    public static final BitSet FOLLOW_projterm_in_non_mt_projterm_list626 = new BitSet(new long[]{0x0000000000000102L});
    public static final BitSet FOLLOW_arith_expr_in_projterm656 = new BitSet(new long[]{0x0000000010000002L});
    public static final BitSet FOLLOW_KW_AS_in_projterm659 = new BitSet(new long[]{0x0000000000800000L});
    public static final BitSet FOLLOW_Identifier_in_projterm663 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_arith_expr_main_in_arith_expr700 = new BitSet(new long[]{0x0000000000004402L,0x0000000000000000L,0x0000000000020440L});
    public static final BitSet FOLLOW_arith_expr_operator_in_arith_expr703 = new BitSet(new long[]{0x10A0010041800000L,0x0008000040420080L,0x0000000000040008L});
    public static final BitSet FOLLOW_arith_expr_main_in_arith_expr705 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_attr_in_arith_expr_main782 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_const_value_in_arith_expr_main787 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_aggr_expr_in_arith_expr_main792 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_aggr_distinct_expr_in_arith_expr_main797 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_func_expr_in_arith_expr_main802 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_LPAREN_in_arith_expr_main814 = new BitSet(new long[]{0x10A0010041800000L,0x0008000040420080L,0x0000000000040008L});
    public static final BitSet FOLLOW_arith_expr_in_arith_expr_main816 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000002000L});
    public static final BitSet FOLLOW_RPAREN_in_arith_expr_main818 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_ORDER_in_order_by_clause830 = new BitSet(new long[]{0x0000002000000000L});
    public static final BitSet FOLLOW_KW_BY_in_order_by_clause832 = new BitSet(new long[]{0x0080000001800000L});
    public static final BitSet FOLLOW_order_by_list_in_order_by_clause834 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_orderterm_in_order_by_list861 = new BitSet(new long[]{0x0000000000000102L});
    public static final BitSet FOLLOW_COMMA_in_order_by_list872 = new BitSet(new long[]{0x0080000001800000L});
    public static final BitSet FOLLOW_order_by_list_in_order_by_list874 = new BitSet(new long[]{0x0000000000000102L});
    public static final BitSet FOLLOW_order_expr_in_orderterm900 = new BitSet(new long[]{0x0002000020000002L,0x0000000080000000L});
    public static final BitSet FOLLOW_asc_desc_in_orderterm902 = new BitSet(new long[]{0x0000000000000002L,0x0000000080000000L});
    public static final BitSet FOLLOW_null_spec_in_orderterm905 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_NULLS_in_null_spec955 = new BitSet(new long[]{0x0800000000000000L,0x0000000000001000L});
    public static final BitSet FOLLOW_set_in_null_spec957 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_attr_in_order_expr974 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_const_int_in_order_expr979 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_WHERE_in_opt_where_clause990 = new BitSet(new long[]{0x10A0010041800000L,0x0008000050420080L,0x0000000000040008L});
    public static final BitSet FOLLOW_non_mt_cond_list_in_opt_where_clause992 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_HAVING_in_opt_having_clause1019 = new BitSet(new long[]{0x10A0010041800000L,0x0008000050420080L,0x0000000000040008L});
    public static final BitSet FOLLOW_non_mt_cond_list_in_opt_having_clause1021 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_GROUP_in_opt_group_by_clause1048 = new BitSet(new long[]{0x0000002000000000L});
    public static final BitSet FOLLOW_KW_BY_in_opt_group_by_clause1050 = new BitSet(new long[]{0x0080000000800000L});
    public static final BitSet FOLLOW_non_mt_attr_list_in_opt_group_by_clause1052 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_FROM_in_from_clause1079 = new BitSet(new long[]{0x0000000000800000L});
    public static final BitSet FOLLOW_non_mt_relation_list_in_from_clause1081 = new BitSet(new long[]{0x0000000000000002L,0x0000042000002800L});
    public static final BitSet FOLLOW_KW_LEFT_in_from_clause1088 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L});
    public static final BitSet FOLLOW_KW_RIGHT_in_from_clause1092 = new BitSet(new long[]{0x0000000000000000L,0x0000002000000000L});
    public static final BitSet FOLLOW_KW_OUTER_in_from_clause1096 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000800L});
    public static final BitSet FOLLOW_KW_JOIN_in_from_clause1100 = new BitSet(new long[]{0x0000000000800000L});
    public static final BitSet FOLLOW_relation_variable_in_from_clause1102 = new BitSet(new long[]{0x0000000000000000L,0x0000000200000000L});
    public static final BitSet FOLLOW_KW_ON_in_from_clause1104 = new BitSet(new long[]{0x10A0010041800000L,0x0008000050420080L,0x0000000000040008L});
    public static final BitSet FOLLOW_non_mt_cond_list_in_from_clause1106 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_non_mt_cond_list_main_in_non_mt_cond_list1171 = new BitSet(new long[]{0x0000000008000002L,0x8000000400000000L});
    public static final BitSet FOLLOW_cond_list_operator_in_non_mt_cond_list1174 = new BitSet(new long[]{0x10A0010041800000L,0x0008000050420080L,0x0000000000040008L});
    public static final BitSet FOLLOW_non_mt_cond_list_main_in_non_mt_cond_list1176 = new BitSet(new long[]{0x0000000008000002L,0x8000000400000000L});
    public static final BitSet FOLLOW_KW_NOT_in_non_mt_cond_list_main1246 = new BitSet(new long[]{0x10A0010041800000L,0x0008000050420080L,0x0000000000040008L});
    public static final BitSet FOLLOW_non_mt_cond_list_main_in_non_mt_cond_list_main1248 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_unary_condition_in_non_mt_cond_list_main1253 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_LPAREN_in_non_mt_cond_list_main1258 = new BitSet(new long[]{0x10A0010041800000L,0x0008000040420080L,0x0000000000040008L});
    public static final BitSet FOLLOW_unary_condition_in_non_mt_cond_list_main1260 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000002000L});
    public static final BitSet FOLLOW_RPAREN_in_non_mt_cond_list_main1262 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_arith_expr_in_unary_condition1273 = new BitSet(new long[]{0x0000000100340000L,0x0000000000000000L,0x0000000000000106L});
    public static final BitSet FOLLOW_between_condition_right_in_unary_condition1276 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_compare_condition_right_in_unary_condition1280 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_BETWEEN_in_between_condition_right1293 = new BitSet(new long[]{0x10A0010041800000L,0x0008000040420080L,0x0000000000040008L});
    public static final BitSet FOLLOW_arith_expr_in_between_condition_right1295 = new BitSet(new long[]{0x0000000008000000L});
    public static final BitSet FOLLOW_KW_AND_in_between_condition_right1297 = new BitSet(new long[]{0x10A0010041800000L,0x0008000040420080L,0x0000000000040008L});
    public static final BitSet FOLLOW_arith_expr_in_between_condition_right1299 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_unary_condition_operator_in_compare_condition_right1310 = new BitSet(new long[]{0x10A0010041800000L,0x0008000040420080L,0x0000000000040008L});
    public static final BitSet FOLLOW_arith_expr_in_compare_condition_right1312 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_StringLiteral_in_const_string1361 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_arith_expr_in_non_mt_arg_list1372 = new BitSet(new long[]{0x0000000000000102L});
    public static final BitSet FOLLOW_COMMA_in_non_mt_arg_list1383 = new BitSet(new long[]{0x10A0010041800000L,0x0008000040420080L,0x0000000000040008L});
    public static final BitSet FOLLOW_non_mt_arg_list_in_non_mt_arg_list1385 = new BitSet(new long[]{0x0000000000000102L});
    public static final BitSet FOLLOW_relation_variable_in_non_mt_relation_list1410 = new BitSet(new long[]{0x0000000000000102L});
    public static final BitSet FOLLOW_COMMA_in_non_mt_relation_list1421 = new BitSet(new long[]{0x0000000000800000L});
    public static final BitSet FOLLOW_non_mt_relation_list_in_non_mt_relation_list1423 = new BitSet(new long[]{0x0000000000000102L});
    public static final BitSet FOLLOW_Identifier_in_relation_variable1455 = new BitSet(new long[]{0x0000000010000002L,0x0000000000000000L,0x0000000000000010L});
    public static final BitSet FOLLOW_LSQUARE_in_relation_variable1458 = new BitSet(new long[]{0x0000000000000000L,0x0000124020000000L});
    public static final BitSet FOLLOW_window_type_in_relation_variable1460 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000004000L});
    public static final BitSet FOLLOW_RSQUARE_in_relation_variable1462 = new BitSet(new long[]{0x0000000010000002L});
    public static final BitSet FOLLOW_KW_AS_in_relation_variable1468 = new BitSet(new long[]{0x0000000000800000L});
    public static final BitSet FOLLOW_Identifier_in_relation_variable1472 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_RANGE_in_window_type1513 = new BitSet(new long[]{0x1020000001000000L,0x0040000040000080L,0x0000000000040000L});
    public static final BitSet FOLLOW_time_spec_in_window_type1519 = new BitSet(new long[]{0x0000000000000002L,0x0004000000000000L});
    public static final BitSet FOLLOW_KW_SLIDE_in_window_type1523 = new BitSet(new long[]{0x0000000001000000L});
    public static final BitSet FOLLOW_time_spec_in_window_type1525 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_const_value_in_window_type1533 = new BitSet(new long[]{0x0000000000000000L,0x0000000200000000L});
    public static final BitSet FOLLOW_KW_ON_in_window_type1535 = new BitSet(new long[]{0x0000000000800000L});
    public static final BitSet FOLLOW_Identifier_in_window_type1537 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_UNBOUNDED_in_window_type1544 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_NOW_in_window_type1553 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_ROWS_in_window_type1558 = new BitSet(new long[]{0x0000000001000000L});
    public static final BitSet FOLLOW_Integer_in_window_type1560 = new BitSet(new long[]{0x0000000000000002L,0x0004000000000000L});
    public static final BitSet FOLLOW_KW_SLIDE_in_window_type1564 = new BitSet(new long[]{0x0000000001000000L});
    public static final BitSet FOLLOW_Integer_in_window_type1566 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_PARTITION_in_window_type1573 = new BitSet(new long[]{0x0000002000000000L});
    public static final BitSet FOLLOW_KW_BY_in_window_type1575 = new BitSet(new long[]{0x0080000000800000L});
    public static final BitSet FOLLOW_non_mt_attr_list_in_window_type1577 = new BitSet(new long[]{0x0000000000000000L,0x0000100000000000L});
    public static final BitSet FOLLOW_KW_ROWS_in_window_type1579 = new BitSet(new long[]{0x0000000001000000L});
    public static final BitSet FOLLOW_Integer_in_window_type1586 = new BitSet(new long[]{0x0000000000000002L,0x0000020000000000L});
    public static final BitSet FOLLOW_KW_RANGE_in_window_type1589 = new BitSet(new long[]{0x0000000001000000L});
    public static final BitSet FOLLOW_time_spec_in_window_type1591 = new BitSet(new long[]{0x0000000000000002L,0x0004000000000000L});
    public static final BitSet FOLLOW_KW_SLIDE_in_window_type1594 = new BitSet(new long[]{0x0000000001000000L});
    public static final BitSet FOLLOW_time_spec_in_window_type1596 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_attr_in_non_mt_attr_list1645 = new BitSet(new long[]{0x0000000000000102L});
    public static final BitSet FOLLOW_COMMA_in_non_mt_attr_list1656 = new BitSet(new long[]{0x0080000000800000L});
    public static final BitSet FOLLOW_non_mt_attr_list_in_non_mt_attr_list1658 = new BitSet(new long[]{0x0000000000000102L});
    public static final BitSet FOLLOW_interval_value_in_const_value1688 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_const_string_in_const_value1693 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_NULL_in_const_value1698 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_const_int_in_const_value1703 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_FLOAT_in_const_value1708 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_DOUBLE_in_const_value1713 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_INTERVAL_in_interval_value1724 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000040000L});
    public static final BitSet FOLLOW_const_string_in_interval_value1726 = new BitSet(new long[]{0x0000600000000000L});
    public static final BitSet FOLLOW_set_in_interval_value1728 = new BitSet(new long[]{0x0000000000000000L,0x0010000000000000L});
    public static final BitSet FOLLOW_KW_TO_in_interval_value1736 = new BitSet(new long[]{0x0000000000000000L,0x0000C00000000000L});
    public static final BitSet FOLLOW_set_in_interval_value1738 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_Integer_in_time_spec1753 = new BitSet(new long[]{0x0000600000000000L,0x0000C0000F3C0006L});
    public static final BitSet FOLLOW_time_unit_in_time_spec1755 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_aggr_distinct_operator_in_aggr_distinct_expr1842 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000008L});
    public static final BitSet FOLLOW_LPAREN_in_aggr_distinct_expr1844 = new BitSet(new long[]{0x0010000000000000L});
    public static final BitSet FOLLOW_KW_DISTINCT_in_aggr_distinct_expr1846 = new BitSet(new long[]{0x10A0010041800000L,0x0008000040420080L,0x0000000000040008L});
    public static final BitSet FOLLOW_arith_expr_in_aggr_distinct_expr1848 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000002000L});
    public static final BitSet FOLLOW_RPAREN_in_aggr_distinct_expr1850 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_COUNT_in_aggr_expr1904 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000008L});
    public static final BitSet FOLLOW_LPAREN_in_aggr_expr1906 = new BitSet(new long[]{0x10A0010041800000L,0x0008000040420080L,0x0000000000060008L});
    public static final BitSet FOLLOW_arith_expr_in_aggr_expr1910 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000002000L});
    public static final BitSet FOLLOW_STAR_in_aggr_expr1914 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000002000L});
    public static final BitSet FOLLOW_RPAREN_in_aggr_expr1917 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_SUM_in_aggr_expr1947 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000008L});
    public static final BitSet FOLLOW_KW_AVG_in_aggr_expr1951 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000008L});
    public static final BitSet FOLLOW_LPAREN_in_aggr_expr1954 = new BitSet(new long[]{0x10A0010041800000L,0x0008000040420080L,0x0000000000040008L});
    public static final BitSet FOLLOW_arith_expr_in_aggr_expr1956 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000002000L});
    public static final BitSet FOLLOW_KW_MAX_in_aggr_expr1964 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000008L});
    public static final BitSet FOLLOW_KW_MIN_in_aggr_expr1968 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000008L});
    public static final BitSet FOLLOW_LPAREN_in_aggr_expr1971 = new BitSet(new long[]{0x10A0010041800000L,0x0008000040420080L,0x0000000000040008L});
    public static final BitSet FOLLOW_arith_expr_in_aggr_expr1973 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000002000L});
    public static final BitSet FOLLOW_RPAREN_in_aggr_expr1978 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_func_name_in_func_expr2016 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000008L});
    public static final BitSet FOLLOW_LPAREN_in_func_expr2018 = new BitSet(new long[]{0x10B0010041800000L,0x0008000040420080L,0x0000000000040008L});
    public static final BitSet FOLLOW_non_mt_arg_list_in_func_expr2022 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000002000L});
    public static final BitSet FOLLOW_KW_DISTINCT_in_func_expr2026 = new BitSet(new long[]{0x10A0010041800000L,0x0008000040420080L,0x0000000000040008L});
    public static final BitSet FOLLOW_arith_expr_in_func_expr2028 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000002000L});
    public static final BitSet FOLLOW_RPAREN_in_func_expr2031 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_Identifier_in_func_name2067 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_DIFFERENCE_in_using_clause2094 = new BitSet(new long[]{0x0000000000000000L,0x0400000000000000L});
    public static final BitSet FOLLOW_KW_USING_in_using_clause2096 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000008L});
    public static final BitSet FOLLOW_LPAREN_in_using_clause2098 = new BitSet(new long[]{0x0080000001800000L});
    public static final BitSet FOLLOW_usinglist_in_using_clause2100 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000002000L});
    public static final BitSet FOLLOW_RPAREN_in_using_clause2102 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_usingterm_in_usinglist2129 = new BitSet(new long[]{0x0000000000000102L});
    public static final BitSet FOLLOW_COMMA_in_usinglist2140 = new BitSet(new long[]{0x0080000001800000L});
    public static final BitSet FOLLOW_usinglist_in_usinglist2142 = new BitSet(new long[]{0x0000000000000102L});
    public static final BitSet FOLLOW_usingexpr_in_usingterm2168 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_attr_in_usingexpr2179 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_const_int_in_usingexpr2184 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_Identifier_in_attr2195 = new BitSet(new long[]{0x0000000000010002L});
    public static final BitSet FOLLOW_DOT_in_attr2199 = new BitSet(new long[]{0x0000000000800000L});
    public static final BitSet FOLLOW_Identifier_in_attr2201 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_DOT_in_attr2205 = new BitSet(new long[]{0x0080000000000000L});
    public static final BitSet FOLLOW_pseudo_column_in_attr2207 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_pseudo_column_in_attr2214 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_element_time_in_pseudo_column2225 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_ELEMENT_TIME_in_element_time2236 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_Integer_in_const_int2247 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_RSTREAM_in_rstream2258 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_unary_condition_in_synpred2_CongerCQL1253 = new BitSet(new long[]{0x0000000000000002L});

}