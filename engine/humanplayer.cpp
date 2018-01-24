#include <cstdlib>
#include <cstdio>
#include <list>
#include <cctype>
#include <cstring>
#include <sstream>
#include "humanplayer.h"
#include "chessboard.h"
#include "global.h"
#include "config.h"
#include "perfomancemeasurement.h"

using namespace std;
using namespace boost;
using namespace chrono;

HumanPlayer::HumanPlayer(bool mode_slave, int color)
 : ChessPlayer(color),
   mode_slave(mode_slave)
{}

HumanPlayer::~HumanPlayer()
{}

void HumanPlayer::prepare(const ChessBoard &board)
{
    if (NOT mode_slave) {
        board.print();
    }
}

void HumanPlayer::showMove(const ChessBoard & orig_board, Move & move)
{
    ChessBoard board = orig_board;
    if (NOT mode_slave)
        board.print(move);

    board.move(move);
    ChessPlayer::Status status = board.getPlayerStatus(board.next_move_color);
    board.undoMove(move);

    switch(status)
    {
        case ChessPlayer::Checkmate:
            printf("Checkmate\n");
            break;
        case ChessPlayer::Stalemate:
            printf("Stalemate\n");
            break;
        default:
            break;
    }
    double microseconds = measure_evaluation.time_span.count();
    cout << "Perfomance:"
         << "\n\tEvalutation times(total): " << measure_evaluation.times
         << "\n\tEvalutation time (total): " << microseconds
         << "\n\tEvalutation time per call (total): " << (microseconds / measure_evaluation.times)
         << endl;
}


bool HumanPlayer::getMove(const ChessBoard & board, Move & move, AdvancedMoveData * move_data)
{
    string input;

	for(;;) {
        if (NOT mode_slave) {
            printf(">> ");
        } else {
            stringstream str;
            str << "Waiting for input";
            Global::instance().log(str.str());
        }

        input = readInput();

        if(!processInput(board, input, move)) {
            stringstream str;
            str << "Error while parsing input:" << input;
            Global::instance().log(str.str());
			continue;
		}

		if(!board.isValidMove(color, move)) {

            stringstream str;
            str << "Invalid move " << move.toString();
            Global::instance().log(str.str());
			continue;
		}
        if (mode_slave) {
            stringstream str;
            str << "Got move " << move.toString();
            Global::instance().log(str.str());
        }
        if (NOT mode_slave)
            printf("\n");
		break;
	}

    ChessBoard cloned_board = board;
    cloned_board.move(move);
    cloned_board.print(move);
    move.print();

	return true;
}

string HumanPlayer::readInput() const
{
    string line;
    char buffer[20];
    while (line == "") {
            cin >> line;
    }
    if (line == "") {
        line = buffer;
    }

    return line;
}

bool HumanPlayer::processInput(const ChessBoard & board, const string & buf, Move & move) const
{

    if (buf == "quit")
		exit(0);

    optional<Move> opt = Move::fromString(board, buf);
    if (opt) {
         move = *opt;
         return true;
    }

    return false;
}
