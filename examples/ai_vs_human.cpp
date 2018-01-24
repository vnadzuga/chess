#include "chessboard.h"
#include "aiplayer.h"
#include "humanplayer.h"
#include "config.h"

using namespace std;
int main()
{
    // create board and default initialise
    ChessBoard board;
    board.initDefaultSetup();

    // Create players: ai and Human
    ChessPlayer * player1 = new AIPlayer(WHITE, 3);
    ChessPlayer * player2 = new HumanPlayer(false, BLACK);

    ChessPlayer::Status status = ChessPlayer::Normal;

    // prepare
    player1->prepare(board);
    player2->prepare(board);

    while (status == ChessPlayer::Normal || status == ChessPlayer::InCheck) {
        Move move;
        // get move from one player
        player1->getMove(board, move);
        // execute move
        board.move(move);
        // show this move to other player
        player2->showMove(board, move);
        board.print(move);
        status = board.getPlayerStatus(player2->getColor());
        // exchange players
        std::swap(player1, player2);
    }

    switch(status)
    {
        case ChessPlayer::Checkmate:
            cout << "Checkmate: " << (board.next_move_color == WHITE ? "white" : "black") << " are defeated" << endl;
            return 0;
        case ChessPlayer::Stalemate:
            cout << "Stalemate: on " << (board.next_move_color == WHITE ? "white" : "black") << " turn" << endl;
            return 0;
        case ChessPlayer::Draw:
            cout << "50 moves end game: on " << (board.next_move_color == WHITE ? "white" : "black") << " turn" << endl;
            return 0;
        default:
            break;
    }
    delete player1;
    delete player2;

    return 0;
}
