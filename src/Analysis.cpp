#include "../include/Analysis.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <numeric>
#include <algorithm>
#include <vector>
#include <cmath>

Analysis::Analysis() : pixelToMeter(0.002f), pixelToCm(0.2f) {} 
Analysis::~Analysis() {}

RoadMetrics Analysis::analyze(const std::vector<Crack>& cracks, const cv::Mat& image) {
    RoadMetrics m; 
    m.crackCount = cracks.size(); 
    m.widthHistogram.clear();
    
    float totalArea = 0;
    for(const auto& c : cracks) {
        float wCm = c.width * pixelToCm;
        m.totalLength += c.length * pixelToMeter;
        totalArea += c.area;
        m.widthHistogram.push_back(wCm);
    }

    if (!m.widthHistogram.empty()) {
        m.maxWidth = *std::max_element(m.widthHistogram.begin(), m.widthHistogram.end());
        float sum = std::accumulate(m.widthHistogram.begin(), m.widthHistogram.end(), 0.0f);
        m.averageWidth = sum / m.widthHistogram.size();
    }

    float imgArea = (float)(image.cols * image.rows);
    m.totalArea = totalArea * (pixelToCm * pixelToCm / 10000.0f); 
    m.degradationPercent = (imgArea > 0) ? (totalArea / imgArea) * 100.0f : 0;
    m.degradationPercent = std::min(m.degradationPercent * 15.0f, 100.0f); 
    
    // Estimation Profondeur (Heuristique métier : P ~= 0.8 * Largeur)
    m.avgDepth = m.averageWidth * 0.8f;

    // --- INTELLIGENCE ARTIFICIELLE (REGLES METIER) ---
    if (m.crackCount == 0) {
        m.severity = SeverityLevel::TRES_FAIBLE;
        m.severityText = "STRUCTURE SAINE";
        m.repairMethod = "Aucune intervention curative necessaire.";
        m.preventiveAction = "Poursuite du plan de surveillance standard.";
    } 
    else if (m.maxWidth < 0.5) {
        m.severity = SeverityLevel::FAIBLE;
        m.severityText = "DEBUT DE FATIGUE";
        m.repairMethod = "Application d'un enduit de regeneration (Fog Seal).";
        m.preventiveAction = "Verification du vieillissement du liant.";
    }
    else if (m.maxWidth < 2.0) {
        m.severity = SeverityLevel::MOYENNE;
        m.severityText = "DEGRADATION ACTIVE";
        m.repairMethod = "Pontage des fissures (Mastic elastomere a chaud).";
        m.preventiveAction = "Controle de l'impermeabilite de la couche de roulement.";
    }
    else {
        m.severity = SeverityLevel::CRITIQUE;
        m.severityText = "RUPTURE STRUCTURELLE";
        m.repairMethod = "Purge localisee et reprise des enrobes (Grave Bitume).";
        m.preventiveAction = "Diagnostic geotechnique immediat (Portance).";
    }

    return m;
}

bool Analysis::exportReportImage(const std::string& filepath, const RoadMetrics& m, const cv::Mat& resultImg) {
    if(resultImg.empty()) return false;
    int w = 1200, h = 1600;
    cv::Mat doc(h, w, CV_8UC3, cv::Scalar(255,255,255));

    // En-tête Pro
    cv::rectangle(doc, cv::Rect(0,0,w,140), cv::Scalar(30,30,40), -1);
    cv::putText(doc, "RAPPORT D'EXPERTISE ROUTIERE", cv::Point(50,90), cv::FONT_HERSHEY_DUPLEX, 1.5, cv::Scalar(255,255,255), 2);
    
    // Image
    int iW = 900;
    int iH = (int)((float)resultImg.rows/resultImg.cols * iW);
    if(iH > 700) iH = 700;
    cv::Mat s; cv::resize(resultImg, s, cv::Size(iW, iH));
    s.copyTo(doc(cv::Rect(150, 180, iW, iH)));
    cv::rectangle(doc, cv::Rect(150, 180, iW, iH), cv::Scalar(0,0,0), 2);

    int y = 180 + iH + 80;
    
    auto section = [&](std::string t) {
        cv::rectangle(doc, cv::Rect(100, y-35, 1000, 45), cv::Scalar(230,230,230), -1);
        cv::putText(doc, t, cv::Point(120, y), cv::FONT_HERSHEY_DUPLEX, 1.0, cv::Scalar(0,0,0), 2);
        y += 60;
    };

    auto field = [&](std::string k, std::string v) {
        cv::putText(doc, k, cv::Point(120, y), cv::FONT_HERSHEY_SIMPLEX, 0.9, cv::Scalar(80,80,80), 2);
        cv::putText(doc, v, cv::Point(600, y), cv::FONT_HERSHEY_SIMPLEX, 0.9, cv::Scalar(0,0,0), 2);
        cv::line(doc, cv::Point(120, y+15), cv::Point(1080, y+15), cv::Scalar(200,200,200), 1);
        y += 50;
    };

    section("1. ETAT DES LIEUX");
    field("Diagnostic :", m.severityText);
    field("Indice de degradation :", std::to_string((int)m.degradationPercent) + "/100");

    y += 20;
    section("2. ANALYSE QUANTITATIVE");
    field("Nombre d'anomalies :", std::to_string(m.crackCount));
    field("Lineaire total :", std::to_string(m.totalLength).substr(0,5) + " m");
    field("Ouverture maximale :", std::to_string(m.maxWidth).substr(0,4) + " cm");
    field("Profondeur est. :", std::to_string(m.avgDepth).substr(0,4) + " cm");

    y += 20;
    section("3. PLAN D'ACTION RECOMMANDE");
    cv::putText(doc, "> TECHNIQUE : " + m.repairMethod, cv::Point(120, y), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0,0,100), 2);
    y += 40;
    cv::putText(doc, "> PREVENTION : " + m.preventiveAction, cv::Point(120, y), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(50,50,50), 2);

    return cv::imwrite(filepath, doc);
}

// Stubs
bool Analysis::exportToCSV(const std::string&, const RoadMetrics&) { return true; }
SeverityLevel Analysis::calculateSeverity(float, int) { return SeverityLevel::TRES_FAIBLE; }
std::string Analysis::generateReport(const RoadMetrics&) { return ""; }
std::string Analysis::generateDetailedReport(const RoadMetrics&) { return ""; }
bool Analysis::exportToJSON(const std::string&, const RoadMetrics&) { return true; }
bool Analysis::exportToPDF(const std::string&, const RoadMetrics&, const cv::Mat&) { return false; }