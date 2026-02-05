#ifndef WINDOWS_HPP
#define WINDOWS_HPP

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include "Analysis.hpp"
#include "CrackDetector.hpp"
#include "DeepCrackDetector.hpp"
#include "Preprocessing.hpp"

struct Slider { float value; };
enum AppState { STATE_MENU, STATE_DASHBOARD };

class Windows {
public:
    Windows(int width, int height);
    ~Windows();
    bool initialize();
    void run();
    void cleanup();

private:
    Analysis analyzer;
    CrackDetector detector;
    Preprocessing preprocessor;
    DeepCrackDetector aiDetector;

    std::thread workerThread;
    std::atomic<bool> isProcessing;
    std::atomic<bool> threadFinished;
    
    cv::Mat asyncProcessed, asyncGradient, asyncDetection, asyncVisResult;
    RoadMetrics asyncMetrics;

    float zoomLevel;
    Slider sensitivitySlider; 

    void renderMenu();
    void renderDashboard();
    void threadAnalyzeTask(cv::Mat inputImage, float sensitivity);
    void checkAsyncResults();
    void actionLoad();
    void actionAnalyze();
    void onExportReport();
    SDL_Texture* matToTexture(const cv::Mat& mat);

    SDL_Window* window;
    SDL_Renderer* renderer; 
    bool running;
    int winW, winH;
    AppState currentState;

    SDL_Texture *texOriginal, *texPreprocess, *texGradient, *texResult;
    SDL_Texture *texLogo;
    cv::Mat original;
    bool analysisDone;
    float animTime;
    RoadMetrics metrics;
};
#endif