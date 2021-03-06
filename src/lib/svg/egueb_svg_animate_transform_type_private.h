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
#ifndef _EGUEB_SVG_ANIMATE_TRANSFORM_TYPE_PRIVATE_H
#define _EGUEB_SVG_ANIMATE_TRANSFORM_TYPE_PRIVATE_H

Eina_Bool egueb_svg_animate_transform_type_string_from(
		Egueb_Svg_Animate_Transform_Type *thiz, const char *value);
char * egueb_svg_animate_transform_type_string_to(Egueb_Svg_Animate_Transform_Type thiz);

#endif
