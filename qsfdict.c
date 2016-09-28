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
#include <stdint.h>
#include "engine.h"
#include "sf-generate-dictionary-entries.h"

#include "qsfdict.h"

enum
{
	INBUF_LEN	= 1024,
};

static void do_sf_ext_reset(void) { }
static void do_ext_word(void)
{
static const char msg[] = "hello, qt forth!\n";

	sf_push((cell) msg);
	sf_push(sizeof msg - 1);
	do_type();
}

static uint8_t inbuf[INBUF_LEN];
static int inbuf_len, inbuf_idx;
static void do_inbuf(void) { sf_push((cell) inbuf); }
static void do_inbuf_len(void) { sf_push((cell) & inbuf_len); }
static void do_inbuf_idx(void) { sf_push((cell) & inbuf_idx); }

static struct word dict_base_dummy_word[1] = { MKWORD(0, 0, "", 0) };
static const struct word custom_dict[] = {
	/* override the sforth supplied engine reset */
	MKWORD(dict_base_dummy_word,	0,		"",	0)
	MKWORD(custom_dict,		__COUNTER__,	"ext-word",	do_ext_word)
        /* time-related words */
	MKWORD(custom_dict,		__COUNTER__,	"ms",		do_sleep_ms)
        /* widget-related words */
	MKWORD(custom_dict,		__COUNTER__,	"widget-create",	widget_create)
	MKWORD(custom_dict,		__COUNTER__,	"widget-setval",	widget_setval)
	MKWORD(custom_dict,		__COUNTER__,	"widget-settext",	widget_settext)
	MKWORD(custom_dict,		__COUNTER__,	"widget-getval",	widget_getval)
	MKWORD(custom_dict,		__COUNTER__,	"widget-enable",	widget_enable)
	MKWORD(custom_dict,		__COUNTER__,	"widget-disable",	widget_disable)
	MKWORD(custom_dict,		__COUNTER__,	"widget-setstate",	widget_setstate)
	MKWORD(custom_dict,		__COUNTER__,	"widget-set-range",	widget_setrange)
	MKWORD(custom_dict,		__COUNTER__,	"msgbox",		widget_create)
        /* graphics drawing words */
	MKWORD(custom_dict,		__COUNTER__,	"circle",		draw_circle)
	MKWORD(custom_dict,		__COUNTER__,	"rect",			draw_rect)
	MKWORD(custom_dict,		__COUNTER__,	"line",			draw_line)
	MKWORD(custom_dict,		__COUNTER__,	"textout",		draw_text)
        /* graphic item operations */
	MKWORD(custom_dict,		__COUNTER__,	"scale",		gitem_scale)
	MKWORD(custom_dict,		__COUNTER__,	"move-to",		gitem_move_to)
	MKWORD(custom_dict,		__COUNTER__,	"rotate",		gitem_rotate)
	MKWORD(custom_dict,		__COUNTER__,	"delete",		gitem_delete)
        /* turtle graphics words */
	MKWORD(custom_dict,		__COUNTER__,	"left",			turtle_left)
	MKWORD(custom_dict,		__COUNTER__,	"right",		turtle_right)
	MKWORD(custom_dict,		__COUNTER__,	"forward",		turtle_forward)
        /* incoming data processing words */
	MKWORD(custom_dict,		__COUNTER__,	"inbuf",		do_inbuf)
	MKWORD(custom_dict,		__COUNTER__,	"inbuf-len",		do_inbuf_len)
	MKWORD(custom_dict,		__COUNTER__,	"inbuf-idx",		do_inbuf_idx)

}, * custom_dict_start = custom_dict + __COUNTER__;

void sf_ext_init(void)
{
	sf_merge_custom_dictionary(dict_base_dummy_word, custom_dict_start);
}
