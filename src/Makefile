CC=gcc
DEPS=-lrt -lm
SOURCES= flir2temp.c pnm.c
EXECUTABLE= flir2temp

$(EXECUTABLE): $(SOURCES) $(DEPS)
	$(CC) -o $@ $^

.PHONY: clean

clean:
	rm -f *.o *~ $(EXECUTABLE)
