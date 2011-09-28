#!/bin/bash
# build file for polaris

#
# Polaris 构建脚本
HELP_TEXT=" \n 1. 不带任何参数: 显示本文档\n \
    2. 参数 all: 构建整个系统(borealis, marshal, head)\n \
    3. 参数 init: 仅初始化环境，不进行任何编译 \n \
    4. 参数 help: 显示本文档 \n"
#

export CVS_SANDBOX=$HOME/Workspace/borealis_summer_2008
export BOREALIS_HOME=$CVS_SANDBOX/polaris
export BOREALIS_TOOL=/opt/borealis-tools

source $BOREALIS_TOOL/rc

if [ "x$TOOL_PATH_SET" = "x" ]; then
    export TOOL_PATH_SET="tool_path_already_set"
    export PATH=/home/jj/Workspace/borealis_summer_2008/polaris/tool/marshal:$PATH
    export PATH=/home/jj/Workspace/borealis_summer_2008/polaris/tool/head:$PATH
fi

case "x$1" in
    "x" )
        echo -e $HELP_TEXT
        ;;
    "xinit" )
        echo "Environment is set... Ready to work..."
        ;;
    "xhelp" )
        echo -e $HELP_TEXT
        ;;
    "xall" )
        cd utility/unix/
        ./build.borealis.sh
        ./build.borealis.sh -client -tool.marshal -tool.head
        echo "Exit directory: " $(pwd)
        cd -
        ;;
esac

