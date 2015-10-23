IDIR =.
CC=gcc -std=c99
CFLAGS=-I$(IDIR) -g
#CFLAGS=-I$(IDIR) -W -Wall -g

ODIR=obj
LIBS=-lallegro -lallegro_audio -lallegro_acodec -lallegro_image -lallegro_font -lallegro_main -lallegro_ttf -lallegro_primitives -lm

_OBJ = game.o level.o menu.o settings.o input.o main.o parserexpressao.o utils.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

OUTPUTDIR = ../bin
OUTPUTNAME = bcc_chernobyl

$(ODIR)/%.o: %.c
	@$(call make_dir)
	$(CC) -c -o $@ $< $(CFLAGS)

$(OUTPUTDIR)/$(OUTPUTNAME): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
	$(info *************************************** COMPILADO COM SUCESSO ******************************************************)

all: $(OUTPUTDIR)/game

define make_dir
	@mkdir -p $(ODIR)
	for dir in $(SRCDIRS); \
	do \
		mkdir -p $(ODIR)/$$dir; \
	done
endef

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ allegro.log *~ $(OUTPUTDIR)/$(OUTPUTNAME)
