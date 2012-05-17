## 这个目录主要包含两个客户端

enqueue
	发送数据
	
dequeue
	接收数据
	
主要尝试将发送和接收数据分开

## 尝试 Borealis 基础的 Box
运行方法
首先运行 conger，然后运行对应的 dequeue 和 enqueue 就可以了
每次运行之前都要重新运行 conger

# fitler
```
./dequeueFilter
./enqueue filter
```

# map 
```
./dequeueMap
./enqueue map
```

# filter_map
```
./dequeueMap
./enqueue filter_map
```

# union
```
./dequeueFilter
./enqueueUnion
```