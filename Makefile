CXXFLAGS=-std=c++11 -g -Wall -Wextra -Wfatal-errors

SRCS=$(wildcard *.cpp)
HDRS=$(wildcard *.hpp) $(wildcard *.h)
OBJS=$(patsubst %.cpp,%.o,$(SRCS))
TARGET=argparse

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS) $(HDRS)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	$(RM) $(OBJS) $(TARGET)
