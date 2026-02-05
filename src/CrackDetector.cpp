#include "../include/CrackDetector.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>

CrackDetector::CrackDetector() : minCrackArea(50.0), maxCrackArea(500000.0), minAspectRatio(2.5) {}
CrackDetector::~CrackDetector() {}

void CrackDetector::setImage(const cv::Mat& imgInput) {
    if (!imgInput.empty()) imgInput.copyTo(img);
}

std::vector<Crack> CrackDetector::detectCracks() {
    std::vector<Crack> cracks;
    if (img.empty()) return cracks;

    // Sécurisation image grise
    cv::Mat gray;
    if (img.channels() == 3) cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    else if (img.channels() == 4) cv::cvtColor(img, gray, cv::COLOR_BGRA2GRAY);
    else gray = img.clone();

    // 1. Contraste Local (CLAHE) - Indispensable pour les fissures fines
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    clahe->setClipLimit(3.0);
    clahe->setTilesGridSize(cv::Size(8, 8));
    cv::Mat enhanced;
    clahe->apply(gray, enhanced);

    // 2. Lissage (Grain)
    cv::Mat blurred;
    cv::bilateralFilter(enhanced, blurred, 9, 75, 75);

    // 3. Seuil Adaptatif (Expert)
    cv::Mat binary;
    cv::adaptiveThreshold(blurred, binary, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 17, 4);

    // 4. Nettoyage
    cv::Mat k = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::morphologyEx(binary, binary, cv::MORPH_OPEN, k);
    cv::morphologyEx(binary, binary, cv::MORPH_CLOSE, k);

    // 5. Filtrage
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(binary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (const auto& cnt : contours) {
        double area = cv::contourArea(cnt);
        if (area < 80.0) continue; 

        cv::Rect r = cv::boundingRect(cnt);
        float ratio = (float)r.width / r.height;
        if(ratio < 1.0f) ratio = 1.0f/ratio;

        if (ratio < 2.0 && area < 400.0) continue;

        Crack c;
        c.contour = cnt; c.area = area; c.bbox = r;
        c.length = cv::arcLength(cnt, true) / 2.0;
        c.width = area / c.length;
        
        if (cnt.size() >= 5) {
            cv::RotatedRect rot = cv::fitEllipse(cnt);
            c.center = rot.center; c.orientation = rot.angle;
        } else {
            c.center = cv::Point2f(r.x+r.width/2, r.y+r.height/2); c.orientation=0;
        }
        cracks.push_back(c);
    }
    return cracks;
}

cv::Mat CrackDetector::drawCracksDetailed(const cv::Mat& original, const std::vector<Crack>& cracks, bool showLabels) {
    cv::Mat res;
    if (original.channels() == 1) cv::cvtColor(original, res, cv::COLOR_GRAY2BGR);
    else original.copyTo(res);

    for(const auto& c : cracks) {
        // Code couleur sévérité
        cv::Scalar col = (c.width > 5.0) ? cv::Scalar(0,0,255) : (c.width > 2.0 ? cv::Scalar(0,165,255) : cv::Scalar(0,255,255));
        cv::drawContours(res, std::vector<std::vector<cv::Point>>{c.contour}, -1, col, 2);
        
        if(showLabels && c.length > 50) {
            cv::putText(res, std::to_string((int)c.width)+"px", c.bbox.tl(), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,255,0), 1);
        }
    }
    return res;
}

// Visualisation Matrix
cv::Mat CrackDetector::analyzeGradient(const cv::Mat& i) { 
    cv::Mat gray, e;
    if(i.channels()==3) cv::cvtColor(i, gray, cv::COLOR_BGR2GRAY); else gray = i.clone();
    cv::Canny(gray, e, 50, 150);
    cv::cvtColor(e, e, cv::COLOR_GRAY2BGR);
    // Teinte verte
    std::vector<cv::Mat> ch; cv::split(e, ch);
    ch[0] *= 0; ch[2] *= 0; 
    cv::merge(ch, e);
    return e; 
}

// Stubs
double CrackDetector::calculateCrackLength(const std::vector<cv::Point>&) { return 0; }
double CrackDetector::calculateCrackWidth(const std::vector<cv::Point>&, double) { return 0; }
double CrackDetector::calculateOrientation(const std::vector<cv::Point>&) { return 0; }
bool CrackDetector::isValidCrack(const Crack&) { return true; }
cv::Mat CrackDetector::createHeatmap(const cv::Mat& o, const std::vector<Crack>&) { return o.clone(); }
std::vector<double> CrackDetector::getCrackStatistics(const std::vector<Crack>&) { return {}; }
cv::Mat CrackDetector::drawCracks(const cv::Mat& o, const std::vector<Crack>& c) { return drawCracksDetailed(o, c, false); }