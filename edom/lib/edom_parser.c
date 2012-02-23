/* Edom - DOM
 * Copyright (C) 2011 Jorge Luis Zapata
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ctype.h> /* for isspace() */

#include <Eina.h>

#include "Edom.h"
#include "edom_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
struct _Edom_Parser
{
	Eina_Array *contexts;
	Edom_Parser_Descriptor *descriptor;
	char *root;
	char *location;
	void *data;
};

static Eina_Bool _edom_parser_tag_get(Edom_Parser *thiz, const char *content, size_t sz,
		int *tag_id)
{
	if (!thiz->descriptor) return EINA_FALSE;
	if (!thiz->descriptor->tag_get) return EINA_FALSE;
	return thiz->descriptor->tag_get(thiz, content, sz, tag_id);
}
				
static Edom_Tag * _edom_parser_tag_new(Edom_Parser *thiz, int tag_id)
{
	if (!thiz->descriptor) return NULL;
	if (!thiz->descriptor->tag_new) return NULL;
	return thiz->descriptor->tag_new(thiz, tag_id);
}
/*----------------------------------------------------------------------------*
 *                      Eina's simple XML interface                           *
 *----------------------------------------------------------------------------*/
static Eina_Bool _edom_parser_cb(void *data, Eina_Simple_XML_Type type,
		const char *content, unsigned offset, unsigned length)
{
	Edom_Parser *thiz = data;
	Edom_Tag *parent = NULL;
	Eina_Array *contexts;
	Eina_Bool ret;
	int count;
	int tag_id;

	contexts = thiz->contexts;
	count = eina_array_count_get(contexts);
	if (count)
	{
		parent = eina_array_data_get(contexts, count - 1);
	}

	switch (type)
	{
		case EINA_SIMPLE_XML_COMMENT:
			/* FIXME what to do here ... the comments are still being parsed */
			break;
		case EINA_SIMPLE_XML_OPEN:
		case EINA_SIMPLE_XML_OPEN_EMPTY:
		{
			Edom_Tag *new_tag;
			int sz;
			const char *attrs;
			int attr_length;

			attrs = eina_simple_xml_tag_attributes_find(content, length);
			sz = length;
			if (attrs)
			{
				sz = attrs - content;
				attr_length = length - sz;
				while ((sz > 0) && (isspace(content[sz - 1])))
					sz--;
			}
			ret = _edom_parser_tag_get(thiz, content, sz, &tag_id);
			if (!ret)
			{
				/* TODO we should add some flag on the parser to either break or return FALSE
				 * i.e stop on error or not
				 */
				break;
			}
			if (parent)
			{
				/* check the parent supports this tag */
				new_tag = _edom_parser_tag_new(thiz, tag_id);
				/* add it to the current tag */
				edom_tag_child_add(parent, new_tag);
			}
			else
			{
				new_tag = _edom_parser_tag_new(thiz, tag_id);
			}
			/* kind of ugly but works */
			if (type == EINA_SIMPLE_XML_OPEN)
			{
				/* push it the new tag as a new context */
				eina_array_push(thiz->contexts, new_tag);
			}
		}
		break;

		case EINA_SIMPLE_XML_CLOSE:
		ret = _edom_parser_tag_get(thiz, content, length, &tag_id);
		if (!ret)
		{
			break;
		}
		eina_array_pop(thiz->contexts);
		//edom_context_tag_close(context, tag_id);
		break;

		case EINA_SIMPLE_XML_DATA:
		//edom_context_data(context, content, length);
		break;

		case EINA_SIMPLE_XML_CDATA:
		//edom_context_cdata(context, content, length);
		break;

		default:
		break;
	}

	return EINA_TRUE;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Edom_Parser * edom_parser_new(Edom_Parser_Descriptor *descriptor, void *data)
{
	Edom_Parser *thiz;

	thiz = calloc(1, sizeof(Edom_Parser));
	thiz->contexts = eina_array_new(1);
	thiz->descriptor = descriptor;
	thiz->data = data;

	return thiz;
}

EAPI void edom_parser_delete(Edom_Parser *thiz)
{
	eina_array_free(thiz->contexts);
	free(thiz);
}

EAPI void edom_parser_location_set(Edom_Parser *thiz, const char *location)
{
	if (thiz->location)
	{
		free(thiz->location);
		thiz->location = NULL;
	}
	if (location)
	{
		const char *tmp;
		size_t len;

		thiz->location = strdup(location);
		/* get last '/' */
		len = strlen(location);
		tmp = location + len;
		while (*tmp != '/' && tmp != location)
			tmp--;

		len = (tmp - location) + 1;
		thiz->root = malloc(len + 1);
		strncpy(thiz->root, location, len);
		thiz->root[len] = '\0';
	}
}

EAPI const char * edom_parser_location_get(Edom_Parser *thiz)
{
	if (!thiz) return NULL;
	return thiz->location;
}

EAPI const char * edom_parser_root_get(Edom_Parser *thiz)
{
	if (!thiz) return NULL;
	return thiz->root;
}

EAPI Eina_Bool edom_parser_parse(Edom_Parser *thiz, Edom_Context *doc, const char *content, size_t len)
{
	if (!thiz) return EINA_FALSE;

	eina_array_push(thiz->contexts, doc);
	return eina_simple_xml_parse(content, len, EINA_TRUE, _edom_parser_cb, thiz);
}

EAPI void * edom_parser_data_get(Edom_Parser *thiz)
{
	return thiz->data;
}

EAPI void edom_parser_context_push(Edom_Parser *thiz, Edom_Context *c)
{
	if (!thiz) return;
	eina_array_push(thiz->contexts, c);
}

EAPI Edom_Context * edom_parser_context_pop(Edom_Parser *thiz)
{
	Edom_Context *c;

	if (!thiz) return NULL;
	c = eina_array_pop(thiz->contexts);
	return c;
}
