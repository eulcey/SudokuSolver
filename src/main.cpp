#include <iostream>

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cstdlib>

#include "SudokuSolver.hpp"

//#define useCamera

using namespace cv;
using namespace std;

const char* sudokuFile = "/home/mono/workspace/sudoku_solver_opencv/testfiles/sudoku1.png";

const char* windowName = "Sudoku Scanner";
const int blockSize = 2;
const int apertureSize = 3;
const double k = 0.04;
const int thresh = 100;
  RNG rng(12345);


void find_contours(Mat& image, vector<vector<Point>>& contours);


int main(int, char**) {
  VideoCapture cap(0);
  if(!cap.isOpened()) {
    std::cerr << "Couldn't open camera" << std::endl;
    return -1;
  }

  namedWindow(windowName, 1);

  Mat scannedMat, scannedGrey;
 
  bool scannedFinished = false;
  
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
    
    cvtColor(scannedMat, scannedGrey, CV_BGR2GRAY);
    blur(scannedGrey, scannedGrey, Size(3,3));

    vector<vector<Point>> contours;
    find_contours(scannedGrey, contours);

    
    double maxContourArea = 0;
    int maxPoly = -1;
    for(size_t i = 0; i < contours.size(); ++i) {
      double area = contourArea(Mat(contours[i]));
      if(area > maxContourArea) {
	maxContourArea = area;
	maxPoly = i;
      }
    }
    if(maxPoly != -1) {
      const Point* pts[1] = {&contours[maxPoly][0]};
      int npt[] = {4};
      polylines(scannedMat, pts, npt, 1, true, Scalar(0, 0, 255), 3);
    }
    imshow(windowName, scannedMat);
    
    if(waitKey(30) == 'c') {
      break;
    }
#ifndef useCamera
    scannedFinished = true;
#endif // useCamera
  }
  SudokuSolver solver(scannedMat);

  const Mat* solvedMat =  solver.solve();

  waitKey(0);
  return 0;
}

// Rectangle Detection from
//http://stackoverflow.com/questions/8667818/opencv-c-obj-c-detecting-a-sheet-of-paper-square-detection

double angle( cv::Point pt1, cv::Point pt2, cv::Point pt0 ) {
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

void find_contours(Mat& image, vector<vector<Point>>& squares) {
  Mat canny_output;
  vector<vector<Point>> contours;
  vector<Vec4i> hierarchy;

  Canny(image, canny_output, thresh, thresh*2, 3);
  findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

  vector<Point> poly;
  for(size_t i = 0; i < contours.size(); ++i) {
    approxPolyDP(Mat(contours[i]), poly, arcLength(Mat(contours[i]), true)*0.1, true);
    if(poly.size() == 4
       && isContourConvex(Mat(poly))
       && contourArea(Mat(poly)) > 1000
       ) {
      double maxCosine = 0;
      for(size_t j = 2; j < 5; ++j) {
	double cosine = fabs(angle(poly[j%4], poly[j-2], poly[j-1]));
	maxCosine = MAX(maxCosine, cosine);
      }
      if(maxCosine < 0.1) {
	squares.push_back(poly);
      }
    }
  }
  
}
  
