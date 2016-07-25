
class MoveGenerator
{
public:
    //Common routines
    MoveGenerator(int * position, int * positionalParameters, int * allMoves);
    ~MoveGenerator(void);
    int getArraySize(void);
    //Chess routines
    void shuffleMoves(int * previousRootNodes, int * refutationTable, int * pieceValues, int depth);
    bool attackOnKing(int direction);
    bool revealedAttackOnKing(int direction);
    bool enPassantPin(int firstPawnSquare, int secondPawnSquare);
    void enPassant(void);
    void determinePin(int defendingSquare, int direction);
    void whiteCastling(int direction, int toSquare, int rookSquare);
    void blackCastling(int direction, int toSquare, int rookSquare);
    void getLegalMoves(void);
    void getAllMoves(void);
    void getCheckMoves(void);
    void line(int pieceSquare, int targetSquare, int direction);
    void whitePawnMove(int pieceLocaton);
    void blackPawnMove(int pieceLocation);
    void whitePinnedPawnCapture(int pieceLocation, int nextMove);
    void blackPinnedPawnCapture(int pieceLocation, int nextMove);
    void whitePawnAdvance(int pieceLocation);
    void blackPawnAdvance(int pieceLocation);
    void whiteKnightMove(int pieceLocation);
    void blackKnightMove(int pieceLocation);
    void whiteBishopMove(int pieceLocation);
    void blackBishopMove(int pieceLocation);
    void whiteRookMove(int pieceLocation);
    void blackRookMove(int pieceLocation);
    void whiteQueenMove(int pieceLocation);
    void blackQueenMove(int pieceLocation);
    void whiteSlidingMove(int pieceLocation, int pieceStatus);
    void blackSlidingMove(int pieceLocation, int pieceStatus);
    void whiteKingMove(void);
    void blackKingMove(void);
    void displayPosition(void);
    void displayPositionalInformation(void);
    //int SEE(int fromSquare, int captureSquare);
    
private:
    int* position;
    int* positionalInformation;
    int* allMoves;
    //int* killerMoves;
    const int* pieceOrder;
    int pieceStatuses[64];
    int whiteTurn;
    int turnSwitch;
    int currentPiece;
    int kingSquare;
    int arraySize;
    int lastFromSquare;
    int lastToSquare;
    int castlingRights;
    int attackingSquare;
    int attackDirection;
};
