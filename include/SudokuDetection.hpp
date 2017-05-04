#ifndef SUDOKU_DETECTION_HPP
#define SUDOKU_DETECTION_HPP

#include <opencv2/core/mat.hpp>

bool detectSudoku(cv::Mat& scanned, cv::Mat& sudoku);

#endif // SUDOKU_DETECTION_HPP
