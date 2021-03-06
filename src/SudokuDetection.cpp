#include "SudokuDetection.hpp"

#include <iostream>

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "DigitRecognizer.hpp"

#define saveCells

using namespace cv;
using namespace std;

namespace sudoku {
const int thresh = 100;
const int sudokuSize = 9;

  void find_contours(cv::Mat& image, std::vector<std::vector<cv::Point>>& squares);
  void calcCells(const cv::Mat& scanned, const std::vector<cv::Point>& frame, std::vector<cv::Mat>& cells);
  bool scanCells(const std::vector<cv::Mat>& cells, DigitRecognizer& dReg, cv::Mat& res);

bool detectSudoku(Mat& scanned, DigitRecognizer& dReg, Mat& sudoku) {
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

  /*
  if(maxPoly != -1 && contours.size() > 10 && maxContourArea > 10000) {
    Mat copyScanned(scanned);
    const Point* pts[1] = {&contours[maxPoly][0]};
    int npt[] = {4};
    polylines(copyScanned, pts, npt, 1, true, Scalar(0, 0, 255), 10);
    namedWindow("copy", 1);
    imshow("copy", copyScanned);
    //cout << "Max Contour Area: " << maxContourArea << endl;
  }
  */
  
  // TODO better test
  if(maxPoly != -1 && contours.size() > 10 && maxContourArea > 10000) { // test if really so whole sudoku was read
    const Point* pts[1] = {&contours[maxPoly][0]};
    int npt[] = {4};
    polylines(scanned, pts, npt, 1, true, Scalar(0, 0, 255), 10);

    /*
    for(size_t i = 0; i < contours[maxPoly].size(); ++i) {
      cout << contours[maxPoly][i] << " ";
    }
    cout << endl;
    
    cout << "Contour size: " << contours.size() << endl;
    */
    
    vector<Mat> cells;
    Mat sudokuFrame(scannedGrey(Rect(contours[maxPoly][0], contours[maxPoly][2])));
    calcCells(sudokuFrame, contours[maxPoly], cells);

#ifdef saveCells
    for(size_t c = 0; c < cells.size(); ++c) {
      string fname = "camera_training/" + to_string(c) + ".jpg";
      imwrite(fname, cells[c]);
    }
#endif // saveCells
    
    if(!scanCells(cells, dReg, sudoku)) {
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
  for(size_t row = 0; row < cutScanned.rows; row += cutScanned.rows/sudokuSize) {
    for(size_t col = 0; col < cutScanned.cols; col += cutScanned.cols/sudokuSize) {
      res.push_back(cutScanned(cv::Rect(col, row, (cutScanned.cols / sudokuSize),
				  (cutScanned.rows / sudokuSize))).clone());
    }
  }
}


/*
 * returns the matrix of a soduko
 */
bool scanCells(const vector<Mat>& cells, DigitRecognizer& dReg, Mat& res) {
  //  Mat* res = new Mat(9, 9, CV_8U);

  int cutOff = 3;
  int max_BINARY_value = 255;
  int THRESHOLD_VALUE = 100;
  Mat testC = cells[0];
  
  if(testC.rows > 2*cutOff & testC.cols > 2*cutOff) {
  if(testC.channels() == 3) {
    cvtColor(testC, testC, CV_BGR2GRAY);
  } 
  testC = testC(Rect(cutOff, cutOff, testC.cols-(2*cutOff), testC.rows-(2*cutOff)));
  threshold(testC, testC, THRESHOLD_VALUE, max_BINARY_value, 3 );
  resize(testC, testC, Size(DigitRecognizer::NEW_SIDE, DigitRecognizer::NEW_SIDE));
  

  namedWindow("scanCells Bsp" , 1);
  imshow("scanCells Bsp", testC);
  Mat formated;
  if(dReg.processMat(cells[0], formated)) {
    int testNr = dReg.recognize(formated);
    //cout << "Cell 0 recognized as: " << testNr << endl;
  }
  }

  int nrCount = 0;

  for(size_t row = 0; row < sudokuSize; ++row) {
    for(size_t col = 0; col < sudokuSize; ++col) {
      Mat formatedCell;
      
      int v = row*sudokuSize + col;

      
      std::string name = "testfiles/";
      name += to_string(v);
      name += ".jpg";
      imwrite(name, cells[v]);      

      if(dReg.processMat(cells[v], formatedCell)) {

      //cout << "formatedcell: " << formatedCell.size() << endl;
      //      if(formatedCell.rows > 5 && formatedCell.cols > 5) { // test if really a sudokucell is tested
      
	int number = dReg.recognize(formatedCell);

	if(number > 0) {
	  ++nrCount;
	}
	res.at<char>(row, col) = static_cast<char>(number);
	//cout << "scanCells Number rec: " << number << endl;
	//}
      }
      
    }
  }
  if(nrCount > 4) {
    return true;
  } else {
    return false;
  }
}

}; // namespace sudoku
