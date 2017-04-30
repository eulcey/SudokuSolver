#include <iostream>

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "SudokuSolver.hpp"

using namespace cv;
using namespace std;

const char* windowName = "Sudoku Scanner";
const int blockSize = 2;
const int apertureSize = 3;
const double k = 0.04;
const int thresh = 200;

void find_squares(Mat& image, vector<vector<Point>>& squares);

int main(int, char**) {

  VideoCapture cap(0);
  if(!cap.isOpened()) {
    std::cerr << "Couldn't open camera" << std::endl;
    return -1;
  }

  namedWindow(windowName, 1);

 
  bool scannedFinished = false;
  Mat scannedMat, scannedGrey;
  
  while(!scannedFinished) {
    cap >> scannedMat;
    cvtColor(scannedMat, scannedGrey, COLOR_BGR2GRAY);

    vector<vector<Point>> squares;
    find_squares(scannedMat, squares);

    for(size_t i = 0; i < squares.size(); ++i) {
      const Point* pts[1] = {&squares[i][0]};
      int npt[] = {4};
      //rectangle(scannedMat
      polylines(scannedMat, pts, npt, 1, true, Scalar(255, 0, 0));
    }

    imshow(windowName, scannedMat);
    
    if(waitKey(30) == 'c') {
      break;
    }
  }
  SudokuSolver solver(scannedMat);

  const Mat* solvedMat =  solver.solve();

  return 0;
}

double angle( cv::Point pt1, cv::Point pt2, cv::Point pt0 ) {
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

void find_squares(Mat& image, vector<vector<Point>>& squares) {
 
    Mat blurred(image);
    medianBlur(image, blurred, 9);

    Mat gray0(blurred.size(), CV_8U), gray;
    vector<vector<Point>> contours;

    // search every color plane
    for (int c = 0; c < 3; c++) {
      int ch[] = {c, 0};
      mixChannels(&blurred, 1, &gray0, 1, ch, 1);

      // different thresholds to try
      const int threshold_level = 2;
      for (size_t lvl = 0; lvl < threshold_level; ++lvl) {
	// Also try Canny (?)
	if(lvl == 0) {
	  Canny(gray0, gray, 10, 20, 3);
	  dilate(gray, gray, Mat(), Point(-1, -1));
	} else {
	  gray = gray0 >= (lvl+1) * 255 / threshold_level;
	}

	findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

	vector<Point> approx;
	for(size_t i = 0; i < contours.size(); ++i) {
	  approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);
	  // absolute is necessary, because area can be positive or negative
	  if(approx.size() == 4 &&
	     fabs(contourArea(Mat(approx))) > 1000 &&
	     isContourConvex(Mat(approx))) {
	    double maxCosine = 0;
	    for(size_t j = 2; j < 5; ++j) {
	      double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
	      maxCosine = MAX(maxCosine, cosine);
	    }
	    if(maxCosine < 0.3) {
	      squares.push_back(approx);
	    }
	    
	  }
	}
      }
    }
}
