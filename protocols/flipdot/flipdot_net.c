/*                     -*- mode: C; c-file-style: "stroustrup"; -*-
 *
 * Copyright (c) 2013 by Tobias Schneider
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#include "flipdot_net.h"
#include "protocols/uip/uip.h"
#include "protocols/uip/uip_router.h"
#include "hardware/flipdot/flipdot.h"
#include "core/portio/portio.h"

#include "config.h"

#include <util/delay.h>
#include <stdint.h>

static uint8_t flipper[256];

static uint8_t reverse(uint8_t b)
{
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}

void
flipdot_net_init (void)
{
    uip_ipaddr_t ip;
    uip_ipaddr_copy(&ip, all_ones_addr);

    uip_udp_conn_t *udp_conn = uip_udp_new(&ip, 0, flipdot_net_data);

    if (!udp_conn) {
	    return;
    }

    uip_udp_bind(udp_conn, HTONS(FLIPDOT_PORT));

    flipdot_init();

    uint16_t i = 0;
    for(i=0; i<256; i++) {
        flipper[i] = reverse(i);
    }
}


void
flipdot_net_data(void)
{
    if (!uip_newdata()) {
	    return;
    }

    uint8_t buffer[uip_len];
    uint8_t i;
    for(i=0; i<uip_len; i++) {
        uint8_t c = ((uint8_t *)uip_appdata)[i];
        buffer[i] = flipper[c];
    }
    //memcpy(buffer, uip_appdata, uip_len);
    
    //struct flipdot_packet* packet = (struct flipdot_packet*)buffer;
    
    flipdot_data(buffer, uip_len);
}

/*
  -- Ethersex META --
  header(protocols/flipdot/flipdot_net.h)
  net_init(flipdot_net_init)
*/
