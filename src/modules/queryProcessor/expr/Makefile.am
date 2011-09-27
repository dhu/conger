MYFLAGS     =
AM_CFLAGS   = $(MYFLAGS)
AM_CXXFLAGS = $(MYFLAGS)

INCLUDES = @BOREALISINCLUDE@   @BDBINCLUDE@                    \
           -I../../common    -I../../util    -I../runtime      \
           -I../../catalog   -I../

# Note: I don't know how to make antlr produce .h and .cc files rather than .hpp and .cpp
EXPR_GENS = \
	ExprWalker.cpp \
	ExprWalker.hpp \
	ExprParser.cpp \
	ExprParser.hpp \
	ExprLexer.cpp \
	ExprLexer.hpp \
	ExprParserTokenTypes.hpp \
	ExprParserTokenTypes.txt

.makeparser: expr.g
	$(JAVA) -cp $(ANTLR_JAR_FILE) antlr.Tool -o . $<
	date > .makeparser

$(EXPR_GENS): .makeparser
	touch $(EXPR_GENS)

ALL_GENERATED_FILES = .makeparser $(EXPR_GENS)

CLEANFILES=$(ALL_GENERATED_FILES)

noinst_LTLIBRARIES = libborealisqpgen.la libborealisqpexpr.la 

libborealisqpgen_la_SOURCES = $(ALL_GENERATED_FILES)

libborealisqpexpr_la_SOURCES =                                 \
           Aggregate.cc                 Aggregate.h            \
           Expression.cc                Expression.h           \
           EString.cc                   EString.h              \
                                        EStringPool.h          \
                                        ExprUtil.h             \
           NArgs.cc                     NArgs.h                \
           SQLDelete.cc                 SQLDelete.h            \
           SQLSelect.cc                 SQLSelect.h            \
           SQLUtil.cc                   SQLUtil.h              \
           SQLUpdate.cc                 SQLUpdate.h            \
           SQLInsert.cc                 SQLInsert.h            \
           StandardAggregates.cc        StandardAggregates.h   \
           StandardFunctions.cc	                               \
           MathFunctions.cc                                    \
	   SpatioTemporalFunctions.cc

libborealisqpexpr_la_LIBADD = libborealisqpgen.la
