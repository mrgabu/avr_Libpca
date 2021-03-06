#include <dev_pcd8544.h>
#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>

#include <avr/pgmspace.h>


/**
 * @brief global lcd context for stdout
 */
static struct dev_pcd8544_ctx *_g_stdout_lcd = NULL;


/**
 * @brief default 5x7 font
 */
static const uint8_t _font[][5] PROGMEM = {
	{0x00, 0x00, 0x00, 0x00, 0x00} // 20  
	,{0x00, 0x00, 0x5f, 0x00, 0x00} // 21 !
	,{0x00, 0x07, 0x00, 0x07, 0x00} // 22 "
	,{0x14, 0x7f, 0x14, 0x7f, 0x14} // 23 #
	,{0x24, 0x2a, 0x7f, 0x2a, 0x12} // 24 $
	,{0x23, 0x13, 0x08, 0x64, 0x62} // 25 %
	,{0x36, 0x49, 0x55, 0x22, 0x50} // 26 &
	,{0x00, 0x05, 0x03, 0x00, 0x00} // 27 '
	,{0x00, 0x1c, 0x22, 0x41, 0x00} // 28 (
	,{0x00, 0x41, 0x22, 0x1c, 0x00} // 29 )
	,{0x14, 0x08, 0x3e, 0x08, 0x14} // 2a *
	,{0x08, 0x08, 0x3e, 0x08, 0x08} // 2b +
	,{0x00, 0x50, 0x30, 0x00, 0x00} // 2c ,
	,{0x08, 0x08, 0x08, 0x08, 0x08} // 2d -
	,{0x00, 0x60, 0x60, 0x00, 0x00} // 2e .
	,{0x20, 0x10, 0x08, 0x04, 0x02} // 2f /
	,{0x3e, 0x51, 0x49, 0x45, 0x3e} // 30 0
	,{0x00, 0x42, 0x7f, 0x40, 0x00} // 31 1
	,{0x42, 0x61, 0x51, 0x49, 0x46} // 32 2
	,{0x21, 0x41, 0x45, 0x4b, 0x31} // 33 3
	,{0x18, 0x14, 0x12, 0x7f, 0x10} // 34 4
	,{0x27, 0x45, 0x45, 0x45, 0x39} // 35 5
	,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 36 6
	,{0x01, 0x71, 0x09, 0x05, 0x03} // 37 7
	,{0x36, 0x49, 0x49, 0x49, 0x36} // 38 8
	,{0x06, 0x49, 0x49, 0x29, 0x1e} // 39 9
	,{0x00, 0x36, 0x36, 0x00, 0x00} // 3a :
	,{0x00, 0x56, 0x36, 0x00, 0x00} // 3b ;
	,{0x08, 0x14, 0x22, 0x41, 0x00} // 3c <
	,{0x14, 0x14, 0x14, 0x14, 0x14} // 3d =
	,{0x00, 0x41, 0x22, 0x14, 0x08} // 3e >
	,{0x02, 0x01, 0x51, 0x09, 0x06} // 3f ?
	,{0x32, 0x49, 0x79, 0x41, 0x3e} // 40 @
	,{0x7e, 0x11, 0x11, 0x11, 0x7e} // 41 A
	,{0x7f, 0x49, 0x49, 0x49, 0x36} // 42 B
	,{0x3e, 0x41, 0x41, 0x41, 0x22} // 43 C
	,{0x7f, 0x41, 0x41, 0x22, 0x1c} // 44 D
	,{0x7f, 0x49, 0x49, 0x49, 0x41} // 45 E
	,{0x7f, 0x09, 0x09, 0x09, 0x01} // 46 F
	,{0x3e, 0x41, 0x49, 0x49, 0x7a} // 47 G
	,{0x7f, 0x08, 0x08, 0x08, 0x7f} // 48 H
	,{0x00, 0x41, 0x7f, 0x41, 0x00} // 49 I
	,{0x20, 0x40, 0x41, 0x3f, 0x01} // 4a J
	,{0x7f, 0x08, 0x14, 0x22, 0x41} // 4b K
	,{0x7f, 0x40, 0x40, 0x40, 0x40} // 4c L
	,{0x7f, 0x02, 0x0c, 0x02, 0x7f} // 4d M
	,{0x7f, 0x04, 0x08, 0x10, 0x7f} // 4e N
	,{0x3e, 0x41, 0x41, 0x41, 0x3e} // 4f O
	,{0x7f, 0x09, 0x09, 0x09, 0x06} // 50 P
	,{0x3e, 0x41, 0x51, 0x21, 0x5e} // 51 Q
	,{0x7f, 0x09, 0x19, 0x29, 0x46} // 52 R
	,{0x46, 0x49, 0x49, 0x49, 0x31} // 53 S
	,{0x01, 0x01, 0x7f, 0x01, 0x01} // 54 T
	,{0x3f, 0x40, 0x40, 0x40, 0x3f} // 55 U
	,{0x1f, 0x20, 0x40, 0x20, 0x1f} // 56 V
	,{0x3f, 0x40, 0x38, 0x40, 0x3f} // 57 W
	,{0x63, 0x14, 0x08, 0x14, 0x63} // 58 X
	,{0x07, 0x08, 0x70, 0x08, 0x07} // 59 Y
	,{0x61, 0x51, 0x49, 0x45, 0x43} // 5a Z
	,{0x00, 0x7f, 0x41, 0x41, 0x00} // 5b [
	,{0x02, 0x04, 0x08, 0x10, 0x20} // 5c ¥
	,{0x00, 0x41, 0x41, 0x7f, 0x00} // 5d ]
	,{0x04, 0x02, 0x01, 0x02, 0x04} // 5e ^
	,{0x40, 0x40, 0x40, 0x40, 0x40} // 5f _
	,{0x00, 0x01, 0x02, 0x04, 0x00} // 60 `
	,{0x20, 0x54, 0x54, 0x54, 0x78} // 61 a
	,{0x7f, 0x48, 0x44, 0x44, 0x38} // 62 b
	,{0x38, 0x44, 0x44, 0x44, 0x20} // 63 c
	,{0x38, 0x44, 0x44, 0x48, 0x7f} // 64 d
	,{0x38, 0x54, 0x54, 0x54, 0x18} // 65 e
	,{0x08, 0x7e, 0x09, 0x01, 0x02} // 66 f
	,{0x0c, 0x52, 0x52, 0x52, 0x3e} // 67 g
	,{0x7f, 0x08, 0x04, 0x04, 0x78} // 68 h
	,{0x00, 0x44, 0x7d, 0x40, 0x00} // 69 i
	,{0x20, 0x40, 0x44, 0x3d, 0x00} // 6a j 
	,{0x7f, 0x10, 0x28, 0x44, 0x00} // 6b k
	,{0x00, 0x41, 0x7f, 0x40, 0x00} // 6c l
	,{0x7c, 0x04, 0x18, 0x04, 0x78} // 6d m
	,{0x7c, 0x08, 0x04, 0x04, 0x78} // 6e n
	,{0x38, 0x44, 0x44, 0x44, 0x38} // 6f o
	,{0x7c, 0x14, 0x14, 0x14, 0x08} // 70 p
	,{0x08, 0x14, 0x14, 0x18, 0x7c} // 71 q
	,{0x7c, 0x08, 0x04, 0x04, 0x08} // 72 r
	,{0x48, 0x54, 0x54, 0x54, 0x20} // 73 s
	,{0x04, 0x3f, 0x44, 0x40, 0x20} // 74 t
	,{0x3c, 0x40, 0x40, 0x20, 0x7c} // 75 u
	,{0x1c, 0x20, 0x40, 0x20, 0x1c} // 76 v
	,{0x3c, 0x40, 0x30, 0x40, 0x3c} // 77 w
	,{0x44, 0x28, 0x10, 0x28, 0x44} // 78 x
	,{0x0c, 0x50, 0x50, 0x50, 0x3c} // 79 y
	,{0x44, 0x64, 0x54, 0x4c, 0x44} // 7a z
	,{0x00, 0x08, 0x36, 0x41, 0x00} // 7b {
	,{0x00, 0x00, 0x7f, 0x00, 0x00} // 7c |
	,{0x00, 0x41, 0x36, 0x08, 0x00} // 7d }
	,{0x10, 0x08, 0x08, 0x10, 0x08} // 7e ←
	,{0x00, 0x06, 0x09, 0x09, 0x06} // 7f →
};



static int _pcd8544_putc(char c, FILE *stream) {
	pcd8544_putc(_g_stdout_lcd, c);
	return 0;
}


static void _pcd8544_fill(struct dev_pcd8544_ctx *a_disp, uint8_t *data, uint16_t len) {
	if (len > 1) {
		pcd8544_gotoxy(a_disp, 0, 0);
	}

	for (uint8_t y = 0; y < (PCD8544_H >> 3); y++) {
		pcd8544_write(a_disp, PCD8544_CMD, PCD8544_CMD_SET_X(0));
		pcd8544_write(a_disp, PCD8544_CMD, PCD8544_CMD_SET_Y(y));

		for (uint8_t x = 0; x < PCD8544_W; x++) {
			pcd8544_write(a_disp, PCD8544_DATA, *data);
			if (len > 1) {
				data++;
			}
		}
	}
} 


void pcd8544_write(struct dev_pcd8544_ctx *a_disp, 
		uint8_t mode, 
		uint8_t data) {

	if (PCD8544_DATA == mode) {
		GPIO_SET_HIGH(&a_disp->dc);
	}
	else {
		GPIO_SET_LOW(&a_disp->dc);
	}

	GPIO_SET_LOW(&a_disp->sce);
	
	if (a_disp->bus->f_sendc) 
		a_disp->bus->f_sendc(a_disp->bus->priv, &data);

	GPIO_SET_HIGH(&a_disp->sce);
}


void pcd8544_init(struct dev_pcd8544_ctx *a_disp) {

	uint8_t settings[] = {
		PCD8544_CMD_FUNCTION_SET(0, 0, 1),
		// for KamodLCD1 this command is probably not BIAS
		// but something else. If set to any different value
		// you will encounter an vertical offset of 4 pixels
		PCD8544_CMD_TMP_CTRL(0x01),
		PCD8544_CMD_BIAS(0x04),
		PCD8544_CMD_SET_VOP(0x40),
		PCD8544_CMD_FUNCTION_SET(0, 0, 0),
		PCD8544_CMD_DISPLAY_CTRL(E_DISPLAY_NORMAL),
	};

	uint8_t cnt = 0;

	GPIO_CONFIGURE_AS_OUTPUT(&a_disp->res);
	GPIO_CONFIGURE_AS_OUTPUT(&a_disp->dc);
	GPIO_CONFIGURE_AS_OUTPUT(&a_disp->sce);

	GPIO_SET_LOW(&a_disp->res);
	_delay_ms(10);
	GPIO_SET_HIGH(&a_disp->res);

	for (cnt = 0; cnt < sizeof(settings); cnt++) {
		pcd8544_write(a_disp, PCD8544_CMD, settings[cnt]);
		_delay_ms(1);
	}
}


void pcd8544_clrscr(struct dev_pcd8544_ctx *a_disp) {
	pcd8544_fill(a_disp, 0);
	pcd8544_gotoxy(a_disp, 0, 0);
}


void pcd8544_gotoxy(struct dev_pcd8544_ctx *a_disp, uint8_t x, uint8_t y) {
	a_disp->x = x;
	a_disp->y = y;
	pcd8544_write(a_disp, PCD8544_CMD, PCD8544_CMD_SET_X(x));
	pcd8544_write(a_disp, PCD8544_CMD, PCD8544_CMD_SET_Y(y));
}


void pcd8544_putc(struct dev_pcd8544_ctx *a_disp, char c) {
	uint8_t font_length = sizeof(_font[0]);
	
	if (a_disp->y >= (PCD8544_H >> 3)) {
		return;
	}

	if ((a_disp->x + font_length + 1) > PCD8544_W || (c == '\n')) {
		pcd8544_gotoxy(a_disp, 0, a_disp->y + 1);
	}
	
	pcd8544_write(a_disp, PCD8544_DATA, 0x00);
	for (uint8_t x = 0; x < font_length; x++ ) {
		pcd8544_write(a_disp, PCD8544_DATA, pgm_read_byte(&_font[c - 0x20][x]));
	}

	a_disp->x += (font_length + 1);
}


void pcd8544_str(struct dev_pcd8544_ctx *a_disp, char *str) {
	if (!str) {
		return;
	}

	while (str) {
		pcd8544_putc(a_disp, *str++);
	}
}


void pcd8544_install_stdout(struct dev_pcd8544_ctx *a_disp) {
	static FILE lcd_stdout = FDEV_SETUP_STREAM(_pcd8544_putc, NULL, _FDEV_SETUP_WRITE);
	_g_stdout_lcd = a_disp;
	stdout = &lcd_stdout;
}


void pcd8544_putblock(struct dev_pcd8544_ctx *a_disp, uint8_t x, uint8_t y, uint8_t c) {
	pcd8544_gotoxy(a_disp, x, y);
	pcd8544_write(a_disp, PCD8544_DATA, c);
}


void pcd8544_blit(struct dev_pcd8544_ctx *a_disp, uint8_t *data) {
	_pcd8544_fill(a_disp, data, PCD8544_W * PCD8544_H);
}


void pcd8544_fill(struct dev_pcd8544_ctx *a_disp, uint8_t data) {
	_pcd8544_fill(a_disp, &data, 1);
}

