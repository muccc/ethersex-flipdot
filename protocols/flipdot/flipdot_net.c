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
//#include "services/flipdot/flipdot.h"
#include "core/portio/portio.h"

#include "config.h"

#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>

#define BUF ((struct uip_udpip_hdr *) (uip_appdata - UIP_IPUDPH_LEN))

struct flipdot_packet
{
    uint8_t panel_number;
    uint8_t command;
	uint8_t frame[16*20];
}__attribute__((packed));

void
flipdot_net_init (void)
{
    uip_ipaddr_t ip;
    uip_ipaddr_copy(&ip, all_ones_addr);

    uip_udp_conn_t *udp_conn = uip_udp_new(&ip, 0, flipdot_net_main);

    if (!udp_conn) {
	    return;
    }

    uip_udp_bind(udp_conn, HTONS(FLIPDOT_PORT));

    DDRB |= (1<<PB0);
    PORTB |= (1<<PB0);
}


void
flipdot_net_main(void)
{
    static uint8_t index = 0;

    if (!uip_newdata()) {
	    return;
    }

    uip_slen = 0;
    uint16_t len = uip_len;
    uint8_t buffer[uip_len];
    memcpy(buffer, uip_appdata, uip_len);

    struct flipdot_packet* packet = (struct flipdot_packet*)buffer;
    if (buffer[0]-index != 1 &&
        !(index == 0xff && buffer[0] == 0x00)) {
        PORTB ^= (1<<PB0);
    }
    index = buffer[0];
    _delay_ms(10);
}

/*
  -- Ethersex META --
  header(protocols/flipdot/flipdot_net.h)
  net_init(flipdot_net_init)
*/
