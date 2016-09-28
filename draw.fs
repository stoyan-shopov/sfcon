.( graphics primitives drawing module) cr cr


\ circle data

0 value x
0 value y
0 value r

s" button::circle::x y r circle" widget-create
s" spinbox::circle x::to x" widget-create drop
s" spinbox::circle y::to y" widget-create drop
s" spinbox::radius::to r" widget-create drop

\ rectangle data
0 value x
0 value y
0 value w
0 value h

s" button::rectangle::x y w h rect" widget-create
s" spinbox::rect x::to x" widget-create drop
s" spinbox::rect y::to y" widget-create drop
s" spinbox::width::to w" widget-create drop
s" spinbox::height::to h" widget-create drop
