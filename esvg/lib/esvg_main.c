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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h> /* for floor() */

#include <Etex.h>
#include <Ender.h>

#include "Esvg.h"
#include "esvg_private.h"


/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static int _esvg_init_count = 0;

static void _register_enders(void *data)
{
	/* register the dependency */
	ender_loader_load("edom");
	esvg_element_init();
	esvg_renderable_init();
	esvg_svg_init();
}

static void _constructor_callback(Ender_Element *e, void *data)
{
	if (!esvg_is_element(e))
		return;
	esvg_element_initialize(e);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

int esvg_log_dom_global = -1;

Ender_Namespace * eon_namespace_get(void)
{
	static Ender_Namespace *namespace = NULL;

	if (!namespace)
	{
		namespace = ender_namespace_new("esvg");
	}
	return namespace;
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

/**
 * Initialize the esvg library
 */
EAPI int esvg_init(void)
{
	if (++_esvg_init_count != 1)
		return _esvg_init_count;

	if (!eina_init())
	{
		fprintf(stderr, "Esvg: Eina init failed");
		return --_esvg_init_count;
	}

	esvg_log_dom_global = eina_log_domain_register("esvg", ESVG_LOG_COLOR_DEFAULT);
	if (esvg_log_dom_global < 0)
	{
		EINA_LOG_ERR("Esvg: Can not create a general log domain.");
		goto shutdown_eina;
	}

	if (!enesim_init())
	{
		ERR("Enesim init failed");
		goto unregister_log_domain;
	}

	if (!etex_init())
	{
		ERR("Etex init failed");
		goto shutdown_enesim;
	}

	ender_loader_registry_callback_add(_register_enders, NULL);
	ender_init(NULL, NULL);
	ender_element_new_listener_add(_constructor_callback, NULL);

	return _esvg_init_count;

  shutdown_enesim:
	enesim_shutdown();
  unregister_log_domain:
	eina_log_domain_unregister(esvg_log_dom_global);
	esvg_log_dom_global = -1;
  shutdown_eina:
	eina_shutdown();
	return --_esvg_init_count;
}

/**
 * Shutdown the esvg library
 */
EAPI int esvg_shutdown(void)
{
	if (--_esvg_init_count != 0)
		return _esvg_init_count;

	ender_element_new_listener_remove(_constructor_callback, NULL);
	ender_shutdown();
	etex_shutdown();
	enesim_shutdown();
	eina_log_domain_unregister(esvg_log_dom_global);
	esvg_log_dom_global = -1;
	eina_shutdown();

	return _esvg_init_count;
}
