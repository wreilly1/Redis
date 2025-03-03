CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -pthread

TARGET   := myredis

SOURCES  := main.cpp \
            src/Server.cpp \
            src/Database.cpp \
            src/CommandParser.cpp \
            src/Logger.cpp \
            src/ThreadPool.cpp


OBJECTS  := $(SOURCES:.cpp=.o)


all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


run: $(TARGET)
	./$(TARGET) --port 6379


clean:
	rm -f $(OBJECTS) $(TARGET)


.PHONY: all clean run
