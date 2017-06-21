all: dataConsumer dataCollector

dataConsumer: dataConsumer.c extractStrides.c extractFeatures.c decision_maker.c
	gcc -g -o dataConsumer dataConsumer.c extractStrides.c extractFeatures.c decision_maker.c -lm -lfann

dataCollector: dataCollector.c
	gcc -lmraa -o dataCollector dataCollector.c LSM9DS0.c

clean:
	rm -rf dataConsumer *~
	rm -rf dataCollector *~
	rm -rf data.csv
