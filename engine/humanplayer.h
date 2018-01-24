#ifndef HUMAN_PLAYER_H_INCLUDED
#define HUMAN_PLAYER_H_INCLUDED

#include <string>
#include <boost/optional.hpp>
#include "chessplayer.h"

class ChessBoard;
struct Move;

class HumanPlayer: public ChessPlayer {

	public:
	
        HumanPlayer(bool mode_slave, int color);
		
		~HumanPlayer();
		
        virtual void prepare(const ChessBoard & board) override;
        virtual bool getMove(const ChessBoard & board, Move & move, AdvancedMoveData * move_data) override;
        virtual void showMove(const ChessBoard & board, Move & move) override;

		
		/*
		* Read input from stdin
		*/
        std::string readInput() const;
		
		/*
		* Process input. Frees buffer allocated by readInput()
		*/
        bool processInput(const ChessBoard & board, const std::string &buf, Move & move) const;
private:
        bool mode_slave = false;
};

#endif
