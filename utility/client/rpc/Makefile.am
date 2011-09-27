all: rpc_HeadServer.h   rpc_RegionServer.h	rpc_StatisticsServer.h

rpc_RegionServer.h:  ../region/RegionServer.h
rpc_HeadServer.h:    ../../../tool/head/HeadServer.h
rpc_StatisticsServer.h:  ../region/StatisticsServer.h

rpc_%.h:
	nmstl-rpcgen --uppercase --namespace Borealis $^
	mv $(dir $^)$(patsubst %,rpc_%,$(notdir $^)) .

clean:
	cd .. ; find . -name "rpc_*.h" -exec rm {} ";"
