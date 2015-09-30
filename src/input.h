#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

#include <allegro5/allegro.h>

typedef struct key {
	
	bool press;
	bool hold;
	bool release;
	bool repeat;
	
} key;

struct Input {

	key *up;
	key *down;
	key *left;
	key *right;
	key *enter;
	key *backspace;
	
	char text[1024];
	bool captureText;
	bool captureFinish;
	bool textUpdate;
	int caretPos;
	int selectionStart;
	int selectionEnd;
	float caretBlink;

} input;

typedef enum {

	CHAR_INVALID,
	CHAR_NUMBER,
	CHAR_ALPHA,
	CHAR_TOKEN,
	CHAR_SPACE

} CharType;

CharType getCharType(char c);

void inputStart();
void inputUpdate();
void inputKeyPress(ALLEGRO_EVENT ev);
void inputKeyChar(ALLEGRO_EVENT ev);

#endif