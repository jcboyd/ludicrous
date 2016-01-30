#include "MoveGenerator.h"
#include "QMoveGenerator.h"

class search
{
public:
    search(int* initialPosition, int* initialPositionalInformation, int* pieceValues, int depth, int lastEvalaution, bool endgame);
    ~search(void);
    void initialisePieceSquareTables(bool endgame);
    void setPrincipalVariation(int* lastPrincipalVariation);
    int initiateSearch(void);
    int maxFastEvaluate(int fromSquare, int toSquare);
    int minFastEvaluate(int fromSquare, int toSquare);
    int treeSearchMax2(int depth, int maxBest, int minBest);
    int treeSearchMin2(int depth, int maxBest, int minBest);
    int treeSearchMax(int depth, int maxBest, int minBest);
    int treeSearchMin(int depth, int maxBest, int minBest);
    void updatePVTable(int depth, int fromSquare, int toSquare);
    void updateRefutationTable(int depth, int fromSquare, int toSquare);
    int qSearchMax(int maxBest, int minBest);
    int qSearchMin(int maxBest, int minBest);
    void maxPlayMove(int fromSquare, int toSquare);
    void minPlayMove(int fromSquare, int toSquare);
    void maxInvPlayMove(int fromSquare, int toSquare);
    void minInvPlayMove(int fromSquare, int toSquare);
    void playQMove(int fromSquare, int toSquare, int capture);
    void invPlayQMove(int fromSquare, int toSquare, int capture);
    int getBestFromMove(void);
    int getBestToMove(void);
    long getNumberOfEvals(void);
    long getNumberOfQEvals(void);
    long getNumberOfGeneratedMoves(void);
    long getNumberOfQGeneratedMoves(void);
    void displayPosition(void);
    void displayPVTable(void);
    void displayPieceValues(void);
    int* getPVTable(void);
    bool checkCurrentVariation(int fromSquare, int toSquare, int moveIndex);
    void printMove(int fromSquare, int toSquare);
    void maxPlaySpecialMove(int fromSquare, int toSquare);
    void minPlaySpecialMove(int fromSquare, int toSquare);
    void maxInvPlaySpecialMove(int fromSquare, int toSquare);
    void minInvPlaySpecialMove(int fromSquare, int toSquare);
    void displayPositionalInformation(void);
    void displayRefutationTable(void);
    void setPreviousRootNodes(int* previousRootNodes);
    void recordRootNodes(int fromSquare, int toSquare, int evaluation);
    void copyRootNodes(int * copyRootNodes);
    void displayQMovesCounts(void);
    void displayAll(void);
    void displayCurrentVariation(int depth);
    
private:
    int searchDepth;
    int* position;
    int* positionalInformation;
    int* pieceValues;
    const int * whitePieceSquareTables[8];
    const int * blackPieceSquareTables[8];
    const int * currentPieceSquareTables;
    int captures[128];
    int specials[128];
    int pawnPromotions[128];
    int positionalEvaluations[128];
    int currentDepth;
    int currentEvaluation;
    int debuggingCounter;
    long numberOfGeneratedMoves;
    long numberOfQGeneratedMoves;
    long numberOfEvals;
    long numberOfQEvals;
    int PVTable[512];
    int refutationTable[512];
    long lastNumberOfEvals;
    int currentVariation[128];
    int previousRootNodes[1024];
    int rootNodes[1024];
    int rootNodesRecorded;
    int qMovesCounts[128];
    //int hhTable[128][128];
};
