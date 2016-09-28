/*
Copyright (c) 2014-2016 stoyan shopov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#include <math.h>
#include <QDebug>

#include "mainwindow.hxx"
#include "qsfdict.h"

extern "C"
{

static QList<scell> get_iparams_from_sfstack(int n) { QList<scell> p; while (n --) p.prepend(sf_pop()); return p; }
static QByteArray get_qbytearray_from_sfstack(void)
{
	char * s;
	int len;
	len = sf_pop();
	s = (char *) sf_pop();
	return QByteArray(s, len);
}
static QList<QString> get_sparams_from_sfstack(int n) { QList<QString> p; while (n --) p.prepend(get_qbytearray_from_sfstack()); return p; }

/* time-related words */
void do_sleep_ms(void) { sfobj->do_sleep_ms(get_iparams_from_sfstack(1)); }
/* widget-related words */
void widget_create(void) { sfobj->createWidget(get_qbytearray_from_sfstack()); }
void widget_setval(void) { sfobj->do_widget_op("set-val", get_iparams_from_sfstack(1), get_sparams_from_sfstack(0)); }
void widget_settext(void) { sfobj->do_widget_op("set-text", get_iparams_from_sfstack(0), get_sparams_from_sfstack(1)); }
void widget_getval(void) { sfobj->do_widget_op("get-val", get_iparams_from_sfstack(0), get_sparams_from_sfstack(0)); }
void widget_enable(void) { sfobj->do_widget_op("enable", get_iparams_from_sfstack(0), get_sparams_from_sfstack(0)); }
void widget_disable(void) { sfobj->do_widget_op("disable", get_iparams_from_sfstack(0), get_sparams_from_sfstack(0)); }
void widget_setstate(void) { sfobj->do_widget_op("set-state", get_iparams_from_sfstack(1), get_sparams_from_sfstack(0)); }
void widget_setrange(void) { sfobj->do_widget_op("set-range", get_iparams_from_sfstack(2), get_sparams_from_sfstack(0)); }

/* graphic items related words */
void draw_circle(void) { sfobj->create_graphic_item("circle", get_iparams_from_sfstack(3)); }
void draw_rect(void) { sfobj->create_graphic_item("rect", get_iparams_from_sfstack(4)); }
void draw_line(void) { sfobj->create_graphic_item("line", get_iparams_from_sfstack(4)); }
void draw_text(void) { sfobj->create_graphic_item("text", get_iparams_from_sfstack(2)); }


/* graphic items operation words */
void gitem_scale(void) { sfobj->do_graphic_item_op("scale", get_iparams_from_sfstack(1)); }
void gitem_move_to(void) { sfobj->do_graphic_item_op("move-to", get_iparams_from_sfstack(2)); }
void gitem_rotate(void) { sfobj->do_graphic_item_op("rotate", get_iparams_from_sfstack(1)); }
void gitem_delete(void) { sfobj->do_graphic_item_op("delete", QList<scell>()); }

/* turtle graphics words */
static unsigned turtle_angle_degs = 270;
static int turtle_x, turtle_y;
void turtle_left(void) { int x = turtle_angle_degs + sf_pop(); x %= 360; if (x < 0) x += 360; turtle_angle_degs = x; }
void turtle_right(void) { int x = turtle_angle_degs - sf_pop(); x %= 360; if (x < 0) x += 360; turtle_angle_degs = x; qDebug() << "angle is" << x; }
void turtle_forward(void)
{ int nx, ny, d;
	d = sf_pop();
	nx = turtle_x + d * cos(((double) turtle_angle_degs / 180.) * M_PI);
	ny = turtle_y + d * sin(((double) turtle_angle_degs / 180.) * M_PI);
	sfobj->create_graphic_item("line", QList<scell>() << turtle_x << turtle_y << nx << ny);
	turtle_x = nx;
	turtle_y = ny;
}

}
