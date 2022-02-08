CFLAGS=-O2 -Wall

.PHONY = clean

all: txtmap.o persistente.o stringman.o plugin.o
	$(CXX) $(CFLAGS) -otxtmap txtmap.o persistente.o stringman.o plugin.o

plugin.o: plugin.cpp plugin.h
	$(CXX) $(CFLAGS) -c plugin.cpp -oplugin.o

txtmap.o: txtmap.cpp txtmap.h
	$(CXX) $(CFLAGS) -c txtmap.cpp -otxtmap.o

persistente.o: persistente.cpp persistente.h
	$(CXX) $(CFLAGS) -c persistente.cpp -opersistente.o

stringman.o: stringman.cpp stringman.h
	$(CXX) $(CFLAGS) -c stringman.cpp -ostringman.o

clean:
	$(RM) *.o
	$(RM) txtmap
	$(RM) txtmap.exe
