#ifndef SUDOKU_DETECTION_HPP
#define SUDOKU_DETECTION_HPP

#include <opencv2/core/mat.hpp>

namespace sudoku {
  
  class DigitRecognizer;

  bool detectSudoku(cv::Mat& scanned, DigitRecognizer& dReg, cv::Mat& sudoku);


}; // namespace sudoku

#endif // SUDOKU_DETECTION_HPP
