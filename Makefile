all : imprimir ejecutarServidor

ejecutarCliente: compilarCliente cliente
	./cliente

compilarCliente: cliente.c
	gcc cliente.c -o cliente -lm 


compilaServidor: servidor.c
	gcc servidor.c -o servidor -lm

crearTerminal: compilarCliente
	gnome-terminal -- ./cliente

ejecutarServidor: compilaServidor crearTerminal servidor
	./servidor
imprimir:
	
clean:
	rm -r servidor cliente