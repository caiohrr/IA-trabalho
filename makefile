TARGET = reverseGof

SRCDIR = ./src/
INCDIR = ./include/

CC = g++
CFILES = $(wildcard $(SRCDIR)*.cpp)
CFLAGS = -Wall $(foreach D, $(INCDIR), -I$(D))
LDFLAGS = -lz3
OBJFILES = $(patsubst %.cpp, %.o, $(CFILES))

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -f $(TARGET) $(OBJFILES)

# Debug target
debug: CFLAGS += -DDEBUG -g
debug: clean $(TARGET)
