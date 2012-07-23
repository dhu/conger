## 这个目录主要包含 CQL 查询的部署工具
包含两个测试文件，分别是 
```
cql_aggregate.xml  测试单个的 aggregate 
cql_join.xml  测试单个的 join 
```

### 运行方法 
```
./congeradmin cql_query_filename
```

## 客户端的运行方法
需切换目录到 tool/client

###cql_aggregate
```
./dequeueAggregate
./enqueue
```

###cql_join
```
./dequeueJoin
./enqueueUnion
```
