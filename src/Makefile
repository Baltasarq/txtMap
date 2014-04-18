cc=g++

all: txtmap.o persistente.o stringman.o plugin.o
	$(cc) -otxtmap txtmap.o persistente.o stringman.o plugin.o

plugin.o: plugin.cpp plugin.h
	$(cc) -c plugin.cpp

txtmap.o: txtmap.cpp txtmap.h
	$(cc) -c txtmap.cpp

persistente.o: persistente.cpp persistente.h
	$(cc) -c persistente.cpp

stringman.o: stringman.cpp stringman.h
	$(cc) -c stringman.cpp

