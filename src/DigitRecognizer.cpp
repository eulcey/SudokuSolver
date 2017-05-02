#include "DigitRecognizer.hpp"

#include "opencv2/highgui/highgui.hpp"

cv::RNG rnd(0);

void DigitRecognizer::train(const std::vector<cv::Mat> trainingSet) {
}

int DigitRecognizer::recognize(const cv::Mat& number) {
  return rnd.uniform(0, 100);
}

void DigitRecognizer::saveLayers(const std::string& filename) {
}
