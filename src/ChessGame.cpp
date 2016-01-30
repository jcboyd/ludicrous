
#include "ChessGame.h"

chessGame::chessGame()
{
    chessGame::setPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 32, 32);
}

chessGame::~chessGame(void) 
{
}

void chessGame::setPosition(string fen, int lastFromSquare, int lastToSquare)
{
    int squareCount = 0, characterIndex = 0, whitePieceIndex = 0, blackPieceIndex = 0, squareIndex, emptySquares, castlingRights;
    string character;
    
    //Initialise board etc.
    for(int i = 0; i < 128; i++)
    {
        chessBoard[i] = 0;
        positionalInformation[i] = 0;
        pieceValues[i] = 0;
    }
    
    //Initialise positional information
    for(int i = 0; i < 16; i++)
    {
        positionalInformation[16 + i] = -1;
        positionalInformation[32 + i] = -1;
    }
    
    while(squareCount < 64)
    {
        character = fen[characterIndex];
        characterIndex++;
        //Assume 64 squares
        squareIndex = 112 + squareCount%8 - 16*(squareCount/8);
        if(character == "p")
        {
            positionalInformation[32 + blackPieceIndex] = squareIndex;
            positionalInformation[32 + blackPieceIndex + 64] = PAWN;
            chessBoard[squareIndex] = -(32 + blackPieceIndex); //Negative indicates black
            pieceValues[32 + blackPieceIndex] = -MATERIAL_VALUES[PAWN] + BP_SQUARES[squareIndex];
            blackPieceIndex++;
            squareCount++;
        }
        else if(character == "n")
        {
            positionalInformation[32 + blackPieceIndex] = squareIndex;
            positionalInformation[32 + blackPieceIndex + 64] = KNIGHT;
            chessBoard[squareIndex] = -(32 + blackPieceIndex); //Negative indicates black
            pieceValues[32 + blackPieceIndex] = -MATERIAL_VALUES[KNIGHT] + BN_SQUARES[squareIndex];
            blackPieceIndex++;
            squareCount++;
        }
        else if(character == "b")
        {
            positionalInformation[32 + blackPieceIndex] = squareIndex;
            positionalInformation[32 + blackPieceIndex + 64] = BISHOP;
            chessBoard[squareIndex] = -(32 + blackPieceIndex); //Negative indicates black
            pieceValues[32 + blackPieceIndex] = -MATERIAL_VALUES[BISHOP] + BB_SQUARES[squareIndex];
            blackPieceIndex++;
            squareCount++;
        }
        else if(character == "r")
        {
            positionalInformation[32 + blackPieceIndex] = squareIndex;
            positionalInformation[32 + blackPieceIndex + 64] = ROOK;
            chessBoard[squareIndex] = -(32 + blackPieceIndex); //Negative indicates black
            pieceValues[32 + blackPieceIndex] = -MATERIAL_VALUES[ROOK] + BR_SQUARES[squareIndex];
            blackPieceIndex++;
            squareCount++;
        }
        else if(character == "q")
        {
            positionalInformation[32 + blackPieceIndex] = squareIndex;
            positionalInformation[32 + blackPieceIndex + 64] = QUEEN;
            chessBoard[squareIndex] = -(32 + blackPieceIndex); //Negative indicates black
            pieceValues[32 + blackPieceIndex] = -MATERIAL_VALUES[QUEEN] + BQ_SQUARES[squareIndex];
            blackPieceIndex++;
            squareCount++;
        }
        else if(character == "k")
        {
            positionalInformation[BLACK_KING_INDEX] = 32 + blackPieceIndex;
            positionalInformation[32 + blackPieceIndex] = squareIndex;
            positionalInformation[32 + blackPieceIndex + 64] = KING;
            chessBoard[squareIndex] = -(32 + blackPieceIndex); //Negative indicates black
            pieceValues[32 + blackPieceIndex] = -MATERIAL_VALUES[KING];// + BK_SQUARES[squareIndex];
            blackPieceIndex++;
            squareCount++;
        }
        else if(character == "P")
        {
            positionalInformation[16 + whitePieceIndex] = squareIndex;
            positionalInformation[16 + whitePieceIndex + 64] = PAWN;
            chessBoard[squareIndex] = 16 + whitePieceIndex; //Positive indicates white
            pieceValues[16 + whitePieceIndex] = MATERIAL_VALUES[PAWN] + WP_SQUARES[squareIndex];
            whitePieceIndex++;
            squareCount++;
        }
        else if(character == "N")
        {
            positionalInformation[16 + whitePieceIndex] = squareIndex;
            positionalInformation[16 + whitePieceIndex + 64] = KNIGHT;
            chessBoard[squareIndex] = 16 + whitePieceIndex; //Positive indicates white
            pieceValues[16 + whitePieceIndex] = MATERIAL_VALUES[KNIGHT] + WN_SQUARES[squareIndex];
            whitePieceIndex++;
            squareCount++;
        }
        else if(character == "B")
        {
            positionalInformation[16 + whitePieceIndex] = squareIndex;
            positionalInformation[16 + whitePieceIndex + 64] = BISHOP;
            chessBoard[squareIndex] = 16 + whitePieceIndex; //Positive indicates white
            pieceValues[16 + whitePieceIndex] = MATERIAL_VALUES[BISHOP] + WB_SQUARES[squareIndex];
            whitePieceIndex++;
            squareCount++;
        }
        else if(character == "R")
        {
            positionalInformation[16 + whitePieceIndex] = squareIndex;
            positionalInformation[16 + whitePieceIndex + 64] = ROOK;
            chessBoard[squareIndex] = 16 + whitePieceIndex; //Positive indicates white
            pieceValues[16 + whitePieceIndex] = MATERIAL_VALUES[ROOK] + WR_SQUARES[squareIndex];
            whitePieceIndex++;
            squareCount++;
        }
        else if(character == "Q")
        {
            positionalInformation[16 + whitePieceIndex] = squareIndex;
            positionalInformation[16 + whitePieceIndex + 64] =QUEEN;
            chessBoard[squareIndex] = 16 + whitePieceIndex; //Positive indicates white
            pieceValues[16 + whitePieceIndex] = MATERIAL_VALUES[QUEEN] + WQ_SQUARES[squareIndex];
            whitePieceIndex++;
            squareCount++;
        }
        else if(character == "K")
        {
            positionalInformation[WHITE_KING_INDEX] = 16 + whitePieceIndex;
            positionalInformation[16 + whitePieceIndex] = squareIndex;
            positionalInformation[16 + whitePieceIndex + 64] = KING;
            chessBoard[squareIndex] = 16 + whitePieceIndex; //Positive indicates white
            pieceValues[16 + whitePieceIndex] = MATERIAL_VALUES[KING];// + WK_SQUARES[squareIndex];
            whitePieceIndex++;
            squareCount++;
        }
        else if(character == "/") {}
        else //Numeric
        {
            if(character == "1") emptySquares = 1;
            else if(character == "2") emptySquares = 2;
            else if(character == "3") emptySquares = 3;
            else if(character == "4") emptySquares = 4;
            else if(character == "5") emptySquares = 5;
            else if(character == "6") emptySquares = 6;
            else if(character == "7") emptySquares = 7;
            else emptySquares = 8;
            
            squareCount += emptySquares;
            
            for(int i = 0; i < emptySquares; i++) chessGame::chessBoard[squareIndex + i] = 0;
        }
    }
    
    characterIndex++;
    
    //Set turn
    character = fen[characterIndex];
    if(character == "w") positionalInformation[WHITE_TURN] = 1;
    else positionalInformation[WHITE_TURN] = 0;
    
    castlingRights = 0;
    characterIndex += 2;
    character = fen[characterIndex];    
    
    while(character != " ")
    {
        if(character == "K") castlingRights += 8;
        else if(character == "Q") castlingRights += 4;
        else if(character == "k") castlingRights += 2;
        else if(character == "q") castlingRights += 1;
        characterIndex++;
        character = fen[characterIndex];
    }
    
    for(int i = 0; i < 16; i++)
    {
        if(positionalInformation[i + 16] >= 0) positionalInformation[EVALUATION] += pieceValues[i + 16];
        if(positionalInformation[i + 32] >= 0) positionalInformation[EVALUATION] += pieceValues[i + 32];
    }
    
    //Set casting parameter:
    chessGame::positionalInformation[CASTLING] = castlingRights;
    
    characterIndex++;
    
    chessGame::positionalInformation[LAST_FROM_SQUARE] = lastFromSquare;
    chessGame::positionalInformation[LAST_TO_SQUARE] = lastToSquare;
    //Ignore the rest of the string for now
    
    if(queensOff()) positionalInformation[ENDGAME] = 1;
}

int chessGame::translateSquare(string userMove, bool isToSquare)
{
    string files = "abcdefgh", ranks = "12345678";
    
    int fileNumber, rankNumber;
    bool fileFound, rankFound;
    
    fileNumber = 0;
    fileFound = false;
    
    while(fileNumber < files.length() and ! fileFound)
    {
        if(files.substr(fileNumber, 1) == userMove.substr(0 + 2*isToSquare, 1)) fileFound = true;
        else fileNumber++;
    }
    if(fileNumber == files.length()) return -1; //File not found
    
    rankNumber = 0;
    rankFound = false;
    
    while(rankNumber < ranks.length() and ! rankFound)
    {
        if(ranks.substr(rankNumber, 1) == userMove.substr(1 + 2*isToSquare, 1)) rankFound = true;
        else rankNumber++;
    }
    
    if(rankNumber == ranks.length()) return -1; //Rank not found
    
    return 16*rankNumber + fileNumber;
}

bool chessGame::findMove(int fromSquare, int toSquare)
{       
    bool foundMove = false;
    int arraySize, i, allMoves[256];
    
    moveGenerator newMoveGenerator = moveGenerator(chessBoard, positionalInformation, allMoves);
    newMoveGenerator.getLegalMoves();
    arraySize = newMoveGenerator.getArraySize();
    
    i = 0;
    while(i < arraySize and ! foundMove)
    {
        if(fromSquare == allMoves[i] && toSquare == allMoves[i+1]) foundMove = true;
        else i += 2;
    }
    
    return foundMove;
}

bool chessGame::isSpecialMove(int fromSquare, int toSquare)
{
    int pieceType;
    pieceType = positionalInformation[abs(chessBoard[fromSquare]) + 64];
    if(pieceType == PAWN)
    {
        if(positionalInformation[WHITE_TURN]) {
            if(toSquare > 111) return true;
        }
        else if(toSquare < 8) return true;

        if((toSquare - fromSquare)%16 != 0 and chessBoard[toSquare] == 0) return true;
    }
    else if(pieceType == KING)
    {
        if(abs(toSquare - fromSquare) == 2) return true;
    }
    return false;
}

bool chessGame::queensOff(void)
{
    bool blackQueenOff = false, whiteQueenOff = false;
    
    for(int i = 0; i < 16; i++)
    {
        if(positionalInformation[i + 80] == QUEEN) {
            if(positionalInformation[i + 16] == -1) whiteQueenOff = true;
        }
        else if(positionalInformation[i + 96] == QUEEN) {
            if(positionalInformation[i + 32] == -1) blackQueenOff = true;   
        } 
    }
    if(blackQueenOff and whiteQueenOff) return true;
    return false;
}

void chessGame::makeMove(int fromSquare, int toSquare)
{
    int pieceIndex = 0, pieceType = 0, capture = 0, positionalEvaluation = 0;
    const int * pieceSquareTables[6];
    const int * currentPieceSquareTable;
    
    if(fromSquare < 0) //Special move
    {
        pieceIndex = abs(chessBoard[-fromSquare]);
        pieceType = positionalInformation[pieceIndex + 64];
        if(positionalInformation[WHITE_TURN])
        {
            if(pieceType == PAWN)
            {
                if(toSquare > 111) //Promotion
                {
                    positionalEvaluation = MATERIAL_VALUES[QUEEN] - MATERIAL_VALUES[PAWN] + WQ_SQUARES[toSquare] - WP_SQUARES[-fromSquare];
                    positionalInformation[pieceIndex + 64] = QUEEN;
                    capture = toSquare;
                }
                else //En passant capture
                {
                    positionalEvaluation = WP_SQUARES[toSquare] - WP_SQUARES[-fromSquare];
                    capture = toSquare - 16;
                }
            }
            else if(pieceType == KING) //Castle
            {
                positionalEvaluation = WK_SQUARES[toSquare] - WK_SQUARES[-fromSquare];
                if(toSquare == 6)
                {
                    chessBoard[5] = chessBoard[7];
                    chessBoard[7] = 0;
                    positionalInformation[chessBoard[5]] = 5;
                    pieceValues[chessBoard[5]] += WR_SQUARES[5] - WR_SQUARES[7];
                    positionalInformation[EVALUATION] += WR_SQUARES[5] - WR_SQUARES[7];
                    capture = 7;
                }
                else
                {
                    chessBoard[3] = chessBoard[0];
                    chessBoard[0] = 0;
                    positionalInformation[chessBoard[3]] = 3;
                    pieceValues[chessBoard[3]] += WR_SQUARES[3] - WR_SQUARES[0];
                    positionalInformation[EVALUATION] += WR_SQUARES[3] - WR_SQUARES[0];
                    capture = 0;
                }
                if((positionalInformation[CASTLING]&8) == 8) positionalInformation[CASTLING] -= 8;
                if((positionalInformation[CASTLING]&4) == 4) positionalInformation[CASTLING] -= 4;
            }
        }
        else
        {
            if(pieceType == PAWN)
            {
                if(toSquare < 8) //Promotion
                {
                    positionalEvaluation = MATERIAL_VALUES[PAWN] - MATERIAL_VALUES[QUEEN] + WP_SQUARES[-fromSquare] - WQ_SQUARES[toSquare];
                    positionalInformation[pieceIndex + 64] = QUEEN;
                    capture = toSquare;
                }
                else //En passant capture
                {
                    positionalEvaluation = BP_SQUARES[toSquare] - BP_SQUARES[-fromSquare];
                    capture = toSquare + 16;
                }
            }
            else if(pieceType == KING) //Castle
            {
                positionalEvaluation = BK_SQUARES[toSquare] - BK_SQUARES[-fromSquare];
                if(toSquare == 118)
                {
                    chessBoard[117] = chessBoard[119];
                    chessBoard[119] = 0;
                    positionalInformation[-chessBoard[117]] = 117;
                    pieceValues[-chessBoard[117]] += WR_SQUARES[117] - WR_SQUARES[119];
                    positionalInformation[EVALUATION] += WR_SQUARES[117] - WR_SQUARES[119];
                    capture = 119;
                }
                else
                {
                    chessBoard[115] = chessBoard[112];
                    chessBoard[112] = 0;
                    positionalInformation[-chessBoard[115]] = 115;
                    pieceValues[-chessBoard[115]] += WR_SQUARES[115] - WR_SQUARES[112];
                    positionalInformation[EVALUATION] += WR_SQUARES[115] - WR_SQUARES[112];
                    capture = 112;
                }
                if((positionalInformation[CASTLING]&2) == 2) positionalInformation[CASTLING] -= 2;
                if((positionalInformation[CASTLING]&2) == 2) positionalInformation[CASTLING] -= 1;
            }
        }       
    }
    else
    {
        if(positionalInformation[WHITE_TURN])
        {
            pieceSquareTables[KNIGHT] = WN_SQUARES;
            pieceSquareTables[BISHOP] = WB_SQUARES;
            pieceSquareTables[ROOK] = WR_SQUARES;
            pieceSquareTables[QUEEN] = WQ_SQUARES;
            if(positionalInformation[ENDGAME]) 
            {
                pieceSquareTables[PAWN] = WPE_SQUARES;
                pieceSquareTables[KING] = WKE_SQUARES;
            }
            else
            {
                pieceSquareTables[PAWN] = WP_SQUARES;
                pieceSquareTables[KING] = WK_SQUARES;
            }
            pieceIndex = chessBoard[fromSquare];
            if(toSquare == 112 || toSquare == 119)
            {
                if((positionalInformation[CASTLING]&2) == 2) positionalInformation[CASTLING] -= 2;
                if((positionalInformation[CASTLING]&1) == 1) positionalInformation[CASTLING] -= 1;
            }
        }
        else
        {
            pieceSquareTables[KNIGHT] = BN_SQUARES;
            pieceSquareTables[BISHOP] = BB_SQUARES;
            pieceSquareTables[ROOK] = BR_SQUARES;
            pieceSquareTables[QUEEN] = BQ_SQUARES;
            if(positionalInformation[ENDGAME])
            {
                pieceSquareTables[PAWN] = BPE_SQUARES;
                pieceSquareTables[KING] = BKE_SQUARES;
            }
            else 
            {
                pieceSquareTables[PAWN] = BP_SQUARES;
                pieceSquareTables[KING] = BK_SQUARES;
            }
            pieceIndex = -chessBoard[fromSquare];
            if(toSquare == 0 || toSquare == 7)
            {
                if((positionalInformation[CASTLING]&8) == 8) positionalInformation[CASTLING] -= 8;
                if((positionalInformation[CASTLING]&4) == 4) positionalInformation[CASTLING] -= 4;
            }           
        }
        pieceType = positionalInformation[pieceIndex + 64];
        currentPieceSquareTable = pieceSquareTables[pieceType];
        positionalEvaluation = currentPieceSquareTable[toSquare] - currentPieceSquareTable[fromSquare];
        capture = toSquare;
    }
    
    if(pieceType == KING)
    {
        if(positionalInformation[WHITE_TURN])
        {
            if((positionalInformation[CASTLING]&8) == 8) positionalInformation[CASTLING] -= 8;
            if((positionalInformation[CASTLING]&4) == 4) positionalInformation[CASTLING] -= 4;
        }
        else
        {
            if((positionalInformation[CASTLING]&2) == 2) positionalInformation[CASTLING] -= 2;
            if((positionalInformation[CASTLING]&1) == 1) positionalInformation[CASTLING] -= 1;
        }
    }
    
    if(chessBoard[capture] != 0) //Capture
    {
        //Deduct piece value
        positionalInformation[EVALUATION] -= pieceValues[abs(chessBoard[capture])];
        //Flag captured piece
        positionalInformation[abs(chessBoard[capture])] = -1;
        //Remove captured piece
        chessBoard[capture] = 0;
    }
    pieceValues[pieceIndex] += positionalEvaluation;
    positionalInformation[EVALUATION] += positionalEvaluation;
    chessBoard[toSquare] = chessBoard[abs(fromSquare)];
    chessBoard[abs(fromSquare)] = 0;
    positionalInformation[pieceIndex] = toSquare;
    positionalInformation[LAST_FROM_SQUARE] = fromSquare;
    positionalInformation[LAST_TO_SQUARE] = toSquare;
    positionalInformation[WHITE_TURN] = ! positionalInformation[WHITE_TURN];
    
    if(queensOff()) positionalInformation[ENDGAME] = 1;
}

int* chessGame::getBoard(void)
{
    return chessGame::chessBoard;
}

int* chessGame::getPositionalInformation(void)
{
    return positionalInformation;
}

int* chessGame::getPieceValues(void)
{
    return chessGame::pieceValues;
}

void chessGame::displayPosition(void)
{
    cout << "+ - + - + - + - + - + - + - + - +\n";
    for(int j = 7; j >= 0; j--)
    {
        cout << "| ";
        for(int k = 0; k < 8; k++)
        {
            int squareValue = chessGame::chessBoard[j*16 + k];
            if(squareValue != 0)
            {
                int pieceType = chessGame::positionalInformation[abs(chessGame::chessBoard[j*16 + k]) + 64];
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

void chessGame::displayFlippedPosition(void)
{
    cout << "+ - + - + - + - + - + - + - + - +\n";
    for(int j = 0; j <= 7; j++)
    {
        cout << "| ";
        for(int k = 7; k >= 0; k--)
        {
            int squareValue = chessGame::chessBoard[j*16 + k];
            if(squareValue != 0)
            {
                int pieceType = chessGame::positionalInformation[abs(chessGame::chessBoard[j*16 + k]) + 64];
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

void chessGame::displayPieceValues(void)
{
    cout << "\n";
    for(int i = 7; i >= 0; i--)
    {
        cout << "|";
        for(int j = 0; j < 16; j++)
        {
            cout << chessGame::pieceValues[16*i + j];
            cout << "\t|";
        }
        cout << "\n";
    }
}

void chessGame::displayPositionalInformation(void)
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

bool chessGame::isWhiteTurn(void)
{
    if(positionalInformation[WHITE_TURN] == 1) return true;
    return false;
}

void chessGame::displayAll(void)
{
    displayPosition();
    displayPositionalInformation();
    displayPieceValues();
}