
#include "MoveGenerator.h"

moveGenerator::moveGenerator(int * position, int * positionalInformation, int * allMoves)
{
    moveGenerator::position = position;
    moveGenerator::positionalInformation = positionalInformation;
    
    whiteTurn = positionalInformation[WHITE_TURN];
    turnSwitch = (-1 + 2*whiteTurn);
    
    castlingRights = positionalInformation[CASTLING];
    
    lastFromSquare = positionalInformation[LAST_FROM_SQUARE];
    lastToSquare = positionalInformation[LAST_TO_SQUARE];
    
    moveGenerator::allMoves = allMoves;
    arraySize = 0;
}

moveGenerator::~moveGenerator(void) 
{
}

int moveGenerator::getArraySize(void)
{
    return arraySize;
}

//Support functions

bool moveGenerator::attackOnKing(int direction)
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

bool moveGenerator::revealedAttackOnKing(int direction)
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

void moveGenerator::determinePin(int defendingSquare, int searchIndex)
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

void moveGenerator::line(int pieceSquare, int targetSquare, int direction)
{
    int searchSquare = pieceSquare;
    while(true)
    {
        searchSquare += direction;
        if(searchSquare == targetSquare)
        {
            allMoves[arraySize] = pieceSquare;
            allMoves[arraySize + 1] = targetSquare;
            arraySize += 2;
            break;
        }
        if(position[searchSquare] != 0) break;
    }
}

//Principal move manager

void moveGenerator::getLegalMoves(void)
{
    bool directCheck, revealedCheck;
    int toDiff, fromDiff, lastPieceType, boardSquare, i;
    
    directCheck = false;
    revealedCheck = false;
    
    //Initialise piece parameters
    if(whiteTurn)
    {
        pieceOrder = WHITE_PIECE_ORDER;
        kingSquare = positionalInformation[positionalInformation[WHITE_KING_INDEX]];
        if((lastToSquare == 7) & ((castlingRights&8) == 8))
        {
            castlingRights -= 8;
            positionalInformation[CASTLING] -= 8;
        }
        else if((lastToSquare == 0) & ((castlingRights&4) == 4))
        {
            castlingRights -= 4;
            positionalInformation[CASTLING] -= 4;
        }
        lastPieceType = positionalInformation[-position[lastToSquare] + 64];
    }
    else
    {
        pieceOrder = BLACK_PIECE_ORDER;
        kingSquare = positionalInformation[positionalInformation[BLACK_KING_INDEX]];
        if((lastToSquare == 119) & ((castlingRights&2) == 2))
        {
            castlingRights -= 2;
            positionalInformation[CASTLING] -= 2;
        }
        else if((lastToSquare == 112) & ((castlingRights&1) == 1))
        {
            castlingRights -= 1;
            positionalInformation[CASTLING] -= 1;
        }
        lastPieceType = positionalInformation[position[lastToSquare] + 64];
    }
if(lastToSquare != NULL_MOVE_FLAG)
{   
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
            if(whiteTurn) //Black rook moved
            {
                if((lastFromSquare == 112) & ((1&castlingRights) == 1)) positionalInformation[2] -= 1;
                else if((lastFromSquare == 119) & ((2&castlingRights) == 2)) positionalInformation[2] -= 2;
            }
            else //White rook moved
            {
                if((lastFromSquare == 0) & ((4&castlingRights) == 4)) positionalInformation[2] -= 4;
                else if((lastFromSquare == 7) & ((8&castlingRights) == 8)) positionalInformation[2] -= 8;
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
        case KING : //King
            toDiff = kingSquare - lastToSquare;
            if(whiteTurn) //Black king moved
            {
                if((2&castlingRights) == 2) positionalInformation[CASTLING] -= 2;
                if((1&castlingRights) == 1) positionalInformation[CASTLING] -= 1;
            }
            else 
            {
                if((8&castlingRights) == 8) positionalInformation[CASTLING] -= 8;
                if((4&castlingRights) == 4) positionalInformation[CASTLING] -= 4;
            }
            if(lastFromSquare < 0) //Castle
            {
                fromDiff = (lastToSquare - lastFromSquare)/2 - kingSquare;
                toDiff = lastToSquare + 16; //Rig toDiff
            }
            break;
    }
    //Search for revealed check:
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
        if(lastPieceType == PAWN && abs(lastToSquare - lastFromSquare) == 32) 
        {
            moveGenerator::enPassant();
        }
        if(whiteTurn)
        {
            if((8&castlingRights) == 8) moveGenerator::whiteCastling(1, 6, 7);
            if((4&castlingRights) == 4) moveGenerator::whiteCastling(-1, 2, 0);
        }
        else
        {
            if((2&castlingRights) == 2) moveGenerator::blackCastling(1, 118, 119);
            if((1&castlingRights) == 1) moveGenerator::blackCastling(-1, 114, 112);
        }
        //Set check parameter
        positionalInformation[IN_CHECK] = 0;
        getAllMoves();
    }
    else if(directCheck && revealedCheck) //King in double check
    {
        positionalInformation[IN_CHECK] = 1;
        if(whiteTurn) whiteKingMove();
        else blackKingMove();
        allMoves[arraySize] = -2;
    }
    else
    {
        if(directCheck)
        {
            if(lastPieceType == PAWN && abs(lastToSquare - lastFromSquare) == 32) 
            {
                moveGenerator::enPassant();
            }
        }
        //Set check parameter
        positionalInformation[IN_CHECK] = 1;
        getCheckMoves();
        allMoves[arraySize] = -2;
    }
}

//Move generation managers

void moveGenerator::whiteCastling(int direction, int toSquare, int rookSquare)
{
    int boardSquare, i, searchSquare, attackDirection, squareValue, pieceType;
    boardSquare = kingSquare + direction;
    bool obstruction = false;
    
    while(true)
    {
        if(boardSquare == rookSquare)
        {
            allMoves[arraySize] = -kingSquare;
            allMoves[arraySize + 1] = toSquare;
            arraySize += 2;
            break;
        }
        else
        {
            if(position[boardSquare] == 0)
            {
                //Check for pawn attackers
                for(i = 0; i < 2; i++)
                {
                    searchSquare = boardSquare + CASTLING_PAWN_ATTACKS[i];
                    if(position[searchSquare] < 0)
                    {
                        if(positionalInformation[-position[searchSquare] + 64] == 0)
                        {
                            obstruction = true;
                            break;
                        }
                    }
                }
                if(obstruction) break;
                //Check for sliding attacks
                for(i = 0; i < 3; i++)
                {
                    attackDirection = CASTLING_SLIDING_ATTACKS[i];
                    searchSquare = boardSquare;
                    while(true)
                    {
                        searchSquare += attackDirection;
                        if((136&searchSquare) == 0)
                        {
                            squareValue = position[searchSquare];
                            if(squareValue > 0) break;
                            else if(squareValue < 0) //Enemy piece found
                            {
                                pieceType = positionalInformation[-squareValue + 64];
                                if(pieceType == CASTLING_ATTACKERS1[i] || pieceType == CASTLING_ATTACKERS2[i])
                                {
                                   obstruction = true;
                                }
                                break;
                            }
                        }
                        else break;
                    }
                    if(obstruction) break;
                }
                if(obstruction) break;
                //Check for knight attackers
                for(i = 0; i < 4; i++)
                {
                    searchSquare = boardSquare + CASTLING_KNIGHT_ATTACKS[i];
                    if((136&searchSquare) == 0)
                    {
                        if(position[searchSquare] < 0)
                        {
                            if(positionalInformation[-position[searchSquare] + 64] == 1) obstruction = true;
                            break;
                        }
                    }
                }
            }
            else break;
            if(obstruction) break;
            else boardSquare += direction;
        }
    }
}

void moveGenerator::blackCastling(int direction, int toSquare, int rookSquare)
{
    int boardSquare, attackDirection, searchSquare, squareValue, pieceType, i;
    boardSquare = kingSquare + direction;
    bool obstruction = false;
    while(true)
    {
        if(boardSquare == rookSquare)
        {
            allMoves[arraySize] = -kingSquare;
            allMoves[arraySize + 1] = toSquare;
            arraySize += 2;
            break;
        }
        else
        {
            if(position[boardSquare] == 0)
            {
                //Check for pawn attackers
                for(i = 0; i < 2; i++)
                {
                    searchSquare = boardSquare - CASTLING_PAWN_ATTACKS[i];
                    if(position[searchSquare] > 0)
                    {
                        if(positionalInformation[position[searchSquare] + 64] == 0)
                        {
                            obstruction = true;
                            break;
                        }
                    }
                }
                if(obstruction) break;
                //Check for sliding attacks
                for(i = 0; i < 3; i++)
                {
                    attackDirection = -CASTLING_SLIDING_ATTACKS[i];
                    searchSquare = boardSquare;
                    while(true)
                    {
                        searchSquare += attackDirection;
                        if((136&searchSquare) == 0)
                        {
                            squareValue = position[searchSquare];
                            if(squareValue < 0) break;
                            else if(squareValue > 0) //Enemy piece found
                            {
                                pieceType = positionalInformation[squareValue + 64];
                                if(pieceType == CASTLING_ATTACKERS1[i] || pieceType == CASTLING_ATTACKERS2[i])
                                {
                                    obstruction = true;
                                }
                                break;
                            }
                        }
                        else break;
                    }
                    if(obstruction) break;
                }
                if(obstruction) break;
                //Check for knight attackers
                if(boardSquare != toSquare + direction) //Knights do not impede a queenside castle if they defend b1/b8
                {
                    for(int i = 0; i < 4; i++)
                    {
                        searchSquare = boardSquare - CASTLING_KNIGHT_ATTACKS[i];
                        if((136&searchSquare) == 0)
                        {
                            if(position[searchSquare] > 0)
                            {
                                if(positionalInformation[position[searchSquare] + 64] == 1) obstruction = true;
                                break;
                            }
                        }
                    }
                }
            }
            else break;
            if(obstruction) break;
            else boardSquare += direction;
        }
    }
}

bool moveGenerator::enPassantPin(int firstPawnSquare, int secondPawnSquare)
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

void moveGenerator::enPassant(void)
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
                            allMoves[arraySize] = -(lastToSquare + 1);
                            allMoves[arraySize + 1] = lastToSquare + turnSwitch*16;
                            arraySize += 2;
                        }
                    }
                    else
                    {
                        //Negative fromSquare indicated special move (here en passant)
                        allMoves[arraySize] = -(lastToSquare + 1);
                        allMoves[arraySize + 1] = lastToSquare + turnSwitch*16;
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
                            allMoves[arraySize] = -(lastToSquare - 1);
                            allMoves[arraySize + 1] = lastToSquare + turnSwitch*16;
                            arraySize += 2;
                        }
                    }
                    else
                    {
                        //Negative fromSquare indicated special move (here en passant)
                        allMoves[arraySize] = -(lastToSquare - 1);
                        allMoves[arraySize + 1] = lastToSquare + turnSwitch*16;
                        arraySize += 2;
                    }
                }
            }
        }
    }
}

void moveGenerator::shuffleMoves(int * previousRootNodes, int * refutationTable, int * pieceValues, int depth)
{
    int fromSquare, toSquare, shuffleRank;
    shuffleRank = 0;
    
#ifdef PV_MOVES
    if(refutationTable[16*depth + 15])
    {
        for(int i = 0; i < arraySize; i+=2)
        {
            if(allMoves[i] == refutationTable[16*depth + 13] and allMoves[i+1] == refutationTable[16*depth + 14])
            {
                allMoves[i] = allMoves[0];
                allMoves[i+1] = allMoves[1];
                allMoves[0] = refutationTable[16*depth + 13];
                allMoves[1] = refutationTable[16*depth + 14];
                shuffleRank += 2;
            }
        }
        refutationTable[16*depth + 15] = 0;
    }
#endif
    if(depth == 1) //Root nodes
    {
        for(int i = 0; i < arraySize; i+=2)
        {
            fromSquare = previousRootNodes[i];
            toSquare = previousRootNodes[i+1];
            for(int j = shuffleRank; j < arraySize; j+=2)
            {
                if(allMoves[j] == fromSquare && allMoves[j+1] == toSquare)
                {
                    allMoves[j] = allMoves[shuffleRank];
                    allMoves[j+1] = allMoves[shuffleRank + 1];
                    allMoves[shuffleRank] = fromSquare;
                    allMoves[shuffleRank + 1] = toSquare;
                    shuffleRank += 2;
                }
            }
        }
    }
    if(depth == 2)
    {
        int i = 0;
        while(previousRootNodes[i] != 0 && previousRootNodes[i+1] != 0)
        {
            if(lastFromSquare == previousRootNodes[i] && lastToSquare == previousRootNodes[i+1])
            {
                fromSquare = previousRootNodes[i + 256];
                toSquare = previousRootNodes[i + 257];
            }
            i += 2;
        }
        for(i = 0; i < arraySize; i+=2)
        {
            if(allMoves[i] == fromSquare && allMoves[i+1] == toSquare)
            {
                allMoves[i] = allMoves[shuffleRank];
                allMoves[i+1] = allMoves[shuffleRank + 1];
                allMoves[shuffleRank] = fromSquare;
                allMoves[shuffleRank + 1] = toSquare;
                shuffleRank += 2;
                break;
            }
        }
    }
#ifdef STATIC_EXCHANGE_EVALUATION
    int seeEvaluation;
    
    for(int i = shuffleRank; i < arraySize; i+=2)
    {
        if(position[allMoves[i+1]] != 0)
        {
            fromSquare = allMoves[i];
            toSquare = allMoves[i+1];
            seeEvaluation = SEE(fromSquare, toSquare);
            
            if((whiteTurn && seeEvaluation == WHITE_WINS) || (!whiteTurn && seeEvaluation == BLACK_WINS))
            {
                if(i != shuffleRank)
                {
                    allMoves[i] = allMoves[shuffleRank];
                    allMoves[i+1] = allMoves[shuffleRank+1];
                    allMoves[shuffleRank] = fromSquare;
                    allMoves[shuffleRank+1] = toSquare;
                }
                shuffleRank += 2;
            }
        }
    }
#else
    int bestMoveIndex;
    double score, bestScore;
    bool captureFound;
    bestScore = 1;
    captureFound = false;
    for(int i = shuffleRank; i < arraySize; i+=2)
    {
        if(position[allMoves[i+1]] != 0) //capture
        {
            score = abs(pieceValues[-turnSwitch*position[allMoves[i+1]]])/abs(pieceValues[turnSwitch*position[abs(allMoves[i])]]);
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
        fromSquare = allMoves[bestMoveIndex];
        toSquare = allMoves[bestMoveIndex+1];
        allMoves[bestMoveIndex] = allMoves[shuffleRank];
        allMoves[bestMoveIndex+1] = allMoves[shuffleRank + 1];
        allMoves[shuffleRank] = fromSquare;
        allMoves[shuffleRank + 1] = toSquare;
        shuffleRank += 2;   
    }
    for(int i = shuffleRank; i < arraySize; i+=2)
    {
        if(allMoves[i+1] == lastToSquare)
        {
            fromSquare = allMoves[i];
            toSquare = allMoves[i+1];
            allMoves[i] = allMoves[shuffleRank];
            allMoves[i+1] = allMoves[shuffleRank + 1];
            allMoves[shuffleRank] = fromSquare;
            allMoves[shuffleRank + 1] = toSquare;
            shuffleRank += 2;
            break;
        }
    }
#endif
#ifdef KILLERS
    int killerFromSquare, killerToSquare;
    for(int j = 0; j < KILLER_MOVES; j++)
    {
        killerFromSquare = refutationTable[16*depth + 2*j];
        killerToSquare = refutationTable[16*depth + 2*j + 1];
        for(int i = shuffleRank; i < arraySize; i+=2)
        {
            if(allMoves[i] == killerFromSquare and allMoves[i+1] == killerToSquare)
            {
                allMoves[i] = allMoves[shuffleRank];
                allMoves[i+1] = allMoves[shuffleRank + 1];
                allMoves[shuffleRank] = killerFromSquare;
                allMoves[shuffleRank + 1] = killerToSquare;
                shuffleRank += 2;
            }
        }
    }
#endif
}

void moveGenerator::getAllMoves(void)
{
    //Looping variables
    int status, pieceType, pieceStatusIndex;
    if(whiteTurn) //White to play
    {
        for(pieceStatusIndex = 0; pieceStatusIndex < 16; pieceStatusIndex++)
        {
            currentPiece = moveGenerator::pieceOrder[pieceStatusIndex];
            pieceType = positionalInformation[currentPiece + 64];
            switch(pieceType)
            {
                case PAWN : //Pawn
                    switch(pieceStatuses[currentPiece])
                    {
                        case 0 :
                            whitePawnMove(positionalInformation[currentPiece]);
                            break;
                        case -1 :
                            break;
                        case 15 :
                            whitePinnedPawnCapture(positionalInformation[currentPiece], positionalInformation[currentPiece] + 15);
                            break;
                        case 16 :
                            whitePawnAdvance(positionalInformation[currentPiece]);
                            break;
                        case 17 :
                            whitePinnedPawnCapture(positionalInformation[currentPiece], positionalInformation[currentPiece] + 17);
                            break;
                    }
                    break;
                case KNIGHT : //Knight
                    if(pieceStatuses[currentPiece] == 0) whiteKnightMove(positionalInformation[currentPiece]);
                    break;
                case BISHOP : //Bishop
                    status = pieceStatuses[currentPiece];
                    switch(status)
                    {
                        case 0 :
                            whiteBishopMove(positionalInformation[currentPiece]);
                            break;
                        case -1 :
                            break;
                        case 15 :
                        case 17 :
                            whiteSlidingMove(positionalInformation[currentPiece], status);
                            break;
                    }
                    break;
                case ROOK : //Rook
                    status = pieceStatuses[currentPiece];
                    switch(status)
                    {
                        case 0 :
                            whiteRookMove(positionalInformation[currentPiece]);
                            break;
                        case -1 :
                            break;
                        case 1 :
                        case 16 :
                            whiteSlidingMove(positionalInformation[currentPiece], status);
                            break;
                    }
                    break;
                case QUEEN : //Queen
                    status = pieceStatuses[currentPiece];
                    switch(status)
                    {
                        case 0 :
                            whiteQueenMove(positionalInformation[currentPiece]);
                            break;
                        case -1 :
                            break;
                        default :
                            whiteSlidingMove(positionalInformation[currentPiece], status);
                            break;
                    }
                    break;
                case KING : //King
                    whiteKingMove();
                    break;
            }
        }
    }
    else //Black turn
    {
        for(pieceStatusIndex = 0; pieceStatusIndex < 16; pieceStatusIndex++)
        {
            currentPiece = moveGenerator::pieceOrder[pieceStatusIndex];
            pieceType = positionalInformation[currentPiece + 64];
            switch(pieceType)
            {
                case PAWN : //Pawn
                    switch(pieceStatuses[currentPiece])
                    {
                        case 0 :
                            blackPawnMove(positionalInformation[currentPiece]);
                            break;
                        case -1 :
                            break;
                        case 15 :
                            blackPinnedPawnCapture(positionalInformation[currentPiece], positionalInformation[currentPiece] - 15);
                            break;
                        case 16 :
                            blackPawnAdvance(positionalInformation[currentPiece]);
                            break;
                        case 17 :
                            blackPinnedPawnCapture(positionalInformation[currentPiece], positionalInformation[currentPiece] - 17);
                            break;
                    }
                    break;
                case KNIGHT : //Knight
                    if(pieceStatuses[currentPiece] == 0) blackKnightMove(positionalInformation[currentPiece]);
                    break;
                case BISHOP : //Bishop
                    status = pieceStatuses[currentPiece];
                    switch(status)
                    {
                        case 0 :
                            blackBishopMove(positionalInformation[currentPiece]);
                            break;
                        case -1 :
                            break;
                        case 15 :
                        case 17 :
                            blackSlidingMove(positionalInformation[currentPiece], status);
                            break;
                    }
                    break;
                case ROOK : //Rook
                    status = pieceStatuses[currentPiece];
                    switch(status)
                    {
                        case 0 :
                            blackRookMove(positionalInformation[currentPiece]);
                            break;
                        case -1 :
                            break;
                        case 1 :
                        case 16 :
                            blackSlidingMove(positionalInformation[currentPiece], status);
                            break;
                    }
                    break;
                case QUEEN : //Queen
                    status = pieceStatuses[currentPiece];
                    switch(status)
                    {
                        case 0 :
                            blackQueenMove(positionalInformation[currentPiece]);
                            break;
                        case -1 :
                            break;
                        default :
                            blackSlidingMove(positionalInformation[currentPiece], status);
                            break;
                    }
                    break;
                case KING : //King
                    blackKingMove();
                    break;
            }
        }
    }
    //Mark end of move list
    allMoves[arraySize] = -2;
}

void moveGenerator::getCheckMoves(void)
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
                            allMoves[arraySize] = pieceSquare;
                            allMoves[arraySize + 1] = attackingSquare;
                            arraySize += 2;
                            break;
                        case 17 :
                            allMoves[arraySize] = pieceSquare;
                            allMoves[arraySize + 1] = attackingSquare;
                            arraySize += 2;
                            break;
                    }
                    for(square = attackingSquare + attackDirection; square != terminalSquare; square += attackDirection)
                    {
                        if(turnSwitch*(square - pieceSquare) == 16)
                        {
                            allMoves[arraySize] = pieceSquare;
                            allMoves[arraySize + 1] = square;
                            arraySize += 2;
                        }
                        else if(turnSwitch*(square - pieceSquare) == 32)
                        {
                            if(turnSwitch*pieceSquare < (24*whiteTurn - (1 - whiteTurn)*95) && position[pieceSquare + turnSwitch*16] == 0)
                            {
                                allMoves[arraySize] = pieceSquare;
                                allMoves[arraySize + 1] = square;
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
    allMoves[arraySize] = -2;
}

//Move generation functions

void moveGenerator::whitePawnMove(int pieceLocation)
{
    int nextMove;
    //Capturing moves
    for(int i = 0; i < 2; i++)
    {
        nextMove = pieceLocation + PAWN_MOVES[i];
        if((136&nextMove) == 0) //Move on board
        {
            if(position[nextMove] < 0) //Capturable piece
            {
                if(nextMove > 111)
                {
                    allMoves[arraySize] = -pieceLocation;
                    allMoves[arraySize + 1] = nextMove;
                    arraySize += 2;
                }
                else
                {
                    allMoves[arraySize] = pieceLocation;
                    allMoves[arraySize + 1] = nextMove;
                    arraySize += 2;
                }
            }
        }
    }
    //Advancing moves
    nextMove = pieceLocation + 16;
    if((nextMove&136) == 0) //Move on board
    {
        if(position[nextMove] == 0)
        {
            if(nextMove > 111)
            {
                allMoves[arraySize] = -pieceLocation;
                allMoves[arraySize + 1] = nextMove;
                arraySize += 2;
            }
            else
            {
                allMoves[arraySize] = pieceLocation;
                allMoves[arraySize + 1] = nextMove;
                arraySize += 2;
            }
            if(pieceLocation < 24) //Pawn has not yet moved
            {
                nextMove += 16;
                if(position[nextMove] == 0)
                {
                    allMoves[arraySize] = pieceLocation;
                    allMoves[arraySize + 1] = nextMove;
                    arraySize += 2;
                }
            }
        }
    }
}

void moveGenerator::blackPawnMove(int pieceLocation)
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
                    allMoves[arraySize] = -pieceLocation;
                    allMoves[arraySize + 1] = nextMove;
                    arraySize += 2;
                }
                else
                {
                    allMoves[arraySize] = pieceLocation;
                    allMoves[arraySize + 1] = nextMove;
                    arraySize += 2;
                }
            }
        }
    }
    //Advancing moves
    nextMove = pieceLocation - 16;
    if((nextMove&136) == 0) //Move on board
    {
        if(position[nextMove] == 0)
        {
            if(nextMove < 8)
            {
                allMoves[arraySize] = -pieceLocation;
                allMoves[arraySize + 1] = nextMove;
                arraySize += 2;
            }
            else
            {
                allMoves[arraySize] = pieceLocation;
                allMoves[arraySize + 1] = nextMove;
                arraySize += 2;
            }
            if(pieceLocation > 95) //Pawn has not yet moved
            {
                nextMove -= 16;
                if(position[nextMove] == 0)
                {
                    allMoves[arraySize] = pieceLocation;
                    allMoves[arraySize + 1] = nextMove;
                    arraySize += 2;
                }
            }
        }
    }   
}

void moveGenerator::whitePinnedPawnCapture(int pieceLocation, int nextMove)
{
    if((136&nextMove) == 0) //Move on board
    {
        if(position[nextMove] < 0) //Capturable piece
        {
            if(nextMove > 111)
            {
                allMoves[arraySize] = -pieceLocation;
                allMoves[arraySize + 1] = nextMove;
                arraySize += 2;
            }
            else
            {
                allMoves[arraySize] = pieceLocation;
                allMoves[arraySize + 1] = nextMove;
                arraySize += 2;
            }
        }
    }
}

void moveGenerator::blackPinnedPawnCapture(int pieceLocation, int nextMove)
{
    if((136&nextMove) == 0) //Move on board
    {
        if(position[nextMove] > 0) //Capturable piece
        {
            if(nextMove < 8)
            {
                allMoves[arraySize] = -pieceLocation;
                allMoves[arraySize + 1] = nextMove;
                arraySize += 2;
            }
            else 
            {
                allMoves[arraySize] = pieceLocation;
                allMoves[arraySize + 1] = nextMove;
                arraySize += 2;
            }
        }
    }
}

void moveGenerator::whitePawnAdvance(int pieceLocation)
{
    int nextMove = pieceLocation + 16;
    if((nextMove&136) == 0) //Move on board
    {
        if(position[nextMove] == 0)
        {
            if(nextMove > 111)
            {
                allMoves[arraySize] = -pieceLocation;
                allMoves[arraySize + 1] = nextMove;
                arraySize += 2;
            }
            else
            {
                allMoves[arraySize] = pieceLocation;
                allMoves[arraySize + 1] = nextMove;
                arraySize += 2;
            }
            if(pieceLocation < 24) //Pawn has not yet moved
            {
                nextMove += 16;
                if(position[nextMove] == 0)
                {
                    allMoves[arraySize] = pieceLocation;
                    allMoves[arraySize + 1] = nextMove;
                    arraySize += 2;
                }
            }
        }
    }
}

void moveGenerator::blackPawnAdvance(int pieceLocation)
{
    int nextMove = pieceLocation - 16;
    if((nextMove&136) == 0) //Move on board
    {
        if(position[nextMove] == 0)
        {
            if(nextMove < 8)
            {
                allMoves[arraySize] = -pieceLocation;
                allMoves[arraySize + 1] = nextMove;
                arraySize += 2;
            }
            else
            {
                allMoves[arraySize] = pieceLocation;
                allMoves[arraySize + 1] = nextMove;
                arraySize += 2;
            }
            if(pieceLocation > 95) //Pawn has not yet moved
            {
                nextMove -= 16;
                if(position[nextMove] == 0)
                {
                    allMoves[arraySize] = pieceLocation;
                    allMoves[arraySize + 1] = nextMove;
                    arraySize += 2;
                }
            }
        }
    }
}

void moveGenerator::whiteKnightMove(int pieceLocation)
{
    int nextMove;
    for(int i = 0; i < 8; i++)
    {
        nextMove = pieceLocation + KNIGHT_MOVES[i];
        if((nextMove&136) == 0)
        {
            if(position[nextMove] <= 0)
            {
                allMoves[arraySize] = pieceLocation;
                allMoves[arraySize + 1] = nextMove;
                arraySize += 2;
            }
        }
    }
}

void moveGenerator::blackKnightMove(int pieceLocation)
{
    int nextMove;
    for(int i = 0; i < 8; i++)
    {
        nextMove = pieceLocation + KNIGHT_MOVES[i];
        if((nextMove&136) == 0)
        {
            if(position[nextMove] >= 0)
            {
                allMoves[arraySize] = pieceLocation;
                allMoves[arraySize + 1] = nextMove;
                arraySize += 2;
            }
        }
    }
}

void moveGenerator::whiteBishopMove(int pieceLocation)
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
                if(position[boardSquare] <= 0)
                {
                    allMoves[arraySize] = pieceLocation;
                    allMoves[arraySize + 1] = boardSquare;
                    arraySize += 2;
                }
                if(position[boardSquare] != 0) break;
            }
            else break;
        }
    }
}

void moveGenerator::whiteRookMove(int pieceLocation)
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
                if(position[boardSquare] <= 0)
                {
                    allMoves[arraySize] = pieceLocation;
                    allMoves[arraySize + 1] = boardSquare;
                    arraySize += 2;
                }
                if(position[boardSquare] != 0) break;
            }
            else break;
        }
    }
}

void moveGenerator::whiteQueenMove(int pieceLocation)
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
                if(position[boardSquare] <= 0)
                {
                    allMoves[arraySize] = pieceLocation;
                    allMoves[arraySize + 1] = boardSquare;
                    arraySize += 2;
                }
                if(position[boardSquare] != 0) break;
            }
            else break;
        }
    }
}

void moveGenerator::blackBishopMove(int pieceLocation)
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
                if(position[boardSquare] >= 0)
                {
                    allMoves[arraySize] = pieceLocation;
                    allMoves[arraySize + 1] = boardSquare;
                    arraySize += 2;
                }
                if(position[boardSquare] != 0) break;
            }
            else break;
        }
    }
}

void moveGenerator::blackRookMove(int pieceLocation)
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
                if(position[boardSquare] >= 0)
                {
                    allMoves[arraySize] = pieceLocation;
                    allMoves[arraySize + 1] = boardSquare;
                    arraySize += 2;
                }
                if(position[boardSquare] != 0) break;
            }
            else break;
        }
    }
}

void moveGenerator::blackQueenMove(int pieceLocation)
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
                if(position[boardSquare] >= 0)
                {
                    allMoves[arraySize] = pieceLocation;
                    allMoves[arraySize + 1] = boardSquare;
                    arraySize += 2;
                }
                if(position[boardSquare] != 0) break;
            }
            else break;
        }
    }
}

void moveGenerator::whiteKingMove(void)
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
                    allMoves[arraySize] = kingSquare;
                    allMoves[arraySize + 1] = boardSquare;
                    arraySize += 2;
                }
            }
        }
        check = false;
    }
}

void moveGenerator::blackKingMove(void)
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
                    allMoves[arraySize] = kingSquare;
                    allMoves[arraySize + 1] = boardSquare;
                    arraySize += 2;
                }
            }
        }
        check = false;
    }
}

void moveGenerator::whiteSlidingMove(int pieceLocation, int pieceStatus)
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
                    allMoves[arraySize] = pieceLocation;
                    allMoves[arraySize + 1] = boardSquare;
                    arraySize += 2;
                }
                if(position[boardSquare] != 0) break;
            }
            else break;
        }
        pieceStatus = -pieceStatus;
    }
}

void moveGenerator::blackSlidingMove(int pieceLocation, int pieceStatus)
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
                    allMoves[arraySize] = pieceLocation;
                    allMoves[arraySize + 1] = boardSquare;
                    arraySize += 2;
                }
                if(position[boardSquare] != 0) break;
            }
            else break;
        }
        pieceStatus = -pieceStatus;
    }
}

void moveGenerator::displayPosition(void)
{
    cout << "\n\n";
    for(int j = 7; j >= 0; j--)
    {
        cout << "|";
        for(int k = 0; k < 8; k++)
        {
            int squareValue = moveGenerator::position[j*16 + k];
            if(squareValue != 0)
            {
                int pieceType = moveGenerator::positionalInformation[abs(moveGenerator::position[j*16 + k]) + 64];
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

void moveGenerator::displayPositionalInformation(void)
{
    for(int i = 7; i >= 0; i--)
    {
        cout << "|";
        for(int j = 0; j < 16; j++)
        {
            cout << positionalInformation[16*i + j];
            if(positionalInformation[16*i + j] >= 100)
            {
                cout << "|";
            }
            else if(positionalInformation[16*i + j] >= 10)
            {
                cout << " |";
            }
            else if(positionalInformation[16*i + j] >= 0)
            {
                cout << "  |";
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

//int moveGenerator::SEE(int fromSquare, int captureSquare)
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
//              if(captureLayer[i] > 0 && capturerTypes[i] != -1)
//              {
//                  if(capturerTypes[i] < capturingPieceType)
//                  {
//                      capturingPieceType = capturerTypes[i];
//                      currentPieceIndex = i;
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
//              if(captureLayer[i] < 0 && capturerTypes[i] != -1)
//              {
//                  if(capturerTypes[i] < capturingPieceType)
//                  {
//                      capturingPieceType = capturerTypes[i];
//                      currentPieceIndex = i;
//                  }
//              }
//          }
//          if(capturingPieceType > KING) //No capture found
//          {
//              if(numBlackKnights > 0)
//              {
//                  pieceType = KNIGHT;
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
//}
