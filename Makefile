CFLAGS := -Wall
INCLUDES := -I../include
LDFLAGS := -L../libs -lnxsresmanager -lnxsv4l2testcommon -lnxsv4l2 -lnxdrm
LIBS :=

CROSS_COMPILE ?= arm-linux-gnueabi-
CC := $(CROSS_COMPILE)gcc

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

TARGET := test-vipclipper-dmaw

.c.o:
	$(CC) $(INCLUDES) $(CFLAGS) -c $^

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

install: $(TARGET)
	cp $^ ../bin

all: $(TARGET)

.PHONY: clean

clean:
	rm -f *.o
	rm -f $(TARGET)
