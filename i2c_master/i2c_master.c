/* vim:fdm=marker ts=4 et ai
 * {{{
 *
 * Copyright (c) 2008,2009 by Christian Dietrich <stettberger@dokucode.de>
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
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
 }}} */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
        
#include "../config.h"
#include "../debug.h"
#include "i2c_master.h"

#ifdef I2C_MASTER_SUPPORT

void 
i2c_master_init(void)
{
    TWSR = 0;       // Vorteiler  Bitrate = 0
    TWBR = (F_CPU / I2C_MASTER_BAUDRATE - 16) / 2;  // Frequenz setzen
    PORTC |= _BV(PC1) | _BV(PC0); //enable pullup vor i2c
    i2c_master_enable();
}

uint8_t
i2c_master_detect(uint8_t range_start, uint8_t range_end)
{
  uint8_t i;
  uint8_t i2c_address = 0xff;

#ifdef DEBUG_I2C
  debug_printf("I2C: test for base address %x until %x\n", range_start, range_end);
#endif
  for (i = range_start; i < range_end; i++) {
    if (i2c_master_select(i, TW_WRITE)) {
      i2c_address = i;
#ifdef DEBUG_I2C
      debug_printf("I2C: detected at: %X\n", i2c_address);
#endif
      i2c_master_stop();
      break;
    }
    i2c_master_stop();
  }
  return i2c_address;
}

// Wartet bis TWI-Operstion beendet ist
// R�ckgabewert = TWI-Statusbits
uint8_t
i2c_master_do(uint8_t mode)
{
    TWCR = mode;                    // �bertragung starten 
    while(!( TWCR & (1<<TWINT)));   // warten bis Byte �bertragen ist
    return TW_STATUS;           // Returncode = Statusbits 
}

/* Send an i2c stop condition */
void 
i2c_master_stop(void)
{
    TWCR=((1<<TWEN)|(1<<TWINT)|(1<<TWSTO));     // Stopbedingung senden
    while (!(TWCR & (1<<TWSTO)));               // warten bis TWI fertig
}

/* failure, if return == 0 */
uint8_t
i2c_master_select(uint8_t address, uint8_t mode)
{
  #ifdef DEBUG_I2C
    debug_printf("i2c master select adr+mode %X\n", (address << 1) | mode);
  #endif
    /* Start Condition */
    if (i2c_master_do ((1<<TWINT)|(1<<TWSTA)|(1<<TWEN)) != TW_START) return 0; 
    /* address chip */
    TWDR = (address << 1) | mode;
    if (i2c_master_do ((1<<TWINT)|(1<<TWEN)) != TW_MT_SLA_ACK) 
      return 0; 
    return 1; 
}

#endif /* I2C_MASTER_SUPPORT */

