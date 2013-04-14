PDFFILES = report.pdf

all: $(PDFFILES)

report.pdf: report.tex
	pdflatex $^
	pdflatex $^

clean:
	rm -f *.aux *.pdf *.toc
