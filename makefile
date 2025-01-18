LATEX  = pdflatex
PYTHON = python3
BIB = biber
RM = rm -rf
CREATE_BUILD_DIR = mkdir -p "build/$(LAB)"

ifeq ($(OS), Windows_NT)
	RM := rmdir /s /q
	CREATE_BUILD_DIR := powershell -Command "if (-Not (Test-Path -Path "build\$(LAB)")) {New-Item -ItemType Directory -Path "build\$(LAB)"}"
endif

MODE := tex
LAB  := gyroscope

.PHONY: tex, make_tex, scripts, make_scripts, clean

all: $(MODE)

tex:
	$(MAKE) make_tex LAB=$(LAB)

make_tex: $(LAB)/main.tex
	$(CREATE_BUILD_DIR)
	$(LATEX) -output-directory="build/$(LAB)" "$(LAB)/main.tex"

	$(BIB) build/$(LAB)/main

	$(LATEX) -output-directory="build/$(LAB)" "$(LAB)/main.tex"
	$(LATEX) -output-directory="build/$(LAB)" "$(LAB)/main.tex"

scripts:
	$(MAKE) make_scripts LAB=$(LAB)

make_scripts: 
	$(PYTHON) "$(LAB)/scripts/main.py"

clean:
	$(RM) "build/"
	$(RM) "oscillations/auto/"
	$(RM) "oscillations/sections/auto"
	$(RM) "gyroscope/auto/"
	$(RM) "gyroscope/sections/auto"
