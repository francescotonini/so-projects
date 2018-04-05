#!/bin/bash

function startMonitor {
	pid=$(pgrep -f "bash recorder.bash")
	if [ $? -gt 0 ]; then
		# Launch recorder on a new session
		setsid bash recorder.bash >/dev/null 2>&1 &
		echo Monitoraggio avviato: $!
	else
		echo Monitoraggio già in esecuzione
	fi
}

function stopMonitor {
	pid=$(pgrep -f "bash recorder.bash")
	if [ $? -eq 0 ]; then
		kill -TERM -$pid
		wait $pid 2>/dev/null
		echo Monitoraggio terminato: $pid
	else
		echo Monitoraggio non in esecuzione
	fi
}

while true; do
	echo "Monitoraggio server. Comandi disponibili:"
	echo "1) Inizia monitoraggio"
	echo "2) Ferma monitoraggio"
	echo "3) Stampa info utilizzo"
	echo "4) Esci"
	echo "5) Cancella report vecchi"

	read selection
	case $selection in
		1) startMonitor;;
		2) stopMonitor;;
		3) bash query.bash;;
		4) exit 0;;
		5) bash checkSpace.bash;;
	esac
done;
