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
#include "egueb_svg_matrix.h"
#include "egueb_svg_referenceable_units.h"
#include "egueb_svg_attr_referenceable_units.h"
#include "egueb_svg_attr_matrix.h"
#include "egueb_svg_attr_string.h"
#include "egueb_svg_gradient_private.h"

/*
 * TODO
 * the stops_changed flag has to be cleared somewhere but where? add a cleanup
 * function?
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EGUEB_SVG_ELEMENT_GRADIENT_DEEP_GET(n, attr, def, fun)			\
	if (egueb_svg_is_gradient(n))						\
	{									\
		Egueb_Svg_Gradient *thiz;					\
		thiz = EGUEB_SVG_GRADIENT(n);					\
		if (!egueb_dom_attr_is_set(thiz->attr))				\
		{								\
			Egueb_Dom_Node *href = NULL;				\
			egueb_svg_gradient_href_node_get(n, &href);		\
			if (href)						\
			{							\
				fun(href, attr);				\
				egueb_dom_node_unref(href);			\
			}							\
			else							\
			{							\
				*attr = def;					\
			}							\
		}								\
		else								\
		{								\
			egueb_dom_attr_final_get(thiz->attr, attr);		\
		}								\
	}									\
	else									\
	{									\
		Egueb_Dom_Node *href = NULL;					\
		egueb_svg_gradient_href_node_get(n, &href);			\
		if (href)							\
		{								\
			fun(href, attr);					\
			egueb_dom_node_unref(href);				\
		}								\
		else								\
		{								\
			*attr = def;						\
		}								\
	}


#if 0
static void _egueb_svg_element_gradient_stop_mutation_cb(Ender_Element *e,
		const char *event_name,
		void *event_data, void *data)
{
	Egueb_Svg_Element_Gradient *thiz = data;

	thiz->stops_changed = EINA_TRUE;
}

static void _egueb_svg_element_gradient_deep_units_get(Egueb_Svg_Element_Gradient *thiz,
		Egueb_Svg_Element_Gradient_Units *units)
{
	if (!thiz->units.is_set && thiz->href_e)
	{
		Egueb_Svg_Element_Gradient *other;

		other = _egueb_svg_element_gradient_get(thiz->href_t);
		_egueb_svg_element_gradient_deep_units_get(other, units);
	}
	else
		*units = thiz->units.v;
}

static void _egueb_svg_element_gradient_deep_transform_get(Egueb_Svg_Element_Gradient *thiz,
		Enesim_Matrix *transform)
{
	if (!egueb_svg_attribute_animated_transform_is_set(&thiz->transform) && thiz->href_e)
	{
		Egueb_Svg_Element_Gradient *other;

		other = _egueb_svg_element_gradient_get(thiz->href_t);
		_egueb_svg_element_gradient_deep_transform_get(other, transform);
	}
	else
	{
		egueb_svg_attribute_animated_transform_final_get(&thiz->transform, transform);
	}
}

static void _egueb_svg_element_gradient_deep_spread_method_get(Egueb_Svg_Element_Gradient *thiz, 
		Egueb_Svg_Spread_Method *spread_method)
{
	if (!thiz->spread_method.is_set && thiz->href_e)
	{
		Egueb_Svg_Element_Gradient *other;

		other = _egueb_svg_element_gradient_get(thiz->href_t);
		_egueb_svg_element_gradient_deep_spread_method_get(other, spread_method);
	}
	else
		*spread_method = thiz->spread_method.v;
}

static void _egueb_svg_element_gradient_context_generate(Egueb_Svg_Element_Gradient *thiz, Egueb_Svg_Element_Gradient_Context *ctx)
{
	/* if no href set just use our own properties */
	_egueb_svg_element_gradient_deep_units_get(thiz, &ctx->units);
	_egueb_svg_element_gradient_deep_spread_method_get(thiz, &ctx->spread_method);
	_egueb_svg_element_gradient_deep_transform_get(thiz, &ctx->transform);
}

static Eina_Bool _egueb_svg_element_gradient_stop_propagate(Egueb_Dom_Tag *t, Egueb_Dom_Tag *child,
		void *user_data)
{
	Egueb_Svg_Type type;
	Enesim_Renderer_Gradient_Stop *stop;
	Enesim_Renderer *r = user_data;

	/* only propagate stops */
	type = egueb_svg_element_internal_type_get(child);
	if (type != ESVG_TYPE_STOP) return EINA_TRUE;

	stop = egueb_svg_element_stop_gradient_stop_get(child);
	DBG("Adding a gradient stop at position %g with color %08x", stop->pos, stop->argb);
	enesim_renderer_gradient_stop_add(r, stop);
	return EINA_TRUE;
}

static void _egueb_svg_element_gradient_stop_generate(Egueb_Dom_Tag *t, Enesim_Renderer *r)
{
	Egueb_Svg_Element_Gradient *thiz;
	Egueb_Dom_Tag *child;

	thiz = _egueb_svg_element_gradient_get(t);
	/* clear all the stops */
	enesim_renderer_gradient_stop_clear(r);
	/* check if we need to generate our own childs or the other gradient childs */
	child = egueb_dom_tag_child_get(t);
	if (!child && thiz->href_t)
	{
		DBG("Generating relative stops in '%s' from '%s'",
				egueb_svg_element_internal_id_get(t),
				thiz->current.href);
		_egueb_svg_element_gradient_stop_generate(thiz->href_t, r);
	}
	else
	{
		DBG("Generating local stops");
		/* TODO we need to do the setup the stops as those
		 * might not have done the setup yet
		 */
		//egueb_svg_element_internal_child_setup(t, c, error, _egueb_svg_element_svg_setup_interceptor, thiz);
		egueb_dom_tag_child_foreach(t, _egueb_svg_element_gradient_stop_propagate, r);
	}
}
/*----------------------------------------------------------------------------*
 *                       Esvg Paint Server interface                          *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_element_gradient_attribute_set(Ender_Element *e,
		const char *key, const char *value)
{
	if (strcmp(key, "xlink:href") == 0)
	{
		egueb_svg_element_gradient_href_set(e, value);
	}
	else if (!strcmp(key, "gradientUnits"))
	{
		Egueb_Svg_Element_Gradient_Units units;
		egueb_svg_parser_gradient_units_string_from(&units, value);
		egueb_svg_element_gradient_units_set(e, units);
	}
	else if (strcmp(key, "gradientTransform") == 0)
	{
		Enesim_Matrix matrix;

		egueb_svg_transformation_string_from(&matrix, value);
		egueb_svg_element_gradient_transform_set(e, &matrix);
	}
	else if (strcmp(key, "spreadMethod") == 0)
	{
		Egueb_Svg_Spread_Method smethod;

		egueb_svg_parser_spread_method_get(&smethod, value);
		egueb_svg_element_gradient_spread_method_set(e, smethod);
	}
	else
	{
		Egueb_Svg_Element_Gradient *thiz;
		Egueb_Dom_Tag *t;

		t = ender_element_object_get(e);
		thiz = _egueb_svg_element_gradient_get(t);
		if (thiz->descriptor.attribute_set)
			return thiz->descriptor.attribute_set(e, key, value);
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

static int * _egueb_svg_element_gradient_attribute_animated_fetch(Egueb_Dom_Tag *t, const char *name)
{
	Egueb_Svg_Element_Gradient *thiz;

	thiz = _egueb_svg_element_gradient_get(t);
	/* FIXME provide our own */
	if (thiz->descriptor.attribute_animated_fetch)
		return thiz->descriptor.attribute_animated_fetch(t, name);
	return NULL;
}

static Eina_Bool _egueb_svg_element_gradient_child_add(Egueb_Dom_Tag *t, Egueb_Dom_Tag *child)
{
	Egueb_Svg_Element_Gradient *thiz;
	Egueb_Svg_Type type;
	Ender_Element *child_e;

	if (!egueb_svg_is_element_internal(child))
		return EINA_FALSE;

	thiz = _egueb_svg_element_gradient_get(t);
	type = egueb_svg_element_internal_type_get(child);
	/* only support stops and animate variants */
	switch (type)
	{
		case ESVG_TYPE_STOP:
		case ESVG_TYPE_ANIMATE:
		break;

		default:
		ERR("Child %d not supported", type);
		return EINA_FALSE;
	}

	child_e = egueb_svg_element_ender_get(child);
	ender_event_listener_add(child_e, "Mutation", _egueb_svg_element_gradient_stop_mutation_cb, thiz);
	thiz->stops_changed = EINA_TRUE;

	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_element_gradient_child_remove(Egueb_Dom_Tag *t, Egueb_Dom_Tag *child)
{
	Egueb_Svg_Element_Gradient *thiz;

	thiz = _egueb_svg_element_gradient_get(t);
	thiz->stops_changed = EINA_TRUE;
	// remove the event handler
	//ender_event_listener_remove(child_e, "Mutation", _egueb_svg_element_gradient_stop_mutation_cb, thiz);
	return EINA_TRUE;
}

static Egueb_Svg_Element_Setup_Return _egueb_svg_element_gradient_setup(Egueb_Dom_Tag *t,
		Egueb_Svg_Context *c,
		Egueb_Svg_Element_Context *ctx,
		Egueb_Svg_Attribute_Presentation *attr,
		Enesim_Log **error)
{
	Egueb_Svg_Element_Gradient *thiz;
	Eina_Bool child_ret;

	thiz = _egueb_svg_element_gradient_get(t);
	/* in case the href attribute has changed we need to
	 * pick a new reference and use those stops in case
	 * we dont have any
	 */
	if (thiz->state_changed)
	{
		Egueb_Svg_Element_Setup_Return ret;
		Ender_Element *topmost;
		Ender_Element *href_e;
		Egueb_Dom_Tag *href_t;

		/* TODO propagate our own properties in case those are not set
		 * or we better use some "deep" functions that should get the
		 * attribute from its href in case it has one
		 * we need to start adding real properties, not only the values
		 * but the is_set, all wrapped into a single struct
		 */
		if (thiz->href_e)
		{
			/* TODO remove the event handlers from the old href */
			//ender_element_unref(thiz->href_e);
			thiz->href_e = NULL;
			thiz->href_t = NULL;
		}
		egueb_svg_element_internal_topmost_get(t, &topmost);
		if (!topmost)
			goto state_changed_done;

		if (!thiz->current.href)
			goto state_changed_done;

		DBG("Using '%s' to inherit properties and stops from", thiz->current.href);
		/* TODO add the event handlers on the new generate */
		egueb_svg_element_svg_element_get(topmost, thiz->current.href, &href_e);
		if (!href_e)
		{
			WARN("Impossible to get the element '%s'", thiz->current.href);
			goto state_changed_done;
		}

		/* TODO check that the referring gradient is of the same type? */
		/* TODO check if this gradient has childs, if not also generate them */
		thiz->stops_changed = EINA_TRUE;
		thiz->href_e = href_e;
		thiz->href_t = ender_element_object_get(href_e);

		/* do the setup on the href */
		DBG("Doing the setup on the href tag");
		ret = egueb_svg_element_internal_setup(thiz->href_t, c, error);
		/* if the other needs to queue, also queue ourselves
		 * if the other failed we also failed
		 */
		if (ret != ESVG_SETUP_OK)
			return ret;
	}
state_changed_done:
	if (!thiz->stops_changed)
		return ESVG_SETUP_OK;

	/* call the setup on the stops */
	DBG("Doing the setup on the gradient children");
	child_ret = egueb_svg_element_internal_child_setup(t, c, error, NULL, NULL, thiz);
	if (!child_ret) return ESVG_SETUP_FAILED;
	return ESVG_SETUP_OK;
}

static void _egueb_svg_element_gradient_cleanup(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element_Gradient *thiz;

	thiz = _egueb_svg_element_gradient_get(t);
	/* remove the flags */
	thiz->stops_changed = EINA_FALSE;
	/* swap the states */
	if (thiz->state_changed)
	{
		if (thiz->past.href)
		{
			free(thiz->past.href);
			thiz->past.href = NULL;
		}
		if (thiz->current.href)
		{
			thiz->past.href = strdup(thiz->current.href);
		}
		thiz->state_changed = EINA_FALSE;
	}
}

static Eina_Bool _egueb_svg_element_gradient_propagate(Egueb_Dom_Tag *t,
		Egueb_Svg_Context *c,
		const Egueb_Svg_Element_Context *ctx,
		const Egueb_Svg_Attribute_Presentation *attr,
		void *data,
		Enesim_Log **error)
{
	Egueb_Svg_Element_Gradient *thiz;
	Egueb_Svg_Element_Gradient_Context gctx;
	Eina_Bool ret = EINA_TRUE;
	Enesim_Renderer *r = data;

	thiz = _egueb_svg_element_gradient_get(t);
	if (thiz->stops_changed)
	{
		_egueb_svg_element_gradient_stop_generate(t, r);
	}
	/* generate the context */
	_egueb_svg_element_gradient_context_generate(thiz, &gctx);

	if (thiz->descriptor.propagate)
		ret = thiz->descriptor.propagate(t, c, ctx, attr, &gctx, r, error);
	return ret;
}

static Eina_Bool _egueb_svg_element_gradient_reference_add(Egueb_Dom_Tag *t, Egueb_Svg_Referenceable_Reference *rr)
{
	Egueb_Svg_Element_Gradient *thiz;
	Enesim_Renderer *r;

	thiz = _egueb_svg_element_gradient_get(t);
	/* in case some stop has changed:
	 *  the setup will be called
	 *  the stops will be generated
	 *  the stops will be propagated to each reference
	 * so no need to do twice
	 */
	if (thiz->stops_changed)
		return EINA_TRUE;
	if (thiz->state_changed)
		return EINA_TRUE;
	/* add every stop to our newly created reference */
	r = rr->data;
	_egueb_svg_element_gradient_stop_generate(t, r);
	return EINA_TRUE;
}

static void _egueb_svg_element_gradient_free(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element_Gradient *thiz;

	thiz = _egueb_svg_element_gradient_get(t);
	if (thiz->descriptor.free)
		thiz->descriptor.free(t);
	free(thiz);
}
/*----------------------------------------------------------------------------*
 *                           The Ender interface                              *
 *----------------------------------------------------------------------------*/
static void _egueb_svg_element_gradient_href_set(Egueb_Dom_Tag *t, const char *href)
{
	Egueb_Svg_Element_Gradient *thiz;

	thiz = _egueb_svg_element_gradient_get(t);
	ERR("Setting href = %s", href);
	if (thiz->current.href)
	{
		free(thiz->current.href);
		thiz->current.href = NULL;
	}

	if (href)
	{
		thiz->current.href = strdup(href);
	}
	thiz->state_changed = EINA_TRUE;
}

static void _egueb_svg_element_gradient_href_get(Egueb_Dom_Tag *t, const char **href)
{
	Egueb_Svg_Element_Gradient *thiz;

	if (!href) return;

	thiz = _egueb_svg_element_gradient_get(t);
	*href = thiz->current.href;
}

static void _egueb_svg_element_gradient_gradient_units_set(Egueb_Dom_Tag *t, Egueb_Svg_Element_Gradient_Units units)
{
	Egueb_Svg_Element_Gradient *thiz;

	thiz = _egueb_svg_element_gradient_get(t);
	thiz->units.v = units;
	thiz->units.is_set = EINA_TRUE;
}

static void _egueb_svg_element_gradient_gradient_units_get(Egueb_Dom_Tag *t, Egueb_Svg_Element_Gradient_Units *units)
{
	Egueb_Svg_Element_Gradient *thiz;

	thiz = _egueb_svg_element_gradient_get(t);
	if (units) *units = thiz->units.v;
}

static Eina_Bool _egueb_svg_element_gradient_gradient_units_is_set(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element_Gradient *thiz;

	thiz = _egueb_svg_element_gradient_get(t);
	return thiz->units.is_set;
}

static void _egueb_svg_element_gradient_gradient_transform_set(Egueb_Dom_Tag *t, const Egueb_Svg_Matrix_Animated *transform)
{
	Egueb_Svg_Element_Gradient *thiz;
	Enesim_Matrix m;
	Eina_Bool animating;

	thiz = _egueb_svg_element_gradient_get(t);
	enesim_matrix_identity(&m);
	animating = egueb_svg_element_attribute_animate_get(t);
	egueb_svg_attribute_animated_transform_set(&thiz->transform,
		transform, &m, animating);
}

static void _egueb_svg_element_gradient_gradient_transform_get(Egueb_Dom_Tag *t, Egueb_Svg_Matrix_Animated *transform)
{
	Egueb_Svg_Element_Gradient *thiz;

	thiz = _egueb_svg_element_gradient_get(t);
	egueb_svg_attribute_animated_transform_get(&thiz->transform, transform);
}

static Eina_Bool _egueb_svg_element_gradient_gradient_transform_is_set(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element_Gradient *thiz;

	thiz = _egueb_svg_element_gradient_get(t);
	return egueb_svg_attribute_animated_transform_is_set(&thiz->transform);
}

static void _egueb_svg_element_gradient_spread_method_set(Egueb_Dom_Tag *t, Egueb_Svg_Spread_Method spread_method)
{
	Egueb_Svg_Element_Gradient *thiz;

	thiz = _egueb_svg_element_gradient_get(t);
	thiz->spread_method.v = spread_method;
}

static void _egueb_svg_element_gradient_spread_method_get(Egueb_Dom_Tag *t, Egueb_Svg_Spread_Method *spread_method)
{
	Egueb_Svg_Element_Gradient *thiz;

	thiz = _egueb_svg_element_gradient_get(t);
	if (spread_method) *spread_method = thiz->spread_method.v;
}

#endif

static Eina_Bool _egueb_svg_gradient_children_process_cb(
		Egueb_Dom_Node *child, void *data)
{
	Egueb_Dom_Node_Type type;

	egueb_dom_node_type_get(child, &type);
	if (type != EGUEB_DOM_NODE_TYPE_ELEMENT_NODE)
		return EINA_TRUE;
	egueb_dom_element_process(EGUEB_DOM_ELEMENT(child));
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                           Referenceable interface                          *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_gradient_process(
		Egueb_Svg_Referenceable *r)
{
	DBG("Processing a gradient");
	/* iterate over the children and call the process there */
	egueb_dom_node_children_foreach(EGUEB_DOM_NODE(r),
			_egueb_svg_gradient_children_process_cb,
			NULL);
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                              Object interface                              *
 *----------------------------------------------------------------------------*/
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_gradient, Egueb_Svg_Gradient, transform);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_gradient, Egueb_Svg_Gradient, units);
EGUEB_DOM_ATTR_FETCH_DEFINE(egueb_svg_gradient, Egueb_Svg_Gradient, xlink_href);

ENESIM_OBJECT_ABSTRACT_BOILERPLATE(EGUEB_SVG_PAINT_SERVER_DESCRIPTOR,
		Egueb_Svg_Gradient, Egueb_Svg_Gradient_Class,
		egueb_svg_gradient);

static void _egueb_svg_gradient_class_init(void *k)
{
	Egueb_Svg_Referenceable_Class *klass;

	klass = EGUEB_SVG_REFERENCEABLE_CLASS(k);
	klass->process = _egueb_svg_gradient_process;
}

static void _egueb_svg_gradient_class_deinit(void *k)
{
}

static void _egueb_svg_gradient_instance_init(void *o)
{
	Egueb_Svg_Gradient *thiz;

	thiz = EGUEB_SVG_GRADIENT(o);
	thiz->units = egueb_svg_attr_referenceable_units_new(
			egueb_dom_string_ref(EGUEB_SVG_GRADIENT_UNITS),
			EGUEB_SVG_REFERENCEABLE_UNITS_USER_SPACE_ON_USE, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);

	thiz->transform = egueb_svg_attr_matrix_new(
			egueb_dom_string_ref(EGUEB_SVG_GRADIENT_TRANSFORM),
			&EGUEB_SVG_MATRIX_IDENTITY, EINA_TRUE,
			EINA_FALSE, EINA_FALSE);
	thiz->xlink_href = egueb_svg_attr_string_new(
			egueb_dom_string_ref(EGUEB_SVG_XLINK_HREF),
			NULL, EINA_TRUE, EINA_FALSE, EINA_FALSE);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_gradient, transform);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_gradient, units);
	EGUEB_DOM_ELEMENT_CLASS_PROPERTY_ADD(thiz, egueb_svg_gradient, xlink_href);
}

static void _egueb_svg_gradient_instance_deinit(void *o)
{
	Egueb_Svg_Gradient *thiz;

	thiz = EGUEB_SVG_GRADIENT(o);
	/* the properties */
	egueb_dom_node_unref(thiz->units);
	egueb_dom_node_unref(thiz->transform);
	egueb_dom_node_unref(thiz->xlink_href);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
#if 0
/* The ender wrapper */
void egueb_svg_element_gradient_init(void)
{
	_egueb_svg_element_gradient_log = eina_log_domain_register("egueb_svg_element_gradient", ESVG_LOG_COLOR_DEFAULT);
	if (_egueb_svg_element_gradient_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_egueb_svg_element_gradient_init();
}

void egueb_svg_element_gradient_shutdown(void)
{
	if (_egueb_svg_element_gradient_log < 0)
		return;
	_egueb_svg_element_gradient_shutdown();
	eina_log_domain_unregister(_egueb_svg_element_gradient_log);
	_egueb_svg_element_gradient_log = -1;
}

Egueb_Dom_Tag * egueb_svg_element_gradient_new(Egueb_Svg_Element_Gradient_Descriptor *descriptor,
		Egueb_Svg_Type type,
		void *data)
{
	Egueb_Svg_Element_Gradient *thiz;
	Egueb_Svg_Paint_Server_Descriptor pdescriptor;
	Egueb_Dom_Tag *t;

	thiz = calloc(1, sizeof(Egueb_Svg_Element_Gradient));
	if (!thiz) return NULL;

	EINA_MAGIC_SET(thiz, ESVG_ELEMENT_GRADIENT_MAGIC);
	thiz->descriptor.propagate = descriptor->propagate;
	thiz->descriptor.child_add = descriptor->child_add;
	thiz->descriptor.child_remove = descriptor->child_remove;
	thiz->descriptor.setup = descriptor->setup;
	thiz->descriptor.attribute_set = descriptor->attribute_set;
	thiz->descriptor.attribute_animated_fetch = descriptor->attribute_animated_fetch;
	thiz->data = data;

	pdescriptor.child_add = _egueb_svg_element_gradient_child_add;
	pdescriptor.child_remove = _egueb_svg_element_gradient_child_remove;
	pdescriptor.attribute_set = _egueb_svg_element_gradient_attribute_set;
	pdescriptor.attribute_animated_fetch = _egueb_svg_element_gradient_attribute_animated_fetch;
	pdescriptor.attribute_get = descriptor->attribute_get;
	pdescriptor.cdata_set = descriptor->cdata_set;
	pdescriptor.text_set = NULL;
	pdescriptor.text_get = NULL;
	pdescriptor.free = _egueb_svg_element_gradient_free;
	pdescriptor.initialize = descriptor->initialize;
	pdescriptor.setup = _egueb_svg_element_gradient_setup;
	pdescriptor.cleanup = _egueb_svg_element_gradient_cleanup;
	pdescriptor.renderer_new = descriptor->renderer_new;
	pdescriptor.propagate = _egueb_svg_element_gradient_propagate;
	pdescriptor.reference_add = _egueb_svg_element_gradient_reference_add;

	/* Default values */
	thiz->units.v = ESVG_OBJECT_BOUNDING_BOX;
	thiz->spread_method.v = ESVG_SPREAD_METHOD_PAD;
	enesim_matrix_identity(&thiz->transform.base.v);
	enesim_matrix_identity(&thiz->transform.anim.v);

	t = egueb_svg_paint_server_new(&pdescriptor, type, thiz);
	return t;
}

void * egueb_svg_element_gradient_data_get(Egueb_Dom_Tag *t)
{
	Egueb_Svg_Element_Gradient *thiz;

	thiz = _egueb_svg_element_gradient_get(t);
	return thiz->data;
}
#endif

void egueb_svg_gradient_href_node_get(Egueb_Dom_Node *n, Egueb_Dom_Node **href)
{
	Egueb_Svg_Gradient *thiz;
	Egueb_Dom_String *xlink_href = NULL;

	thiz = EGUEB_SVG_GRADIENT(n);
	egueb_dom_attr_final_get(thiz->xlink_href, &xlink_href);
	if (!xlink_href)
	{
		*href = NULL;
	}
	else
	{
		Egueb_Dom_Node *doc = NULL;

		egueb_dom_node_document_get(n, &doc);
		if (!doc)
		{
			WARN("No document set");
			*href = NULL;
		}
		else
		{
			egueb_dom_document_element_get_by_id(doc, xlink_href, href);
			egueb_dom_node_unref(doc);
		}
	}
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_svg_is_gradient(Egueb_Dom_Node *n)
{
	if (!n) return EINA_FALSE;
	if (!enesim_object_instance_inherits(ENESIM_OBJECT_INSTANCE(n),
			EGUEB_SVG_GRADIENT_DESCRIPTOR))
		return EINA_FALSE;
	return EINA_TRUE;
}

EAPI void egueb_svg_gradient_deep_units_get(Egueb_Dom_Node *n,
		Egueb_Svg_Referenceable_Units *units)
{
	EGUEB_SVG_ELEMENT_GRADIENT_DEEP_GET(n, units,
			EGUEB_SVG_REFERENCEABLE_UNITS_USER_SPACE_ON_USE,
			egueb_svg_gradient_deep_units_get);
}

EAPI void egueb_svg_gradient_deep_transform_get(Egueb_Dom_Node *n,
		Enesim_Matrix *transform)
{
	Enesim_Matrix m;

	enesim_matrix_identity(&m);
	EGUEB_SVG_ELEMENT_GRADIENT_DEEP_GET(n, transform, m,
			egueb_svg_gradient_deep_transform_get);
}

#if 0
EAPI void egueb_svg_element_gradient_href_set(Ender_Element *e, const char *href)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_GRADIENT_HREF, href, NULL);
}

EAPI void egueb_svg_element_gradient_href_get(Ender_Element *e, const char **href)
{
	Egueb_Dom_Tag *t;

	t = (Egueb_Dom_Tag *)ender_element_object_get(e);
	_egueb_svg_element_gradient_href_get(t, href);
}

EAPI void egueb_svg_element_gradient_units_set(Ender_Element *e, Egueb_Svg_Element_Gradient_Units units)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_GRADIENT_GRADIENT_UNITS, units, NULL);
}

EAPI void egueb_svg_element_gradient_units_get(Ender_Element *e, Egueb_Svg_Element_Gradient_Units *units)
{
}

EAPI Eina_Bool egueb_svg_element_gradient_units_is_set(Ender_Element *e)
{
	/* for now return false */
	return EINA_FALSE;
}

EAPI void egueb_svg_element_gradient_transform_set(Ender_Element *e, const Enesim_Matrix *transform)
{
	Egueb_Svg_Matrix_Animated a;

	if (!transform)
	{
		ender_element_property_value_set(e, ESVG_ELEMENT_GRADIENT_GRADIENT_TRANSFORM, NULL, NULL);
		return;
	}
	a.base = *transform;
	ender_element_property_value_set(e, ESVG_ELEMENT_GRADIENT_GRADIENT_TRANSFORM, &a, NULL);
}

EAPI void egueb_svg_element_gradient_transform_get(Ender_Element *e, Enesim_Matrix *transform)
{
}

EAPI Eina_Bool egueb_svg_element_gradient_transform_is_set(Ender_Element *e)
{
	/* for now return false */
	return EINA_FALSE;
}

EAPI void egueb_svg_element_gradient_spread_method_set(Ender_Element *e, Egueb_Svg_Spread_Method spread_method)
{
	ender_element_property_value_set(e, ESVG_ELEMENT_GRADIENT_SPREAD_METHOD, spread_method, NULL);
}

EAPI void egueb_svg_element_gradient_spread_method_get(Ender_Element *e, Egueb_Svg_Spread_Method *spread_method)
{
	Egueb_Dom_Tag *t;

	t = (Egueb_Dom_Tag *)ender_element_object_get(e);
	_egueb_svg_element_gradient_spread_method_get(t, spread_method);
}
#endif