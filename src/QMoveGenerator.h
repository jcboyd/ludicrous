
class QMoveGenerator
{
public:
    //Common routines
    QMoveGenerator(int* position, int* positionalParameters, int* allCaptures);
    ~QMoveGenerator(void);
    int getArraySize(void);
    //Chess routines
    void shuffleMoves(int * pieceValues, int currentDepth);
    bool attackOnKing(int direction);
    bool revealedAttackOnKing(int direction);
    void determinePin(int defendingSquare, int direction);
    void line(int pieceSquare, int targetSquare, int direction);
    void getLegalCaptures(void);
    void getAllCaptures(void);
    void getCheckMoves(void);
    bool enPassantPin(int firstPawnSquare, int secondPawnSquare);
    void enPassant(void);
    void whitePawnCapture(int pieceLocaton);
    void blackPawnCapture(int pieceLocation);
    void whitePinnedPawnCapture(int pieceLocation, int nextMove);
    void blackPinnedPawnCapture(int pieceLocation, int nextMove);
    void whiteKnightCapture(int pieceLocation);
    void blackKnightCapture(int pieceLocation);
    void whiteBishopCapture(int pieceLocation);
    void whiteRookCapture(int pieceLocation);
    void whiteQueenCapture(int pieceLocation);
    void blackBishopCapture(int pieceLocation);
    void blackRookCapture(int pieceLocation);
    void blackQueenCapture(int pieceLocation);
    void whiteKingCapture(void);
    void blackKingCapture(void);
    void whiteKingMove(void);
    void blackKingMove(void);
    void displayPosition(void);
    void displayPositionalInformation(void);
    void whiteSlidingCapture(int pieceLocation, int status);
    void blackSlidingCapture(int pieceLocation, int status);
    //int SEE(int fromSquare, int toSquare);
    
private :
    int * position;
    int * positionalInformation;
    int * allCaptures;
    int pieceStatuses[64];
    const int * pieceOrder;
    int kingSquare;
    int whiteTurn;
    int turnSwitch;
    int currentPiece;
    int lastFromSquare;
    int lastToSquare;
    int arraySize;
    int attackingSquare;
    int attackDirection;
    
};
