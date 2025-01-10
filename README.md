# TODO

- [ ] Implement *create* action for automating the creation of a new lab.
- [ ] Support cross-live-editting between Emacs and Visual Studio Code.
- [ ] Support other scripting languages.
- [ ] Support flexible Python actions (for running mulible .py files like needed for *oscillations*)  

# Introduction

This repository contains all the labs that we performed during the fresheman year of physics in the University of Groningen. The labs are:
* Mechanics and Relativity
    * oscillations
    * gyroscope

# Guide (for Visual Studio Code)

**Note:** I am using Linux/Emacs so you might need to tweak your system somehow for it to work properly on Windows. However, it does work on my system so you should not have any problems.

## Prerequisites
1. Install [Visual Studio Code](https://code.visualstudio.com/).
2. Install [MikTex](https://miktex.org/download). (LaTeX compiler)
3. Install [Git](https://git-scm.com/downloads/win). (select *64-bit Git for Windows Setup*)
4. Follow [this manual](https://docs.github.com/en/authentication/connecting-to-github-with-ssh/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent?platform=windows)

Step 4 is somewhat tedious but crucial. This is like auth for git remote repositories. You will use the combination of the your github name and passphrase to push your changes to Github.

## Setup the project

1. Download the archive with the project [here](https://github.com/euwaka/labs).
2. Unzip it.
3. Open it in Visual Studio Code.
4. If extensions were not installed automatically, manually install:
    * [LaTeX](https://marketplace.visualstudio.com/items?itemName=mathematic.vscode-latex)
    * [PDF Viewer](https://marketplace.visualstudio.com/items?itemName=mathematic.vscode-pdf)
    * [Task Explorer](https://marketplace.visualstudio.com/items?itemName=spmeesseman.vscode-taskexplorer)
5. You are ready to go.

# Usage

1. Open a file that you work on (script, LaTeX, etc.).
2. Make changes.
3. Click one of the actions under *Task Explorer* in the Explorer Menu. (see later for more information on actions)

## Project Structure

1. **labs**: the root directory contains separate directories for different labs (e.g., oscillations, gyroscope, etc.). Every file (.py, .jpeg, .tex, .csv, etc.) associated with a specific project is located under the directory with the name of the project.
2. **meta files**:
    * *makefile*: this is a build system that we use for Emacs/Visual Studio Code
    * *.vscode*: those are configuraitons for Visual Studio Code.
    * *.gitignore*: here are listed files that will not be uploaded to Github (like .pdf)
3. **Project files (the same in every lab directory)**:
    * *main.tex*: this is main file, which eventually gets compiled (via means of *makefile*) to the required .pdf in build/<project-name>/main.pdf
    * *sections/\*.tex*: those are sections (aka chapters) that are included in the *main.tex*, like **Theory**, **Preparatory Exercises**, **Experiment Setup**, etc.
    * *images*, *data*: contains *.csv* data files, and images.
    * *scripts*: contains Python script files. (e.g., used for data analysis of .csv data files)

## Action (under Task Explorer)

1. *clean*: removes **build/** and all other unnecessary junk.
2. *<project-name>* (e.g., gyroscope): builds pdf from *<project-name>/main.tex* file.
3. *<project-name>/scripts* (e.g., gyroscope/scripts): runs *<project-name>/scripts/main.py* script file.

# Short manual

## Git

### Description
Git is a version control system. It allows users:
1. Push files to the Github portal.
2. Fetch commits (previous versions of files).
3. Collaboration: *we can work on the project simultaneously, and as long as there are no conflicts, we can easily update Github repo.*
4. Branches: *we can work in different branches so both of us will have their own version of the project. Then, we can merge branches together to get our final project*

### Useful commands

1. Use this command to download the project to your local machine.
```bash
git clone <repository-url>
```

2. Always run this command before making any changes to ensure your local files are up-to-date with mine.
```bash
git pull origin main
```

3. After making changes to the project, you to stage them before a commit and push.
```bash
cd <your root path to Mechanics-Lab>
git add .
```

4. Once files are staged, create a commit to save the changes with a useful short message.
```bash
git commit -m "Useful short message"
```

5. Push changes to the remote repository on Github.
```bash
git push origin main
```

6. So, this is a common workflow:
```bash
cd <path to Machine-Lab>
git add .
git commit -m "Message"
git push origin main
```

**Note:** You can also automate Git commands in Visual Studio Code, but it will be an unnecessary complication. Furthermore, it is pretty handy to work with git via terminal or console. Besides, you can perform all above-mentioned commands (and much more) using Visual Studio Code **Source Control** on the right panel (press *Ctrl-Shift-G*).

## Referencees

1. [LaTeX Docs](https://www.latex-project.org/help/documentation/)
2. [Git Docs](https://git-scm.com/docs)