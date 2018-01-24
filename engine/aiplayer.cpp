#include <cstdlib>
#include <ctime>
#include <list>
#include <vector>
#include <algorithm>
#include "aiplayer.h"
#include "chessboard.h"
#include "perfomancemeasurement.h"


using namespace std;

#ifdef TRACE
static const int QUISCENT_DEPTH = 4;
#else
static const int QUISCENT_DEPTH = 6;
#endif
AIPlayer::AIPlayer(int color, int search_depth)
 : ChessPlayer(color),
   ai_depth(search_depth)
{
	srand(time(NULL));
}

AIPlayer::~AIPlayer()
{}

void AIPlayer::prepare(const ChessBoard &board)
{
}

bool AIPlayer::getMove(const ChessBoard & orig_board, Move & move, AdvancedMoveData *move_data)
{
    ChessBoard & board = const_cast<ChessBoard &>(orig_board);
	vector<Move> candidates;
    list<Move> regulars, simple, captures;
    EvaluationInformation eval;


    int best_value, tmp;

#ifdef TRACE
    vector<list<Move>> best_chain_candidates;
    list<Move> chain, moved;
    eval.moved = &moved;
    eval.best= &chain;
#endif

    eval.depth = ai_depth - 1;
    eval.alpha = - WIN_VALUE;

    if (board.get_all_figures_count() < 10) {
        //target_depth++;
    }
    if (board.get_all_figures_count() <= 5) {
        //target_depth++;
    }

	// first assume we are loosing
    best_value = -KING_VALUE;

	// get all moves
    MoveGenerator<false>::getMoves(board, board.next_move_color, simple, captures);
    regulars.swap(captures);
    //instead of copying
    copy(simple.begin(), simple.end(), back_inserter(regulars));

	// loop over all moves
	for(list<Move>::iterator it = regulars.begin(); it != regulars.end(); ++it)
	{
		// execute move
        board.move(*it);

#ifdef TRACE
        eval.moved->push_back(*it);
#endif

        bool current_king_vulnerable   = board.isVulnerable((board.next_move_color ? board.black_king_pos : board.white_king_pos), board.next_move_color);
        bool previous_king_vulnerable = board.isVulnerable((board.next_move_color ? board.white_king_pos : board.black_king_pos), TOGGLE_COLOR(board.next_move_color));

		// check if own king is vulnerable now
        if(NOT previous_king_vulnerable) {

            if((*it).capture != EMPTY || previous_king_vulnerable || current_king_vulnerable)
                eval.quiescent = true;
            else
                eval.quiescent = false;

#ifdef TRACE
            chain.clear();
            Global::instance().log(string("Try move: ") + it->toString());

#endif
            eval.beta = -best_value;
			// recursion
            tmp = -evalAlphaBeta(board, &eval);
#ifdef TRACE
            stringstream sstr;
            Global::instance().log("=============================================");
            sstr << "Figures count: " << board.get_all_figures_count() << endl;
            sstr << "Depth: "         << eval.depth << endl;

            sstr << "non_pawn_kick_moves_count: " << board.non_pawn_kick_moves_count << endl;
            sstr << "Available move (" << tmp << ")" << it->toString()
                                    << " because of next chain: ";
            for (Move & m: chain) {
                sstr << m.toString() << "; ";
            }
            Global::instance().log(sstr.str());
            Global::instance().log("=============================================");
#endif
            if(tmp > best_value) {
                best_value = tmp;
#ifdef TRACE
                best_chain_candidates.clear();
                best_chain_candidates.push_back(chain);
#endif
				candidates.clear();
				candidates.push_back(*it);
			}
            else if(tmp == best_value) {
				candidates.push_back(*it);
#ifdef TRACE

                best_chain_candidates.push_back(chain);
#endif
			}
		}

		// undo move and inc iterator
		board.undoMove(*it);
#ifdef TRACE
        eval.moved->pop_back();
#endif
    }
    if (move_data)
        move_data->board_evaluation = best_value;

	// loosing the game?
    if(best_value < -WIN_VALUE) {
		return false;
	}
	else {
		// select random move from candidate moves
        int select = rand() % candidates.size();
        move = candidates[select];
#ifdef TRACE
        stringstream tmp;
        Global::instance().log("=============================================");
        tmp << "Figures count: " << board.get_all_figures_count() << endl;
        tmp << "Depth: "         << ai_depth << endl;

        tmp << "non_pawn_kick_moves_count: " << board.non_pawn_kick_moves_count << endl;
        tmp << "Selected move (" << best_value << ")" << move.toString()
                                << " because of next chain: ";
        for (Move & move: best_chain_candidates[select]) {
            tmp << move.toString() << "; ";
        }
        Global::instance().log(tmp.str());
        Global::instance().log("=============================================");
#endif
		return true;
    }
}

void AIPlayer::showMove(const ChessBoard &board, Move &move)
{

}

int AIPlayer::evalAlphaBeta(ChessBoard & board, const EvaluationInformation * info) const
{
#ifdef TRACE
    list<Move> chain;
    Move best_move = EMPTY_MOVE;
#endif
    list<Move> regulars, simple, ignored, captures;
    int best_value, tmp, alpha = info->alpha;

    bool long_depth = false, checkmate;
    if(info->depth <= 0 && !info->quiescent) {

        return +evaluateBoard(board);
    } else if (info->quiescent && info->depth <= -QUISCENT_DEPTH) {
        //limit maximum recursion
        return +evaluateBoard(board);
    } else if (info->quiescent && info->depth <= 0) {
        long_depth = true;
    }

	// first assume we are loosing
    best_value = -WIN_VALUE + board.non_pawn_kick_moves_count; // in case we are winning lets win less moves

    if (long_depth && !info->quiescent) {
        // get only captures
        MoveGenerator<true>::getMoves(board, board.next_move_color, ignored, regulars);
    } else {
        MoveGenerator<false>::getMoves(board, board.next_move_color, simple, captures);
        //instead of copying
        regulars.swap(captures);
        copy(simple.begin(), simple.end(), back_inserter(regulars));
    }

    // assume we have a state_mate
    bool stalemate = true;

    bool our_king_vulnerable_before = board.isVulnerable(board.next_move_color ? board.black_king_pos : board.white_king_pos, board.next_move_color);

    if (our_king_vulnerable_before) {
        checkmate = true;
    } else {
        checkmate = false;
    }

	// loop over all moves
    for(list<Move>::iterator it = regulars.begin();
        info->alpha <= info->beta && it != regulars.end(); ++it)
    {
		// execute move
        board.move(*it);
#ifdef TRACE
        info->moved->push_back(*it);

        chain.clear();
        {
            static int break_counter = 0;
            stringstream trace;
            break_counter++;
            trace << break_counter << ": Try submove:";
            if (break_counter == 554) {
                cout << endl;
            }
            for (Move & move : *info->moved) {
                trace << move.toString() + "->";
            }
            Global::instance().log(trace.str());

        }
#endif
        int current_king_pos = board.next_move_color ? board.black_king_pos : board.white_king_pos;
        int previous_king_pos = board.next_move_color ? board.white_king_pos : board.black_king_pos;

        bool is_invalid_move = false;

        //check if king is near the other king which is invalid
        if (abs(current_king_pos - previous_king_pos) == 1
                || (abs(current_king_pos - previous_king_pos) >= 7 && abs(current_king_pos - previous_king_pos) <= 9)
                ) {
            is_invalid_move = true;
        }

        bool current_king_vulnerable  = board.isVulnerable(current_king_pos, board.next_move_color);
        bool previous_king_vulnerable = board.isVulnerable(previous_king_pos, TOGGLE_COLOR(board.next_move_color));

        //make a move and making a king vulnerable is illegal
        if (previous_king_vulnerable) {
            is_invalid_move = true;
        }

        if (NOT is_invalid_move && NOT previous_king_vulnerable) {
            checkmate = false;
            stalemate = false;
        }

		// check if own king is vulnerable now
        if(NOT is_invalid_move) {
            bool quiescent = false;
            if((*it).capture != EMPTY || current_king_vulnerable || previous_king_vulnerable)
                quiescent = true;
            else
                quiescent = false;

            EvaluationInformation nested_information;
            nested_information.depth     = info->depth - 1;
            nested_information.alpha     = - info->beta;
            nested_information.beta      = - info->alpha;
            nested_information.quiescent =  quiescent;
#ifdef TRACE
            nested_information.moved = info->moved;
            nested_information.best = &chain;
#endif
            
            if (board.non_pawn_kick_moves_count >= 50) {
                tmp = 0;
            } else {
                // recursion 'n' pruning
                tmp = -evalAlphaBeta(board, &nested_information);
            }
#ifdef TRACE
            {
                stringstream trace;
                for (Move & move : *info->moved) {
                    trace << move.toString() + "->";
                }
                trace << tmp;
                Global::instance().log(trace.str());
            }
#endif
            if(tmp > best_value) {
                best_value = tmp;
#ifdef TRACE
                best_move = *it;
                *info->best = *nested_information.best;
#endif
                if(tmp > alpha) {
                    alpha = tmp;
				}
			}
		}

		// undo move and inc iterator
		board.undoMove(*it);
#ifdef TRACE
        info->moved->pop_back();
#endif
    }

#ifdef TRACE
    if (best_move.figure)
        info->best->push_front(best_move);
    
#endif
    if (checkmate) {
        // it is not stalemate :), this is checkmate
        stalemate = false;
    }
    //stalemate is not so bad :)
    return stalemate == true ? 0 : best_value;
}

int AIPlayer::evaluateBoard(const ChessBoard & board) const
{//A7G7->G8F8->F6E6->F8G7->E6D7
    EVALUATION_PROF_POINT;
    int figure, pos, sum = 0, summand, row, col, edge_distance_row, edge_distance_col;
#   ifdef TRACE
    static int br_counter = 0;
    br_counter ++;
    stringstream sstr;
    sstr << "Evalutaion Point: " << br_counter;
    Global::instance().log(sstr.str());
#   endif
    int black_sum = 0, white_sum = 0;
	for(pos = 0; pos < 64; pos++)
	{
		figure = board.square[pos];
		switch(FIGURE(figure))
		{
			case PAWN:
				summand = PAWN_VALUE;
                row = pos / 8;
                if (IS_BLACK(figure)) {
                    summand += 6 - row;
                } else {
                    summand += row - 1;
                }
				break;
			case ROOK:
				summand = ROOK_VALUE;
				break;
			case KNIGHT:
				summand = KNIGHT_VALUE;
				break;
			case BISHOP:
				summand = BISHOP_VALUE;
				break;
			case QUEEN:
				summand = QUEEN_VALUE;
				break;
			default:
				summand = 0;
				break;
		}
        if (IS_BLACK(figure)) {
            black_sum +=  summand;
        } else {
            white_sum +=  summand;
        }
	}
    struct {
        int pos;
        int opp_pos;
        int color;
        int figure;
        int * value;
    } kings[2] = {
        {board.white_king_pos, board.black_king_pos, WHITE, board.square[board.white_king_pos], &white_sum},
        {board.black_king_pos, board.white_king_pos, BLACK, board.square[board.black_king_pos], &black_sum}
    };
    for (int i = 0; i < 2; i++) {

        if (kings[i].figure) {
            if (*kings[i].value < ROOK_VALUE) {
                // in case we have small amount of figures it is better to have a king nearer to the center
                row = kings[i].pos / 8;
                col = kings[i].pos % 8;

                edge_distance_row = min(row, 7 - row);
                edge_distance_col = min(col, 7 - col);

                int additional_minimum = min(edge_distance_row, edge_distance_col);

                *kings[i].value += additional_minimum + edge_distance_row + edge_distance_col - 6;

                // lets got also a fee to be as far from the opponent king
                int opp_row = kings[i].opp_pos / 8;
                int opp_col = kings[i].opp_pos % 8;

                int king_distance = abs(row - opp_row) + abs(col - opp_col);
                *kings[i].value += king_distance;
            }
            *kings[i].value += KING_VALUE;
        }
    }
//    // it is
//    sum -= board.fifty_moves;

    if (IS_BLACK(this->color) )
        sum = black_sum - white_sum;
    else
        sum = white_sum - black_sum;

	
    return (board.next_move_color != this->color ? -sum : sum);
}

