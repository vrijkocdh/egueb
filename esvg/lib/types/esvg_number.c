/* Esvg - SVG
 * Copyright (C) 2012 Jorge Luis Zapata
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#include "esvg_private_main.h"

#include "esvg_types.h"
#include "esvg_private_types.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT _esvg_number_log

static int _esvg_number_log = -1;

static Ender_Property *ESVG_NUMBER_VALUE;

static Ender_Property *ESVG_NUMBER_ANIMATED_BASE;
static Ender_Property *ESVG_NUMBER_ANIMATED_ANIM;

#define _esvg_number_new NULL
#define _esvg_number_delete NULL
#define _esvg_number_value_set NULL
#define _esvg_number_value_get NULL
#define _esvg_number_value_is_set NULL
#include "generated/esvg_generated_number.c"

#define _esvg_number_animated_new NULL
#define _esvg_number_animated_delete NULL
#define _esvg_number_animated_base_set NULL
#define _esvg_number_animated_base_get NULL
#define _esvg_number_animated_base_is_set NULL
#define _esvg_number_animated_anim_set NULL
#define _esvg_number_animated_anim_get NULL
#define _esvg_number_animated_anim_is_set NULL
#include "generated/esvg_generated_number_animated.c"
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void esvg_number_init(void)
{
	_esvg_number_log = eina_log_domain_register("esvg_number", ESVG_LOG_COLOR_DEFAULT);
	if (_esvg_number_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_esvg_number_init();
	_esvg_number_animated_init();
}

void esvg_number_shutdown(void)
{
	_esvg_number_shutdown();
	_esvg_number_animated_shutdown();
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

