#pragma once

#include <array>
#include <istream>
#include <optional>
#include <ostream>

constexpr std::size_t NROWS = 6;
constexpr std::size_t NCOLS = 7;

enum class Pawn { P0, P1, EMPTY, UNKOWN };

class Board {
public:
  explicit Board(const std::array<std::array<Pawn, NCOLS>, NROWS>, const Pawn);
  explicit Board(const std::array<std::array<Pawn, NCOLS>, NROWS>, const Pawn,
                 const std::array<std::size_t, NCOLS>);
  explicit Board() = default;

  void Show(std::ostream &os) const noexcept;

  Board &Move(const std::size_t col) noexcept;

  std::optional<Pawn> FindWinner() const noexcept;
  std::optional<Pawn> Simulate(std::size_t);

  bool HasFinished() const noexcept;
  bool IsValidMove(std::size_t) const noexcept;

  static Board Make(std::istream &is, const Pawn turn);

private:
  std::array<std::array<Pawn, NCOLS>, NROWS> _grid;
  Pawn _turn;
  std::array<std::size_t, NCOLS> _fill;

  bool CheckSeq(std::size_t, std::size_t, std::int64_t,
                std::int64_t) const noexcept;
};

class Simulation {
public:
  std::array<double, NCOLS> win_rates;
  std::size_t best_move;
};

class Simulator {
public:
  explicit Simulator() = default;
  explicit Simulator(Pawn pawn);

  std::optional<Pawn> SimulateMove(const Board &, std::size_t) const noexcept;
  Simulation EvalBestMove(const Board &, std::size_t) const noexcept;

private:
  Pawn _pawn;
};
