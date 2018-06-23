main:
	
	gcc -pthread -o client gtk_player.c clientSam.c getUser.c `pkg-config --libs gtk+-2.0 libvlc` `pkg-config --cflags gtk+-2.0 libvlc` -w

clean:
	rm client

