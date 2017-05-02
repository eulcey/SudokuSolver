#include <iostream>

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cstdlib>

#include "SudokuDetection.hpp"
#include "SudokuSolver.hpp"

//#define useCamera

using namespace cv;
using namespace std;

const char* sudokuFile = "/home/mono/workspace/sudoku_solver_opencv/testfiles/sudoku2.png";

const char* windowName = "Sudoku Scanner";

void find_contours(Mat& image, vector<vector<Point>>& contours);


int main(int, char**) {
  VideoCapture cap(0);
  if(!cap.isOpened()) {
    std::cerr << "Couldn't open camera" << std::endl;
    return -1;
  }

  namedWindow(windowName, 1);

  Mat scannedMat;
  bool scannedFinished = false;
  Mat* sudoku;
  
  while(!scannedFinished) {
#ifdef useCamera
    cap >> scannedMat;
#else
    scannedMat = imread(sudokuFile, CV_LOAD_IMAGE_COLOR);
    if(!scannedMat.data) {
      cerr << "Couldn't open file" << endl;
      return -1;
    }
#endif // useCamera

    sudoku = detectSudoku(scannedMat);
    
    imshow(windowName, scannedMat);

    if(sudoku) {
      scannedFinished = true;
    }
    
    if(waitKey(30) == 'c') {
      break;
    }
#ifndef useCamera
    scannedFinished = true;
#endif // useCamera
  }
  
  SudokuSolver solver(*sudoku);
  const Mat* solvedMat = solver.solve();
  
  cout << *solvedMat << endl;

  waitKey(0);
  return 0;
}
