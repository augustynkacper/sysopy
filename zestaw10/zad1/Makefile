
all:
	gcc client.c -o client -lpthread
	gcc server.c -o server -lpthread
	@echo "./server [port] [name]"
	@echo "./client [nick] [web] [port]"
	@echo "./client [nick] [unix] [name]"

clean:
	rm -rf socket server client