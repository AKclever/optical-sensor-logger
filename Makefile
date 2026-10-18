CC = cc
CPPFLAGS += -D_POSIX_C_SOURCE=200809L
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g

TARGET = optlog
OBJECTS = main.o sensor.o logger.o

.PHONY: all clean format
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

main.o: main.c sensor.h logger.h
sensor.o: sensor.c sensor.h
logger.o: logger.c logger.h

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

format:
	clang-format -i main.c sensor.c sensor.h logger.c logger.h

clean:
	$(RM) $(TARGET) $(OBJECTS)
