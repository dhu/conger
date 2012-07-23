# gdb 调试 conger 时的一些工具命令
#
# author: hellojinjie
#
#

define cqlbreak
    set breakpoint pending on
    break ParseDriver.cc:63
end

document cqlbreak
    调试解析CQL时要设的断点
end

define cqlcontext
    if $argc == 0
        help cqlcontext
    else
        set $context = $arg0
        printf "Origin CQL: %s\n", $context.origin_cql._M_dataplus._M_p 
    end

end

document cqlcontext
    打印解析 CQL 的 context

end

