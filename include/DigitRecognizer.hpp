#ifndef DIGIT_RECOGNIZER_HPP
#define DIGIT_RECOGNIZER_HPP

#include <opencv2/core/core.hpp>

namespace sudoku {

class DigitRecognizer {
public:
  static const int FEATURE_COUNT = 400;
  static const int NEW_SIDE = 20;
  static const int HIDDEN_NODES = 30;
  static const int OUTPUT_NODES = 10;
  static const int TRAINING_CYCLES;
  
  static const std::string FIRST_NAME;
  static const std::string SEC_NAME;
 
  DigitRecognizer()
    : trained(false) {
    _firstL = cv::Mat1d(HIDDEN_NODES, FEATURE_COUNT+1);
    randu(_firstL, cv::Scalar(-1.0), cv::Scalar(1.0));
    _secL = cv::Mat1d(OUTPUT_NODES, HIDDEN_NODES);
    cv::randu(_secL, cv::Scalar(-1.0), cv::Scalar(1.0));
  }
  DigitRecognizer(cv::Mat fLayer, cv::Mat secLayer)
    : trained(true), _firstL(fLayer), _secL(secLayer) {}

  void train(const cv::Mat& trainingSet, const cv::Mat& trainingValues);
  int recognize(const cv::Mat& number);
  int saveLayers(const std::string& filename);
  bool processMat(const cv::Mat& im, cv::Mat& res);

private:
  bool trained;
  cv::Mat _firstL, _secL;
  cv::Mat sigmoid(const cv::Mat&);
  cv::Mat sigmoidDer(const cv::Mat&);
};

  bool loadNN(const std::string& filename, DigitRecognizer& dr);

}; // namespace sudoku

#endif //DIGIT_RECOGNIZER_HPP
