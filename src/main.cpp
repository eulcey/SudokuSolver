#include <iostream>

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cstdlib>

#include "SudokuDetection.hpp"
#include "SudokuSolver.hpp"
#include "DigitRecognizer.hpp"

#define useCamera

using namespace cv;
using namespace std;
using namespace sudoku;

const char* sudokuFile = "files/sudoku1.png";
const char* NN_MATRICES_FILE = "files/NN_matrices.xml";
const char* windowName = "Sudoku Scanner";

void find_contours(Mat& image, vector<vector<Point>>& contours);


int main(int, char**) {
#ifdef useCamera
  VideoCapture cap(0);
  if(!cap.isOpened()) {
    std::cerr << "Couldn't open camera" << std::endl;
    return -1;
  }
#endif // useCamera
  DigitRecognizer dReg;
  if(!loadNN(NN_MATRICES_FILE, dReg)) {
    cerr << "Couldn't load matrices for DigitRecognizer" << endl;
    return -1;
  }

  namedWindow(windowName, 1);

  Mat scannedMat;
  bool scannedFinished = false;
  Mat sudoku(9,9, CV_8U);
  
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

    
    
    imshow(windowName, scannedMat);

    if(detectSudoku(scannedMat, dReg, sudoku)) {
      cout << "Sudoku detected!" << endl;
      cout << sudoku << endl;
#ifndef useCamera
      scannedFinished = true; // finish scan if image from file is used
#endif // useCamera
    }
    if(waitKey(30) == 'c') {
      scannedFinished = true;
      break;
    }
#ifndef useCamera
    //scannedFinished = true;
#endif // useCamera
  }
  
  if(scannedFinished) {
    SudokuSolver solver(sudoku);
    Mat solved;
    if(solver.solve(solved)) {

      cout << "Sudoku solved" << endl;
      cout << solved << endl;
    
      waitKey(0);
    }
  }
  return 0;
}
