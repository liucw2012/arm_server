all: Server Client

Server: Server.c
	gcc $^ -o $@ -lpthread
Client: Client.c
	gcc $^ -o $@

clean:
	rm -rf Server Client
