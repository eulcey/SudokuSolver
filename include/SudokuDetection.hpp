#ifndef SUDOKU_DETECTION_HPP
#define SUDOKU_DETECTION_HPP

#include <opencv2/core/mat.hpp>

namespace sudoku {

  // Forward Declaration of the class DigitRecognizer
  class DigitRecognizer;

  // tries to detect a Sudoku matrix from the given image
  // Para: scanned any image to scan for a sudoku
  // Para: dReg is the DigitRecognizer to detect the digits in the sudoku
  // Para: sudoku is the detected sudoku, if any was found
  bool detectSudoku(cv::Mat& scanned, DigitRecognizer& dReg, cv::Mat& sudoku);


}; // namespace sudoku

#endif // SUDOKU_DETECTION_HPP
