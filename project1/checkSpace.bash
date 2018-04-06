#!/bin/bash
THRESHOLD=64 # kbytes

# Check if folder "reports" exists, otherwise exit
if [ ! -d "reports" ]; then
	echo Nessun report da rimuovere
	exit 0
fi

# Cds into reports
cd reports/

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