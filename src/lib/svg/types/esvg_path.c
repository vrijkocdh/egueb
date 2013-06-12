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
#include "egueb_svg_main_private.h"

#include "egueb_svg_types.h"
#include "egueb_svg_types_private.h"

#include "egueb_svg_path_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ESVG_LOG_DEFAULT _egueb_svg_path_log

static int _egueb_svg_path_log = -1;

static void _egueb_svg_path_command_cb(Egueb_Svg_Element_Path_Command *cmd, void *data)
{
	Egueb_Svg_Path_Seg_List *thiz = data;
	egueb_svg_path_seg_list_add(thiz, cmd);
}
/*----------------------------------------------------------------------------*
 *                            Ender interface Path                            *
 *----------------------------------------------------------------------------*/
#define _egueb_svg_path_seg_list_new egueb_svg_path_seg_list_new
#define _egueb_svg_path_seg_list_delete egueb_svg_path_seg_list_unref
#include "egueb_svg_generated_path_seg_list.c"
/*----------------------------------------------------------------------------*
 *                           Path related functions                           *
 *----------------------------------------------------------------------------*/
static Eina_Bool _egueb_svg_path_number_get(char **attr, double *x)
{
	char *iter;
	char *endptr;

	iter = *attr;
	ESVG_SPACE_COMMA_SKIP(iter);
	*x = eina_strtod(iter, &endptr);
	if (iter == endptr)
		return EINA_FALSE;

	*attr = endptr;

	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_path_flag_get(char **attr, Eina_Bool *b)
{
	char *iter;

	iter = *attr;
	ESVG_SPACE_COMMA_SKIP(iter);
	if (*iter == '0')
	{
		*b = EINA_FALSE;
	}
	else if (*iter == '1')
	{
		*b = EINA_TRUE;
	}
	else
	{
		ERR("can not convert flag");
		return EINA_FALSE;
	}
	*attr = iter + 1;
	return EINA_TRUE;
}

static Eina_Bool _egueb_svg_path_point_get(char **attr, Egueb_Svg_Point *p)
{
	if (!_egueb_svg_path_number_get(attr, &p->x))
	{
		ERR("can not convert number");
		return EINA_FALSE;
	}

	if (!_egueb_svg_path_number_get(attr, &p->y))
	{
		ERR("can not convert number");
		return EINA_FALSE;
	}

	return EINA_TRUE;
}

static Eina_Bool egueb_svg_parser_path_line_to(Eina_Bool relative,
		char **value, Egueb_Svg_Element_Path_Command *cmd)
{
	Egueb_Svg_Point p;

	if (!_egueb_svg_path_point_get(value, &p))
	{
		ERR("Can not get point");
		return EINA_FALSE;
	}

	cmd->type = ESVG_PATH_LINE_TO;
	cmd->relative = relative;
	cmd->data.line_to.x = p.x;
	cmd->data.line_to.y = p.y;

	return EINA_TRUE;
}

static Eina_Bool egueb_svg_parser_path_move_to(Eina_Bool relative,
		char **value, Egueb_Svg_Element_Path_Command *cmd)
{
	Egueb_Svg_Point p;

	if (!_egueb_svg_path_point_get(value, &p))
	{
		ERR("Can not get point");
		return EINA_FALSE;
	}
	cmd->type = ESVG_PATH_MOVE_TO;
	cmd->relative = relative;
	cmd->data.move_to.x = p.x;
	cmd->data.move_to.y = p.y;

	return EINA_TRUE;
}

static Eina_Bool egueb_svg_parser_path_hline_to(Eina_Bool relative,
		char **value, Egueb_Svg_Element_Path_Command *cmd)
{
	double c;

	if (!_egueb_svg_path_number_get(value, &c))
	{
		ERR("Can not get coord");
		return EINA_FALSE;
	}
	cmd->type = ESVG_PATH_HLINE_TO;
	cmd->relative = relative;
	cmd->data.hline_to.c = c;

	return EINA_TRUE;
}

static Eina_Bool egueb_svg_parser_path_vline_to(Eina_Bool relative,
		char **value, Egueb_Svg_Element_Path_Command *cmd)
{
	double c;

	if (!_egueb_svg_path_number_get(value, &c))
	{
		ERR("Can not get coord");
		return EINA_FALSE;
	}
	cmd->type = ESVG_PATH_VLINE_TO;
	cmd->relative = relative;
	cmd->data.hline_to.c = c;

	return EINA_TRUE;
}

static Eina_Bool egueb_svg_parser_path_cubic_to(Eina_Bool relative,
		char **value, Egueb_Svg_Element_Path_Command *cmd)
{
	Egueb_Svg_Point ctrl0, ctrl1, p;

	if (!_egueb_svg_path_point_get(value, &ctrl0))
	{
		ERR("Can not get control point %s", *value);
		return EINA_FALSE;
	}

	if (!_egueb_svg_path_point_get(value, &ctrl1))
	{
		ERR("Can not get control point");
		return EINA_FALSE;
	}

	if (!_egueb_svg_path_point_get(value, &p))
	{
		ERR("Can not get point");
		return EINA_FALSE;
	}

	cmd->type = ESVG_PATH_CUBIC_TO;
	cmd->relative = relative;
	cmd->data.cubic_to.ctrl_x0 = ctrl0.x;
	cmd->data.cubic_to.ctrl_y0 = ctrl0.y;
	cmd->data.cubic_to.ctrl_x1 = ctrl1.x;
	cmd->data.cubic_to.ctrl_y1 = ctrl1.y;
	cmd->data.cubic_to.x = p.x;
	cmd->data.cubic_to.y = p.y;

	return EINA_TRUE;
}

static Eina_Bool egueb_svg_parser_path_scubic_to(Eina_Bool relative,
		char **value, Egueb_Svg_Element_Path_Command *cmd)
{
	Egueb_Svg_Point ctrl, p;

	if (!_egueb_svg_path_point_get(value, &ctrl))
	{
		ERR("Can not get control point");
		return EINA_FALSE;
	}

	if (!_egueb_svg_path_point_get(value, &p))
	{
		ERR("Can not get point");
		return EINA_FALSE;
	}

	cmd->type = ESVG_PATH_SCUBIC_TO;
	cmd->relative = relative;
	cmd->data.scubic_to.ctrl_x = ctrl.x;
	cmd->data.scubic_to.ctrl_y = ctrl.y;
	cmd->data.scubic_to.x = p.x;
	cmd->data.scubic_to.y = p.y;

	return EINA_TRUE;
}

static Eina_Bool egueb_svg_parser_path_quadratic_to(	Eina_Bool relative,
		char **value, Egueb_Svg_Element_Path_Command *cmd)
{
	Egueb_Svg_Point ctrl, p;

	if (!_egueb_svg_path_point_get(value, &ctrl))
	{
		ERR("Can not get control point");
		return EINA_FALSE;
	}

	if (!_egueb_svg_path_point_get(value, &p))
	{
		ERR("Can not get point");
		return EINA_FALSE;
	}

	cmd->type = ESVG_PATH_QUADRATIC_TO;
	cmd->relative = relative;
	cmd->data.quadratic_to.ctrl_x = ctrl.x;
	cmd->data.quadratic_to.ctrl_y = ctrl.y;
	cmd->data.quadratic_to.x = p.x;
	cmd->data.quadratic_to.y = p.y;

	return EINA_TRUE;

}

static Eina_Bool egueb_svg_parser_path_squadratic_to(Eina_Bool relative,
		char **value, Egueb_Svg_Element_Path_Command *cmd)
{
	Egueb_Svg_Point p;

	if (!_egueb_svg_path_point_get(value, &p))
	{
		ERR("Can not get point");
		return EINA_FALSE;
	}
	cmd->type = ESVG_PATH_SQUADRATIC_TO;
	cmd->relative = relative;
	cmd->data.squadratic_to.x = p.x;
	cmd->data.squadratic_to.y = p.y;

	return EINA_TRUE;
}

static Eina_Bool egueb_svg_parser_path_arc_to(Eina_Bool relative,
		char **value, Egueb_Svg_Element_Path_Command *cmd)
{
	Egueb_Svg_Point p, radii;
	Eina_Bool large, sweep;
	double angle;

	if (!_egueb_svg_path_point_get(value, &radii))
	{
		ERR("can not get radii");
		return EINA_FALSE;
	}

	if (!_egueb_svg_path_number_get(value, &angle))
	{
		ERR("can not convert number");
		return EINA_FALSE;
	}

	if (!_egueb_svg_path_flag_get(value, &large))
	{
		ERR("can not convert the large flag");
		return EINA_FALSE;
	}

	if (!_egueb_svg_path_flag_get(value, &sweep))
	{
		ERR("can not convert the sweep flag");
		return EINA_FALSE;
	}

	if (!_egueb_svg_path_point_get(value, &p))
	{
		ERR("Can not get point");
		return EINA_FALSE;
	}

	cmd->type = ESVG_PATH_ARC_TO;
	cmd->relative = relative;
	cmd->data.arc_to.rx = radii.x;
	cmd->data.arc_to.ry = radii.y;
	cmd->data.arc_to.angle = angle;
	cmd->data.arc_to.large = large;
	cmd->data.arc_to.sweep = sweep;
	cmd->data.arc_to.x = p.x;
	cmd->data.arc_to.y = p.y;

	return EINA_TRUE;
}

static Eina_Bool egueb_svg_parser_path_close(char **value,
		Egueb_Svg_Element_Path_Command *cmd)
{
	cmd->type = ESVG_PATH_CLOSE;
	cmd->relative = EINA_FALSE;

	return EINA_TRUE;
}

static Eina_Bool egueb_svg_parser_command(char command, char **value,
		Egueb_Svg_Element_Path_Command *cmd)
{
	Eina_Bool ret = EINA_TRUE;

	switch (command)
	{
		case 'L':
		ret = egueb_svg_parser_path_line_to(EINA_FALSE, value, cmd);
		break;

		case 'l':
		ret = egueb_svg_parser_path_line_to(EINA_TRUE, value, cmd);
		break;

		case 'M':
		ret = egueb_svg_parser_path_move_to(EINA_FALSE, value, cmd);
		break;

		case 'm':
		ret = egueb_svg_parser_path_move_to(EINA_TRUE, value, cmd);
		break;

		case 'H':
		ret = egueb_svg_parser_path_hline_to(EINA_FALSE, value, cmd);
		break;

		case 'h':
		ret = egueb_svg_parser_path_hline_to(EINA_TRUE, value, cmd);
		break;

		case 'V':
		ret = egueb_svg_parser_path_vline_to(EINA_FALSE, value, cmd);
		break;

		case 'v':
		ret = egueb_svg_parser_path_vline_to(EINA_TRUE, value, cmd);
		break;

		case 'C':
		ret = egueb_svg_parser_path_cubic_to(EINA_FALSE, value, cmd);
		break;

		case 'c':
		ret = egueb_svg_parser_path_cubic_to(EINA_TRUE, value, cmd);
		break;

		case 'S':
		ret = egueb_svg_parser_path_scubic_to(EINA_FALSE, value, cmd);
		break;

		case 's':
		ret = egueb_svg_parser_path_scubic_to(EINA_TRUE, value, cmd);
		break;

		case 'Q':
		ret = egueb_svg_parser_path_quadratic_to(EINA_FALSE, value, cmd);
		break;

		case 'q':
		ret = egueb_svg_parser_path_quadratic_to(EINA_TRUE, value, cmd);
		break;

		case 'T':
		ret = egueb_svg_parser_path_squadratic_to(EINA_FALSE, value, cmd);
		break;

		case 't':
		ret = egueb_svg_parser_path_squadratic_to(EINA_TRUE, value, cmd);
		break;

		case 'A':
		ret = egueb_svg_parser_path_arc_to(EINA_FALSE, value, cmd);
		break;

		case 'a':
		ret = egueb_svg_parser_path_arc_to(EINA_TRUE, value, cmd);
		break;

		case 'z':
		case 'Z':
		ret = egueb_svg_parser_path_close(value, cmd);
		break;

		default:
		ret = EINA_FALSE;
		break;
	}
	return ret;
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void egueb_svg_path_init(void)
{
	_egueb_svg_path_log = eina_log_domain_register("egueb_svg_path", ESVG_LOG_COLOR_DEFAULT);
	if (_egueb_svg_path_log < 0)
	{
		EINA_LOG_ERR("Can not create log domain.");
		return;
	}
	_egueb_svg_path_seg_list_init();
}

void egueb_svg_path_shutdown(void)
{
	_egueb_svg_path_seg_list_shutdown();
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool egueb_svg_path_data_string_from(const char *value, Egueb_Svg_Command_Cb cb, void *data)
{
	Eina_Bool ret = EINA_TRUE;
	Eina_Bool first = EINA_TRUE;
	char last_command = 0;
	char *iter = (char *)value;

	if (!cb) return EINA_FALSE;

	ESVG_SPACE_SKIP(iter);
	/* empty path data */
	if (!*iter) return EINA_FALSE;
	/* First char must be 'M' or 'm' */
	if ((*iter != 'M') &&
	    (*iter != 'm'))
	{
		ERR("First char not 'M' or 'm' but '%c'", *iter);
		return EINA_FALSE;
	}
	while (*iter)
	{
		Egueb_Svg_Element_Path_Command cmd;
		char command;

 		command = *iter;
		iter++;
		ret = egueb_svg_parser_command(command, &iter, &cmd);
		if (!ret)
		{
			/* try with the last command */
			iter--;
			ret = egueb_svg_parser_command(last_command, &iter, &cmd);
			if (ret)
			{
				cb(&cmd, data);
			}
		}
		else
		{
			/* everything went ok, update the last command */
			last_command = command;
			cb(&cmd, data);
		}

		if (!ret)
		{
			ERR("Unsupported path data instruction (%c) %s", command, iter);
			break;
		}
		/* for the 'move' case the next elements should be lines */
		if ((command == 'm' || command == 'M'))
		{
			/* the next commands should be lines */
			if (command == 'm')
				last_command = 'l';
			else
				last_command = 'L';
		}
		first = EINA_FALSE;
		ESVG_SPACE_COMMA_SKIP(iter);
	}
	return ret;
}

EAPI Egueb_Svg_Path_Seg_List * egueb_svg_path_seg_list_new(void)
{
	Egueb_Svg_Path_Seg_List *thiz;

	thiz = calloc(1, sizeof(Egueb_Svg_Path_Seg_List));
	thiz->ref = 1;
	return thiz;
}

EAPI Egueb_Svg_Path_Seg_List * egueb_svg_path_seg_list_ref(Egueb_Svg_Path_Seg_List *thiz)
{
	thiz->ref++;
	return thiz;
}

EAPI void egueb_svg_path_seg_list_unref(Egueb_Svg_Path_Seg_List *thiz)
{
	thiz->ref--;
	if (!thiz->ref)
	{
		egueb_svg_path_seg_list_clear(thiz);
		free(thiz);
	}
}

EAPI void egueb_svg_path_seg_list_clear(Egueb_Svg_Path_Seg_List *thiz)
{
	Egueb_Svg_Element_Path_Command *cmd;
	EINA_LIST_FREE (thiz->commands, cmd)
	{
		free (cmd);
	}
	thiz->changed = EINA_TRUE;
}

EAPI void egueb_svg_path_seg_list_add(Egueb_Svg_Path_Seg_List *thiz, const Egueb_Svg_Element_Path_Command *cmd)
{
	Egueb_Svg_Element_Path_Command *new_cmd;

	if (!cmd) return;

	new_cmd = calloc(1, sizeof(Egueb_Svg_Element_Path_Command));
	*new_cmd = *cmd;

	thiz->commands = eina_list_append(thiz->commands, new_cmd);
	thiz->changed = EINA_TRUE;
}

EAPI Eina_Bool egueb_svg_path_seg_list_string_from(Egueb_Svg_Path_Seg_List *thiz,
		const char *attr)
{
	egueb_svg_path_seg_list_clear(thiz);
	return egueb_svg_path_data_string_from(attr, _egueb_svg_path_command_cb, thiz);
}