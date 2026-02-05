# 🚧 RoadDetectorEnsao — Système d’Expertise Routière

<p align="center">
  <img src="assets/screen-1.png" width="850" alt="RoadDetectorEnsao – Aperçu global">
</p>

![C++](https://img.shields.io/badge/C++-17-blue.svg?style=flat&logo=c%2B%2B)
![OpenCV](https://img.shields.io/badge/OpenCV-4.x-green.svg?style=flat&logo=opencv)
![SDL3](https://img.shields.io/badge/SDL-3.0-orange.svg?style=flat&logo=sdl)
![License](https://img.shields.io/badge/License-MIT-lightgrey.svg)

> **Système expert de vision par ordinateur pour la détection, la mesure et la classification automatique des dégradations routières.**

---

## 📝 À propos

**RoadDetectorEnsao** est un logiciel desktop haute performance développé en **C++ natif**, destiné à l’analyse automatique de l’état des chaussées à partir d’images.

Contrairement aux approches classiques limitées à la simple détection, ce projet intègre un **pipeline avancé de vision par ordinateur** combiné à un **système expert décisionnel**, permettant de produire un **diagnostic structurel complet** :
- type de fissure
- sévérité
- recommandation de réparation

---

## ✨ Fonctionnalités Clés

- 🔍 **Détection précise**  
  Isolation robuste des fissures du bitume grâce à des filtres adaptatifs et morphologiques, même sous éclairage non uniforme.

- 📏 **Mesure algorithmique**  
  Implémentation de l’algorithme de **Zhang–Suen (squelettisation)** pour calculer la longueur réelle et la largeur moyenne des fissures.

- 🧠 **Système expert**  
  Classification automatique de la gravité (Faible, Moyenne, Critique) et génération de recommandations de maintenance (pontage, purge, reprofilage…).

- ⚡ **Architecture haute performance**  
  - Multithreading (thread de calcul séparé de l’interface)
  - Gestion mémoire optimisée (smart pointers, références)

- 🎨 **Interface professionnelle**  
  Interface graphique moderne basée sur **SDL3 + ImGui**, avec visualisation temps réel de chaque étape du traitement.

---

## 🛠️ Architecture Technique

### 🧠 Pipeline de Traitement d’Image

Le cœur du système repose sur une chaîne de traitement rigoureuse :

<p align="center">
  <img src="assets/screen-2.png" width="750" alt="Image originale">
</p>

<p align="center">
  <img src="assets/screen-3.png" width="750" alt="Amélioration de contraste CLAHE">
</p>

<p align="center">
  <img src="assets/screen-4.png" width="750" alt="Binarisation et filtrage morphologique">
</p>

<p align="center">
  <img src="assets/screen-5.png" width="750" alt="Squelettisation Zhang-Suen">
</p>

**Étapes principales :**
1. Acquisition de l’image routière
2. Amélioration du contraste (CLAHE)
3. Binarisation adaptative
4. Nettoyage morphologique
5. Squelettisation
6. Extraction de métriques géométriques
7. Diagnostic via règles expertes

---

## 🎨 Interface Graphique

<p align="center">
  <img src="assets/screen-6.png" width="800" alt="Interface SDL3 + ImGui">
</p>

<p align="center">
  <img src="assets/screen-7.png" width="800" alt="Diagnostic et visualisation temps réel">
</p>

L’interface permet :
- la navigation entre les étapes du pipeline
- la visualisation comparative (original / traité)
- l’affichage du diagnostic final et des mesures

---

## 🧱 Structure du Projet

```text
RoadDetectorEnsao/
├── assets/          # Images pour le README
├── src/             # Code source C++
├── include/         # Headers
├── lib/             # Librairies
├── fonts/           # Polices UI
├── obj/             # Fichiers objets
├── README.md
├── Makefile
make
./RoadDetectorEnsao
