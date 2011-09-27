#!/usr/bin/perl -w

$SET_LIB_PATH="env LD_LIBRARY_PATH=/pro/borealis/software/nmstl/lib:/pro/borealis/software/bdb/lib:/pro/borealis/software/antlr/lib:/pro/borealis/software/xerces/lib:/pro/sand/software/antlr-2.7.2/lib:/pro/sand/software/bdb-4.0.14/lib:/home/bb/in_nmstl/lib:/pro/sand/software/xerces-c.2.3.0/lib:/pro/aurora/tools/linux/built_with_gcc3.2/BerkeleyDB.4.0/lib/:/pro/aurora/tools/linux/built_with_gcc3.2/xerces-c-src2_1_0/lib/:/usr/lib/kde2:/u/alexr/Aurora/qt-x11-free-3.0.5/lib:/usr:/pro/aurora/linux/BerkeleyDB.4.0/lib::/pro/sand/software/nmstl/lib:/pro/sand/software/bdb-4.0.14/lib:/pro/sand/software/antlr-2.7.2/lib:/pro/sand/software/xerces-c.2.3.0/lib" ;


@nodes=("127.0.0.1:15000", "127.0.0.1:20000");

$RUN_CLS="../../../src/src/CentralLookupServer -d 127.0.0.1:30000 2";
$RUN_BOREALIS="../../../src/src/borealis";
$RUN_LOAD_MGR="../../../tool/optimizer/loadManager/LoadManager";
$RUN_HEAD="../../../tool/head/BigGiantHead";
$RUN_ADD_QUERY="$RUN_HEAD lmtest.xml lmtest_deploy.xml";
$RUN_LMTEST="./lmtest 500";
#$RUN_2N_QUERY="$RUN_HEAD lmtest2n.xml lmtest2n_deploy.xml";
#$RUN_1N_QUERY="$RUN_HEAD lmtest.xml lmtest_deploy.xml";
#$INPUTGEN="../../../tool/optimizer/loadManager/inputgen";

# Run a given command in a xterm 
# param 1 command
my $xterm_index = 0;
sub xrun{
  # set xterm position
  my $ypos = $xterm_index * 100; 
  $xterm_index++;

  my $command = $_[0];
  system ("echo Run $command");
  system("xterm -T \"$command\" -geometry 80x5+0+$ypos -e $SET_LIB_PATH sh -c \"$command\" &");
}
# Run a given command in a xterm and add loop after the program 
sub hrun{
  # set xterm position
  my $ypos = $xterm_index * 100; 
  $xterm_index++;

  my $command = $_[0];
  system ("echo Run $command");
  system("xterm -T \"$command\" -geometry 80x5+0+$ypos -e $SET_LIB_PATH sh -c \"$command & loop\" &");
}


# This function runs all borealis nodes whose name are given in @nodes 
# in the first argument
sub xrun_borealis {
  foreach my $node (@nodes) {
    my $command="$RUN_BOREALIS -d $node";
    xrun($command);
  }
}

my  $action="all";
if ( @ARGV !=  0 ) {
  $action= $ARGV[0];
}

for ($action) {
  /all/ and do {
    xrun($RUN_CLS);
    sleep 1;
    xrun_borealis;
    xrun($RUN_LOAD_MGR);
    sleep 2;
    hrun($RUN_ADD_QUERY);
    xrun($RUN_LMTEST);
    last;
  };
  /stop/ and do {
    system("killall xterm");
    system("killall LoadManager");
    system("killall CentralLookupServer");
    last;
  };
  
  print "No such action: $action";
}


