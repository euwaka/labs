# Contents

- [TODO](#todo)
- [Introduction](#introduction)
- [Guide (for Visual Studio Code)](#guide-for-visual-studio-code)
  - [Prerequisites](#prerequisites)
  - [MikTex on Windows](#miktex-on-windows)
  - [Github](#github)
- [Usage](#usage)
  - [Project Structure](#project-structure)
- [Actions](#actions-under-task-explorer)
- [References](#references)

# TODO

- [ ] Implement *create* action for automating the creation of a new lab.
- [ ] Support real-time collaborative editing between Emacs and Visual Studio Code.
- [ ] Support other scripting languages (like Lua or Julia)
- [ ] Support flexible Python actions (for running multiple .py files like needed for *oscillations*)
- [ ] Update vscode actions with C

# Introduction

This repository contains all the Physics and Computer Science labs and assignments that I and/or my lab partners performed in the University of Groningen. The labs are:
* Mechanics and Relativity
    * oscillations
    * gyroscope
* Electricity and Magnetism
    * capacitors
* Algorithms and Data Structures in C
    * planes
	* equations
* Assignments
    * linear-algebra
	* calculus
	* computational-methods-1

# Guide (for Visual Studio Code)

**Note:** I am using Linux/Emacs so you might need to tweak your system somewhat for it to work properly on Windows/MacOS.

## Prerequisites
1. Install [Visual Studio Code](https://code.visualstudio.com/).
2. Install [MikTex](https://miktex.org/download). (LaTeX compiler) (see below for further configuration)
3. Install [GNU Makefile](https://sourceforge.net/projects/gnuwin32/files/make/3.81/make-3.81.exe/download?use_mirror=altushost-swe&download=) (build system)
4. Install [Git](https://git-scm.com/downloads/win). (select *64-bit Git for Windows Setup*)
5. Follow [this manual](https://docs.github.com/en/authentication/connecting-to-github-with-ssh/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent?platform=windows) to setup git ssh.

Stup 3 require one more configuration on Windows - adding the *make* program to PATH. To do this, follow the steps under [MikTex](#miktex-on-windows) 

Step 5 is tedious but crucial. This is like auth for git remote repositories. You will use the combination of your github name and passphrase to push your changes to Github. If you use the Task (aka the button) in Visual Studio Code, you would also need to enter the passphrase in the prompt.

## Setup the project

### MikTex on Windows

1. Type *PATH* in the search bar. Then, open *Edit the system environment variables*
![Search bar](meta/OpenPATH.png)

2. In the appeared window, click *Environment Variables...*
![Environment Variables...](meta/ClickEnv.png)

3. Then, double-click the *Path* like shown in the image below
![Double-click Path](meta/DoubleClick.png)

4. In the new window, click *New*
![Click New](meta/ClickNew.png)

5. In the prompt, enter this **exact** path: *C:\\Program Files (x86)\\GnuWin32\\bin* and click *OK* twice like shown in the image below
![Enter PATH to the makefile executable](meta/ClickOK.png)

6. Done.

### Github 

![Visual Studio Code Setup](meta/vscode.png)

1. Download the archive with the project [here](https://github.com/euwaka/labs/tree/electromagnetism).
2. Unzip it.
3. Open it in Visual Studio Code. You can configure Visual Studio Code to your liking (an example is shown on the image above).
4. If extensions were not installed automatically, manually install:
    * [LaTeX](https://marketplace.visualstudio.com/items?itemName=mathematic.vscode-latex)
    * [PDF Viewer](https://marketplace.visualstudio.com/items?itemName=mathematic.vscode-pdf)
    * [Task Explorer](https://marketplace.visualstudio.com/items?itemName=spmeesseman.vscode-taskexplorer)
5. You are ready to go.

**Note**: on the first LaTeX compilation, *MikTex* will prompt you to install many LaTeX packages that are used in LaTeX source files. This is done only once, and there is a tick (see the image below) on the prompt which allows the installation of all the required packages at once.
![Avoid MikTex](meta/MikTex.png)

# Usage

1. Open a file that you work on (Python script, LaTeX source, etc.).
2. Make changes.
3. Click one of the actions under *Task Explorer* in the Explorer Menu. (see later for more information on actions)

## Project Structure

1. **labs**: the root directory contains separate directories for different labs (e.g., oscillations, gyroscope, etc.). Every file (.py, .jpeg, .tex, .csv, etc.) associated with a specific project is located under the directory with the name of the project.
2. **meta files**:
    * *makefile*: this is a build system that we use for Emacs/Visual Studio Code
    * *.vscode*: those are configuraitons for Visual Studio Code.
    * *.gitignore*: here are listed files/directories that will not be uploaded to Github (like .pdf)
	* *meta/*: images for the README.md file are here
3. **Project files (the same in every lab directory)**:
    * *main.tex*: this is the main file, which eventually gets compiled (via means of *makefile*) to the required .pdf in build/<project-name>/main.pdf
    * *sections/\*.tex*: those are sections (aka chapters) that are included in the *main.tex*, like **Theory** (theory.tex), **Preparatory Exercises** (prep.tex), **Methods** (methods.tex), etc.
    * *images*, *data*: contains *.csv* data files, and images.
    * *scripts*: contains Python script files. (e.g., used for data analysis of .csv data files, or simulations)

## Actions (under Task Explorer)

The only actions that can be clicked are located under the *vscode* label. Other labels like *python* or *make* can be removed by the right click and then "add to excludes list." Besides, Visual Studio Code is highly customizable so you might tweak it to your preference.

![Task Explorer](meta/TaskExp.png)

1. *clean*: removes **build/** and all other unnecessary junk.
2. *<project-name>* (e.g., gyroscope): builds the .pdf file from *<project-name>/main.tex* file.
3. *<project-name>/scripts* (e.g., gyroscope/scripts): runs *<project-name>/scripts/main.py* Python script file.
4. *git/pull*: pulls the most recent changed from Github. **NOTE**: Choose your branch!
5. *git/push*: updates Github with your changes, and prompts the commit comment. **NOTE**: Write the comment in the prompt! Choose your branch!

## References

1. [LaTeX Docs](https://www.latex-project.org/help/documentation/)
2. [Git Docs](https://git-scm.com/docs)
