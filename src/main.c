#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "utils.h"
#include "input.h"
#include "game.h"
#include "menu.h"
#include "settings.h"
#include "level.h"

//cena a ser carregada de primeira
//o padrão é MENU, mas se precisar só mudar
#define FSCENE MENU

bool loadFonts() {
	//o número na direita indica o tamanho da fonte.
	//como ele depende do tamanho da janela, a fonte é numerada
	//de acordo com a razão da altura da janela x1000.
	// (7/135)*1000 = 51.851... ~= 52
	// (1/15)*1000 = 66.6... ~= 67
	LOADFONT(data.font_Regular52,game.height*7/135,Ubuntu-R.ttf);
	LOADFONT(data.font_Regular37,game.height*5/135,Ubuntu-R.ttf);
	LOADFONT(data.font_Bold67,game.height/15,Ubuntu-B.ttf);
	return true;
}

void unloadFonts() {
	UNLOADFONT(data.font_Regular52);
	UNLOADFONT(data.font_Regular37);
	UNLOADFONT(data.font_Bold67);
}

bool load() {
	LOADBITMAP(data.bitmap_keys,keys.png);
	LOADBITMAP(data.bitmap_parallax1,parallax1.png);
	LOADBITMAP(data.bitmap_playerMenu,playerMenu.png);
	LOADSAMPLE(data.sample_bgm,bgm.ogg);
	LOADSAMPLE(data.sample_moan,moan.ogg);
	LOADSAMPLE(data.sample_select,select.wav);
	LOADSAMPLE(data.sample_select2,select2.wav);
	return true;
}

void unload() {
	UNLOADBITMAP(data.bitmap_keys);
	UNLOADBITMAP(data.bitmap_parallax1);
	UNLOADBITMAP(data.bitmap_playerMenu);
}

bool start() {
	srand(time(NULL));
	al_set_window_title(game.display,"A.I.-Sama");
	al_clear_to_color(al_map_rgb(0,0,0));
	al_flip_display();
	
	//inicia o input
	inputStart();
	
	//seleciona a cena inicial. se retornar false, deu algum erro ao carregar
	if (!sceneSelect(FSCENE)) {
		return false;
	}
	scene.tempo = -1;
	scene.exitRequest = false;
	
	return true;
}

bool update() {
	//verifica as transições
	bool sceneLoaded = false;
	if (scene.tempo > 0) {
		scene.tempo -= game.delta*2.5;
		if (scene.tempo <= 0) {
			scene.tempo--;
			//se retornar false, a janela será fechada
			if (scene.exitRequest || !sceneForceLoad(scene.nextScene)) {
				return false;
			}
			sceneLoaded = true;
			al_clear_to_color(al_map_rgb(0,0,0));
			al_flip_display();
		}
	} else if (scene.tempo < 0) {
		scene.tempo += game.delta*2.5;
		if (scene.tempo > 0) scene.tempo = 0;
	}
	
	//no caso de nenhuma cena nova ter sido chamada
	if (!sceneLoaded) {
		//updates da cena
		(*scene.update)();
		(*scene.draw)();
		
		//letterbox
		//dá pra botar uns gráficos bonitinhos no lugar de faixas pretas chatas
		if (scene.showLetterbox) {
			if (game.offsetx > 0) {
				al_draw_filled_rectangle(0,0,game.offsetx,game.height,al_map_rgb(0,0,0));
				al_draw_filled_rectangle(game.offsetx+game.width,0,game.fwidth,game.fheight,al_map_rgb(0,0,0));
			} else if (game.offsety > 0) {
				al_draw_filled_rectangle(0,0,game.width,game.offsety,al_map_rgb(0,0,0));
				al_draw_filled_rectangle(0,game.offsety+game.height,game.fwidth,game.fheight,al_map_rgb(0,0,0));
			}
		}
		
		//efeito de fade in/out
		if (scene.tempo > 0) {
			al_draw_filled_rectangle(0,0,game.fwidth,game.fheight,al_map_rgba_f(0,0,0,ease((1-scene.tempo)*1.125)));
		} else if (scene.tempo < 0) {
			al_draw_filled_rectangle(0,0,game.fwidth,game.fheight,al_map_rgba_f(0,0,0,ease(-scene.tempo*1.125)));
		}

		al_flip_display();
	}
	
	//atualiza o input
	inputUpdate();
	
	return true;
}

void calculateWindowSize() {
	float prop = (float)game.fwidth/game.fheight;
	if (prop > game.idealProp+.01) {
		game.width = (int)round(game.fheight*game.idealProp);
		game.height = game.fheight;
		game.offsetx = (game.fwidth-game.width)/2;
		game.offsety = 0;
	} else if (prop < game.idealProp-.01) {
		game.width = game.fwidth;
		game.height = (int)round(game.fwidth/game.idealProp);
		game.offsetx = 0;
		game.offsety = (game.fheight-game.height)/2;
	} else {
		game.width = game.fwidth;
		game.height = game.fheight;
		game.offsetx = game.offsety = 0;
	}
	printf("janela: %dx%d (%dx%d)\n",game.fwidth,game.fheight,game.width,game.height);
}

void getArgs(int argc,char **args) {
	//analisa os argumentos recebidos.
	//-w diz a largura da janela;
	//-h diz a altura;
	//-s diz a largura x altura.
	bool isWidth;
	int a,b,m,n;
	for (a = 0; a < argc; a++) {
		if (args[a][0] != '-') continue;
		if ((args[a][1] == 'w' || args[a][1] == 'W') && args[a][2] == '=') {
			n = 0;
			for (b = 3; args[a][b] != '\0'; b++) {
				if (args[a][b] < '0' || args[a][b] > '9') {
					n = 0;
					break;
				}
				n = (n*10)+(args[a][b]-'0');
			}
			if (n > 0) {
				game.fwidth = n;
			}
		} else if ((args[a][1] == 'h' || args[a][1] == 'H') && args[a][2] == '=') {
			n = 0;
			for (b = 3; args[a][b] != '\0'; b++) {
				if (args[a][b] < '0' || args[a][b] > '9') {
					n = 0;
					break;
				}
				n = (n*10)+(args[a][b]-'0');
			}
			if (n > 0) {
				game.fheight = n;
			}
		} else if ((args[a][1] == 's' || args[a][1] == 'S') && args[a][2] == '=') {
			m = 0;
			n = 0;
			isWidth = true;
			for (b = 3; args[a][b] != '\0'; b++) {
				if (args[a][b] < '0' || args[a][b] > '9') {
					if (isWidth && (args[a][b] == 'x' || args[a][b] == 'X')) {
						isWidth = false;
						continue;
					}
					m = n = 0;
					break;
				}
				if (isWidth) {
					m = (m*10)+(args[a][b]-'0');
				} else {
					n = (n*10)+(args[a][b]-'0');
				}
			}
			if (m > 0 && n > 0) {
				game.fwidth = m;
				game.fheight = n;
			}
		}
	}
}

int main(int argc,char **args) {
	game.fwidth = game.fheight = 0;
	getArgs(argc,args);
	
	//cálculo inicial da resolução, e outras coisas
	game.idealProp = 16.0/9.0;
	if (game.fwidth <= 0) {
		if (game.fheight <= 0) {
			game.fwidth = 1024;
			game.fheight = 576;
		} else {
			game.fwidth = (int)round(game.fheight*game.idealProp);
		}
	} else if (game.fheight <= 0) {
		game.fheight = (int)round(game.fwidth/game.idealProp);
	}
	al_set_new_display_flags(ALLEGRO_WINDOWED);
	al_set_new_display_flags(ALLEGRO_RESIZABLE);
	al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS,1,ALLEGRO_SUGGEST);
	al_set_new_display_option(ALLEGRO_SAMPLES,4,ALLEGRO_SUGGEST);
	calculateWindowSize();
	
	//timer
	game.fps = 60;
	game.delta = 1.0/game.fps;
	
	//configurações
	game.volumeBgm = 1;
	game.volumeSfx = 1;
	game.showPopups = true;
	
	//inicia tudo q o allegro precisa pra iniciar
	if (!al_init()) {
		fprintf(stderr,"erro: o allegro não pôde ser inicializado\n");
		return -1;
	}
	if (!al_install_mouse()) {
		fprintf(stderr,"erro: o mouse não pôde ser inicializado\n");
		return -1;
	}
	if (!al_install_keyboard()) {
		fprintf(stderr,"erro: o teclado não pôde ser inicializado\n");
		return -1;
	}
	if (!al_install_audio()) {
		fprintf(stderr,"erro: o áudio não pôde ser inicializado\n");
		return -1;
	}
	if (!al_reserve_samples(16)) {
		fprintf(stderr,"erro: o áudio não pôde ser configurado\n");
		return -1;
	}
	if (!al_init_primitives_addon()) {
		fprintf(stderr,"erro: o addon primitives não pôde ser inicializado\n");
		return -1;
	}
	if (!al_init_image_addon()) {
		fprintf(stderr,"erro: o addon image não pôde ser inicializado\n");
		return -1;
	}
	if (!al_init_acodec_addon()) {
		fprintf(stderr,"erro: o addon acodec não pôde ser inicializado\n");
		return -1;
	}
	al_init_font_addon();
	if (!al_init_ttf_addon()) {
		fprintf(stderr,"erro: o addon ttf não pôde ser inicializado\n");
		return -1;
	}
	game.timer = al_create_timer(game.delta);
	if (!game.timer) {
		fprintf(stderr,"erro: o timer não pôde ser criado\n");
		return -1;
	}
	game.display = al_create_display(game.fwidth,game.fheight);
	if (!game.display) {
		fprintf(stderr,"erro: o display não pôde ser criado\n");
		al_destroy_timer(game.timer);
		return -1;
	}
	game.eventQueue = al_create_event_queue();
	if (!game.eventQueue) {
		fprintf(stderr,"erro: a fila de eventos não pôde ser criada\n");
		al_destroy_timer(game.timer);
		al_destroy_display(game.display);
		return -1;
	}
	al_register_event_source(game.eventQueue,al_get_display_event_source(game.display));
	al_register_event_source(game.eventQueue,al_get_timer_event_source(game.timer));
	al_register_event_source(game.eventQueue,al_get_mouse_event_source());
	al_register_event_source(game.eventQueue,al_get_keyboard_event_source());
	
	//início do programa
	game.path = al_get_standard_path(ALLEGRO_EXENAME_PATH);
	if (!load() || !loadFonts() || !start()) {
		unload();
		unloadFonts();
		al_destroy_timer(game.timer);
		al_destroy_display(game.display);
		al_destroy_event_queue(game.eventQueue);
		return -1;
	}
	
	//update
	al_start_timer(game.timer);
	bool upd = true;
	while (1) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(game.eventQueue,&ev);
		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			exitGame();
		} else if (ev.type == ALLEGRO_EVENT_TIMER) {
			upd = true;
		} else if (ev.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
			al_acknowledge_resize(game.display);
			game.fwidth = al_get_display_width(game.display);
			game.fheight = al_get_display_height(game.display);
			int lastHeight = game.height;
			calculateWindowSize();
			if (lastHeight != game.height) {
				unloadFonts();
				if (!loadFonts()) {
					break;
				}
			}
		} else if (ev.type == ALLEGRO_EVENT_KEY_DOWN
				|| ev.type == ALLEGRO_EVENT_KEY_UP
				|| ev.type == ALLEGRO_EVENT_KEY_CHAR) {
			if (input.captureText && ev.type == ALLEGRO_EVENT_KEY_CHAR) {
				inputKeyChar(ev);
			} else {
				inputKeyPress(ev);
			}
		}
		if (upd && al_is_event_queue_empty(game.eventQueue)) {
			upd = false;
			if (!update()) {
				break;
			}
		}
	}
	
	//fim do programa
	(*scene.unload)();
	unload();
	unloadFonts();
	al_uninstall_audio();
	al_destroy_timer(game.timer);
	al_destroy_display(game.display);
	al_destroy_event_queue(game.eventQueue);
	return 0;
}