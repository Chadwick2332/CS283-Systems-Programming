#!/bin/sh

for i in {1..10}
do
	echo "run ${i}"
	time $1
done
