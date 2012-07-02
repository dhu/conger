
##################################################################

MYFLAGS     = 
AM_CFLAGS   = $(MYFLAGS)
AM_CXXFLAGS = $(MYFLAGS)

INCLUDES= @BOREALISINCLUDE@   @EXTERNALINCLUDE@                            \
          -I../common       -I../util         -I../rpc                     \
          -I../catalog      -I../optimizer/nhopt                          

lib_LTLIBRARIES = libcongercql.la  

libcongercql_la_SOURCES =                                 \
           CongerCQLLexer.c	CongerCQLLexer.h	\
           CongerCQLParser.c	CongerCQLParser.h	\	
           ParseContext.cc	ParseContext.h	\
		   ParseDriver.cc	ParseDriver.h
           

LDADD_MODULES = @NMSTLLIB@                             

#
###########################  end  Makefile.am  ################################
