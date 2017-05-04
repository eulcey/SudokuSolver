#include "SudokuDetection.hpp"

#include <iostream>

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "DigitRecognizer.hpp"

using namespace cv;
using namespace std;

const int thresh = 100;
const int sudokuSize = 9;

void find_contours(Mat& image, vector<vector<Point>>& squares);
void calcCells(const Mat& scanned, const vector<Point>& frame, vector<Mat>& cells);
bool scanCells(const vector<Mat>& cells, Mat& res);

bool detectSudoku(Mat& scanned, Mat& sudoku) {
  Mat scannedGrey;
      
  cvtColor(scanned, scannedGrey, CV_BGR2GRAY);
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


  // TODO better test
  if(maxPoly != -1 && contours.size() > 20) { // test if really so whole sudoku was read
    const Point* pts[1] = {&contours[maxPoly][0]};
    int npt[] = {4};
    polylines(scanned, pts, npt, 1, true, Scalar(0, 0, 255), 3);
    for(size_t i = 0; i < contours[maxPoly].size(); ++i) {
      cout << contours[maxPoly][i] << " ";
    }
    cout << endl;

    // cout << "Contour size: " << contours.size() << endl;

    vector<Mat> cells;
    calcCells(scannedGrey(Rect(contours[maxPoly][0], contours[maxPoly][2])), contours[maxPoly], cells);
    if(!scanCells(cells, sudoku)) {
      return false;
    }
    return true;
  }
  return false;
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


void calcCells(const Mat& scanned, const vector<Point>& frame, vector<Mat>& res) {
  int colToCut = scanned.cols%sudokuSize;
  int rowToCut = scanned.rows%sudokuSize;
  int x = rowToCut/2;
  int y = colToCut/2;
  Mat cutScanned = scanned(cv::Rect(y, x, scanned.cols-colToCut, scanned.rows - rowToCut)).clone();
  for(size_t col = 0; col < cutScanned.cols; col += cutScanned.cols/sudokuSize) {
    for(size_t row = 0; row < cutScanned.rows; row += cutScanned.rows/sudokuSize) {
      res.push_back(cutScanned(cv::Rect(col, row, (cutScanned.cols / sudokuSize),
				  (cutScanned.rows / sudokuSize))).clone());
    }
  }
}

void formatCell(const Mat& cell, int fCount, Mat& formated) {
}

/*
 * returns the matrix of a soduko
 */
bool scanCells(const vector<Mat>& cells, Mat& res) {
  //  Mat* res = new Mat(9, 9, CV_8U);

  namedWindow("cell" , 1);
  imshow("cell", cells[39]);

  cout << cells[39].size() << endl;
  
  DigitRecognizer dReg;
  if(!loadNN("NN_matrices.xml", dReg)) {
    cerr << "Couldn't load matrices for DigitRecognizer" << endl;
    return false;
  }
  

  for(size_t col = 0; col < sudokuSize; ++col) {
    for(size_t row = 0; row < sudokuSize; ++row) {
      Mat formatedCell;

      
      int v = row+sudokuSize*col;
      std::string name = "testfiles/";
      name += to_string(v);
      name += ".jpg";
      imwrite(name, cells[v]);
      
      
      formatCell(cells[row + sudokuSize*col], DigitRecognizer::FEATURE_COUNT, formatedCell);
      
      res.at<char>(row, col) = dReg.recognize(formatedCell);
    }
  }
  
  return true;
}
