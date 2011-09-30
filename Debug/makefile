################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include utility/stale/tinydbwraper/test/subdir.mk
-include utility/stale/tinydbwraper/subdir.mk
-include utility/stale/test/sunion/src/subdir.mk
-include utility/stale/runtime/subdir.mk
-include utility/stale/boxes/subdir.mk
-include utility/client/region/subdir.mk
-include utility/client/region/.libs/subdir.mk
-include utility/client/java/src/subdir.mk
-include utility/client/java/src/clientApi/subdir.mk
-include utility/client/global/subdir.mk
-include tool/optimizer/loadShedder/subdir.mk
-include tool/optimizer/loadManager/subdir.mk
-include tool/optimizer/subdir.mk
-include tool/marshal/subdir.mk
-include tool/loadGenerator/wlgen/subdir.mk
-include tool/loadGenerator/simple/subdir.mk
-include tool/head/subdir.mk
-include tool/head/.libs/subdir.mk
-include test/valid/two_chains/subdir.mk
-include test/valid/system/subdir.mk
-include test/valid/revisions/subdir.mk
-include test/valid/move/subdir.mk
-include test/valid/global/subdir.mk
-include test/valid/deploy/subdir.mk
-include test/valid/connect/subdir.mk
-include test/valid/catalog/subdir.mk
-include test/valid/array/subdir.mk
-include test/unit/subdir.mk
-include test/simple/subdir.mk
-include test/developer/subdir.mk
-include test/composite/updates/subdir.mk
-include test/composite/sunion/src/subdir.mk
-include test/composite/revision/subdir.mk
-include test/composite/loadnetwork/subdir.mk
-include test/composite/loadbox/subdir.mk
-include test/composite/fault/subdir.mk
-include test/composite/deltaProcessing/video/subdir.mk
-include test/composite/deltaProcessing/stars/subdir.mk
-include test/composite/deltaProcessing/ais/subdir.mk
-include test/composite/NHLoadShedder/subdir.mk
-include test/composite/GlobalLoadShedder/chain/subdir.mk
-include src/src/subdir.mk
-include src/modules/util/subdir.mk
-include src/modules/util/.libs/subdir.mk
-include src/modules/queryProcessor/storageMgr/subdir.mk
-include src/modules/queryProcessor/storageMgr/.libs/subdir.mk
-include src/modules/queryProcessor/statsMgr/subdir.mk
-include src/modules/queryProcessor/statsMgr/.libs/subdir.mk
-include src/modules/queryProcessor/runtime/subdir.mk
-include src/modules/queryProcessor/runtime/.libs/subdir.mk
-include src/modules/queryProcessor/expr/subdir.mk
-include src/modules/queryProcessor/expr/.libs/subdir.mk
-include src/modules/queryProcessor/boxes/subdir.mk
-include src/modules/queryProcessor/boxes/.libs/subdir.mk
-include src/modules/queryProcessor/subdir.mk
-include src/modules/queryProcessor/.libs/subdir.mk
-include src/modules/optimizer/nhopt/subdir.mk
-include src/modules/optimizer/nhopt/.libs/subdir.mk
-include src/modules/optimizer/local/subdir.mk
-include src/modules/optimizer/local/.libs/subdir.mk
-include src/modules/ha/subdir.mk
-include src/modules/ha/.libs/subdir.mk
-include src/modules/common/subdir.mk
-include src/modules/common/.libs/subdir.mk
-include src/modules/catalog/subdir.mk
-include src/modules/catalog/.libs/subdir.mk
-include src/modules/availMon/subdir.mk
-include src/modules/availMon/.libs/subdir.mk
-include src/external/udb/stub/subdir.mk
-include src/external/udb/stub/.libs/subdir.mk
-include src/external/udb/subdir.mk
-include src/external/subdir.mk
-include src/external/bdb/stub/subdir.mk
-include src/external/bdb/stub/.libs/subdir.mk
-include src/external/bdb/subdir.mk
-include src/external/array/stub/subdir.mk
-include src/external/array/stub/.libs/subdir.mk
-include src/external/array/subdir.mk
-include src/external/.libs/subdir.mk
-include demo/sigmod2005/cube/bot_src/src/subdir.mk
-include demo/sigmod2005/cube/bot_src/enet/subdir.mk
-include demo/parallel/subdir.mk
-include demo/mitre/subdir.mk
-include demo/ids/subdir.mk
-include demo/ha/subdir.mk
-include demo/contrast_decision/subdir.mk
-include demo/contrast_composite/subdir.mk
-include demo/array/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: polaris

# Tool invocations
polaris: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++  -o"polaris" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) polaris
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
