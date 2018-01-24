#ifndef AI_PLAYER_H_INCLUDED
#define AI_PLAYER_H_INCLUDED

#include "chessplayer.h"
#include <global.h>
#include <list>


// Pieces' values
#define WIN_VALUE  50000	// win the game
#define PAWN_VALUE    30	// 8x
#define ROOK_VALUE    90	// 2x
#define KNIGHT_VALUE  85	// 2x
#define BISHOP_VALUE  84	// 2x
#define QUEEN_VALUE  300	// 1x
#define KING_VALUE 	 ((PAWN_VALUE * 8) + (ROOK_VALUE * 2) \
						+ (KNIGHT_VALUE * 2) + (BISHOP_VALUE * 2) + QUEEN_VALUE + WIN_VALUE)

class ChessBoard;

struct EvaluationInformation {
    int depth = 0;
    int alpha = 0;
    int beta = 0;
    bool quiescent = 0;
    #ifdef TRACE
    std::list<Move> * moved = NULL;
    std::list<Move> * best = NULL;
    #endif
};

class AIPlayer: public ChessPlayer {

	public:
	
        AIPlayer(int color, int search_depth);

		~AIPlayer();

        void prepare(const ChessBoard & board) override;
        bool getMove(const ChessBoard & board, Move & move, AdvancedMoveData * move_data = nullptr) override;
        void showMove(const ChessBoard & board, Move & move) override;

		/*
		* MinMax search for best possible outcome
		*/ 
        int evalAlphaBeta(ChessBoard & board, const EvaluationInformation * info) const;

		/*
		* For now, this checks only material
		*/
		int evaluateBoard(const ChessBoard & board) const;
	
	protected:

		/*
		* how deep to min-max
		*/
        int ai_depth;
};

#endif
