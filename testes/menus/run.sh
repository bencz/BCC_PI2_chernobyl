#!/bin/bash
echo "--"
gcc main.c \
game.c \
menu.c \
settings.c \
level.c \
utils.c \
ParserExpressao.c \
-o bin/game `pkg-config --libs allegro-5.0 allegro_acodec-5.0 allegro_audio-5.0 allegro_color-5.0 allegro_dialog-5.0 allegro_font-5.0 allegro_image-5.0 allegro_main-5.0 allegro_memfile-5.0 allegro_physfs-5.0 allegro_primitives-5.0 allegro_ttf-5.0` -std=c99 -lm
./bin/game
echo "--"