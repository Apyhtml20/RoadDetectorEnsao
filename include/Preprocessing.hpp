#ifndef PREPROCESSING_HPP
#define PREPROCESSING_HPP

#include <opencv2/opencv.hpp>

class Preprocessing {
public:
    Preprocessing();
    ~Preprocessing();

    // Pipeline principal
    cv::Mat preprocess(const cv::Mat& input);
    
    // Méthodes individuelles (Version Pro)
    cv::Mat toGray(const cv::Mat& input);
    cv::Mat enhanceContrastCLAHE(const cv::Mat& gray);
    cv::Mat denoise(const cv::Mat& img);
    cv::Mat sharpen(const cv::Mat& img);
    cv::Mat morphologicalOperations(const cv::Mat& img);
    
    // Méthodes avancées
    cv::Mat enhanceEdges(const cv::Mat& img);
    cv::Mat adaptiveThreshold(const cv::Mat& img);
    cv::Mat histogramEqualization(const cv::Mat& img);

    // Helper pour compatibilité
    cv::Mat applyGaussianBlur(const cv::Mat& input, int kernelSize);
    
    // Configuration
    void setClipLimit(double limit) { clipLimit = limit; }
    void setTileSize(int size) { tileGridSize = cv::Size(size, size); }
    void setDenoiseStrength(int strength) { denoiseStrength = strength; }

private:
    double clipLimit;
    cv::Size tileGridSize;
    int denoiseStrength;
};

#endif // PREPROCESSING_HPP