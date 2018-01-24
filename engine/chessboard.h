#pragma once

#include "chessplayer.h"
#include <string>
#include <list>
#include <boost/optional.hpp>

#include "global.h"

// Pieces defined in lower 4 bits
#define EMPTY	0x00	// Empty square
#define PAWN	0x01	// Bauer
#define ROOK	0x02	// Turm
#define KNIGHT  0x03	// Springer
#define BISHOP  0x04	// Laeufer
#define QUEEN   0x05	// Koenigin
#define KING	0x06	// Koenig

// Extract figure's type
#define FIGURE(x) (0x0F & x)

// Attributes reside in upper 4 bits
#define SET_BLACK(x) (x | 0x10)
#define IS_BLACK(x)  (0x10 & x)
#define IS_WHITE(x)  (!IS_BLACK(x))
#define OPPOSITE(x)  (0x10 ^ x)


#define SET_MOVED(x)  (x | 0x20)
#define SET_UNMOVED(x) (x & ~0x20)
#define IS_MOVED(x)  (0x20 & x)

// For pawn en passant candidates
#define SET_PASSANT(x)   (x | 0x40)
#define CLEAR_PASSANT(x) (x & 0xbf)
#define IS_PASSANT(x)    (0x40 & x)

// For pawn promotion
#define SET_PROMOTED(x)   (x | 0x80)
#define IS_PROMOTED(x)    (0x80 & x)
#define CLEAR_PROMOTED(x) (x & 0x7f)

class ChessBoard;
struct Move
{
	/*
	* Prints sth. like "Black queen from D8 to D7."
	*/
    void print(void) const;

    std::string toString(void) const;
    static boost::optional<Move> fromString(const ChessBoard & board, const std::string & str);
	
	/*
	* True if moves are equal.
	*/
	bool operator==(const Move & b) const;

    char figure;	// figure which is moved
	char from, to;	// board is seen one-dimensional
    char capture;	// piece that resides at destination square

    /**
     * @brief passant_pos_opponent is a position of the opponent's
     * passant pawn position durign the move
     */
    signed short passant_pos_opponent = -1;

    int non_pawn_kick_moves_count_opponent = 0;
};
typedef boost::optional<Move> TMoveOpt;
static const Move EMPTY_MOVE = 
{
    .figure = 0, 
    .from = 0, 
    .to = 0, 
    .capture = 0, 
    .passant_pos_opponent = -1, 
    .non_pawn_kick_moves_count_opponent = 0
};

enum Position {
    A1 = 0, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8
};


class ChessBoard
{
public:
	ChessBoard();

    /*
	* Print ASCII representation of board.
	*/
    void print(Move move=EMPTY_MOVE) const;

    /*
    * Load standartised FEN annotation
    */
    void loadFEN(const std::string &position);

    /*
    * Load standartised FEN annotation
    */
    std::string toFEN() const;


	/*
	* Returns an ASCII char representing the figure.
	*/
	char getASCIIrepr(int figure) const;

	/*
	* Initialize board for a normal game.
	*/
	void initDefaultSetup(void);

    /*
    * Updates internal figures count
    */
    void refreshFigures();


    void toogleColor() {
        next_move_color = TOGGLE_COLOR(next_move_color);
    }


	/*
	* Returns true, if the square given by pos is vulnerable to the opponent.
	* This is used to determine if castling is legal or if kings are in check.
	* En passant is not taken into account.
	*/
	bool isVulnerable(int pos, int color) const;

	/*
	* True if move is a valid move for player of given color. Please note, that
	* a move that puts the player's own king in check, is also treated as
	* invalid.
	*/
    bool isValidMove(int color, const Move &move) const ;

	/*
	* Returns the status of player of given color. This method is not declared
	* const, because it needs to simulate moves on the board to draw a
	* conclusion.
	*/
	ChessPlayer::Status getPlayerStatus(int color);

	/*
	* Move and undo moves
	*/
	void move(const Move & move);
	void undoMove(const Move & move);

	void movePawn(const Move & move);
	void undoMovePawn(const Move & move);

	void moveKing(const Move & move);
	void undoMoveKing(const Move & move);

    int black_figures_count() const {
        return figures_count[1];
    }
    int white_figures_count() const {
        return figures_count[0];
    }
    int get_figures_count(int color) const {
        return figures_count[color >> 4];
    }
    int get_all_figures_count() const {
        return figures_count[0] + figures_count[1];
    }

	// THE BOARD ITSELF
	char square[8*8];

	// to keep track of the kings
	char black_king_pos;
	char white_king_pos;

    int non_pawn_kick_moves_count = 0;
    int figures_count[2] = {0, 0};

    int next_move_color = WHITE;

    int move_number = 1;

    signed short passant_pos = -1;

};
template<bool capture_only>
class MoveGenerator {
public:
    /*
    * Generates all moves for one side.
    */
   static void getMoves(ChessBoard & board, int color, std::list<Move> & moves,
        std::list<Move> & captures);

    /*
    * All possible moves for a pawn piece.
    */
    static void getPawnMoves(ChessBoard & board, int figure, int pos, std::list<Move> & moves,
        std::list<Move> & captures);

    /*
    * All possible moves for a rook piece.
    */
    static void getRookMoves(ChessBoard & board, int figure, int pos, std::list<Move> & moves,
        std::list<Move> & captures);

    /*
    * All possible moves for a knight piece.
    */
    static void getKnightMoves(ChessBoard & board, int figure, int pos, std::list<Move> & moves,
        std::list<Move> & captures);

    /*
    * All possible moves for a bishop piece.
    */
    static void getBishopMoves(ChessBoard & board, int figure, int pos, std::list<Move> & moves,
        std::list<Move> & captures);

    /*
    * All possible moves for a queen piece.
    */
    static void getQueenMoves(ChessBoard & board, int figure, int pos, std::list<Move> & moves,
        std::list<Move> & captures);

    /*
    * All possible moves for a king piece.
    */
    static void getKingMoves(ChessBoard & board, int figure, int pos, std::list<Move> & moves,
        std::list<Move> & captures);
};


