all:clean client server

client:
	gcc -Wall ssl-client.c -o client -lssl -lcrypto

server:
	gcc -Wall ssl-server.c -o server -lssl -lcrypto

clean:
	rm -rf client server
