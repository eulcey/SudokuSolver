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

const char* sudokuFile = "sudokus/sudoku1.png";
#ifndef useCamera
const char* NN_MATRICES_FILE = "files/NN_matrices_image.xml";
#else
const char* NN_MATRICES_FILE = "files/NN_matrices_camera.xml";
#endif // useCamera
const char* windowName = "Sudoku Scanner";

//void find_contours(Mat& image, vector<vector<Point>>& contours);

void setMostFrequent(const Mat& sudoku, vector<vector<vector<int>>>& freq);
void getMostFrequent(const vector<vector<vector<int>>>& freq, Mat& final);

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
  Mat finalSudoku;//(sudoku);//, CV_32S)
  sudoku.convertTo(finalSudoku, CV_32S);
  vector<vector<vector<int>>> frequencies;
  for(size_t row = 0; row < 9; ++row) {
    vector<vector<int>> rowV;
    for(size_t col = 0; col < 9; ++col) {
      vector<int> colV({0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
      rowV.push_back(colV);
    }
    frequencies.push_back(rowV);
  }
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

    int saveCount = 0;
    if(detectSudoku(scannedMat, dReg, sudoku) && saveCount < 3) {
    
      saveCount++;
      imshow(windowName, (scannedMat));
      //cout << "Sudoku detected!" << endl;
      //cout << sudoku << endl;
      setMostFrequent(sudoku, frequencies);
#ifndef useCamera
      scannedFinished = true; // finish scan if image from file is used
#endif // useCamera
    }
    char input = waitKey(30);
    if(input == 'c') {
      scannedFinished = true;
      break;
    }
#ifndef useCamera
    //scannedFinished = true;
#endif // useCamera
  }

  // freq right values
  int val = frequencies[0][0][5];
  cout << "For (0,0) val 5: " << val << endl;
  val = frequencies[5][0][7];
  cout << "For (5,0) val 7: " << val << endl;
  val = frequencies[8][8][9];
  cout << "For (8, 8) val 9: " << val << endl;

  vector<int> firstCell = frequencies[0][1];
  for(size_t i = 0; i < firstCell.size(); ++i) {
    cout << "freq for " << i << " is " << firstCell[i] << endl;
  }

 getMostFrequent(frequencies, finalSudoku);
  cout << "Finalsudoku: " << endl;
  cout << finalSudoku << endl;
    
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

/*
 * updates and gets the sudoku matrix with the 
 * most frequent number for each cell
 * according to all read sudokus
 */
void setMostFrequent(const Mat& sudoku, vector<vector<vector<int>>>& freqArr) {
  for(size_t row = 0; row < 9; ++row) {
    for(size_t col = 0; col < 9; ++col) {
      int nr = static_cast<int>(sudoku.at<char>(row, col));
      // char val = sudoku.at<char>(row, col);
      //cout << "Value: " << val << " equals int: " << nr << endl;
      freqArr[row][col][nr] += 1;
    }
  }
}

void getMostFrequent(const vector<vector<vector<int>>>& freqArr, Mat& final) {
  for(size_t row = 0; row < 9; ++row) {
    for(size_t col = 0; col < 9; ++col) {
      int maxFreq = 0;
      int maxNr = 0;
      for(size_t nr = 0; nr < 10; ++nr) {
	int nrFreq = freqArr[row][col][nr];
	if(nrFreq > maxFreq) {
	  maxFreq = nrFreq;
	  maxNr = nr;
	}
      }
      //cout << "For nr: " << maxNr << " freq is " << maxFreq << endl;
      final.at<int>(row, col) = maxNr;
    }
  }
}
