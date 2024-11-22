LATEX=pdflatex

all: oscillations/build/oscillations.pdf

oscillations/build/oscillations.pdf: oscillations/main.tex
	mkdir -p oscillations/build/
	$(LATEX) -output-directory=oscillations/build/ oscillations/main.tex
	$(LATEX) -output-directory=oscillations/build/ oscillations/main.tex

clean:
	rm -rf oscillations/build/
