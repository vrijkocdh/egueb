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
#ifndef _EGUEB_DOM_ATTR_OBJECT_PRIVATE_H_
#define _EGUEB_DOM_ATTR_OBJECT_PRIVATE_H_

#include "egueb_dom_string.h"
#include "egueb_dom_node.h"
#include "egueb_dom_attr.h"
#include "egueb_dom_value.h"
#include "egueb_dom_attr_private.h"

#define EGUEB_DOM_ATTR_OBJECT_DESCRIPTOR egueb_dom_attr_object_descriptor_get()
#define EGUEB_DOM_ATTR_OBJECT_CLASS_GET(o) EGUEB_DOM_ATTR_OBJECT_CLASS(		\
		(ENESIM_OBJECT_INSTANCE(o))->klass)
#define EGUEB_DOM_ATTR_OBJECT_CLASS(k) ENESIM_OBJECT_CLASS_CHECK(k,		\
		Egueb_Dom_Attr_Object_Class, EGUEB_DOM_ATTR_OBJECT_DESCRIPTOR)
#define EGUEB_DOM_ATTR_OBJECT(o) ENESIM_OBJECT_INSTANCE_CHECK(o,			\
		Egueb_Dom_Attr, EGUEB_DOM_ATTR_OBJECT_DESCRIPTOR)

typedef struct _Egueb_Dom_Attr_Object
{
	Egueb_Dom_Attr parent;
} Egueb_Dom_Attr_Object;

typedef Eina_Bool (*Egueb_Dom_Attr_Object_Value_Get)(Egueb_Dom_Attr *thiz,
		Egueb_Dom_Attr_Type type, void ***o);
typedef Egueb_Dom_Value_Descriptor * (*Egueb_Dom_Attr_Object_Value_Descriptor_Get)
		(Egueb_Dom_Attr *thiz);

typedef struct _Egueb_Dom_Attr_Object_Class
{
	Egueb_Dom_Attr_Class parent;
	Egueb_Dom_Attr_Object_Value_Get value_get;
} Egueb_Dom_Attr_Object_Class;

Eina_Bool egueb_dom_attr_object_value_get(Egueb_Dom_Attr *p,
		Egueb_Dom_Attr_Type type, void ***o);

/*
 * Helper macro to define an object attribute with style, default and
 * animation values
 */
#define EGUEB_DOM_ATTR_OBJECT_BOILERPLATE(attr, value_prefix,		\
		 attr_prefix) 							\
										\
typedef struct _##attr								\
{										\
	Egueb_Dom_Attr_Object base;						\
	void *styled;								\
	void *anim;								\
	void *value;								\
	void *def;								\
} attr;										\
										\
typedef struct _##attr##_Class							\
{										\
	Egueb_Dom_Attr_Object_Class base;					\
} attr##_Class;									\
										\
static Eina_Bool _##attr_prefix##_value_get(Egueb_Dom_Attr *p,			\
		Egueb_Dom_Attr_Type type, void ***o)				\
{										\
	attr *thiz;								\
										\
	thiz = ENESIM_OBJECT_INSTANCE_CHECK(p, attr,				\
			attr_prefix##_descriptor_get());			\
	switch (type)								\
	{									\
		case EGUEB_DOM_ATTR_TYPE_ANIMATED:				\
		*o = (void **)&thiz->anim;					\
		break;								\
										\
		case EGUEB_DOM_ATTR_TYPE_STYLED:				\
		*o = (void **)&thiz->styled;					\
		break;								\
										\
		case EGUEB_DOM_ATTR_TYPE_BASE:					\
		*o = (void **)&thiz->value;					\
		break;								\
										\
		case EGUEB_DOM_ATTR_TYPE_DEFAULT:				\
		*o = (void **)&thiz->def;					\
		break;								\
										\
		default:							\
		return EINA_FALSE;						\
		break;								\
	}									\
	return EINA_TRUE;							\
}										\
										\
static const Egueb_Dom_Value_Descriptor *					\
_##attr_prefix##_value_descriptor_get(Egueb_Dom_Attr *p)			\
{										\
	return value_prefix##_descriptor_get();					\
}										\
										\
ENESIM_OBJECT_INSTANCE_BOILERPLATE(EGUEB_DOM_ATTR_OBJECT_DESCRIPTOR,		\
		attr, attr##_Class, attr_prefix)				\
										\
static void _##attr_prefix##_class_init(void *k)				\
{										\
	Egueb_Dom_Attr_Class *p_klass;						\
	Egueb_Dom_Attr_Object_Class *o_klass;					\
										\
	p_klass = EGUEB_DOM_ATTR_CLASS(k);					\
	p_klass->value_descriptor_get = _##attr_prefix##_value_descriptor_get;	\
	o_klass = EGUEB_DOM_ATTR_OBJECT_CLASS(k);				\
	o_klass->value_get = _##attr_prefix##_value_get;			\
}										\
										\
static void _##attr_prefix##_instance_init(void *o)				\
{										\
}										\
										\
static void _##attr_prefix##_instance_deinit(void *o)				\
{										\
}

#endif
