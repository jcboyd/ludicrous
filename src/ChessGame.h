#include <string>


class ChessGame
{
public:
    ChessGame();
    ~ChessGame(void);
    void initialiseBoard(void);
    int* getBoard(void);
    int* getPositionalInformation(void);
    int* getPieceValues(void);
    void setPosition(std::string fen, int lastFromSquare, int lastToSquare);
    bool isSpecialMove(int fromSquare, int toSquare);
    void makeMove(int toSquare, int fromSquare);
    int translateSquare(std::string userMove, bool isToSquare);
    bool findMove(int fromSquare, int toSquare);
    void displayPosition(void);
    void displayFlippedPosition(void);
    void displayPieceValues(void);
    void displayPositionalInformation(void);
    bool isWhiteTurn(void);
    bool queensOff(void);
    void displayAll(void);
    
private:
    int chessBoard[128];
    int positionalInformation[128];
    int pieceValues[128];
    int currentEvaluation;
};
