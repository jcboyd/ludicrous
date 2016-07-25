#include <iostream>
#include <string>
#include "IterativeDeepener.h"
#include "Search.h"
#include "Globals.h"

using namespace std;


IterativeDeepener::IterativeDeepener(int* position, int* positionalInformation, int* pieceValues)
{
    this->position = position;
    this->positionalInformation = positionalInformation;
    this->pieceValues = pieceValues;
    this->baseEvaluation = positionalInformation[EVALUATION];
    this->lastFromSquare = positionalInformation[LAST_FROM_SQUARE];
    this->lastToSquare = positionalInformation[LAST_TO_SQUARE];
    for(int i = 0; i < 1024; i++) this->rootNodes[i] = 0;
    max_time = 3;
}

IterativeDeepener::~IterativeDeepener(void) {}

void IterativeDeepener::iterativeDeepening(double time_limit)
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
        Search newSearch = Search(position, positionalInformation, pieceValues, i, baseEvaluation, positionalInformation[ENDGAME]);
        newSearch.setPrincipalVariation(this->principalVariation);
        newSearch.setPreviousRootNodes(rootNodes);
        
        eval = newSearch.initiateSearch();
        end_time = clock();
        clock_ticks_taken = end_time - start_time;
        time_in_seconds = clock_ticks_taken/((double)CLOCKS_PER_SEC);
        
        recordPrincipalVariation(newSearch.getPVTable(), i);
        
        this->principalFromSquare = this->principalVariation[0];
        this->principalToSquare = this->principalVariation[1];
        if(displayAnalysis)
        {
            cout << "\tt" << time_in_seconds << " : ";
            cout << "d" << i << " : " << eval/100.0 << " : ";
            this->displayPrincipalVariation(i);
        }
        newSearch.copyRootNodes(this->rootNodes);
        this->sortRootNodes();
        
        if(i == maximumDepth and time_in_seconds < max_time) maximumDepth++;
    }
}

void IterativeDeepener::setMaxTime(double maximumTime)
{
    max_time = maximumTime;
}

void IterativeDeepener::sortRootNodes(void)
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

void IterativeDeepener::setDisplayAnalysis(bool displayAnalysisSwitch)
{
    this->displayAnalysis = displayAnalysisSwitch;
}

void IterativeDeepener::recordPrincipalVariation(int* PVTable, int length)
{
    //Construct principal variation
    for(int i = 0; i < 512; i++)
    {
        this->principalVariation[i] = 0;
    }
    
    for(int i = 0; i < length; i++)
    {
        this->principalVariation[2*i] = PVTable[(length - 1)*34 - 2*i];
        this->principalVariation[2*i+1] = PVTable[(length - 1)*34 - 2*i + 1];
    }
}

void IterativeDeepener::displayPrincipalVariation(int length)
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

void IterativeDeepener::displayPosition(void)
{
    cout << "\n";
    for(int j = 7; j >= 0; j--)
    {
        cout << "|";
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

void IterativeDeepener::reset(int* position, int* positionalInformation, int* pieceValues, int maximumDepth)
{
    this->position = position;
    this->positionalInformation = positionalInformation;
    this->pieceValues = pieceValues;
    this->maximumDepth = maximumDepth;
    this->lastFromSquare = positionalInformation[LAST_FROM_SQUARE];
    this->lastToSquare = positionalInformation[LAST_TO_SQUARE];
    this->baseEvaluation = positionalInformation[EVALUATION];
}

int IterativeDeepener::getPrincipalFromSquare(void)
{
    return this->principalFromSquare;
}

int IterativeDeepener::getPrincipalToSquare(void)
{
    return this->principalToSquare;
}
