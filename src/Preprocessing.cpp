#include "../include/Preprocessing.hpp"

Preprocessing::Preprocessing() : clipLimit(2.0), tileGridSize(8, 8), denoiseStrength(10) {}
Preprocessing::~Preprocessing() {}

cv::Mat Preprocessing::preprocess(const cv::Mat& input) {
    if (input.empty()) return cv::Mat();
    // On retourne l'image telle quelle car le CrackDetector gère le lissage spécifique
    // pour ne pas perdre les détails fins des fissures dans le bitume.
    return input; 
}

// Méthodes utilitaires conservées pour la structure
cv::Mat Preprocessing::toGray(const cv::Mat& input) {
    cv::Mat gray;
    if (input.channels() == 3) cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
    else if (input.channels() == 4) cv::cvtColor(input, gray, cv::COLOR_BGRA2GRAY);
    else gray = input.clone();
    return gray;
}

cv::Mat Preprocessing::enhanceContrastCLAHE(const cv::Mat& i) { return i; }
cv::Mat Preprocessing::denoise(const cv::Mat& i) { return i; }
cv::Mat Preprocessing::sharpen(const cv::Mat& i) { return i; }
cv::Mat Preprocessing::morphologicalOperations(const cv::Mat& i) { return i; }
cv::Mat Preprocessing::enhanceEdges(const cv::Mat& i) { return i; }
cv::Mat Preprocessing::adaptiveThreshold(const cv::Mat& i) { return i; }
cv::Mat Preprocessing::histogramEqualization(const cv::Mat& i) { return i; }
cv::Mat Preprocessing::applyGaussianBlur(const cv::Mat& i, int) { return i; }