#include <algorithm>
#include <array>
#include <iostream>
#include <iterator>
#include <optional>
#include <random>
#include <utility>

#include "game.hpp"

Board::Board(const std::array<std::array<Pawn, NCOLS>, NROWS> grid,
             const Pawn turn)
    : _turn{turn} {
  for (std::size_t row = 0; row < NROWS; row++) {
    for (std::size_t col = 0; col < NCOLS; col++) {
      _grid[row][col] = grid[row][col];
    }
  }

  std::fill(std::begin(_fill), std::end(_fill), 0);
}

Board::Board(const std::array<std::array<Pawn, NCOLS>, NROWS> grid,
             const Pawn turn, const std::array<std::size_t, NCOLS> fill)
    : Board(grid, turn) {
  std::copy(fill.cbegin(), fill.cend(), _fill.begin());
}

void Board::Show(std::ostream &os) const noexcept {
  for (std::int64_t row = NROWS - 1; row >= 0; row--) {
    for (std::size_t col = 0; col < NCOLS; col++) {
      Pawn value = _grid[row][col];
      switch (value) {
      case Pawn::EMPTY: os << '-'; break;
      case Pawn::P0: os << '0'; break;
      case Pawn::P1: os << '1'; break;
      default: os << 'E';
      }
    }
    os << std::endl;
  }
}

Board &Board::Move(const std::size_t col) noexcept {
  _grid[_fill[col]++][col] = _turn;
  _turn = _turn == Pawn::P1 ? Pawn::P0 : Pawn::P1;
  return *this;
}

std::optional<Pawn> Board::FindWinner() const noexcept {
  const std::array<std::pair<std::int64_t, std::int64_t>, 4> dirs = {
      {{0, 1}, {1, 0}, {1, 1}, {1, -1}}};

  for (std::size_t row = 0; row < NROWS; row++) {
    for (std::size_t col = 0; col < NCOLS; col++) {
      for (const auto &dir : dirs) {
        if (CheckSeq(row, col, dir.first, dir.second)) {
          return _grid[row][col];
        }
      }
    }
  }

  return {};
}

bool Board::CheckSeq(const std::size_t row, const std::size_t col,
                     const std::int64_t drow,
                     const std::int64_t dcol) const noexcept {
  Pawn c = _grid[row][col];
  if (c == Pawn::EMPTY) return false;

  for (std::size_t i = 0; i < 4; i++) {
    const std::int64_t crow = row + drow * i;
    const std::int64_t ccol = col + dcol * i;
    if (crow >= NROWS || crow < 0 || ccol >= NCOLS || _grid[crow][ccol] != c) {
      return false;
    }
  }

  return true;
}

bool Board::HasFinished() const noexcept {
  for (std::size_t row = 0; row < NROWS; row++) {
    for (std::size_t col = 0; col < NCOLS; col++) {
      if (_grid[row][col] == Pawn::EMPTY) { return false; }
    }
  }
  return true;
}

bool Board::IsValidMove(const std::size_t i) const noexcept {
  return _fill[i] < NROWS;
}

Board Board::Make(std::istream &is, const Pawn turn) {
  std::array<std::array<Pawn, NCOLS>, NROWS> grid;
  std::string line;
  std::size_t row = NROWS - 1;
  std::array<std::size_t, NCOLS> fill = {0};
  while (std::getline(is, line)) {
    auto it = std::begin(grid[row]);
    for (char c : line) {
      switch (c) {
      case '-': *it = Pawn::EMPTY; break;
      case '1':
        *it = Pawn::P1;
        fill[std::distance(grid[row].begin(), it)]++;
        break;
      case '0':
        *it = Pawn::P0;
        fill[std::distance(grid[row].begin(), it)]++;
        break;
      default: *it = Pawn::UNKOWN;
      }
      it++;
    }
    row--;
  }
  return Board(grid, turn, fill);
}

Simulator::Simulator(const Pawn pawn) : _pawn{pawn} {}

std::optional<Pawn>
Simulator::SimulateMove(const Board &board,
                        const std::size_t col) const noexcept {
  Board tboard(board);
  tboard.Move(col);

  std::optional<Pawn> winner = tboard.FindWinner();
  if (winner.has_value()) return winner.value();

  if (tboard.HasFinished()) return {};

  std::random_device rd;
  std::mt19937 gen(rd());

  std::vector<std::size_t> nfill;
  nfill.reserve(NCOLS);
  for (std::size_t i = 0; i < NCOLS; i++) {
    if (tboard.IsValidMove(i)) { nfill.emplace_back(i); }
  }
  std::uniform_int_distribution<> distrib(0, nfill.size() - 1);

  while (!tboard.HasFinished() and !nfill.empty()) {
    const std::size_t pick = distrib(gen);
    const std::size_t ncol = nfill[pick];
    tboard.Move(ncol);

    std::optional<Pawn> winner = tboard.FindWinner();
    if (winner.has_value()) return winner.value();

    const std::size_t prev_size = nfill.size();
    nfill.clear();
    for (std::size_t i = 0; i < NCOLS; i++) {
      if (tboard.IsValidMove(i)) { nfill.emplace_back(i); }
    }
    if (prev_size != nfill.size()) {
      distrib = std::uniform_int_distribution<>(0, nfill.size() - 1);
    }
  }

  return {};
}

Simulation Simulator::EvalBestMove(const Board &board,
                                   std::size_t simulations_num) const noexcept {
  std::array<std::size_t, NCOLS> win_counts = {0}, total_counts = {0};
  std::array<std::size_t, NCOLS> valid_moves;
  std::size_t valid_moves_size = 0;

  for (std::size_t i = 0; i < NCOLS; i++) {
    if (board.IsValidMove(i)) { valid_moves[valid_moves_size++] = i; }
  }

  if (!valid_moves_size) { return Simulation{{0}, 0}; }

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, valid_moves_size - 1);

  while (simulations_num--) {
    const std::size_t pick = distrib(gen);
    const std::size_t col = valid_moves[pick];

    std::optional<Pawn> simulation = SimulateMove(board, col);
    total_counts[col]++;

    if (simulation.has_value() and simulation.value() == _pawn) {
      win_counts[col]++;
    }
  }

  Simulation simulation{{0}, 0};
  double best_rate = 0;

  for (std::size_t i : valid_moves) {
    double win_rate = static_cast<double>(win_counts[i]) / total_counts[i];
    if (win_rate > best_rate) {
      best_rate = win_rate;
      simulation.best_move = i;
    }
    simulation.win_rates[i] = win_rate;
  }

  return simulation;
}
