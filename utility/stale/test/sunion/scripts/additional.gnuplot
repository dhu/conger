set terminal postscript eps 26

# --------------------------------------------------
# Additional graph for experiment1 
# (adding the one with the limited history onto this graph as well)
set output "experiment1.state_size.together.eps"
set xlabel "State size (tuples)"

set ylabel "Max. proc. delay (ms)"

set xrange [0:4000]
set yrange [0:4500]

set xtics 500
set ytics 500
set pointsize 2

plot    "experiment3.dat.state_size" using 1:3 title "Undo (2 boxes)" with linespoints, \
        "experiment3.dat.state_size" using 1:6 title "Checkpoint (2 boxes)" with linespoints, \
        "experiment1.dat.state_size" using 1:3 title "Undo" with linespoints, \
        "experiment1.dat.state_size" using 1:6 title "Checkpoint" with linespoints

### PNG ###
set terminal png medium
set pointsize 1.5
set output "experiment1.state_size.together.png"
plot "experiment1.dat.state_size" using 1:3 title "Undo" with linespoints, \
     "experiment1.dat.state_size" using 1:6 title "Checkpoint" with linespoints 3
set terminal postscript eps 26
set pointsize 2
### PNG ###


#        "experiment2.dat.state_size" using 1:3 title "Undo (limited history)" with linespoints, \

# --------------------------------------------------
# Additional graph for experiment1 
# (adding the one with the limited history onto this graph as well)
set output "experiment1.tuple_rate.together.eps"
set xlabel "Failure size (tuples)"

set ylabel "Max. proc. delay (ms)"

set xrange [5000:45000]
set yrange [0:5500]

set xtics 10000
set ytics 1000
set pointsize 2

plot    "experiment3.dat.tuple_rate" using 1:3 title "Undo (2 boxes)" with linespoints, \
        "experiment3.dat.tuple_rate" using 1:6 title "Checkpoint (2 boxes)" with linespoints, \
        "experiment1.dat.tuple_rate" using 1:3 title "Undo" with linespoints, \
        "experiment2.dat.tuple_rate" using 1:3 title "Undo (limited history)" with linespoints, \
        "experiment1.dat.tuple_rate" using 1:6 title "Checkpoint" with linespoints

### PNG ###
set terminal png medium
set pointsize 1.5
set output "experiment1.tuple_rate.together.png"
plot "experiment1.dat.tuple_rate" using 1:3 title "Undo" with linespoints, \
     "experiment1.dat.tuple_rate" using 1:6 title "Checkpoint" with linespoints 3, \
     "experiment2.dat.tuple_rate" using 1:3 title "Undo (limited history)" with linespoints 4
set terminal postscript eps 26
set pointsize 2
### PNG ###


# --------------------------------------------------
## Additional graph for benefits1 (blocking vs non-blocking)
## Basically we put both graphs together
set output "benefits1.together.eps"
set xlabel "Delay, Alpha X (ms)"

set ylabel "Max. proc. delay (ms)"
set y2label "Number tentative tuples"

set xrange [0:6000]
set yrange [0:15000]

set xtics 1000
set ytics 2000
set pointsize 1.5

plot    "benefits1.dat.original.dup" using 1:3 title "Process (tentative)" with linespoints, \
        "benefits1.dat.dup" using 1:3 title "Delay (tentative)" with linespoints, \
        "benefits1.dat.delay" using 1:3 title "Delay (proc. time)" with linespoints, \
        "benefits1.dat.original.delay" using 1:3 title "Process (proc. time)" with linespoints



# --------------------------------------------------
## Additional graph for benefits2 (collaboration vs no-collaboration)
## Basically same graph just different scale

set output "benefits2.dat.delay.eps"

set xlabel "Failure duration (seconds)"
set ylabel "Max. proc. delay (ms)"
set y2label

set xrange [2000:60000]
set yrange [0:25000]
set xtics ( "5" 5000, "10" 10000, "15" 15000, "20" 20000, "25" 25000, "30" 30000, "35" 35000, "40" 40000, "45" 45000, "50" 50000, "55" 55000, "60" 60000)
set ytics 5000
set key top left

set pointsize 1.5
f(x) = 3000
plot    f(x) title "" with lines, \
        "benefits2.dat.delay" using 1:9 title "Process & process" with linespoints 1, \
        "benefits2.dat.delay" using 1:6 title "Delay & process" with linespoints 2, \
        "benefits2.dat.delay" using 1:12 title "Process & delay" with linespoints 3, \
        "benefits2.dat.delay" using 1:3 title "Delay & delay" with linespoints 4, \
        "benefits2.dat.delay" using 1:18 title "Process & suspend" with linespoints 8, \
        "benefits2.dat.delay" using 1:15 title "Delay & suspend" with linespoints 6

### PNG ###
set terminal png medium
set pointsize 1.5
set output "benefits2.dat.delay.png"
replot
set terminal postscript eps 26
set pointsize 2
### PNG ###

set output "benefits2.dat.delay.zoom.eps"
set xrange [2000:15000]
set yrange [0:10000]
set xtics ( "2" 2000, "4" 4000, "6" 6000, "8" 8000, "10" 10000, "12" 12000, "14" 14000, "16" 16000, "18" 18000, "20" 20000) 
replot

### PNG ###
set terminal png medium
set pointsize 1.5
set output "benefits2.dat.delay.zoom.png"
replot
set terminal postscript eps 26
set pointsize 2
### PNG ###


# ----------------
set output "benefits2.dat.dup.eps"
set xlabel "Failure duration (seconds)"
set ylabel "Number tentative tuples"
set xrange [2000:60000]
set yrange [0:500000]

set xtics ( "5" 5000, "10" 10000, "15" 15000, "20" 20000, "25" 25000, "30" 30000, "35" 35000, "40" 40000, "45" 45000, "50" 50000, "55" 55000, "60" 60000)
set ytics 50000

set pointsize 1.5
f(x) = 3000
plot    "benefits2.dat.dup" using 1:9 title "Process & process" with linespoints 1, \
        "benefits2.dat.dup" using 1:6 title "Delay & process" with linespoints 2, \
        "benefits2.dat.dup" using 1:12 title "Process & delay" with linespoints 3, \
        "benefits2.dat.dup" using 1:3 title "Delay & delay" with linespoints 4, \
        "benefits2.dat.dup" using 1:18 title "Process & suspend" with linespoints 8, \
        "benefits2.dat.dup" using 1:15 title "Delay & suspend" with linespoints 6

### PNG ###
set terminal png medium
set pointsize 1.5
set output "benefits2.dat.dup.png"
replot
set terminal postscript eps 26
set pointsize 2
### PNG ###

set output "benefits2.dat.dup.zoom.eps"
set xrange [2000:15000]
set yrange [0:100000]
set ytics 10000
set xtics ( "2" 2000, "4" 4000, "6" 6000, "8" 8000, "10" 10000, "12" 12000, "14" 14000, "16" 16000, "18" 18000, "20" 20000) 
replot

### PNG ###
set terminal png medium
set pointsize 1.5
set output "benefits2.dat.dup.zoom.png"
replot
set terminal postscript eps 26
set pointsize 2
### PNG ###

# --------------------------------------------------
## Additional graph for adaptive algorithm 
## (including the flat line showing the max delay)

set output "adaptive1.dat.delay.eps"

set xlabel "Failure duration (log scale)"
set ylabel "Max. proc. delay (ms)"
set y2label

set xrange [500:300000]
set xtics ( "1s" 1000,"2.5" 2500, "5s" 5000, "20s" 20000, "1mn" 60000, "5mn" 300000 )
set log x

set yrange [0:4000]
set ytics 500

f(x) = 3000
g(x) = 2400

set pointsize 1.5
plot    f(x) title "Max. Delay" with lines, g(x) title "80% Max. Delay" with lines, "adaptive1.dat.delay" using 1:3 title "Adaptive" with linespoints

set terminal fig monochrome fontsize 14 thickness 1
set output "adaptive1.dat.delay.fig"
replot

set terminal postscript eps 26
set nologscale x

# --------------------------------------------------
## Additional graph for adaptive algorithm 

set output "adaptive1.dat.dup.eps"
set xlabel "Failure duration (log scale)"
set ylabel "Number tentative tuples"

set xrange [500:300000]
set xtics ( "1s" 1000, "2.5" 2500, "5s" 5000, "20s" 20000, "1mn" 60000, "5mn" 300000 )
set yrange [1:1309338]
set ytics ( 10, 100, 1000, 3000, "10K" 10000, "100K" 100000, "1M" 1000000)
#set log xy
f(x) = 3000

set pointsize 1.5
plot f(x) title "" with lines, "adaptive1.dat.dup" using 1:3 title "Adaptive" with linespoints

set terminal fig monochrome fontsize 14 thickness 1
set output "adaptive1.dat.dup.fig"
replot

set terminal postscript eps 26
set nologscale xy



# --------------------------------------------------
set output "micro3.eps"

set xlabel "State size (tuples)"
set ylabel "Duration (microseconds)"

set xrange [0:2501]
set yrange [0:650]
set xtics 500
set ytics 100


set pointsize 2
plot    "ConsistencyMngr.log.micro3" using 3:5 title "" with lines, \
        "SJoinQBox.log.micro3.copy" using 3:5 title "" with lines,\
        "SJoinQBox.log.micro3.read" using 3:5 title "" with lines, \
        "ConsistencyMngr.log.micro3" using 3:5:7:9 title "Checkpoint" with errorbars, \
        "SJoinQBox.log.micro3.copy" using 3:5:7:9 title "Copy state" with errorbars,\
        "SJoinQBox.log.micro3.read" using 3:5:7:9 title "Compute stream markers" with errorbars




# --------------------------------------------------
### XXX Remember to manually add the x axis
## Graph with multiple nodes
## (including the flat line showing the max delay)

set terminal postscript eps enhanced color solid
set output "many-nodes.delay.eps"

set xlabel "Number of nodes"
set ylabel "Max. proc. delay (ms)"

set xrange [0:5]
set yrange [0:11500]
set xtics 1
set ytics 2000

set arrow 1 from 0.75,2000 to 0.75,10000 nohead
set arrow 2 from 0.75,10000 to 1.45,10000 nohead
set label "Process and process" at 1.55,10000 left font "Times,24"

set arrow 3 from 1,2000 to 1,9000 nohead lt 2
set arrow 4 from 1,9000 to 1.45,9000 nohead lt 2
set label "Delay and delay" at 1.55,9000 left font "Times,24"

set arrow 5 from 1.25,2000 to 1.25,8000 nohead lt 3
set arrow 6 from 1.25,8000 to 1.45,8000 nohead lt 3
set label "Process and suspend" at 1.55,8000 left font "Times,24"

show arrow

set pointsize 1.5
set boxwidth 0.25
plot    "many-nodes.dat.4.PP.delay" using 1:3 title "" with boxes lt 1, \
        "many-nodes.dat.4.DD.delay" using 1:3 title "" with boxes lt 2, \
        "many-nodes.dat.4.PS.delay" using 1:3 title "" with boxes lt 3

#set linestyle 2 lt 1 lw 2
#set boxwidth 0.66
#plot "many-nodes.dat.4.delay" using 1:3 title "" with boxes ls 2

### PNG ###
set terminal png medium
set pointsize 1.5
set output "many-nodes.delay.png"
replot
set terminal postscript eps 26
set pointsize 2
### PNG ###

reset 

# --------------------------------------------------
set terminal postscript eps enhanced color solid
set output "many-nodes.dup.eps"

set xlabel "Number of nodes"
set ylabel "Number tentative tuples"

set xrange [0:5]
set yrange [0:35000]
set xtics 1
set ytics 5000

set arrow 1 from 0.75,15000 to 0.75,31000 nohead
set arrow 2 from 0.75,31000 to 1.45,31000 nohead
set label "Process and process" at 1.55,31000 left font "Times,24"

set arrow 3 from 1,15000 to 1,28000 nohead lt 2
set arrow 4 from 1,28000 to 1.45,28000 nohead lt 2
set label "Delay and delay" at 1.55,28000 left font "Times,24"

set arrow 5 from 1.25,15000 to 1.25,25000 nohead lt 3
set arrow 6 from 1.25,25000 to 1.45,25000 nohead lt 3
set label "Process and suspend" at 1.55,25000 left font "Times,24"

show arrow

set pointsize 1.5
set boxwidth 0.25
plot    "many-nodes.dat.4.PP.dup" using 1:3 title "" with boxes, \
        "many-nodes.dat.4.DD.dup" using 1:3 title "" with boxes, \
        "many-nodes.dat.4.PS.dup" using 1:3 title "" with boxes


### PNG ###
set terminal png medium
set pointsize 1.5
set output "many-nodes.dup.png"
replot
set terminal postscript eps 26
set pointsize 2
### PNG ###




