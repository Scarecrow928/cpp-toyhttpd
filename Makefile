CXX=g++
CXXFLAGS=-g -std=c++17 -Wall -I.
DIR=build
BIN=toyhttpd

all: directories $(BIN) 

$(DIR)/request.o: request.cpp
	$(CXX) $(CXXFLAGS) $(DEBUG) -c -o $(DIR)/request.o request.cpp

$(DIR)/myutil.o: myutil.cpp
	$(CXX) $(CXXFLAGS) $(DEBUG) -c -o $(DIR)/myutil.o myutil.cpp

$(DIR)/response.o: response.cpp
	$(CXX) $(CXXFLAGS) $(DEBUG) -c -o $(DIR)/response.o response.cpp 

$(DIR)/requesthandler.o: requesthandler.cpp 
	$(CXX) $(CXXFLAGS) $(DEBUG) -c -o $(DIR)/requesthandler.o requesthandler.cpp 

$(DIR)/toyserver.o: toyserver.cpp 
	$(CXX) $(CXXFLAGS) $(DEBUG) -c -o $(DIR)/toyserver.o toyserver.cpp

$(DIR)/main.o: main.cpp 
	$(CXX) $(CXXFLAGS) $(DEBUG) -c -o $(DIR)/main.o main.cpp

$(BIN): $(DIR)/main.o $(DIR)/toyserver.o $(DIR)/requesthandler.o \
				$(DIR)/response.o $(DIR)/myutil.o $(DIR)/request.o
	$(CXX) $(CXXFLAGS) $(DEBUG) -pthread -o $(BIN) $(DIR)/main.o \
		$(DIR)/toyserver.o $(DIR)/requesthandler.o $(DIR)/response.o \
		$(DIR)/myutil.o $(DIR)/request.o

.PHONY: directories
directories:
	mkdir -p $(DIR)

.PHONY: debug	
debug: DEBUG = -DDEBUG

debug: all

.PHONY: clean
clean:
	rm -rf $(DIR) && rm -f $(BIN)