CC=gcc
DEPS=-lrt -lm
SOURCES= flir_tool.c
EXECUTABLE= flir_tool

$(EXECUTABLE): $(SOURCES) $(DEPS)
	$(CC) -o $@ $^

.PHONY: clean

clean:
	rm -f *.o *~ $(EXECUTABLE)
