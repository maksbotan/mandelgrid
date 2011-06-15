
SOURCES=	mandel.c c_mandel_wrap.c
OBJECTS=	${SOURCES:.c=.o}

PYINC=		$(shell python-config --include)
PYLIBS=		$(shell python-config --libs)

all: _c_mandel.so

_c_mandel.so: $(OBJECTS)
	gcc -shared -o $@ $(OBJECTS) $(PYLIBS)

c_mandel_wrap.c:
	swig -python c_mandel.i

%.o: %.c
	gcc -o $@ -c $< -fPIC $(PYINC) $(CFLAGS) -ffast-math

clean:
	rm -rf *.pyc *.o *.so *_wrap.c c_mandel.py

.PHONY: clean
