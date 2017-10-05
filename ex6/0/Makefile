# program execcutable name
TARGET = io_test_blink

COMPILER = gcc
# compiler flags
CFLAGS = -g -Wall

# linker flags
LDFLAGS = -lcomedi -lm

# list of sources 
SOURCES = $(shell find -name "*.c")

# default rule, to compile everything
all: $(TARGET)

# define object files 
OBJECTS = $(SOURCES:.c=.o)

# link programs
$(TARGET): $(OBJECTS)
	$(COMPILER) $^ $(LDFLAGS) -o $@ 
	
# compile
%.o : %.c
	$(COMPILER) $< $(CFLAGS) -c -o $@ 
	
# cleaning 
clean:
	rm -f $(TARGET) $(OBJECTS)
