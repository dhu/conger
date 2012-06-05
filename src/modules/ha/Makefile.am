MYFLAGS =  
AM_CFLAGS   = $(MYFLAGS)
AM_CXXFLAGS = $(MYFLAGS)

INCLUDES = @BOREALISINCLUDE@   @EXTERNALINCLUDE@                           \
           -I../common                                                     \
           -I../util                                                       \
           -I../rpc                                                        \
           -I../queryProcessor                                             \
           -I../queryProcessor/expr                                        \
           -I../queryProcessor/boxes                                       \
           -I../queryProcessor/runtime                                     \
           -I../queryProcessor/storageMgr                                  \
           -I../queryProcessor/statsMgr                                    \
           -I../catalog                                                    \
           -I../optimizer/nhopt	                                           \
           -I../queryProcessor/congerDeploy	

lib_LTLIBRARIES = libborealisha.la

libborealisha_la_SOURCES =  HASegment.cc             HASegment.h           \
                            QueryProcessorWithHA.cc                        \
                            AuroraNodeWithHA.cc                            \
                            HA.cc     HA.h
