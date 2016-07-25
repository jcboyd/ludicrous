CXX = g++
CXXFLAGS = -Wall -g
BIN=./
SOURCE=./src/

ludicrous: main.o ChessGame.o \
			 MoveGenerator.o \
			 QMoveGenerator.o \
			 Search.o \
			 IterativeDeepener.o
	$(CXX) $(CXXFLAGS) -o ludicrous $(BIN)main.o $(BIN)ChessGame.o \
		$(BIN)MoveGenerator.o $(BIN)QMoveGenerator.o $(BIN)Search.o $(BIN)IterativeDeepener.o

main.o: $(SOURCE)main.cpp $(SOURCE)ChessGame.h $(SOURCE)Globals.h
	$(CXX) $(CXXFLAGS) -c $(SOURCE)main.cpp -o $(BIN)main.o

MoveGenerator.o: $(SOURCE)MoveGenerator.h
	$(CXX) $(CXXFLAGS) -c $(SOURCE)MoveGenerator.cpp -o $(BIN)MoveGenerator.o

QMoveGenerator.o: $(SOURCE)QMoveGenerator.h
	$(CXX) $(CXXFLAGS) -c $(SOURCE)QMoveGenerator.cpp -o $(BIN)QMoveGenerator.o

Search.o: $(SOURCE)Search.h $(SOURCE)MoveGenerator.h $(SOURCE)QMoveGenerator.h
	$(CXX) $(CXXFLAGS) -c $(SOURCE)Search.cpp -o $(BIN)Search.o

IterativeDeepener.o: $(SOURCE)IterativeDeepener.h $(SOURCE)Search.h
	$(CXX) $(CXXFLAGS) -c $(SOURCE)IterativeDeepener.cpp -o $(BIN)IterativeDeepener.o

ChessGame.o: $(SOURCE)ChessGame.h $(SOURCE)IterativeDeepener.h
	$(CXX) $(CXXFLAGS) -c $(SOURCE)ChessGame.cpp -o $(BIN)ChessGame.o