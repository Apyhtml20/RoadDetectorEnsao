#ifndef CRACK_DETECTOR_HPP
#define CRACK_DETECTOR_HPP

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

struct Crack {
    std::vector<cv::Point> contour;
    double length;              // en pixels
    double width;               // en pixels
    double area;                // en pixels²
    cv::Rect bbox;
    cv::Point2f center;
    double orientation;         // angle en degrés
    double perimeter;

    Crack() : length(0), width(0), area(0), orientation(0), perimeter(0) {}
};

class CrackDetector {
public:
    CrackDetector();
    ~CrackDetector();

    // Configuration
    void setImage(const cv::Mat& imgGray);
    void setMinCrackArea(double area) { minCrackArea = area; }
    void setMaxCrackArea(double area) { maxCrackArea = area; }
    void setMinAspectRatio(double ratio) { minAspectRatio = ratio; }

    // Détection
    std::vector<Crack> detectCracks();
    
    // Visualisation
    cv::Mat drawCracks(const cv::Mat& original, const std::vector<Crack>& cracks);
    cv::Mat drawCracksDetailed(const cv::Mat& original, const std::vector<Crack>& cracks, bool showLabels = true);
    cv::Mat createHeatmap(const cv::Mat& original, const std::vector<Crack>& cracks);

    // Analyse de gradient
    cv::Mat analyzeGradient(const cv::Mat& image);
    
    // Statistiques
    std::vector<double> getCrackStatistics(const std::vector<Crack>& cracks);

private:
    cv::Mat img;
    double minCrackArea;
    double maxCrackArea;
    double minAspectRatio;

    // Méthodes privées
    double calculateCrackLength(const std::vector<cv::Point>& contour);
    double calculateCrackWidth(const std::vector<cv::Point>& contour, double length);
    double calculateOrientation(const std::vector<cv::Point>& contour);
    bool isValidCrack(const Crack& crack);
};

#endif // CRACK_DETECTOR_HPP