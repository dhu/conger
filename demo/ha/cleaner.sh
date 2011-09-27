#!/bin/sh
while read line  # For as many lines as the input file has...
do
line=${line/paste:/}   # Output the line itself.
line=${line/capture:/}
echo $line
done

exit 0