/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985-1988 by Supoj Sutanthavibul
 * Parts Copyright (c) 1989-2015 by Brian V. Smith
 * Parts Copyright (c) 1991 by Paul King
 * Parts Copyright (c) 2016-2020 by Thomas Loimer
 *
 * Any party obtaining a copy of these files is granted, free of charge, a
 * full and unrestricted irrevocable, world-wide, paid up, royalty-free,
 * nonexclusive right and license to deal in this software and documentation
 * files (the "Software"), including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense and/or sell copies of
 * the Software, and to permit persons who receive copies from any such
 * party to do so, with the only requirement being that the above copyright
 * and this permission notice remain intact.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "e_copy2.h"

#include <stdio.h>
#ifdef I18N
#include <locale.h>
#endif

#include "resources.h"
#include "mode.h"
#include "object.h"
#include "f_save.h"
#include "u_create.h"
#include "u_drag.h"
#include "u_elastic.h"
#include "u_markers.h"
#include "u_search.h"
#include "w_canvas.h"
#include "w_cursor.h"
#include "w_mousefun.h"
#include "w_msgpanel.h"

#include "d_box.h"


#include "resources.h"
#include "mode.h"
#include "object.h"
#include "f_util.h"
#include "u_create.h"
#include "u_elastic.h"
#include "u_list.h"
#include "u_redraw.h"
#include "w_canvas.h"
#include "w_cursor.h"
#include "w_indpanel.h"
#include "w_mousefun.h"
#include "w_msgpanel.h"

/* local routine declarations */
static void	init_copy(F_line *p, int type, int x, int y, int px, int py);
static void	init_arb_copy(F_line *p, int type, int x, int y, int px,int py);
static void	init_constrained_copy(F_line *p, int type, int x, int y, int px,
					int py);
static void	init_copy_to_scrap(F_line *p, int type, int x, int y, int px,
					int py);



void
copy_selected2(void)
{
    canvas_kbd_proc = null_proc;
    canvas_locmove_proc = null_proc;
    canvas_ref_proc = null_proc;
    init_searchproc_left(init_arb_copy);
    init_searchproc_middle(init_constrained_copy);
    init_searchproc_right(init_copy_to_scrap);
    canvas_leftbut_proc = object_search_left;
    canvas_middlebut_proc = object_search_middle;
    canvas_rightbut_proc = object_search_right;
    return_proc = copy_selected2;
    set_cursor(pick15_cursor);
    set_mousefun("copy object", "horiz/vert copy", "copy to cut buf",
		LOC_OBJ, LOC_OBJ, LOC_OBJ);
    reset_action_on();
}

static void
init_arb_copy(F_line *p, int type, int x, int y, int px, int py)
{
    constrained = MOVE_ARB;
    init_copy(p, type, x, y, px, py);
    set_mousefun("place object", "array placement", "cancel",
		LOC_OBJ, LOC_OBJ, LOC_OBJ);
    draw_mousefun_canvas();
}

static void
init_constrained_copy(F_line *p, int type, int x, int y, int px, int py)
{
    constrained = MOVE_HORIZ_VERT;
    init_copy(p, type, x, y, px, py);
    set_mousefun("place object", "array placement", "cancel",
		LOC_OBJ, LOC_OBJ, LOC_OBJ);
    draw_mousefun_canvas();
}

static void
init_copy(F_line *p, int type, int x, int y, int px, int py)
{
    /* turn off all markers */
    update_markers(0);
    switch (type) {
    case O_COMPOUND:
	set_cursor(null_cursor);
	cur_c = (F_compound *) p;
	new_c = copy_compound(cur_c);
      //

	init_compounddragging(new_c, px, py);
	break;
    case O_POLYLINE:
	set_cursor(null_cursor);
      F_line	   *new_1;
	cur_l = (F_line *) p;
	new_l = copy_line(cur_l);
      //new_1->pen_color = cur_pencolor;
      new_l->fill_color = cur_fillcolor;
      new_l->fill_style = cur_fillstyle;
	init_linedragging(new_l, px, py);
	break;
    case O_TXT:
	set_cursor(null_cursor);
	cur_t = (F_text *) p;
	new_t = copy_text(cur_t);
      //

	init_textdragging(new_t, x, y);
	break;
    case O_ELLIPSE:
	set_cursor(null_cursor);
	cur_e = (F_ellipse *) p;
	new_e = copy_ellipse(cur_e);
      //

	init_ellipsedragging(new_e, px, py);
	break;
    case O_ARC:
	set_cursor(null_cursor);
	cur_a = (F_arc *) p;
	new_a = copy_arc(cur_a);
      //

	init_arcdragging(new_a, px, py);
	break;
    case O_SPLINE:
	set_cursor(null_cursor);
	cur_s = (F_spline *) p;
	new_s = copy_spline(cur_s);
      //

	init_splinedragging(new_s, px, py);
	break;
    default:
	return;
    }
}

static void
init_copy_to_scrap(F_line *p, int type, int x, int y, int px, int py)
{
	(void)x; (void)y; (void)px; (void)py;
    FILE	   *fp;
    FILE	   *open_cut_file(void);

    if ((fp=open_cut_file())==NULL)
	return;
#ifdef I18N
    /* set the numeric locale to C so we get decimal points for numbers */
    setlocale(LC_NUMERIC, "C");
#endif  /* I18N */
    write_fig_header(fp);

    switch (type) {
    case O_COMPOUND:
	cur_c = (F_compound *) p;
	write_compound(fp, cur_c);
	break;
    case O_ARC:
	cur_a = (F_arc *) p;
	write_arc(fp, cur_a);
	break;
    case O_ELLIPSE:
	cur_e = (F_ellipse *) p;
	write_ellipse(fp, cur_e);
	break;
    case O_POLYLINE:
	cur_l = (F_line *) p;
	write_line(fp, cur_l);
	break;
    case O_TXT:
	cur_t = (F_text *) p;
	write_text(fp, cur_t);
	break;
    case O_SPLINE:
	cur_s = (F_spline *) p;
	write_spline(fp, cur_s);
	break;
    default:
	fclose(fp);
#ifdef I18N
	/* reset to original locale */
	setlocale(LC_NUMERIC, "");
#endif  /* I18N */
	return;
    }
#ifdef I18N
    /* reset to original locale */
    setlocale(LC_NUMERIC, "");
#endif  /* I18N */
    put_msg("Object copied to scrapfile %s",cut_buf_name);
    fclose(fp);
}
