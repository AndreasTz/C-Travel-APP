all: travelMonitor monitor

travelMonitor: travelMonitor.c bloom.c
	gcc travelMonitor.c bloom.c -o travelMonitor

monitor: monitor.c skipList.c citizen.c bloom.c date.c request.c
	gcc monitor.c skipList.c citizen.c bloom.c date.c request.c -o monitor

