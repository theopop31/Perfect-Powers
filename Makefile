build:
	gcc tema1.c utils.c -o tema1 -lpthread -Wall -lm
clean:
	rm -rf tema1 utils