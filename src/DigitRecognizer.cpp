#include "DigitRecognizer.hpp"

//#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <cmath>

using namespace std;
using namespace cv;

namespace sudoku {
  
  const std::string DigitRecognizer::FIRST_NAME = "FirstLayer";
  const std::string DigitRecognizer::SEC_NAME = "SecondLayer";
  const int DigitRecognizer::TRAINING_CYCLES = 200000;
  int const max_BINARY_value = 255;
  const int THRESHOLD_VALUE = 100;
  const int cutOff = 3;
 
RNG rnd(0);

Mat DigitRecognizer::sigmoid(const Mat& m) {
  Mat dest;
  exp(-m, dest);
  return 1.0/(1.0 + dest);
}
  
Mat DigitRecognizer::sigmoidDer(const Mat& m) {
  Mat dest = sigmoid(m);
  return dest.mul(1.0 - dest);
}
void DigitRecognizer::train(const Mat& trainingSet, const Mat& trainingValues) {
  
  int count = 0;
  vector<double> error;
  for(size_t i = 0; i < TRAINING_CYCLES; ++i) {
    double cycle_error = 0;
    for(size_t imNr = 0; imNr < trainingSet.rows; ++imNr) {
      Mat image = trainingSet.row(imNr);
      transpose(image, image);
      //image.convertTo(image, CV_64F);
      double bias = 1.0f;
      image.push_back(bias);
      int number = trainingValues.at<int>(imNr, 0);
      Mat target = Mat::zeros(10, 1, CV_64F);
      target.at<double>(number, 0) = 1.0;
      ++count;
      Mat firstStep = _firstL * image;
      Mat hiddenAct = sigmoid(firstStep);
      Mat secondStep = _secL * hiddenAct;
      Mat result = sigmoid(secondStep); 
      Mat error = target - result;
      sigmoidDer(secondStep);
      Mat deltaOut = sigmoidDer(secondStep).mul(error);
      Mat hiddenActT = hiddenAct.t();
      Mat lTwoChange = deltaOut * hiddenAct.t();
      Mat deltaHidden = sigmoidDer(firstStep).mul(_secL.t() * deltaOut);
      Mat lOneChange = deltaHidden * image.t();
      _secL += lTwoChange;
      _firstL += lOneChange;

      if (count % (TRAINING_CYCLES/2) == 0) {
	cout << "Train cycle: " << count << endl;
	//	cout << _firstL.type() << "*" << image.type() << endl;
	//	cout << _secL.size() << "*" << (_firstL*image).size() << endl;
	/*
	cout << "For Number " << number << " Target and Result is: " << endl;
	cout << target.t() << endl;
	cout << result.t() << endl;
	*/
      }

      cycle_error += sqrt(error.dot(error)/error.rows);
      //cout << "deltaOUt: " << deltaOut.size() <<" secLN: " << (_secL).size() << " secLT: " << secT.size() << endl;
      //Mat test = deltaOut * _secL;
      //cout << "test: " << test.size() << endl;
      //cout << "deltaOUt*secLT: " << (secT * deltaOut).size() << endl;
      //cout << "sigD firststep: " << sigmoidDer(firstStep).size() << endl;
      //cout << "deltaHidden " << deltaHidden.size() << endl;
      //cout << "hiddenAct" << hiddenAct.size() << endl;
    }
    error.push_back(cycle_error);
  }
  cout << count << " Images trained" << endl;
  trained = true;
  /*
  float arr[] = {1, 2, 3};
  float arr2[] = {1, 2, 3, 4, 5, 6};
  Mat a(1, 3, CV_32F, arr);
  Mat b(2, 3, CV_32F, arr2);
  Mat c = a * b.t();
  Mat d = b * a.t();
  cout << "a: " << a.size() << " b: " << b.size() << " c: " << c.size() << " d: " << d.size() << endl;
  */
  for(size_t i = 0; i < error.size(); i += (TRAINING_CYCLES*8)) {
    int cycle = i * (TRAINING_CYCLES*8);
    cout << "Cylce " << cycle << ": Error: " << error[i] << endl;
  }
}

int DigitRecognizer::recognize(const Mat& number) {
  Mat testN;
  //  processMat(number, testN);
  if(!trained) {
    cerr << "DigitRecognizer not trained!" << endl;
    return -1;
  }
  Mat image = number.t();
  
  // cout << "size: " << image.size() << " channels" << image.channels() << endl;
  double bias = 1.0f;
  image.push_back(bias);
  
  image.convertTo(image, CV_64F);
  Mat firstStep = _firstL * image;
  Mat hiddenAct = sigmoid(firstStep);
  Mat secondStep = _secL * hiddenAct;
  Mat result = sigmoid(secondStep);
  //cout << "Recog res: " << result << endl;
  double min, max;
  Point min_loc, max_loc;
  minMaxLoc(result, &min, &max, &min_loc, &max_loc);
  //cout << "Predicted value: " << max_loc.y << endl;
  //cout << "Predicted point: " << min << ", " << max << ", " << min_loc << ", " << max_loc << endl;
  return max_loc.y;
}

int DigitRecognizer::saveLayers(const string& filename) {
  FileStorage fs(filename, FileStorage::WRITE);

  fs << DigitRecognizer::FIRST_NAME << _firstL;
  fs << DigitRecognizer::SEC_NAME << _secL;

  fs.release();

  return 0;
}


bool loadNN(const string& filename, DigitRecognizer& dr) {
  FileStorage fs;
  fs.open(filename, FileStorage::READ);
  if(!fs.isOpened()) {
    cerr << "Failed to open: " << filename << endl;
    return false;
  }

  Mat firstL;
  Mat secL;

  fs[DigitRecognizer::FIRST_NAME] >> firstL;
  fs[DigitRecognizer::SEC_NAME] >> secL;

  dr = DigitRecognizer(firstL, secL);
  return true;
}

bool DigitRecognizer::processMat(const Mat& im, Mat& res) {
  if(im.rows <= 2*cutOff || im.cols <= 2*cutOff) {
    return false;
  }
  if(im.channels() == 3) {
    cvtColor(im, res, CV_BGR2GRAY);
    res = res(Rect(cutOff, cutOff, im.cols-(2*cutOff), im.rows-(2*cutOff)));
  } else {
    res = im(Rect(cutOff, cutOff, im.cols-(2*cutOff), im.rows-(2*cutOff)));
  }
  threshold(res, res, THRESHOLD_VALUE, max_BINARY_value, 3 );
  resize(res, res, Size(DigitRecognizer::NEW_SIDE, DigitRecognizer::NEW_SIDE));
  
  res = res.reshape(1,1);
  res.convertTo(res, CV_64F);
  double minV, maxV;
  minMaxLoc(res, &minV, &maxV);
  res -= maxV/2;
  res /= maxV/2;
  return true;
}

}; // namespace sudoku
