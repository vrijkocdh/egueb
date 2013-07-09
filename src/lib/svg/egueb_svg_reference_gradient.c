/* Egueb
 * Copyright (C) 2011 - 2013 Jorge Luis Zapata
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
#include "egueb_svg_main_private.h"
#include "egueb_svg_main.h"
#include "egueb_svg_reference_gradient_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                             Reference interface                            *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_reference_gradient_process(Egueb_Svg_Reference *r)
{
	Egueb_Svg_Reference_Gradient *thiz;
	Egueb_Svg_Reference_Gradient_Class *klass;
	Enesim_Renderer *ren = NULL;
	Egueb_Dom_Node *stop_node = NULL;
	Eina_Bool ret = EINA_TRUE;

	DBG("Processing a gradient reference");
	thiz = EGUEB_SVG_REFERENCE_GRADIENT(r);
	klass = EGUEB_SVG_REFERENCE_GRADIENT_CLASS_GET(r);
	/* first the implementation */
	if (klass->process)
		ret = klass->process(thiz);
	if (!ret) return ret;

	egueb_svg_reference_paint_server_renderer_get(r, &ren);
	if (!ren) return EINA_FALSE;

	/* now the common stops */
	egueb_svg_gradient_deep_stop_get(r->referenceable, &stop_node);
	enesim_renderer_gradient_stop_clear(ren);
	while (stop_node)
	{
		Egueb_Dom_Node *tmp;

		if (egueb_svg_element_is_stop(stop_node))
		{
			Enesim_Renderer_Gradient_Stop *stop;
			stop = egueb_svg_element_stop_gradient_stop_get(stop_node);
			enesim_renderer_gradient_stop_add(ren, stop);
		}
		egueb_dom_node_sibling_next_get(stop_node, &tmp);
		egueb_dom_node_unref(stop_node);
		stop_node = tmp;
		
	}
	enesim_renderer_unref(ren);

	return ret;
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_ABSTRACT_BOILERPLATE(EGUEB_SVG_REFERENCE_PAINT_SERVER_DESCRIPTOR,
		Egueb_Svg_Reference_Gradient,
		Egueb_Svg_Reference_Gradient_Class,
		egueb_svg_reference_gradient);

static void _egueb_svg_reference_gradient_class_init(void *k)
{
	Egueb_Svg_Reference_Class *klass;

	klass = EGUEB_SVG_REFERENCE_CLASS(k);
	klass->process = _egueb_svg_reference_gradient_process;
}

static void _egueb_svg_reference_gradient_class_deinit(void *k)
{
}

static void _egueb_svg_reference_gradient_instance_init(void *o)
{
}

static void _egueb_svg_reference_gradient_instance_deinit(void *o)
{
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

