#ifndef SUDOKU_SOLVER_HPP
#define SUDOKU_SOLVER_HPP

#include <opencv2/core/mat.hpp>

class SudokuSolver {
public:
  SudokuSolver(cv::Mat mat)
    : matrix(mat) {}

  bool solve(cv::Mat& solved);

private:
  cv::Mat matrix;
};

#endif // SUDOKU_SOLVER_HPP
