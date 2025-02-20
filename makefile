# Basic usage
# $ make MODE=latex NAME=calculus NUMBER=5
# $ make MODE=latex NAME=gyroscope
# 
# $ make MODE=scripts NAME=calculus NUMBER=3
# $ make MODE=scripts NAME=gyroscope
#
# $ make MODE=c SUBMODE=build NAME=planes
# $ make MODE=c SUBMODE=run NAME=planes
# $ make MODE=c SUBMODE=example NUMBER=1 NAME=planes
# $ make MODE=c SUBMODE=debug NUMBER=1 NAME=planes
#
# $ make clean
#
# $ make venv
# $ make jup

.RECIPEPREFIX=>

# ===================================================================
# Define global variables and constants. TODO: Create ALL function
# ===================================================================
NAMES := "calculus" "computational-methods-1" "gyroscope" "linear-algebra" "oscillations" "planes" "equations"
PACKAGES := numpy matplotlib scikit-learn jupyter seaborn

MODE    ?= latex
SUBMODE ?= build
NAME    ?= $(word 1, $(NAMES))
NUMBER  ?= -1

# ===================================================================
# Define compilers and interpreters.
# ===================================================================
TEX := pdflatex
PY  := python3
BIB := biber
CC  := gcc

CFLAGS := -g -O2 -std=c99 -pedantic -Wall -Wno-unused-result 
CLIBS  := -lm

# ===================================================================
# Define OS-dependent variables and functions.
# ===================================================================

# UNIX/Linux (default)

define remove-directory
@rm -rf $(1)
endef

define make-directory
@mkdir -p $(1)
endef

# Windows
ifeq ($(OS), Windows_NT)

override define remove-directory
@rmdir /s /q
endef

override define make-directory
@powershell -Command "if (-Not (Test-Path -Path $(1))) {New-Item -ItemType Directory -Path $(1)}"
endef

endif

# ===================================================================
# Phony and default targets
# ===================================================================

.PHONY: all

all: $(MODE)

# ===================================================================
# Define targets for each $(MODE).
# ===================================================================

latex:
ifeq ($(NUMBER), -1)
> @echo "Compiling LaTeX for $(NAME)..."
> $(call make-directory, build/$(NAME)/)
> @$(TEX) --output-directory=build/$(NAME)/ $(NAME)/main.tex
> @$(TEX) --output-directory=build/$(NAME)/ $(NAME)/main.tex
> @if [ -f $(NAME)/references.bib ]; then \
>     $(BIB) build/$(NAME)/main; \
>     $(TEX) --output-directory=build/$(NAME)/ $(NAME)/main.tex; \
> fi
else
> @echo "Compiling LaTeX for $(NAME) and week $(NUMBER)..."
> $(call make-directory, build/$(NAME)/W$(NUMBER))
> @$(TEX) --output-directory=build/$(NAME)/W$(NUMBER)/ $(NAME)/W$(NUMBER)/main.tex
> @$(TEX) --output-directory=build/$(NAME)/W$(NUMBER)/ $(NAME)/W$(NUMBER)/main.tex
> @if [ -f $(NAME)/W$(NUMBER)/references.bib ]; then \
>     $(BIB) build/$(NAME)/W$(NUMBER)/main; \
>     $(TEX) --output-directory=build/$(NAME)/W$(NUMBER)/ $(NAME)/W$(NUMBER)/main.tex; \
> fi
endif

scripts:
ifeq ($(NUMBER), -1)
> @echo "Running Python script(s) for $(NAME)..."
> @$(PY) $(NAME)/scripts/main.py
else
> @echo "Running Python script(s) for $(NAME) and week $(NUMBER)..."
> @$(PY) $(NAME)/W$(NUMBER)/scripts/main.py
endif

c:
ifeq ($(SUBMODE), build)
> @echo "Building C source files for $(NAME)..."
> $(call make-directory, build/$(NAME)/)
> @$(CC) $(CFLAGS) $(wildcard $(NAME)/src/*.c) -o build/$(NAME)/$(NAME).o $(CLIBS)

else ifeq ($(SUBMODE), run)
> @echo "Running build/$(NAME)/$(NAME).o..."
> @./build/$(NAME)/$(NAME).o

else ifeq ($(SUBMODE), example)
ifeq ($(NUMBER), -1)
> @echo "Running build/$(NAME)/$(NAME).o with $(NAME)/data/exampleInput.txt..."
> @cat $(NAME)/data/exampleInput.txt | ./build/$(NAME)/$(NAME).o 
else
> @echo "Running build/$(NAME)/$(NAME).o with $(NAME)/data/exampleInput$(NUMBER).txt..."
> @cat $(NAME)/data/exampleInput$(NUMBER).txt | ./build/$(NAME)/$(NAME).o
endif

else ifeq ($(SUBMODE), valgrind)
ifeq ($(NUMBER), -1)
> @echo "Debugging build/$(NAME)/$(NAME).o with $(NAME)/data/exampleInput.txt..."
> @cat $(NAME)/data/exampleInput.txt | valgrind ./build/$(NAME)/$(NAME).o 
else
> @echo "Debugging build/$(NAME)/$(NAME).o with $(NAME)/data/exampleInput$(NUMBER).txt..."
> @cat $(NAME)/data/exampleInput$(NUMBER).txt | valgrind ./build/$(NAME)/$(NAME).o
endif

else ifeq ($(SUBMODE), debug-build)
> @echo "Building with build/$(NAME)/$(NAME).o with debug information..."
> $(call make-directory, build/$(NAME)/)
> @$(CC) $(CFLAGS) -g $(wildcard $(NAME)/src/*.c) -o build/$(NAME)/$(NAME).o $(CLIBS)

endif

# ===================================================================
# clean, venv 
# ===================================================================

clean:
> @echo "Cleaning unnecessary files..."
> $(call remove-directory, build/)
> $(call remove-directory, venv/)
> $(call remove-directory, oscillations/auto/)
> $(call remove-directory, oscillations/sections/auto)
> $(call remove-directory, gyroscope/auto/)
> $(call remove-directory, gyroscope/sections/auto)

venv:
> @echo "Making a Python virtual environment and installing necessary Python packages..."
> @$(PY) -m venv venv
> @echo "Upgrading pip..."
> @venv/bin/pip install --upgrade pip
> @echo "Installing necessary Python packages..."
> @for pkg in $(PACKAGES); do \
> echo "Installing $$pkg..."; \
> venv/bin/pip install $$pkg; \
> done

jup:
> @echo "Running jupyter notebooks as an external process..."
> venv/bin/jupyter lab
