#!/bin/bash

# Cds into reports
cd reports/

# 1.
# Sorts by last modification. Because file doesn't change after creation,
# we can assume that last modification date equals creation date
lastReport=$(ls -t report_*.csv | head -n1)
echo Last report: $lastReport
while read user; do
	echo $user: $(cat $lastReport | cut -d ";" -f1 | grep "\b$user\b" | wc -l)
done <<< "$(cat $lastReport | cut -d ";" -f1 | sort -u)"

# 2.
read -p "Inserire utente: " user

# Check if user exists
id -u $user >/dev/null 2>&1
if [ $? -eq 0 ]; then
	for file in $(ls -t report_*.csv); do
		echo report: $file processi $(cat $file | cut -d ";" -f1 | grep "\b$user\b" | wc -l)
	done
else
	echo $user non esiste
fi

exit 0
