all : crearTerminal ejecutarServidor

compilarCliente: p2-client.c
	gcc p2-client.c -o p2-client

compilaServidor: p2-server.c
	gcc p2-server.c -o p2-server

ejecutarCliente: compilarCliente p2-client
	./p2-client

ejecutarServidor: compilaServidor p2-server
	./p2-server

crearTerminal: compilarCliente
	gnome-terminal -- ./p2-client

compilarIndexador: p2-index.c
	gcc p2-index.c -o p2-index

ejecutarIndexador: compilarIndexador p2-index
	./p2-index

clean: 
	rm -r p2-server p2-client log.txt
	
cleanIndexador: 
	rm -r salidaHash salidaIndex p2-index
