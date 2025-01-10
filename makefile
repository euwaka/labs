.RECIPEPREFIX = >

LATEX  = pdflatex
PYTHON = python3

MODE := tex
LAB  := gyroscope

.PHONY: tex, make_tex, scripts, make_scripts, clean

all: $(MODE)

tex:
> $(MAKE) make_tex LAB=$(LAB)

make_tex: $(LAB)/main.tex
> mkdir -p build/$(LAB)
> $(LATEX) -output-directory=build/$(LAB) $(LAB)/main.tex

scripts:
> $(MAKE) make_scripts LAB=$(LAB)

make_scripts: $(LAB)/scripts/main.py
> $(PYTHON) $(LAB)/scripts/main.py

clean:
> rm -rf build/
> rm -rf oscillations/auto/
> rm -rf oscillations/sections/auto
> rm -rf gyroscope/auto/
> rm -rf gyroscope/sections/auto