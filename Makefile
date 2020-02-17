#
CC = gcc
CXX = g++
CFLAGS = -g -c -I/usr/include/hidapi
CXXFLAGS = -c -Wall -O1 -I/usr/include/hidapi
LFLAGS =
LIBS = -L/usr/lib/arm-linux-gnueabihf -lhidapi-libusb
LINK = g++

#
SRCS = usbrh-usblib-hidapi.cpp main.cpp
OBJS=$(SRCS:.cpp=.o)
TARGET = usbrh-usblib-hidapi

#
all: $(TARGET)

$(TARGET): Makefile $(OBJS)
	$(LINK) -o $(TARGET) $(LFLAGS) $(LIBS) $(OBJS)

clean:
	-rm $(TARGET) $(OBJS)

.cpp.o:
	$(CXX) $(CXXFLAGS) $<

#
