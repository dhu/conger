#!/bin/sh
for i in $(ls *$1*);do mv ${i} ${i/$1/$2}; done
