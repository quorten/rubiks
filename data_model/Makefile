# Make the documentation for the Rubik's cube data model.

# The default output driver is dvipdfm.  This can be changed by
# changing the file "mydriver.tex".

all: data_model.pdf

# The images were designed as SVG in Inkscape, then they were
# converted to EPS with Inkscape.  Ghostscript was used to get version
# 1.2 PDF files (needed for dvipdfm).

GS_DEFS = -dSAFER -q -dBATCH -dNOPAUSE -sDEVICE=pdfwrite -dCompatibilityLevel=1.2

image1.pdf: image1.eps
	gs $(GS_DEFS) -dDEVICEWIDTHPOINTS=120 -dDEVICEHEIGHTPOINTS=120 -sOutputFile=image1.pdf image1.eps

image2.pdf: image2.eps
	gs $(GS_DEFS) -dDEVICEWIDTHPOINTS=176 -dDEVICEHEIGHTPOINTS=136 -sOutputFile=image2.pdf image2.eps

image3.pdf: image3.eps
	gs $(GS_DEFS) -dDEVICEWIDTHPOINTS=340 -dDEVICEHEIGHTPOINTS=90 -sOutputFile=image3.pdf image3.eps

image1.bb: image1.pdf
	ebb image1.pdf

image2.bb: image2.pdf
	ebb image2.pdf

image3.bb: image3.pdf
	ebb image3.pdf

data_model.dvi: data_model.tex mydriver.tex image1.bb image2.bb image3.bb
	latex data_model.tex

data_model.pdf: data_model.dvi
	dvipdfm data_model.dvi

mostlyclean:
	rm -f *.bb data_model.ps data_model.dvi *.log *.aux

clean: mostlyclean
	rm -f data_model.pdf

maintainer-clean: clean
	rm -f image1.pdf image2.pdf image3.pdf

# As clean as it can get
nonsource-clean: maintainer-clean
	rm -f *.pdf *.eps
