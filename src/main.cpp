#include <iostream>

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "SudokuSolver.hpp"

using namespace cv;

int main(int, char**) {

  VideoCapture cap(0);
  if(!cap.isOpened()) {
    std::cerr << "Couldn't open camera" << std::endl;
    return -1;
  }

  namedWindow("Video", 1);

  Mat scannedMat;
  
  while(true) {
    Mat frame;
    cap >> frame;
    imshow("Video", frame);
    
    if(waitKey(30) == 'c') {
      break;
    }
  }

  SudokuSolver solver(scannedMat);

  const Mat* solvedMat =  solver.solve();

  return 0;
}

