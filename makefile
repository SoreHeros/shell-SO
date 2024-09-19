p0.out: p0.c pid.o echo.o
	gcc -Wall -o p0.out p0.c *.o

pid.o: p0/pid.c p0/pid.h
	gcc -c p0/pid.c

echo.o: p0/echo.c p0/echo.h
	gcc -c p0/echo.c

clean:
	rm *.out *.o