#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

const float FPS = 60;
bool repeat = true;
bool redraw = true;
enum {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT};
bool key[4] = {false, false, false, false};

typedef struct
{
	float x;
	float y;
}v2;

typedef struct
{
	int sizeX;
	int sizeY;

	v2 pos;

	v2 maxspeed;
	float accell;
	v2 speed;

	float top, down, left, right;
}entity;

void collision(entity *a, entity *b)
{
	if((a->down > b->top && (a->top < b->top || a->top < b->down)) &&
		 (a->right > b->left && a->left < b->right))
	{
  	a->speed.y = 0;
	}

	if((a->right > b->left && (a->left < b->left || a->left < b->right)) &&
		 (a->down > b->top && a->top < b->down))
	{
		a->speed.x = 0;
	}
}

int main()
{
	entity player;

	player.sizeX = 32;
	player.sizeY = 64;
	player.pos.x = 100;
	player.pos.y = 100;
	player.maxspeed.x = 10.0f, player.maxspeed.y = 10.0f;
	player.speed.x = 0.0f, player.speed.y = 0.0f;
	player.accell = 2.0f;

	entity block;

	block.sizeX = 32;
	block.sizeY = 32;
	block.pos.x = 500;
	block.pos.y = 300;

	block.top = block.pos.y;
	block.down = block.sizeY + block.pos.y;
	block.left = block.pos.x;
	block.right = block.sizeX + block.pos.x;

	ALLEGRO_COLOR player_color;
	ALLEGRO_COLOR block_color;
	//ALLEGRO_COLOR debugblock_color;

	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;

	//Initialization

	if(!al_init())
	{
		printf("Ta iniciando sabosta naum!\n");
		return -1;
	}

	display = al_create_display(800, 600);

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

	//update player top, down, left and right

	while(repeat)
	{
		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);

		if(event.type == ALLEGRO_EVENT_TIMER)
		{

			if(key[KEY_UP])
			{
				if(player.speed.y > 0)
					player.speed.y = 0;
				else if(player.speed.y > -player.maxspeed.y)
			  	player.speed.y -= player.accell;
				else if(player.speed.y < -player.maxspeed.y)
					player.speed.y = -player.maxspeed.y;
				//printf("up!\n");
			}

			if(key[KEY_DOWN])
			{
				if(player.speed.y < 0)
					player.speed.y = 0;
				else if(player.speed.y < player.maxspeed.y)
			  	player.speed.y += player.accell;
				else if(player.speed.y > player.maxspeed.y)
					player.speed.y = player.maxspeed.y;
				//printf("down!\n");
			}

			else
			{
				if(player.speed.y > 0)
				{
					player.speed.y -= 0.5f;
					if (player.speed.y < 0)
						player.speed.y = 0;
				}
				else if(player.speed.y < 0)
				{
					player.speed.y += 0.5f;
					if (player.speed.y > 0)
						player.speed.y = 0;
				}
			}

			if(key[KEY_LEFT])
			{
				if(player.speed.x > 0)
					player.speed.x = 0;
				else if(player.speed.x > -player.maxspeed.x)
			  	player.speed.x -= player.accell;
				else if(player.speed.x < -player.maxspeed.x)
					player.speed.x = -player.maxspeed.x;
				//printf("left!\n");
			}

			if(key[KEY_RIGHT])
			{
				if(player.speed.x < 0)
					player.speed.x = 0;
				else if(player.speed.x < player.maxspeed.x)
			  	player.speed.x += player.accell;
				else if(player.speed.x > player.maxspeed.x)
					player.speed.x = player.maxspeed.x;
				//printf("right!\n");
			}

			else
			{
				if(player.speed.x > 0)
				{
					player.speed.x -= 0.5f;
					if (player.speed.x < 0)
						player.speed.x = 0;
				}
				else if(player.speed.x < 0)
				{
					player.speed.x += 0.5f;
					if (player.speed.x > 0)
						player.speed.x = 0;
				}
			}

			player.top = player.pos.y + player.speed.y;
			player.down = player.sizeY + player.pos.y + player.speed.y;
			player.left = player.pos.x + player.speed.x;
			player.right = player.sizeX + player.pos.x + player.speed.x;

			collision(&player, &block);

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
              key[KEY_UP] = true;
              break;

           case ALLEGRO_KEY_DOWN:
              key[KEY_DOWN] = true;
              break;

           case ALLEGRO_KEY_LEFT:
              key[KEY_LEFT] = true;
              break;

           case ALLEGRO_KEY_RIGHT:
              key[KEY_RIGHT] = true;
              break;
        }
     }

     else if(event.type == ALLEGRO_EVENT_KEY_UP)
		 {
        switch(event.keyboard.keycode)
				{
           case ALLEGRO_KEY_UP:
              key[KEY_UP] = false;
							break;

           case ALLEGRO_KEY_DOWN:
              key[KEY_DOWN] = false;
              break;

           case ALLEGRO_KEY_LEFT:
              key[KEY_LEFT] = false;
              break;

           case ALLEGRO_KEY_RIGHT:
              key[KEY_RIGHT] = false;
              break;

           case ALLEGRO_KEY_ESCAPE:
              repeat = false;
              break;
      	}
			}

			if(redraw && al_is_event_queue_empty(event_queue))
			{
				redraw = false;
				al_clear_to_color(al_map_rgb(0,0,0));

				//debugblock_color = al_map_rgb(255, 0, 0);
				// if(player.speed.x > 0)
				// 	al_draw_rectangle(player.pos.x, player.pos.y, player.pos.x + player.sizeX + player.speed.x,
				// 										player.pos.y + player.sizeY + player.speed.y, debugblock_color, 1);
				// else if(player.speed.x < 0)
				// 	al_draw_rectangle(player.pos.x + player.speed.x, player.pos.y + player.speed.y, player.pos.x + player.sizeX,
				// 										player.pos.y + player.sizeY, debugblock_color, 1);

				player_color = al_map_rgb(255, 0, 255);
				al_draw_rectangle(player.pos.x, player.pos.y, player.pos.x + player.sizeX,
													player.pos.y + player.sizeY, player_color, 1);

				block_color = al_map_rgb(255, 255, 255);
				al_draw_rectangle(block.pos.x, block.pos.y, block.pos.x + block.sizeX,
													block.pos.y + block.sizeY, block_color, 1);

				al_flip_display();
			}
		}

  al_destroy_timer(timer);
  al_destroy_display(display);
  al_destroy_event_queue(event_queue);

	printf("Hurray!! Deu tudo certo nessa PORRAAA!!\n");

	return 0;
}
