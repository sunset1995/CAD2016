CC     = g++
CFLAGS = -O2 -std=c++11

SRCS      = $(wildcard sources/*.cpp)
OBJS      = $(addprefix build/,$(notdir $(SRCS:.cpp=.o)))

CAD2016: $(OBJS)
	$(CC) $(CFLAGS) -o build/CAD2016 $^

build/%.o: sources/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm build/*
