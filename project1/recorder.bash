#!/bin/bash
PAUSE_FOR=120 # seconds

# Creates the folder "reports" if it doesn't already exist
mkdir -p reports
cd reports

while true; do
	filename="report_$(date +%Y%m%d_%H%M%S).csv"
	while read line; do
		u=$(echo $line | cut -d " " -f1) # User
		p=$(echo $line | cut -d " " -f2) # PID
		s=$(echo $line | cut -d " " -f3) # Start
		t=$(echo $line | cut -d " " -f4) # Time
		c=$(echo $line | cut -d " " -f5) # Command
		
		lotc=$(echo $s | wc -m) # Length Of Third Column
		if [ "$lotc" = "4" ]; then # Check if third column contains the month
			s=$(echo $line | cut -d " " -f3-4) # Day of month
			t=$(echo $line | cut -d " " -f5) # Time
			c=$(echo $line | cut -d " " -f6) # Command
		fi

		echo "$u;$p;$c;$s;$t" >> $filename
	done <<< "$(ps --no-headers -eo "user:32 pid start time command" | tr -s " ")"

	echo Done! Waiting $PAUSE_FOR seconds until next execution
	sleep $PAUSE_FOR
done
