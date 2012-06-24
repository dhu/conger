// $ANTLR 3.4 CongerCQL.g 2012-06-23 22:12:55

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
        "<invalid>", "<EOR>", "<DOWN>", "<UP>", "AMPERSAND", "BITWISEOR", "BITWISEXOR", "COLON", "COMMA", "COMMENT", "CONCATENATE", "CharSetLiteral", "CharSetName", "DIV", "DIVIDE", "DOLLAR", "DOT", "Digit", "EQUAL", "Exponent", "GREATERTHAN", "GREATERTHANOREQUALTO", "HexDigit", "Identifier", "Integer", "KW_AFTER", "KW_ALL", "KW_AND", "KW_AS", "KW_ASC", "KW_AVG", "KW_BEFORE", "KW_BETWEEN", "KW_BIGINT", "KW_BINARY", "KW_BOOLEAN", "KW_BOTH", "KW_BY", "KW_COLUMN", "KW_COMMENT", "KW_COUNT", "KW_CREATE", "KW_CROSS", "KW_DATE", "KW_DATETIME", "KW_DAY", "KW_DAYS", "KW_DECODE", "KW_DEFAULT", "KW_DESC", "KW_DESCRIBE", "KW_DIFFERENCE", "KW_DISTINCT", "KW_DOUBLE", "KW_DSTREAM", "KW_ELEMENT_TIME", "KW_EXCEPT", "KW_EXISTS", "KW_FALSE", "KW_FIRST", "KW_FLOAT", "KW_FROM", "KW_FULL", "KW_GROUP", "KW_HAVING", "KW_HOUR", "KW_HOURS", "KW_IF", "KW_IN", "KW_INT", "KW_INTERSECT", "KW_INTERVAL", "KW_INTO", "KW_IS", "KW_ISTREAM", "KW_JOIN", "KW_LAST", "KW_LEFT", "KW_LIKE", "KW_LIMIT", "KW_LONG", "KW_MAX", "KW_MICROSECOND", "KW_MICROSECONDS", "KW_MILLISECOND", "KW_MILLISECONDS", "KW_MIN", "KW_MINUS", "KW_MINUTE", "KW_MINUTES", "KW_NANOSECOND", "KW_NANOSECONDS", "KW_NOT", "KW_NOW", "KW_NULL", "KW_NULLS", "KW_OF", "KW_ON", "KW_OR", "KW_ORDER", "KW_OUT", "KW_OUTER", "KW_OVER", "KW_PARTITION", "KW_PASSING", "KW_PLUS", "KW_RANGE", "KW_RIGHT", "KW_ROW", "KW_ROWS", "KW_RSTREAM", "KW_SECOND", "KW_SECONDS", "KW_SELECT", "KW_SET", "KW_SLIDE", "KW_SUM", "KW_TIMEOUT", "KW_TO", "KW_TRUE", "KW_UNBOUNDED", "KW_UNION", "KW_UPDATE", "KW_USE", "KW_USING", "KW_VALUE", "KW_VIEW", "KW_WHERE", "KW_WITH", "KW_XOR", "LCURLY", "LESSTHAN", "LESSTHANOREQUALTO", "LPAREN", "LSQUARE", "Letter", "MINUS", "MOD", "NOTEQUAL", "Number", "PLUS", "QUESTION", "RCURLY", "RPAREN", "RSQUARE", "RegexComponent", "SEMICOLON", "STAR", "StringLiteral", "TILDE", "TOK_AGGR", "TOK_AGGR_EXPR", "TOK_ARG_LIST", "TOK_ARITH_EXPR", "TOK_ATTR_LIST", "TOK_BETWEEN", "TOK_BINARY", "TOK_COMPARE", "TOK_COND", "TOK_COND_LIST", "TOK_FROM", "TOK_FUNC", "TOK_GROUP_BY", "TOK_HAVING", "TOK_JOIN", "TOK_PROJTERM", "TOK_PROJTERM_LIST", "TOK_QUERY", "TOK_RELATION_LIST", "TOK_RELATION_VARIABLE", "TOK_SELECT", "TOK_SFW", "TOK_UNARY_COND", "TOK_USING", "TOK_WHERE", "TOK_WINDOW", "WS"
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
    // CongerCQL.g:44:1: statement : query EOF ;
    public final CongerCQLParser.statement_return statement() throws RecognitionException {
        CongerCQLParser.statement_return retval = new CongerCQLParser.statement_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token EOF2=null;
        CongerCQLParser.query_return query1 =null;


        CommonTree EOF2_tree=null;

        try {
            // CongerCQL.g:45:2: ( query EOF )
            // CongerCQL.g:45:4: query EOF
            {
            root_0 = (CommonTree)adaptor.nil();


            pushFollow(FOLLOW_query_in_statement218);
            query1=query();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) adaptor.addChild(root_0, query1.getTree());

            EOF2=(Token)match(input,EOF,FOLLOW_EOF_in_statement220); if (state.failed) return retval;
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
    // CongerCQL.g:48:1: query : ( sfw_block | binary );
    public final CongerCQLParser.query_return query() throws RecognitionException {
        CongerCQLParser.query_return retval = new CongerCQLParser.query_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        CongerCQLParser.sfw_block_return sfw_block3 =null;

        CongerCQLParser.binary_return binary4 =null;



        try {
            // CongerCQL.g:49:2: ( sfw_block | binary )
            int alt1=2;
            int LA1_0 = input.LA(1);

            if ( (LA1_0==KW_SELECT) ) {
                alt1=1;
            }
            else if ( (LA1_0==Identifier) ) {
                alt1=2;
            }
            else {
                if (state.backtracking>0) {state.failed=true; return retval;}
                NoViableAltException nvae =
                    new NoViableAltException("", 1, 0, input);

                throw nvae;

            }
            switch (alt1) {
                case 1 :
                    // CongerCQL.g:49:4: sfw_block
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    pushFollow(FOLLOW_sfw_block_in_query231);
                    sfw_block3=sfw_block();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, sfw_block3.getTree());

                    }
                    break;
                case 2 :
                    // CongerCQL.g:50:4: binary
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    pushFollow(FOLLOW_binary_in_query236);
                    binary4=binary();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, binary4.getTree());

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
    // CongerCQL.g:53:1: binary : view1= Identifier binary_operator view2= Identifier -> ^( TOK_BINARY $view1 $view2 binary_operator ) ;
    public final CongerCQLParser.binary_return binary() throws RecognitionException {
        CongerCQLParser.binary_return retval = new CongerCQLParser.binary_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token view1=null;
        Token view2=null;
        CongerCQLParser.binary_operator_return binary_operator5 =null;


        CommonTree view1_tree=null;
        CommonTree view2_tree=null;
        RewriteRuleTokenStream stream_Identifier=new RewriteRuleTokenStream(adaptor,"token Identifier");
        RewriteRuleSubtreeStream stream_binary_operator=new RewriteRuleSubtreeStream(adaptor,"rule binary_operator");
        try {
            // CongerCQL.g:54:2: (view1= Identifier binary_operator view2= Identifier -> ^( TOK_BINARY $view1 $view2 binary_operator ) )
            // CongerCQL.g:54:4: view1= Identifier binary_operator view2= Identifier
            {
            view1=(Token)match(input,Identifier,FOLLOW_Identifier_in_binary249); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_Identifier.add(view1);


            pushFollow(FOLLOW_binary_operator_in_binary251);
            binary_operator5=binary_operator();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_binary_operator.add(binary_operator5.getTree());

            view2=(Token)match(input,Identifier,FOLLOW_Identifier_in_binary255); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_Identifier.add(view2);


            // AST REWRITE
            // elements: view2, binary_operator, view1
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
            // 55:6: -> ^( TOK_BINARY $view1 $view2 binary_operator )
            {
                // CongerCQL.g:55:9: ^( TOK_BINARY $view1 $view2 binary_operator )
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
    // CongerCQL.g:58:1: binary_operator : KW_UNION ;
    public final CongerCQLParser.binary_operator_return binary_operator() throws RecognitionException {
        CongerCQLParser.binary_operator_return retval = new CongerCQLParser.binary_operator_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token KW_UNION6=null;

        CommonTree KW_UNION6_tree=null;

        try {
            // CongerCQL.g:59:2: ( KW_UNION )
            // CongerCQL.g:59:4: KW_UNION
            {
            root_0 = (CommonTree)adaptor.nil();


            KW_UNION6=(Token)match(input,KW_UNION,FOLLOW_KW_UNION_in_binary_operator285); if (state.failed) return retval;
            if ( state.backtracking==0 ) {
            KW_UNION6_tree = 
            (CommonTree)adaptor.create(KW_UNION6)
            ;
            adaptor.addChild(root_0, KW_UNION6_tree);
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
    // $ANTLR end "binary_operator"


    public static class sfw_block_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "sfw_block"
    // CongerCQL.g:62:1: sfw_block : select_clause from_clause ( ( opt_group_by_clause )? ( opt_having_clause )? | opt_where_clause ( opt_group_by_clause )? ( opt_having_clause )? ) -> ^( TOK_SFW select_clause from_clause ( opt_where_clause )? ( opt_group_by_clause )? ( opt_having_clause )? ) ;
    public final CongerCQLParser.sfw_block_return sfw_block() throws RecognitionException {
        CongerCQLParser.sfw_block_return retval = new CongerCQLParser.sfw_block_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        CongerCQLParser.select_clause_return select_clause7 =null;

        CongerCQLParser.from_clause_return from_clause8 =null;

        CongerCQLParser.opt_group_by_clause_return opt_group_by_clause9 =null;

        CongerCQLParser.opt_having_clause_return opt_having_clause10 =null;

        CongerCQLParser.opt_where_clause_return opt_where_clause11 =null;

        CongerCQLParser.opt_group_by_clause_return opt_group_by_clause12 =null;

        CongerCQLParser.opt_having_clause_return opt_having_clause13 =null;


        RewriteRuleSubtreeStream stream_select_clause=new RewriteRuleSubtreeStream(adaptor,"rule select_clause");
        RewriteRuleSubtreeStream stream_opt_group_by_clause=new RewriteRuleSubtreeStream(adaptor,"rule opt_group_by_clause");
        RewriteRuleSubtreeStream stream_from_clause=new RewriteRuleSubtreeStream(adaptor,"rule from_clause");
        RewriteRuleSubtreeStream stream_opt_having_clause=new RewriteRuleSubtreeStream(adaptor,"rule opt_having_clause");
        RewriteRuleSubtreeStream stream_opt_where_clause=new RewriteRuleSubtreeStream(adaptor,"rule opt_where_clause");
        try {
            // CongerCQL.g:63:2: ( select_clause from_clause ( ( opt_group_by_clause )? ( opt_having_clause )? | opt_where_clause ( opt_group_by_clause )? ( opt_having_clause )? ) -> ^( TOK_SFW select_clause from_clause ( opt_where_clause )? ( opt_group_by_clause )? ( opt_having_clause )? ) )
            // CongerCQL.g:63:4: select_clause from_clause ( ( opt_group_by_clause )? ( opt_having_clause )? | opt_where_clause ( opt_group_by_clause )? ( opt_having_clause )? )
            {
            pushFollow(FOLLOW_select_clause_in_sfw_block297);
            select_clause7=select_clause();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_select_clause.add(select_clause7.getTree());

            pushFollow(FOLLOW_from_clause_in_sfw_block299);
            from_clause8=from_clause();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_from_clause.add(from_clause8.getTree());

            // CongerCQL.g:64:2: ( ( opt_group_by_clause )? ( opt_having_clause )? | opt_where_clause ( opt_group_by_clause )? ( opt_having_clause )? )
            int alt6=2;
            int LA6_0 = input.LA(1);

            if ( (LA6_0==EOF||(LA6_0 >= KW_GROUP && LA6_0 <= KW_HAVING)) ) {
                alt6=1;
            }
            else if ( (LA6_0==KW_WHERE) ) {
                alt6=2;
            }
            else {
                if (state.backtracking>0) {state.failed=true; return retval;}
                NoViableAltException nvae =
                    new NoViableAltException("", 6, 0, input);

                throw nvae;

            }
            switch (alt6) {
                case 1 :
                    // CongerCQL.g:64:3: ( opt_group_by_clause )? ( opt_having_clause )?
                    {
                    // CongerCQL.g:64:3: ( opt_group_by_clause )?
                    int alt2=2;
                    int LA2_0 = input.LA(1);

                    if ( (LA2_0==KW_GROUP) ) {
                        alt2=1;
                    }
                    switch (alt2) {
                        case 1 :
                            // CongerCQL.g:64:3: opt_group_by_clause
                            {
                            pushFollow(FOLLOW_opt_group_by_clause_in_sfw_block304);
                            opt_group_by_clause9=opt_group_by_clause();

                            state._fsp--;
                            if (state.failed) return retval;
                            if ( state.backtracking==0 ) stream_opt_group_by_clause.add(opt_group_by_clause9.getTree());

                            }
                            break;

                    }


                    // CongerCQL.g:64:24: ( opt_having_clause )?
                    int alt3=2;
                    int LA3_0 = input.LA(1);

                    if ( (LA3_0==KW_HAVING) ) {
                        alt3=1;
                    }
                    switch (alt3) {
                        case 1 :
                            // CongerCQL.g:64:24: opt_having_clause
                            {
                            pushFollow(FOLLOW_opt_having_clause_in_sfw_block307);
                            opt_having_clause10=opt_having_clause();

                            state._fsp--;
                            if (state.failed) return retval;
                            if ( state.backtracking==0 ) stream_opt_having_clause.add(opt_having_clause10.getTree());

                            }
                            break;

                    }


                    }
                    break;
                case 2 :
                    // CongerCQL.g:65:7: opt_where_clause ( opt_group_by_clause )? ( opt_having_clause )?
                    {
                    pushFollow(FOLLOW_opt_where_clause_in_sfw_block316);
                    opt_where_clause11=opt_where_clause();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_opt_where_clause.add(opt_where_clause11.getTree());

                    // CongerCQL.g:65:24: ( opt_group_by_clause )?
                    int alt4=2;
                    int LA4_0 = input.LA(1);

                    if ( (LA4_0==KW_GROUP) ) {
                        alt4=1;
                    }
                    switch (alt4) {
                        case 1 :
                            // CongerCQL.g:65:24: opt_group_by_clause
                            {
                            pushFollow(FOLLOW_opt_group_by_clause_in_sfw_block318);
                            opt_group_by_clause12=opt_group_by_clause();

                            state._fsp--;
                            if (state.failed) return retval;
                            if ( state.backtracking==0 ) stream_opt_group_by_clause.add(opt_group_by_clause12.getTree());

                            }
                            break;

                    }


                    // CongerCQL.g:65:45: ( opt_having_clause )?
                    int alt5=2;
                    int LA5_0 = input.LA(1);

                    if ( (LA5_0==KW_HAVING) ) {
                        alt5=1;
                    }
                    switch (alt5) {
                        case 1 :
                            // CongerCQL.g:65:45: opt_having_clause
                            {
                            pushFollow(FOLLOW_opt_having_clause_in_sfw_block321);
                            opt_having_clause13=opt_having_clause();

                            state._fsp--;
                            if (state.failed) return retval;
                            if ( state.backtracking==0 ) stream_opt_having_clause.add(opt_having_clause13.getTree());

                            }
                            break;

                    }


                    }
                    break;

            }


            // AST REWRITE
            // elements: opt_where_clause, from_clause, opt_having_clause, select_clause, opt_group_by_clause
            // token labels: 
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 67:9: -> ^( TOK_SFW select_clause from_clause ( opt_where_clause )? ( opt_group_by_clause )? ( opt_having_clause )? )
            {
                // CongerCQL.g:67:12: ^( TOK_SFW select_clause from_clause ( opt_where_clause )? ( opt_group_by_clause )? ( opt_having_clause )? )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_SFW, "TOK_SFW")
                , root_1);

                adaptor.addChild(root_1, stream_select_clause.nextTree());

                adaptor.addChild(root_1, stream_from_clause.nextTree());

                // CongerCQL.g:68:9: ( opt_where_clause )?
                if ( stream_opt_where_clause.hasNext() ) {
                    adaptor.addChild(root_1, stream_opt_where_clause.nextTree());

                }
                stream_opt_where_clause.reset();

                // CongerCQL.g:68:27: ( opt_group_by_clause )?
                if ( stream_opt_group_by_clause.hasNext() ) {
                    adaptor.addChild(root_1, stream_opt_group_by_clause.nextTree());

                }
                stream_opt_group_by_clause.reset();

                // CongerCQL.g:68:48: ( opt_having_clause )?
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
    // CongerCQL.g:71:1: select_clause : KW_SELECT ( STAR | non_mt_projterm_list ) -> ^( TOK_SELECT ( STAR )? ( non_mt_projterm_list )? ) ;
    public final CongerCQLParser.select_clause_return select_clause() throws RecognitionException {
        CongerCQLParser.select_clause_return retval = new CongerCQLParser.select_clause_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token KW_SELECT14=null;
        Token STAR15=null;
        CongerCQLParser.non_mt_projterm_list_return non_mt_projterm_list16 =null;


        CommonTree KW_SELECT14_tree=null;
        CommonTree STAR15_tree=null;
        RewriteRuleTokenStream stream_KW_SELECT=new RewriteRuleTokenStream(adaptor,"token KW_SELECT");
        RewriteRuleTokenStream stream_STAR=new RewriteRuleTokenStream(adaptor,"token STAR");
        RewriteRuleSubtreeStream stream_non_mt_projterm_list=new RewriteRuleSubtreeStream(adaptor,"rule non_mt_projterm_list");
        try {
            // CongerCQL.g:72:2: ( KW_SELECT ( STAR | non_mt_projterm_list ) -> ^( TOK_SELECT ( STAR )? ( non_mt_projterm_list )? ) )
            // CongerCQL.g:72:4: KW_SELECT ( STAR | non_mt_projterm_list )
            {
            KW_SELECT14=(Token)match(input,KW_SELECT,FOLLOW_KW_SELECT_in_select_clause374); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_KW_SELECT.add(KW_SELECT14);


            // CongerCQL.g:72:14: ( STAR | non_mt_projterm_list )
            int alt7=2;
            int LA7_0 = input.LA(1);

            if ( (LA7_0==STAR) ) {
                alt7=1;
            }
            else if ( ((LA7_0 >= Identifier && LA7_0 <= Integer)||LA7_0==KW_AVG||LA7_0==KW_COUNT||LA7_0==KW_ELEMENT_TIME||LA7_0==KW_MAX||LA7_0==KW_MIN||LA7_0==KW_NULL||LA7_0==KW_SUM||LA7_0==LPAREN||LA7_0==Number||LA7_0==StringLiteral) ) {
                alt7=2;
            }
            else {
                if (state.backtracking>0) {state.failed=true; return retval;}
                NoViableAltException nvae =
                    new NoViableAltException("", 7, 0, input);

                throw nvae;

            }
            switch (alt7) {
                case 1 :
                    // CongerCQL.g:72:15: STAR
                    {
                    STAR15=(Token)match(input,STAR,FOLLOW_STAR_in_select_clause377); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_STAR.add(STAR15);


                    }
                    break;
                case 2 :
                    // CongerCQL.g:72:22: non_mt_projterm_list
                    {
                    pushFollow(FOLLOW_non_mt_projterm_list_in_select_clause381);
                    non_mt_projterm_list16=non_mt_projterm_list();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_non_mt_projterm_list.add(non_mt_projterm_list16.getTree());

                    }
                    break;

            }


            // AST REWRITE
            // elements: non_mt_projterm_list, STAR
            // token labels: 
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 73:9: -> ^( TOK_SELECT ( STAR )? ( non_mt_projterm_list )? )
            {
                // CongerCQL.g:73:12: ^( TOK_SELECT ( STAR )? ( non_mt_projterm_list )? )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_SELECT, "TOK_SELECT")
                , root_1);

                // CongerCQL.g:73:25: ( STAR )?
                if ( stream_STAR.hasNext() ) {
                    adaptor.addChild(root_1, 
                    stream_STAR.nextNode()
                    );

                }
                stream_STAR.reset();

                // CongerCQL.g:73:31: ( non_mt_projterm_list )?
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
    // CongerCQL.g:76:1: non_mt_projterm_list : projterm ( options {greedy=true; } : COMMA projterm )* -> ^( TOK_PROJTERM_LIST ( projterm )+ ) ;
    public final CongerCQLParser.non_mt_projterm_list_return non_mt_projterm_list() throws RecognitionException {
        CongerCQLParser.non_mt_projterm_list_return retval = new CongerCQLParser.non_mt_projterm_list_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token COMMA18=null;
        CongerCQLParser.projterm_return projterm17 =null;

        CongerCQLParser.projterm_return projterm19 =null;


        CommonTree COMMA18_tree=null;
        RewriteRuleTokenStream stream_COMMA=new RewriteRuleTokenStream(adaptor,"token COMMA");
        RewriteRuleSubtreeStream stream_projterm=new RewriteRuleSubtreeStream(adaptor,"rule projterm");
        try {
            // CongerCQL.g:77:2: ( projterm ( options {greedy=true; } : COMMA projterm )* -> ^( TOK_PROJTERM_LIST ( projterm )+ ) )
            // CongerCQL.g:77:4: projterm ( options {greedy=true; } : COMMA projterm )*
            {
            pushFollow(FOLLOW_projterm_in_non_mt_projterm_list413);
            projterm17=projterm();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_projterm.add(projterm17.getTree());

            // CongerCQL.g:77:13: ( options {greedy=true; } : COMMA projterm )*
            loop8:
            do {
                int alt8=2;
                int LA8_0 = input.LA(1);

                if ( (LA8_0==COMMA) ) {
                    alt8=1;
                }


                switch (alt8) {
            	case 1 :
            	    // CongerCQL.g:77:37: COMMA projterm
            	    {
            	    COMMA18=(Token)match(input,COMMA,FOLLOW_COMMA_in_non_mt_projterm_list424); if (state.failed) return retval; 
            	    if ( state.backtracking==0 ) stream_COMMA.add(COMMA18);


            	    pushFollow(FOLLOW_projterm_in_non_mt_projterm_list426);
            	    projterm19=projterm();

            	    state._fsp--;
            	    if (state.failed) return retval;
            	    if ( state.backtracking==0 ) stream_projterm.add(projterm19.getTree());

            	    }
            	    break;

            	default :
            	    break loop8;
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
            // 78:9: -> ^( TOK_PROJTERM_LIST ( projterm )+ )
            {
                // CongerCQL.g:78:12: ^( TOK_PROJTERM_LIST ( projterm )+ )
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
    // CongerCQL.g:82:1: projterm : arith_expr ( KW_AS alias= Identifier )? -> ^( TOK_PROJTERM arith_expr ( KW_AS )? ( $alias)? ) ;
    public final CongerCQLParser.projterm_return projterm() throws RecognitionException {
        CongerCQLParser.projterm_return retval = new CongerCQLParser.projterm_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token alias=null;
        Token KW_AS21=null;
        CongerCQLParser.arith_expr_return arith_expr20 =null;


        CommonTree alias_tree=null;
        CommonTree KW_AS21_tree=null;
        RewriteRuleTokenStream stream_KW_AS=new RewriteRuleTokenStream(adaptor,"token KW_AS");
        RewriteRuleTokenStream stream_Identifier=new RewriteRuleTokenStream(adaptor,"token Identifier");
        RewriteRuleSubtreeStream stream_arith_expr=new RewriteRuleSubtreeStream(adaptor,"rule arith_expr");
        try {
            // CongerCQL.g:83:2: ( arith_expr ( KW_AS alias= Identifier )? -> ^( TOK_PROJTERM arith_expr ( KW_AS )? ( $alias)? ) )
            // CongerCQL.g:83:4: arith_expr ( KW_AS alias= Identifier )?
            {
            pushFollow(FOLLOW_arith_expr_in_projterm457);
            arith_expr20=arith_expr();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_arith_expr.add(arith_expr20.getTree());

            // CongerCQL.g:83:15: ( KW_AS alias= Identifier )?
            int alt9=2;
            int LA9_0 = input.LA(1);

            if ( (LA9_0==KW_AS) ) {
                alt9=1;
            }
            switch (alt9) {
                case 1 :
                    // CongerCQL.g:83:16: KW_AS alias= Identifier
                    {
                    KW_AS21=(Token)match(input,KW_AS,FOLLOW_KW_AS_in_projterm460); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_KW_AS.add(KW_AS21);


                    alias=(Token)match(input,Identifier,FOLLOW_Identifier_in_projterm464); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_Identifier.add(alias);


                    }
                    break;

            }


            // AST REWRITE
            // elements: alias, arith_expr, KW_AS
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
            // 84:9: -> ^( TOK_PROJTERM arith_expr ( KW_AS )? ( $alias)? )
            {
                // CongerCQL.g:84:12: ^( TOK_PROJTERM arith_expr ( KW_AS )? ( $alias)? )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_PROJTERM, "TOK_PROJTERM")
                , root_1);

                adaptor.addChild(root_1, stream_arith_expr.nextTree());

                // CongerCQL.g:84:38: ( KW_AS )?
                if ( stream_KW_AS.hasNext() ) {
                    adaptor.addChild(root_1, 
                    stream_KW_AS.nextNode()
                    );

                }
                stream_KW_AS.reset();

                // CongerCQL.g:84:46: ( $alias)?
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
    // CongerCQL.g:87:1: arith_expr : arith_expr_main ( arith_expr_operator arith_expr_main )? -> ^( TOK_ARITH_EXPR arith_expr_main ( arith_expr_operator )? ( arith_expr_main )? ) ;
    public final CongerCQLParser.arith_expr_return arith_expr() throws RecognitionException {
        CongerCQLParser.arith_expr_return retval = new CongerCQLParser.arith_expr_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        CongerCQLParser.arith_expr_main_return arith_expr_main22 =null;

        CongerCQLParser.arith_expr_operator_return arith_expr_operator23 =null;

        CongerCQLParser.arith_expr_main_return arith_expr_main24 =null;


        RewriteRuleSubtreeStream stream_arith_expr_operator=new RewriteRuleSubtreeStream(adaptor,"rule arith_expr_operator");
        RewriteRuleSubtreeStream stream_arith_expr_main=new RewriteRuleSubtreeStream(adaptor,"rule arith_expr_main");
        try {
            // CongerCQL.g:88:2: ( arith_expr_main ( arith_expr_operator arith_expr_main )? -> ^( TOK_ARITH_EXPR arith_expr_main ( arith_expr_operator )? ( arith_expr_main )? ) )
            // CongerCQL.g:88:4: arith_expr_main ( arith_expr_operator arith_expr_main )?
            {
            pushFollow(FOLLOW_arith_expr_main_in_arith_expr501);
            arith_expr_main22=arith_expr_main();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_arith_expr_main.add(arith_expr_main22.getTree());

            // CongerCQL.g:88:20: ( arith_expr_operator arith_expr_main )?
            int alt10=2;
            int LA10_0 = input.LA(1);

            if ( (LA10_0==CONCATENATE||LA10_0==DIVIDE||LA10_0==MINUS||LA10_0==PLUS||LA10_0==STAR) ) {
                alt10=1;
            }
            switch (alt10) {
                case 1 :
                    // CongerCQL.g:88:21: arith_expr_operator arith_expr_main
                    {
                    pushFollow(FOLLOW_arith_expr_operator_in_arith_expr504);
                    arith_expr_operator23=arith_expr_operator();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_arith_expr_operator.add(arith_expr_operator23.getTree());

                    pushFollow(FOLLOW_arith_expr_main_in_arith_expr506);
                    arith_expr_main24=arith_expr_main();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_arith_expr_main.add(arith_expr_main24.getTree());

                    }
                    break;

            }


            // AST REWRITE
            // elements: arith_expr_operator, arith_expr_main, arith_expr_main
            // token labels: 
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 90:9: -> ^( TOK_ARITH_EXPR arith_expr_main ( arith_expr_operator )? ( arith_expr_main )? )
            {
                // CongerCQL.g:90:12: ^( TOK_ARITH_EXPR arith_expr_main ( arith_expr_operator )? ( arith_expr_main )? )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_ARITH_EXPR, "TOK_ARITH_EXPR")
                , root_1);

                adaptor.addChild(root_1, stream_arith_expr_main.nextTree());

                // CongerCQL.g:90:45: ( arith_expr_operator )?
                if ( stream_arith_expr_operator.hasNext() ) {
                    adaptor.addChild(root_1, stream_arith_expr_operator.nextTree());

                }
                stream_arith_expr_operator.reset();

                // CongerCQL.g:90:66: ( arith_expr_main )?
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
    // CongerCQL.g:93:1: arith_expr_operator : ( PLUS | MINUS | STAR | DIVIDE | CONCATENATE );
    public final CongerCQLParser.arith_expr_operator_return arith_expr_operator() throws RecognitionException {
        CongerCQLParser.arith_expr_operator_return retval = new CongerCQLParser.arith_expr_operator_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token set25=null;

        CommonTree set25_tree=null;

        try {
            // CongerCQL.g:94:5: ( PLUS | MINUS | STAR | DIVIDE | CONCATENATE )
            // CongerCQL.g:
            {
            root_0 = (CommonTree)adaptor.nil();


            set25=(Token)input.LT(1);

            if ( input.LA(1)==CONCATENATE||input.LA(1)==DIVIDE||input.LA(1)==MINUS||input.LA(1)==PLUS||input.LA(1)==STAR ) {
                input.consume();
                if ( state.backtracking==0 ) adaptor.addChild(root_0, 
                (CommonTree)adaptor.create(set25)
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
    // CongerCQL.g:97:1: arith_expr_main : ( attr | const_value | aggr_expr | func_expr | LPAREN arith_expr RPAREN -> arith_expr );
    public final CongerCQLParser.arith_expr_main_return arith_expr_main() throws RecognitionException {
        CongerCQLParser.arith_expr_main_return retval = new CongerCQLParser.arith_expr_main_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token LPAREN30=null;
        Token RPAREN32=null;
        CongerCQLParser.attr_return attr26 =null;

        CongerCQLParser.const_value_return const_value27 =null;

        CongerCQLParser.aggr_expr_return aggr_expr28 =null;

        CongerCQLParser.func_expr_return func_expr29 =null;

        CongerCQLParser.arith_expr_return arith_expr31 =null;


        CommonTree LPAREN30_tree=null;
        CommonTree RPAREN32_tree=null;
        RewriteRuleTokenStream stream_RPAREN=new RewriteRuleTokenStream(adaptor,"token RPAREN");
        RewriteRuleTokenStream stream_LPAREN=new RewriteRuleTokenStream(adaptor,"token LPAREN");
        RewriteRuleSubtreeStream stream_arith_expr=new RewriteRuleSubtreeStream(adaptor,"rule arith_expr");
        try {
            // CongerCQL.g:98:2: ( attr | const_value | aggr_expr | func_expr | LPAREN arith_expr RPAREN -> arith_expr )
            int alt11=5;
            switch ( input.LA(1) ) {
            case Identifier:
                {
                int LA11_1 = input.LA(2);

                if ( (LA11_1==EOF||LA11_1==COMMA||LA11_1==CONCATENATE||LA11_1==DIVIDE||LA11_1==DOT||LA11_1==EQUAL||(LA11_1 >= GREATERTHAN && LA11_1 <= GREATERTHANOREQUALTO)||(LA11_1 >= KW_AND && LA11_1 <= KW_AS)||LA11_1==KW_BETWEEN||LA11_1==KW_FROM||(LA11_1 >= KW_GROUP && LA11_1 <= KW_HAVING)||LA11_1==KW_OR||LA11_1==KW_TIMEOUT||LA11_1==KW_WHERE||LA11_1==KW_XOR||(LA11_1 >= LESSTHAN && LA11_1 <= LESSTHANOREQUALTO)||LA11_1==MINUS||LA11_1==NOTEQUAL||LA11_1==PLUS||(LA11_1 >= RPAREN && LA11_1 <= RSQUARE)||LA11_1==STAR) ) {
                    alt11=1;
                }
                else if ( (LA11_1==LPAREN) ) {
                    alt11=4;
                }
                else {
                    if (state.backtracking>0) {state.failed=true; return retval;}
                    NoViableAltException nvae =
                        new NoViableAltException("", 11, 1, input);

                    throw nvae;

                }
                }
                break;
            case KW_ELEMENT_TIME:
                {
                alt11=1;
                }
                break;
            case Integer:
            case KW_NULL:
            case Number:
            case StringLiteral:
                {
                alt11=2;
                }
                break;
            case KW_AVG:
            case KW_COUNT:
            case KW_MAX:
            case KW_MIN:
            case KW_SUM:
                {
                alt11=3;
                }
                break;
            case LPAREN:
                {
                alt11=5;
                }
                break;
            default:
                if (state.backtracking>0) {state.failed=true; return retval;}
                NoViableAltException nvae =
                    new NoViableAltException("", 11, 0, input);

                throw nvae;

            }

            switch (alt11) {
                case 1 :
                    // CongerCQL.g:98:4: attr
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    pushFollow(FOLLOW_attr_in_arith_expr_main583);
                    attr26=attr();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, attr26.getTree());

                    }
                    break;
                case 2 :
                    // CongerCQL.g:99:4: const_value
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    pushFollow(FOLLOW_const_value_in_arith_expr_main588);
                    const_value27=const_value();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, const_value27.getTree());

                    }
                    break;
                case 3 :
                    // CongerCQL.g:100:4: aggr_expr
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    pushFollow(FOLLOW_aggr_expr_in_arith_expr_main593);
                    aggr_expr28=aggr_expr();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, aggr_expr28.getTree());

                    }
                    break;
                case 4 :
                    // CongerCQL.g:101:4: func_expr
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    pushFollow(FOLLOW_func_expr_in_arith_expr_main598);
                    func_expr29=func_expr();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, func_expr29.getTree());

                    }
                    break;
                case 5 :
                    // CongerCQL.g:104:4: LPAREN arith_expr RPAREN
                    {
                    LPAREN30=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_arith_expr_main610); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_LPAREN.add(LPAREN30);


                    pushFollow(FOLLOW_arith_expr_in_arith_expr_main612);
                    arith_expr31=arith_expr();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_arith_expr.add(arith_expr31.getTree());

                    RPAREN32=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_arith_expr_main614); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_RPAREN.add(RPAREN32);


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
                    // 106:3: -> arith_expr
                    {
                        adaptor.addChild(root_0, stream_arith_expr.nextTree());

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
    // $ANTLR end "arith_expr_main"


    public static class opt_where_clause_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "opt_where_clause"
    // CongerCQL.g:109:1: opt_where_clause : KW_WHERE non_mt_cond_list -> ^( TOK_WHERE non_mt_cond_list ) ;
    public final CongerCQLParser.opt_where_clause_return opt_where_clause() throws RecognitionException {
        CongerCQLParser.opt_where_clause_return retval = new CongerCQLParser.opt_where_clause_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token KW_WHERE33=null;
        CongerCQLParser.non_mt_cond_list_return non_mt_cond_list34 =null;


        CommonTree KW_WHERE33_tree=null;
        RewriteRuleTokenStream stream_KW_WHERE=new RewriteRuleTokenStream(adaptor,"token KW_WHERE");
        RewriteRuleSubtreeStream stream_non_mt_cond_list=new RewriteRuleSubtreeStream(adaptor,"rule non_mt_cond_list");
        try {
            // CongerCQL.g:110:2: ( KW_WHERE non_mt_cond_list -> ^( TOK_WHERE non_mt_cond_list ) )
            // CongerCQL.g:110:4: KW_WHERE non_mt_cond_list
            {
            KW_WHERE33=(Token)match(input,KW_WHERE,FOLLOW_KW_WHERE_in_opt_where_clause634); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_KW_WHERE.add(KW_WHERE33);


            pushFollow(FOLLOW_non_mt_cond_list_in_opt_where_clause636);
            non_mt_cond_list34=non_mt_cond_list();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_non_mt_cond_list.add(non_mt_cond_list34.getTree());

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
            // 111:9: -> ^( TOK_WHERE non_mt_cond_list )
            {
                // CongerCQL.g:111:12: ^( TOK_WHERE non_mt_cond_list )
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
    // CongerCQL.g:114:1: opt_having_clause : KW_HAVING non_mt_cond_list -> ^( TOK_HAVING non_mt_cond_list ) ;
    public final CongerCQLParser.opt_having_clause_return opt_having_clause() throws RecognitionException {
        CongerCQLParser.opt_having_clause_return retval = new CongerCQLParser.opt_having_clause_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token KW_HAVING35=null;
        CongerCQLParser.non_mt_cond_list_return non_mt_cond_list36 =null;


        CommonTree KW_HAVING35_tree=null;
        RewriteRuleTokenStream stream_KW_HAVING=new RewriteRuleTokenStream(adaptor,"token KW_HAVING");
        RewriteRuleSubtreeStream stream_non_mt_cond_list=new RewriteRuleSubtreeStream(adaptor,"rule non_mt_cond_list");
        try {
            // CongerCQL.g:115:2: ( KW_HAVING non_mt_cond_list -> ^( TOK_HAVING non_mt_cond_list ) )
            // CongerCQL.g:115:4: KW_HAVING non_mt_cond_list
            {
            KW_HAVING35=(Token)match(input,KW_HAVING,FOLLOW_KW_HAVING_in_opt_having_clause663); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_KW_HAVING.add(KW_HAVING35);


            pushFollow(FOLLOW_non_mt_cond_list_in_opt_having_clause665);
            non_mt_cond_list36=non_mt_cond_list();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_non_mt_cond_list.add(non_mt_cond_list36.getTree());

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
            // 116:9: -> ^( TOK_HAVING non_mt_cond_list )
            {
                // CongerCQL.g:116:12: ^( TOK_HAVING non_mt_cond_list )
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
    // CongerCQL.g:119:1: opt_group_by_clause : KW_GROUP KW_OVER LSQUARE window_type RSQUARE KW_BY non_mt_attr_list -> ^( TOK_GROUP_BY window_type non_mt_attr_list ) ;
    public final CongerCQLParser.opt_group_by_clause_return opt_group_by_clause() throws RecognitionException {
        CongerCQLParser.opt_group_by_clause_return retval = new CongerCQLParser.opt_group_by_clause_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token KW_GROUP37=null;
        Token KW_OVER38=null;
        Token LSQUARE39=null;
        Token RSQUARE41=null;
        Token KW_BY42=null;
        CongerCQLParser.window_type_return window_type40 =null;

        CongerCQLParser.non_mt_attr_list_return non_mt_attr_list43 =null;


        CommonTree KW_GROUP37_tree=null;
        CommonTree KW_OVER38_tree=null;
        CommonTree LSQUARE39_tree=null;
        CommonTree RSQUARE41_tree=null;
        CommonTree KW_BY42_tree=null;
        RewriteRuleTokenStream stream_KW_OVER=new RewriteRuleTokenStream(adaptor,"token KW_OVER");
        RewriteRuleTokenStream stream_LSQUARE=new RewriteRuleTokenStream(adaptor,"token LSQUARE");
        RewriteRuleTokenStream stream_RSQUARE=new RewriteRuleTokenStream(adaptor,"token RSQUARE");
        RewriteRuleTokenStream stream_KW_GROUP=new RewriteRuleTokenStream(adaptor,"token KW_GROUP");
        RewriteRuleTokenStream stream_KW_BY=new RewriteRuleTokenStream(adaptor,"token KW_BY");
        RewriteRuleSubtreeStream stream_window_type=new RewriteRuleSubtreeStream(adaptor,"rule window_type");
        RewriteRuleSubtreeStream stream_non_mt_attr_list=new RewriteRuleSubtreeStream(adaptor,"rule non_mt_attr_list");
        try {
            // CongerCQL.g:120:2: ( KW_GROUP KW_OVER LSQUARE window_type RSQUARE KW_BY non_mt_attr_list -> ^( TOK_GROUP_BY window_type non_mt_attr_list ) )
            // CongerCQL.g:120:4: KW_GROUP KW_OVER LSQUARE window_type RSQUARE KW_BY non_mt_attr_list
            {
            KW_GROUP37=(Token)match(input,KW_GROUP,FOLLOW_KW_GROUP_in_opt_group_by_clause692); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_KW_GROUP.add(KW_GROUP37);


            KW_OVER38=(Token)match(input,KW_OVER,FOLLOW_KW_OVER_in_opt_group_by_clause694); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_KW_OVER.add(KW_OVER38);


            LSQUARE39=(Token)match(input,LSQUARE,FOLLOW_LSQUARE_in_opt_group_by_clause696); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_LSQUARE.add(LSQUARE39);


            pushFollow(FOLLOW_window_type_in_opt_group_by_clause698);
            window_type40=window_type();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_window_type.add(window_type40.getTree());

            RSQUARE41=(Token)match(input,RSQUARE,FOLLOW_RSQUARE_in_opt_group_by_clause700); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_RSQUARE.add(RSQUARE41);


            KW_BY42=(Token)match(input,KW_BY,FOLLOW_KW_BY_in_opt_group_by_clause702); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_KW_BY.add(KW_BY42);


            pushFollow(FOLLOW_non_mt_attr_list_in_opt_group_by_clause704);
            non_mt_attr_list43=non_mt_attr_list();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_non_mt_attr_list.add(non_mt_attr_list43.getTree());

            // AST REWRITE
            // elements: non_mt_attr_list, window_type
            // token labels: 
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 121:9: -> ^( TOK_GROUP_BY window_type non_mt_attr_list )
            {
                // CongerCQL.g:121:12: ^( TOK_GROUP_BY window_type non_mt_attr_list )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_GROUP_BY, "TOK_GROUP_BY")
                , root_1);

                adaptor.addChild(root_1, stream_window_type.nextTree());

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
    // CongerCQL.g:125:1: from_clause : KW_FROM non_mt_relation_list ( KW_JOIN relation_variable KW_ON non_mt_cond_list ( KW_TIMEOUT timeout= Integer )? )? -> ^( TOK_FROM non_mt_relation_list ( ^( TOK_JOIN relation_variable KW_ON non_mt_cond_list ( KW_TIMEOUT $timeout)? ) )? ) ;
    public final CongerCQLParser.from_clause_return from_clause() throws RecognitionException {
        CongerCQLParser.from_clause_return retval = new CongerCQLParser.from_clause_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token timeout=null;
        Token KW_FROM44=null;
        Token KW_JOIN46=null;
        Token KW_ON48=null;
        Token KW_TIMEOUT50=null;
        CongerCQLParser.non_mt_relation_list_return non_mt_relation_list45 =null;

        CongerCQLParser.relation_variable_return relation_variable47 =null;

        CongerCQLParser.non_mt_cond_list_return non_mt_cond_list49 =null;


        CommonTree timeout_tree=null;
        CommonTree KW_FROM44_tree=null;
        CommonTree KW_JOIN46_tree=null;
        CommonTree KW_ON48_tree=null;
        CommonTree KW_TIMEOUT50_tree=null;
        RewriteRuleTokenStream stream_KW_JOIN=new RewriteRuleTokenStream(adaptor,"token KW_JOIN");
        RewriteRuleTokenStream stream_KW_TIMEOUT=new RewriteRuleTokenStream(adaptor,"token KW_TIMEOUT");
        RewriteRuleTokenStream stream_Integer=new RewriteRuleTokenStream(adaptor,"token Integer");
        RewriteRuleTokenStream stream_KW_ON=new RewriteRuleTokenStream(adaptor,"token KW_ON");
        RewriteRuleTokenStream stream_KW_FROM=new RewriteRuleTokenStream(adaptor,"token KW_FROM");
        RewriteRuleSubtreeStream stream_relation_variable=new RewriteRuleSubtreeStream(adaptor,"rule relation_variable");
        RewriteRuleSubtreeStream stream_non_mt_cond_list=new RewriteRuleSubtreeStream(adaptor,"rule non_mt_cond_list");
        RewriteRuleSubtreeStream stream_non_mt_relation_list=new RewriteRuleSubtreeStream(adaptor,"rule non_mt_relation_list");
        try {
            // CongerCQL.g:126:2: ( KW_FROM non_mt_relation_list ( KW_JOIN relation_variable KW_ON non_mt_cond_list ( KW_TIMEOUT timeout= Integer )? )? -> ^( TOK_FROM non_mt_relation_list ( ^( TOK_JOIN relation_variable KW_ON non_mt_cond_list ( KW_TIMEOUT $timeout)? ) )? ) )
            // CongerCQL.g:126:4: KW_FROM non_mt_relation_list ( KW_JOIN relation_variable KW_ON non_mt_cond_list ( KW_TIMEOUT timeout= Integer )? )?
            {
            KW_FROM44=(Token)match(input,KW_FROM,FOLLOW_KW_FROM_in_from_clause734); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_KW_FROM.add(KW_FROM44);


            pushFollow(FOLLOW_non_mt_relation_list_in_from_clause736);
            non_mt_relation_list45=non_mt_relation_list();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_non_mt_relation_list.add(non_mt_relation_list45.getTree());

            // CongerCQL.g:127:2: ( KW_JOIN relation_variable KW_ON non_mt_cond_list ( KW_TIMEOUT timeout= Integer )? )?
            int alt13=2;
            int LA13_0 = input.LA(1);

            if ( (LA13_0==KW_JOIN) ) {
                alt13=1;
            }
            switch (alt13) {
                case 1 :
                    // CongerCQL.g:127:3: KW_JOIN relation_variable KW_ON non_mt_cond_list ( KW_TIMEOUT timeout= Integer )?
                    {
                    KW_JOIN46=(Token)match(input,KW_JOIN,FOLLOW_KW_JOIN_in_from_clause741); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_KW_JOIN.add(KW_JOIN46);


                    pushFollow(FOLLOW_relation_variable_in_from_clause743);
                    relation_variable47=relation_variable();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_relation_variable.add(relation_variable47.getTree());

                    KW_ON48=(Token)match(input,KW_ON,FOLLOW_KW_ON_in_from_clause745); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_KW_ON.add(KW_ON48);


                    pushFollow(FOLLOW_non_mt_cond_list_in_from_clause747);
                    non_mt_cond_list49=non_mt_cond_list();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_non_mt_cond_list.add(non_mt_cond_list49.getTree());

                    // CongerCQL.g:127:52: ( KW_TIMEOUT timeout= Integer )?
                    int alt12=2;
                    int LA12_0 = input.LA(1);

                    if ( (LA12_0==KW_TIMEOUT) ) {
                        alt12=1;
                    }
                    switch (alt12) {
                        case 1 :
                            // CongerCQL.g:127:53: KW_TIMEOUT timeout= Integer
                            {
                            KW_TIMEOUT50=(Token)match(input,KW_TIMEOUT,FOLLOW_KW_TIMEOUT_in_from_clause750); if (state.failed) return retval; 
                            if ( state.backtracking==0 ) stream_KW_TIMEOUT.add(KW_TIMEOUT50);


                            timeout=(Token)match(input,Integer,FOLLOW_Integer_in_from_clause754); if (state.failed) return retval; 
                            if ( state.backtracking==0 ) stream_Integer.add(timeout);


                            }
                            break;

                    }


                    }
                    break;

            }


            // AST REWRITE
            // elements: non_mt_relation_list, KW_ON, KW_TIMEOUT, relation_variable, non_mt_cond_list, timeout
            // token labels: timeout
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleTokenStream stream_timeout=new RewriteRuleTokenStream(adaptor,"token timeout",timeout);
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 128:9: -> ^( TOK_FROM non_mt_relation_list ( ^( TOK_JOIN relation_variable KW_ON non_mt_cond_list ( KW_TIMEOUT $timeout)? ) )? )
            {
                // CongerCQL.g:128:12: ^( TOK_FROM non_mt_relation_list ( ^( TOK_JOIN relation_variable KW_ON non_mt_cond_list ( KW_TIMEOUT $timeout)? ) )? )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_FROM, "TOK_FROM")
                , root_1);

                adaptor.addChild(root_1, stream_non_mt_relation_list.nextTree());

                // CongerCQL.g:129:13: ( ^( TOK_JOIN relation_variable KW_ON non_mt_cond_list ( KW_TIMEOUT $timeout)? ) )?
                if ( stream_KW_ON.hasNext()||stream_KW_TIMEOUT.hasNext()||stream_relation_variable.hasNext()||stream_non_mt_cond_list.hasNext()||stream_timeout.hasNext() ) {
                    // CongerCQL.g:129:13: ^( TOK_JOIN relation_variable KW_ON non_mt_cond_list ( KW_TIMEOUT $timeout)? )
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

                    // CongerCQL.g:129:65: ( KW_TIMEOUT $timeout)?
                    if ( stream_KW_TIMEOUT.hasNext()||stream_timeout.hasNext() ) {
                        adaptor.addChild(root_2, 
                        stream_KW_TIMEOUT.nextNode()
                        );

                        adaptor.addChild(root_2, stream_timeout.nextNode());

                    }
                    stream_KW_TIMEOUT.reset();
                    stream_timeout.reset();

                    adaptor.addChild(root_1, root_2);
                    }

                }
                stream_KW_ON.reset();
                stream_KW_TIMEOUT.reset();
                stream_relation_variable.reset();
                stream_non_mt_cond_list.reset();
                stream_timeout.reset();

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
    // CongerCQL.g:132:1: non_mt_cond_list : non_mt_cond_list_main ( cond_list_operator non_mt_cond_list_main )* -> ^( TOK_COND_LIST ( ^( TOK_COND ( cond_list_operator )? non_mt_cond_list_main ) )+ ) ;
    public final CongerCQLParser.non_mt_cond_list_return non_mt_cond_list() throws RecognitionException {
        CongerCQLParser.non_mt_cond_list_return retval = new CongerCQLParser.non_mt_cond_list_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        CongerCQLParser.non_mt_cond_list_main_return non_mt_cond_list_main51 =null;

        CongerCQLParser.cond_list_operator_return cond_list_operator52 =null;

        CongerCQLParser.non_mt_cond_list_main_return non_mt_cond_list_main53 =null;


        RewriteRuleSubtreeStream stream_cond_list_operator=new RewriteRuleSubtreeStream(adaptor,"rule cond_list_operator");
        RewriteRuleSubtreeStream stream_non_mt_cond_list_main=new RewriteRuleSubtreeStream(adaptor,"rule non_mt_cond_list_main");
        try {
            // CongerCQL.g:133:2: ( non_mt_cond_list_main ( cond_list_operator non_mt_cond_list_main )* -> ^( TOK_COND_LIST ( ^( TOK_COND ( cond_list_operator )? non_mt_cond_list_main ) )+ ) )
            // CongerCQL.g:133:4: non_mt_cond_list_main ( cond_list_operator non_mt_cond_list_main )*
            {
            pushFollow(FOLLOW_non_mt_cond_list_main_in_non_mt_cond_list818);
            non_mt_cond_list_main51=non_mt_cond_list_main();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_non_mt_cond_list_main.add(non_mt_cond_list_main51.getTree());

            // CongerCQL.g:133:26: ( cond_list_operator non_mt_cond_list_main )*
            loop14:
            do {
                int alt14=2;
                int LA14_0 = input.LA(1);

                if ( (LA14_0==KW_AND||LA14_0==KW_OR||LA14_0==KW_XOR) ) {
                    alt14=1;
                }


                switch (alt14) {
            	case 1 :
            	    // CongerCQL.g:133:27: cond_list_operator non_mt_cond_list_main
            	    {
            	    pushFollow(FOLLOW_cond_list_operator_in_non_mt_cond_list821);
            	    cond_list_operator52=cond_list_operator();

            	    state._fsp--;
            	    if (state.failed) return retval;
            	    if ( state.backtracking==0 ) stream_cond_list_operator.add(cond_list_operator52.getTree());

            	    pushFollow(FOLLOW_non_mt_cond_list_main_in_non_mt_cond_list823);
            	    non_mt_cond_list_main53=non_mt_cond_list_main();

            	    state._fsp--;
            	    if (state.failed) return retval;
            	    if ( state.backtracking==0 ) stream_non_mt_cond_list_main.add(non_mt_cond_list_main53.getTree());

            	    }
            	    break;

            	default :
            	    break loop14;
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
            // 134:9: -> ^( TOK_COND_LIST ( ^( TOK_COND ( cond_list_operator )? non_mt_cond_list_main ) )+ )
            {
                // CongerCQL.g:134:12: ^( TOK_COND_LIST ( ^( TOK_COND ( cond_list_operator )? non_mt_cond_list_main ) )+ )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_COND_LIST, "TOK_COND_LIST")
                , root_1);

                if ( !(stream_non_mt_cond_list_main.hasNext()) ) {
                    throw new RewriteEarlyExitException();
                }
                while ( stream_non_mt_cond_list_main.hasNext() ) {
                    // CongerCQL.g:134:28: ^( TOK_COND ( cond_list_operator )? non_mt_cond_list_main )
                    {
                    CommonTree root_2 = (CommonTree)adaptor.nil();
                    root_2 = (CommonTree)adaptor.becomeRoot(
                    (CommonTree)adaptor.create(TOK_COND, "TOK_COND")
                    , root_2);

                    // CongerCQL.g:134:39: ( cond_list_operator )?
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
    // CongerCQL.g:137:1: cond_list_operator : ( KW_XOR | KW_OR | KW_AND );
    public final CongerCQLParser.cond_list_operator_return cond_list_operator() throws RecognitionException {
        CongerCQLParser.cond_list_operator_return retval = new CongerCQLParser.cond_list_operator_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token set54=null;

        CommonTree set54_tree=null;

        try {
            // CongerCQL.g:138:5: ( KW_XOR | KW_OR | KW_AND )
            // CongerCQL.g:
            {
            root_0 = (CommonTree)adaptor.nil();


            set54=(Token)input.LT(1);

            if ( input.LA(1)==KW_AND||input.LA(1)==KW_OR||input.LA(1)==KW_XOR ) {
                input.consume();
                if ( state.backtracking==0 ) adaptor.addChild(root_0, 
                (CommonTree)adaptor.create(set54)
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
    // CongerCQL.g:141:1: non_mt_cond_list_main options {backtrack=true; } : ( KW_NOT non_mt_cond_list_main | unary_condition | LPAREN unary_condition RPAREN );
    public final CongerCQLParser.non_mt_cond_list_main_return non_mt_cond_list_main() throws RecognitionException {
        CongerCQLParser.non_mt_cond_list_main_return retval = new CongerCQLParser.non_mt_cond_list_main_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token KW_NOT55=null;
        Token LPAREN58=null;
        Token RPAREN60=null;
        CongerCQLParser.non_mt_cond_list_main_return non_mt_cond_list_main56 =null;

        CongerCQLParser.unary_condition_return unary_condition57 =null;

        CongerCQLParser.unary_condition_return unary_condition59 =null;


        CommonTree KW_NOT55_tree=null;
        CommonTree LPAREN58_tree=null;
        CommonTree RPAREN60_tree=null;

        try {
            // CongerCQL.g:143:2: ( KW_NOT non_mt_cond_list_main | unary_condition | LPAREN unary_condition RPAREN )
            int alt15=3;
            switch ( input.LA(1) ) {
            case KW_NOT:
                {
                alt15=1;
                }
                break;
            case Identifier:
            case Integer:
            case KW_AVG:
            case KW_COUNT:
            case KW_ELEMENT_TIME:
            case KW_MAX:
            case KW_MIN:
            case KW_NULL:
            case KW_SUM:
            case Number:
            case StringLiteral:
                {
                alt15=2;
                }
                break;
            case LPAREN:
                {
                int LA15_13 = input.LA(2);

                if ( (synpred2_CongerCQL()) ) {
                    alt15=2;
                }
                else if ( (true) ) {
                    alt15=3;
                }
                else {
                    if (state.backtracking>0) {state.failed=true; return retval;}
                    NoViableAltException nvae =
                        new NoViableAltException("", 15, 13, input);

                    throw nvae;

                }
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
                    // CongerCQL.g:143:4: KW_NOT non_mt_cond_list_main
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    KW_NOT55=(Token)match(input,KW_NOT,FOLLOW_KW_NOT_in_non_mt_cond_list_main893); if (state.failed) return retval;
                    if ( state.backtracking==0 ) {
                    KW_NOT55_tree = 
                    (CommonTree)adaptor.create(KW_NOT55)
                    ;
                    adaptor.addChild(root_0, KW_NOT55_tree);
                    }

                    pushFollow(FOLLOW_non_mt_cond_list_main_in_non_mt_cond_list_main895);
                    non_mt_cond_list_main56=non_mt_cond_list_main();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, non_mt_cond_list_main56.getTree());

                    }
                    break;
                case 2 :
                    // CongerCQL.g:144:4: unary_condition
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    pushFollow(FOLLOW_unary_condition_in_non_mt_cond_list_main900);
                    unary_condition57=unary_condition();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, unary_condition57.getTree());

                    }
                    break;
                case 3 :
                    // CongerCQL.g:145:4: LPAREN unary_condition RPAREN
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    LPAREN58=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_non_mt_cond_list_main905); if (state.failed) return retval;
                    if ( state.backtracking==0 ) {
                    LPAREN58_tree = 
                    (CommonTree)adaptor.create(LPAREN58)
                    ;
                    adaptor.addChild(root_0, LPAREN58_tree);
                    }

                    pushFollow(FOLLOW_unary_condition_in_non_mt_cond_list_main907);
                    unary_condition59=unary_condition();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, unary_condition59.getTree());

                    RPAREN60=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_non_mt_cond_list_main909); if (state.failed) return retval;
                    if ( state.backtracking==0 ) {
                    RPAREN60_tree = 
                    (CommonTree)adaptor.create(RPAREN60)
                    ;
                    adaptor.addChild(root_0, RPAREN60_tree);
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
    // CongerCQL.g:149:1: unary_condition : arith_expr ( between_condition_right | compare_condition_right ) ;
    public final CongerCQLParser.unary_condition_return unary_condition() throws RecognitionException {
        CongerCQLParser.unary_condition_return retval = new CongerCQLParser.unary_condition_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        CongerCQLParser.arith_expr_return arith_expr61 =null;

        CongerCQLParser.between_condition_right_return between_condition_right62 =null;

        CongerCQLParser.compare_condition_right_return compare_condition_right63 =null;



        try {
            // CongerCQL.g:150:2: ( arith_expr ( between_condition_right | compare_condition_right ) )
            // CongerCQL.g:150:4: arith_expr ( between_condition_right | compare_condition_right )
            {
            root_0 = (CommonTree)adaptor.nil();


            pushFollow(FOLLOW_arith_expr_in_unary_condition921);
            arith_expr61=arith_expr();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) adaptor.addChild(root_0, arith_expr61.getTree());

            // CongerCQL.g:150:15: ( between_condition_right | compare_condition_right )
            int alt16=2;
            int LA16_0 = input.LA(1);

            if ( (LA16_0==KW_BETWEEN) ) {
                alt16=1;
            }
            else if ( (LA16_0==EQUAL||(LA16_0 >= GREATERTHAN && LA16_0 <= GREATERTHANOREQUALTO)||(LA16_0 >= LESSTHAN && LA16_0 <= LESSTHANOREQUALTO)||LA16_0==NOTEQUAL) ) {
                alt16=2;
            }
            else {
                if (state.backtracking>0) {state.failed=true; return retval;}
                NoViableAltException nvae =
                    new NoViableAltException("", 16, 0, input);

                throw nvae;

            }
            switch (alt16) {
                case 1 :
                    // CongerCQL.g:150:16: between_condition_right
                    {
                    pushFollow(FOLLOW_between_condition_right_in_unary_condition924);
                    between_condition_right62=between_condition_right();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, between_condition_right62.getTree());

                    }
                    break;
                case 2 :
                    // CongerCQL.g:150:42: compare_condition_right
                    {
                    pushFollow(FOLLOW_compare_condition_right_in_unary_condition928);
                    compare_condition_right63=compare_condition_right();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, compare_condition_right63.getTree());

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
    // CongerCQL.g:153:1: between_condition_right : KW_BETWEEN arith_expr KW_AND arith_expr ;
    public final CongerCQLParser.between_condition_right_return between_condition_right() throws RecognitionException {
        CongerCQLParser.between_condition_right_return retval = new CongerCQLParser.between_condition_right_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token KW_BETWEEN64=null;
        Token KW_AND66=null;
        CongerCQLParser.arith_expr_return arith_expr65 =null;

        CongerCQLParser.arith_expr_return arith_expr67 =null;


        CommonTree KW_BETWEEN64_tree=null;
        CommonTree KW_AND66_tree=null;

        try {
            // CongerCQL.g:154:2: ( KW_BETWEEN arith_expr KW_AND arith_expr )
            // CongerCQL.g:154:4: KW_BETWEEN arith_expr KW_AND arith_expr
            {
            root_0 = (CommonTree)adaptor.nil();


            KW_BETWEEN64=(Token)match(input,KW_BETWEEN,FOLLOW_KW_BETWEEN_in_between_condition_right941); if (state.failed) return retval;
            if ( state.backtracking==0 ) {
            KW_BETWEEN64_tree = 
            (CommonTree)adaptor.create(KW_BETWEEN64)
            ;
            adaptor.addChild(root_0, KW_BETWEEN64_tree);
            }

            pushFollow(FOLLOW_arith_expr_in_between_condition_right943);
            arith_expr65=arith_expr();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) adaptor.addChild(root_0, arith_expr65.getTree());

            KW_AND66=(Token)match(input,KW_AND,FOLLOW_KW_AND_in_between_condition_right945); if (state.failed) return retval;
            if ( state.backtracking==0 ) {
            KW_AND66_tree = 
            (CommonTree)adaptor.create(KW_AND66)
            ;
            adaptor.addChild(root_0, KW_AND66_tree);
            }

            pushFollow(FOLLOW_arith_expr_in_between_condition_right947);
            arith_expr67=arith_expr();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) adaptor.addChild(root_0, arith_expr67.getTree());

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
    // CongerCQL.g:157:1: compare_condition_right : unary_condition_operator arith_expr ;
    public final CongerCQLParser.compare_condition_right_return compare_condition_right() throws RecognitionException {
        CongerCQLParser.compare_condition_right_return retval = new CongerCQLParser.compare_condition_right_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        CongerCQLParser.unary_condition_operator_return unary_condition_operator68 =null;

        CongerCQLParser.arith_expr_return arith_expr69 =null;



        try {
            // CongerCQL.g:158:2: ( unary_condition_operator arith_expr )
            // CongerCQL.g:158:4: unary_condition_operator arith_expr
            {
            root_0 = (CommonTree)adaptor.nil();


            pushFollow(FOLLOW_unary_condition_operator_in_compare_condition_right958);
            unary_condition_operator68=unary_condition_operator();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) adaptor.addChild(root_0, unary_condition_operator68.getTree());

            pushFollow(FOLLOW_arith_expr_in_compare_condition_right960);
            arith_expr69=arith_expr();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) adaptor.addChild(root_0, arith_expr69.getTree());

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
    // CongerCQL.g:161:1: unary_condition_operator : ( LESSTHAN | LESSTHANOREQUALTO | GREATERTHAN | GREATERTHANOREQUALTO | EQUAL | NOTEQUAL );
    public final CongerCQLParser.unary_condition_operator_return unary_condition_operator() throws RecognitionException {
        CongerCQLParser.unary_condition_operator_return retval = new CongerCQLParser.unary_condition_operator_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token set70=null;

        CommonTree set70_tree=null;

        try {
            // CongerCQL.g:162:5: ( LESSTHAN | LESSTHANOREQUALTO | GREATERTHAN | GREATERTHANOREQUALTO | EQUAL | NOTEQUAL )
            // CongerCQL.g:
            {
            root_0 = (CommonTree)adaptor.nil();


            set70=(Token)input.LT(1);

            if ( input.LA(1)==EQUAL||(input.LA(1) >= GREATERTHAN && input.LA(1) <= GREATERTHANOREQUALTO)||(input.LA(1) >= LESSTHAN && input.LA(1) <= LESSTHANOREQUALTO)||input.LA(1)==NOTEQUAL ) {
                input.consume();
                if ( state.backtracking==0 ) adaptor.addChild(root_0, 
                (CommonTree)adaptor.create(set70)
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
    // CongerCQL.g:165:1: const_string : StringLiteral ;
    public final CongerCQLParser.const_string_return const_string() throws RecognitionException {
        CongerCQLParser.const_string_return retval = new CongerCQLParser.const_string_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token StringLiteral71=null;

        CommonTree StringLiteral71_tree=null;

        try {
            // CongerCQL.g:166:2: ( StringLiteral )
            // CongerCQL.g:166:4: StringLiteral
            {
            root_0 = (CommonTree)adaptor.nil();


            StringLiteral71=(Token)match(input,StringLiteral,FOLLOW_StringLiteral_in_const_string1009); if (state.failed) return retval;
            if ( state.backtracking==0 ) {
            StringLiteral71_tree = 
            (CommonTree)adaptor.create(StringLiteral71)
            ;
            adaptor.addChild(root_0, StringLiteral71_tree);
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
    // CongerCQL.g:169:1: non_mt_arg_list : arith_expr ( options {greedy=true; } : COMMA non_mt_arg_list )* -> ^( TOK_ARG_LIST ( arith_expr )+ ) ;
    public final CongerCQLParser.non_mt_arg_list_return non_mt_arg_list() throws RecognitionException {
        CongerCQLParser.non_mt_arg_list_return retval = new CongerCQLParser.non_mt_arg_list_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token COMMA73=null;
        CongerCQLParser.arith_expr_return arith_expr72 =null;

        CongerCQLParser.non_mt_arg_list_return non_mt_arg_list74 =null;


        CommonTree COMMA73_tree=null;
        RewriteRuleTokenStream stream_COMMA=new RewriteRuleTokenStream(adaptor,"token COMMA");
        RewriteRuleSubtreeStream stream_arith_expr=new RewriteRuleSubtreeStream(adaptor,"rule arith_expr");
        RewriteRuleSubtreeStream stream_non_mt_arg_list=new RewriteRuleSubtreeStream(adaptor,"rule non_mt_arg_list");
        try {
            // CongerCQL.g:170:2: ( arith_expr ( options {greedy=true; } : COMMA non_mt_arg_list )* -> ^( TOK_ARG_LIST ( arith_expr )+ ) )
            // CongerCQL.g:170:4: arith_expr ( options {greedy=true; } : COMMA non_mt_arg_list )*
            {
            pushFollow(FOLLOW_arith_expr_in_non_mt_arg_list1020);
            arith_expr72=arith_expr();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_arith_expr.add(arith_expr72.getTree());

            // CongerCQL.g:170:15: ( options {greedy=true; } : COMMA non_mt_arg_list )*
            loop17:
            do {
                int alt17=2;
                int LA17_0 = input.LA(1);

                if ( (LA17_0==COMMA) ) {
                    alt17=1;
                }


                switch (alt17) {
            	case 1 :
            	    // CongerCQL.g:170:39: COMMA non_mt_arg_list
            	    {
            	    COMMA73=(Token)match(input,COMMA,FOLLOW_COMMA_in_non_mt_arg_list1031); if (state.failed) return retval; 
            	    if ( state.backtracking==0 ) stream_COMMA.add(COMMA73);


            	    pushFollow(FOLLOW_non_mt_arg_list_in_non_mt_arg_list1033);
            	    non_mt_arg_list74=non_mt_arg_list();

            	    state._fsp--;
            	    if (state.failed) return retval;
            	    if ( state.backtracking==0 ) stream_non_mt_arg_list.add(non_mt_arg_list74.getTree());

            	    }
            	    break;

            	default :
            	    break loop17;
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
            // 171:3: -> ^( TOK_ARG_LIST ( arith_expr )+ )
            {
                // CongerCQL.g:171:6: ^( TOK_ARG_LIST ( arith_expr )+ )
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
    // CongerCQL.g:174:1: non_mt_relation_list : relation_variable ( options {greedy=true; } : COMMA non_mt_relation_list )* -> ^( TOK_RELATION_LIST ( relation_variable )+ ) ;
    public final CongerCQLParser.non_mt_relation_list_return non_mt_relation_list() throws RecognitionException {
        CongerCQLParser.non_mt_relation_list_return retval = new CongerCQLParser.non_mt_relation_list_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token COMMA76=null;
        CongerCQLParser.relation_variable_return relation_variable75 =null;

        CongerCQLParser.non_mt_relation_list_return non_mt_relation_list77 =null;


        CommonTree COMMA76_tree=null;
        RewriteRuleTokenStream stream_COMMA=new RewriteRuleTokenStream(adaptor,"token COMMA");
        RewriteRuleSubtreeStream stream_relation_variable=new RewriteRuleSubtreeStream(adaptor,"rule relation_variable");
        RewriteRuleSubtreeStream stream_non_mt_relation_list=new RewriteRuleSubtreeStream(adaptor,"rule non_mt_relation_list");
        try {
            // CongerCQL.g:175:2: ( relation_variable ( options {greedy=true; } : COMMA non_mt_relation_list )* -> ^( TOK_RELATION_LIST ( relation_variable )+ ) )
            // CongerCQL.g:175:4: relation_variable ( options {greedy=true; } : COMMA non_mt_relation_list )*
            {
            pushFollow(FOLLOW_relation_variable_in_non_mt_relation_list1058);
            relation_variable75=relation_variable();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_relation_variable.add(relation_variable75.getTree());

            // CongerCQL.g:175:22: ( options {greedy=true; } : COMMA non_mt_relation_list )*
            loop18:
            do {
                int alt18=2;
                int LA18_0 = input.LA(1);

                if ( (LA18_0==COMMA) ) {
                    alt18=1;
                }


                switch (alt18) {
            	case 1 :
            	    // CongerCQL.g:175:46: COMMA non_mt_relation_list
            	    {
            	    COMMA76=(Token)match(input,COMMA,FOLLOW_COMMA_in_non_mt_relation_list1069); if (state.failed) return retval; 
            	    if ( state.backtracking==0 ) stream_COMMA.add(COMMA76);


            	    pushFollow(FOLLOW_non_mt_relation_list_in_non_mt_relation_list1071);
            	    non_mt_relation_list77=non_mt_relation_list();

            	    state._fsp--;
            	    if (state.failed) return retval;
            	    if ( state.backtracking==0 ) stream_non_mt_relation_list.add(non_mt_relation_list77.getTree());

            	    }
            	    break;

            	default :
            	    break loop18;
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
            // 176:9: -> ^( TOK_RELATION_LIST ( relation_variable )+ )
            {
                // CongerCQL.g:176:12: ^( TOK_RELATION_LIST ( relation_variable )+ )
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
    // CongerCQL.g:179:1: relation_variable : variableName= Identifier ( LSQUARE window_type RSQUARE )? ( KW_AS alias= Identifier )? -> ^( TOK_RELATION_VARIABLE $variableName ( ^( TOK_WINDOW window_type ) )? ( $alias)? ) ;
    public final CongerCQLParser.relation_variable_return relation_variable() throws RecognitionException {
        CongerCQLParser.relation_variable_return retval = new CongerCQLParser.relation_variable_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token variableName=null;
        Token alias=null;
        Token LSQUARE78=null;
        Token RSQUARE80=null;
        Token KW_AS81=null;
        CongerCQLParser.window_type_return window_type79 =null;


        CommonTree variableName_tree=null;
        CommonTree alias_tree=null;
        CommonTree LSQUARE78_tree=null;
        CommonTree RSQUARE80_tree=null;
        CommonTree KW_AS81_tree=null;
        RewriteRuleTokenStream stream_KW_AS=new RewriteRuleTokenStream(adaptor,"token KW_AS");
        RewriteRuleTokenStream stream_LSQUARE=new RewriteRuleTokenStream(adaptor,"token LSQUARE");
        RewriteRuleTokenStream stream_RSQUARE=new RewriteRuleTokenStream(adaptor,"token RSQUARE");
        RewriteRuleTokenStream stream_Identifier=new RewriteRuleTokenStream(adaptor,"token Identifier");
        RewriteRuleSubtreeStream stream_window_type=new RewriteRuleSubtreeStream(adaptor,"rule window_type");
        try {
            // CongerCQL.g:180:2: (variableName= Identifier ( LSQUARE window_type RSQUARE )? ( KW_AS alias= Identifier )? -> ^( TOK_RELATION_VARIABLE $variableName ( ^( TOK_WINDOW window_type ) )? ( $alias)? ) )
            // CongerCQL.g:180:4: variableName= Identifier ( LSQUARE window_type RSQUARE )? ( KW_AS alias= Identifier )?
            {
            variableName=(Token)match(input,Identifier,FOLLOW_Identifier_in_relation_variable1103); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_Identifier.add(variableName);


            // CongerCQL.g:180:28: ( LSQUARE window_type RSQUARE )?
            int alt19=2;
            int LA19_0 = input.LA(1);

            if ( (LA19_0==LSQUARE) ) {
                alt19=1;
            }
            switch (alt19) {
                case 1 :
                    // CongerCQL.g:180:29: LSQUARE window_type RSQUARE
                    {
                    LSQUARE78=(Token)match(input,LSQUARE,FOLLOW_LSQUARE_in_relation_variable1106); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_LSQUARE.add(LSQUARE78);


                    pushFollow(FOLLOW_window_type_in_relation_variable1108);
                    window_type79=window_type();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_window_type.add(window_type79.getTree());

                    RSQUARE80=(Token)match(input,RSQUARE,FOLLOW_RSQUARE_in_relation_variable1110); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_RSQUARE.add(RSQUARE80);


                    }
                    break;

            }


            // CongerCQL.g:180:59: ( KW_AS alias= Identifier )?
            int alt20=2;
            int LA20_0 = input.LA(1);

            if ( (LA20_0==KW_AS) ) {
                alt20=1;
            }
            switch (alt20) {
                case 1 :
                    // CongerCQL.g:180:61: KW_AS alias= Identifier
                    {
                    KW_AS81=(Token)match(input,KW_AS,FOLLOW_KW_AS_in_relation_variable1116); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_KW_AS.add(KW_AS81);


                    alias=(Token)match(input,Identifier,FOLLOW_Identifier_in_relation_variable1120); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_Identifier.add(alias);


                    }
                    break;

            }


            // AST REWRITE
            // elements: variableName, window_type, alias
            // token labels: alias, variableName
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleTokenStream stream_alias=new RewriteRuleTokenStream(adaptor,"token alias",alias);
            RewriteRuleTokenStream stream_variableName=new RewriteRuleTokenStream(adaptor,"token variableName",variableName);
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 181:9: -> ^( TOK_RELATION_VARIABLE $variableName ( ^( TOK_WINDOW window_type ) )? ( $alias)? )
            {
                // CongerCQL.g:181:12: ^( TOK_RELATION_VARIABLE $variableName ( ^( TOK_WINDOW window_type ) )? ( $alias)? )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_RELATION_VARIABLE, "TOK_RELATION_VARIABLE")
                , root_1);

                adaptor.addChild(root_1, stream_variableName.nextNode());

                // CongerCQL.g:181:50: ( ^( TOK_WINDOW window_type ) )?
                if ( stream_window_type.hasNext() ) {
                    // CongerCQL.g:181:50: ^( TOK_WINDOW window_type )
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

                // CongerCQL.g:181:78: ( $alias)?
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
    // CongerCQL.g:184:1: window_type : ( KW_RANGE range= time_spec ( KW_SLIDE slidetime= time_spec )? ( KW_ON arith_expr )? -> ^( TOK_WINDOW KW_RANGE $range ( KW_SLIDE $slidetime)? ) | KW_ROWS row= Integer ( KW_SLIDE slide= Integer )? -> ^( TOK_WINDOW KW_ROWS $row ( KW_SLIDE $slide)? ) );
    public final CongerCQLParser.window_type_return window_type() throws RecognitionException {
        CongerCQLParser.window_type_return retval = new CongerCQLParser.window_type_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token row=null;
        Token slide=null;
        Token KW_RANGE82=null;
        Token KW_SLIDE83=null;
        Token KW_ON84=null;
        Token KW_ROWS86=null;
        Token KW_SLIDE87=null;
        CongerCQLParser.time_spec_return range =null;

        CongerCQLParser.time_spec_return slidetime =null;

        CongerCQLParser.arith_expr_return arith_expr85 =null;


        CommonTree row_tree=null;
        CommonTree slide_tree=null;
        CommonTree KW_RANGE82_tree=null;
        CommonTree KW_SLIDE83_tree=null;
        CommonTree KW_ON84_tree=null;
        CommonTree KW_ROWS86_tree=null;
        CommonTree KW_SLIDE87_tree=null;
        RewriteRuleTokenStream stream_KW_ROWS=new RewriteRuleTokenStream(adaptor,"token KW_ROWS");
        RewriteRuleTokenStream stream_KW_SLIDE=new RewriteRuleTokenStream(adaptor,"token KW_SLIDE");
        RewriteRuleTokenStream stream_Integer=new RewriteRuleTokenStream(adaptor,"token Integer");
        RewriteRuleTokenStream stream_KW_ON=new RewriteRuleTokenStream(adaptor,"token KW_ON");
        RewriteRuleTokenStream stream_KW_RANGE=new RewriteRuleTokenStream(adaptor,"token KW_RANGE");
        RewriteRuleSubtreeStream stream_time_spec=new RewriteRuleSubtreeStream(adaptor,"rule time_spec");
        RewriteRuleSubtreeStream stream_arith_expr=new RewriteRuleSubtreeStream(adaptor,"rule arith_expr");
        try {
            // CongerCQL.g:185:2: ( KW_RANGE range= time_spec ( KW_SLIDE slidetime= time_spec )? ( KW_ON arith_expr )? -> ^( TOK_WINDOW KW_RANGE $range ( KW_SLIDE $slidetime)? ) | KW_ROWS row= Integer ( KW_SLIDE slide= Integer )? -> ^( TOK_WINDOW KW_ROWS $row ( KW_SLIDE $slide)? ) )
            int alt24=2;
            int LA24_0 = input.LA(1);

            if ( (LA24_0==KW_RANGE) ) {
                alt24=1;
            }
            else if ( (LA24_0==KW_ROWS) ) {
                alt24=2;
            }
            else {
                if (state.backtracking>0) {state.failed=true; return retval;}
                NoViableAltException nvae =
                    new NoViableAltException("", 24, 0, input);

                throw nvae;

            }
            switch (alt24) {
                case 1 :
                    // CongerCQL.g:185:4: KW_RANGE range= time_spec ( KW_SLIDE slidetime= time_spec )? ( KW_ON arith_expr )?
                    {
                    KW_RANGE82=(Token)match(input,KW_RANGE,FOLLOW_KW_RANGE_in_window_type1161); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_KW_RANGE.add(KW_RANGE82);


                    pushFollow(FOLLOW_time_spec_in_window_type1165);
                    range=time_spec();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_time_spec.add(range.getTree());

                    // CongerCQL.g:185:29: ( KW_SLIDE slidetime= time_spec )?
                    int alt21=2;
                    int LA21_0 = input.LA(1);

                    if ( (LA21_0==KW_SLIDE) ) {
                        alt21=1;
                    }
                    switch (alt21) {
                        case 1 :
                            // CongerCQL.g:185:31: KW_SLIDE slidetime= time_spec
                            {
                            KW_SLIDE83=(Token)match(input,KW_SLIDE,FOLLOW_KW_SLIDE_in_window_type1169); if (state.failed) return retval; 
                            if ( state.backtracking==0 ) stream_KW_SLIDE.add(KW_SLIDE83);


                            pushFollow(FOLLOW_time_spec_in_window_type1173);
                            slidetime=time_spec();

                            state._fsp--;
                            if (state.failed) return retval;
                            if ( state.backtracking==0 ) stream_time_spec.add(slidetime.getTree());

                            }
                            break;

                    }


                    // CongerCQL.g:185:62: ( KW_ON arith_expr )?
                    int alt22=2;
                    int LA22_0 = input.LA(1);

                    if ( (LA22_0==KW_ON) ) {
                        alt22=1;
                    }
                    switch (alt22) {
                        case 1 :
                            // CongerCQL.g:185:63: KW_ON arith_expr
                            {
                            KW_ON84=(Token)match(input,KW_ON,FOLLOW_KW_ON_in_window_type1178); if (state.failed) return retval; 
                            if ( state.backtracking==0 ) stream_KW_ON.add(KW_ON84);


                            pushFollow(FOLLOW_arith_expr_in_window_type1180);
                            arith_expr85=arith_expr();

                            state._fsp--;
                            if (state.failed) return retval;
                            if ( state.backtracking==0 ) stream_arith_expr.add(arith_expr85.getTree());

                            }
                            break;

                    }


                    // AST REWRITE
                    // elements: KW_SLIDE, KW_RANGE, range, slidetime
                    // token labels: 
                    // rule labels: retval, range, slidetime
                    // token list labels: 
                    // rule list labels: 
                    // wildcard labels: 
                    if ( state.backtracking==0 ) {

                    retval.tree = root_0;
                    RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);
                    RewriteRuleSubtreeStream stream_range=new RewriteRuleSubtreeStream(adaptor,"rule range",range!=null?range.tree:null);
                    RewriteRuleSubtreeStream stream_slidetime=new RewriteRuleSubtreeStream(adaptor,"rule slidetime",slidetime!=null?slidetime.tree:null);

                    root_0 = (CommonTree)adaptor.nil();
                    // 186:6: -> ^( TOK_WINDOW KW_RANGE $range ( KW_SLIDE $slidetime)? )
                    {
                        // CongerCQL.g:186:9: ^( TOK_WINDOW KW_RANGE $range ( KW_SLIDE $slidetime)? )
                        {
                        CommonTree root_1 = (CommonTree)adaptor.nil();
                        root_1 = (CommonTree)adaptor.becomeRoot(
                        (CommonTree)adaptor.create(TOK_WINDOW, "TOK_WINDOW")
                        , root_1);

                        adaptor.addChild(root_1, 
                        stream_KW_RANGE.nextNode()
                        );

                        adaptor.addChild(root_1, stream_range.nextTree());

                        // CongerCQL.g:186:38: ( KW_SLIDE $slidetime)?
                        if ( stream_KW_SLIDE.hasNext()||stream_slidetime.hasNext() ) {
                            adaptor.addChild(root_1, 
                            stream_KW_SLIDE.nextNode()
                            );

                            adaptor.addChild(root_1, stream_slidetime.nextTree());

                        }
                        stream_KW_SLIDE.reset();
                        stream_slidetime.reset();

                        adaptor.addChild(root_0, root_1);
                        }

                    }


                    retval.tree = root_0;
                    }

                    }
                    break;
                case 2 :
                    // CongerCQL.g:187:4: KW_ROWS row= Integer ( KW_SLIDE slide= Integer )?
                    {
                    KW_ROWS86=(Token)match(input,KW_ROWS,FOLLOW_KW_ROWS_in_window_type1211); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_KW_ROWS.add(KW_ROWS86);


                    row=(Token)match(input,Integer,FOLLOW_Integer_in_window_type1215); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_Integer.add(row);


                    // CongerCQL.g:187:24: ( KW_SLIDE slide= Integer )?
                    int alt23=2;
                    int LA23_0 = input.LA(1);

                    if ( (LA23_0==KW_SLIDE) ) {
                        alt23=1;
                    }
                    switch (alt23) {
                        case 1 :
                            // CongerCQL.g:187:26: KW_SLIDE slide= Integer
                            {
                            KW_SLIDE87=(Token)match(input,KW_SLIDE,FOLLOW_KW_SLIDE_in_window_type1219); if (state.failed) return retval; 
                            if ( state.backtracking==0 ) stream_KW_SLIDE.add(KW_SLIDE87);


                            slide=(Token)match(input,Integer,FOLLOW_Integer_in_window_type1223); if (state.failed) return retval; 
                            if ( state.backtracking==0 ) stream_Integer.add(slide);


                            }
                            break;

                    }


                    // AST REWRITE
                    // elements: KW_SLIDE, slide, row, KW_ROWS
                    // token labels: slide, row
                    // rule labels: retval
                    // token list labels: 
                    // rule list labels: 
                    // wildcard labels: 
                    if ( state.backtracking==0 ) {

                    retval.tree = root_0;
                    RewriteRuleTokenStream stream_slide=new RewriteRuleTokenStream(adaptor,"token slide",slide);
                    RewriteRuleTokenStream stream_row=new RewriteRuleTokenStream(adaptor,"token row",row);
                    RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

                    root_0 = (CommonTree)adaptor.nil();
                    // 188:9: -> ^( TOK_WINDOW KW_ROWS $row ( KW_SLIDE $slide)? )
                    {
                        // CongerCQL.g:188:12: ^( TOK_WINDOW KW_ROWS $row ( KW_SLIDE $slide)? )
                        {
                        CommonTree root_1 = (CommonTree)adaptor.nil();
                        root_1 = (CommonTree)adaptor.becomeRoot(
                        (CommonTree)adaptor.create(TOK_WINDOW, "TOK_WINDOW")
                        , root_1);

                        adaptor.addChild(root_1, 
                        stream_KW_ROWS.nextNode()
                        );

                        adaptor.addChild(root_1, stream_row.nextNode());

                        // CongerCQL.g:188:38: ( KW_SLIDE $slide)?
                        if ( stream_KW_SLIDE.hasNext()||stream_slide.hasNext() ) {
                            adaptor.addChild(root_1, 
                            stream_KW_SLIDE.nextNode()
                            );

                            adaptor.addChild(root_1, stream_slide.nextNode());

                        }
                        stream_KW_SLIDE.reset();
                        stream_slide.reset();

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
    // CongerCQL.g:191:1: non_mt_attr_list : attr ( options {greedy=true; } : COMMA non_mt_attr_list )* -> ^( TOK_ATTR_LIST ( attr )+ ) ;
    public final CongerCQLParser.non_mt_attr_list_return non_mt_attr_list() throws RecognitionException {
        CongerCQLParser.non_mt_attr_list_return retval = new CongerCQLParser.non_mt_attr_list_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token COMMA89=null;
        CongerCQLParser.attr_return attr88 =null;

        CongerCQLParser.non_mt_attr_list_return non_mt_attr_list90 =null;


        CommonTree COMMA89_tree=null;
        RewriteRuleTokenStream stream_COMMA=new RewriteRuleTokenStream(adaptor,"token COMMA");
        RewriteRuleSubtreeStream stream_non_mt_attr_list=new RewriteRuleSubtreeStream(adaptor,"rule non_mt_attr_list");
        RewriteRuleSubtreeStream stream_attr=new RewriteRuleSubtreeStream(adaptor,"rule attr");
        try {
            // CongerCQL.g:192:2: ( attr ( options {greedy=true; } : COMMA non_mt_attr_list )* -> ^( TOK_ATTR_LIST ( attr )+ ) )
            // CongerCQL.g:192:4: attr ( options {greedy=true; } : COMMA non_mt_attr_list )*
            {
            pushFollow(FOLLOW_attr_in_non_mt_attr_list1263);
            attr88=attr();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_attr.add(attr88.getTree());

            // CongerCQL.g:192:9: ( options {greedy=true; } : COMMA non_mt_attr_list )*
            loop25:
            do {
                int alt25=2;
                int LA25_0 = input.LA(1);

                if ( (LA25_0==COMMA) ) {
                    alt25=1;
                }


                switch (alt25) {
            	case 1 :
            	    // CongerCQL.g:192:33: COMMA non_mt_attr_list
            	    {
            	    COMMA89=(Token)match(input,COMMA,FOLLOW_COMMA_in_non_mt_attr_list1274); if (state.failed) return retval; 
            	    if ( state.backtracking==0 ) stream_COMMA.add(COMMA89);


            	    pushFollow(FOLLOW_non_mt_attr_list_in_non_mt_attr_list1276);
            	    non_mt_attr_list90=non_mt_attr_list();

            	    state._fsp--;
            	    if (state.failed) return retval;
            	    if ( state.backtracking==0 ) stream_non_mt_attr_list.add(non_mt_attr_list90.getTree());

            	    }
            	    break;

            	default :
            	    break loop25;
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
            // 193:9: -> ^( TOK_ATTR_LIST ( attr )+ )
            {
                // CongerCQL.g:193:12: ^( TOK_ATTR_LIST ( attr )+ )
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
    // CongerCQL.g:196:1: const_value : ( const_string | KW_NULL | const_int | const_float );
    public final CongerCQLParser.const_value_return const_value() throws RecognitionException {
        CongerCQLParser.const_value_return retval = new CongerCQLParser.const_value_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token KW_NULL92=null;
        CongerCQLParser.const_string_return const_string91 =null;

        CongerCQLParser.const_int_return const_int93 =null;

        CongerCQLParser.const_float_return const_float94 =null;


        CommonTree KW_NULL92_tree=null;

        try {
            // CongerCQL.g:197:2: ( const_string | KW_NULL | const_int | const_float )
            int alt26=4;
            switch ( input.LA(1) ) {
            case StringLiteral:
                {
                alt26=1;
                }
                break;
            case KW_NULL:
                {
                alt26=2;
                }
                break;
            case Integer:
                {
                alt26=3;
                }
                break;
            case Number:
                {
                alt26=4;
                }
                break;
            default:
                if (state.backtracking>0) {state.failed=true; return retval;}
                NoViableAltException nvae =
                    new NoViableAltException("", 26, 0, input);

                throw nvae;

            }

            switch (alt26) {
                case 1 :
                    // CongerCQL.g:197:4: const_string
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    pushFollow(FOLLOW_const_string_in_const_value1306);
                    const_string91=const_string();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, const_string91.getTree());

                    }
                    break;
                case 2 :
                    // CongerCQL.g:198:4: KW_NULL
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    KW_NULL92=(Token)match(input,KW_NULL,FOLLOW_KW_NULL_in_const_value1311); if (state.failed) return retval;
                    if ( state.backtracking==0 ) {
                    KW_NULL92_tree = 
                    (CommonTree)adaptor.create(KW_NULL92)
                    ;
                    adaptor.addChild(root_0, KW_NULL92_tree);
                    }

                    }
                    break;
                case 3 :
                    // CongerCQL.g:199:4: const_int
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    pushFollow(FOLLOW_const_int_in_const_value1316);
                    const_int93=const_int();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, const_int93.getTree());

                    }
                    break;
                case 4 :
                    // CongerCQL.g:200:4: const_float
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    pushFollow(FOLLOW_const_float_in_const_value1321);
                    const_float94=const_float();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, const_float94.getTree());

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


    public static class time_spec_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "time_spec"
    // CongerCQL.g:203:1: time_spec : Integer time_unit ;
    public final CongerCQLParser.time_spec_return time_spec() throws RecognitionException {
        CongerCQLParser.time_spec_return retval = new CongerCQLParser.time_spec_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token Integer95=null;
        CongerCQLParser.time_unit_return time_unit96 =null;


        CommonTree Integer95_tree=null;

        try {
            // CongerCQL.g:204:2: ( Integer time_unit )
            // CongerCQL.g:204:4: Integer time_unit
            {
            root_0 = (CommonTree)adaptor.nil();


            Integer95=(Token)match(input,Integer,FOLLOW_Integer_in_time_spec1332); if (state.failed) return retval;
            if ( state.backtracking==0 ) {
            Integer95_tree = 
            (CommonTree)adaptor.create(Integer95)
            ;
            adaptor.addChild(root_0, Integer95_tree);
            }

            pushFollow(FOLLOW_time_unit_in_time_spec1334);
            time_unit96=time_unit();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) adaptor.addChild(root_0, time_unit96.getTree());

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
    // CongerCQL.g:207:1: time_unit : ( KW_DAY | KW_DAYS | KW_HOUR | KW_HOURS | KW_MINUTE | KW_MINUTES | KW_SECOND | KW_SECONDS | KW_MILLISECOND | KW_MILLISECONDS | KW_MICROSECOND | KW_MICROSECONDS | KW_NANOSECOND | KW_NANOSECONDS );
    public final CongerCQLParser.time_unit_return time_unit() throws RecognitionException {
        CongerCQLParser.time_unit_return retval = new CongerCQLParser.time_unit_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token set97=null;

        CommonTree set97_tree=null;

        try {
            // CongerCQL.g:208:2: ( KW_DAY | KW_DAYS | KW_HOUR | KW_HOURS | KW_MINUTE | KW_MINUTES | KW_SECOND | KW_SECONDS | KW_MILLISECOND | KW_MILLISECONDS | KW_MICROSECOND | KW_MICROSECONDS | KW_NANOSECOND | KW_NANOSECONDS )
            // CongerCQL.g:
            {
            root_0 = (CommonTree)adaptor.nil();


            set97=(Token)input.LT(1);

            if ( (input.LA(1) >= KW_DAY && input.LA(1) <= KW_DAYS)||(input.LA(1) >= KW_HOUR && input.LA(1) <= KW_HOURS)||(input.LA(1) >= KW_MICROSECOND && input.LA(1) <= KW_MILLISECONDS)||(input.LA(1) >= KW_MINUTE && input.LA(1) <= KW_NANOSECONDS)||(input.LA(1) >= KW_SECOND && input.LA(1) <= KW_SECONDS) ) {
                input.consume();
                if ( state.backtracking==0 ) adaptor.addChild(root_0, 
                (CommonTree)adaptor.create(set97)
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


    public static class aggr_expr_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "aggr_expr"
    // CongerCQL.g:224:1: aggr_expr : ( KW_COUNT LPAREN ( arith_expr_main | STAR ) RPAREN -> ^( TOK_AGGR KW_COUNT ( arith_expr_main )? ( STAR )? ) | ( ( KW_SUM | KW_AVG ) LPAREN arith_expr_main | ( KW_MAX | KW_MIN ) LPAREN arith_expr_main ) RPAREN -> ^( TOK_AGGR ( KW_SUM )? ( KW_AVG )? ( KW_MAX )? ( KW_MIN )? ^( TOK_AGGR_EXPR arith_expr_main ) ) );
    public final CongerCQLParser.aggr_expr_return aggr_expr() throws RecognitionException {
        CongerCQLParser.aggr_expr_return retval = new CongerCQLParser.aggr_expr_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token KW_COUNT98=null;
        Token LPAREN99=null;
        Token STAR101=null;
        Token RPAREN102=null;
        Token KW_SUM103=null;
        Token KW_AVG104=null;
        Token LPAREN105=null;
        Token KW_MAX107=null;
        Token KW_MIN108=null;
        Token LPAREN109=null;
        Token RPAREN111=null;
        CongerCQLParser.arith_expr_main_return arith_expr_main100 =null;

        CongerCQLParser.arith_expr_main_return arith_expr_main106 =null;

        CongerCQLParser.arith_expr_main_return arith_expr_main110 =null;


        CommonTree KW_COUNT98_tree=null;
        CommonTree LPAREN99_tree=null;
        CommonTree STAR101_tree=null;
        CommonTree RPAREN102_tree=null;
        CommonTree KW_SUM103_tree=null;
        CommonTree KW_AVG104_tree=null;
        CommonTree LPAREN105_tree=null;
        CommonTree KW_MAX107_tree=null;
        CommonTree KW_MIN108_tree=null;
        CommonTree LPAREN109_tree=null;
        CommonTree RPAREN111_tree=null;
        RewriteRuleTokenStream stream_KW_COUNT=new RewriteRuleTokenStream(adaptor,"token KW_COUNT");
        RewriteRuleTokenStream stream_KW_MAX=new RewriteRuleTokenStream(adaptor,"token KW_MAX");
        RewriteRuleTokenStream stream_RPAREN=new RewriteRuleTokenStream(adaptor,"token RPAREN");
        RewriteRuleTokenStream stream_KW_MIN=new RewriteRuleTokenStream(adaptor,"token KW_MIN");
        RewriteRuleTokenStream stream_STAR=new RewriteRuleTokenStream(adaptor,"token STAR");
        RewriteRuleTokenStream stream_KW_SUM=new RewriteRuleTokenStream(adaptor,"token KW_SUM");
        RewriteRuleTokenStream stream_KW_AVG=new RewriteRuleTokenStream(adaptor,"token KW_AVG");
        RewriteRuleTokenStream stream_LPAREN=new RewriteRuleTokenStream(adaptor,"token LPAREN");
        RewriteRuleSubtreeStream stream_arith_expr_main=new RewriteRuleSubtreeStream(adaptor,"rule arith_expr_main");
        try {
            // CongerCQL.g:225:2: ( KW_COUNT LPAREN ( arith_expr_main | STAR ) RPAREN -> ^( TOK_AGGR KW_COUNT ( arith_expr_main )? ( STAR )? ) | ( ( KW_SUM | KW_AVG ) LPAREN arith_expr_main | ( KW_MAX | KW_MIN ) LPAREN arith_expr_main ) RPAREN -> ^( TOK_AGGR ( KW_SUM )? ( KW_AVG )? ( KW_MAX )? ( KW_MIN )? ^( TOK_AGGR_EXPR arith_expr_main ) ) )
            int alt31=2;
            int LA31_0 = input.LA(1);

            if ( (LA31_0==KW_COUNT) ) {
                alt31=1;
            }
            else if ( (LA31_0==KW_AVG||LA31_0==KW_MAX||LA31_0==KW_MIN||LA31_0==KW_SUM) ) {
                alt31=2;
            }
            else {
                if (state.backtracking>0) {state.failed=true; return retval;}
                NoViableAltException nvae =
                    new NoViableAltException("", 31, 0, input);

                throw nvae;

            }
            switch (alt31) {
                case 1 :
                    // CongerCQL.g:225:4: KW_COUNT LPAREN ( arith_expr_main | STAR ) RPAREN
                    {
                    KW_COUNT98=(Token)match(input,KW_COUNT,FOLLOW_KW_COUNT_in_aggr_expr1421); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_KW_COUNT.add(KW_COUNT98);


                    LPAREN99=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_aggr_expr1423); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_LPAREN.add(LPAREN99);


                    // CongerCQL.g:225:20: ( arith_expr_main | STAR )
                    int alt27=2;
                    int LA27_0 = input.LA(1);

                    if ( ((LA27_0 >= Identifier && LA27_0 <= Integer)||LA27_0==KW_AVG||LA27_0==KW_COUNT||LA27_0==KW_ELEMENT_TIME||LA27_0==KW_MAX||LA27_0==KW_MIN||LA27_0==KW_NULL||LA27_0==KW_SUM||LA27_0==LPAREN||LA27_0==Number||LA27_0==StringLiteral) ) {
                        alt27=1;
                    }
                    else if ( (LA27_0==STAR) ) {
                        alt27=2;
                    }
                    else {
                        if (state.backtracking>0) {state.failed=true; return retval;}
                        NoViableAltException nvae =
                            new NoViableAltException("", 27, 0, input);

                        throw nvae;

                    }
                    switch (alt27) {
                        case 1 :
                            // CongerCQL.g:225:22: arith_expr_main
                            {
                            pushFollow(FOLLOW_arith_expr_main_in_aggr_expr1427);
                            arith_expr_main100=arith_expr_main();

                            state._fsp--;
                            if (state.failed) return retval;
                            if ( state.backtracking==0 ) stream_arith_expr_main.add(arith_expr_main100.getTree());

                            }
                            break;
                        case 2 :
                            // CongerCQL.g:225:40: STAR
                            {
                            STAR101=(Token)match(input,STAR,FOLLOW_STAR_in_aggr_expr1431); if (state.failed) return retval; 
                            if ( state.backtracking==0 ) stream_STAR.add(STAR101);


                            }
                            break;

                    }


                    RPAREN102=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_aggr_expr1434); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_RPAREN.add(RPAREN102);


                    // AST REWRITE
                    // elements: arith_expr_main, STAR, KW_COUNT
                    // token labels: 
                    // rule labels: retval
                    // token list labels: 
                    // rule list labels: 
                    // wildcard labels: 
                    if ( state.backtracking==0 ) {

                    retval.tree = root_0;
                    RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

                    root_0 = (CommonTree)adaptor.nil();
                    // 226:9: -> ^( TOK_AGGR KW_COUNT ( arith_expr_main )? ( STAR )? )
                    {
                        // CongerCQL.g:226:12: ^( TOK_AGGR KW_COUNT ( arith_expr_main )? ( STAR )? )
                        {
                        CommonTree root_1 = (CommonTree)adaptor.nil();
                        root_1 = (CommonTree)adaptor.becomeRoot(
                        (CommonTree)adaptor.create(TOK_AGGR, "TOK_AGGR")
                        , root_1);

                        adaptor.addChild(root_1, 
                        stream_KW_COUNT.nextNode()
                        );

                        // CongerCQL.g:226:32: ( arith_expr_main )?
                        if ( stream_arith_expr_main.hasNext() ) {
                            adaptor.addChild(root_1, stream_arith_expr_main.nextTree());

                        }
                        stream_arith_expr_main.reset();

                        // CongerCQL.g:226:49: ( STAR )?
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
                    // CongerCQL.g:227:4: ( ( KW_SUM | KW_AVG ) LPAREN arith_expr_main | ( KW_MAX | KW_MIN ) LPAREN arith_expr_main ) RPAREN
                    {
                    // CongerCQL.g:227:4: ( ( KW_SUM | KW_AVG ) LPAREN arith_expr_main | ( KW_MAX | KW_MIN ) LPAREN arith_expr_main )
                    int alt30=2;
                    int LA30_0 = input.LA(1);

                    if ( (LA30_0==KW_AVG||LA30_0==KW_SUM) ) {
                        alt30=1;
                    }
                    else if ( (LA30_0==KW_MAX||LA30_0==KW_MIN) ) {
                        alt30=2;
                    }
                    else {
                        if (state.backtracking>0) {state.failed=true; return retval;}
                        NoViableAltException nvae =
                            new NoViableAltException("", 30, 0, input);

                        throw nvae;

                    }
                    switch (alt30) {
                        case 1 :
                            // CongerCQL.g:227:6: ( KW_SUM | KW_AVG ) LPAREN arith_expr_main
                            {
                            // CongerCQL.g:227:6: ( KW_SUM | KW_AVG )
                            int alt28=2;
                            int LA28_0 = input.LA(1);

                            if ( (LA28_0==KW_SUM) ) {
                                alt28=1;
                            }
                            else if ( (LA28_0==KW_AVG) ) {
                                alt28=2;
                            }
                            else {
                                if (state.backtracking>0) {state.failed=true; return retval;}
                                NoViableAltException nvae =
                                    new NoViableAltException("", 28, 0, input);

                                throw nvae;

                            }
                            switch (alt28) {
                                case 1 :
                                    // CongerCQL.g:227:7: KW_SUM
                                    {
                                    KW_SUM103=(Token)match(input,KW_SUM,FOLLOW_KW_SUM_in_aggr_expr1464); if (state.failed) return retval; 
                                    if ( state.backtracking==0 ) stream_KW_SUM.add(KW_SUM103);


                                    }
                                    break;
                                case 2 :
                                    // CongerCQL.g:227:16: KW_AVG
                                    {
                                    KW_AVG104=(Token)match(input,KW_AVG,FOLLOW_KW_AVG_in_aggr_expr1468); if (state.failed) return retval; 
                                    if ( state.backtracking==0 ) stream_KW_AVG.add(KW_AVG104);


                                    }
                                    break;

                            }


                            LPAREN105=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_aggr_expr1471); if (state.failed) return retval; 
                            if ( state.backtracking==0 ) stream_LPAREN.add(LPAREN105);


                            pushFollow(FOLLOW_arith_expr_main_in_aggr_expr1473);
                            arith_expr_main106=arith_expr_main();

                            state._fsp--;
                            if (state.failed) return retval;
                            if ( state.backtracking==0 ) stream_arith_expr_main.add(arith_expr_main106.getTree());

                            }
                            break;
                        case 2 :
                            // CongerCQL.g:228:5: ( KW_MAX | KW_MIN ) LPAREN arith_expr_main
                            {
                            // CongerCQL.g:228:5: ( KW_MAX | KW_MIN )
                            int alt29=2;
                            int LA29_0 = input.LA(1);

                            if ( (LA29_0==KW_MAX) ) {
                                alt29=1;
                            }
                            else if ( (LA29_0==KW_MIN) ) {
                                alt29=2;
                            }
                            else {
                                if (state.backtracking>0) {state.failed=true; return retval;}
                                NoViableAltException nvae =
                                    new NoViableAltException("", 29, 0, input);

                                throw nvae;

                            }
                            switch (alt29) {
                                case 1 :
                                    // CongerCQL.g:228:7: KW_MAX
                                    {
                                    KW_MAX107=(Token)match(input,KW_MAX,FOLLOW_KW_MAX_in_aggr_expr1481); if (state.failed) return retval; 
                                    if ( state.backtracking==0 ) stream_KW_MAX.add(KW_MAX107);


                                    }
                                    break;
                                case 2 :
                                    // CongerCQL.g:228:16: KW_MIN
                                    {
                                    KW_MIN108=(Token)match(input,KW_MIN,FOLLOW_KW_MIN_in_aggr_expr1485); if (state.failed) return retval; 
                                    if ( state.backtracking==0 ) stream_KW_MIN.add(KW_MIN108);


                                    }
                                    break;

                            }


                            LPAREN109=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_aggr_expr1488); if (state.failed) return retval; 
                            if ( state.backtracking==0 ) stream_LPAREN.add(LPAREN109);


                            pushFollow(FOLLOW_arith_expr_main_in_aggr_expr1490);
                            arith_expr_main110=arith_expr_main();

                            state._fsp--;
                            if (state.failed) return retval;
                            if ( state.backtracking==0 ) stream_arith_expr_main.add(arith_expr_main110.getTree());

                            }
                            break;

                    }


                    RPAREN111=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_aggr_expr1495); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_RPAREN.add(RPAREN111);


                    // AST REWRITE
                    // elements: KW_AVG, KW_MAX, arith_expr_main, KW_MIN, KW_SUM
                    // token labels: 
                    // rule labels: retval
                    // token list labels: 
                    // rule list labels: 
                    // wildcard labels: 
                    if ( state.backtracking==0 ) {

                    retval.tree = root_0;
                    RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

                    root_0 = (CommonTree)adaptor.nil();
                    // 230:9: -> ^( TOK_AGGR ( KW_SUM )? ( KW_AVG )? ( KW_MAX )? ( KW_MIN )? ^( TOK_AGGR_EXPR arith_expr_main ) )
                    {
                        // CongerCQL.g:230:12: ^( TOK_AGGR ( KW_SUM )? ( KW_AVG )? ( KW_MAX )? ( KW_MIN )? ^( TOK_AGGR_EXPR arith_expr_main ) )
                        {
                        CommonTree root_1 = (CommonTree)adaptor.nil();
                        root_1 = (CommonTree)adaptor.becomeRoot(
                        (CommonTree)adaptor.create(TOK_AGGR, "TOK_AGGR")
                        , root_1);

                        // CongerCQL.g:230:23: ( KW_SUM )?
                        if ( stream_KW_SUM.hasNext() ) {
                            adaptor.addChild(root_1, 
                            stream_KW_SUM.nextNode()
                            );

                        }
                        stream_KW_SUM.reset();

                        // CongerCQL.g:230:31: ( KW_AVG )?
                        if ( stream_KW_AVG.hasNext() ) {
                            adaptor.addChild(root_1, 
                            stream_KW_AVG.nextNode()
                            );

                        }
                        stream_KW_AVG.reset();

                        // CongerCQL.g:230:39: ( KW_MAX )?
                        if ( stream_KW_MAX.hasNext() ) {
                            adaptor.addChild(root_1, 
                            stream_KW_MAX.nextNode()
                            );

                        }
                        stream_KW_MAX.reset();

                        // CongerCQL.g:230:47: ( KW_MIN )?
                        if ( stream_KW_MIN.hasNext() ) {
                            adaptor.addChild(root_1, 
                            stream_KW_MIN.nextNode()
                            );

                        }
                        stream_KW_MIN.reset();

                        // CongerCQL.g:230:55: ^( TOK_AGGR_EXPR arith_expr_main )
                        {
                        CommonTree root_2 = (CommonTree)adaptor.nil();
                        root_2 = (CommonTree)adaptor.becomeRoot(
                        (CommonTree)adaptor.create(TOK_AGGR_EXPR, "TOK_AGGR_EXPR")
                        , root_2);

                        adaptor.addChild(root_2, stream_arith_expr_main.nextTree());

                        adaptor.addChild(root_1, root_2);
                        }

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
    // CongerCQL.g:233:1: func_expr : func_name LPAREN ( non_mt_arg_list | KW_DISTINCT arith_expr ) RPAREN -> ^( TOK_FUNC func_name ( non_mt_arg_list )? ( KW_DISTINCT )? ( arith_expr )? ) ;
    public final CongerCQLParser.func_expr_return func_expr() throws RecognitionException {
        CongerCQLParser.func_expr_return retval = new CongerCQLParser.func_expr_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token LPAREN113=null;
        Token KW_DISTINCT115=null;
        Token RPAREN117=null;
        CongerCQLParser.func_name_return func_name112 =null;

        CongerCQLParser.non_mt_arg_list_return non_mt_arg_list114 =null;

        CongerCQLParser.arith_expr_return arith_expr116 =null;


        CommonTree LPAREN113_tree=null;
        CommonTree KW_DISTINCT115_tree=null;
        CommonTree RPAREN117_tree=null;
        RewriteRuleTokenStream stream_RPAREN=new RewriteRuleTokenStream(adaptor,"token RPAREN");
        RewriteRuleTokenStream stream_KW_DISTINCT=new RewriteRuleTokenStream(adaptor,"token KW_DISTINCT");
        RewriteRuleTokenStream stream_LPAREN=new RewriteRuleTokenStream(adaptor,"token LPAREN");
        RewriteRuleSubtreeStream stream_arith_expr=new RewriteRuleSubtreeStream(adaptor,"rule arith_expr");
        RewriteRuleSubtreeStream stream_func_name=new RewriteRuleSubtreeStream(adaptor,"rule func_name");
        RewriteRuleSubtreeStream stream_non_mt_arg_list=new RewriteRuleSubtreeStream(adaptor,"rule non_mt_arg_list");
        try {
            // CongerCQL.g:234:2: ( func_name LPAREN ( non_mt_arg_list | KW_DISTINCT arith_expr ) RPAREN -> ^( TOK_FUNC func_name ( non_mt_arg_list )? ( KW_DISTINCT )? ( arith_expr )? ) )
            // CongerCQL.g:234:4: func_name LPAREN ( non_mt_arg_list | KW_DISTINCT arith_expr ) RPAREN
            {
            pushFollow(FOLLOW_func_name_in_func_expr1538);
            func_name112=func_name();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) stream_func_name.add(func_name112.getTree());

            LPAREN113=(Token)match(input,LPAREN,FOLLOW_LPAREN_in_func_expr1540); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_LPAREN.add(LPAREN113);


            // CongerCQL.g:234:21: ( non_mt_arg_list | KW_DISTINCT arith_expr )
            int alt32=2;
            int LA32_0 = input.LA(1);

            if ( ((LA32_0 >= Identifier && LA32_0 <= Integer)||LA32_0==KW_AVG||LA32_0==KW_COUNT||LA32_0==KW_ELEMENT_TIME||LA32_0==KW_MAX||LA32_0==KW_MIN||LA32_0==KW_NULL||LA32_0==KW_SUM||LA32_0==LPAREN||LA32_0==Number||LA32_0==StringLiteral) ) {
                alt32=1;
            }
            else if ( (LA32_0==KW_DISTINCT) ) {
                alt32=2;
            }
            else {
                if (state.backtracking>0) {state.failed=true; return retval;}
                NoViableAltException nvae =
                    new NoViableAltException("", 32, 0, input);

                throw nvae;

            }
            switch (alt32) {
                case 1 :
                    // CongerCQL.g:234:23: non_mt_arg_list
                    {
                    pushFollow(FOLLOW_non_mt_arg_list_in_func_expr1544);
                    non_mt_arg_list114=non_mt_arg_list();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_non_mt_arg_list.add(non_mt_arg_list114.getTree());

                    }
                    break;
                case 2 :
                    // CongerCQL.g:234:41: KW_DISTINCT arith_expr
                    {
                    KW_DISTINCT115=(Token)match(input,KW_DISTINCT,FOLLOW_KW_DISTINCT_in_func_expr1548); if (state.failed) return retval; 
                    if ( state.backtracking==0 ) stream_KW_DISTINCT.add(KW_DISTINCT115);


                    pushFollow(FOLLOW_arith_expr_in_func_expr1550);
                    arith_expr116=arith_expr();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) stream_arith_expr.add(arith_expr116.getTree());

                    }
                    break;

            }


            RPAREN117=(Token)match(input,RPAREN,FOLLOW_RPAREN_in_func_expr1553); if (state.failed) return retval; 
            if ( state.backtracking==0 ) stream_RPAREN.add(RPAREN117);


            // AST REWRITE
            // elements: func_name, KW_DISTINCT, arith_expr, non_mt_arg_list
            // token labels: 
            // rule labels: retval
            // token list labels: 
            // rule list labels: 
            // wildcard labels: 
            if ( state.backtracking==0 ) {

            retval.tree = root_0;
            RewriteRuleSubtreeStream stream_retval=new RewriteRuleSubtreeStream(adaptor,"rule retval",retval!=null?retval.tree:null);

            root_0 = (CommonTree)adaptor.nil();
            // 235:9: -> ^( TOK_FUNC func_name ( non_mt_arg_list )? ( KW_DISTINCT )? ( arith_expr )? )
            {
                // CongerCQL.g:235:12: ^( TOK_FUNC func_name ( non_mt_arg_list )? ( KW_DISTINCT )? ( arith_expr )? )
                {
                CommonTree root_1 = (CommonTree)adaptor.nil();
                root_1 = (CommonTree)adaptor.becomeRoot(
                (CommonTree)adaptor.create(TOK_FUNC, "TOK_FUNC")
                , root_1);

                adaptor.addChild(root_1, stream_func_name.nextTree());

                // CongerCQL.g:235:33: ( non_mt_arg_list )?
                if ( stream_non_mt_arg_list.hasNext() ) {
                    adaptor.addChild(root_1, stream_non_mt_arg_list.nextTree());

                }
                stream_non_mt_arg_list.reset();

                // CongerCQL.g:235:50: ( KW_DISTINCT )?
                if ( stream_KW_DISTINCT.hasNext() ) {
                    adaptor.addChild(root_1, 
                    stream_KW_DISTINCT.nextNode()
                    );

                }
                stream_KW_DISTINCT.reset();

                // CongerCQL.g:235:63: ( arith_expr )?
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
    // CongerCQL.g:238:1: func_name : Identifier ;
    public final CongerCQLParser.func_name_return func_name() throws RecognitionException {
        CongerCQLParser.func_name_return retval = new CongerCQLParser.func_name_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token Identifier118=null;

        CommonTree Identifier118_tree=null;

        try {
            // CongerCQL.g:239:2: ( Identifier )
            // CongerCQL.g:239:4: Identifier
            {
            root_0 = (CommonTree)adaptor.nil();


            Identifier118=(Token)match(input,Identifier,FOLLOW_Identifier_in_func_name1589); if (state.failed) return retval;
            if ( state.backtracking==0 ) {
            Identifier118_tree = 
            (CommonTree)adaptor.create(Identifier118)
            ;
            adaptor.addChild(root_0, Identifier118_tree);
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


    public static class attr_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "attr"
    // CongerCQL.g:242:1: attr : ( Identifier ( DOT Identifier | DOT pseudo_column )? | pseudo_column );
    public final CongerCQLParser.attr_return attr() throws RecognitionException {
        CongerCQLParser.attr_return retval = new CongerCQLParser.attr_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token Identifier119=null;
        Token DOT120=null;
        Token Identifier121=null;
        Token DOT122=null;
        CongerCQLParser.pseudo_column_return pseudo_column123 =null;

        CongerCQLParser.pseudo_column_return pseudo_column124 =null;


        CommonTree Identifier119_tree=null;
        CommonTree DOT120_tree=null;
        CommonTree Identifier121_tree=null;
        CommonTree DOT122_tree=null;

        try {
            // CongerCQL.g:243:2: ( Identifier ( DOT Identifier | DOT pseudo_column )? | pseudo_column )
            int alt34=2;
            int LA34_0 = input.LA(1);

            if ( (LA34_0==Identifier) ) {
                alt34=1;
            }
            else if ( (LA34_0==KW_ELEMENT_TIME) ) {
                alt34=2;
            }
            else {
                if (state.backtracking>0) {state.failed=true; return retval;}
                NoViableAltException nvae =
                    new NoViableAltException("", 34, 0, input);

                throw nvae;

            }
            switch (alt34) {
                case 1 :
                    // CongerCQL.g:243:4: Identifier ( DOT Identifier | DOT pseudo_column )?
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    Identifier119=(Token)match(input,Identifier,FOLLOW_Identifier_in_attr1600); if (state.failed) return retval;
                    if ( state.backtracking==0 ) {
                    Identifier119_tree = 
                    (CommonTree)adaptor.create(Identifier119)
                    ;
                    adaptor.addChild(root_0, Identifier119_tree);
                    }

                    // CongerCQL.g:243:15: ( DOT Identifier | DOT pseudo_column )?
                    int alt33=3;
                    int LA33_0 = input.LA(1);

                    if ( (LA33_0==DOT) ) {
                        int LA33_1 = input.LA(2);

                        if ( (LA33_1==Identifier) ) {
                            alt33=1;
                        }
                        else if ( (LA33_1==KW_ELEMENT_TIME) ) {
                            alt33=2;
                        }
                    }
                    switch (alt33) {
                        case 1 :
                            // CongerCQL.g:243:17: DOT Identifier
                            {
                            DOT120=(Token)match(input,DOT,FOLLOW_DOT_in_attr1604); if (state.failed) return retval;
                            if ( state.backtracking==0 ) {
                            DOT120_tree = 
                            (CommonTree)adaptor.create(DOT120)
                            ;
                            adaptor.addChild(root_0, DOT120_tree);
                            }

                            Identifier121=(Token)match(input,Identifier,FOLLOW_Identifier_in_attr1606); if (state.failed) return retval;
                            if ( state.backtracking==0 ) {
                            Identifier121_tree = 
                            (CommonTree)adaptor.create(Identifier121)
                            ;
                            adaptor.addChild(root_0, Identifier121_tree);
                            }

                            }
                            break;
                        case 2 :
                            // CongerCQL.g:243:34: DOT pseudo_column
                            {
                            DOT122=(Token)match(input,DOT,FOLLOW_DOT_in_attr1610); if (state.failed) return retval;
                            if ( state.backtracking==0 ) {
                            DOT122_tree = 
                            (CommonTree)adaptor.create(DOT122)
                            ;
                            adaptor.addChild(root_0, DOT122_tree);
                            }

                            pushFollow(FOLLOW_pseudo_column_in_attr1612);
                            pseudo_column123=pseudo_column();

                            state._fsp--;
                            if (state.failed) return retval;
                            if ( state.backtracking==0 ) adaptor.addChild(root_0, pseudo_column123.getTree());

                            }
                            break;

                    }


                    }
                    break;
                case 2 :
                    // CongerCQL.g:244:4: pseudo_column
                    {
                    root_0 = (CommonTree)adaptor.nil();


                    pushFollow(FOLLOW_pseudo_column_in_attr1619);
                    pseudo_column124=pseudo_column();

                    state._fsp--;
                    if (state.failed) return retval;
                    if ( state.backtracking==0 ) adaptor.addChild(root_0, pseudo_column124.getTree());

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
    // CongerCQL.g:247:1: pseudo_column : element_time ;
    public final CongerCQLParser.pseudo_column_return pseudo_column() throws RecognitionException {
        CongerCQLParser.pseudo_column_return retval = new CongerCQLParser.pseudo_column_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        CongerCQLParser.element_time_return element_time125 =null;



        try {
            // CongerCQL.g:248:2: ( element_time )
            // CongerCQL.g:248:4: element_time
            {
            root_0 = (CommonTree)adaptor.nil();


            pushFollow(FOLLOW_element_time_in_pseudo_column1630);
            element_time125=element_time();

            state._fsp--;
            if (state.failed) return retval;
            if ( state.backtracking==0 ) adaptor.addChild(root_0, element_time125.getTree());

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
    // CongerCQL.g:251:1: element_time : KW_ELEMENT_TIME ;
    public final CongerCQLParser.element_time_return element_time() throws RecognitionException {
        CongerCQLParser.element_time_return retval = new CongerCQLParser.element_time_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token KW_ELEMENT_TIME126=null;

        CommonTree KW_ELEMENT_TIME126_tree=null;

        try {
            // CongerCQL.g:252:2: ( KW_ELEMENT_TIME )
            // CongerCQL.g:252:4: KW_ELEMENT_TIME
            {
            root_0 = (CommonTree)adaptor.nil();


            KW_ELEMENT_TIME126=(Token)match(input,KW_ELEMENT_TIME,FOLLOW_KW_ELEMENT_TIME_in_element_time1641); if (state.failed) return retval;
            if ( state.backtracking==0 ) {
            KW_ELEMENT_TIME126_tree = 
            (CommonTree)adaptor.create(KW_ELEMENT_TIME126)
            ;
            adaptor.addChild(root_0, KW_ELEMENT_TIME126_tree);
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
    // CongerCQL.g:255:1: const_int : Integer ;
    public final CongerCQLParser.const_int_return const_int() throws RecognitionException {
        CongerCQLParser.const_int_return retval = new CongerCQLParser.const_int_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token Integer127=null;

        CommonTree Integer127_tree=null;

        try {
            // CongerCQL.g:256:2: ( Integer )
            // CongerCQL.g:256:4: Integer
            {
            root_0 = (CommonTree)adaptor.nil();


            Integer127=(Token)match(input,Integer,FOLLOW_Integer_in_const_int1652); if (state.failed) return retval;
            if ( state.backtracking==0 ) {
            Integer127_tree = 
            (CommonTree)adaptor.create(Integer127)
            ;
            adaptor.addChild(root_0, Integer127_tree);
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


    public static class const_float_return extends ParserRuleReturnScope {
        CommonTree tree;
        public Object getTree() { return tree; }
    };


    // $ANTLR start "const_float"
    // CongerCQL.g:259:1: const_float : Number ;
    public final CongerCQLParser.const_float_return const_float() throws RecognitionException {
        CongerCQLParser.const_float_return retval = new CongerCQLParser.const_float_return();
        retval.start = input.LT(1);


        CommonTree root_0 = null;

        Token Number128=null;

        CommonTree Number128_tree=null;

        try {
            // CongerCQL.g:260:2: ( Number )
            // CongerCQL.g:260:4: Number
            {
            root_0 = (CommonTree)adaptor.nil();


            Number128=(Token)match(input,Number,FOLLOW_Number_in_const_float1664); if (state.failed) return retval;
            if ( state.backtracking==0 ) {
            Number128_tree = 
            (CommonTree)adaptor.create(Number128)
            ;
            adaptor.addChild(root_0, Number128_tree);
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
    // $ANTLR end "const_float"

    // $ANTLR start synpred2_CongerCQL
    public final void synpred2_CongerCQL_fragment() throws RecognitionException {
        // CongerCQL.g:144:4: ( unary_condition )
        // CongerCQL.g:144:4: unary_condition
        {
        pushFollow(FOLLOW_unary_condition_in_synpred2_CongerCQL900);
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


 

    public static final BitSet FOLLOW_query_in_statement218 = new BitSet(new long[]{0x0000000000000000L});
    public static final BitSet FOLLOW_EOF_in_statement220 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_sfw_block_in_query231 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_binary_in_query236 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_Identifier_in_binary249 = new BitSet(new long[]{0x0000000000000000L,0x0200000000000000L});
    public static final BitSet FOLLOW_binary_operator_in_binary251 = new BitSet(new long[]{0x0000000000800000L});
    public static final BitSet FOLLOW_Identifier_in_binary255 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_UNION_in_binary_operator285 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_select_clause_in_sfw_block297 = new BitSet(new long[]{0x2000000000000000L});
    public static final BitSet FOLLOW_from_clause_in_sfw_block299 = new BitSet(new long[]{0x8000000000000002L,0x8000000000000001L});
    public static final BitSet FOLLOW_opt_group_by_clause_in_sfw_block304 = new BitSet(new long[]{0x0000000000000002L,0x0000000000000001L});
    public static final BitSet FOLLOW_opt_having_clause_in_sfw_block307 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_opt_where_clause_in_sfw_block316 = new BitSet(new long[]{0x8000000000000002L,0x0000000000000001L});
    public static final BitSet FOLLOW_opt_group_by_clause_in_sfw_block318 = new BitSet(new long[]{0x0000000000000002L,0x0000000000000001L});
    public static final BitSet FOLLOW_opt_having_clause_in_sfw_block321 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_SELECT_in_select_clause374 = new BitSet(new long[]{0x0080010041800000L,0x0010000040420000L,0x0000000000180820L});
    public static final BitSet FOLLOW_STAR_in_select_clause377 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_non_mt_projterm_list_in_select_clause381 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_projterm_in_non_mt_projterm_list413 = new BitSet(new long[]{0x0000000000000102L});
    public static final BitSet FOLLOW_COMMA_in_non_mt_projterm_list424 = new BitSet(new long[]{0x0080010041800000L,0x0010000040420000L,0x0000000000100820L});
    public static final BitSet FOLLOW_projterm_in_non_mt_projterm_list426 = new BitSet(new long[]{0x0000000000000102L});
    public static final BitSet FOLLOW_arith_expr_in_projterm457 = new BitSet(new long[]{0x0000000010000002L});
    public static final BitSet FOLLOW_KW_AS_in_projterm460 = new BitSet(new long[]{0x0000000000800000L});
    public static final BitSet FOLLOW_Identifier_in_projterm464 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_arith_expr_main_in_arith_expr501 = new BitSet(new long[]{0x0000000000004402L,0x0000000000000000L,0x0000000000081100L});
    public static final BitSet FOLLOW_arith_expr_operator_in_arith_expr504 = new BitSet(new long[]{0x0080010041800000L,0x0010000040420000L,0x0000000000100820L});
    public static final BitSet FOLLOW_arith_expr_main_in_arith_expr506 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_attr_in_arith_expr_main583 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_const_value_in_arith_expr_main588 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_aggr_expr_in_arith_expr_main593 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_func_expr_in_arith_expr_main598 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_LPAREN_in_arith_expr_main610 = new BitSet(new long[]{0x0080010041800000L,0x0010000040420000L,0x0000000000100820L});
    public static final BitSet FOLLOW_arith_expr_in_arith_expr_main612 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000008000L});
    public static final BitSet FOLLOW_RPAREN_in_arith_expr_main614 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_WHERE_in_opt_where_clause634 = new BitSet(new long[]{0x0080010041800000L,0x0010000050420000L,0x0000000000100820L});
    public static final BitSet FOLLOW_non_mt_cond_list_in_opt_where_clause636 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_HAVING_in_opt_having_clause663 = new BitSet(new long[]{0x0080010041800000L,0x0010000050420000L,0x0000000000100820L});
    public static final BitSet FOLLOW_non_mt_cond_list_in_opt_having_clause665 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_GROUP_in_opt_group_by_clause692 = new BitSet(new long[]{0x0000000000000000L,0x0000004000000000L});
    public static final BitSet FOLLOW_KW_OVER_in_opt_group_by_clause694 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000040L});
    public static final BitSet FOLLOW_LSQUARE_in_opt_group_by_clause696 = new BitSet(new long[]{0x0000000000000000L,0x0000240000000000L});
    public static final BitSet FOLLOW_window_type_in_opt_group_by_clause698 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000010000L});
    public static final BitSet FOLLOW_RSQUARE_in_opt_group_by_clause700 = new BitSet(new long[]{0x0000002000000000L});
    public static final BitSet FOLLOW_KW_BY_in_opt_group_by_clause702 = new BitSet(new long[]{0x0080000000800000L});
    public static final BitSet FOLLOW_non_mt_attr_list_in_opt_group_by_clause704 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_FROM_in_from_clause734 = new BitSet(new long[]{0x0000000000800000L});
    public static final BitSet FOLLOW_non_mt_relation_list_in_from_clause736 = new BitSet(new long[]{0x0000000000000002L,0x0000000000000800L});
    public static final BitSet FOLLOW_KW_JOIN_in_from_clause741 = new BitSet(new long[]{0x0000000000800000L});
    public static final BitSet FOLLOW_relation_variable_in_from_clause743 = new BitSet(new long[]{0x0000000000000000L,0x0000000200000000L});
    public static final BitSet FOLLOW_KW_ON_in_from_clause745 = new BitSet(new long[]{0x0080010041800000L,0x0010000050420000L,0x0000000000100820L});
    public static final BitSet FOLLOW_non_mt_cond_list_in_from_clause747 = new BitSet(new long[]{0x0000000000000002L,0x0020000000000000L});
    public static final BitSet FOLLOW_KW_TIMEOUT_in_from_clause750 = new BitSet(new long[]{0x0000000001000000L});
    public static final BitSet FOLLOW_Integer_in_from_clause754 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_non_mt_cond_list_main_in_non_mt_cond_list818 = new BitSet(new long[]{0x0000000008000002L,0x0000000400000000L,0x0000000000000002L});
    public static final BitSet FOLLOW_cond_list_operator_in_non_mt_cond_list821 = new BitSet(new long[]{0x0080010041800000L,0x0010000050420000L,0x0000000000100820L});
    public static final BitSet FOLLOW_non_mt_cond_list_main_in_non_mt_cond_list823 = new BitSet(new long[]{0x0000000008000002L,0x0000000400000000L,0x0000000000000002L});
    public static final BitSet FOLLOW_KW_NOT_in_non_mt_cond_list_main893 = new BitSet(new long[]{0x0080010041800000L,0x0010000050420000L,0x0000000000100820L});
    public static final BitSet FOLLOW_non_mt_cond_list_main_in_non_mt_cond_list_main895 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_unary_condition_in_non_mt_cond_list_main900 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_LPAREN_in_non_mt_cond_list_main905 = new BitSet(new long[]{0x0080010041800000L,0x0010000040420000L,0x0000000000100820L});
    public static final BitSet FOLLOW_unary_condition_in_non_mt_cond_list_main907 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000008000L});
    public static final BitSet FOLLOW_RPAREN_in_non_mt_cond_list_main909 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_arith_expr_in_unary_condition921 = new BitSet(new long[]{0x0000000100340000L,0x0000000000000000L,0x0000000000000418L});
    public static final BitSet FOLLOW_between_condition_right_in_unary_condition924 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_compare_condition_right_in_unary_condition928 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_BETWEEN_in_between_condition_right941 = new BitSet(new long[]{0x0080010041800000L,0x0010000040420000L,0x0000000000100820L});
    public static final BitSet FOLLOW_arith_expr_in_between_condition_right943 = new BitSet(new long[]{0x0000000008000000L});
    public static final BitSet FOLLOW_KW_AND_in_between_condition_right945 = new BitSet(new long[]{0x0080010041800000L,0x0010000040420000L,0x0000000000100820L});
    public static final BitSet FOLLOW_arith_expr_in_between_condition_right947 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_unary_condition_operator_in_compare_condition_right958 = new BitSet(new long[]{0x0080010041800000L,0x0010000040420000L,0x0000000000100820L});
    public static final BitSet FOLLOW_arith_expr_in_compare_condition_right960 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_StringLiteral_in_const_string1009 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_arith_expr_in_non_mt_arg_list1020 = new BitSet(new long[]{0x0000000000000102L});
    public static final BitSet FOLLOW_COMMA_in_non_mt_arg_list1031 = new BitSet(new long[]{0x0080010041800000L,0x0010000040420000L,0x0000000000100820L});
    public static final BitSet FOLLOW_non_mt_arg_list_in_non_mt_arg_list1033 = new BitSet(new long[]{0x0000000000000102L});
    public static final BitSet FOLLOW_relation_variable_in_non_mt_relation_list1058 = new BitSet(new long[]{0x0000000000000102L});
    public static final BitSet FOLLOW_COMMA_in_non_mt_relation_list1069 = new BitSet(new long[]{0x0000000000800000L});
    public static final BitSet FOLLOW_non_mt_relation_list_in_non_mt_relation_list1071 = new BitSet(new long[]{0x0000000000000102L});
    public static final BitSet FOLLOW_Identifier_in_relation_variable1103 = new BitSet(new long[]{0x0000000010000002L,0x0000000000000000L,0x0000000000000040L});
    public static final BitSet FOLLOW_LSQUARE_in_relation_variable1106 = new BitSet(new long[]{0x0000000000000000L,0x0000240000000000L});
    public static final BitSet FOLLOW_window_type_in_relation_variable1108 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000010000L});
    public static final BitSet FOLLOW_RSQUARE_in_relation_variable1110 = new BitSet(new long[]{0x0000000010000002L});
    public static final BitSet FOLLOW_KW_AS_in_relation_variable1116 = new BitSet(new long[]{0x0000000000800000L});
    public static final BitSet FOLLOW_Identifier_in_relation_variable1120 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_RANGE_in_window_type1161 = new BitSet(new long[]{0x0000000001000000L});
    public static final BitSet FOLLOW_time_spec_in_window_type1165 = new BitSet(new long[]{0x0000000000000002L,0x0008000200000000L});
    public static final BitSet FOLLOW_KW_SLIDE_in_window_type1169 = new BitSet(new long[]{0x0000000001000000L});
    public static final BitSet FOLLOW_time_spec_in_window_type1173 = new BitSet(new long[]{0x0000000000000002L,0x0000000200000000L});
    public static final BitSet FOLLOW_KW_ON_in_window_type1178 = new BitSet(new long[]{0x0080010041800000L,0x0010000040420000L,0x0000000000100820L});
    public static final BitSet FOLLOW_arith_expr_in_window_type1180 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_ROWS_in_window_type1211 = new BitSet(new long[]{0x0000000001000000L});
    public static final BitSet FOLLOW_Integer_in_window_type1215 = new BitSet(new long[]{0x0000000000000002L,0x0008000000000000L});
    public static final BitSet FOLLOW_KW_SLIDE_in_window_type1219 = new BitSet(new long[]{0x0000000001000000L});
    public static final BitSet FOLLOW_Integer_in_window_type1223 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_attr_in_non_mt_attr_list1263 = new BitSet(new long[]{0x0000000000000102L});
    public static final BitSet FOLLOW_COMMA_in_non_mt_attr_list1274 = new BitSet(new long[]{0x0080000000800000L});
    public static final BitSet FOLLOW_non_mt_attr_list_in_non_mt_attr_list1276 = new BitSet(new long[]{0x0000000000000102L});
    public static final BitSet FOLLOW_const_string_in_const_value1306 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_NULL_in_const_value1311 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_const_int_in_const_value1316 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_const_float_in_const_value1321 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_Integer_in_time_spec1332 = new BitSet(new long[]{0x0000600000000000L,0x000180000F3C0006L});
    public static final BitSet FOLLOW_time_unit_in_time_spec1334 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_COUNT_in_aggr_expr1421 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000020L});
    public static final BitSet FOLLOW_LPAREN_in_aggr_expr1423 = new BitSet(new long[]{0x0080010041800000L,0x0010000040420000L,0x0000000000180820L});
    public static final BitSet FOLLOW_arith_expr_main_in_aggr_expr1427 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000008000L});
    public static final BitSet FOLLOW_STAR_in_aggr_expr1431 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000008000L});
    public static final BitSet FOLLOW_RPAREN_in_aggr_expr1434 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_SUM_in_aggr_expr1464 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000020L});
    public static final BitSet FOLLOW_KW_AVG_in_aggr_expr1468 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000020L});
    public static final BitSet FOLLOW_LPAREN_in_aggr_expr1471 = new BitSet(new long[]{0x0080010041800000L,0x0010000040420000L,0x0000000000100820L});
    public static final BitSet FOLLOW_arith_expr_main_in_aggr_expr1473 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000008000L});
    public static final BitSet FOLLOW_KW_MAX_in_aggr_expr1481 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000020L});
    public static final BitSet FOLLOW_KW_MIN_in_aggr_expr1485 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000020L});
    public static final BitSet FOLLOW_LPAREN_in_aggr_expr1488 = new BitSet(new long[]{0x0080010041800000L,0x0010000040420000L,0x0000000000100820L});
    public static final BitSet FOLLOW_arith_expr_main_in_aggr_expr1490 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000008000L});
    public static final BitSet FOLLOW_RPAREN_in_aggr_expr1495 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_func_name_in_func_expr1538 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000000020L});
    public static final BitSet FOLLOW_LPAREN_in_func_expr1540 = new BitSet(new long[]{0x0090010041800000L,0x0010000040420000L,0x0000000000100820L});
    public static final BitSet FOLLOW_non_mt_arg_list_in_func_expr1544 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000008000L});
    public static final BitSet FOLLOW_KW_DISTINCT_in_func_expr1548 = new BitSet(new long[]{0x0080010041800000L,0x0010000040420000L,0x0000000000100820L});
    public static final BitSet FOLLOW_arith_expr_in_func_expr1550 = new BitSet(new long[]{0x0000000000000000L,0x0000000000000000L,0x0000000000008000L});
    public static final BitSet FOLLOW_RPAREN_in_func_expr1553 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_Identifier_in_func_name1589 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_Identifier_in_attr1600 = new BitSet(new long[]{0x0000000000010002L});
    public static final BitSet FOLLOW_DOT_in_attr1604 = new BitSet(new long[]{0x0000000000800000L});
    public static final BitSet FOLLOW_Identifier_in_attr1606 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_DOT_in_attr1610 = new BitSet(new long[]{0x0080000000000000L});
    public static final BitSet FOLLOW_pseudo_column_in_attr1612 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_pseudo_column_in_attr1619 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_element_time_in_pseudo_column1630 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_KW_ELEMENT_TIME_in_element_time1641 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_Integer_in_const_int1652 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_Number_in_const_float1664 = new BitSet(new long[]{0x0000000000000002L});
    public static final BitSet FOLLOW_unary_condition_in_synpred2_CongerCQL900 = new BitSet(new long[]{0x0000000000000002L});

}