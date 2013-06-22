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

#ifndef FLIPDOT_H
#define FLIPDOT_H

#include <stdint.h>

#ifndef F_CPU
#define F_CPU 16000000
#endif

enum sreg {
	ROW,
	COL
};

#define DATA_COL (1<<PC6) //output 7
#define DATA_ROW (1<<PC0) //output 1
#define STROBE   (1<<PC1) //output 2

#define OE_WHITE (1<<PC2) //output 3
#define OE_BLACK (1<<PC3) //output 4

#define CLK_COL  (1<<PC4) //output 5
#define CLK_ROW  (1<<PC5) //output 6

#define CLK_DELAY  1			/* us */
#define FLIP_DELAY 2			/* ms */
#define STROBE_DELAY 1			/* us */
//#define LINE_DELAY 2			/* ms */

#define DISP_COLS   24 + 20
#define MODULE_COLS 20
#define DISP_ROWS   16

#define ROW_GAP 4

void flipdot_init(void);
void flipdot_data(uint8_t *frames, uint16_t size);

#endif /* FLIPDOT_H */
