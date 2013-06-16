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

#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>

void
flipdot_init(void)
{
    DDRB |= (1<<PB0);
    PORTB |= (1<<PB0);
}

void
flipdot_data(uint8_t *frames, uint16_t size)
{
    static uint8_t index;

    if (frames[0]-index != 1 &&
        !(index == 0xff && frames[0] == 0x00)) {
        PORTB ^= (1<<PB0);
    }
    index = frames[0];
    _delay_ms(10);
}

