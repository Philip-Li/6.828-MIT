6.828 requires you to setup toolchain according to the following page:
https://pdos.csail.mit.edu/6.828/2014/tools.html

I setup my toolchain on a mbp with osx 10.11.5. Most of what's mentioned in the guide works very well. But I had to make some changes. I tried Linux before. It was much smoother.

[GCC]
Problem:
gtype-desc.c:8829:18: error: subscripted value is neither array nor pointer nor vector

Solution:
http://www.tuicool.com/articles/bMZrQz
modify write_field_root method in gcc/gengtype.c. This is easy.

[QEMU]
Problem:
customized versions of QEMU for 6.828 are not compatiable with latest glib, which are the only version brew offers. In a word, dependency hell. This really took me a long time to solve. I even went as far as to compile glib from source, only to find things got more complicated (of course, dependency hell!!).

Solution:
use Rudix to install. I remember version number is 2.44 (but I could be wrong)


