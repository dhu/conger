
#all: rpc_Lookup.h rpc_QueryProcessor.h rpc_HA.h rpc_LocalLoadShedder.h rpc_NHLoadShedder.h rpc_NHOptimizer.h rpc_CentralLookupServer.h rpc_QOSOptimizer.h rpc_UpdateServer.h rpc_AvailabilityMonitor.h
#all: rpc_Lookup.h rpc_QueryProcessor.h rpc_LocalLoadShedder.h rpc_NHLoadShedder.h rpc_NHOptimizer.h rpc_CentralLookupServer.h rpc_QOSOptimizer.h rpc_UpdateServer.h rpc_AvailabilityMonitor.h
all: rpc_QueryProcessor.h rpc_LocalLoadShedder.h rpc_NHLoadShedder.h rpc_NHOptimizer.h rpc_QOSOptimizer.h rpc_UpdateServer.h rpc_AvailabilityMonitor.h

rpc_LocalLoadShedder.h: ../optimizer/local/LocalLoadShedder.h
rpc_NHLoadShedder.h: ../optimizer/nhopt/NHLoadShedder.h
rpc_NHOptimizer.h: ../optimizer/nhopt/NHOptimizer.h
rpc_AvailabilityMonitor.h: ../availMon/AvailabilityMonitor.h
#rpc_Lookup.h: ../catalog/central/Lookup.h
rpc_QueryProcessor.h: ../queryProcessor/QueryProcessor.h
#rpc_HA.h: ../ha/HA.h
#rpc_CentralLookupServer.h: ../../src/CentralLookupServer.h
rpc_QOSOptimizer.h: ../optimizer/nhopt/QOSOptimizer.h
rpc_UpdateServer.h: ../queryProcessor/UpdateServer.h

rpc_%.h:
	nmstl-rpcgen --uppercase --namespace Borealis $^
	mv $(dir $^)$(patsubst %,rpc_%,$(notdir $^)) .

clean:
	cd .. ; find . -name "rpc_*.h" -exec rm {} ";"
