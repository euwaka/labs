# Contents

- [TODO](#todo)
- [Introduction](#introduction)
- [Guide](#guide)
  - [Prerequisites](#prerequisites)
  - [Setup](#setup)
- [Usage](#usage)
  - [Project Structure](#project-structure)
  - [Actions](#actions)
- [Appendix](#appendix)
  - [MikTex](#miktex)
  - [Results](#results)
- [References](#references)

# TODO

- [ ] Implement *create* action for automating the creation of a new lab.
- [ ] Support real-time collaborative editing between Emacs and Visual Studio Code.
- [ ] Support other scripting languages (like Lua or Julia)
- [ ] Support flexible Python actions (for running multiple .py files like needed for *oscillations*)
- [ ] Update vscode actions with C

# Introduction

This repository contains all the Physics and Computer Science labs and assignments that I and/or my lab partners performed in the University of Groningen. The labs are:
* Mechanics and Relativity (Artur Topal + Tyn Rendering)
    * oscillations
    * gyroscope
* Electricity and Magnetism (Maartje + Artur Topal)
    * capacitors
* Algorithms and Data Structures in C (Artur Topal + Lucas)
    * planes
	* equations
* Assignments (Artur Topal)
    * linear-algebra
	* calculus
	* computational-methods-1

# Guide

**Note:** I am using Linux/Emacs so you might need to tweak your system somewhat for it to work properly on Windows/MacOS.

## Prerequisites
1. Install [Visual Studio Code](https://code.visualstudio.com/).
2. Install [MikTex](https://miktex.org/download). (LaTeX compiler) (see below for further configuration)
3. Install [GNU Makefile](https://sourceforge.net/projects/gnuwin32/files/make/3.81/make-3.81.exe/download?use_mirror=altushost-swe&download=) (build system)
4. Install [Git](https://git-scm.com/downloads/win). (select *64-bit Git for Windows Setup*)
5. Follow [this manual](https://docs.github.com/en/authentication/connecting-to-github-with-ssh/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent?platform=windows) to setup git ssh.

On Windows, step 3 also requires the addition of *make* to PATH. To do this, follow the [MikTex](#miktex) appendix. Step 5 is tedious but important. This allows you to setup authentication for all git remote repositories. On Windows (not on MacOS/Unix/Linux), after each git push/pull (discussed later), you have to enter your username and passphrase to push the changes on the server.

## Setup

<img src="meta/vscode.png" alt="Visual Studio Code Setup" width="400" />

1. Download the archive with the project under "*Code/Download Zip*" using one of the links below:
   - Tyn: [main](https://github.com/euwaka/labs)
   - Maartje: [electromagnetism](https://github.com/euwaka/labs/tree/electromagnetism)
   - Joren: [test](https://github.com/euwaka/labs/tree/test)
2. Unzip it in your preferred path.
3. Open it in Visual Studio Code. You can configure Visual Studio Code to your liking (an example is shown on the image above).
4. If extensions were not installed automatically, manually install:
    * [LaTeX](https://marketplace.visualstudio.com/items?itemName=mathematic.vscode-latex)
    * [PDF Viewer](https://marketplace.visualstudio.com/items?itemName=mathematic.vscode-pdf)
    * [Task Explorer](https://marketplace.visualstudio.com/items?itemName=spmeesseman.vscode-taskexplorer)
5. You are ready to go.

**Note**: on the first LaTeX compilation, *MikTex* will prompt you to install many LaTeX packages that are used in LaTeX source files. This is done only once, and there is a tick (see the image below) on the prompt which allows the installation of all the required packages at once.

<img src="meta/MikTex.png" alt="Avoid MikTex" width="400" />

# Usage

1. Open a file that you work on (Python script, LaTeX source, etc.).
2. Make changes.
3. Click one of the actions under *Task Explorer* in the Explorer Menu. (see [Actions](#actions) for more informations on *Task Explorer*)

## Project Structure

1. **labs**: the root directory contains separate directories for different labs (e.g., oscillations, gyroscope, etc.). Every file (.py, .jpeg, .tex, .csv, etc.) associated with a specific project is located under the directory with the name of the project.
2. **meta files**:
    * *makefile*: this is a build system that we use for Emacs/Visual Studio Code
    * *.vscode*: those are configuraitons for Visual Studio Code.
    * *.gitignore*: here are listed files/directories that will not be uploaded to Github (like .pdf)
	* *meta/*: images for the README.md file are here
3. **Project files (the same in every lab directory)**:
    * *main.tex*: this is the main file, which eventually gets compiled (via means of *makefile*) to the required .pdf in build/<project-name>/main.pdf
    * *sections/\*.tex*: those are sections (aka chapters) that are included in the *main.tex*, like **Theory** (theory.tex), **Preparatory Exercises** (prep_asnwers.tex), **Methods** (methods.tex), etc.
    * *images*, *data*: contains *.csv* data files, and images.
    * *scripts*: contains Python script files. (e.g., used for data analysis of .csv data files, or simulations)

## Actions

The only actions that can be clicked are located under the *vscode* label. Other labels like *python* or *make* can be removed by the right click and then "add to excludes list."

<img src="meta/TaskExp.png" alt="Task Explorer" width="400" />

1. *clean*: removes **build/** and all other unnecessary junk.
2. *<project-name>* (e.g., gyroscope or capacitors): builds the .pdf file from *<project-name>/main.tex* file.
3. *<project-name>/scripts* (e.g., gyroscope/scripts): runs *<project-name>/scripts/main.py* Python script file.
4. *git/pull*: pulls the most recent changed from Github.
5. *git/push*: updates Github with your changes, and prompts the commit comment. **NOTE**: Write the commit comment in the prompt!

# Appendix

## MikTex

1. Type *PATH* in the search bar. Then, open *Edit the system environment variables*

<img src="meta/OpenPATH.png" alt="Search bar" width="300" />

2. In the appeared window, click *Environment Variables...*

<img src="meta/ClickEnv.png" alt="Environment Variables..." width="300" />

3. Then, double-click the *Path* like shown in the image below

<img src="meta/DoubleClick.png" alt="Double-click Path" width="300" />

4. In the new window, click *New*

<img src="meta/ClickNew.png" alt="Click New" width="300" />

5. In the prompt, enter this **exact** path: *C:\\Program Files (x86)\\GnuWin32\\bin* and click *OK* twice like shown in the image below

<img src="meta/ClickOK.png" alt="Enter PATH to the makefile executable" width="300">

6. Done.

## Results

| Subject  | A1     | A2     | A3     | A4     |
| ---      | ---    | ---    | ---    | ---    |
| Calculus | Week 3 | Week 4 | Week 5 | Week 7 |
| ---      | ---    | ---    | ---    | ---    |
| Grade    | 9.5    | 9.0    | 9.5    | 9.3    |
| M&R      | oscillations | gyroscope | - | - |
| ---      | ---    | ---    | ---    | ---    |
| Grade    | 10 | 10 | - | - |

# References

1. [LaTeX Docs](https://www.latex-project.org/help/documentation/)
2. [Git Docs](https://git-scm.com/docs)


