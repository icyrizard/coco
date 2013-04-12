PDFFILES = report.pdf

all: $(PDFFILES)

report.pdf: report.tex
	pdflatex $^

clean:
	rm -f *.aux *.pdf *.toc
