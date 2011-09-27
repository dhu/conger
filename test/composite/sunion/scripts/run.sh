#!/bin/bash

for ((k=0; k < 5; k++))
do
    ./runtest distributed many-nodes.params:62 setup3.dat 120000 hosts.txt

#echo "*** Iteration $k: 60 second experiments ***"
#./runtest distributed many-nodes.params:54 setup1e.dat 120000 hosts.txt
#./runtest distributed many-nodes.params:53 setup1e.dat 120000 hosts.txt
#./runtest distributed many-nodes.params:52 setup1e.dat 120000 hosts.txt
#./runtest distributed many-nodes.params:51 setup1e.dat 120000 hosts.txt
#./runtest distributed many-nodes.params:50 setup1e.dat 120000 hosts.txt
#./runtest distributed many-nodes.params:49 setup1e.dat 120000 hosts.txt
#./runtest distributed many-nodes.params:48 setup1e.dat 120000 hosts.txt
#./runtest distributed many-nodes.params:47 setup1e.dat 120000 hosts.txt

# echo "*** Iteration $k: 30 second experiments ***"
# ./runtest distributed many-nodes.params:14 setup1a.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:13 setup1a.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:12 setup1a.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:11 setup1a.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:10 setup1a.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:9 setup1a.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:8 setup1a.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:7 setup1a.dat 90000 hosts.txt

# echo "*** Iteration $k: Done with 30 second experiments, now running 15 second ***"
# ./runtest distributed many-nodes.params:24 setup1b.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:23 setup1b.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:22 setup1b.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:21 setup1b.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:20 setup1b.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:19 setup1b.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:18 setup1b.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:17 setup1b.dat 90000 hosts.txt

# echo "*** Iteration $k: Done with 15 second experiments, now running 10 second ***"
# ./runtest distributed many-nodes.params:34 setup1c.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:33 setup1c.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:32 setup1c.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:31 setup1c.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:30 setup1c.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:29 setup1c.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:28 setup1c.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:27 setup1c.dat 90000 hosts.txt

# echo "*** Iteration $k: Done with 10 second experiments, now running 5 second ***"
# ./runtest distributed many-nodes.params:44 setup1d.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:43 setup1d.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:42 setup1d.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:41 setup1d.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:40 setup1d.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:39 setup1d.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:38 setup1d.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:37 setup1d.dat 90000 hosts.txt

# echo "*** Iteration $k: Running setup 3 ***"
# ./runtest distributed many-nodes.params:61 setup3.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:60 setup3.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:59 setup3.dat 90000 hosts.txt
# ./runtest distributed many-nodes.params:58 setup3.dat 90000 hosts.txt

echo "*** Iteration $k: Done with ALL ***"

done                   


