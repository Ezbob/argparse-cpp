CXXFLAGS=-std=c++11 -g -Wall -Wextra -Wfatal-errors

SRCS=$(wildcard *.cpp)
HDRS=$(wildcard *.hpp) $(wildcard *.h)
OBJS=$(patsubst %.cpp,%.o,$(SRCS))
TARGET=argparse

$(TARGET): $(OBJS) $(HDRS)
	$(CXX) $(CXXFLAGS) -o $@ $^

.PHONY: clean
clean:
	$(RM) $(OBJS) $(TARGET)
