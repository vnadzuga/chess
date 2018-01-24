#include <functional>
#include <string>
#include <utility>
#include <iostream>
#include <limits.h>
#include <boost/format.hpp>
#include <string>
#include <exception>
#include <boost/optional.hpp>

#include "gtest/gtest.h"

#include "tests.h"
#include "aiplayer.h"
#include "config.h"

using namespace std;
using namespace boost;
Tests::Tests()
{
    Global::instance().setLoggingFileName("black.log");
}

bool Tests::RunAll()
{
    typedef pair<function<bool()>, string > Handler ;
    Handler tests[] = {
        {bind(&Tests::TestEvaluation, *this), "Test evalutation"},
        {bind(&Tests::TestEvaluation, *this), "Test checkmate in 3 halfmoves"}
    };
    for (auto test: tests) {
        if (! test.first()) {
            cout << test.second << " FAILED!!!" << endl;
            return false;
        }
    }

    return true;
}

bool Tests::TestEvaluation()
{
    // same positions but opposite colors
    const char * positions[] = {
        "8/8/3K4/8/3k4/5r2/8/8 b - - 2 3",
        "8/8/3k4/8/3K4/5R2/8/8 w - - 2 3",
    };

    for (const char * position: positions) {
        board.loadFEN(position);
        AIPlayer player(board.next_move_color, 3);
        Move move;
        AdvancedMoveData advanced;
        bool found = player.getMove(board, move, &advanced);
        if (NOT found) return false;
        if (NOT (move.to == F6 && move.from == F3)) {
            return false;
        }
    }
    return true;

}
bool Tests::TestCheckMateIn3HalfMoves()
{
    // same positions but opposite colors
    const char * positions[] = {
        "6k1/8/6K1/8/1B6/8/8/3B4 w - - 2 3",
        "6K1/8/6k1/8/1b6/8/8/3b4 b - - 2 3",
    };
    Config config = Config::from_start_color(WHITE);

    for (const char * position: positions) {
        board.loadFEN(position);

        AIPlayer player(board.next_move_color, 4);
        Move move = EMPTY_MOVE;
        bool found = player.getMove(board, move);
        if (NOT found) return false;
        if (NOT (move.to == B3 && move.from == D1)) {
            board.print(move);
            return false;
        }
    }
    return true;

}
void Tests::TestAdvanced()
{
    board.loadFEN("5k2/5p2/8/8/8/8/1PPPPPP1/4K3 w - - 2 3");
    AdvancedMoveData advanced;
    
    AIPlayer ai(WHITE, 3);
    Move move;
    ai.getMove(board, move, &advanced);
    
    EXPECT_GT(advanced.board_evaluation, 0);
    
}

void Tests::FiguresCount()
{
    board.initDefaultSetup();
    EXPECT_EQ(board.black_figures_count(), 16);
    EXPECT_EQ(board.white_figures_count(), 16);
}

void Tests::WrongAppearingFigures()
{
    /// bug that was fixed,
    /// bug was about a new figure that occured on the board.
    ChessBoard board;
    board.loadFEN("k7/4p1p1/8/4P3/8/8/5P2/K7 b - - 0 1");

    EXPECT_FALSE(IS_MOVED(board.square[G7]));
    EXPECT_FALSE(IS_MOVED(board.square[E7]));


    auto apply_single_move = [&](string smove) -> Move {
        optional<Move> omove;
        omove = Move::fromString(board, smove);
        std::list<Move> moves;
        MoveGenerator<false>::getMoves(board, board.next_move_color, moves, moves);
        for (Move & move : moves) {
            if (move.to == omove->to) {
                board.move(move);
                return move;
            }
        }
        return EMPTY_MOVE;
    };

    auto apply_single_capture = [&](string smove) -> Move {
        optional<Move> omove;
        omove = Move::fromString(board, smove);
        std::list<Move> moves, captures;
        MoveGenerator<false>::getMoves(board, board.next_move_color, moves, captures);
        for (Move & move : captures) {
            if (move.to == omove->to) {
                board.move(move);
                return move;
            }
        }
        return EMPTY_MOVE;
    };

    Move passant_move = apply_single_move("g7g5");
    board.undoMove(passant_move);
    EXPECT_FALSE(IS_PASSANT(board.square[G7]));

    list<string> moves_pawn_ahead = {"g7g5", "a1a2", "g5g4"};

    for (string & str_move : moves_pawn_ahead) {
        apply_single_move(str_move);
    }

    EXPECT_FALSE(IS_PASSANT(board.square[G4]));

    // test the same but with capture

    board.loadFEN("k7/8/8/4p3/3P1P2/8/8/K7 b - f3 0 1");
    EXPECT_TRUE(board.square[F4] && IS_PASSANT(board.square[F4]));
    Move capt = apply_single_capture("e5d4");
    EXPECT_FALSE(board.square[F4] && IS_PASSANT(board.square[F4]));

    board.undoMove(capt);
    EXPECT_TRUE(board.square[F4] && IS_PASSANT(board.square[F4]));

}
void Tests::TestFenNegative()
{
    ChessBoard board;
    string fen = "rnbqkbnr/pppppppp/8/8/P7/8/1PPPPPPP/RNBQKBNR w KQkq 9 -";

    EXPECT_THROW(board.loadFEN(fen), std::runtime_error);

}
void Tests::CheckPassant()
{
    string fen;

    fen = "5k2/8/8/8/5p2/8/4P3/4K3 w - -";
    board.loadFEN(fen);

    Move move = EMPTY_MOVE;
    move.from = E2;
    move.to = E4;
    move.figure = board.square[move.from];

    board.move(move);

    EXPECT_EQ(board.passant_pos, E4);
    EXPECT_TRUE(IS_PASSANT(board.square[E4]));

    board.undoMove(move);

    EXPECT_EQ(board.passant_pos, -1);
    EXPECT_FALSE(IS_PASSANT(board.square[E2]));

    board.move(move);

    EXPECT_EQ(board.passant_pos, E4);
    EXPECT_TRUE(IS_PASSANT(board.square[E4]));

    board.initDefaultSetup();
    Move move1 = EMPTY_MOVE;
    move1.from = E2;
    move1.to = E4;
    move1.figure = board.square[move1.from];

    board.move(move1);

    Move move2 = EMPTY_MOVE;
    move2.from = E7;
    move2.to = E6;
    move2.figure = board.square[move2.from];
    move2.passant_pos_opponent = E4;

    board.move(move2);

    EXPECT_EQ(board.passant_pos, -1);
    EXPECT_FALSE(IS_PASSANT(board.square[E4]));

    board.undoMove(move2);

    EXPECT_EQ(board.passant_pos, E4);
    EXPECT_TRUE(IS_PASSANT(board.square[E4]));

    board.undoMove(move1);

    EXPECT_EQ(board.passant_pos, -1);

}
void Tests::CheckBlackPassant()
{
    board.initDefaultSetup();

    optional<Move> move1 = Move::fromString(board, "E2E3");
    board.toogleColor();
    EXPECT_TRUE(move1);

    optional<Move> move2 = Move::fromString(board, "E7E5");
    board.toogleColor();
    EXPECT_TRUE(move2);

    if (move1 && move2) {
        board.move(*move1);
        board.move(*move2);

        EXPECT_EQ(board.passant_pos, E5);
        EXPECT_TRUE(IS_PASSANT(board.square[E5]));

        board.undoMove(*move2);

        EXPECT_EQ(board.passant_pos, -1);

        board.undoMove(*move1);

        EXPECT_EQ(board.passant_pos, -1);
    } else {
        EXPECT_TRUE(false) << "Wrong else case";
    }
}
void Tests::TestMoveFromStringPositive() {
    optional<Move> move;
    board.initDefaultSetup();

    move = Move::fromString(board, "E2E4");
    EXPECT_TRUE(move);

    board.move(*move);
    EXPECT_EQ(board.passant_pos, E4);

    move = Move::fromString(board, "E7E5");
    EXPECT_TRUE(move);
    EXPECT_EQ(move->passant_pos_opponent, E4);

}

void Tests::TestMoveFromStringNegative() {
    optional<Move> move;
    board.initDefaultSetup();

    move = Move::fromString(board, "");
    EXPECT_FALSE(move);

    move = Move::fromString(board, "B1b3");
    EXPECT_FALSE(move);

    move = Move::fromString(board, "E1E1");
    EXPECT_FALSE(move);

    move = Move::fromString(board, "E7E6");
    EXPECT_FALSE(move) << "It is white turn now";
}
void Tests::TestFenOut()
{
    string fen;
    fen = "rnbq1bnr/pppkpppp/8/3p4/4P3/5N2/PPPP1PPP/RNBQKB1R w KQ - 2 3";

    board.loadFEN(fen);
    EXPECT_EQ(board.toFEN(), fen);
}
void Tests::TestFen()
{
    string fen, basic_fen;

    TestFenOut();
    CheckPassant();
    CheckBlackPassant();

    board.loadFEN("k7/4p1p1/8/4P3/8/8/5P2/K7 b - - 0 1");
    EXPECT_TRUE(IS_MOVED(board.square[E5]));

    basic_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    board.initDefaultSetup();
    EXPECT_EQ(board.toFEN(), basic_fen);
    auto move_opt = Move::fromString(board, "e2e4");
    assert(move_opt);
    board.move(*move_opt);

    EXPECT_EQ(board.toFEN(), "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");


    fen = "5k2/8/8/5p2/4P3/8/8/4K3 w - f6 2 3";
    board.loadFEN(fen);
    EXPECT_EQ(board.passant_pos, F5);

    fen = "5k2/8/8/8/4Pp2/8/8/4K3 b - e3 2 3";
    board.loadFEN(fen);
    EXPECT_TRUE(IS_PASSANT(board.square[E4]));
    EXPECT_EQ(board.passant_pos, E4);

    Move move = EMPTY_MOVE;
    move.from = F4;
    move.to = F3;
    move.passant_pos_opponent = E4;
    move.figure = board.square[F4];

    board.move(move);

    EXPECT_EQ(board.next_move_color, WHITE);

    EXPECT_EQ(board.passant_pos, -1);
    EXPECT_FALSE(IS_PASSANT(board.square[E4]));

}

TEST(TestFen, Negative)
{
    Tests tests;
    tests.TestFenNegative();
}
TEST(TestFen, Positive)
{
    Tests tests;
    tests.TestFen();
}
TEST(TestAdvanced, Positive)
{
    Tests tests;
    tests.TestAdvanced();
}

TEST(TestCheckMateIn3HalfMoves, Positive)
{
    Tests tests;
    EXPECT_TRUE(tests.TestCheckMateIn3HalfMoves());
}
TEST(TestEvaluation, Positive)
{
    Tests tests;
    EXPECT_TRUE(tests.TestEvaluation());
}

TEST(MoveFromString, Positive)
{
    Tests tests;
    tests.TestMoveFromStringPositive();
}
TEST(MoveFromString, Negative)
{
    Tests tests;
    tests.TestMoveFromStringNegative();
}
TEST(FiguresCount, _)
{
    Tests tests;
    tests.FiguresCount();
}
TEST(WrongAppearingFigures, _)
{
    Tests tests;
    tests.WrongAppearingFigures();
}

