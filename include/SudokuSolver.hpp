#ifndef SUDOKU_SOLVER_HPP
#define SUDOKU_SOLVER_HPP

#include <opencv2/core/mat.hpp>

class SudokuSolver {
public:
  SudokuSolver(cv::Mat mat)
    : matrix(mat) {}

  const cv::Mat* solve();

private:
  cv::Mat matrix;
};

#endif // SUDOKU_SOLVER_HPP
