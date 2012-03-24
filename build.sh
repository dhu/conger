#!/bin/bash
# build script for polaris

#
# Polaris 构建脚本
HELP_TEXT=" \n 1. 不带任何参数,或参数 help: 显示本帮助文档\n \
    2. 参数 all: 构建整个系统(borealis, marshal, head)\n \
    3. 参数 init: 仅初始化环境，不进行任何编译 \n  "
#

export CVS_SANDBOX=$HOME/Workspace/borealis_summer_2008
export BOREALIS_HOME=$CVS_SANDBOX/conger
export BOREALIS_TOOL=/opt/borealis-tools

# NMSTL 调试输出的级别
export LOG_LEVEL="2"

source $BOREALIS_TOOL/rc

# 确保没有重复导入
if [ "x$TOOL_PATH_SET" = "x" ]; then
    export TOOL_PATH_SET="tool_path_already_set"
    export PATH=/home/jj/Workspace/borealis_summer_2008/conger/tool/marshal:$PATH
    export PATH=/home/jj/Workspace/borealis_summer_2008/conger/tool/head:$PATH
fi

# 判断这个脚本是在独立的子进程中执行的，还是在当前进程通过 source 运行的.
if [ "x$0" = "xbash" ]; then
    echo "Environment is initialized... Ready to work..."
else
    case "x$1" in
        "x" | "xhelp" )
            echo -e $HELP_TEXT
            ;;
        "xinit" )
            echo "Use source to initialize environment...try  source build.sh"
            ;;
        "xall" )
            cd utility/unix/
            ./build.borealis.sh
            ./build.borealis.sh -client -tool.marshal -tool.head
            echo "Exit directory: " $(pwd)
            cd -
            ;;
        * )
            echo "Unknow option: "$1
            ;;
    esac
fi

