#ifndef ANALYSIS_HPP
#define ANALYSIS_HPP

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include "CrackDetector.hpp"

enum class SeverityLevel { TRES_FAIBLE, FAIBLE, MOYENNE, ELEVEE, CRITIQUE };

struct RoadMetrics {
    // Métriques
    int crackCount = 0;
    float totalLength = 0.0f;
    float totalArea = 0.0f;
    float degradationPercent = 0.0f;
    float averageWidth = 0.0f;
    float maxWidth = 0.0f;
    float avgDepth = 0.0f; // Profondeur estimée
    
    // Expertise
    SeverityLevel severity = SeverityLevel::TRES_FAIBLE;
    std::string severityText = "NON ANALYSE";
    std::string repairMethod;   
    std::string preventiveAction; 
    
    // Données Graphiques
    std::vector<float> widthHistogram; 
};

class Analysis {
public:
    Analysis();
    ~Analysis();

    RoadMetrics analyze(const std::vector<Crack>& cracks, const cv::Mat& image);
    bool exportReportImage(const std::string& filepath, const RoadMetrics& metrics, const cv::Mat& resultImage);

    // Fonctions utilitaires
    bool exportToCSV(const std::string& filepath, const RoadMetrics& metrics);
    bool exportToJSON(const std::string& filepath, const RoadMetrics& metrics);
    bool exportToPDF(const std::string& filepath, const RoadMetrics& metrics, const cv::Mat& image);
    SeverityLevel calculateSeverity(float width, int count);
    std::string generateReport(const RoadMetrics& metrics);
    std::string generateDetailedReport(const RoadMetrics& metrics);

private:
    float pixelToMeter;
    float pixelToCm;
};

#endif