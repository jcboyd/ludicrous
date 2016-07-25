#include <cstdlib>

#ifndef H_RANDOM
#define H_RANDOM

#define ALPHA_BETA
#define NODE_ORDERING
#define KILLERS
#define PV_MOVES
#define EVALUATE_PAWN_STRUCTURE
//#define HISTORY_HEURISTIC
//#define PERFT
//#define DEBUG
//#define STATIC_EXCHANGE_EVALUATION
//define Q_STATIC_EXCHANGE_EVALUATION


const int LATE_MOVE_INDEX = 6;
const int NULL_MOVE_FLAG = -100;

const int INF = 1000000000;
const int KILLER_MOVES = 3;

enum SEE_OUTCOME {BLACK_WINS, EQUALITY, WHITE_WINS};

enum PIECE_TYPES {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING};

enum POSITIONAL_PARAMETERS {
    WHITE_TURN, IN_CHECK, CASTLING, WHITE_KING_INDEX, BLACK_KING_INDEX, EVALUATION, LAST_FROM_SQUARE, LAST_TO_SQUARE, ENDGAME};

const int WHITE_PIECE_ORDER[16] = {16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
const int BLACK_PIECE_ORDER[16] = {40, 41, 42, 43, 44, 45, 46, 47, 32, 33, 34, 35, 36, 37, 38, 39};
//const int BLACK_PIECE_ORDER[16] = {32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47};

//const int WHITE_Q_PIECE_ORDER[16] = {24, 25, 26, 27, 28, 29, 30, 31, 16, 17, 18, 19, 20, 21, 22, 23};
const int WHITE_Q_PIECE_ORDER[16] = {16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
const int BLACK_Q_PIECE_ORDER[16] = {40, 41, 42, 43, 44, 45, 46, 47, 32, 33, 34, 35, 36, 37, 38, 39};

const int SPECIAL_PIECES[8] = {1, 0, 0, 0, 0, 1, 0, 0};

//Names

const char BOARD_SQUARE_NAMES[] = 
"abcdefgh11111111abcdefgh22222222abcdefgh33333333abcdefgh44444444abcdefgh55555555abcdefgh66666666abcdefgh77777777abcdefgh88888888";

const char WHITE_PIECE_NAMES[] = "PNBRQK";
const char BLACK_PIECE_NAMES[] = "pnbrqk";

//Movement arrays

const int PAWN_MOVES[2] = {15, 17};
const int KNIGHT_MOVES[8] = {33, 18, -14, -31, -33, -18, 14, 31};
const int BISHOP_MOVES[8] = {15, -15, 17, -17};
const int ROOK_MOVES[8] = {1, -1, 16, -16};
const int QUEEN_MOVES[8] = {15, 16, 17, 1, -15, -16, -17, -1};
const int KING_MOVES[8] = {15, 16, 17, 1, -15, -16, -17, -1};

//Legality arrays

const int SEARCH_SQUARES[8] = {1, -1, 15, -15, 16, -16, 17, -17};
const int ATTACKERS1[8] = {QUEEN, QUEEN, QUEEN, QUEEN, QUEEN, QUEEN, QUEEN, QUEEN};
const int ATTACKERS2[8] = {ROOK, ROOK, BISHOP, BISHOP, ROOK, ROOK, BISHOP, BISHOP};

const int INITIAL_INDICES[64] = {
    0, 2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,-1, 4,
    6, 8,-1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,-1,
    0,-1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0};

const int CASTLING_PAWN_ATTACKS[2] = {15, 17};
const int CASTLING_SLIDING_ATTACKS[4] = {15, 16, 17, 0};
const int CASTLING_KNIGHT_ATTACKS[4] = {14, 18, 31, 33};

const int CASTLING_ATTACKERS1[4] = {QUEEN, QUEEN, QUEEN, QUEEN};
const int CASTLING_ATTACKERS2[4] = {BISHOP, ROOK, BISHOP, ROOK};

const int P_DIFFS[128] = {
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0, 15,
   16, 17,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0, 
   32,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0};

const int N_DIFFS[128] = {
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0, 14,  0,
    0,  0, 18,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0, 31,
    0, 33,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0};

const int B_DIFFS[128] = {
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0, 15,
    0, 17,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0, 15,  0,
    0,  0, 17,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0, 15,  0,  0,
    0,  0,  0, 17,  0,  0,  0,  0, /**/  0,  0,  0,  0, 15,  0,  0,  0,
    0,  0,  0,  0, 17,  0,  0,  0, /**/  0,  0,  0, 15,  0,  0,  0,  0,
    0,  0,  0,  0,  0, 17,  0,  0, /**/  0,  0, 15,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0, 17,  0, /**/  0, 15,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0, 17, /**/ 15,  0,  0,  0,  0,  0,  0,  0};

const int R_DIFFS[128] = { 
    0,  1,  1,  1,  1,  1,  1,  1, /**/  0,  0,  0,  0,  0,  0,  0,  0,
   16,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0, 
   16,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
   16,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
   16,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
   16,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
   16,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
   16,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0};

const int Q_DIFFS[128] = { 
    0,  1,  1,  1,  1,  1,  1,  1, /**/  0,  0,  0,  0,  0,  0,  0, 15,
   16, 17,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0, 15,  0,
   16,  0, 17,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0, 15,  0,  0,
   16,  0,  0, 17,  0,  0,  0,  0, /**/  0,  0,  0,  0, 15,  0,  0,  0,
   16,  0,  0,  0, 17,  0,  0,  0, /**/  0,  0,  0, 15,  0,  0,  0,  0,
   16,  0,  0,  0,  0, 17,  0,  0, /**/  0,  0, 15,  0,  0,  0,  0,  0, 
   16,  0,  0,  0,  0,  0, 17,  0, /**/  0, 15,  0,  0,  0,  0,  0,  0,
   16,  0,  0,  0,  0,  0,  0, 17, /**/ 15,  0,  0,  0,  0,  0,  0,  0};

//Piece value arrays

const int MATERIAL_VALUES[6] = {100, 300, 325, 500, 900, 1000000};

//Piece square tables

const int DOUBLE_PAWN_PENALTY = 10;

const int WP_SQUARES[128] = {/*WHITE PAWN SQUARE TABLES*/
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0, 10, 10,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0, 15, 20, 20, 15,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0, 25, 25,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0, 30, 30,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
   50, 50, 50, 50, 50, 50, 50, 50, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0};

const int WPE_SQUARES[128] = {/*BLACK QUEEN SQUARE TABLES*/
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0, 10, 10,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0, 15, 20, 20, 15,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0, 25, 25,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0, 30, 30,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
   50, 50, 50, 50, 50, 50, 50, 50, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0};

const int BP_SQUARES[128] = {/*BLACK PAWN SQUARE TABLES*/
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
  -50,-50,-50,-50,-50,-50,-50,-50, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,-30,-30,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,-25,-25,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,-15,-20,-20,-15,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,-10,-10,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0};

const int BPE_SQUARES[128] = {/*BLACK QUEEN SQUARE TABLES*/
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
  -50,-50,-50,-50,-50,-50,-50,-50, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,-30,-30,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,-25,-25,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,-15,-20,-20,-15,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,-10,-10,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0};

const int WN_SQUARES[128] = {/*WHITE KNIGHT SQUARE TABLES*/
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  5,  5,  5,  5,  5,  5,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  5, 10, 10, 10, 10,  5,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  5, 10, 15, 15, 10,  5,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 15, 20, 20, 15, 10,  5, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 15, 15, 15, 15, 10,  5, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 10, 10, 10, 10, 10,  5, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    5,  5,  5,  5,  5,  5,  5,  5, /**/  0,  0,  0,  0,  0,  0,  0,  0};

const int BN_SQUARES[128] = {/*BLACK KNIGHT SQUARE TABLES*/
   -5, -5, -5, -5, -5, -5, -5, -5, /**/  0,  0,  0,  0,  0,  0,  0,  0,
   -5,-10,-10,-10,-10,-10,-10, -5, /**/  0,  0,  0,  0,  0,  0,  0,  0,
   -5,-10,-15,-15,-15,-15,-10, -5, /**/  0,  0,  0,  0,  0,  0,  0,  0,
   -5,-10,-15,-20,-20,-15,-10, -5, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0, -5,-10,-15,-15,-10, -5,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0, -5,-10,-10,-10,-10, -5,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0, -5, -5, -5, -5, -5, -5,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0};

const int WB_SQUARES[128] = {/*WHITE BISHOP SQUARE TABLES*/
    5,  2,  0,  0,  0,  0,  2,  5, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    2, 10,  7,  5,  5,  7, 10,  2, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  7, 15, 12, 12, 15,  7,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  5, 12, 20, 20, 12,  5,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 17, 25, 25, 17, 10,  5, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    5, 12, 20, 17, 17, 20, 12,  5, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    7, 15, 12, 10, 10, 12, 15,  7, /**/  0,  0,  0,  0,  0,  0,  0,  0,
   10,  7,  5,  5,  5,  5,  7, 10, /**/  0,  0,  0,  0,  0,  0,  0,  0};

const int BB_SQUARES[128] = {/*BLACK BISHOP SQUARE TABLES*/
  -10, -7, -5, -5, -5, -5, -7,-10, /**/  0,  0,  0,  0,  0,  0,  0,  0,
   -7,-15,-12,-10,-10,-12,-15, -7, /**/  0,  0,  0,  0,  0,  0,  0,  0,
   -5,-12,-20,-17,-17,-20,-12, -5, /**/  0,  0,  0,  0,  0,  0,  0,  0,
   -5,-10,-17,-25,-25,-17,-10, -5, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0, -5,-12,-20,-20,-12, -5,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0, -7,-15,-12,-12,-15, -7,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
   -2,-10, -7, -5, -5, -7,-10, -2, /**/  0,  0,  0,  0,  0,  0,  0,  0,
   -5, -2,  0,  0,  0,  0, -2, -5, /**/  0,  0,  0,  0,  0,  0,  0,  0};

const int WR_SQUARES[128] = {/*WHITE ROOK SQUARE TABLES*/
   15, 15, 20, 20, 20, 20, 15, 15, /**/  0,  0,  0,  0,  0,  0,  0,  0,
   10, 10, 15, 15, 15, 15, 10, 10, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  5,  5,  5,  5,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  5,  5,  5,  5,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  5,  5,  5,  5,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  5,  5,  5,  5,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
   20, 20, 25, 25, 25, 25, 20, 20, /**/  0,  0,  0,  0,  0,  0,  0,  0,
   30, 30, 35, 35, 35, 35, 30, 30, /**/  0,  0,  0,  0,  0,  0,  0,  0};

const int BR_SQUARES[128] = {/*BLACK ROOK SQUARE TABLES*/
  -30,-30,-35,-35,-35,-35,-30,-30, /**/  0,  0,  0,  0,  0,  0,  0,  0,
  -20,-20,-25,-25,-25,-25,-20,-20, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0, -5, -5, -5, -5,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0, -5, -5, -5, -5,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0, -5, -5, -5, -5,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0, -5, -5, -5, -5,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
  -10,-10,-15,-15,-15,-15,-10,-10, /**/  0,  0,  0,  0,  0,  0,  0,  0,
  -15,-15,-20,-20,-20,-20,-15,-15, /**/  0,  0,  0,  0,  0,  0,  0,  0};

const int WQ_SQUARES[128] = {/*WHITE QUEEN SQUARE TABLES*/
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0};

const int BQ_SQUARES[128] = {/*BLACK QUEEN SQUARE TABLES*/
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0};

const int WK_SQUARES[128] = {/*WHITE KING SQUARE TABLES*/
   30, 35,  5,  0,  0,  5, 35, 30, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0, -5, -5, -5, -5,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
  -80,-80,-80,-80,-80,-80,-80,-80, /**/  0,  0,  0,  0,  0,  0,  0,  0,
  -85,-85,-85,-85,-85,-85,-85,-85, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0};

const int WKE_SQUARES[128] = {/*WHITE KING ENDGAME SQUARE TABLES*/
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0, 10, 10, 10, 10, 10, 10,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0, 10, 20, 20, 20, 20, 10,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0, 10, 20, 30, 30, 20, 10,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0, 15, 25, 35, 35, 25, 15,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0, 15, 25, 25, 25, 25, 15,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0, 15, 15, 15, 15, 15, 15,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0};

const int BK_SQUARES[128] = {/*BLACK KING SQUARE TABLES*/
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
   80, 80, 80, 80, 80, 80, 80, 80, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  5,  5,  5,  5,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
  -30,-35, -5,  0,  0, -5,-35,-30, /**/  0,  0,  0,  0,  0,  0,  0,  0};

const int BKE_SQUARES[128] = {/*BLACK KING ENDGAME SQUARE TABLES*/
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,-15,-15,-15,-15,-15,-15,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,-15,-25,-25,-25,-25,-15,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,-15,-25,-35,-35,-25,-15,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,-10,-20,-30,-30,-20,-10,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,-10,-20,-20,-20,-20,-10,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,-10,-10,-10,-10,-10,-10,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0, /**/  0,  0,  0,  0,  0,  0,  0,  0};

#endif