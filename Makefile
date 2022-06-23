all : ejecutarServidor

ejecutarCliente: compilarCliente cliente
	./cliente

compilarCliente: cliente.c
	gcc cliente.c -o cliente


compilaServidor: servidor.c
	gcc servidor.c -o servidor

crearTerminal: compilarCliente
	gnome-terminal -- ./cliente

ejecutarServidor: compilaServidor crearTerminal servidor
	./servidor

compilarIndexador: indexador.c
	gcc indexador.c -o indexador

ejecutarIndexador: compilarIndexador indexador
	./indexador
clean: 
	rm -r servidor cliente
cleanIndexador: 
	rm -r salidaHash salidaIndex indexador
