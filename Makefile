CC      = g++
C       = cpp

CFLAGS  = -g

ifeq ("$(shell uname)", "Darwin")
  LDFLAGS     = -framework Foundation -framework GLUT -framework OpenGL -lOpenImageIO -lm
else
  ifeq ("$(shell uname)", "Linux")
    LDFLAGS   = -L /usr/lib64/ -lglut -lGL -lGLU
  endif
endif

PROJECT		= Planet

OBJECTS = ${PROJECT}.o 

${PROJECT}:	${PROJECT}.o  
	${CC} ${CFLAGS} ${LFLAGS} -o ${PROJECT} ${OBJECTS} ${LDFLAGS}

%.o: %.cpp
	${CC} -c ${CFLAGS} *.${C}

clean:
	rm -f core.* *.o *~ ${PROJECT}