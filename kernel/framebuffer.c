// framebuffer.c -- Brad Slayter

#include "framebuffer.h"
#include "mailbox.h"
#include "teletext.h"

#include "lib/string.h"

/* Mailbox call to get screen resolution failed */
#define FBFAIL_GET_RESOLUTION		1
/* Mailbox call returned bad resolution */
#define FBFAIL_GOT_INVALID_RESOLUTION	2
/* Mailbox call to setup FB failed */
#define FBFAIL_SETUP_FRAMEBUFFER	3
/* Setup call FB returned an invalid list of response tags */
#define FBFAIL_INVALID_TAGS		4
/* Setup FB call returned an invalid response for the framebuffer tag */
#define FBFAIL_INVALID_TAG_RESPONSE	5
/* Setup FB call returned an invalid address/size */
#define FBFAIL_INVALID_TAG_DATA		6
/* Read FB pitch call returned an invalid response */
#define FBFAIL_INVALID_PITCH_RESPONSE	7
/* Read FB pitch call returned an invalid pitch value */
#define FBFAIL_INVALID_PITCH_DATA	8

/* Character cells are 6x10 */
#define CHARSIZE_X	6
#define CHARSIZE_Y	10

/* Screen parameters set in fb_init() */
static unsigned int screenbase, screensize;
static unsigned int fb_x, fb_y, pitch;
/* Max x/y character cell */
static unsigned int max_x, max_y;

static void newline();
void console_write(char *text);

void fb_init() {
	uint32_t var;
	uint32_t count;
	uint32_t physicalScreenbase;

	volatile uint32_t mailbuffer[256] __attribute__((aligned (16)));

	uint32_t physicalMB = (uint32_t)mailbuffer;

	mailbuffer[0] = 8 * 4;
	mailbuffer[1] = 0;
	mailbuffer[2] = 0x40003;
	mailbuffer[3] = 8;
	mailbuffer[4] = 0;
	mailbuffer[5] = 0;
	mailbuffer[6] = 0;
	mailbuffer[7] = 0;

	writeMailbox(8, physicalMB);

	var = readMailbox(8);

	if (mailbuffer[1] != 0x80000000)
		return; // we failed

	fb_x = mailbuffer[5];
	fb_y = mailbuffer[6];

	if (fb_x == 0 || fb_y == 0) {
		fb_x = 640;
		fb_y = 480;
	}

	uint32_t c = 1;
	mailbuffer[c++] = 0;

	mailbuffer[c++] = 0x00048003;	// Tag id (set physical size)
	mailbuffer[c++] = 8;		// Value buffer size (bytes)
	mailbuffer[c++] = 8;		// Req. + value length (bytes)
	mailbuffer[c++] = fb_x;		// Horizontal resolution
	mailbuffer[c++] = fb_y;		// Vertical resolution

	mailbuffer[c++] = 0x00048004;	// Tag id (set virtual size)
	mailbuffer[c++] = 8;		// Value buffer size (bytes)
	mailbuffer[c++] = 8;		// Req. + value length (bytes)
	mailbuffer[c++] = fb_x;		// Horizontal resolution
	mailbuffer[c++] = fb_y;		// Vertical resolution

	mailbuffer[c++] = 0x00048005;	// Tag id (set depth)
	mailbuffer[c++] = 4;		// Value buffer size (bytes)
	mailbuffer[c++] = 4;		// Req. + value length (bytes)
	mailbuffer[c++] = 16;		// 16 bpp

	mailbuffer[c++] = 0x00040001;	// Tag id (allocate framebuffer)
	mailbuffer[c++] = 8;		// Value buffer size (bytes)
	mailbuffer[c++] = 4;		// Req. + value length (bytes)
	mailbuffer[c++] = 16;		// Alignment = 16
	mailbuffer[c++] = 0;		// Space for response

	mailbuffer[c++] = 0;		// Terminating tag

	mailbuffer[0] = c*4;		// Buffer size

	writeMailbox(8, physicalMB);

	var = readMailbox(8);

	if (mailbuffer[1] != 0x80000000)
		return;

	count = 2;
	while ((var = mailbuffer[count])) {
		if (var == 0x40001)
			break;

		count += 3 + (mailbuffer[count+1] >> 2);

		if (count > c)
			return;
	}

	if (mailbuffer[count+2] != 0x80000008)
		return;

	physicalScreenbase = mailbuffer[count+3];
	screensize = mailbuffer[count+4];

	if (physicalScreenbase == 0 || screensize == 0)
		return;

	screenbase = physicalScreenbase;

	/* Get the framebuffer pitch (bytes per line) */
	mailbuffer[0] = 7 * 4;		// Total size
	mailbuffer[1] = 0;		// Request
	mailbuffer[2] = 0x40008;	// Display size
	mailbuffer[3] = 4;		// Buffer size
	mailbuffer[4] = 0;		// Request size
	mailbuffer[5] = 0;		// Space for pitch
	mailbuffer[6] = 0;		// End tag

	writeMailbox(8, physicalMB);

	var = readMailbox(8);

	if (mailbuffer[4] != 0x80000004)
		return;

	pitch = mailbuffer[5];
	if (pitch == 0)
		return;

	max_x = fb_x / CHARSIZE_X;
	max_y = fb_y / CHARSIZE_Y;

	monPuts("Framebuffer initialised. Address = ");
	monPutHex(physicalScreenbase);
	monPuts(" (physical), ");
	monPutHex(screenbase);
	monPuts(" (virtual), \nsize = ");
	monPutHex(screensize);
	monPuts(", resolution = ");
	monPutDec(fb_x);
	monPuts("x");
	monPutDec(fb_y);
	monPut('\n');
}

/* Current console text cursor position (ie. where the next character will
 * be written
*/
static int consx = 0;
static int consy = 0;

/* Current fg/bg colour */
static unsigned short int fgcolour = 0xffff;
static unsigned short int bgcolour = 0;

/* A small stack to allow temporary colour changes in text */
static unsigned int colour_stack[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
static unsigned int colour_sp = 8;

void drawPixel(int x, int y, int val) {
	volatile unsigned short int *ptr;
	unsigned int addr = y*pitch + x*2;
	ptr = (unsigned short int *)(screenbase+addr);
	*ptr = val;
}

void drawChar(int x, int y, char c);
void monPuts(const char *str);
void monPut(char c);

void drawStuff() {
	//for (int i = 0; i < 10; i++)
		//drawPixel(100, 100, 0b0000011111111111);

	drawChar(100, 95, 'a');
	monPuts("Hello, World!\n");
	monPuts("Next line? :D\n");
}

/* Move to a new line, and, if at the bottom of the screen, scroll the
 * framebuffer 1 character row upwards, discarding the top row
 */
static void newline()
{
	unsigned int source;
	/* Number of bytes in a character row */
	register unsigned int rowbytes = CHARSIZE_Y * pitch;

	consx = 0;
	if(consy<(max_y-1))
	{
		consy++;
		return;
	}

	/* Copy a screen's worth of data (minus 1 character row) from the
	 * second row to the first
	 */

	/* Calculate the address to copy the screen data from */
	source = screenbase + rowbytes;
	memmove((void *)screenbase, (void *)source, (max_y-1)*rowbytes);

	/* Clear last line on screen */
	memclr((void *)(screenbase + (max_y-1)*rowbytes), rowbytes);
}

void drawChar(int x, int y, char c) {
	unsigned char ch = c - 32;

	for (int row = 0; row < CHARSIZE_Y; row++) {
		//int col;
		for (int col = CHARSIZE_X-2; col >= 0; col--) {
			if (row < (CHARSIZE_Y-1) && (teletext[ch][row] & (1 << col))) {
				// draw a pixel
				drawPixel(x+(CHARSIZE_X-col), y+row, 0b0000011111111111);
			} else {
				// background
				drawPixel(x+(CHARSIZE_X-col), y+row, 0b0000000000011111);
			}
		}

		drawPixel(x+CHARSIZE_X+1, y+row, 0b0000000000011111);
	}
}

void monPut(char c) {
	if (c == '\n') {
		newline();
		return;
	}

	drawChar(consx*CHARSIZE_X, consy*CHARSIZE_Y, c);

	consx++;
	if (consx >= max_x) {
		newline();
	}
}

void monPuts(const char *str) {
	int i = 0;
	while (str[i])
		monPut(str[i++]);
}

void monPutHex(uint32_t n) {
	static char hex [] = { '0', '1', '2', '3', '4', '5', '6', '7',
                        '8', '9' ,'A', 'B', 'C', 'D', 'E', 'F' };

	char buffer[16];

	int len = 0, k = 0;
	do {
		buffer[len] = hex[n&0xF];
		len++;
		n >>= 4;
	} while (n != 0);

	for (; k < len/2; k++) {
		buffer[k] ^= buffer[len-k-1];
		buffer[len-k-1] ^= buffer[k];
		buffer[k] ^= buffer[len-k-1];
	}

	buffer[len] = '\0';

	char pre[100] = "0x\0";

	strcat(pre, buffer);

	monPuts(pre);
}

void monPutDec(int n) {
	if (n == 0) {
        monPut('0');
        return;
    } else if (n < 0) {
    	monPut('-');
    	n = -n;
    }

    int acc = n;
    char c[32];
    int i = 0;
    while (acc > 0) {
        c[i] = '0' + acc%10;
        acc /= 10;
        i++;
    }
    c[i] = 0;

    char c2[32];
    c2[i--] = 0;
    int j = 0;
    while(i >= 0) {
        c2[i--] = c[j++];
    }
    monPuts(c2);
}

void console_write(char *text)
{
	volatile unsigned short int *ptr;

	unsigned int row, addr;
	int col;
	unsigned char ch;

	/* Double parentheses to silence compiler warnings about
	 * assignments as boolean values
	 */
	while((ch = (unsigned char)*text))
	{
		text++;

		/* Deal with control codes */
		switch(ch)
		{
			case 1: fgcolour = 0b1111100000000000; continue;
			case 2: fgcolour = 0b0000011111100000; continue;
			case 3: fgcolour = 0b0000000000011111; continue;
			case 4: fgcolour = 0b1111111111100000; continue;
			case 5: fgcolour = 0b1111100000011111; continue;
			case 6: fgcolour = 0b0000011111111111; continue;
			case 7: fgcolour = 0b1111111111111111; continue;
			case 8: fgcolour = 0b0000000000000000; continue;
				/* Half brightness */
			case 9: fgcolour = (fgcolour >> 1) & 0b0111101111101111; continue;
			case 10: newline(); continue;
			case 11: /* Colour stack push */
				if(colour_sp)
					colour_sp--;
				colour_stack[colour_sp] =
					fgcolour | (bgcolour<<16);
				continue;
			case 12: /* Colour stack pop */
				fgcolour = colour_stack[colour_sp] & 0xffff;
				bgcolour = colour_stack[colour_sp] >> 16;
				if(colour_sp<8)
					colour_sp++;
				continue;
			case 17: bgcolour = 0b1111100000000000; continue;
			case 18: bgcolour = 0b0000011111100000; continue;
			case 19: bgcolour = 0b0000000000011111; continue;
			case 20: bgcolour = 0b1111111111100000; continue;
			case 21: bgcolour = 0b1111100000011111; continue;
			case 22: bgcolour = 0b0000011111111111; continue;
			case 23: bgcolour = 0b1111111111111111; continue;
			case 24: bgcolour = 0b0000000000000000; continue;
				/* Half brightness */
			case 25: bgcolour = (bgcolour >> 1) & 0b0111101111101111; continue;
		}

		/* Unknown control codes, and anything >127, get turned into
		 * spaces. Anything >=32 <=127 gets 32 subtracted from it to
		 * turn it into a value between 0 and 95, to index into the
		 * character definitions table
		 */
		if(ch<32)
		{
			ch=0;
		}
		else
		{
			if(ch>127)
				ch=0;
			else
				ch-=32;
		}

		/* Plot character onto screen
		 *
		 * CHARSIZE_Y and CHARSIZE_X are the size of the block the
		 * character occupies. The character itself is one pixel
		 * smaller in each direction, and is located in the upper left
		 * of the block
		 */
		for(row=0; row<CHARSIZE_Y; row++)
		{
			addr = (row+consy*CHARSIZE_Y)*pitch + consx*CHARSIZE_X*2;

			for(col=(CHARSIZE_X-2); col>=0; col--)
			{
				ptr = (unsigned short int *)(screenbase+addr);

				addr+=2;

				if(row<(CHARSIZE_Y-1) && (teletext[ch][row] & (1<<col)))
					*ptr = fgcolour;
				else
					*ptr = bgcolour;
			}

			ptr = (unsigned short int *)(screenbase+addr);
			*ptr = bgcolour;
		}

		if(++consx >=max_x)
		{
			newline();
		}
	}
}