# Makefile pour Road Crack Detection System (avec ImGui)
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Chemins des bibliothèques (Adaptez si nécessaire)
OPENCV_INCLUDE = /ucrt64/include/opencv4
SDL3_INCLUDE = /ucrt64/include/SDL3
SDL3_TTF_INCLUDE = /ucrt64/include/SDL3_ttf

# Ajout de include/imgui pour faciliter les imports
INCLUDES = -I$(OPENCV_INCLUDE) -I$(SDL3_INCLUDE) -I$(SDL3_TTF_INCLUDE) -Iinclude -Iinclude/imgui
LDFLAGS = -Llib -L/ucrt64/lib

# Liste des bibliothèques
# Ajout de -limm32 (Indispensable pour ImGui sur Windows)
LIBS = -lopencv_core455 \
       -lopencv_imgproc455 \
       -lopencv_imgcodecs455 \
       -lopencv_highgui455 \
       -lopencv_photo455 \
       -lopencv_dnn455 \
       -lSDL3 -lSDL3_ttf \
       -lcomdlg32 -lole32 -luuid -limm32

SRCDIR = src
IMGUIDIR = src/imgui
OBJDIR = obj
BINDIR = .
TARGET = $(BINDIR)/vision_test.exe

# 1. Sources principales (dans src/)
SOURCES_MAIN = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS_MAIN = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES_MAIN))

# 2. Sources ImGui (dans src/imgui/)
SOURCES_IMGUI = $(wildcard $(IMGUIDIR)/*.cpp)
OBJECTS_IMGUI = $(patsubst $(IMGUIDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES_IMGUI))

# Tout combiner
OBJECTS = $(OBJECTS_MAIN) $(OBJECTS_IMGUI)

# Détection OS pour les commandes
ifeq ($(OS),Windows_NT)
	RM = cmd /C del /Q /F
	RMDIR = cmd /C rmdir /S /Q
	MKDIR = cmd /C mkdir
	FIXPATH = $(subst /,\,$1)
else
	RM = rm -f
	RMDIR = rm -rf
	MKDIR = mkdir -p
	FIXPATH = $1
endif

all: $(OBJDIR) $(TARGET)

$(OBJDIR):
	@if not exist $(call FIXPATH,$(OBJDIR)) $(MKDIR) $(call FIXPATH,$(OBJDIR))

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)
	@echo ================================
	@echo Compilation terminee avec succes!
	@echo Executable: $(TARGET)
	@echo ================================

# Règle pour les fichiers .cpp dans src/
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Règle pour les fichiers .cpp dans src/imgui/
$(OBJDIR)/%.o: $(IMGUIDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@if exist $(call FIXPATH,$(OBJDIR)) $(RMDIR) $(call FIXPATH,$(OBJDIR))
	@if exist $(call FIXPATH,$(TARGET)) $(RM) $(call FIXPATH,$(TARGET))
	@echo Nettoyage termine!

rebuild: clean all