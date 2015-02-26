// stdio.c -- Brad Slayter

#include "lib/stdio.h"
#include "framebuffer.h"

void setColorForLevel(int level) {
	switch (level) {
		case K_ERROR:
			monSetColor(BG_BLACK, FG_RED);
			monPuts("ERROR");
			break;
		case K_WARN:
			monSetColor(BG_BLACK, FG_MAGENTA);
			monPuts("WARN");
			break;
		case K_OK:
			monSetColor(BG_BLACK, FG_GREEN);
			monPuts("OK");
			break;
		case K_INFO:
			monSetColor(BG_BLACK, FG_CYAN);
			monPuts("INFO");
			break;
		case K_DEBUG:
			monSetColor(BG_BLACK, FG_MAGENTA);
			monPuts("DEBUG");
		default:
			monSetColor(BG_BLACK, FG_WHITE);
	}
}

void kprintf(int level, const char *format, ...) {
	if (level == K_DEBUG && !DEBUG)
		return;

	if (level != K_NONE) {
		monSetColor(BG_BLACK, FG_WHITE); // print level
		monPut('[');
		setColorForLevel(level);
		monSetColor(BG_BLACK, FG_WHITE);
		monPuts("] ");
	}

	int i = 0;
	va_list list;
	va_start(list, format);
	uint8_t varNext = 0;
	while (format[i]) {
		if (format[i] == '%') {
			varNext = 1;
		} else if (varNext == 1) {
			switch (format[i]) {
				case 'd':
					monPutDec(va_arg(list, int));
					break;
				case 'x':
					monPutHex(va_arg(list, uint32_t));
					break;
				case 's':
					monPuts(va_arg(list, char*));
					break;
				case 'c':
					monPut(va_arg(list, int));
					break;
				default:
					break;
			}
			varNext = 0;
		} else {
			monPut(format[i]);
		}

		i++;
	}
}

/*char getch() {
	int key = KEY_UNKNOWN;

	while (key == KEY_UNKNOWN) {
		key = kb_get_last_key();
	}

	kb_discard_last_key();

	return kb_key_to_ascii(key);
}*/