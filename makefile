TARGET = reverseGof

SRCDIR = ./src/
INCDIR = ./include/

CC = g++
CFILES = $(foreach D, $(SRCDIR), $(wildcard $(D)*.cpp))
CFLAGS = -Wall -lz3
LDFLAGS = $(foreach D, $(INCDIR), -I$(D))
OBJFILES = $(patsubst %.cpp, %.o, $(CFILES))

all: $(TARGET)

$(TARGET) : $(OBJFILES)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o : %.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $^

clean:
	rm -f $(TARGET) $(OBJFILES)

# Debug target
debug: CFLAGS += -DDEBUG -g
debug: clean $(TARGET)
