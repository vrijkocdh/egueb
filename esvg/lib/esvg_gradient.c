/* Esvg - SVG
 * Copyright (C) 2011 Jorge Luis Zapata, Vincent Torri
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
#include "Esvg.h"
#include "esvg_private.h"
#include "esvg_values.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_GRADIENT_MAGIC_CHECK(d) \
	do {\
		if (!EINA_MAGIC_CHECK(d, ESVG_GRADIENT_MAGIC))\
			EINA_MAGIC_FAIL(d, ESVG_GRADIENT_MAGIC);\
	} while(0)

typedef struct _Esvg_Gradient
{
	EINA_MAGIC
	Esvg_Gradient_Setup setup;
	Esvg_Gradient_Units units;
	Eina_List *stops;
	void *data;
} Esvg_Gradient;
/*----------------------------------------------------------------------------*
 *                       The radial gradient paint server                     *
 *----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*
 *                       The linear gradient paint server                     *
 *----------------------------------------------------------------------------*/
static Esvg_Gradient * _esvg_gradient_get(Enesim_Renderer *r)
{
	Esvg_Gradient *thiz;

	thiz = esvg_paint_server_data_get(r);
	ESVG_GRADIENT_MAGIC_CHECK(thiz);

	return thiz;
}
/*----------------------------------------------------------------------------*
 *                         Esvg Element interface                             *
 *----------------------------------------------------------------------------*/
static Eina_Bool _esvg_gradient_setup(Enesim_Renderer *r, Enesim_Renderer *rel)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(r);
	thiz->setup(r, rel, thiz->stops);
	return EINA_TRUE;
}

static void _esvg_gradient_cleanup(Enesim_Renderer *r)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(r);
	/* FIXME for later */
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Enesim_Renderer * esvg_gradient_new(Esvg_Gradient_Descriptor *descriptor,
		void *data)
{
	Esvg_Gradient *thiz;
	Esvg_Paint_Server_Descriptor pdescriptor;
	Enesim_Renderer *r;

	thiz = calloc(1, sizeof(Esvg_Gradient));
	if (!thiz) return NULL;

	EINA_MAGIC_SET(thiz, ESVG_GRADIENT_MAGIC);
	thiz->setup = descriptor->setup;
	thiz->data = data;

	pdescriptor.name_get = descriptor->name_get;
	pdescriptor.renderer_get = descriptor->renderer_get;
	pdescriptor.setup = _esvg_gradient_setup;
	pdescriptor.clone = descriptor->clone;

	/* Default values */
	r = esvg_paint_server_new(&pdescriptor, thiz);
	return r;
}

void * esvg_gradient_data_get(Enesim_Renderer *r)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(r);
	return thiz->data;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool esvg_is_gradient(Enesim_Renderer *r)
{
	return EINA_TRUE;
}

EAPI void esvg_gradient_stop_add(Enesim_Renderer *r, Esvg_Gradient_Stop *s)
{
	Esvg_Gradient *thiz;
	Esvg_Gradient_Stop *stop;

	if (!s) return;

	thiz = _esvg_gradient_get(r);

	stop = malloc(sizeof(Esvg_Gradient_Stop));
	*stop = *s;
	thiz->stops = eina_list_append(thiz->stops, stop);
}

EAPI void esvg_gradient_stop_get(Enesim_Renderer *r, const Eina_List **l)
{
	Esvg_Gradient *thiz;

	if (!l) return;

	thiz = _esvg_gradient_get(r);
	*l = thiz->stops;
}

EAPI void esvg_gradient_units_set(Enesim_Renderer *r, Esvg_Gradient_Units units)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(r);
	thiz->units = units;
}

EAPI void esvg_gradient_units_get(Enesim_Renderer *r, Esvg_Gradient_Units *units)
{
	Esvg_Gradient *thiz;

	thiz = _esvg_gradient_get(r);
	if (units) *units = thiz->units;
}
