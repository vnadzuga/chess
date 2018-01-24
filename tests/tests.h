#pragma once
#include "chessboard.h"
class Tests
{
public:
    Tests();
    bool RunAll();
    bool TestEvaluation();
    bool TestCheckMateIn3HalfMoves();
    void TestFenNegative();
    void CheckPassant();
    void CheckBlackPassant();
    void TestFenOut();
    void TestFen();
    void TestAdvanced();
    void FiguresCount();
    void WrongAppearingFigures();

    void TestMoveFromStringPositive();
    void TestMoveFromStringNegative();

    ChessBoard board;
};
