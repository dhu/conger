#include "StandardAggregates.h"

BOREALIS_NAMESPACE_BEGIN

NMSTL_SERIAL_DEFINE( CountAggregate,  1011);
NMSTL_SERIAL_DEFINE( count_window,    1012);


////////////////////////////

NMSTL_SERIAL_DEFINE( MinAggregate,    1013);
NMSTL_SERIAL_DEFINE( IntMinWindow,    1014);
NMSTL_SERIAL_DEFINE( LongMinWindow,   1015);
NMSTL_SERIAL_DEFINE( ShortMinWindow,  1016);
NMSTL_SERIAL_DEFINE( ByteMinWindow,   1017);
NMSTL_SERIAL_DEFINE( SingleMinWindow, 1018);
NMSTL_SERIAL_DEFINE( DoubleMinWindow, 1019);

////////////////////////////

NMSTL_SERIAL_DEFINE( max_aggregate,   1020);
NMSTL_SERIAL_DEFINE( IntMaxWindow,    1021);
NMSTL_SERIAL_DEFINE( LongMaxWindow,   1022);
NMSTL_SERIAL_DEFINE( ShortMaxWindow,  1023);
NMSTL_SERIAL_DEFINE( ByteMaxWindow,   1024);
NMSTL_SERIAL_DEFINE( SingleMaxWindow, 1025);
NMSTL_SERIAL_DEFINE( DoubleMaxWindow, 1026);

////////////////////////////

NMSTL_SERIAL_DEFINE( sum_aggregate,   1030);
NMSTL_SERIAL_DEFINE( IntSumWindow,    1031);
NMSTL_SERIAL_DEFINE( LongSumWindow,   1032);
NMSTL_SERIAL_DEFINE( ShortSumWindow,  1033);
NMSTL_SERIAL_DEFINE( ByteSumWindow,   1034);
NMSTL_SERIAL_DEFINE( SingleSumWindow, 1035);
NMSTL_SERIAL_DEFINE( DoubleSumWindow, 1036);

////////////////////////////

NMSTL_SERIAL_DEFINE( avg_aggregate,   1040);
NMSTL_SERIAL_DEFINE( IntAvgWindow,    1041);
NMSTL_SERIAL_DEFINE( LongAvgWindow,   1042);
NMSTL_SERIAL_DEFINE( ShortAvgWindow,  1043);
NMSTL_SERIAL_DEFINE( ByteAvgWindow,   1044);
NMSTL_SERIAL_DEFINE( SingleAvgWindow, 1045);
NMSTL_SERIAL_DEFINE( DoubleAvgWindow, 1046);

////////////////////////////

NMSTL_SERIAL_DEFINE( FirstvalAggregate,    1050);
NMSTL_SERIAL_DEFINE( IntFirstvalWindow,    1051);
NMSTL_SERIAL_DEFINE( LongFirstvalWindow,   1052);
NMSTL_SERIAL_DEFINE( ShortFirstvalWindow,  1053);
NMSTL_SERIAL_DEFINE( ByteFirstvalWindow,   1054);
NMSTL_SERIAL_DEFINE( SingleFirstvalWindow, 1055);
NMSTL_SERIAL_DEFINE( DoubleFirstvalWindow, 1056);

////////////////////////////

NMSTL_SERIAL_DEFINE( LastvalAggregate,    1060);
NMSTL_SERIAL_DEFINE( IntLastvalWindow,    1061);
NMSTL_SERIAL_DEFINE( LongLastvalWindow,   1062);
NMSTL_SERIAL_DEFINE( ShortLastvalWindow,  1063);
NMSTL_SERIAL_DEFINE( ByteLastvalWindow,   1064);
NMSTL_SERIAL_DEFINE( SingleLastvalWindow, 1065);
NMSTL_SERIAL_DEFINE( DoubleLastvalWindow, 1066);

BOREALIS_NAMESPACE_END
//
/////////////////////// end  StandardAggregates.cc  //////////////////////////
