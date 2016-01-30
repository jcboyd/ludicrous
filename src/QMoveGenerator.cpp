#include "QMoveGenerator.h"

qMoveGenerator::qMoveGenerator(int* position, int* positionalInformation, int* allCaptures)
{
    qMoveGenerator::position = position;
    qMoveGenerator::positionalInformation = positionalInformation;
    
    whiteTurn = positionalInformation[WHITE_TURN];
    turnSwitch = (-1 + 2*whiteTurn);
    
    lastFromSquare = positionalInformation[LAST_FROM_SQUARE];
    lastToSquare = positionalInformation[LAST_TO_SQUARE];
    
    qMoveGenerator::allCaptures = allCaptures;
    arraySize = 0;
}

qMoveGenerator::~qMoveGenerator(void) {}

int qMoveGenerator::getArraySize(void) 
{
    return qMoveGenerator::arraySize;
}

//Support functions

bool qMoveGenerator::attackOnKing(int direction)
{
    int searchSquare = lastToSquare + direction;
    while(searchSquare != kingSquare)
    {
        if(position[searchSquare] != 0) return false;
        searchSquare += direction;
    }
    attackingSquare = lastToSquare;
    attackDirection = direction;
    return true; //King square reached without obstruction, hence check.
}

bool qMoveGenerator::revealedAttackOnKing(int direction)
{
    int boardSquare, squareValue, pieceType;
    boardSquare = kingSquare;
    while(true)
    {
        boardSquare += direction;
        if((136&boardSquare) == 0)
        {
            squareValue = position[boardSquare];
            if(turnSwitch*squareValue > 0) return false;
            else if(turnSwitch*squareValue < 0)
            {
                if(abs(direction) == 15 || abs(direction) == 17)
                {
                    pieceType = positionalInformation[abs(squareValue) + 64];
                    if(pieceType == BISHOP || pieceType == QUEEN) //Bishop or queen
                    {
                        attackingSquare = boardSquare;
                        attackDirection = -direction; //Direction from king is reverse
                        return true;
                    }
                    else return false;
                }
                else
                {
                    pieceType = positionalInformation[abs(squareValue) + 64]; //pieceLists
                    if(pieceType == ROOK || pieceType == QUEEN) //Rook or queen
                    {
                        attackingSquare = boardSquare;
                        attackDirection = -direction; //Direction from king is reverse
                        return true;
                    }
                    else return false;
                }
            }
        }
        else return false;
    }
}

void qMoveGenerator::determinePin(int defendingSquare, int searchIndex)
{
    int boardSquare, squareValue, pieceType;
    boardSquare = defendingSquare;
    while(true)
    {
        boardSquare += SEARCH_SQUARES[searchIndex];
        if((136&boardSquare) == 0)
        {
            squareValue = turnSwitch*position[boardSquare];
            if(squareValue > 0) break;
            else if(squareValue < 0)
            {
                pieceType = positionalInformation[-squareValue + 64];
                if(pieceType == ATTACKERS1[searchIndex] || pieceType == ATTACKERS2[searchIndex])
                {
                    pieceStatuses[abs(position[defendingSquare])] = abs(SEARCH_SQUARES[searchIndex]);
                }
                break;
            }
        }
        else break;
    }
}

void qMoveGenerator::line(int pieceSquare, int targetSquare, int direction)
{
    int searchSquare = pieceSquare;
    while(true)
    {
        searchSquare += direction;
        if(searchSquare == targetSquare)
        {
            allCaptures[arraySize] = pieceSquare;
            allCaptures[arraySize + 1] = targetSquare;
            arraySize += 2;
            break;
        }
        if(position[searchSquare] != 0) break;
    }
}

//Principal move manager

void qMoveGenerator::getLegalCaptures(void)
{
    bool directCheck, revealedCheck;
    int toDiff, fromDiff, lastPieceType, boardSquare, i;
    
    directCheck = false;
    revealedCheck = false;
    
    //Initialise piece parameters
    if(whiteTurn)
    {
        pieceOrder = WHITE_Q_PIECE_ORDER;
        kingSquare = positionalInformation[positionalInformation[WHITE_KING_INDEX]];
        lastPieceType = positionalInformation[-position[lastToSquare] + 64];
    }
    else
    {
        pieceOrder = BLACK_Q_PIECE_ORDER;
        kingSquare = positionalInformation[positionalInformation[BLACK_KING_INDEX]];
        lastPieceType = positionalInformation[position[lastToSquare] + 64];
    }
    //Search for direct check:
    fromDiff = lastFromSquare - kingSquare;
    switch(lastPieceType)
    {
        case PAWN : //Pawn
            toDiff = turnSwitch*(lastToSquare - kingSquare);
            if(toDiff == 15 || toDiff == 17)
            {
                attackingSquare = lastToSquare;
                attackDirection = -turnSwitch*toDiff;
                directCheck = true;
            }
            if(lastFromSquare < 0)
            {
                if(! directCheck)
                {
                    fromDiff = -(kingSquare + lastFromSquare);
                    if((fromDiff%15) == 0)
                    {
                        directCheck = revealedAttackOnKing(abs(fromDiff)/(fromDiff/15));
                    }
                    else if((fromDiff%16) == 0)
                    {
                        directCheck = revealedAttackOnKing(abs(fromDiff)/(fromDiff/16));
                    }
                    else if((fromDiff%17) == 0)
                    {
                        directCheck = revealedAttackOnKing(abs(fromDiff)/(fromDiff/17));
                    }
                    else if(abs(fromDiff) < 7)
                    {
                        directCheck = revealedAttackOnKing(abs(fromDiff)/(fromDiff));
                    }
                }
                toDiff = lastToSquare + turnSwitch*16 - kingSquare; //Examine toSquare of captured pawn
                if((toDiff%15) == 0)
                {
                    revealedCheck = revealedAttackOnKing(abs(toDiff)/(toDiff/15));
                }
                else if((toDiff%17) == 0)
                {
                    revealedCheck = revealedAttackOnKing(abs(toDiff)/(toDiff/17));
                }
                fromDiff = 18;
            }
            break;
        case KNIGHT : //Knight
            toDiff = abs(lastToSquare - kingSquare);
            if(toDiff == 14 || toDiff == 18 || toDiff == 31 || toDiff == 33)
            {
                attackingSquare = lastToSquare;
                attackDirection = toDiff;
                directCheck = true;
            }
            break;
        case BISHOP : //Bishop
            toDiff = kingSquare - lastToSquare;
            if(toDiff%15 == 0) //Enemy bishop may be attacking along left diagonal
            {
                directCheck = attackOnKing(abs(toDiff)/(toDiff/15)); 
            }
            else if(toDiff%17 == 0) //Enemy bishop may be attacking along right diagonal
            {
                directCheck = attackOnKing(abs(toDiff)/(toDiff/17));
            }
            break;
        case ROOK : //Rook
            toDiff = kingSquare - lastToSquare;
            if(toDiff%16 == 0) //Enemy rook may be attacking along file
            {
                directCheck = attackOnKing(abs(toDiff)/(toDiff/16));
            }
            else if(abs(toDiff) < 8) //Enemy rook may be attacking along rank
            {
                directCheck = attackOnKing(abs(toDiff)/toDiff);
            }
            break;
        case QUEEN : //Queen
            toDiff = kingSquare - lastToSquare;
            if(toDiff%15 == 0) //Enemy queen may be attacking along left diagonal
            {
                directCheck = attackOnKing(abs(toDiff)/(toDiff/15));
            }
            else if(toDiff%16 == 0) //Enemy queen may be attacking along file
            {
                directCheck = attackOnKing(abs(toDiff)/(toDiff/16));
            }
            else if(toDiff%17 == 0) //Enemy queen may be attacking along right diagonal
            {
                directCheck = attackOnKing(abs(toDiff)/(toDiff/17));
            }
            else if(abs(toDiff) < 8) //Enemy queen may be attacking along rank
            {
                directCheck = attackOnKing(abs(toDiff)/toDiff);
            }
            if(lastFromSquare < 0) //Pawn promotion
            {
                fromDiff = -lastFromSquare - kingSquare;
            }
            break;
        case KING :
            toDiff = kingSquare - lastToSquare;
            if(lastFromSquare < 0) //Castle
            {
                fromDiff = (lastToSquare - lastFromSquare)/2 - kingSquare;
                toDiff = lastToSquare + 16; //Rig toDiff
            }
            break;
    }
    //Search for revealed check
    if(fromDiff%15 == 0) //Potential attack from along left diagonal
    {
        if(toDiff%15 != 0) revealedCheck = revealedAttackOnKing(abs(fromDiff)/(fromDiff/15));
    }
    else if(fromDiff%16 == 0) //Potential attack along file
    {
        if(toDiff%16 != 0) revealedCheck = revealedAttackOnKing(abs(fromDiff)/(fromDiff/16));
    }
    else if(fromDiff%17 == 0) //Potential attack along right diagonal
    {
        if(toDiff%17 != 0) revealedCheck = revealedAttackOnKing(abs(fromDiff)/(fromDiff/17));
    }
    else if(abs(fromDiff) < 7) //Potential attack along rank
    {
        if(abs(toDiff) > 7) revealedCheck = revealedAttackOnKing(abs(fromDiff)/fromDiff);
    }
    //Initialise piece status array
    for(i = 0; i < 16; i++)
    {
        if(positionalInformation[pieceOrder[i]] >= 0) pieceStatuses[pieceOrder[i]] = 0;
        else pieceStatuses[pieceOrder[i]] = -1; //Capture status
    }
    //Find pinned pieces
    for(i = 0; i < 8; i++)
    {
        boardSquare = kingSquare;
        while(true)
        {
            boardSquare += SEARCH_SQUARES[i]; //Search along current ray
            if((136&boardSquare) == 0) //Move is on board
            {
                if(turnSwitch*position[boardSquare] > 0) //Defending piece found
                {
                    determinePin(boardSquare, i);
                    break;
                }
                else if(turnSwitch*position[boardSquare] < 0) break;
            }
            else break;
        }
    }
    //Generate moves based on king safety
    if(!directCheck && !revealedCheck)
    {
        if(lastPieceType == 0 && abs(lastToSquare - lastFromSquare) == 32) enPassant();
        positionalInformation[IN_CHECK] = 0;
        getAllCaptures();
    }
    else if(directCheck && revealedCheck) //King in double check
    {
        positionalInformation[IN_CHECK] = 1;
        if(whiteTurn) whiteKingMove();
        else blackKingMove();
    }
    else
    {
        if(directCheck)
        {
            if(lastPieceType == 0 && abs(lastToSquare - lastFromSquare) == 32) enPassant();
        }
        positionalInformation[IN_CHECK] = 1;
        getCheckMoves();
    }
}

//More generation managers

void qMoveGenerator::getAllCaptures(void)
{
    //Looping variables
    int status;
    int pieceType;
    if(whiteTurn) //White to play
    {
        for(int pieceStatusIndex = 0; pieceStatusIndex < 16; pieceStatusIndex++)
        {
            currentPiece = qMoveGenerator::pieceOrder[pieceStatusIndex];
            pieceType = positionalInformation[currentPiece + 64];
            switch(pieceType)
            {
                case PAWN : //Pawn
                    switch(pieceStatuses[currentPiece])
                    {
                        case 0 :
                            whitePawnCapture(positionalInformation[currentPiece]);
                            break;
                        case -1 :
                            break;
                        case 15 :
                            whitePinnedPawnCapture(positionalInformation[currentPiece], positionalInformation[currentPiece] + 15);
                            break;
                        case 16 :
                            break;
                        case 17 :
                            whitePinnedPawnCapture(positionalInformation[currentPiece], positionalInformation[currentPiece] + 17);
                            break;
                    }
                    break;
                case KNIGHT : //Knight
                    if(pieceStatuses[currentPiece] == 0) whiteKnightCapture(positionalInformation[currentPiece]);
                    break;
                case BISHOP : //Bishop
                    status = pieceStatuses[currentPiece];
                    switch(status)
                    {
                        case 0 :
                            whiteBishopCapture(positionalInformation[currentPiece]);
                            break;
                        case -1 :
                            break;
                        case 15 :
                        case 17 :
                            whiteSlidingCapture(positionalInformation[currentPiece], status);
                            break;
                        break;
                    }
                    break;
                case ROOK : //Rook
                    status = pieceStatuses[currentPiece];
                    switch(status)
                    {
                        case 0 :
                            whiteRookCapture(positionalInformation[currentPiece]);
                            break;
                        case -1 :
                            break;
                        case 1 :
                        case 16 :
                            whiteSlidingCapture(positionalInformation[currentPiece], status);
                            break;
                    }
                    break;
                case QUEEN : //Queen
                    status = pieceStatuses[currentPiece];
                    switch(status)
                    {
                        case 0 :
                            whiteQueenCapture(positionalInformation[currentPiece]);
                            break;
                        case -1 :
                            break;
                        default :
                            whiteSlidingCapture(positionalInformation[currentPiece], status);
                            break;
                    }
                    break;
                case KING : //King
                    whiteKingCapture();
                    break;
            }
        }
    }
    else //Black turn
    {
        for(int pieceStatusIndex = 0; pieceStatusIndex < 16; pieceStatusIndex++)
        {
            currentPiece = qMoveGenerator::pieceOrder[pieceStatusIndex];
            pieceType = positionalInformation[currentPiece + 64];
            switch(pieceType)
            {
                case PAWN : //Pawn
                    switch(pieceStatuses[currentPiece])
                    {
                        case 0 :
                            blackPawnCapture(positionalInformation[currentPiece]);
                            break;
                        case -1 :
                            break;
                        case 15 :
                            blackPinnedPawnCapture(positionalInformation[currentPiece], positionalInformation[currentPiece] - 15);
                            break;
                        case 16 :
                            break;
                        case 17 :
                            blackPinnedPawnCapture(positionalInformation[currentPiece], positionalInformation[currentPiece] - 17);
                            break;
                    }
                    break;
                case KNIGHT : //Knight
                    if(pieceStatuses[currentPiece] == 0) blackKnightCapture(positionalInformation[currentPiece]);
                    break;
                case BISHOP : //Bishop
                    status = pieceStatuses[currentPiece];
                    switch(status)
                    {
                        case 0 :
                            blackBishopCapture(positionalInformation[currentPiece]);
                            break;
                        case -1 :
                            break;
                        case 15 :
                        case 17 :
                            blackSlidingCapture(positionalInformation[currentPiece], status);
                            break;
                    }
                    break;
                case ROOK : //Rook
                    status = pieceStatuses[currentPiece];
                    switch(status)
                    {
                        case 0 :
                            blackRookCapture(positionalInformation[currentPiece]);
                            break;
                        case -1 :
                            break;
                        case 1 :
                        case 16 :
                            blackSlidingCapture(positionalInformation[currentPiece], status);
                            break;
                    }
                    break;
                case QUEEN : //Queen
                    status = pieceStatuses[currentPiece];
                    switch(status)
                    {
                        case 0 :
                            blackQueenCapture(positionalInformation[currentPiece]);
                            break;
                        case -1 :
                            break;
                        default :
                            blackSlidingCapture(positionalInformation[currentPiece], status);
                            break;
                    }
                    break;
                case KING : //King
                    blackKingCapture();
                    break;
            }
        }
    }
}

void qMoveGenerator::getCheckMoves(void)
{
    int terminalSquare, pieceType, pieceSquare, pieceStatusIndex, boardSquare, square;
    
    if(positionalInformation[abs(position[attackingSquare]) + 64] == KNIGHT) terminalSquare = attackingSquare + attackDirection;
    else terminalSquare = kingSquare;
    
    for(pieceStatusIndex = 0; pieceStatusIndex < 16; pieceStatusIndex++)
    {
        currentPiece = pieceOrder[pieceStatusIndex];
        if(pieceStatuses[currentPiece] == 0) //A pinned piece logically cannot intercept a check from another piece
        {
            pieceType = positionalInformation[currentPiece + 64];
            pieceSquare = positionalInformation[currentPiece];
            switch(pieceType)
            {
                case PAWN :
                    switch(turnSwitch*(attackingSquare - pieceSquare))
                    {
                        case 15 :
                            allCaptures[arraySize] = pieceSquare;
                            allCaptures[arraySize + 1] = attackingSquare;
                            arraySize += 2;
                            break;
                        case 17 :
                            allCaptures[arraySize] = pieceSquare;
                            allCaptures[arraySize + 1] = attackingSquare;
                            arraySize += 2;
                            break;
                    }
                    for(square = attackingSquare + attackDirection; square != terminalSquare; square += attackDirection)
                    {
                        if(turnSwitch*(square - pieceSquare) == 16)
                        {
                            allCaptures[arraySize] = pieceSquare;
                            allCaptures[arraySize + 1] = square;
                            arraySize += 2;
                        }
                        else if(turnSwitch*(square - pieceSquare) == 32)
                        {
                            if(turnSwitch*pieceSquare < (24*whiteTurn - (1 - whiteTurn)*95) && position[pieceSquare + turnSwitch*16] == 0)
                            {
                                allCaptures[arraySize] = pieceSquare;
                                allCaptures[arraySize + 1] = square;
                                arraySize += 2;
                            }
                        }
                    }
                    break;
                case KNIGHT :
                    for(boardSquare = attackingSquare; boardSquare != terminalSquare; boardSquare += attackDirection)
                    {
                        if(pieceSquare > boardSquare)
                        {
                            if(N_DIFFS[pieceSquare - boardSquare] > 0) line(pieceSquare, boardSquare, -N_DIFFS[pieceSquare - boardSquare]);
                        }
                        else
                        {
                            if(N_DIFFS[boardSquare - pieceSquare] > 0) line(pieceSquare, boardSquare, N_DIFFS[boardSquare - pieceSquare]);
                        }
                    }
                    break;
                case BISHOP :
                    for(boardSquare = attackingSquare; boardSquare != terminalSquare; boardSquare += attackDirection)
                    {
                        if(pieceSquare > boardSquare)
                        {
                            if(B_DIFFS[pieceSquare - boardSquare] > 0) line(pieceSquare, boardSquare, -B_DIFFS[pieceSquare - boardSquare]);
                        }
                        else
                        {
                            if(B_DIFFS[boardSquare - pieceSquare] > 0) line(pieceSquare, boardSquare, B_DIFFS[boardSquare - pieceSquare]);
                        }
                    }
                    break;
                case ROOK :
                    for(boardSquare = attackingSquare; boardSquare != terminalSquare; boardSquare += attackDirection)
                    {
                        if(pieceSquare > boardSquare)
                        {
                            if(R_DIFFS[pieceSquare - boardSquare] > 0) line(pieceSquare, boardSquare, -R_DIFFS[pieceSquare - boardSquare]);
                        }
                        else
                        {
                            if(R_DIFFS[boardSquare - pieceSquare] > 0) line(pieceSquare, boardSquare, R_DIFFS[boardSquare - pieceSquare]);
                        }
                    }
                    break;
                case QUEEN :
                    for(boardSquare = attackingSquare; boardSquare != terminalSquare; boardSquare += attackDirection)
                    {
                        if(pieceSquare > boardSquare)
                        {
                            if(Q_DIFFS[pieceSquare - boardSquare] > 0) line(pieceSquare, boardSquare, -Q_DIFFS[pieceSquare - boardSquare]);
                        }
                        else
                        {
                            if(Q_DIFFS[boardSquare - pieceSquare] > 0) line(pieceSquare, boardSquare, Q_DIFFS[boardSquare - pieceSquare]);
                        }
                    }
                    break;
                case KING :
                    if(whiteTurn) whiteKingMove();
                    else blackKingMove();
                    break;
            }
        }
    }
}

bool qMoveGenerator::enPassantPin(int firstPawnSquare, int secondPawnSquare)
{
    int searchDirection, boardSquare, squareValue, pieceType;
    
    searchDirection = abs(firstPawnSquare - kingSquare)/(firstPawnSquare - kingSquare);
    boardSquare = kingSquare + searchDirection;
    
    while(true)
    {
        if((136&boardSquare) == 0)
        {
            if(boardSquare != firstPawnSquare && boardSquare != secondPawnSquare)
            {
                squareValue = turnSwitch*position[boardSquare];
                if(squareValue > 0) return false; //Allied piece found
                else if(squareValue < 0)
                {
                    pieceType = positionalInformation[abs(squareValue) + 64];
                    if(pieceType == ROOK || pieceType == QUEEN) return true; //Rook or queen
                }
            }
        }
        else return false;
        boardSquare = boardSquare + searchDirection;
    }
}

void qMoveGenerator::enPassant(void)
{
    int squareValue, pieceType, status;
    if((136&(lastToSquare + 1)) == 0) //Check for en passant capture
    {
        squareValue = turnSwitch*position[lastToSquare + 1];
        if(squareValue > 0) // piece in capturing position
        {
            pieceType = positionalInformation[squareValue + 64];
            if(pieceType == PAWN) //Pawn
            {
                status = pieceStatuses[squareValue];
                //Only permissible pin is the diagonal of the capture
                if(status == 0 || status == (15*whiteTurn + (1 - whiteTurn)*17))
                {
                    //Ensure capture does not reveal king to check:
                    if(abs(lastToSquare - kingSquare) < 7) //King shares rank
                    {
                        if(!enPassantPin(lastToSquare, lastToSquare + 1))
                        {
                            allCaptures[arraySize] = -(lastToSquare + 1);
                            allCaptures[arraySize + 1] = lastToSquare + turnSwitch*16;
                            arraySize += 2;
                        }
                    }
                    else
                    {
                        //Negative fromSquare indicated special move (here en passant)
                        allCaptures[arraySize] = -(lastToSquare + 1);
                        allCaptures[arraySize + 1] = lastToSquare + turnSwitch*16;
                        arraySize += 2;
                    }
                }
            }
        }
    }
    if((136&(lastToSquare - 1)) == 0)
    {
        squareValue = turnSwitch*position[lastToSquare - 1];
        if(squareValue > 0)
        {
            pieceType = positionalInformation[squareValue + 64];
            if(pieceType == PAWN)
            {
                status = pieceStatuses[squareValue];
                if(status == 0 || status == (17*whiteTurn + (1 - whiteTurn)*15))
                {
                    //Ensure capture does not reveal king to check:
                    if(abs(lastToSquare - kingSquare) < 7) //King shares rank
                    {
                        if(!enPassantPin(lastToSquare, lastToSquare - 1))
                        {
                            allCaptures[arraySize] = -(lastToSquare - 1);
                            allCaptures[arraySize + 1] = lastToSquare + turnSwitch*16;
                            arraySize += 2;
                        }
                    }
                    else
                    {
                        //Negative fromSquare indicated special move (here en passant)
                        allCaptures[arraySize] = -(lastToSquare - 1);
                        allCaptures[arraySize + 1] = lastToSquare + turnSwitch*16;
                        arraySize += 2;
                    }
                }
            }
        }
    }
}

void qMoveGenerator::shuffleMoves(int * pieceValues, int currentDepth)
{
    int fromSquare, toSquare, shuffleRank;
    shuffleRank = 0;
#ifdef Q_STATIC_EXCHANGE_EVALUATION
    int loserShuffleRank, seeEvaluation;
    
    loserShuffleRank = arraySize;
    
    for(int i = shuffleRank; i < loserShuffleRank; i+=2)
    {
        if(position[allCaptures[i+1]] != 0) //Move could be check evasion move
        {
            fromSquare = allCaptures[i];
            toSquare = allCaptures[i+1];
            seeEvaluation = SEE(fromSquare, toSquare);
            
            if((whiteTurn && seeEvaluation == WHITE_WINS) || (!whiteTurn && seeEvaluation == BLACK_WINS))
            {
                if(i != shuffleRank)
                {
                    allCaptures[i] = allCaptures[shuffleRank];
                    allCaptures[i+1] = allCaptures[shuffleRank+1];
                    allCaptures[shuffleRank] = fromSquare;
                    allCaptures[shuffleRank+1] = toSquare;
                }
                shuffleRank += 2;
            }
            else if((!whiteTurn && seeEvaluation == WHITE_WINS) || (whiteTurn && seeEvaluation == BLACK_WINS))
            {
                loserShuffleRank -= 2;
                if(i != loserShuffleRank)
                {
                    allCaptures[i] = allCaptures[loserShuffleRank];
                    allCaptures[i+1] = allCaptures[loserShuffleRank+1];
                    allCaptures[loserShuffleRank] = fromSquare;
                    allCaptures[loserShuffleRank+1] = toSquare;
                }
                i -= 2;
            }
        }
    }
    arraySize = loserShuffleRank;
#else
    int bestMoveIndex;
    double score, bestScore = 1;
    bool captureFound;
    captureFound = false;
    for(int i = shuffleRank; i < arraySize; i+=2)
    {
        if(position[allCaptures[i+1]] != 0) //capture
        {
            score = abs(pieceValues[-turnSwitch*position[allCaptures[i+1]]])/abs(pieceValues[turnSwitch*position[abs(allCaptures[i])]]);
            if(score > bestScore)
            {
                bestScore = score;
                bestMoveIndex = i;
                captureFound = true; 
            }
        }
    }
    if(captureFound)
    {
        fromSquare = allCaptures[bestMoveIndex];
        toSquare = allCaptures[bestMoveIndex+1];
        allCaptures[bestMoveIndex] = allCaptures[shuffleRank];
        allCaptures[bestMoveIndex + 1] = allCaptures[shuffleRank + 1];
        allCaptures[shuffleRank] = fromSquare;
        allCaptures[shuffleRank + 1] = toSquare;
        shuffleRank += 2;   
    }
    
    for(int i = shuffleRank; i < arraySize; i+=2)
    {
        if(allCaptures[i+1] == lastToSquare)
        {
            fromSquare = allCaptures[i];
            toSquare = allCaptures[i+1];
            allCaptures[i] = allCaptures[shuffleRank];
            allCaptures[i+1] = allCaptures[shuffleRank+1];
            allCaptures[shuffleRank] = fromSquare;
            allCaptures[shuffleRank+1] = toSquare;
            break;
        }
    }
#endif
}

//Move generation functions

void qMoveGenerator::whitePawnCapture(int pieceLocation)
{
    int nextMove, i;
    //Capturing moves
    for(i = 0; i < 2; i++)
    {
        nextMove = pieceLocation + PAWN_MOVES[i];
        if((136&nextMove) == 0) //Move on board
        {
            if(position[nextMove] < 0) //Capturable piece
            {
                if(nextMove > 111)
                {
                    allCaptures[arraySize] = -pieceLocation;
                    allCaptures[arraySize + 1] = nextMove;
                    arraySize += 2;
                }
                else
                {
                    allCaptures[arraySize] = pieceLocation;
                    allCaptures[arraySize + 1] = nextMove;
                    arraySize += 2;
                }
            }
        }
    }
}

void qMoveGenerator::blackPawnCapture(int pieceLocation)
{
    int nextMove;
    //Capturing moves
    for(int i = 0; i < 2; i++)
    {
        nextMove = pieceLocation - PAWN_MOVES[i];
        if((136&nextMove) == 0) //Move on board
        {
            if(position[nextMove] > 0) //Capturable piece
            {
                if(nextMove < 8)
                {
                    allCaptures[arraySize] = -pieceLocation;
                    allCaptures[arraySize + 1] = nextMove;
                    arraySize += 2;
                }
                else 
                {
                    allCaptures[arraySize] = pieceLocation;
                    allCaptures[arraySize + 1] = nextMove;
                    arraySize += 2;
                }
            }
        }
    }
}

void qMoveGenerator::whitePinnedPawnCapture(int pieceLocation, int nextMove)
{
    if((136&nextMove) == 0) //Move on board
    {
        if(position[nextMove] < 0) //Capturable piece
        {
            if(nextMove > 111)
            {
                allCaptures[arraySize] = -pieceLocation;
                allCaptures[arraySize + 1] = nextMove;
                arraySize += 2;
            }
            else
            {
                allCaptures[arraySize] = pieceLocation;
                allCaptures[arraySize + 1] = nextMove;
                arraySize += 2;
            }
        }
    }
}

void qMoveGenerator::blackPinnedPawnCapture(int pieceLocation, int nextMove)
{
    if((136&nextMove) == 0) //Move on board
    {
        if(position[nextMove] > 0) //Capturable piece
        {
            if(nextMove < 8)
            {
                allCaptures[arraySize] = -pieceLocation;
                allCaptures[arraySize + 1] = nextMove;
                arraySize += 2;
            }
            else 
            {
                allCaptures[arraySize] = pieceLocation;
                allCaptures[arraySize + 1] = nextMove;
                arraySize += 2;
            }
        }
    }
}

void qMoveGenerator::whiteKnightCapture(int pieceLocation)
{
    int nextMove;
    for(int i = 0; i < 8; i++)
    {
        nextMove = pieceLocation + KNIGHT_MOVES[i];
        if((nextMove&136) == 0)
        {
            if(position[nextMove] < 0)
            {
                allCaptures[arraySize] = pieceLocation;
                allCaptures[arraySize + 1] = nextMove;
                arraySize += 2;
            }
        }
    }
}

void qMoveGenerator::blackKnightCapture(int pieceLocation)
{
    int nextMove;
    for(int i = 0; i < 8; i++)
    {
        nextMove = pieceLocation + KNIGHT_MOVES[i];
        if((nextMove&136) == 0)
        {
            if(position[nextMove] > 0)
            {
                allCaptures[arraySize] = pieceLocation;
                allCaptures[arraySize + 1] = nextMove;
                arraySize += 2;
            }
        }
    }
}

void qMoveGenerator::whiteBishopCapture(int pieceLocation)
{
    int nextDirection, boardSquare;
    for(int i = 0; i < 4; i++)
    {
        nextDirection = BISHOP_MOVES[i];
        boardSquare = pieceLocation;
        while(true)
        {
            boardSquare += nextDirection;
            if((136&boardSquare) == 0)
            {
                if(position[boardSquare] < 0)
                {
                    allCaptures[arraySize] = pieceLocation;
                    allCaptures[arraySize + 1] = boardSquare;
                    arraySize += 2;
                    break;
                }
                else if(position[boardSquare] > 0) break;
            }
            else break;
        }
    }
}

void qMoveGenerator::whiteRookCapture(int pieceLocation)
{
    int nextDirection, boardSquare;
    for(int i = 0; i < 4; i++)
    {
        nextDirection = ROOK_MOVES[i];
        boardSquare = pieceLocation;
        while(true)
        {
            boardSquare += nextDirection;
            if((136&boardSquare) == 0)
            {
                if(position[boardSquare] < 0)
                {
                    allCaptures[arraySize] = pieceLocation;
                    allCaptures[arraySize + 1] = boardSquare;
                    arraySize += 2;
                    break;
                }
                else if(position[boardSquare] > 0) break;
            }
            else break;
        }
    }
}

void qMoveGenerator::whiteQueenCapture(int pieceLocation)
{
    int nextDirection, boardSquare;
    for(int i = 0; i < 8; i++)
    {
        nextDirection = QUEEN_MOVES[i];
        boardSquare = pieceLocation;
        while(true)
        {
            boardSquare += nextDirection;
            if((136&boardSquare) == 0)
            {
                if(position[boardSquare] < 0)
                {
                    allCaptures[arraySize] = pieceLocation;
                    allCaptures[arraySize + 1] = boardSquare;
                    arraySize += 2;
                    break;
                }
                else if(position[boardSquare] > 0) break;
            }
            else break;
        }
    }
}

void qMoveGenerator::blackBishopCapture(int pieceLocation)
{
    int nextDirection, boardSquare;
    for(int i = 0; i < 4; i++)
    {
        nextDirection = BISHOP_MOVES[i];
        boardSquare = pieceLocation;
        while(true)
        {
            boardSquare += nextDirection;
            if((136&boardSquare) == 0)
            {
                if(position[boardSquare] > 0)
                {
                    allCaptures[arraySize] = pieceLocation;
                    allCaptures[arraySize + 1] = boardSquare;
                    arraySize += 2;
                    break;
                }
                else if(position[boardSquare] < 0) break;
            }
            else break;
        }
    }
}

void qMoveGenerator::blackRookCapture(int pieceLocation)
{
    int nextDirection, boardSquare;
    for(int i = 0; i < 4; i++)
    {
        nextDirection = ROOK_MOVES[i];
        boardSquare = pieceLocation;
        while(true)
        {
            boardSquare += nextDirection;
            if((136&boardSquare) == 0)
            {
                if(position[boardSquare] > 0)
                {
                    allCaptures[arraySize] = pieceLocation;
                    allCaptures[arraySize + 1] = boardSquare;
                    arraySize += 2;
                    break;
                }
                else if(position[boardSquare] < 0) break;
            }
            else break;
        }
    }
}

void qMoveGenerator::blackQueenCapture(int pieceLocation)
{
    int nextDirection, boardSquare;
    for(int i = 0; i < 8; i++)
    {
        nextDirection = QUEEN_MOVES[i];
        boardSquare = pieceLocation;
        while(true)
        {
            boardSquare += nextDirection;
            if((136&boardSquare) == 0)
            {
                if(position[boardSquare] > 0)
                {
                    allCaptures[arraySize] = pieceLocation;
                    allCaptures[arraySize + 1] = boardSquare;
                    arraySize += 2;
                    break;
                }
                else if(position[boardSquare] < 0) break;
            }
            else break;
        }
    }
}

void qMoveGenerator::whiteKingCapture(void)
{
    int check = false;
    int boardSquare, searchSquare, pieceType, i, j;
    
    for(i = 0; i < 8; i++)
    {
        boardSquare = kingSquare + KING_MOVES[i];
        if((136&boardSquare) == 0)
        {
            if(position[boardSquare] < 0)
            {
                //Look for knight attacks
                for(j = 0; j < 8; j++)
                {
                    searchSquare = boardSquare + KNIGHT_MOVES[j];
                    if((136&searchSquare) == 0)
                    {
                        if(position[searchSquare] < 0)
                        {
                            if(positionalInformation[-position[searchSquare] + 64] == KNIGHT)
                            {
                                check = true;
                                break; //Move invalidated - no need to continue
                            }
                        }
                    }
                }
                if(!check)
                {
                    //Look for sliding piece attacks
                    for(j = 0; j < 8; j++)
                    {
                        searchSquare = boardSquare;
                        while(true)
                        {
                            searchSquare += SEARCH_SQUARES[j];
                            if((136&searchSquare) == 0)
                            {
                                if(position[searchSquare] < 0)
                                {
                                    pieceType = positionalInformation[-position[searchSquare] + 64];
                                    if(pieceType == ATTACKERS1[j] || pieceType == ATTACKERS2[j]) check = true;
                                    break;
                                }
                                else if(position[searchSquare] > 0)
                                {
                                    //This condition prevents king from hiding behind himself
                                    if(searchSquare != kingSquare) break;
                                }
                            }
                            else break;
                        }
                        if(check) break;
                    }
                }
                if(!check)
                {
                    //Look for pawn attacks
                    for(j = 0; j < 2; j++)
                    {
                        searchSquare = boardSquare + PAWN_MOVES[j];
                        if((136&searchSquare) == 0)
                        {
                            if(position[searchSquare] < 0) //Enemy piece found
                            {
                                if(positionalInformation[-position[searchSquare] + 64] == PAWN)
                                {
                                    check = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                if(!check)
                {
                    //Look for enemy king
                    for(j = 0; j < 8; j++)
                    {
                        searchSquare = boardSquare + KING_MOVES[j];
                        if((136&searchSquare) == 0)
                        {
                            if(position[searchSquare] < 0) //Enemy piece found
                            {
                                if(positionalInformation[-position[searchSquare] + 64] == KING)
                                {
                                    check = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                if(!check)
                {
                    allCaptures[arraySize] = kingSquare;
                    allCaptures[arraySize + 1] = boardSquare;
                    arraySize += 2;
                }
            }
        }
        check = false;
    }   
}

void qMoveGenerator::blackKingCapture(void)
{
    int check = false;
    int boardSquare, searchSquare, pieceType, i, j;
    
    for(i = 0; i < 8; i++)
    {
        boardSquare = kingSquare + KING_MOVES[i];
        if((136&boardSquare) == 0)
        {
            if(position[boardSquare] > 0)
            {
                //Look for knight attacks
                for(j = 0; j < 8; j++)
                {
                    searchSquare = boardSquare + KNIGHT_MOVES[j];
                    if((136&searchSquare) == 0)
                    {
                        if(position[searchSquare] > 0)
                        {
                            if(positionalInformation[position[searchSquare] + 64] == KNIGHT) //Knight
                            {
                                check = true;
                                break;
                            }
                        }
                    }
                }
                if(!check)
                {
                    //Look for sliding piece attacks
                    for(j = 0; j < 8; j++)
                    {
                        searchSquare = boardSquare;
                        while(true)
                        {
                            searchSquare += SEARCH_SQUARES[j];
                            if((136&searchSquare) == 0)
                            {
                                if(position[searchSquare] > 0)
                                {
                                    pieceType = positionalInformation[position[searchSquare] + 64];
                                    if(pieceType == ATTACKERS1[j] || pieceType == ATTACKERS2[j]) check = true;
                                    break;
                                }
                                else if(position[searchSquare] < 0)
                                {
                                    if(searchSquare != kingSquare) break;
                                }
                            }
                            else break;
                        }
                        if(check) break;
                    }
                } 
                if(!check)
                {
                    //Look for pawn attacks
                    for(j = 0; j < 2; j++)
                    {
                        searchSquare = boardSquare - PAWN_MOVES[j];
                        if((136&searchSquare) == 0)
                        {
                            if(position[searchSquare] > 0) //Enemy piece found
                            {
                                if(positionalInformation[position[searchSquare] + 64] == PAWN) //Pawn
                                {
                                    check = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                if(!check)
                {
                    //Look for enemy king
                    for(j = 0; j < 8; j++)
                    {
                        searchSquare = boardSquare + KING_MOVES[j];
                        if((136&searchSquare) == 0)
                        {
                            if(position[searchSquare] > 0) //Enemy piece found
                            {
                                if(positionalInformation[position[searchSquare] + 64] == KING)
                                {
                                    check = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                if(!check)
                {
                    allCaptures[arraySize] = kingSquare;
                    allCaptures[arraySize + 1] = boardSquare;
                    arraySize += 2;
                }
            }
        }
        check = false;
    }
}

void qMoveGenerator::whiteKingMove(void)
{
    int check = false;
    int boardSquare, searchSquare, pieceType, i, j;
    
    for(i = 0; i < 8; i++)
    {
        boardSquare = kingSquare + KING_MOVES[i];
        if((136&boardSquare) == 0)
        {
            if(position[boardSquare] <= 0)
            {
                //Look for knight attacks
                for(j = 0; j < 8; j++)
                {
                    searchSquare = boardSquare + KNIGHT_MOVES[j];
                    if((136&searchSquare) == 0)
                    {
                        if(position[searchSquare] < 0)
                        {
                            if(positionalInformation[-position[searchSquare] + 64] == 1)
                            {
                                check = true;
                                break; //Move invalidated - no need to continue
                            }
                        }
                    }
                }
                if(!check)
                {
                    //Look for sliding piece attacks
                    for(j = 0; j < 8; j++)
                    {
                        searchSquare = boardSquare;
                        while(true)
                        {
                            searchSquare += SEARCH_SQUARES[j];
                            if((136&searchSquare) == 0)
                            {
                                if(position[searchSquare] < 0)
                                {
                                    pieceType = positionalInformation[-position[searchSquare] + 64];
                                    if(pieceType == ATTACKERS1[j] || pieceType == ATTACKERS2[j]) check = true;
                                    break;
                                }
                                else if(position[searchSquare] > 0)
                                {
                                    //This condition prevents king from hiding behind himself
                                    if(searchSquare != kingSquare) break;
                                }
                            }
                            else break;
                        }
                        if(check) break;
                    }
                }
                if(!check)
                {
                    //Look for pawn attacks
                    for(j = 0; j < 2; j++)
                    {
                        searchSquare = boardSquare + PAWN_MOVES[j];
                        if((136&searchSquare) == 0)
                        {
                            if(position[searchSquare] < 0) //Enemy piece found
                            {
                                if(positionalInformation[-position[searchSquare] + 64] == 0)
                                {
                                    check = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                if(!check)
                {
                    //Look for enemy king
                    for(j = 0; j < 8; j++)
                    {
                        searchSquare = boardSquare + KING_MOVES[j];
                        if((136&searchSquare) == 0)
                        {
                            if(position[searchSquare] < 0) //Enemy piece found
                            {
                                if(positionalInformation[-position[searchSquare] + 64] == KING)
                                {
                                    check = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                if(!check)
                {
                    allCaptures[arraySize] = kingSquare;
                    allCaptures[arraySize + 1] = boardSquare;
                    arraySize += 2;
                }
            }
        }
        check = false;
    }
}

void qMoveGenerator::blackKingMove(void)
{
    int check = false;
    int boardSquare, searchSquare, pieceType, i, j;
    
    for(i = 0; i < 8; i++)
    {
        boardSquare = kingSquare + KING_MOVES[i];
        if((136&boardSquare) == 0)
        {
            if(position[boardSquare] >= 0)
            {
                //Look for knight attacks
                for(j = 0; j < 8; j++)
                {
                    searchSquare = boardSquare + KNIGHT_MOVES[j];
                    if((136&searchSquare) == 0)
                    {
                        if(position[searchSquare] > 0)
                        {
                            if(positionalInformation[position[searchSquare] + 64] == 1) //Knight
                            {
                                check = true;
                                break;
                            }
                        }
                    }
                }
                if(!check)
                {
                    //Look for sliding piece attacks
                    for(j = 0; j < 8; j++)
                    {
                        searchSquare = boardSquare;
                        while(true)
                        {
                            searchSquare += SEARCH_SQUARES[j];
                            if((136&searchSquare) == 0)
                            {
                                if(position[searchSquare] > 0)
                                {
                                    pieceType = positionalInformation[position[searchSquare] + 64];
                                    if(pieceType == ATTACKERS1[j] || pieceType == ATTACKERS2[j]) check = true;
                                    break;
                                }
                                else if(position[searchSquare] < 0)
                                {
                                    if(searchSquare != kingSquare) break;
                                }
                            }
                            else break;
                        }
                        if(check) break;
                    }
                } 
                if(!check)
                {
                    //Look for pawn attacks
                    for(j = 0; j < 2; j++)
                    {
                        searchSquare = boardSquare - PAWN_MOVES[j];
                        if((136&searchSquare) == 0)
                        {
                            if(position[searchSquare] > 0) //Enemy piece found
                            {
                                if(positionalInformation[position[searchSquare] + 64] == 0) //Pawn
                                {
                                    check = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                if(!check)
                {
                    //Look for enemy king
                    for(j = 0; j < 8; j++)
                    {
                        searchSquare = boardSquare + KING_MOVES[j];
                        if((136&searchSquare) == 0)
                        {
                            if(position[searchSquare] > 0) //Enemy piece found
                            {
                                if(positionalInformation[position[searchSquare] + 64] == KING)
                                {
                                    check = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            if(!check)
                {
                    allCaptures[arraySize] = kingSquare;
                    allCaptures[arraySize + 1] = boardSquare;
                    arraySize += 2;
                }
            }
        }
        check = false;
    }
}

void qMoveGenerator::displayPosition(void)
{
    cout << "\n\n";
    for(int j = 7; j >= 0; j--)
    {
        cout << "|";
        for(int k = 0; k < 8; k++)
        {
            int squareValue = qMoveGenerator::position[j*16 + k];
            if(squareValue != 0)
            {
                int pieceType = qMoveGenerator::positionalInformation[abs(qMoveGenerator::position[j*16 + k]) + 64];
                if(squareValue > 0)
                {
                    cout << WHITE_PIECE_NAMES[pieceType];
                }
                else 
                {
                    cout << BLACK_PIECE_NAMES[pieceType];
                }
                cout << "|";            
            }
            else
            {
                cout << " |";
            }           
        }       
        cout << "\n";
    }
    cout << "\n";
}

void qMoveGenerator::displayPositionalInformation(void)
{
    cout << "\n";
    for(int i = 7; i >= 0; i--)
    {
        cout << "|";
        for(int j = 0; j < 16; j++)
        {
            cout << qMoveGenerator::positionalInformation[16*i + j];
            cout << "\t|";
        }
        cout << "\n";
    }
}

void qMoveGenerator::whiteSlidingCapture(int pieceLocation, int pieceStatus)
{
    int boardSquare;
    for(int i = 0; i < 2; i++)
    {
        boardSquare = pieceLocation;
        while(true)
        {
            boardSquare += pieceStatus;
            if((136&boardSquare) == 0)
            {
                if(position[boardSquare] <= 0)
                {
                    allCaptures[arraySize] = pieceLocation;
                    allCaptures[arraySize + 1] = boardSquare;
                    arraySize += 2;
                }
                if(position[boardSquare] != 0) break;
            }
            else break;
        }
        pieceStatus = -pieceStatus;
    }
}

void qMoveGenerator::blackSlidingCapture(int pieceLocation, int pieceStatus)
{
    int boardSquare;
    for(int i = 0; i < 2; i++)
    {
        boardSquare = pieceLocation;
        while(true)
        {
            boardSquare += pieceStatus;
            if((136&boardSquare) == 0)
            {
                if(position[boardSquare] >= 0)
                {
                    allCaptures[arraySize] = pieceLocation;
                    allCaptures[arraySize + 1] = boardSquare;
                    arraySize += 2;
                }
                if(position[boardSquare] != 0) break;
            }
            else break;
        }
        pieceStatus = -pieceStatus;
    }
}

//int qMoveGenerator::SEE(int fromSquare, int captureSquare)
//{
//  int searchSquare, capturingPieceType, capturedPieceType, currentPieceIndex, pieceType, evaluation, numWhiteKnights, numBlackKnights, i, j;
//  int lastPieceType;
//  int captureLayer[8], capturerTypes[8];
//  bool whiteToCapture, whiteHoldsEquality = false, blackHoldsEquality = false;
//  
//  numWhiteKnights = 0;
//  numBlackKnights = 0;
//  
//  capturingPieceType = positionalInformation[turnSwitch*position[fromSquare] + 64];
//  capturedPieceType = positionalInformation[-turnSwitch*position[captureSquare] + 64];
//  
//  evaluation = turnSwitch * MATERIAL_VALUES[capturedPieceType];
//  if(whiteTurn)
//  {
//      if(evaluation > (MATERIAL_VALUES[capturingPieceType] + MATERIAL_VALUES[PAWN])) return WHITE_WINS; //White wins
//  }
//  else
//  {
//      if(evaluation < -(MATERIAL_VALUES[capturingPieceType] + MATERIAL_VALUES[PAWN])) return BLACK_WINS; //Black wins
//  }
//  
//  lastPieceType = capturingPieceType;
//  whiteToCapture = ! whiteTurn;
//  
//  for(i = 0; i < 8; i++)
//  {
//      //Find first layer
//      j = 1;
//      while(true)
//      {
//          searchSquare = captureSquare + j*SEARCH_SQUARES[i];
//          if(searchSquare != fromSquare)
//          {
//              if((136&searchSquare) == 0)
//              {
//                  if(position[searchSquare] > 0) //White piece found
//                  {
//                      pieceType = positionalInformation[position[searchSquare] + 64];
//                      if(pieceType == ATTACKERS1[i] || pieceType == ATTACKERS2[i])
//                      {
//                          captureLayer[i] = searchSquare;
//                          capturerTypes[i] = pieceType;
//                      }
//                      else if(j == 1)
//                      {
//                          if(pieceType == PAWN)
//                          {
//                              if(SEARCH_SQUARES[i] == -15 || SEARCH_SQUARES[i] == -17)
//                              {
//                                  captureLayer[i] = searchSquare;
//                                  capturerTypes[i] = PAWN;
//                              }
//                              else capturerTypes[i] = -1;
//                          }
//                          else if(pieceType == KING)
//                          {
//                              captureLayer[i] = searchSquare;
//                              capturerTypes[i] = KING;
//                          }
//                          else capturerTypes[i] = -1;
//                      }
//                      else capturerTypes[i] = -1;
//                      break;
//                  }
//                  else if(position[searchSquare] < 0) //Black piece found
//                  {
//                      pieceType = positionalInformation[-position[searchSquare] + 64];
//                      if(pieceType == ATTACKERS1[i] || pieceType == ATTACKERS2[i])
//                      {
//                          captureLayer[i] = -searchSquare;
//                          capturerTypes[i] = pieceType;
//                      }
//                      else if(j == 1)
//                      {
//                          if(pieceType == PAWN)
//                          {
//                              if(SEARCH_SQUARES[i] == 15 || SEARCH_SQUARES[i] == 17)
//                              {
//                                  captureLayer[i] = -searchSquare;
//                                  capturerTypes[i] = PAWN;
//                              }
//                              else capturerTypes[i] = -1;
//                          }
//                          else if(pieceType == KING)
//                          {
//                              captureLayer[i] = -searchSquare;
//                              capturerTypes[i] = KING;
//                          }
//                          else capturerTypes[i] = -1;
//                      }
//                      else capturerTypes[i] = -1;
//                      break;
//                  }
//              }
//              else
//              {
//                  capturerTypes[i] = -1;
//                  break;
//              }
//          }
//          j++;
//      }
//      //Find knights
//      searchSquare = captureSquare + KNIGHT_MOVES[i];
//      if((136&searchSquare) == 0 && searchSquare != fromSquare)
//      {
//          if(position[searchSquare] > 0)
//          {
//              if(positionalInformation[position[searchSquare] + 64] == KNIGHT) numWhiteKnights++;
//          }
//          else if(position[searchSquare] < 0)
//          {
//              if(positionalInformation[-position[searchSquare] + 64] == KNIGHT) numBlackKnights++;
//          }
//      }
//  }
//  
//  while(true)
//  {
//      if(whiteToCapture)
//      {
//          if(evaluation > MATERIAL_VALUES[PAWN])
//          {
//              if(blackHoldsEquality) return EQUALITY;
//              return WHITE_WINS;
//          }
//          else if(-MATERIAL_VALUES[PAWN] < evaluation && evaluation < MATERIAL_VALUES[PAWN]) whiteHoldsEquality = true;
//          capturingPieceType = KING + 1;
//          for(i = 0; i < 8; i++)
//          {
//              if(capturerTypes[i] != -1)
//              {
//                  if(position[abs(captureLayer[i])] > 0)
//                  {
//                      if(capturerTypes[i] < capturingPieceType)
//                      {
//                          capturingPieceType = capturerTypes[i];
//                          currentPieceIndex = i;
//                      }
//                  }
//              }
//          }
//          if(capturingPieceType > KING) //No capture found
//          {
//              if(numWhiteKnights > 0)
//              {
//                  capturingPieceType = KNIGHT;
//                  numWhiteKnights--;
//              }
//              else
//              {
//                  if(whiteHoldsEquality) return EQUALITY;
//                  else return BLACK_WINS;
//              }
//          }
//          else
//          {
//              j = 1;
//              while(true)
//              {
//                  searchSquare = captureLayer[currentPieceIndex] + j*SEARCH_SQUARES[currentPieceIndex];
//                  if((136&searchSquare) == 0)
//                  {
//                      if(position[searchSquare] > 0) //White piece found
//                      {
//                          pieceType = positionalInformation[position[searchSquare] + 64];
//                          if(pieceType == ATTACKERS1[currentPieceIndex] || pieceType == ATTACKERS2[currentPieceIndex])
//                          {
//                              captureLayer[currentPieceIndex] = searchSquare;
//                              capturerTypes[currentPieceIndex] = pieceType;
//                          }
//                          else capturerTypes[currentPieceIndex] = -1;
//                          break;
//                      }
//                      else if(position[searchSquare] < 0) //Black piece found
//                      {
//                          pieceType = positionalInformation[-position[searchSquare] + 64];
//                          if(pieceType == ATTACKERS1[currentPieceIndex] || pieceType == ATTACKERS2[currentPieceIndex])
//                          {
//                              captureLayer[currentPieceIndex] = -searchSquare;
//                              capturerTypes[currentPieceIndex] = pieceType;
//                          }
//                          else capturerTypes[currentPieceIndex] = -1;
//                          break;
//                      }
//                  }
//                  else
//                  {
//                      capturerTypes[currentPieceIndex] = -1;
//                      break;
//                  }
//                  j++;
//              }
//          }
//      }
//      else
//      {
//          if(evaluation < -MATERIAL_VALUES[PAWN])
//          {
//              if(whiteHoldsEquality) return EQUALITY;
//              return BLACK_WINS;
//          }
//          else if(-MATERIAL_VALUES[PAWN] < evaluation && evaluation < MATERIAL_VALUES[PAWN]) blackHoldsEquality = true;
//          capturingPieceType = KING + 1;
//          for(i = 0; i < 8; i++)
//          {
//              if(capturerTypes[i] != -1)
//              {
//                  if(position[abs(captureLayer[i])] < 0)
//                  {
//                  if(capturerTypes[i] < capturingPieceType)
//                  {
//                      capturingPieceType = capturerTypes[i];
//                      currentPieceIndex = i;
//                  }
//                  }
//              }
//          }
//          if(capturingPieceType > KING) //No capture found
//          {
//              if(numBlackKnights > 0)
//              {
//                  capturingPieceType = KNIGHT;
//                  numWhiteKnights--;
//              }
//              else
//              {
//                  if(blackHoldsEquality) return EQUALITY;
//                  else return WHITE_WINS;
//              }
//          }
//          else
//          {
//              j = 1;
//              while(true)
//              {
//                  searchSquare = -captureLayer[currentPieceIndex] + j*SEARCH_SQUARES[currentPieceIndex];
//                  if((136&searchSquare) == 0)
//                  {
//                      if(position[searchSquare] > 0) //White piece found
//                      {
//                          pieceType = positionalInformation[position[searchSquare] + 64];
//                          if(pieceType == ATTACKERS1[currentPieceIndex] || pieceType == ATTACKERS2[currentPieceIndex])
//                          {
//                              captureLayer[currentPieceIndex] = searchSquare;
//                              capturerTypes[currentPieceIndex] = pieceType;
//                          }
//                          else capturerTypes[currentPieceIndex] = -1;
//                          break;
//                      }
//                      else if(position[searchSquare] < 0) //Black piece found
//                      {
//                          pieceType = positionalInformation[-position[searchSquare] + 64];
//                          if(pieceType == ATTACKERS1[currentPieceIndex] || pieceType == ATTACKERS2[currentPieceIndex])
//                          {
//                              captureLayer[currentPieceIndex] = -searchSquare;
//                              capturerTypes[currentPieceIndex] = pieceType;
//                          }
//                          else capturerTypes[currentPieceIndex] = -1;
//                          break;
//                      }
//                  }
//                  else
//                  {
//                      capturerTypes[currentPieceIndex] = -1;
//                      break;
//                  }
//                  j++;
//              }
//          }       
//      }
//      if(whiteToCapture)
//      {
//          evaluation += MATERIAL_VALUES[lastPieceType];
//      }
//      else
//      {
//          evaluation -= MATERIAL_VALUES[lastPieceType];
//      }
//      lastPieceType = capturingPieceType;
//      whiteToCapture = ! whiteToCapture;
//  }
//  
////    for(int i = 0; i < 8; i++)
////    {
////        captureLayer[i] = toSquare
////        captureTypes[i] = -2; //Set initial values
////    }   
////    Find knights    
////    searchSquare = captureSquare + KNIGHT_MOVES[i];
////    if((136&searchSquare) == 0 && searchSquare != fromSquare)
////    {
////        if(position[searchSquare] > 0)
////        {
////            if(positionalInformation[position[searchSquare] + 64] == KNIGHT) numWhiteKnights++;
////        }
////        else if(position[searchSquare] < 0)
////        {
////            if(positionalInformation[-position[searchSquare] + 64] == KNIGHT) numBlackKnights++;
////        }
////    }
////
////    captureLayer[lastCapturingDirection] = toSquare; 
////
////    while(! noCaptures)
////    {
////        if(evaluation > sideSwitch*MATERIAL_VALUES[PAWN])
////        {
////            if(blackHoldsEquality) return EQUALITY;
////            return whiteToCapture * WHITE_WINS + (1 - whiteToCapture) * BLACK_WINS;
////        }
////        else if(abs(evaluation) < MATERIAL_VALUES[PAWN]) blackHoldsEquality = true;
////        capturingPieceType = KING + 1;
////        
////        for(i = 0; i < 8; i++)
////        {
////            if(captureTypes[i] == -2)
////            {
////                j = 1;
////                while(true)
////                {
////                    searchSquare = captureLayer[i] + j*SEARCH_SQUARES[i];
////                    if(searchSquare != fromSquare)
////                    {
////                        if((136&searchSquare) == 0)
////                        {
////                            if(position[searchSquare] != 0)
////                            {
////                                if(position[searchSquare] > 0) sideSwitch = 1;
////                                else sideSwitch = -1;
////                                pieceType = positionalInformation[sideSwitch * position[searchSquare] + 64];
////                                if(pieceType == ATTACKERS1[i] || pieceType == ATTACKERS2[i])
////                                {
////                                        captureLayer[i] = sideSwitch * searchSquare;
////                                        capturerTypes[i] = pieceType;
////                                }
////                                else if(j == 1)
////                                {
////                                    if(pieceType == PAWN)
////                                    {
////                                        if(SEARCH_SQUARES[i] == -sideSwitch * 15 || SEARCH_SQUARES[i] == -sideSwitch * 17)
////                                        {
////                                            captureLayer[i] = sideSwitch * searchSquare;
////                                            capturerTypes[i] = PAWN;
////                                        }
////                                        else capturerTypes[i] = -1;
////                                    }
////                                    else if(pieceType == KING)
////                                    {
////                                        captureLayer[i] = sideSwitch * searchSquare;
////                                        capturerTypes[i] = KING;
////                                    }
////                                    else capturerTypes[i] = -1;
////                                }
////                                else capturerTypes[i] = -1;
////                                break;
////                            }
////                        }
////                    }
////                    j++;
////                }
////            }
////            if(captureLayer[i] < 0 && capturerTypes[i] != -1)
////            {
////                if(capturerTypes[i] < capturingPieceType)
////                {
////                    capturingPieceType = capturerTypes[i];
////                    currentPieceIndex = i;
////                }
////            }
////        }
////        if(capturingPieceType > KING) //No capture found
////        {
////            if(numBlackKnights > 0)
////            {
////                capturingPieceType = KNIGHT;
////                numWhiteKnights--;
////            }
////            else
////            {
////                if(blackHoldsEquality) return EQUALITY;
////                else return WHITE_WINS;
////            }
////        }
////        else captureTypes[currentPieceIndex] = -2;
////        
////        evaluation += sideSwitch*MATERIAL_VALUES[lastPieceType];
////        lastPieceType = capturingPieceType;
////        sideSwitch = -sideSwitch;
////        whiteToCapture = ! whiteToCapture;
////    }
//}