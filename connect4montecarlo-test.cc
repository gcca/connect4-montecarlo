#include <fstream>
#include <sstream>

#include <gtest/gtest.h>

#include "game.hpp"

class BoardTest : public testing::Test {
protected:
  void SetUp() override {
    std::ifstream ifs("./demo1.txt");
    board = Board::Make(ifs, Pawn::P1);
  }

  Board board;
};

TEST_F(BoardTest, Show) {
  std::ostringstream oss;
  board.Show(oss);

  EXPECT_EQ(oss.str(),
            "-------\n-------\n-------\n----1--\n--0010-\n0-10101\n");
}

TEST_F(BoardTest, Move) {
  board.Move(4);

  std::ostringstream oss;
  board.Show(oss);

  EXPECT_EQ(oss.str(),
            "-------\n-------\n----1--\n----1--\n--0010-\n0-10101\n");
}

TEST_F(BoardTest, Move2) {
  board.Move(4).Move(4);

  std::ostringstream oss;
  board.Show(oss);

  EXPECT_EQ(oss.str(),
            "-------\n----0--\n----1--\n----1--\n--0010-\n0-10101\n");
}

TEST_F(BoardTest, FindWinner) {
  EXPECT_EQ(board.Move(4).FindWinner().value(), Pawn::P1);
}

TEST_F(BoardTest, HasNotFinished) { EXPECT_FALSE(board.HasFinished()); }

TEST(BoardTest2, HasFinished) {
  std::ifstream ifs("./demo2.txt");
  Board board = Board::Make(ifs, Pawn::P1);
  EXPECT_TRUE(board.HasFinished());
}

class SimulatorTest : public testing::Test {
protected:
  void SetUp() override {
    std::ifstream ifs("./demo1.txt");
    board = Board::Make(ifs, Pawn::P1);
    simulator = Simulator(Pawn::P1);
  }

  Board board;
  Simulator simulator;
};

TEST_F(SimulatorTest, Simulate) {
  auto winner = simulator.SimulateMove(board, 4);

  EXPECT_TRUE(winner.has_value());
  EXPECT_EQ(winner.value(), Pawn::P1);
}

TEST_F(SimulatorTest, BestMove) {
  auto simulation = simulator.EvalBestMove(board, 2000);
  EXPECT_EQ(simulation.best_move, 4);
}
