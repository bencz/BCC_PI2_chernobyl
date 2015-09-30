#include <stdio.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

//constants/defines

#define TILESIZE 32
#define TILEMAP_X 30
#define TILEMAP_Y 17
#define WINDOW_X 960
#define WINDOW_Y 540
#define FPS 60.0f

enum {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT};
bool key[4] = {false, false, false, false};
bool redraw = true;

bool IN_GAME = true;

//tilemap array!
//               1                    8                      16                      24                30
int tilemap[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //1
                 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //8
                 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, //16
                 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,};

//structs

typedef struct{
  float x;
  float y;
}v2;

typedef struct{
  int sizeX;
  int sizeY;

  v2 pos;

  v2 speed;
  
  int topleft, topright, botleft, botright;

}entity;

//collision function
bool collision(entity *a, int tilemap[])
{
  if(tilemap[a->topleft] == 1 || tilemap[a->topright] == 1 || tilemap[a->botleft] == 1 || tilemap[a->botright] == 1)
    return true;
  else
    return false;
}

int main()
{
  entity player;
  player.pos.x = 64;
  player.pos.y = 64;
  player.sizeX = 32;
  player.sizeY = 32;

  int LEFT, RIGHT, TOP, BOT;

  ALLEGRO_COLOR player_color;
  ALLEGRO_COLOR tile_color;

  //DEBUG TILES COLOR
  //ALLEGRO_COLOR player_tile;

  ALLEGRO_DISPLAY *display = NULL;
  ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;

	if(!al_init())
	{
		printf("Ta iniciando sabosta naum!\n");
		return -1;
	}

	display = al_create_display(WINDOW_X, WINDOW_Y);
  al_set_window_position(display, (1920 - WINDOW_X)/2, (1080 - WINDOW_Y)/2);

	if(!display)
	{
		printf("Deu zica pra fazer a bosta do display!\n");
		return -1;
	}

  if(!al_install_keyboard()) {
		 fprintf(stderr, "Num ta catando keyboard nesse caralho!\n");
		 return -1;
	}

	timer = al_create_timer(1.0 / FPS);
	if(!timer) {
		 fprintf(stderr, "Num rolou timer, peidao!\n");
		 return -1;
	}

	event_queue = al_create_event_queue();
	if(!event_queue) {
		 fprintf(stderr, "Num rolou a porra do event queue!!\n");
		 al_destroy_display(display);
		 al_destroy_timer(timer);
		 return -1;
	}

	al_register_event_source(event_queue, al_get_display_event_source(display));

	al_register_event_source(event_queue, al_get_timer_event_source(timer));

	al_register_event_source(event_queue, al_get_keyboard_event_source());

	al_clear_to_color(al_map_rgb(0,0,0));

  al_flip_display();

  al_start_timer(timer);
  while(IN_GAME)
  {
  	ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);

		if(event.type == ALLEGRO_EVENT_TIMER)
		{

			if(key[KEY_UP])
			{
        player.speed.y = -TILESIZE/8;
			}

			else if(key[KEY_DOWN])
			{
        player.speed.y = TILESIZE/8;
			}

			else
			{
        player.speed.y = 0;
			}

			if(key[KEY_LEFT])
			{
        player.speed.x = -TILESIZE/8;
			}

			else if(key[KEY_RIGHT])
			{
        player.speed.x = TILESIZE/8;
			}

			else
			{
        player.speed.x = 0;
			}

      LEFT = (floor(player.pos.x/TILESIZE)*TILESIZE);
      RIGHT = (ceil(player.pos.x/TILESIZE)*TILESIZE);
      TOP = (floor(player.pos.y/TILESIZE)*TILESIZE);
      BOT = (ceil(player.pos.y/TILESIZE)*TILESIZE);

      player.topleft = (LEFT + TOP*TILEMAP_X)/TILESIZE;
      player.topright = (RIGHT + TOP*TILEMAP_X)/TILESIZE;
      player.botleft = (LEFT + BOT*TILEMAP_X)/TILESIZE;
      player.botright = (RIGHT + BOT*TILEMAP_X)/TILESIZE;

      player.pos.x += player.speed.x;
			player.pos.y += player.speed.y;

			redraw = true;

		}

		else if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			break;

		else if(event.type == ALLEGRO_EVENT_KEY_DOWN)
		{
        switch(event.keyboard.keycode)
				{
           case ALLEGRO_KEY_UP:
           {
              key[KEY_UP] = true;
              break;
           }

           case ALLEGRO_KEY_DOWN:
           {
              key[KEY_DOWN] = true;
              break;
           }

           case ALLEGRO_KEY_LEFT:
           {
              key[KEY_LEFT] = true;
              break;
           }

           case ALLEGRO_KEY_RIGHT:
           {
              key[KEY_RIGHT] = true;
              break;
           }
        }
     }

     else if(event.type == ALLEGRO_EVENT_KEY_UP)
     {
        switch(event.keyboard.keycode)
	      {
           case ALLEGRO_KEY_UP:
           {
              key[KEY_UP] = false;
	            break;
           }

           case ALLEGRO_KEY_DOWN:
           {
              key[KEY_DOWN] = false;
              break;
           }

           case ALLEGRO_KEY_LEFT:
           {
              key[KEY_LEFT] = false;
              break;
           }

           case ALLEGRO_KEY_RIGHT:
           {
              key[KEY_RIGHT] = false;
              break;
           }

           case ALLEGRO_KEY_ESCAPE:
           {
              IN_GAME = false;
              break;
           }
      	}
			}


      if(redraw && al_is_event_queue_empty(event_queue))
  		{
  			redraw = false;
        al_clear_to_color(al_map_rgb(0,0,0));

        tile_color = al_map_rgb(255, 255, 255);

        int i, j;
        for(i = 0; i < TILEMAP_Y; i++)
        {
          for(j = 0; j < TILEMAP_X; j++)
          {
            if(tilemap[(i*TILEMAP_X)+j] == 1)
            {
              al_draw_rectangle(j*TILESIZE, i*TILESIZE, (j+1)*TILESIZE,
                               (i+1)*TILESIZE, tile_color, 1);
            }
          }
        }

        // DRAW PLAYER TILES, FOR DEBUG

        // player_tile = al_map_rgb(0, 255, 0);
        //
        // al_draw_rectangle(LEFT, TOP, LEFT + TILESIZE, TOP + TILESIZE, player_tile, 1);
        // al_draw_rectangle(RIGHT, TOP, RIGHT + TILESIZE, TOP + TILESIZE, player_tile, 1);
        // al_draw_rectangle(LEFT, BOT, LEFT + TILESIZE, BOT + TILESIZE, player_tile, 1);
        // al_draw_rectangle(RIGHT, BOT, RIGHT + TILESIZE, BOT + TILESIZE, player_tile, 1);

        if(collision(&player, tilemap))
          player_color = al_map_rgb(255,0,0);
        else
          player_color = al_map_rgb(0,255,0);

        al_draw_rectangle(player.pos.x, player.pos.y, player.pos.x + player.sizeX,
                          player.pos.y + player.sizeY, player_color, 1);

        al_flip_display();
      }
    }


  al_destroy_timer(timer);
  al_destroy_display(display);
  al_destroy_event_queue(event_queue);

	printf("Hurray!! Deu tudo certo nessa PORRAAA!!\n");

  return 0;
}
