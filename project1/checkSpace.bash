#!/bin/bash
THRESHOLD=64 # kbytes

cd reports

if [ $(du -c report_*.csv | tail -n1 | cut -f1) -le $THRESHOLD ]; then
	echo Niente da cancellare...
else
	while [ $(du -c report_*.csv | tail -n1 | cut -f1) -gt $THRESHOLD ]; do
		# echo Current: $(du -c report_*.csv | tail -n1 | cut -f1) threshold: $THRESHOLD
		lastFile=$(ls -tr report_*.csv | head -n1)
		echo $lastFile rimosso
		rm $lastFile
	done;
fi