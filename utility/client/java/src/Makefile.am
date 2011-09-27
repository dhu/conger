################################################################################
#
#  A client subscribes to an Output stream from a Borealis node.
#
#  Creates:  subscribe
#            utility/client/java/lib/clientApi.jar
#            utility/client/java/lib/medusaXmlRpc.jar
#
#  Requires: utility/client/java/src/medusaXmlRpc/lib/xmlrpc.jar
#            http://mirrors.isc.org/pub/apache/ws/xmlrpc/xmlrpc-client-3.0rc1.jar
#...............................................................................


MYFLAGS =
AM_CFLAGS = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

MAIN = subscribe

bin_PROGRAMS = $(MAIN)
BOREALIS_SRC = ../../@BOREALISROOT@

LDADD_EXTRAS = @XERCESCLIB@   @NMSTLLIB@

LDADD = -L$(BOREALIS_SRC)/modules/common        -lborealiscommon   \
        -L$(BOREALIS_SRC)/modules/util          -lborealisutil     \
        $(LDADD_EXTRAS)

INCLUDES = @XERCESCINCLUDE@   @NMSTLINCLUDE@                       \
           -I$(BOREALIS_SRC)/modules/common                        \
           -I$(BOREALIS_SRC)/modules/queryProcessor/statsMgr       \
           -I$(BOREALIS_SRC)/modules/queryProcessor                \
           -I$(BOREALIS_SRC)/modules/util                          \
           -I$(BOREALIS_SRC)/modules/rpc                           \
           -I$(BOREALIS_SRC)/modules/catalog


DEPENDENCIES = $(BOREALIS_SRC)/modules/common/libborealiscommon.la   

subscribe_DEPENDENCIES = 

subscribe_SOURCES = ${subscribe_DEPENDENCIES} clientApi/subscribe.cc

java_JAVA = clientApi/CompactSerializable.class            \
            clientApi/CCompatibleInputStream.class         \
            clientApi/CCompatibleOutputStream.class        \
            clientApi/StreamEvent.class                    \
            clientApi/Subscription.class                   \
            clientApi/TupleHeader.class                    \
            clientApi/Tuple.class                          \
            clientApi/TupleReceptionEventHandler.class     \
            clientApi/Marshal.class                        \
            clientApi/TcpDump.class                        \
            clientApi/HexDump.class                        \
            clientApi.jar                                  \
            medusaXmlRpc/MedusaEventHandler.class          \
            medusaXmlRpc/MedusaEvent.class                 \
            medusaXmlRpc/MedusaEventListener.class         \
            medusaXmlRpc/MedusaXmlRpc.class                \
            medusaXmlRpc/RuntimeArgs.class                 \
            medusaXmlRpc/SchemaVector.class                \
            medusaXmlRpc/ServerNode.class                  \
            medusaXmlRpc/StreamEventVector.class           \
            medusaXmlRpc/StreamVector.class                \
            medusaXmlRpc/SubscriptionVector.class          \
            medusaXmlRpc.jar

%.class: %.java
	javac -classpath .:medusaXmlRpc/lib/xmlrpc.jar $<

../lib:
	mkdir ../lib

medusaXmlRpc.jar: ../lib
	cp  medusaXmlRpc/lib/xmlrpc.jar  ../lib/medusaXmlRpc.jar
	jar uvf  ../lib/medusaXmlRpc.jar  medusaXmlRpc/*.class

clientApi.jar: ../lib
	jar cvf  ../lib/clientApi.jar  clientApi/*.class

classjava.stamp: $(java_JAVA)
	echo timestamp > classjava.stamp	

CLEANFILES = ${subscribe_DEPENDENCIES}                        \
             clientApi/*.class     ../lib/clientApi.jar       \
             medusaXmlRpc/*.class  ../lib/medusaXmlRpc.jar

#
################################################################################
