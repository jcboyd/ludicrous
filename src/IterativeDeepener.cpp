#include "IterativeDeepener.h"

iterativeDeepener::iterativeDeepener(int* position, int* positionalInformation, int* pieceValues)
{
    iterativeDeepener::position = position;
    iterativeDeepener::positionalInformation = positionalInformation;
    iterativeDeepener::pieceValues = pieceValues;
    iterativeDeepener::baseEvaluation = positionalInformation[EVALUATION];
    iterativeDeepener::lastFromSquare = positionalInformation[LAST_FROM_SQUARE];
    iterativeDeepener::lastToSquare = positionalInformation[LAST_TO_SQUARE];
    for(int i = 0; i < 1024; i++) iterativeDeepener::rootNodes[i] = 0;
    max_time = 3;
}

iterativeDeepener::~iterativeDeepener(void) {}

void iterativeDeepener::iterativeDeepening(double time_limit)
{
    clock_t start_time = clock();
    clock_t end_time;
    clock_t clock_ticks_taken;
    double time_in_seconds;
    int eval;


    
    for(int i = 1; i <= maximumDepth; i++)
    {
        positionalInformation[LAST_FROM_SQUARE] = lastFromSquare;
        positionalInformation[LAST_TO_SQUARE] = lastToSquare;
        search::search newSearch = search::search(position, positionalInformation, pieceValues, i, baseEvaluation, positionalInformation[ENDGAME]);
        newSearch.setPrincipalVariation(iterativeDeepener::principalVariation);
        newSearch.setPreviousRootNodes(rootNodes);
        
        eval = newSearch.initiateSearch();
        end_time = clock();
        clock_ticks_taken = end_time - start_time;
        time_in_seconds = clock_ticks_taken/((double)CLOCKS_PER_SEC);
        
        recordPrincipalVariation(newSearch.getPVTable(), i);
        
        iterativeDeepener::principalFromSquare = iterativeDeepener::principalVariation[0];
        iterativeDeepener::principalToSquare = iterativeDeepener::principalVariation[1];
        if(displayAnalysis)
        {
            cout << "t" << time_in_seconds << " : ";
            cout << "d" << i << " : " << eval/100.0 << " : ";
            iterativeDeepener::displayPrincipalVariation(i);
        }
        newSearch.copyRootNodes(iterativeDeepener::rootNodes);
        iterativeDeepener::sortRootNodes();
        
        if(i == maximumDepth and time_in_seconds < max_time) maximumDepth++;
    }
}

void iterativeDeepener::setMaxTime(double maximumTime)
{
    max_time = maximumTime;
}

void iterativeDeepener::sortRootNodes(void)
{
    int turnSwitch, nodeFrom, nodeTo, killerFrom, killerTo, evaluation, numNodes, nodesSorted, bestIndex, bestEval, i;
    turnSwitch = -1 + 2 * positionalInformation[WHITE_TURN];
    
    numNodes = 0;
    while(rootNodes[2*numNodes] != 0 && rootNodes[2*numNodes + 1] != 0)
    {
        numNodes++;
    }
    nodesSorted = 0;
    
    bestIndex = 0;
    
    while(nodesSorted < numNodes)
    {
        bestEval = -turnSwitch*MATERIAL_VALUES[KING];
        for(i = nodesSorted; i < numNodes; i++)
        {
            evaluation = rootNodes[2*i + 512];
            if(turnSwitch*evaluation > turnSwitch*bestEval)
            {
                bestIndex = i;
                bestEval = evaluation;
            }
        }
        nodeFrom = rootNodes[2*bestIndex];
        nodeTo = rootNodes[2*bestIndex + 1];
        killerFrom = rootNodes[2*bestIndex + 256];
        killerTo = rootNodes[2*bestIndex + 257];
        rootNodes[2*bestIndex] = rootNodes[2*nodesSorted];
        rootNodes[2*bestIndex + 1] = rootNodes[2*nodesSorted + 1];
        rootNodes[2*bestIndex + 256] = rootNodes[2*nodesSorted + 256];
        rootNodes[2*bestIndex + 257] = rootNodes[2*nodesSorted + 257];
        rootNodes[2*bestIndex + 512] = rootNodes[2*nodesSorted + 512];
        rootNodes[2*nodesSorted] = nodeFrom;
        rootNodes[2*nodesSorted + 1] = nodeTo;
        rootNodes[2*nodesSorted + 256] = killerFrom;
        rootNodes[2*nodesSorted + 257] = killerTo;
        rootNodes[2*nodesSorted + 512] = bestEval;
        nodesSorted++;
    }
}

void iterativeDeepener::setDisplayAnalysis(bool displayAnalysisSwitch)
{
    iterativeDeepener::displayAnalysis = displayAnalysisSwitch;
}

void iterativeDeepener::recordPrincipalVariation(int* PVTable, int length)
{
    //Construct principal variation
    for(int i = 0; i < 512; i++)
    {
        iterativeDeepener::principalVariation[i] = 0;
    }
    
    for(int i = 0; i < length; i++)
    {
        iterativeDeepener::principalVariation[2*i] = PVTable[(length - 1)*34 - 2*i];
        iterativeDeepener::principalVariation[2*i+1] = PVTable[(length - 1)*34 - 2*i + 1];
    }
}

void iterativeDeepener::displayPrincipalVariation(int length)
{
    for(int i = 0; i < 2*length; i += 2)
    {
        int fromSquare = abs(principalVariation[i]);
        int toSquare = principalVariation[i+1];
        string move;
        if(fromSquare < 0) //Special move
        {
            if(fromSquare == -1) //Castling
            {
                if(toSquare == 6 || toSquare == 118)
                {
                    cout << "O-O ";
                }
                else
                {
                    cout << "O-O-O ";
                }

            }
            else if(fromSquare == -2) //Pawn promotion
            {
            }
            else //En passant capture
            {
                move += BOARD_SQUARE_NAMES[fromSquare];
                move += BOARD_SQUARE_NAMES[fromSquare + 8];
                move += BOARD_SQUARE_NAMES[toSquare];
                move += BOARD_SQUARE_NAMES[toSquare + 8];
                cout << move;
                cout << " ";
            }
        }
        else 
        {
            move += BOARD_SQUARE_NAMES[fromSquare];
            move += BOARD_SQUARE_NAMES[fromSquare + 8];
            move += BOARD_SQUARE_NAMES[toSquare];
            move += BOARD_SQUARE_NAMES[toSquare + 8];
            cout << move;
            cout << " ";
        }
    }
    cout << "\n";
}

void iterativeDeepener::displayPosition(void)
{
    cout << "\n";
    for(int j = 7; j >= 0; j--)
    {
        cout << "|";
        for(int k = 0; k < 8; k++)
        {
            int squareValue = iterativeDeepener::position[j*16 + k];
            if(squareValue != 0)
            {
                int pieceType = iterativeDeepener::positionalInformation[abs(iterativeDeepener::position[j*16 + k]) + 64];
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

void iterativeDeepener::reset(int* position, int* positionalInformation, int* pieceValues, int maximumDepth)
{
    iterativeDeepener::position = position;
    iterativeDeepener::positionalInformation = positionalInformation;
    iterativeDeepener::pieceValues = pieceValues;
    iterativeDeepener::maximumDepth = maximumDepth;
    iterativeDeepener::lastFromSquare = positionalInformation[LAST_FROM_SQUARE];
    iterativeDeepener::lastToSquare = positionalInformation[LAST_TO_SQUARE];
    iterativeDeepener::baseEvaluation = positionalInformation[EVALUATION];
}

int iterativeDeepener::getPrincipalFromSquare(void)
{
    return iterativeDeepener::principalFromSquare;
}

int iterativeDeepener::getPrincipalToSquare(void)
{
    return iterativeDeepener::principalToSquare;
}