## 这个目录主要包含测试用的客户端

enqueue
	发送数据
	
dequeue
	接收数据
	
主要尝试将发送和接收数据分开

## 输入数据的生成
`gendata.py` 是用 python 写的一个脚本，用来生成测试用的输入数据  
这里我们主要用到了 600000 和 600529 两个股票的 2011.3.15 这一天的数据  

## 尝试 Borealis 基础的 Box
运行方法
首先运行 conger，然后运行对应的 dequeue 和 enqueue 就可以了  
每次运行之前都要重新运行 conger  

### fitler
```
./dequeueFilter
./enqueue filter
```

### map 
```
./dequeueMap
./enqueue map
```

### filter_map
```
./dequeueMap
./enqueue filter_map
```

### union
```
./dequeueFilter
./enqueueUnion
```

### join
```
./dequeueJoin
./enqueueUnion join
```

### aggregate
```
./dequeueAggregate
./enqueue aggregate
```

### aggregate_join
```
./dequeueAggregateJoin
./enqueue aggregate_join
```

