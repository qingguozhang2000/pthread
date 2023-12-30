all: a.out

a.out: main.c common.c guest-wait.c guest-dine.c
	gcc -g -pthread main.c common.c guest-wait.c guest-dine.c -o a.out

clean:
	rm -rf a.out*
