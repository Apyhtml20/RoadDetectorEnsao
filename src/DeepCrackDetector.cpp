#include "DeepCrackDetector.hpp"
#include <iostream>

DeepCrackDetector::DeepCrackDetector() : modelLoaded(false) {
    // Tente de charger un modèle par défaut si présent
    loadModel("assets/model.onnx");
}

DeepCrackDetector::~DeepCrackDetector() {}

bool DeepCrackDetector::loadModel(const std::string& modelPath) {
    try {
        net = cv::dnn::readNetFromONNX(modelPath);
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
        modelLoaded = !net.empty();
        if(modelLoaded) std::cout << "[IA] Modele charge: " << modelPath << std::endl;
    } catch (const cv::Exception& e) {
        std::cerr << "[IA] Erreur chargement modele: " << e.what() << std::endl;
        modelLoaded = false;
    }
    return modelLoaded;
}

bool DeepCrackDetector::isModelLoaded() const { return modelLoaded; }

cv::Mat DeepCrackDetector::predict(const cv::Mat& input) {
    if (!modelLoaded || input.empty()) return cv::Mat::zeros(input.size(), CV_8UC1);

    // 1. Preprocessing pour le reseau (Resize + Normalisation)
    cv::Mat blob;
    cv::dnn::blobFromImage(input, blob, 1.0/255.0, cv::Size(inputWidth, inputHeight), cv::Scalar(), true, false);

    // 2. Inference
    net.setInput(blob);
    cv::Mat prob = net.forward();

    // 3. Post-processing (Parsing de la sortie)
    // La forme de sortie depend du modele, supposons ici [1, 1, H, W] ou [1, H, W]
    cv::Mat scoreMap;
    if (prob.dims == 4) {
        scoreMap = cv::Mat(prob.size[2], prob.size[3], CV_32F, prob.ptr<float>());
    } else {
        scoreMap = cv::Mat(prob.size[1], prob.size[2], CV_32F, prob.ptr<float>());
    }

    // Seuillage a 50% de confiance
    cv::Mat binary;
    cv::threshold(scoreMap, binary, 0.5, 255, cv::THRESH_BINARY);
    binary.convertTo(binary, CV_8UC1);

    // Remise a l'echelle originale
    cv::resize(binary, binary, input.size(), 0, 0, cv::INTER_NEAREST);

    return binary;
}

std::vector<Crack> DeepCrackDetector::maskToCracks(const cv::Mat& binaryMask) {
    std::vector<Crack> cracks;
    std::vector<std::vector<cv::Point>> contours;
    
    cv::findContours(binaryMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (const auto& cnt : contours) {
        double area = cv::contourArea(cnt);
        if (area < 50) continue; // Filtre bruit

        Crack c;
        c.contour = cnt;
        c.area = area;
        c.length = cv::arcLength(cnt, false);
        c.bbox = cv::boundingRect(cnt);
        // Estimation largeur moyenne = Aire / Longueur
        c.width = (c.length > 0) ? (c.area / c.length) : 0; 
        
        cracks.push_back(c);
    }
    return cracks;
}