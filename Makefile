default:
	gcc src/bello.c -o dist/bello
	gcc src/main.c src/tokenize.c src/alias.c -o dist/myshell

run: default
	./dist/myshell