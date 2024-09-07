#include <iostream>

#include "game.hpp"

void ShowUsage(const char *progname) {
  std::cerr << "usage: " << progname << " [0|1]" << std::endl;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    ShowUsage(argv[1]);
    return EXIT_FAILURE;
  }

  Pawn pawn = Pawn::EMPTY;
  try {
    pawn = std::stol(argv[1]) == 1 ? Pawn::P1 : Pawn::P0;
  } catch (std::invalid_argument &) {
    ShowUsage(argv[1]);
    return EXIT_FAILURE;
  }

  Board board = Board::Make(std::cin, pawn);
  board.Show(std::cout);
  std::cout << std::endl;

  Simulator simulator(pawn);
  Simulation simulation = simulator.EvalBestMove(board, 5000);

  std::cout << "best move: " << simulation.best_move << std::endl
            << "win rates:" << std::endl;
  for (std::size_t i = 0; i < simulation.win_rates.size(); i++) {
    std::cout << "  " << i << " : " << simulation.win_rates[i] << std::endl;
  }

  return EXIT_SUCCESS;
}
