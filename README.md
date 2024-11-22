# Mechanics-Lab: Oscillations and Gyroscope

**Note:** I am using Linux/Emacs so you might need to tweak your system somehow for it work properly. However, it does work on my system so you should not have any problems.

# Prerequisites
1. Install [Visual Studio Code](https://code.visualstudio.com/).
2. Install [MikTex](https://miktex.org/download). (LaTeX compiler)
3. Install [Git](https://git-scm.com/downloads/win). (select *64-bit Git for Windows Setup*)

# Setup the project

1. Download the archive with the project [here](https://github.com/euwaka/Mechanics-Lab).
2. Unzip it.
3. Open it in Visual Studio Code.
4. In Visual Studio Code, install this extension: **LaTeX Workshop**
5. Press *Ctrl+Shift+P* and enter *Preferences: Open Keyboard Shortcuts (JSON)*
6. In between the **[]**, paste this code:
```json
{
    "key": "ctrl+alt+c",
    "command": "workbench.action.tasks.runTask",
    "args": "Clean-up"
}
```
7. Save the file.

# Usage

## Project Structure

1. meta files:
    * *makefile*: this is configuration for Linux/Emacs.
    * *.vscode*: those are configurations for Visual Studio Code.
    * *.gitignore*: here are listed files that will not be uploaded to Github (like .pdf, since it is not necessary)

2. project files:
    * *oscillations/*: this contains all LaTeX documents, *.csv* data, and images/graphs for **Oscillations Lab 1**.
    * *gyroscope/*: this contains all LaTeX documents, *.csv* data, and images/graphs for **Gyroscope Lab 2**.

3. LaTeX files (in *oscillations/* directory):
    * *main.tex*: this is main file, which eventually gets compiled to the required .pdf (**IMPORTANT**: Add your surname and S-number to the *\author* field.)
    * *sections/\*.tex*: those are sections (aka chapters) that are included in the *main.tex*, like **Theory**, **Preparatory Exercises**, **Experiment Setup**, etc.
    * *images*, *data*: contains *.csv* data files, and images.

## LaTeX in Visual Studio

After you have installed everything, this is what you do:

1. Make changes to .tex files.
2. Press *Ctrl-Shift-B* to compile LaTeX. In *oscillations/build/*, you get desired pdf.
3. Press *Ctrl-Alt-C* to clean-up. (remove build/ since it is unnecessary and bulky to commit to GitHub) This step is not necessary because build/ folder is listed under *.gitignore*, so it will not be pushed to Github anyways.
4. When working with **LaTeX Workshop** in visual studio code, you get auto-completions, math symbols gallery, and a whole lot of other useful stuff.

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