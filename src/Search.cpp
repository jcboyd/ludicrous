#include <iostream>
#include "Search.h"
#include "MoveGenerator.h"
#include "QMoveGenerator.h"
#include "Globals.h"

using namespace std;


Search::Search(int* initialPosition, int* initialPositionalInformation, int* pieceValues, int depth, int lastEvaluation, bool endgame)
{
    this->position = initialPosition;
    this->positionalInformation = initialPositionalInformation;
    this->pieceValues = pieceValues;
    this->searchDepth = depth;
    this->currentDepth = 0;
    this->currentEvaluation = lastEvaluation;
    this->numberOfEvals = 0;
    this->numberOfQEvals = 0;
    this->numberOfGeneratedMoves = 0;
    this->numberOfQGeneratedMoves = 0;
    this->lastNumberOfEvals = 0;
    for(int i = 0; i < 128; i++)
    {
        this->positionalEvaluations[i] = 0;
        this->captures[i] = 0;
        this->specials[i] = 0;
        this->pawnPromotions[i] = 0;
        this->qMovesCounts[i] = 0;
        //for(int j = 0; j < 128; j++)
//      {
//          hhTable[i][j] = 0;
//      }
    }
    this->initialisePieceSquareTables(endgame);
    this->currentPieceSquareTables = 0;
    this->rootNodesRecorded = 0;
    for(int i = 0; i < 512; i++)
    {
        this->refutationTable[i] = 0;
        this->PVTable[i] = 0;
        this->rootNodes[2*i] = 0;
        this->rootNodes[2*i+1] = 0;
    }   
}

Search::~Search(void)
{
}

void Search::setPreviousRootNodes(int* previousRootNodes)
{
    for(int i = 0; i < 1024; i++)
    {
        this->previousRootNodes[i] = previousRootNodes[i];
    }
}

void Search::setPrincipalVariation(int* lastPrincipalVariation)
{
    for(int i = 0; i < searchDepth; i++)
    {
        this->PVTable[32*(searchDepth - 1) + 2*(searchDepth - i - 1)] = lastPrincipalVariation[2*i];
        this->PVTable[32*(searchDepth - 1) + 2*(searchDepth - i - 1) + 1] = lastPrincipalVariation[2*i+1];
        this->refutationTable[16*i + 13] = lastPrincipalVariation[2*i];
        this->refutationTable[16*i + 14] = lastPrincipalVariation[2*i+1];
        this->refutationTable[16*i + 15] = 1;
    }       
}

void Search::initialisePieceSquareTables(bool endgame)
{
    if(endgame)
    {
        this->whitePieceSquareTables[PAWN] = WPE_SQUARES;
        this->whitePieceSquareTables[KING] = WKE_SQUARES;
        this->blackPieceSquareTables[PAWN] = BPE_SQUARES;
        this->blackPieceSquareTables[KING] = BKE_SQUARES;
    }
    else 
    {
        this->whitePieceSquareTables[PAWN] = WP_SQUARES;
        this->whitePieceSquareTables[KING] = WK_SQUARES;
        this->blackPieceSquareTables[PAWN] = BP_SQUARES;
        this->blackPieceSquareTables[KING] = BK_SQUARES;
    }

    this->whitePieceSquareTables[KNIGHT] = WN_SQUARES;
    this->whitePieceSquareTables[BISHOP] = WB_SQUARES;
    this->whitePieceSquareTables[ROOK] = WR_SQUARES;
    this->whitePieceSquareTables[QUEEN] = WQ_SQUARES;
    this->whitePieceSquareTables[6] = 0;
    this->whitePieceSquareTables[7] = 0;
    
    this->blackPieceSquareTables[KNIGHT] = BN_SQUARES;
    this->blackPieceSquareTables[BISHOP] = BB_SQUARES;
    this->blackPieceSquareTables[ROOK] = BR_SQUARES;
    this->blackPieceSquareTables[QUEEN] = BQ_SQUARES;
    this->blackPieceSquareTables[6] = 0;
    this->blackPieceSquareTables[7] = 0;
}

int Search::initiateSearch(void)
{
    if(positionalInformation[WHITE_TURN] == 1) //White turn
    {
        return treeSearchMax(searchDepth, -INF, INF);
    }
    else //Black turn
    {
        return treeSearchMin(searchDepth, -INF, INF);
    }
}

void Search::copyRootNodes(int * copyRootNodes)
{
    for(int i = 0; i < 1024; i++)
    {
        copyRootNodes[i] = this->rootNodes[i];
    }
}

int Search::maxFastEvaluate(int fromSquare, int toSquare)
{
    currentPieceSquareTables = whitePieceSquareTables[positionalInformation[position[fromSquare] + 64]];
    return currentEvaluation + currentPieceSquareTables[toSquare] - currentPieceSquareTables[fromSquare];
}

int Search::minFastEvaluate(int fromSquare, int toSquare)
{
    currentPieceSquareTables = blackPieceSquareTables[positionalInformation[-position[fromSquare] + 64]];
    return currentEvaluation + currentPieceSquareTables[toSquare] - currentPieceSquareTables[fromSquare];
}

int Search::treeSearchMax(int depth, int maxBest, int minBest)
{
    int arraySize, evaluation, castling, allMoves[256];
    
    MoveGenerator generator(position, positionalInformation, allMoves);
    generator.getLegalMoves();
    generator.shuffleMoves(previousRootNodes, refutationTable, pieceValues, currentDepth + 1);
    arraySize = generator.getArraySize();
    
    if(arraySize == 0)
    {
        if(positionalInformation[IN_CHECK]) return -MATERIAL_VALUES[KING];
        return 0;
    }
    if(positionalInformation[IN_CHECK] and (depth + currentDepth) < searchDepth) depth++;
    
    currentDepth++;
    castling = positionalInformation[CASTLING];
    
    if(depth == 1)
    {
        for(int i = 0; i < arraySize; i += 2)
        {
            if(allMoves[i] < 0 || position[allMoves[i+1]] != 0)
            {
                positionalInformation[LAST_FROM_SQUARE] = allMoves[i];
                positionalInformation[LAST_TO_SQUARE] = allMoves[i+1];
                maxPlayMove(allMoves[i], allMoves[i+1]);
                evaluation = qSearchMin(maxBest, minBest);
                maxInvPlayMove(allMoves[i], allMoves[i+1]);
                positionalInformation[CASTLING] = castling;
            }
            else evaluation = maxFastEvaluate(allMoves[i], allMoves[i+1]);
            
            if(evaluation >= minBest)
            {
                updateRefutationTable(currentDepth, allMoves[i], allMoves[i+1]);
                currentDepth--;
                return minBest;
            }
            if(evaluation > maxBest)
            {
                maxBest = evaluation;
                updatePVTable(depth, allMoves[i], allMoves[i+1]);
            }
        }
    }
    else
    {
        if(depth > 3 && ! positionalInformation[IN_CHECK] && ! positionalInformation[ENDGAME])
        {
            positionalInformation[WHITE_TURN] = ! positionalInformation[WHITE_TURN];
            positionalInformation[LAST_FROM_SQUARE] = positionalInformation[positionalInformation[WHITE_KING_INDEX]];
            positionalInformation[LAST_TO_SQUARE] = NULL_MOVE_FLAG;
            evaluation = treeSearchMin(depth - 3, maxBest, minBest);
            if(evaluation >= minBest)
            {
                currentDepth--;
                return minBest;
            }
            positionalInformation[WHITE_TURN] = ! positionalInformation[WHITE_TURN];
            positionalInformation[CASTLING] = castling;
        }
        for(int i = 0; i < arraySize; i+=2)
        {
            positionalInformation[LAST_FROM_SQUARE] = allMoves[i];
            positionalInformation[LAST_TO_SQUARE] = allMoves[i+1];
            
            if(i > LATE_MOVE_INDEX && depth > 2 && position[positionalInformation[LAST_TO_SQUARE]] == 0)
            {
                maxPlayMove(allMoves[i], allMoves[i+1]);
                evaluation = treeSearchMin(depth - 2, maxBest, minBest);
                maxInvPlayMove(allMoves[i], allMoves[i+1]);
                positionalInformation[LAST_FROM_SQUARE] = allMoves[i];
                positionalInformation[LAST_TO_SQUARE] = allMoves[i+1];
                positionalInformation[CASTLING] = castling;
            }
            else evaluation = maxBest + 1;
            
            if(evaluation > maxBest)
            {
                maxPlayMove(allMoves[i], allMoves[i+1]);
                evaluation = treeSearchMin(depth - 1, maxBest, minBest);
                maxInvPlayMove(allMoves[i], allMoves[i+1]);
            }
            positionalInformation[CASTLING] = castling;
            
            if(evaluation >= minBest)
            {
                updateRefutationTable(currentDepth, allMoves[i], allMoves[i+1]);
                currentDepth--;
                return minBest;
            }
            if(evaluation > maxBest)
            {
                maxBest = evaluation;
                updatePVTable(depth, allMoves[i], allMoves[i+1]);
            }
            if(depth == searchDepth) recordRootNodes(allMoves[i], allMoves[i+1], evaluation);
        }
    }
    currentDepth--;
    return maxBest;
}

int Search::treeSearchMin(int depth, int maxBest, int minBest)
{
    int arraySize, evaluation, castling, allMoves[256];
    
    MoveGenerator generator(position, positionalInformation, allMoves);
    generator.getLegalMoves();
    generator.shuffleMoves(previousRootNodes, refutationTable, pieceValues, currentDepth + 1);
    arraySize = generator.getArraySize();
    
    if(arraySize == 0)
    {
        if(positionalInformation[IN_CHECK]) return MATERIAL_VALUES[KING];
        return 0;
    }
    if(positionalInformation[IN_CHECK] and searchDepth > (depth + currentDepth)) depth++;
    
    currentDepth++;
    castling = positionalInformation[CASTLING];
    
    if(depth == 1)
    {
        for(int i = 0; i < arraySize; i+=2)
        {
            if(allMoves[i] < 0 || position[allMoves[i+1]] != 0)
            {
                positionalInformation[LAST_FROM_SQUARE] = allMoves[i];
                positionalInformation[LAST_TO_SQUARE] = allMoves[i+1];
                minPlayMove(allMoves[i], allMoves[i+1]);
                evaluation = qSearchMax(maxBest, minBest);
                minInvPlayMove(allMoves[i], allMoves[i+1]);
                positionalInformation[CASTLING] = castling;
            }
            else evaluation = minFastEvaluate(allMoves[i], allMoves[i+1]);
            
            if(evaluation <= maxBest)
            {
                updateRefutationTable(currentDepth, allMoves[i], allMoves[i+1]);
                currentDepth--;
                return maxBest;
            }
            if(evaluation < minBest)
            {
                minBest = evaluation;
                updatePVTable(depth, allMoves[i], allMoves[i+1]);
            }
        }
    }
    else
    {
        if(depth > 3 && ! positionalInformation[IN_CHECK] && ! positionalInformation[ENDGAME])
        {
            positionalInformation[WHITE_TURN] = ! positionalInformation[WHITE_TURN];
            positionalInformation[LAST_FROM_SQUARE] = positionalInformation[positionalInformation[BLACK_KING_INDEX]];
            positionalInformation[LAST_TO_SQUARE] = NULL_MOVE_FLAG;
            evaluation = treeSearchMax(depth - 3, maxBest, minBest);
            if(evaluation <= maxBest)
            {
                currentDepth--;
                return maxBest;
            }
            positionalInformation[WHITE_TURN] = ! positionalInformation[WHITE_TURN];
            positionalInformation[CASTLING] = castling;
        }
        for(int i = 0; i < arraySize; i+=2)
        {
            positionalInformation[LAST_FROM_SQUARE] = allMoves[i];
            positionalInformation[LAST_TO_SQUARE] = allMoves[i+1];
            
            if(i > LATE_MOVE_INDEX && depth > 2 && position[positionalInformation[LAST_TO_SQUARE]] == 0)
            {
                minPlayMove(allMoves[i], allMoves[i+1]);
                evaluation = treeSearchMax(depth - 2, maxBest, minBest);
                minInvPlayMove(allMoves[i], allMoves[i+1]);
                positionalInformation[LAST_FROM_SQUARE] = allMoves[i];
                positionalInformation[LAST_TO_SQUARE] = allMoves[i+1];
                positionalInformation[CASTLING] = castling;
            }
            else evaluation = minBest - 1;
            
            if(evaluation < minBest)
            {
                minPlayMove(allMoves[i], allMoves[i+1]);
                evaluation = treeSearchMax(depth - 1, maxBest, minBest);
                minInvPlayMove(allMoves[i], allMoves[i+1]);
            }
            positionalInformation[CASTLING] = castling;
            
            if(evaluation <= maxBest)
            {
                updateRefutationTable(currentDepth, allMoves[i], allMoves[i+1]);
                currentDepth--;
                return maxBest;
            }
            if(evaluation < minBest)
            {
                minBest = evaluation;
                updatePVTable(depth, allMoves[i], allMoves[i+1]);
            }
            if(depth == searchDepth) recordRootNodes(allMoves[i], allMoves[i+1], evaluation);
        }
    }
    currentDepth--;
    return minBest;
}

void Search::recordRootNodes(int fromSquare, int toSquare, int evaluation)
{
    rootNodes[rootNodesRecorded*2] = fromSquare;
    rootNodes[rootNodesRecorded*2 + 1] = toSquare;
    rootNodes[rootNodesRecorded*2 + 256] = refutationTable[32 + 2*((refutationTable[40] - 1 + KILLER_MOVES)%KILLER_MOVES)];
    rootNodes[rootNodesRecorded*2 + 257] = refutationTable[32 + 2*((refutationTable[40] - 1 + KILLER_MOVES)%KILLER_MOVES) + 1];
    rootNodes[rootNodesRecorded*2 + 512] = evaluation;
    rootNodesRecorded++;
}

bool Search::checkCurrentVariation(int fromSquare, int toSquare, int moveIndex)
{
    if(currentVariation[moveIndex*2] == fromSquare and currentVariation[moveIndex*2 + 1] == toSquare) return true;
    return false;
}

void Search::printMove(int fromSquare, int toSquare)
{
    cout << "(" << BOARD_SQUARE_NAMES[fromSquare] << BOARD_SQUARE_NAMES[fromSquare + 8] << ", ";
    cout << BOARD_SQUARE_NAMES[toSquare] << BOARD_SQUARE_NAMES[toSquare + 8] << ") : ";
}

void Search::updatePVTable(int depth, int fromSquare, int toSquare)
{
    for(int j = 0; j < depth - 1; j++)
    {
        this->PVTable[32*depth + 2*j - 32] = this->PVTable[32*depth + 2*j - 64];
        this->PVTable[32*depth + 2*j - 31] = this->PVTable[32*depth + 2*j - 63];
    }
    this->PVTable[34*depth - 34] = fromSquare;
    this->PVTable[34*depth - 33] = toSquare;
}

void Search::updateRefutationTable(int depth, int fromSquare, int toSquare)
{
    refutationTable[16*depth + 2*refutationTable[16*depth + 8]] = fromSquare;
    refutationTable[16*depth + 2*refutationTable[16*depth + 8] + 1] = toSquare;
    refutationTable[16*depth + 8] = (refutationTable[16*depth + 8] + 1)%KILLER_MOVES;
}

void Search::maxPlayMove(int fromSquare, int toSquare)
{
    if(fromSquare < 0) this->maxPlaySpecialMove(-fromSquare, toSquare); //Special move
    else //Normal move
    {
        if(position[toSquare] != 0) //Capture
        {
            //Record capture
            captures[currentDepth] = position[toSquare];
            //Deduct material and positional value
            currentEvaluation -= pieceValues[-position[toSquare]];
            //Flag captured piece
            positionalInformation[-position[toSquare]] = -1;
#ifdef EVALUATE_PAWN_STRUCTURE
            if(positionalInformation[position[fromSquare] + 64] == PAWN) currentEvaluation -= DOUBLE_PAWN_PENALTY;
#endif
        }
        currentPieceSquareTables = whitePieceSquareTables[positionalInformation[position[fromSquare] + 64]];
        positionalEvaluations[currentDepth] = currentPieceSquareTables[toSquare] - currentPieceSquareTables[fromSquare];
        pieceValues[position[fromSquare]] += positionalEvaluations[currentDepth];
        currentEvaluation += positionalEvaluations[currentDepth];
        position[toSquare] = position[fromSquare];
        position[fromSquare] = 0;
        positionalInformation[position[toSquare]] = toSquare;
    }
    positionalInformation[WHITE_TURN] = 0;
}

void Search::maxPlaySpecialMove(int fromSquare, int toSquare)
{
    if(toSquare > 111) //Pawn promotion
    {
        if(position[toSquare] != 0) //Capture
        {
            //Record capture
            captures[currentDepth] = position[toSquare];
            //Deduct material and positional value
            currentEvaluation -= pieceValues[-position[toSquare]];
            //Flag captured piece
            positionalInformation[-position[toSquare]] = -1;
        }
        //pawnPromotions[currentDepth]++;
        //currentPieceSquareTables = whitePieceSquareTables[pawnPromotions[currentDepth]];
        pawnPromotions[currentDepth] = QUEEN;
        positionalEvaluations[currentDepth] = WQ_SQUARES[toSquare] - WP_SQUARES[fromSquare];
        positionalEvaluations[currentDepth] += MATERIAL_VALUES[QUEEN] - MATERIAL_VALUES[PAWN];
        pieceValues[position[fromSquare]] += positionalEvaluations[currentDepth];
        currentEvaluation += positionalEvaluations[currentDepth];
        position[toSquare] = position[fromSquare];
        position[fromSquare] = 0;
        positionalInformation[position[toSquare]] = toSquare;
        //Change piece type
        positionalInformation[position[toSquare] + 64] = QUEEN;
        specials[currentDepth] = 0;
    }
    else if(abs(toSquare - fromSquare) == 2) //Castling
    {
        if(toSquare == 6) //Kingside castle
        {
            //Move king's rook
            position[5] = position[7];
            position[7] = 0;
            positionalInformation[position[5]] = 5;
            pieceValues[position[5]] += WR_SQUARES[5] - WR_SQUARES[7];
            currentEvaluation += WR_SQUARES[5] - WR_SQUARES[7];
        }
        else //Queenside castle
        {
            //Move queen's rook
            position[3] = position[0];
            position[0] = 0;
            positionalInformation[position[3]] = 3;
            pieceValues[position[3]] += WR_SQUARES[3] - WR_SQUARES[0];
            currentEvaluation += WR_SQUARES[3] - WR_SQUARES[0];
        }
        position[toSquare] = position[fromSquare];
        position[fromSquare] = 0;
        positionalInformation[position[toSquare]] = toSquare;
        currentEvaluation += whitePieceSquareTables[5][toSquare] - whitePieceSquareTables[5][fromSquare];
        specials[currentDepth] = 1;
        //Note there is no need to update the king's positional value because it will never be captured.
    }
    else //En passant capture
    {
        //Record capture
        captures[currentDepth] = position[toSquare - 16];
        //Deduct material and positional value
        currentEvaluation -= pieceValues[-position[toSquare - 16]];
        //Flag captured piece
        positionalInformation[-position[toSquare - 16]] = -1;
        //Free board square
        position[toSquare - 16] = 0;
        //Move capturing piece
        position[toSquare] = position[fromSquare];
        position[fromSquare] = 0;
        positionalInformation[position[toSquare]] = toSquare;
        positionalEvaluations[currentDepth] = whitePieceSquareTables[PAWN][toSquare] - whitePieceSquareTables[PAWN][fromSquare];
        pieceValues[position[toSquare]] += positionalEvaluations[currentDepth];
        currentEvaluation += positionalEvaluations[currentDepth];
        specials[currentDepth] = 2;
    }
}

void Search::minPlayMove(int fromSquare, int toSquare)
{
    if(fromSquare < 0) minPlaySpecialMove(-fromSquare, toSquare); //Special move
    else //Normal move
    {
        if(position[toSquare] != 0) //Capture
        {
            //Record capture
            captures[currentDepth] = position[toSquare];
            //Deduct material and positional value
            currentEvaluation -= pieceValues[position[toSquare]];
            //Flag captured piece
            positionalInformation[position[toSquare]] = -1;
#ifdef EVALUATE_PAWN_STRUCTURE
            if(positionalInformation[-position[fromSquare] + 64] == PAWN) currentEvaluation -= -DOUBLE_PAWN_PENALTY;
#endif
        }
        currentPieceSquareTables = blackPieceSquareTables[positionalInformation[-position[fromSquare] + 64]];
        positionalEvaluations[currentDepth] = currentPieceSquareTables[toSquare] - currentPieceSquareTables[fromSquare];
        pieceValues[-position[fromSquare]] += positionalEvaluations[currentDepth];
        currentEvaluation += positionalEvaluations[currentDepth];
        position[toSquare] = position[fromSquare];
        position[fromSquare] = 0;
        positionalInformation[-position[toSquare]] = toSquare;
    }
    positionalInformation[WHITE_TURN] = 1;
}

void Search::minPlaySpecialMove(int fromSquare, int toSquare)
{
    if(toSquare < 8) //Pawn promotion
    {
        if(position[toSquare] != 0) //Capture
        {
            //Record capture
            captures[currentDepth] = position[toSquare];
            //Deduct material and positional value
            currentEvaluation -= pieceValues[position[toSquare]];
            //Flag captured piece
            positionalInformation[position[toSquare]] = -1;
        }
        //pawnPromotions[currentDepth]++;
        //currentPieceSquareTables = blackPieceSquareTables[pawnPromotions[currentDepth]];
        pawnPromotions[currentDepth] = QUEEN;
        positionalEvaluations[currentDepth] = BQ_SQUARES[toSquare] - BP_SQUARES[fromSquare];
        positionalEvaluations[currentDepth] += -MATERIAL_VALUES[QUEEN] + MATERIAL_VALUES[PAWN];
        pieceValues[-position[fromSquare]] += positionalEvaluations[currentDepth];
        currentEvaluation += positionalEvaluations[currentDepth];
        position[toSquare] = position[fromSquare];
        position[fromSquare] = 0;
        positionalInformation[-position[toSquare]] = toSquare;
        //Change piece type
        positionalInformation[-position[toSquare] + 64] = QUEEN;
        specials[currentDepth] = 0;
    }
    else if(abs(toSquare - fromSquare) == 2) //Castling
    {
        if(toSquare == 118) //Kingside castle
        {
            //Move king's rook
            position[117] = position[119];
            position[119] = 0;
            positionalInformation[-position[117]] = 117;
            pieceValues[-position[117]] += BR_SQUARES[117] - BR_SQUARES[119];
            currentEvaluation += BR_SQUARES[117] - BR_SQUARES[119];
        }
        else //Queenside castle 
        {
            //Move queen's rook
            position[115] = position[112];
            position[112] = 0;
            positionalInformation[-position[115]] = 115;
            pieceValues[-position[115]] += BR_SQUARES[115] - BR_SQUARES[112];
            currentEvaluation += BR_SQUARES[115] - BR_SQUARES[112];
        }
        position[toSquare] = position[fromSquare];
        position[fromSquare] = 0;
        positionalInformation[-position[toSquare]] = toSquare;
        currentEvaluation += blackPieceSquareTables[5][toSquare] - blackPieceSquareTables[5][fromSquare];
        specials[currentDepth] = 1;
        //Note there is no need to update the king's positional value because it will never be captured.
    }
    else //En passant capture
    {
        //Record capture
        captures[currentDepth] = position[toSquare + 16];
        //Deduct material and positional value
        currentEvaluation -= pieceValues[position[toSquare + 16]];
        //Flag captured piece
        positionalInformation[position[toSquare + 16]] = -1;
        //Free board square
        position[toSquare + 16] = 0;
        //Move capturing piece
        position[toSquare] = position[fromSquare];
        position[fromSquare] = 0;
        positionalInformation[-position[toSquare]] = toSquare;
        positionalEvaluations[currentDepth] = blackPieceSquareTables[PAWN][toSquare] - blackPieceSquareTables[PAWN][fromSquare];
        pieceValues[-position[toSquare]] += positionalEvaluations[currentDepth];
        currentEvaluation += positionalEvaluations[currentDepth];
        specials[currentDepth] = 2;
    }
}

void Search::maxInvPlayMove(int fromSquare, int toSquare)
{
    if(fromSquare < 0) this->maxInvPlaySpecialMove(-fromSquare, toSquare);//Special move 
    else //Normal move
    {
        positionalInformation[position[toSquare]] = fromSquare;
        position[fromSquare] = position[toSquare];
        pieceValues[position[fromSquare]] -= positionalEvaluations[currentDepth];
        currentEvaluation -= positionalEvaluations[currentDepth];
        if(captures[currentDepth] != 0)
        {
            position[toSquare] = captures[currentDepth];
            positionalInformation[-captures[currentDepth]] = toSquare;
            currentEvaluation += pieceValues[-position[toSquare]];
            captures[currentDepth] = 0;
#ifdef EVALUATE_PAWN_STRUCTURE
            if(positionalInformation[position[fromSquare] + 64] == PAWN) currentEvaluation += DOUBLE_PAWN_PENALTY;
#endif
        }
        else position[toSquare] = 0;
    }
    positionalInformation[WHITE_TURN] = 1;
}

void Search::maxInvPlaySpecialMove(int fromSquare, int toSquare)
{
    if(specials[currentDepth] == 0)
    {
        position[fromSquare] = position[toSquare];
        positionalInformation[position[fromSquare]] = fromSquare;
        positionalInformation[position[fromSquare] + 64] = PAWN;
        pieceValues[position[fromSquare]] -= positionalEvaluations[currentDepth];
        currentEvaluation -= positionalEvaluations[currentDepth];
        if(captures[currentDepth] != 0)
        {
            position[toSquare] = captures[currentDepth];
            positionalInformation[-captures[currentDepth]] = toSquare;
            currentEvaluation += pieceValues[-position[toSquare]];
            captures[currentDepth] = 0;
        }
        else position[toSquare] = 0;
        pawnPromotions[currentDepth] = 0;
    }
    else if(specials[currentDepth] == 1)
    {
        if(toSquare == 6) //Kingside castle
        {
            //Move king's rook
            position[7] = position[5];
            position[5] = 0;
            positionalInformation[position[7]] = 7;
            pieceValues[position[7]] += WR_SQUARES[7] - WR_SQUARES[5];
            currentEvaluation += WR_SQUARES[7] - WR_SQUARES[5];
        }
        else //Queenside castle
        {
            //Move queen's rook
            position[0] = position[3];
            position[3] = 0;
            positionalInformation[position[0]] = 0;
            pieceValues[position[0]] += WR_SQUARES[0] - WR_SQUARES[3];
            currentEvaluation += WR_SQUARES[0] - WR_SQUARES[3];
        }
        position[fromSquare] = position[toSquare];
        position[toSquare] = 0;
        positionalInformation[position[fromSquare]] = fromSquare;
        currentEvaluation += whitePieceSquareTables[5][fromSquare] - whitePieceSquareTables[5][toSquare];
    }
    else
    {
        //Restore captured pawn
        position[toSquare - 16] = captures[currentDepth];
        positionalInformation[-captures[currentDepth]] = toSquare - 16;
        currentEvaluation += pieceValues[-captures[currentDepth]];
        captures[currentDepth] = 0;
        position[fromSquare] = position[toSquare];
        position[toSquare] = 0;
        positionalInformation[position[fromSquare]] = fromSquare;
        pieceValues[position[fromSquare]] -= positionalEvaluations[currentDepth];
        currentEvaluation -= positionalEvaluations[currentDepth];
    }
    specials[currentDepth] = 0;
}

void Search::minInvPlayMove(int fromSquare, int toSquare)
{
    if(fromSquare < 0) minInvPlaySpecialMove(-fromSquare, toSquare); //Special move
    else //Normal move
    {
        positionalInformation[-position[toSquare]] = fromSquare;
        position[fromSquare] = position[toSquare];
        pieceValues[-position[fromSquare]] -= positionalEvaluations[currentDepth];
        currentEvaluation -= positionalEvaluations[currentDepth];
        if(captures[currentDepth] != 0)
        {
            position[toSquare] = captures[currentDepth];
            positionalInformation[captures[currentDepth]] = toSquare;
            currentEvaluation += pieceValues[position[toSquare]];
            captures[currentDepth] = 0;
#ifdef EVALUATE_PAWN_STRUCTURE
            if(positionalInformation[-position[fromSquare] + 64] == PAWN) currentEvaluation += -DOUBLE_PAWN_PENALTY;
#endif
        }
        else position[toSquare] = 0;
    }
    positionalInformation[WHITE_TURN] = 0;
}

void Search::minInvPlaySpecialMove(int fromSquare, int toSquare)
{
    if(specials[currentDepth] == 0)
    {
        position[fromSquare] = position[toSquare];
        positionalInformation[-position[fromSquare]] = fromSquare;
        positionalInformation[-position[fromSquare] + 64] = PAWN;
        pieceValues[-position[fromSquare]] -= positionalEvaluations[currentDepth];
        currentEvaluation -= positionalEvaluations[currentDepth];
        if(captures[currentDepth] != 0)
        {
            position[toSquare] = captures[currentDepth];
            positionalInformation[captures[currentDepth]] = toSquare;
            currentEvaluation += pieceValues[position[toSquare]];
            captures[currentDepth] = 0;
        }
        else position[toSquare] = 0;
        pawnPromotions[currentDepth] = 0;
    }
    else if(specials[currentDepth] == 1)
    {
        if(toSquare == 118) //Kingside castle
        {
            //Move king's rook
            position[119] = position[117];
            position[117] = 0;
            positionalInformation[-position[119]] = 119;
            pieceValues[-position[119]] += BR_SQUARES[119] - BR_SQUARES[117];
            currentEvaluation += BR_SQUARES[119] - BR_SQUARES[117];
        }
        else //Queenside castle 
        {
            //Move queen's rook
            position[112] = position[115];
            position[115] = 0;
            positionalInformation[-position[112]] = 112;
            pieceValues[-position[112]] += BR_SQUARES[112] - BR_SQUARES[115];
            currentEvaluation += BR_SQUARES[112] - BR_SQUARES[115];
        }
        position[fromSquare] = position[toSquare];
        position[toSquare] = 0;
        positionalInformation[-position[fromSquare]] = fromSquare;
        currentEvaluation += blackPieceSquareTables[5][fromSquare] - blackPieceSquareTables[5][toSquare];
    }
    else 
    {
        position[toSquare + 16] = captures[currentDepth];
        positionalInformation[captures[currentDepth]] = toSquare + 16;
        currentEvaluation += pieceValues[captures[currentDepth]];
        captures[currentDepth] = 0;
        position[fromSquare] = position[toSquare];
        position[toSquare] = 0;
        positionalInformation[-position[fromSquare]] = fromSquare;
        pieceValues[-position[fromSquare]] -= positionalEvaluations[currentDepth];
        currentEvaluation -= positionalEvaluations[currentDepth];
    }
    specials[currentDepth] = 0;
}

int Search::getBestFromMove(void)
{
    return this->PVTable[34*this->searchDepth - 34];
}

int Search::getBestToMove(void)
{
    return this->PVTable[34*this->searchDepth - 33];
}

long Search::getNumberOfEvals(void)
{
    return this->numberOfEvals;
}

long Search::getNumberOfQEvals(void)
{
    return numberOfQEvals;
}

long Search::getNumberOfGeneratedMoves(void)
{
    return this->numberOfGeneratedMoves;
}

long Search::getNumberOfQGeneratedMoves(void)
{
    return this->numberOfQGeneratedMoves;
}

void Search::displayPosition(void)
{
    cout << "\n+ - + - + - + - + - + - + - + - +\n";
    for(int j = 7; j >= 0; j--)
    {
        cout << "| ";
        for(int k = 0; k < 8; k++)
        {
            int squareValue = this->position[j*16 + k];
            if(squareValue != 0)
            {
                int pieceType = this->positionalInformation[abs(this->position[j*16 + k]) + 64];
                if(squareValue > 0)
                {
                    cout << WHITE_PIECE_NAMES[pieceType];
                }
                else
                {
                    cout << BLACK_PIECE_NAMES[pieceType];
                }
                cout << " | ";      
            }
            else
            {
                cout << "  | ";
            }           
        }       
        cout << "\n+ - + - + - + - + - + - + - + - +\n";
    }
}

void Search::displayPVTable(void)
{
    cout << "\n\n";
    for(int j = 16; j >= 0; j--)
    {
        cout << "|";
        for(int k = 0; k < 32; k++)
        {
            cout << this->PVTable[j*32 + k];
            cout << "|";
        }
        cout << "\n";
    }
}

int* Search::getPVTable(void)
{
    return this->PVTable;
}

//Standing pat is a sort of advanced prune which relies on the principle that
//the player to move can always at least equal the static score. Hence we
//record a new lower bound if necessary.
int Search::qSearchMax(int maxBest, int minBest)
{
    if(currentEvaluation >= minBest) return minBest;
    if(currentEvaluation > maxBest) maxBest = currentEvaluation;
    
    int allCaptures[128];
    QMoveGenerator generator(this->position, this->positionalInformation, allCaptures);
    generator.getLegalCaptures();
#ifdef NODE_ORDERING
    generator.shuffleMoves(pieceValues, currentDepth + 1);
#endif
    int arraySize = generator.getArraySize();
    this->numberOfQGeneratedMoves += arraySize/2;
    if(arraySize == 0) 
    {
        if(positionalInformation[IN_CHECK]) return -MATERIAL_VALUES[KING];
        return currentEvaluation;
    }
    currentDepth++;
    for(int i = 0 ; i < arraySize; i += 2)
    {
#ifdef DEBUG
        this->currentVariation[2*(currentDepth - 1)] = allCaptures[i];
        this->currentVariation[2*currentDepth - 1] = allCaptures[i+1];
#endif
        this->positionalInformation[LAST_FROM_SQUARE] = allCaptures[i];
        this->positionalInformation[LAST_TO_SQUARE] = allCaptures[i+1];
        maxPlayMove(allCaptures[i], allCaptures[i+1]);
        int evaluation = qSearchMin(maxBest, minBest);
        if(evaluation > maxBest) maxBest = evaluation;
        maxInvPlayMove(allCaptures[i], allCaptures[i+1]);
        if(evaluation >= minBest)
        {
            currentDepth--;
            return minBest;
        }
    }
    currentDepth--;
    return maxBest;
}

int Search::qSearchMin(int maxBest, int minBest)
{   
    if(currentEvaluation <= maxBest) return maxBest;
    if(currentEvaluation < minBest) minBest = currentEvaluation;
    
    int allCaptures[128];
    QMoveGenerator generator(this->position, this->positionalInformation, allCaptures);
    generator.getLegalCaptures();
#ifdef NODE_ORDERING
    generator.shuffleMoves(pieceValues, currentDepth + 1);
#endif
    int arraySize = generator.getArraySize();
    this->numberOfQGeneratedMoves += arraySize/2;
    if(arraySize == 0) 
    {
        if(positionalInformation[IN_CHECK]) return MATERIAL_VALUES[KING];
        return currentEvaluation;
    }
    currentDepth++;
    for(int i = 0 ; i < arraySize; i += 2)
    {
#ifdef DEBUG
        this->currentVariation[2*(currentDepth - 1)] = allCaptures[i];
        this->currentVariation[2*currentDepth - 1] = allCaptures[i+1];
#endif
        this->positionalInformation[LAST_FROM_SQUARE] = allCaptures[i];
        this->positionalInformation[LAST_TO_SQUARE] = allCaptures[i+1];
        minPlayMove(allCaptures[i], allCaptures[i+1]);
        int evaluation = qSearchMax(maxBest, minBest);
        if(evaluation < minBest) minBest = evaluation;
        minInvPlayMove(allCaptures[i], allCaptures[i+1]);
        
        if(evaluation <= maxBest)
        {
            currentDepth--;
            return maxBest;
        }
    }
    currentDepth--;
    return minBest;
}

void Search::displayPositionalInformation(void)
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

void Search::displayPieceValues(void)
{
    cout << "\n";
    for(int i = 7; i >= 0; i--)
    {
        cout << "|";
        for(int j = 0; j < 16; j++)
        {
            cout << this->pieceValues[16*i + j];
            cout << "\t|";
        }
        cout << "\n";
    }
}

void Search::displayRefutationTable(void)
{
    cout << "\n";
    for(int i = 7; i >= 0; i--)
    {
        cout << "|";
        for(int j = 0; j < 16; j++)
        {
            cout << this->refutationTable[16*i + j];
            cout << "\t|";
        }
        cout << "\n";
    }
}

void Search::displayAll(void)
{
    displayPosition();
    displayPositionalInformation();
    displayPieceValues();
}

void Search::displayCurrentVariation(int depth)
{
    for(int i = 0; i < depth; i++)
    {
        cout << "(" << currentVariation[2*i] << ", " << currentVariation[2*i + 1] << ")";
    }
}

//int Search::treeSearchMax2(int depth, int maxBest, int minBest)
//{
//  int evaluation, i, arraySize, castling, allMoves[256];
//  
//  MoveGenerator generator(this->position, this->positionalInformation, allMoves);
//  generator.getLegalMoves();
//  arraySize = generator.getArraySize();
//#ifdef NODE_ORDERING
//  generator.shuffleMoves(previousRootNodes, refutationTable, pieceValues, currentDepth + 1);
//#endif
//  this->numberOfGeneratedMoves += arraySize/2;
//  
//  if(arraySize == 0) //Checkmate or stalemate
//  {
//      if(this->positionalInformation[IN_CHECK]) //Player in check - checkmate
//      {
//          return -MATERIAL_VALUES[KING];
//      }
//      else return 0; //Evaluation
//  }
//  
//  if(positionalInformation[IN_CHECK] and searchDepth > (depth + currentDepth))
//  {
//      depth++;
//  }
//  
//  currentDepth++;
//  castling = positionalInformation[CASTLING];
//  
//  if(depth == 1) //Leaf nodes reached
//  {
//      for(i = 0; i < arraySize; i += 2)
//      {
//          if(allMoves[i] >= 0)
//          {
//              if(position[allMoves[i+1]] == 0) //No capture, hence stop branching
//              {
//                  currentPieceSquareTables = whitePieceSquareTables[positionalInformation[position[allMoves[i]] + 64]];
//                  evaluation = currentEvaluation + currentPieceSquareTables[allMoves[i+1]] - currentPieceSquareTables[allMoves[i]];
//                  numberOfEvals++;
//              }
//              else
//              {
//#ifdef DEBUG
//                  this->currentVariation[2*currentDepth - 2] = allMoves[i];
//                  this->currentVariation[2*currentDepth - 1] = allMoves[i+1];
//#endif
//                  this->positionalInformation[LAST_FROM_SQUARE] = allMoves[i];
//                  this->positionalInformation[LAST_TO_SQUARE] = allMoves[i+1];
//                  this->maxPlayMove(allMoves[i], allMoves[i+1]);
//                  evaluation = this->qSearchMin(maxBest, minBest);
//                  this->maxInvPlayMove(allMoves[i], allMoves[i+1]);
//              }
//          }
//          else //Special move
//          {
//              this->maxPlayMove(allMoves[i], allMoves[i+1]);
//              evaluation = currentEvaluation;
//              numberOfEvals++;
//              this->maxInvPlayMove(allMoves[i], allMoves[i+1]);
//          }
//#ifdef ALPHA_BETA
//          if(evaluation >= minBest)
//          {
//              updateRefutationTable(currentDepth, allMoves[i], allMoves[i+1]);
//              currentDepth--;
//              return minBest;
//          }
//#endif
//          if(evaluation > maxBest)
//          {
//              maxBest = evaluation;
//              this->updatePVTable(depth, allMoves[i], allMoves[i+1]);
//          }
//#ifdef PERFT
//          if(this->searchDepth == 1)
//          {
//              printMove(allMoves[i], allMoves[i+1]);
//              cout << 1 << "\n";
//          }
//#endif
//      }
//  }
//  else
//  {
//      i = 0;
//      
//      if(depth > 3 and ! positionalInformation[IN_CHECK])
//      {
//          
//          this->positionalInformation[WHITE_TURN] = !this->positionalInformation[WHITE_TURN];
//          this->positionalInformation[LAST_FROM_SQUARE] = positionalInformation[positionalInformation[WHITE_KING_INDEX]];
//          this->positionalInformation[LAST_TO_SQUARE] = -100;
//          evaluation = this->treeSearchMin(depth - 3, maxBest, minBest);
//          if(evaluation >= minBest)
//          {
//              currentDepth--;
//              return minBest;
//          }
//          positionalInformation[CASTLING] = castling;
//      }
//      
//      
//      while(true)
//      {
//          if(allMoves[i] == -1)
//          {
//              generator.getAllMoves(); //End of heuristic moves
//          }
//          else if(allMoves[i] == -2) 
//          {
//              if(i != arraySize)
//              {
//                  currentDepth--;
//              }
//              break; //End of allMoves
//          }
//          else
//          {
//#ifdef DEBUG
//              this->currentVariation[2*currentDepth - 2] = allMoves[i];
//              this->currentVariation[2*currentDepth - 1] = allMoves[i+1];
//#endif
//              //this->positionalInformation[LAST_FROM_SQUARE] = allMoves[i];
//              //this->positionalInformation[LAST_TO_SQUARE] = allMoves[i+1];
//              //this->maxPlayMove(allMoves[i], allMoves[i+1]);
//              
//              
//              if(i > LATE_MOVE_INDEX and depth > 2 and position[abs(allMoves[i+1])] == 0)
//              {
//                  this->positionalInformation[LAST_FROM_SQUARE] = allMoves[i];
//                  this->positionalInformation[LAST_TO_SQUARE] = allMoves[i+1];
//                  this->maxPlayMove(allMoves[i], allMoves[i+1]);
//                  evaluation = this->treeSearchMin(depth - 2, maxBest, minBest);
//                  this->maxInvPlayMove(allMoves[i], allMoves[i+1]);
//                  positionalInformation[CASTLING] = castling;
//              }
//              else
//              {
//                  evaluation = maxBest + 1;
//              }
//              
//              if(evaluation > maxBest)
//              {
//                  this->positionalInformation[LAST_FROM_SQUARE] = allMoves[i];
//                  this->positionalInformation[LAST_TO_SQUARE] = allMoves[i+1];
//                  this->maxPlayMove(allMoves[i], allMoves[i+1]);
//                  evaluation = this->treeSearchMin(depth - 1, maxBest, minBest);
//                  this->maxInvPlayMove(allMoves[i], allMoves[i+1]);
//                  positionalInformation[CASTLING] = castling;
//              }
//              
//              
//              //evaluation = this->treeSearchMin(depth - 1, maxBest, minBest);
//              
//              
//              
//              
//              
//              
//              //this->maxInvPlayMove(allMoves[i], allMoves[i+1]);
//              //positionalInformation[CASTLING] = castling; //Restore castling information
//#ifdef ALPHA_BETA
//              if(evaluation >= minBest)
//              {
//                  updateRefutationTable(currentDepth, allMoves[i], allMoves[i+1]);
//                  currentDepth--;
//                  return minBest;
//              }
//#endif
//              if(evaluation > maxBest)
//              {
//                  maxBest = evaluation;
//                  this->updatePVTable(depth, allMoves[i], allMoves[i+1]);
//              }
//#ifdef PERFT
//              if(searchDepth - depth == 0)
//              {
//                  printMove(abs(allMoves[i]), allMoves[i+1]);
//                  cout << numberOfEvals - lastNumberOfEvals << "\n";
//                  lastNumberOfEvals = numberOfEvals;
//              }
//#endif
//              if(depth == searchDepth) //Root node
//              {
//                  recordRootNodes(allMoves[i], allMoves[i+1], evaluation);
//              }
//              i += 2;
//          }
//      }
//  }
//  currentDepth--;
//  return maxBest;
//}
//
//int Search::treeSearchMin2(int depth, int maxBest, int minBest)
//{
//  int evaluation, i, arraySize, castling, allMoves[256];
//  
//  MoveGenerator generator(this->position, this->positionalInformation, allMoves);
//  generator.getLegalMoves();
//#ifdef NODE_ORDERING
//  generator.shuffleMoves(previousRootNodes, refutationTable, pieceValues, currentDepth + 1);
//#endif
//  arraySize = generator.getArraySize();
//  this->numberOfGeneratedMoves += arraySize/2;
//  
//  if(arraySize == 0) //Checkmate  or stalemate
//  {
//      if(this->positionalInformation[IN_CHECK]) return MATERIAL_VALUES[KING];
//      else return 0;
//  }
//  
//  if(positionalInformation[IN_CHECK] and searchDepth > (depth + currentDepth)) depth++;
//  
//  currentDepth++;
//  castling = positionalInformation[CASTLING];
//  
//  if(depth == 1) //Leaf nodes reached
//  {
//      for(i = 0; i < arraySize; i += 2)
//      {
//          if(allMoves[i] >= 0)
//          {
//              if(position[allMoves[i+1]] == 0) //No capture, hence stop branching
//              {
//                  currentPieceSquareTables = blackPieceSquareTables[positionalInformation[-position[allMoves[i]] + 64]];
//                  evaluation = currentEvaluation + currentPieceSquareTables[allMoves[i+1]] - currentPieceSquareTables[allMoves[i]];
//                  numberOfEvals++;
//              }
//              else
//              {
//#ifdef DEBUG
//                  this->currentVariation[2*currentDepth - 2] = allMoves[i];
//                  this->currentVariation[2*currentDepth - 1] = allMoves[i+1];
//#endif
//                  this->positionalInformation[LAST_FROM_SQUARE] = allMoves[i];
//                  this->positionalInformation[LAST_TO_SQUARE] = allMoves[i+1];
//                  this->minPlayMove(allMoves[i], allMoves[i+1]);
//                  evaluation = this->qSearchMax(maxBest, minBest);
//                  this->minInvPlayMove(allMoves[i], allMoves[i+1]);
//              }
//          }
//          else //Special move
//          {
//              this->minPlayMove(allMoves[i], allMoves[i+1]);
//              evaluation = currentEvaluation;
//              numberOfEvals++;
//              this->minInvPlayMove(allMoves[i], allMoves[i+1]);
//          }
//#ifdef ALPHA_BETA
//          if(evaluation <= maxBest) 
//          {
//              updateRefutationTable(currentDepth, allMoves[i], allMoves[i+1]);
//              currentDepth--;
//              return maxBest;
//          }
//#endif
//          if(evaluation < minBest)
//          {
//              minBest = evaluation;
//              this->updatePVTable(depth, allMoves[i], allMoves[i+1]);
//          }
//#ifdef PERFT
//          if(this->searchDepth == 1)
//          {
//              printMove(allMoves[i], allMoves[i+1]);
//              cout << 1 << "\n";
//          }
//#endif            
//      }
//  }
//  else
//  {
//      if(depth > 3 and ! positionalInformation[IN_CHECK])
//      {
//          this->positionalInformation[WHITE_TURN] = !this->positionalInformation[WHITE_TURN];
//          this->positionalInformation[LAST_FROM_SQUARE] = positionalInformation[positionalInformation[BLACK_KING_INDEX]];
//          this->positionalInformation[LAST_TO_SQUARE] = -100;
//          evaluation = this->treeSearchMax(depth - 3, maxBest, minBest);
//          if(evaluation <= maxBest)
//          {
//              currentDepth--;
//              return maxBest;
//          }
//          positionalInformation[CASTLING] = castling;
//      }
//      
//      
//      i = 0;
//      while(true)
//      {
//          if(allMoves[i] == -1) 
//          {
//              generator.getAllMoves(); //End of killer moves
//          }
//          else if(allMoves[i] == -2)
//          {
//              if(i != arraySize)
//              {
//                  currentDepth--;
//              }
//              break; //End of all moves
//          }
//          else
//          {
//#ifdef DEBUG
//              this->currentVariation[2*currentDepth - 2] = allMoves[i];
//              this->currentVariation[2*currentDepth - 1] = allMoves[i+1];
//#endif
//              //this->positionalInformation[LAST_FROM_SQUARE] = allMoves[i];
//              //this->positionalInformation[LAST_TO_SQUARE] = allMoves[i+1];
//              //this->minPlayMove(allMoves[i], allMoves[i+1]);
//              
//              if(i > LATE_MOVE_INDEX and depth > 2 and position[abs(allMoves[i+1])] == 0)
//              {
//                  this->positionalInformation[LAST_FROM_SQUARE] = allMoves[i];
//                  this->positionalInformation[LAST_TO_SQUARE] = allMoves[i+1];
//                  this->minPlayMove(allMoves[i], allMoves[i+1]);
//                  evaluation = this->treeSearchMax(depth - 2, maxBest, minBest);
//                  this->minInvPlayMove(allMoves[i], allMoves[i+1]);
//                  positionalInformation[CASTLING] = castling;
//              }
//              else
//              {
//                  evaluation = minBest - 1;
//              }
//              
//              if(evaluation < minBest)
//              {
//                  this->positionalInformation[LAST_FROM_SQUARE] = allMoves[i];
//                  this->positionalInformation[LAST_TO_SQUARE] = allMoves[i+1];
//                  this->minPlayMove(allMoves[i], allMoves[i+1]);
//                  evaluation = this->treeSearchMax(depth - 1, maxBest, minBest);
//                  this->minInvPlayMove(allMoves[i], allMoves[i+1]);
//                  positionalInformation[CASTLING] = castling;
//              }
//              
//              //evaluation = this->treeSearchMax(depth - 1, maxBest, minBest);
//              
//              
//              //this->minInvPlayMove(allMoves[i], allMoves[i+1]);
//              //positionalInformation[CASTLING] = castling; //Restore castling information
//#ifdef ALPHA_BETA
//              if(evaluation <= maxBest)
//              {
//                  updateRefutationTable(currentDepth, allMoves[i], allMoves[i+1]);
//                  currentDepth--;
//                  return maxBest;
//              }
//#endif
//              if(evaluation < minBest)
//              {
//                  minBest = evaluation;
//                  
//                  this->updatePVTable(depth, allMoves[i], allMoves[i+1]);
//              }
//#ifdef PERFT
//              if(depth == searchDepth)
//              {
//                  printMove(abs(allMoves[i]), allMoves[i+1]);
//                  cout << numberOfEvals - lastNumberOfEvals << "\n";
//                  lastNumberOfEvals = numberOfEvals;
//              }
//#endif
//              if(depth == searchDepth) //Root node
//              {
//                  recordRootNodes(allMoves[i], allMoves[i+1], evaluation);
//              }
//              i += 2;
//          }
//      }
//  }
//  currentDepth--;
//  return minBest;
//}

//if(checkCurrentVariation(102, 86, 0) and checkCurrentVariation(35, 86, 1) and checkCurrentVariation(68, 52, 2))
//  {
//if(checkCurrentVariation(19, 52, 3) and checkCurrentVariation(101, 86, 4) and checkCurrentVariation(50, 66, 5))
//  {
//if(checkCurrentVariation(116, 52, 6) and checkCurrentVariation(67, 82, 7) and checkCurrentVariation(99, 67, 8))
//  {
//if(checkCurrentVariation(-66, 83, 9))
//  {
//      displayPosition();
//      displayPositionalInformation();
//      displayPieceValues();
//  }
//  }
//  }
//  }

//if(checkCurrentVariation(115, 100, 0) and checkCurrentVariation(50, 67, 1) and checkCurrentVariation(116, 99, 2))
//  {
//      if(checkCurrentVariation(37, 70, 3) and checkCurrentVariation(99, 83, 4) and checkCurrentVariation(5, 65, 5))
//      {
//          if(checkCurrentVariation(103, 71, 6) and checkCurrentVariation(3, 71, 7) and checkCurrentVariation(85, 71, 8))
//          {
//              if(checkCurrentVariation(19, 49, 9) and checkCurrentVariation(98, 66, 10) and checkCurrentVariation(-67, 82, 11))
//              {
//                  displayPosition();
//                  displayPositionalInformation();
//                  displayPieceValues();
//              }
//          }
//      }
//  }
