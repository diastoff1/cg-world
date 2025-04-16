# set this variable to the directory in which you saved the common files
commondir = ../common/

all : lab3-3 lab3-4a lab3-4d

lab3-3 : lab3-3.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c
	g++ -Wall -o lab3-3 -I$(commondir) -I../common/Linux -DGL_GLEXT_PROTOTYPES lab3-3.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c -lX11 -lGL -lm

lab3-4a : lab3-4a.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c
	g++ -Wall -o lab3-4a -I$(commondir) -I../common/Linux -DGL_GLEXT_PROTOTYPES lab3-4a.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c -lX11 -lGL -lm

lab3-4d : lab3-4d.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c
	g++ -Wall -o lab3-4d -I$(commondir) -I../common/Linux -DGL_GLEXT_PROTOTYPES lab3-4d.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c -lX11 -lGL -lm

clean :
	rm lab3-3
	rm lab3-4a
	rm lab3-4d

