NAME=prezentace

$(NAME).pdf: $(NAME).ps
	ps2pdf $(NAME).ps

$(NAME).ps: $(NAME).dvi
	dvips $(NAME).dvi

$(NAME).dvi: $(NAME).tex  
	latex $(NAME).tex

.PHONY: 
	clean pack
clean:
	-rm -f $(NAME).dvi $(NAME).aux $(NAME).toc $(NAME).log $(NAME).out $(NAME).snm $(NAME).nav


