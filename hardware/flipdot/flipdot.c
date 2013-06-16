/*
* Copyright (c) 2013 Franz Nord
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 3
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
* For more information on the GPL, please go to:
* http://www.gnu.org/copyleft/gpl.html
*/

#include "flipdot.h"
#include "config.h"

#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>

#define DATA_COL (1<<PC6) //output 7
#define DATA_ROW (1<<PC0) //output 1
#define STROBE   (1<<PC1) //output 2

#define OE_WHITE (1<<PC3) //output 4
#define OE_BLACK (1<<PC2) //output 3

#define CLK_COL  (1<<PC4) //output 5
#define CLK_ROW  (1<<PC5) //output 6

#define CLK_DELAY  2			/* us */
#define FLIP_DELAY 5			/* ms */
#define STROBE_DELAY 2			/* us */
//#define LINE_DELAY 2			/* ms */

#define DISP_COLS   24 + 20
#define DISP_ROWS   16

//#define FRAME_DELAY 0

enum sreg {
	ROW,
	COL
};

#define ISBITSET(x,i) ((x[i>>3] & (1<<(i&7)))!=0)
#define SETBIT(x,i) x[i>>3]|=(1<<(i&7));
#define CLEARBIT(x,i) x[i>>3]&=(1<<(i&7))^0xFF;

#define DATA(reg)								\
	((reg == ROW) ? DATA_ROW : DATA_COL)
#define CLK(reg)								\
	((reg == ROW) ? CLK_ROW : CLK_COL)
#define OE(reg)									\
	((reg == ROW) ? OE_ROW : OE_COL)

void sreg_push_bit(enum sreg reg, uint8_t bit);
void sreg_fill(enum sreg reg, int count, uint8_t *data);

void strobe(void);
void flip_pixels(void);

void display_frame(uint8_t *row_data);


void
flipdot_init(void)
{
	// init ports
	DDRC  |=  (DATA_COL|STROBE|OE_WHITE|OE_BLACK|CLK_ROW|CLK_COL|DATA_ROW);
	PORTC &= ~(DATA_COL|STROBE|OE_WHITE|OE_BLACK|CLK_ROW|CLK_COL|DATA_ROW);

    DDRB |= (1<<PB0);
    PORTB |= (1<<PB0);
}

void
flipdot_data(uint8_t *frames, uint16_t size)
{

    display_frame(frames);
#if 0    
    static uint8_t index;

    if (frames[0]-index != 1 &&
        !(index == 0xff && frames[0] == 0x00)) {
        PORTB ^= (1<<PB0);
    }
    index = frames[0];
    _delay_ms(10);
#endif
}


void write_bit(uint8_t *byte, int pos, uint8_t bit) {
	*(byte + pos/8) |= (bit << pos%8);
}

void display_frame(uint8_t *data) {
	uint8_t row_select[DISP_ROWS/8];
	
	for (int row = 0; row < DISP_ROWS; ++row) { /* Every row */
		uint8_t *row_data = data + row * (DISP_COLS-4)/8; /* -4 should be +4 above */

		for (int i = 0; i < DISP_ROWS/8; ++i) { /* Clear row_select */
			row_select[i] = 0;
		}
		SETBIT(row_select, row); /* Set selected row */
		sreg_fill(COL, DISP_ROWS, row_select); /* Fill row select shift register */
		
		sreg_fill(ROW, DISP_COLS, row_data); /* Fill row data shift register */
		strobe();
		flip_pixels();

		/* _delay_ms(LINE_DELAY); */
	}
}

/* Output bit on reg and pulse clk signal */
void sreg_push_bit(enum sreg reg, uint8_t bit) {
	if (bit) {
		PORTC |= DATA(reg);			/* set data bit */
	} else {
		PORTC &= ~DATA(reg);			/* unset data bit */
	}
	
	PORTC |= CLK(reg); 			/* clk high */
	_delay_us(CLK_DELAY);		/* Wait */
	PORTC &= ~CLK(reg);			/* clk low */
}

/* Fill register reg with count bits from data LSB first */
void sreg_fill(enum sreg reg, int count, uint8_t *data) {
	if (reg == ROW) {
		count -= 4;
	}
	int i = 0;
	int halt_count = 0;
	while (i < count) {
		if (reg == ROW) {
			if (i > 20 && halt_count < 4) {
				++halt_count;
				--i;
			}
		}
		sreg_push_bit(reg, ISBITSET(data, i));
		++i;
	}
}

void strobe(void) {
	PORTC |= STROBE;
	_delay_us(STROBE_DELAY);
	PORTC &= ~STROBE;
}

void flip_pixels(void) {
	// set white pixels
	PORTC |= (OE_WHITE);
	PORTC &= ~(OE_BLACK);

	_delay_ms(FLIP_DELAY);

	// set black pixels
	PORTC |= (OE_BLACK);
	PORTC &= ~(OE_WHITE);
	
	_delay_ms(FLIP_DELAY);

	PORTC &= ~(OE_BLACK);
	PORTC &= ~(OE_WHITE);
}

