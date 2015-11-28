#include <stdbool.h>

#include <allegro5/allegro.h>

#include "game.h"
#include "input.h"

CharType getCharType(char c) {
	if (c >= '0' && c <= '9') return CHAR_NUMBER;
	if (c >= 'A' && c <= 'Z') return CHAR_ALPHA;
	if (c >= 'a' && c <= 'z') return CHAR_ALPHA;
	if (c == '.' || c == ',' || c == '/' || c == '*' || c == '(' ||
		c == ')' || c == '+' || c == '-' || c == '^' || c == '=') return CHAR_TOKEN;
	if (c == ' ') return CHAR_SPACE;
	return CHAR_INVALID;
}

key *initKey() {
	key *k = (key*)malloc(sizeof(key));
	k->press = k->hold = k->release = k->repeat = false;
	return k;
}

void resetKey(key *k) {
	k->press = k->release = k->repeat = false;
}

void inputStart() {
	input.up = initKey();
	input.down = initKey();
	input.left = initKey();
	input.right = initKey();
	input.enter = initKey();
	input.backspace = initKey();
	input.space = initKey();
	input.tab = initKey();
	input.escape = initKey();
	
	input.inactivity = 0;
	input.text[0] = '\0';
	input.captureText = false;
	input.captureFinish = false;
	input.textUpdate = false;
	input.caretPos = 0;
	input.caretBlink = 0;
	input.selectionStart = -1;
}

void inputUpdate() {
	resetKey(input.up);
	resetKey(input.down);
	resetKey(input.left);
	resetKey(input.right);
	resetKey(input.enter);
	resetKey(input.backspace);
	resetKey(input.space);
	resetKey(input.tab);
	resetKey(input.escape);
	
	input.inactivity += game.delta;
	input.captureFinish = false;
	input.textUpdate = false;
	input.caretBlink += game.delta*1.5;
	if (input.caretBlink >= 1) {
		input.caretBlink--;
	}
}

void inputKeyPress(ALLEGRO_EVENT ev) {
	key* k;
	if (input.captureText) {
		if (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_ENTER) {
			input.captureText = false;
			input.captureFinish = true;
			input.enter->press = true;
			input.enter->hold = true;
			return;
		}
		switch (ev.keyboard.keycode) {
			case ALLEGRO_KEY_UP: k = input.up; break;
			case ALLEGRO_KEY_DOWN: k = input.down; break;
			case ALLEGRO_KEY_TAB: k = input.tab; break;
			case ALLEGRO_KEY_ESCAPE: k = input.escape; break;
			default: return;
		}
	} else {
		switch (ev.keyboard.keycode) {
			case ALLEGRO_KEY_UP: k = input.up; break;
			case ALLEGRO_KEY_DOWN: k = input.down; break;
			case ALLEGRO_KEY_LEFT: k = input.left; break;
			case ALLEGRO_KEY_RIGHT: k = input.right; break;
			case ALLEGRO_KEY_ENTER: k = input.enter; break;
			case ALLEGRO_KEY_BACKSPACE: k = input.backspace; break;
			case ALLEGRO_KEY_SPACE: k = input.space; break;
			case ALLEGRO_KEY_TAB: k = input.tab; break;
			case ALLEGRO_KEY_ESCAPE: k = input.escape; break;
			default: return;
		}
	}
	input.inactivity = 0;
	if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
		k->press = true;
		k->hold = true;
	} else if (ev.type == ALLEGRO_EVENT_KEY_CHAR) {
		k->repeat = true;
	} else {
		k->release = true;
		k->hold = false;
	}
}

void inputKeyChar(ALLEGRO_EVENT ev) {
	input.inactivity = 0;
	input.caretBlink = 0;
	int len = strlen(input.text);
	if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT) {
		//aperta para a esquerda
		int prev = input.caretPos;
		if (input.caretPos > 0) {
			if (ev.keyboard.modifiers & ALLEGRO_KEYMOD_CTRL) {
				CharType ct = getCharType(input.text[input.caretPos-1]);
				do {
					input.caretPos--;
				} while (input.caretPos > 0 && getCharType(input.text[input.caretPos-1]) == ct);
			} else {
				input.caretPos--;
			}
		}
		if (ev.keyboard.modifiers & ALLEGRO_KEYMOD_SHIFT) {
			if (prev == input.caretPos) {
			} else if (input.selectionStart == -1) {
				input.selectionStart = input.caretPos;
				input.selectionEnd = prev;
			} else if (prev > input.selectionStart && input.caretPos <= input.selectionStart) {
				input.selectionEnd = input.selectionStart;
				input.selectionStart = input.caretPos;
			} else if (input.caretPos >= input.selectionStart) {
				input.selectionEnd = input.caretPos;
			} else {
				input.selectionStart = input.caretPos;
			}
			if (input.selectionStart == input.selectionEnd) {
				input.selectionStart = -1;
			}
		} else {
			input.selectionStart = -1;
		}
	} else if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
		//aperta para a direita
		int prev = input.caretPos;
		if (input.caretPos < len) {
			if (ev.keyboard.modifiers & ALLEGRO_KEYMOD_CTRL) {
				CharType ct = getCharType(input.text[input.caretPos]);
				do {
					input.caretPos++;
				} while (input.caretPos < len && getCharType(input.text[input.caretPos]) == ct);
			} else {
				input.caretPos++;
			}
		}
		if (ev.keyboard.modifiers & ALLEGRO_KEYMOD_SHIFT) {
			if (prev == input.caretPos) {
			} else if (input.selectionStart == -1) {
				input.selectionStart = prev;
				input.selectionEnd = input.caretPos;
			} else if (prev < input.selectionEnd && input.caretPos >= input.selectionEnd) {
				input.selectionStart = input.selectionEnd;
				input.selectionEnd = input.caretPos;
			} else if (input.caretPos <= input.selectionEnd) {
				input.selectionStart = input.caretPos;
			} else {
				input.selectionEnd = input.caretPos;
			}
			if (input.selectionStart == input.selectionEnd) {
				input.selectionStart = -1;
			}
		} else {
			input.selectionStart = -1;
		}
	} else if (ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE) {
		//apaga com backspace
		if (input.selectionStart == -1 && (ev.keyboard.modifiers & ALLEGRO_KEYMOD_CTRL) && input.caretPos > 0) {
			input.selectionStart = input.selectionEnd = input.caretPos;
			CharType ct = getCharType(input.text[input.selectionStart-1]);
			do {
				input.selectionStart--;
			} while (input.caretPos > 0 && getCharType(input.text[input.selectionStart-1]) == ct);
		}
		if (input.selectionStart != -1) {
			input.caretPos = input.selectionStart;
			do {
				input.text[input.selectionStart] = input.text[input.selectionEnd];
				input.selectionStart++;
				input.selectionEnd++;
			} while (input.text[input.selectionEnd-1] != '\0');
			input.selectionStart = -1;
			input.textUpdate = true;
		} else if (input.caretPos > 0) {
			input.caretPos--;
			for (int a = input.caretPos; input.text[a] != '\0'; a++) {
				input.text[a] = input.text[a+1];
			}
			input.textUpdate = true;
		}
	} else if (ev.keyboard.keycode == ALLEGRO_KEY_DELETE) {
		//apaga com delete
		if (input.selectionStart == -1 && (ev.keyboard.modifiers & ALLEGRO_KEYMOD_CTRL) && input.caretPos < len) {
			input.selectionStart = input.selectionEnd = input.caretPos;
			CharType ct = getCharType(input.text[input.selectionEnd]);
			do {
				input.selectionEnd++;
			} while (input.caretPos < len && getCharType(input.text[input.selectionEnd]) == ct);
		}
		if (input.selectionStart != -1) {
			input.caretPos = input.selectionStart;
			do {
				input.text[input.selectionStart] = input.text[input.selectionEnd];
				input.selectionStart++;
				input.selectionEnd++;
			} while (input.text[input.selectionEnd-1] != '\0');
			input.selectionStart = -1;
			input.textUpdate = true;
		} else if (input.caretPos < len) {
			for (int a = input.caretPos; input.text[a] != '\0'; a++) {
				input.text[a] = input.text[a+1];
			}
			input.textUpdate = true;
		}
	} else if (ev.keyboard.keycode == ALLEGRO_KEY_A && (ev.keyboard.modifiers & ALLEGRO_KEYMOD_CTRL)) {
		//ctrl+a
		if (len > 0) {
			input.selectionStart = 0;
			input.selectionEnd = input.caretPos = len;
		}
	} else if (len < 1023) {
		//digita alguma coisa
		char c = ev.keyboard.unichar;
		if (getCharType(c) != CHAR_INVALID) {
			if (input.selectionStart != -1) {
				input.caretPos = input.selectionStart;
				do {
					input.text[input.selectionStart] = input.text[input.selectionEnd];
					input.selectionStart++;
					input.selectionEnd++;
				} while (input.text[input.selectionEnd-1] != '\0');
				input.selectionStart = -1;
			}
			if (c == ',') c = '.';
			if (c == '=') c = '+';
			for (int a = len; a > input.caretPos; a--) {
				input.text[a] = input.text[a-1];
			}
			input.text[input.caretPos] = c;
			input.text[len+1] = '\0';
			input.caretPos++;
			input.textUpdate = true;
		}
	}
}