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

void sf_ext_init(void);
/*
 *
 * time related words
 *
 */
void do_sleep_ms(void);
/*
 *
 * qt widget related words
 *
 */
void widget_create(void);
void widget_setval(void);
void widget_settext(void);
void widget_getval(void);
void widget_enable(void);
void widget_disable(void);
void widget_setstate(void);
void widget_setrange(void);

/*
 *
 * graphics drawing words
 *
 */
void draw_circle(void);
void draw_rect(void);
void draw_line(void);
void draw_text(void);
/*
 *
 * graphics items operations
 *
 */
void gitem_scale(void);
void gitem_move_to(void);
void gitem_rotate(void);
void gitem_delete(void);
/*
 *
 * turtle graphics words
 *
 */
void turtle_left(void);
void turtle_right(void);
void turtle_forward(void);
