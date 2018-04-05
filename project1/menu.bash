#!/bin/bash

function menu {
	echo "Monitoraggio server. Comandi disponibili:"
	echo "1) Inizia monitoraggio"
	echo "2) Ferma monitoraggio"
	echo "3) Stampa info utilizzo"
	echo "4) Esci"
	echo "5) Cancella report vecchi"

	read selection
	case $selection in
		1) startMonitor && menu;;
		2) stopMonitor && menu;;
		3) bash query.bash;;
		4) exit 0;;
		5) bash checkSpace.bash && menu;;
		*) menu;;
	esac
}

function startMonitor {
	pid=$(pgrep -f "bash recorder.bash")
	if [ $? -gt 0 ]; then
		bash recorder.bash >/dev/null 2>&1 &
		echo "Monitoraggio avviato"
	fi
}

function stopMonitor {
	pid=$(pgrep -f "bash recorder.bash")
	if [ $? -eq 0 ]; then
		kill -TERM $pid
		wait $pid 2>/dev/null
		echo "Monitoraggio terminato ($pid)"
	fi
}

menu
