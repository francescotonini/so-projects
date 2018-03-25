#!/bin/bash

# Cds into reports
cd reports/

# 1.
# Since reports aren't modified once they have been created, we can sort by last modification
# time
lastReport=$(ls -t report_*.csv | head -n1)
echo Last report: $lastReport
while read user; do
	echo $user: $(cat $lastReport | cut -d ";" -f1 | grep $user | wc -l)
done <<< "$(cat $lastReport | cut -d ";" -f1 | sort -u)"

# 2.
read -p "Inserire utente: " user
for file in $(ls -t report_*.csv); do
	echo report: $file processi $(cat $file | cut -d ";" -f1 | grep $user | wc -l)
done

exit 0
