#ifndef DIGIT_RECOGNIZER_HPP
#define DIGIT_RECOGNIZER_HPP

#include <opencv2/core/core.hpp>

namespace sudoku {

  // A neural network for digit recognition
  // The images have to be 20x20 pixels for the training and prediction
  class DigitRecognizer {
  public:
    // The total amount of pixels the given Image is predicted by
    static const int FEATURE_COUNT = 400;

    // The target size of the image after processMat was called on it
    static const int NEW_SIDE = 20;

    // The amount of hidden nodes in the neural network
    static const int HIDDEN_NODES = 30;

    // The amount of output nodes of the neural network, representing the digits 0-9
    static const int OUTPUT_NODES = 10;

    // The amount of training cycles the neural network uses to train itself
    static const int TRAINING_CYCLES;

    // the name of the first weight matrix, which is used in saving it to a file
    static const std::string FIRST_NAME;

    // the name of the second weight matrix, which is used in saving it to a file
    static const std::string SEC_NAME;
    
    DigitRecognizer()
      : trained(false) {
      _firstL = cv::Mat1d(HIDDEN_NODES, FEATURE_COUNT+1);
      randu(_firstL, cv::Scalar(-1.0), cv::Scalar(1.0));
      _secL = cv::Mat1d(OUTPUT_NODES, HIDDEN_NODES);
      cv::randu(_secL, cv::Scalar(-1.0), cv::Scalar(1.0));
    }

    // Predictor for Digits
    // first Matrix is weight for input layer to hidden layer
    //    it is a 401x30 matrix of doubles
    // second matrix is weight for hidden layer to output layer
    //    it is a 30x10 matrix of doubles
    DigitRecognizer(cv::Mat fLayer, cv::Mat secLayer)
      : trained(true), _firstL(fLayer), _secL(secLayer) {}

    // Trains a neural network
    // Para: trainingSet is a 400 by X matrix, where X is the count of the examples
    // Para: trainingValues is a X by 1 matrix, where X is the same count of examples
    void train(const cv::Mat& trainingSet, const cv::Mat& trainingValues);

    // Predicts the given image as a number
    // Para: number is a X by X image
    // Output: is the predicted digit between 0 and 9
    int recognize(const cv::Mat& number);

    // Saves the learned weights in an xml file for the given filename
    // Para: the path where to save the file
    int saveLayers(const std::string& filename);

    // Converts the input im to the internal processable form res
    // para: im is any image matrix
    // para: res is a 400x1 matrix of the input image
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
