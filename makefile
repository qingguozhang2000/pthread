all: a.out demo.exe

a.out: main.c
	gcc -g -pthread main.c -o a.out

demo.exe: demo.c
	gcc -g -pthread demo.c -o demo.exe

clean:
	rm -rf a.out* demo.exe*
