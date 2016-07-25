#include "Globals.h"
#include "MoveGenerator.h"
#include "QMoveGenerator.h"
#include "Search.h"
#include "IterativeDeepener.h"
#include "ChessGame.h"
#include <time.h>
#include <iostream>
#include <string>

using namespace std;


bool isAlpha(string characterString, int index)
{
    string alphabet = "abcdefghijklmnopqrstuvwxyz";
    int i = 0;
    bool isAlphabetic = false;

    while(i < alphabet.length() and ! isAlphabetic)
    {
        if(characterString[index] == alphabet[i]) isAlphabetic = true;
        else i++;
    }
    return isAlphabetic;
}

bool isNum(string characterString, int index)
{
    string numbers = "0123456789";
    int i = 0;
    bool isNumeric = false;

    while(i < numbers.length() and ! isNumeric)
    {
        if(characterString[index] == numbers[i]) isNumeric = true;
        else i++;
    }
    return isNumeric;
}

void moveGeneratorTest(void)
{   
    string reaction;
    ChessGame game;
    cout << "Go!";
    clock_t init, final;
    init=clock();

    for(int i = 0; i < 1000000; i++)
    {
        int allMoves[80];
        MoveGenerator generator(game.getBoard(), game.getPositionalInformation(), allMoves);
        generator.getLegalMoves();
    }
    final=clock()-init;
    cout << (double)final/((double)CLOCKS_PER_SEC);
    cout << "\nMoves generated per second: ";
    cout << 20000000/((double)final/((double)CLOCKS_PER_SEC));
    cout << "\nStop!\n";
    cin >> reaction;
}

void searchTest(void)
{
    string reaction;
    ChessGame game;
    //game.setPosition("r1bqkb1r/ppp2ppp/2n2n2/3pp3/2B1P3/2N2N2/PPPP1PPP/R1BQK2R w KQkq - 0 5", 32, 32);

    clock_t init, final;
    cout << "Go!\n";
    int eval;
    init=clock();

    //DEBUG QUIESCENCE SEARCHING

    Search newSearch(game.getBoard(), game.getPositionalInformation(), game.getPieceValues(), 6, 0, false);
    eval = newSearch.initiateSearch();
    final=clock()-init;
    cout << "\nElapsed time: ";
    cout << (double)final/((double)CLOCKS_PER_SEC);
    cout << "\nEvaluation: ";
    cout << eval;
    newSearch.displayPosition();
    cout << "\nNumber of evaluations: ";
    cout << newSearch.getNumberOfEvals();
    cout << "\nNumber of qEvaluations: ";
    cout << newSearch.getNumberOfQEvals();
    cout << "\nNumber of moves generated: ";
    cout << newSearch.getNumberOfGeneratedMoves();
    cout << "\nNumber of qMoves generated: ";
    cout << newSearch.getNumberOfQGeneratedMoves();
    cout << "\nStop!\n";
    newSearch.displayPVTable();

    //cin >> reaction;
}

void perftTest(void)
{
    int eval;
    ChessGame game;
    game.setPosition("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 32, 32);
    //game.makeMove(4, 5);
    //game.makeMove(-1, 118);
    //game.displayPosition();
//  game.makeMove(4, 19);
//  game.makeMove(115, 98);
//  game.makeMove(19, 36);
//  game.makeMove(98, 38);
    Search newSearch(game.getBoard(), game.getPositionalInformation(), game.getPieceValues(), 2, 0, false);
    eval = newSearch.initiateSearch();

    cout << "Total: " << newSearch.getNumberOfEvals() << "\n";

    newSearch.displayPosition();

    //game.makeMove();
}

void iterativeDeepeningTest(void)
{
    ChessGame newGame;

    IterativeDeepener newIterativeDeepener(newGame.getBoard(), newGame.getPositionalInformation(), newGame.getPieceValues());
    newIterativeDeepener.reset(newGame.getBoard(), newGame.getPositionalInformation(), newGame.getPieceValues(), 8);

    newGame.makeMove(17, 49);
    newGame.makeMove(100, 84);
    newGame.makeMove(2, 17);
    newGame.makeMove(118, 85);
    newGame.makeMove(49, 65);
    newGame.makeMove(99, 67);
    newGame.makeMove(6, 37);
    newGame.makeMove(117, 83);
    newGame.makeMove(20, 36);
    newGame.makeMove(-116, 118);
    newGame.makeMove(37, 68);

    newGame.displayAll();

    //newGame.setPosition("8/8/3k4/8/8/K7/1p4PP/8 b - - 0 1", 49, 48);

    newIterativeDeepener.setDisplayAnalysis(true);
    newIterativeDeepener.iterativeDeepening(1000);
}

std::string helpMenu(void) {
    return  "Commands:\n" 
            "\tnew\t\t\tstart new game\n" 
            "\tflip\t\t\tchange sides\n"
            "\tdisplay\t\t\tdisplay board position\n"
            "\treveal\t\t\tdisplay evaluations and principal variations\n"
            "\tset [fen]\t\tset position with fen notation\n"
            "\tanalyse\t\t\tset infinite search depth\n"
            "\tblitz\t\t\tlimit think time\n"
            "\t{a-h}{1-8}{a-h}{1-8}\tplay move\n"               
            "\tauthor\t\t\tdisplay author details\n"
            "\thelp\t\t\tdisplay help menu\n"
            "\texit\t\t\texit\n";
}

void gameRoutine(void)
{
    ChessGame newGame;
    IterativeDeepener newIterativeDeepener(newGame.getBoard(), newGame.getPositionalInformation(), newGame.getPieceValues());
    
    //newGame.setPosition("8/8/3k4/8/K7/8/1p4PP/8 w - - 0 1", 49, 48);

    int toSquare, fromSquare;
    bool displayAnalysis, blitz;
    displayAnalysis = false;
    blitz = false;

    cout << helpMenu();
    
    string command;
    cout << "> ";
    cin >> command;
    
    while(command != "exit")
    {
        if(command == "new")
        {
            newGame.setPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 32, 32);
        }
        else if(command == "flip")
        {
            newIterativeDeepener.reset(newGame.getBoard(), newGame.getPositionalInformation(), newGame.getPieceValues(), 8);
            newIterativeDeepener.setDisplayAnalysis(displayAnalysis);
            newIterativeDeepener.iterativeDeepening(1000);
            newGame.makeMove(newIterativeDeepener.getPrincipalFromSquare(), newIterativeDeepener.getPrincipalToSquare());
        }
        else if(command == "display")
        {
            if(newGame.isWhiteTurn()) newGame.displayPosition();
            else newGame.displayFlippedPosition();
        }
        else if(command == "reveal")
        {
            displayAnalysis = ! displayAnalysis;
        }
        else if(command.substr(0, 4) == "set ")
        {
            newGame.setPosition(command.substr(4, command.length() - 4), 32, 32);
        }
        else if(command == "analyse")
        {
            newIterativeDeepener.reset(newGame.getBoard(), newGame.getPositionalInformation(), newGame.getPieceValues(), 8);
            newIterativeDeepener.setDisplayAnalysis(displayAnalysis);
            newIterativeDeepener.iterativeDeepening(1000);
        }
        else if(command == "blitz")
        {
            blitz = ! blitz;
            if(blitz) newIterativeDeepener.setMaxTime(1);
            else newIterativeDeepener.setMaxTime(3);
        }
        else if(command == "help")
        {
            cout << helpMenu();
        }
        else if(command == "author")
        {
            cout << "Joseph Cai Boyd, 2010\n";
        }
        else
        {
            if(command.length() == 4)
            {
                fromSquare = newGame.translateSquare(command, false);
                toSquare = newGame.translateSquare(command, true);
                if(fromSquare != -1 and toSquare != -1) //Valid move
                {
                    if(newGame.isSpecialMove(fromSquare, toSquare)) fromSquare = -fromSquare;
                    if(newGame.findMove(fromSquare, toSquare))
                    {
                        newGame.makeMove(fromSquare, toSquare);
                        newIterativeDeepener.reset(newGame.getBoard(), newGame.getPositionalInformation(), newGame.getPieceValues(), 8);
                        newIterativeDeepener.setDisplayAnalysis(displayAnalysis);
                        newIterativeDeepener.iterativeDeepening(1000);
                        newGame.makeMove(newIterativeDeepener.getPrincipalFromSquare(), newIterativeDeepener.getPrincipalToSquare());
                    }
                    else cout << "Illegal move\n";
                }
                else cout << "Command not found\n";
            }
            else cout << "Command not found\n";
        }
        cout << "> ";
        cin >> command;
    }
}

int main()
{
    //searchTest();
    //perftTest();
    //iterativeDeepeningTest();
    gameRoutine();
    
    return 0;
}