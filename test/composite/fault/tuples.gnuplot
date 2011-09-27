#set terminal fig monochrome fontsize 14 thickness 1
#set output "tuples1.fig"

set terminal postscript eps 26
set output "tuples1.eps"

set xlabel "Time (msec)"
set ylabel "Tuple sequence number"

plot "results1.dat" title ""

#set output "tuples2.fig"
set output "tuples2.eps"
plot "results2.dat" title ""



