# 🚧 Road Vanguard - Système d'Expertise Routière

![C++](https://img.shields.io/badge/C++-17-blue.svg?style=flat&logo=c%2B%2B)
![OpenCV](https://img.shields.io/badge/OpenCV-4.x-green.svg?style=flat&logo=opencv)
![SDL3](https://img.shields.io/badge/SDL-3.0-orange.svg?style=flat&logo=sdl)
![License](https://img.shields.io/badge/License-MIT-lightgrey.svg)

> **Un système expert de vision par ordinateur pour la détection, la mesure et la classification automatique des dégradations routières.**

---


## 📝 À propos

**Road Vanguard** est un logiciel desktop haute performance développé en **C++ natif**. Il permet aux ingénieurs en génie civil d'analyser l'état d'une chaussée à partir d'une simple image. 

Contrairement aux approches classiques, ce projet intègre un pipeline de vision avancé (CLAHE, Zhang-Suen) et un moteur de décision expert pour fournir non seulement une détection, mais un véritable **diagnostic structurel** (Sévérité, Type de réparation).

## ✨ Fonctionnalités Clés

* **🔍 Détection Précise :** Utilisation de filtres adaptatifs et morphologiques pour isoler les fissures du bitume, même en conditions d'éclairage difficiles.
* **📏 Mesure Algorithmique :** Implémentation de l'algorithme de **Zhang-Suen (Squelettisation)** pour calculer la longueur réelle et la largeur moyenne au pixel près.
* **🧠 Système Expert :** Classification automatique de la gravité (Faible, Moyenne, Critique) et préconisation des travaux (Pontage, Purge, etc.).
* **⚡ Architecture Haute Performance :** * Multithreading (Worker Thread pour le calcul, Main Thread pour l'UI).
    * Gestion mémoire optimisée (Smart Pointers, références).
* **🎨 Interface Pro :** GUI moderne basée sur **ImGui** et **SDL3** avec visualisation temps réel des étapes de traitement (Gradient, Binarisation).

## 🛠️ Architecture Technique

### Pipeline de Traitement d'Image
Le cœur du logiciel repose sur une chaîne de traitement stricte :

```mermaid

