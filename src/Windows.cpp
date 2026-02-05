#include "../include/Windows.hpp"
#include "../include/FileDialog.hpp"
#include "../include/imgui/imgui.h"
#include "../include/imgui/imgui_internal.h"
#include "../include/imgui/imgui_impl_sdl3.h"
#include "../include/imgui/imgui_impl_sdlrenderer3.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

// --- THEME ---
void SetupRoadTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 10.0f;
    style.FrameRounding = 6.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.12f, 0.95f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
    ImVec4 roadYellow = ImVec4(0.95f, 0.70f, 0.05f, 1.00f);
    style.Colors[ImGuiCol_Button] = roadYellow; 
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 0.8f, 0.1f, 1.0f);
    style.Colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = roadYellow;
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.2f, 1.0f, 0.4f, 1.0f);
}

bool ButtonRoad(const char* label, const ImVec2& size_arg) {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.1f, 0.1f, 1.0f)); 
    bool pressed = ImGui::Button(label, size_arg);
    ImGui::PopStyleColor();
    return pressed;
}

void DrawRoadBackground() {
    ImDrawList* draw = ImGui::GetBackgroundDrawList();
    ImVec2 s = ImGui::GetMainViewport()->Size;
    draw->AddRectFilledMultiColor(ImVec2(0,0), s, 
        IM_COL32(30,30,35,255), IM_COL32(30,30,35,255), 
        IM_COL32(10,10,15,255), IM_COL32(10,10,15,255));
    
    float t = (float)SDL_GetTicks() / 1000.0f;
    float offset = fmod(t * 180.0f, 120.0f); 
    for (float y = -120+offset; y < s.y; y += 120) 
        draw->AddRectFilled(ImVec2(s.x/2-6, y), ImVec2(s.x/2+6, y+60), IM_COL32(255,255,255,180));
}

Windows::Windows(int w, int h) 
    : isProcessing(false), threadFinished(false), zoomLevel(1.0f),
      window(nullptr), renderer(nullptr), running(true),
      winW(w), winH(h), currentState(STATE_MENU),
      texOriginal(nullptr), texPreprocess(nullptr), texGradient(nullptr), texResult(nullptr), texLogo(nullptr),
      analysisDone(false), animTime(0)
{
    sensitivitySlider.value = 50.0f;
}

Windows::~Windows() { cleanup(); }

SDL_Texture* Windows::matToTexture(const cv::Mat& mat) {
    if (mat.empty()) return nullptr;
    cv::Mat rgbMat;
    if (mat.channels() == 4) cv::cvtColor(mat, rgbMat, cv::COLOR_BGRA2RGBA); 
    else if (mat.channels() == 3) cv::cvtColor(mat, rgbMat, cv::COLOR_BGR2RGB);
    else if (mat.channels() == 1) cv::cvtColor(mat, rgbMat, cv::COLOR_GRAY2RGB);
    else return nullptr;

    SDL_Surface* surface = SDL_CreateSurfaceFrom(rgbMat.cols, rgbMat.rows, 
        (mat.channels()==4 ? SDL_PIXELFORMAT_ABGR8888 : SDL_PIXELFORMAT_RGB24), 
        rgbMat.data, rgbMat.step);
    if (!surface) return nullptr;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    return tex;
}

bool Windows::initialize() {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) return false;
    window = SDL_CreateWindow("RoadDetectorEnsao", winW, winH, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    renderer = SDL_CreateRenderer(window, NULL);
    IMGUI_CHECKVERSION(); ImGui::CreateContext(); ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    ImFont* f = ImGui::GetIO().Fonts->AddFontFromFileTTF("assets/font.ttf", 18.0f);
    if (!f) ImGui::GetIO().Fonts->AddFontDefault();
    ImFontConfig c; c.MergeMode = false;
    ImGui::GetIO().Fonts->AddFontFromFileTTF("assets/font.ttf", 36.0f, &c); 
    
    SetupRoadTheme();
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    cv::Mat logoMat = cv::imread("assets/ensa_logo.png", cv::IMREAD_UNCHANGED);
    if (!logoMat.empty()) texLogo = matToTexture(logoMat);

    return true;
}

void Windows::run() {
    while (running) {
        SDL_Event e; while (SDL_PollEvent(&e)) { ImGui_ImplSDL3_ProcessEvent(&e); if (e.type == SDL_EVENT_QUIT) running = false; }
        checkAsyncResults(); 
        ImGui_ImplSDLRenderer3_NewFrame(); ImGui_ImplSDL3_NewFrame(); ImGui::NewFrame();
        if (currentState == STATE_MENU) renderMenu(); else renderDashboard();
        ImGui::Render(); ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer); SDL_RenderPresent(renderer);
    }
}

void Windows::renderMenu() {
    DrawRoadBackground();
    ImVec2 c = ImGui::GetMainViewport()->GetCenter();
    ImVec2 winSize(600, 500);
    
    ImGui::SetNextWindowPos(c, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(winSize);
    
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.05f, 0.05f, 0.07f, 0.90f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f); 
    
    ImGui::Begin("M", 0, ImGuiWindowFlags_NoDecoration);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 pMin = ImGui::GetWindowPos();
    ImVec2 pMax = ImVec2(pMin.x + winSize.x, pMin.y + winSize.y);
    dl->AddRect(pMin, pMax, IM_COL32(255, 180, 0, 150), 10.0f, 0, 2.0f); 

    // TITRE
    ImGui::SetCursorPosY(50);
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]); 
    const char* title = "RoadDetectorEnsao";
    float tw = ImGui::CalcTextSize(title).x;
    ImGui::SetCursorPosX((winSize.x - tw) * 0.5f);
    ImGui::TextColored(ImVec4(1,1,1,1), "%s", title);
    ImGui::PopFont();
    
    // LOGO - CORRECTION FLOAT SDL3
    if (texLogo) {
        float logoH = 120.0f;
        float w, h; 
        SDL_GetTextureSize(texLogo, &w, &h);
        float logoW = (w / h) * logoH;
        
        ImGui::SetCursorPosY(110);
        ImGui::SetCursorPosX((winSize.x - logoW) * 0.5f);
        ImGui::Image((ImTextureID)texLogo, ImVec2(logoW, logoH));
    }

    // SOUS-TITRE
    ImGui::SetCursorPosY(250);
    const char* sub = "SYSTEME D'EXPERTISE ROUTIERE";
    float sw = ImGui::CalcTextSize(sub).x;
    ImGui::SetCursorPosX((winSize.x - sw) * 0.5f);
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1), "%s", sub);

    // BOUTON
    ImGui::SetCursorPosY(320);
    ImGui::SetCursorPosX(125);
    dl->AddRectFilled(ImVec2(pMin.x+130, pMin.y+325), ImVec2(pMin.x+480, pMin.y+385), IM_COL32(0,0,0,100), 4.0f);
    if (ButtonRoad("CHARGER UNE INSPECTION", ImVec2(350, 60))) actionLoad();
    
    ImGui::SetCursorPosY(450); 
    const char* foot = "ENSA Oujda - Genie Informatique 3";
    float fw = ImGui::CalcTextSize(foot).x;
    ImGui::SetCursorPosX((winSize.x - fw) * 0.5f);
    ImGui::TextDisabled("%s", foot);

    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

void Windows::renderDashboard() {
    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->Pos); ImGui::SetNextWindowSize(ImVec2(420, vp->Size.y));
    ImGui::Begin("Side", 0, ImGuiWindowFlags_NoDecoration);
    
    if (ImGui::Button("ACCUEIL")) currentState = STATE_MENU;
    ImGui::SameLine(); ImGui::TextDisabled(" | DOSSIER #ES-2026-X");
    ImGui::Separator();
    
    if (analysisDone) {
        ImGui::Spacing();
        // Section 1 : DIAGNOSTIC
        if(ImGui::CollapsingHeader("1. DIAGNOSTIC STRUCTUREL", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Spacing();
            ImGui::Text("Etat de la structure :");
            ImVec4 col = (metrics.severity >= SeverityLevel::MOYENNE) ? ImVec4(1,0.2f,0.2f,1) : ImVec4(0.2f,0.8f,0.2f,1);
            ImGui::TextColored(col, "%s", metrics.severityText.c_str());
            ImGui::ProgressBar(metrics.degradationPercent/100.0f, ImVec2(-1, 15));
            ImGui::Spacing();
        }

        // Section 2 : METRIQUES
        if(ImGui::CollapsingHeader("2. METRIQUES PRECISES", ImGuiTreeNodeFlags_DefaultOpen)) {
            if(ImGui::BeginTable("Metrics", 2, ImGuiTableFlags_BordersInnerH)) {
                ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Fissures"); ImGui::TableSetColumnIndex(1); ImGui::Text("%d", metrics.crackCount);
                ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Longueur Tot."); ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f m", metrics.totalLength);
                ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Largeur Max"); ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f cm", metrics.maxWidth);
                ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Prof. Est."); ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f cm", metrics.avgDepth);
                ImGui::EndTable();
            }
            ImGui::Spacing();
            ImGui::Text("Distribution (Largeur)");
            if (!metrics.widthHistogram.empty()) {
                float vals[50] = {0};
                float maxVal = metrics.maxWidth; if (maxVal < 1.0f) maxVal = 1.0f;
                for (float w : metrics.widthHistogram) {
                    int idx = (int)((w / maxVal) * 49);
                    if (idx >= 0 && idx < 50) vals[idx]++;
                }
                ImGui::PlotLines("##c", vals, 50, 0, NULL, 0, FLT_MAX, ImVec2(-1, 60));
            }
        }

        // Section 3 : ACTION
        if(ImGui::CollapsingHeader("3. PRECONISATIONS", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.95f, 0.7f, 0.05f, 1), "> TECHNIQUE :");
            ImGui::TextWrapped("%s", metrics.repairMethod.c_str());
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.95f, 0.7f, 0.05f, 1), "> PREVENTION :");
            ImGui::TextWrapped("%s", metrics.preventiveAction.c_str());
        }
        
        ImGui::Spacing();
        if (ButtonRoad("GENERER RAPPORT PDF", ImVec2(-1, 45))) onExportReport();
        
    } else {
        ImGui::Spacing();
        ImGui::Text("En attente d'analyse...");
        ImGui::Spacing();
        if (isProcessing) ImGui::Button("CALCUL EN COURS...", ImVec2(-1, 45));
        else if (ButtonRoad("LANCER L'ANALYSE", ImVec2(-1, 45))) actionAnalyze();
    }
    ImGui::End();
    
    // ZONE VIEWER (Images avec Titres)
    ImGui::SetNextWindowPos(ImVec2(vp->Pos.x + 420, vp->Pos.y)); 
    ImGui::SetNextWindowSize(ImVec2(vp->Size.x - 420, vp->Size.y));
    ImGui::Begin("View", 0, ImGuiWindowFlags_NoDecoration);
    
    if (texOriginal) {
        ImVec2 region = ImGui::GetContentRegionAvail();
        ImVec2 sz = ImVec2(region.x * 0.5f - 10, region.y * 0.5f - 10);
        
        auto ShowImageWithTitle = [&](const char* title, SDL_Texture* tex) {
            ImGui::BeginGroup();
            // Titre stylé
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.25f, 1));
            ImGui::Button(title, ImVec2(sz.x, 25)); 
            ImGui::PopStyleColor();
            
            if(tex) {
                float ar = (float)original.cols/original.rows;
                ImVec2 isz = sz; isz.y -= 25; 
                if(ar > isz.x/isz.y) isz.y = isz.x/ar; else isz.x = isz.y*ar;
                ImGui::Image((ImTextureID)tex, isz);
            } else {
                ImGui::Dummy(ImVec2(sz.x, sz.y - 25));
            }
            ImGui::EndGroup();
        };

        ShowImageWithTitle("1. SOURCE ORIGINALE", texOriginal);
        ImGui::SameLine();
        ShowImageWithTitle("2. GRADIENT ", texGradient);
        
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
        
        ShowImageWithTitle("3. BINARISATION", texPreprocess);
        ImGui::SameLine();
        ShowImageWithTitle("4. DETECTION FINALE", texResult);
    }
    ImGui::End();
}

void Windows::actionLoad() {
    #ifdef _WIN32
    std::string path = openFileDialogWindows(); 
    #else
    std::string path = "assets/road_sample.jpg"; 
    #endif
    if (path.empty()) return;
    cv::Mat loaded = cv::imread(path);
    if (!loaded.empty()) {
        original = loaded;
        if (texOriginal) { SDL_DestroyTexture(texOriginal); } texOriginal = matToTexture(original);
        if (texResult) { SDL_DestroyTexture(texResult); texResult = nullptr; }
        analysisDone = false; metrics = RoadMetrics(); currentState = STATE_DASHBOARD;
    }
}

void Windows::actionAnalyze() {
    if (original.empty() || isProcessing) return;
    isProcessing = true; threadFinished = false;
    workerThread = std::thread(&Windows::threadAnalyzeTask, this, original.clone(), sensitivitySlider.value);
    workerThread.detach();
}

void Windows::threadAnalyzeTask(cv::Mat input, float) {
    cv::Mat g; if(input.channels()==3) cv::cvtColor(input,g,cv::COLOR_BGR2GRAY); else g=input.clone();
    cv::Mat edges = detector.analyzeGradient(input); 
    asyncGradient = edges;
    
    detector.setImage(input);
    auto cracks = detector.detectCracks();
    asyncMetrics = analyzer.analyze(cracks, input);
    asyncVisResult = detector.drawCracksDetailed(input, cracks);
    
    cv::Mat bin; 
    cv::adaptiveThreshold(g, bin, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 15, 3);
    cv::cvtColor(bin, bin, cv::COLOR_GRAY2BGR); 
    asyncProcessed = bin;
    
    threadFinished = true;
}

void Windows::checkAsyncResults() {
    if (isProcessing && threadFinished) {
        if (texResult) SDL_DestroyTexture(texResult); texResult = matToTexture(asyncVisResult);
        if (texGradient) SDL_DestroyTexture(texGradient); texGradient = matToTexture(asyncGradient);
        if (texPreprocess) SDL_DestroyTexture(texPreprocess); texPreprocess = matToTexture(asyncProcessed);
        metrics = asyncMetrics; analysisDone = true; isProcessing = false;
    }
}

void Windows::onExportReport() {
    #ifdef _WIN32
    std::string path = saveFileDialogWindows("Image JPG (*.jpg)\0*.jpg\0", "jpg");
    #else
    std::string path = "report.jpg";
    #endif
    if (!path.empty()) analyzer.exportReportImage(path, metrics, asyncVisResult);
}

void Windows::cleanup() {
    ImGui_ImplSDLRenderer3_Shutdown(); ImGui_ImplSDL3_Shutdown(); ImGui::DestroyContext();
    if (texOriginal) SDL_DestroyTexture(texOriginal);
    if (texResult) SDL_DestroyTexture(texResult);
    if (texGradient) SDL_DestroyTexture(texGradient);
    if (texPreprocess) SDL_DestroyTexture(texPreprocess);
    if (texLogo) SDL_DestroyTexture(texLogo);
    SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); SDL_Quit();
}
