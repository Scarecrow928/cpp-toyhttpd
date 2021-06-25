CXX=g++
CXXFLAGS=-g -std=c++17 -Wall
BIN=toyhttpd

all: $(BIN)

toyhttpd: main.cpp toyserver.cpp toyserver.h
	$(CXX) $(CXXFLAGS) $(DEBUG) -pthread -o $(BIN) main.cpp \
		toyserver.h toyserver.cpp \
		requesthandler.h requesthandler.cpp \
		request.h request.cpp \
		response.h response.cpp \
		myutil.h myutil.cpp
		
debug: DEBUG = -DDEBUG

debug: all

clean:
	rm -f $(BIN)