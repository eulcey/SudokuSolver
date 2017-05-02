#ifndef DIGIT_RECOGNIZER_HPP
#define DIGIT_RECOGNIZER_HPP

#include <opencv2/core/mat.hpp>

class DigitRecognizer {
public:
  DigitRecognizer()
    : trained(false) {}
  DigitRecognizer(cv::Mat fLayer, cv::Mat secLayer)
    : trained(true), _firstL(fLayer), _secL(secLayer) {}

  void train(const std::vector<cv::Mat> trainingSet);
  int recognize(const cv::Mat& number);
  void saveLayers(const std::string& filename);
private:
  bool trained;
  cv::Mat _firstL, _secL;
};

#endif //DIGIT_RECOGNIZER_HPP
