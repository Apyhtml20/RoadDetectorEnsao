#ifndef DEEP_CRACK_DETECTOR_HPP
#define DEEP_CRACK_DETECTOR_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <vector>
#include "CrackDetector.hpp" // Pour la structure Crack

class DeepCrackDetector {
public:
    DeepCrackDetector();
    ~DeepCrackDetector();

    // Tente de charger le modèle ONNX
    bool loadModel(const std::string& modelPath);
    
    // Prédit le masque de fissure (Noir = fond, Blanc = fissure)
    cv::Mat predict(const cv::Mat& input);

    // Convertit le masque binaire en objets Crack exploitables par Analysis
    std::vector<Crack> maskToCracks(const cv::Mat& binaryMask);

    bool isModelLoaded() const;

private:
    cv::dnn::Net net;
    bool modelLoaded;
    const int inputWidth = 448; // Taille standard pour beaucoup de U-Nets
    const int inputHeight = 448;
};

#endif // DEEP_CRACK_DETECTOR_HPP