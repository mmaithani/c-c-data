/**
* compile with the following command
* gcc -Wall -g getUser.c -o getUser `pkg-config --cflags gtk+-2.0` `pkg-config --libs gtk+-2.0`
*/

#include <gtk/gtk.h>

GtkWidget** allSongs;
char **allMp3, **allArtists;
char user2[50];

int addSongs(GtkWidget *box) {
    FILE* fp = fopen("musicaCliente.txt", "r");
    
    char c[50];
    int cInt;
    int i = 0;
    strcpy(c, scanner(fp));
    cInt = atoi(c);
    allSongs = calloc (cInt,sizeof(GtkWidget*));
    allMp3 = calloc (cInt,sizeof(char*));
    allArtists = calloc (cInt,sizeof(char*));

    for(i = 0;i < cInt;i++) {
        allMp3[i] = calloc (256,sizeof(char));
        allArtists[i] = calloc (256,sizeof(char));
    }
    i = 0;
    strcpy(c, scanner(fp));

    while(strcmp(c, "EOF")){
        strcpy(c, scanner(fp));
        
        strcpy(allArtists[i],c);
        strcpy(c, scanner(fp));
        
        allSongs[i] = gtk_check_button_new_with_label(c);
        gtk_box_pack_start (GTK_BOX (box), allSongs[i], TRUE, FALSE, 5);
        gtk_widget_show(allSongs[i]);
        strcpy(allMp3[i],c);
        strcpy(c, scanner(fp));
        i++;
    }
    return cInt;
}

void addPlaylist(GtkWidget *object, GtkWidget** data) {
    int i;
    int howMany = 0;
    FILE* fp = fopen(user2, "a");
    for(i = 0; i < data[1]; i++) {
        if(gtk_toggle_button_get_active (allSongs[i])) {
            howMany++;
        }
    }

    fprintf(fp, "%s->%d:", gtk_entry_get_text (data[2]), howMany);
    for(i = 0; i < data[1]; i++) {
        if(gtk_toggle_button_get_active (allSongs[i])) {
            fprintf(fp, ":%s.mp3::%s::%s:", allMp3[i], allArtists[i], allMp3[i]);
        }
    }
    fclose(fp);
}

void enviarTexto(int sock,char* cual){
    /* Open the file that we wish to transfer */
   char *src;
   src = calloc(1000,sizeof(char));
   sprintf(src,"/home/franco/Documents/SO/Proyecto 1/Final/Server/source/%s",cual);
   FILE *fp = fopen(src,"rb");

    if(fp==NULL)
    { 
        printf("File open error");
    }   
    /* Read data from file and send it */
    while(1)
    {
        /* First read file in chunks of 256 bytes */

        unsigned char buff[256]={0};
        int nread = fread(buff,1,256,fp);
        printf("Bytes leidos: %d \n", nread);        


        /* If read was success, send data. */
        if(nread > 0)
        {
            printf("Enviando \n");
            write(sock, buff, nread);
        }

        /*
         * There is something tricky going on with read .. 
         * Either there was error, or we reached end of file.
         */
        if (nread < 256)
        {
            if (feof(fp))
                printf("End of file\n");
            if (ferror(fp))
                printf("Error de lectura.\n");
            break;
            
        }
    }
    fclose(fp);
}


int newPlaylist(GtkWidget *widget, gpointer data2) {
    strcpy(user2, data2);
    GtkWidget *window;
    GtkWidget *label;
    GtkWidget *entryAddPlaylist;
    GtkWidget *box;
    GtkWidget *btnAddPlaylist;

    GdkColor color;
    color.red = 0000;
    color.green = 0000;
    color.blue = 0;

    GdkColor color2;
    color2.red = 47;
    color2.green = 79;
    color2.blue = 79;
    int cInt = 0;
    gtk_init (NULL, NULL);
    
    // Create the top-level window
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width(GTK_CONTAINER(window),10);
    gtk_widget_set_size_request(GTK_WINDOW(window),500,200);
    gdk_color_parse ("#008B8B", &color2);
    gtk_widget_modify_bg(GTK_WINDOW(window), GTK_STATE_NORMAL, &color2);
    gtk_window_set_urgency_hint (GTK_WINDOW(window), TRUE);
    gtk_widget_show  (window);



    // Create a vbox and attach it to the window
    box = gtk_vbox_new (FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), box); 
    gtk_widget_show(box);


    // Create a label and add it to the box
    label = gtk_label_new ("Escriba el nombre del playlist");
    gtk_box_pack_start (GTK_BOX (box), label, TRUE, FALSE, 5);
    gtk_widget_show(label);

     // Create an entry field and add it to the box
    entryAddPlaylist = gtk_entry_new_with_max_length(10);
    gtk_box_pack_start (GTK_BOX (box), entryAddPlaylist, TRUE, FALSE, 5);
    gtk_widget_show(entryAddPlaylist);
    gtk_widget_grab_focus(entryAddPlaylist);


    cInt = addSongs(box);

    btnAddPlaylist = gtk_button_new_with_label("Iniciar");
    gtk_box_pack_start(GTK_BOX(box), btnAddPlaylist, TRUE, FALSE, 5);
    gtk_widget_show(btnAddPlaylist);

    gdk_color_parse ("#ffffff", &color);
    gtk_widget_modify_text(GTK_WIDGET(btnAddPlaylist),GTK_STATE_NORMAL, &color);  
   
    GtkWidget** data = calloc (3,sizeof(GtkWidget*));
    

    data[0] = window;
    data[1] = cInt;
    data[2] = entryAddPlaylist;
    g_signal_connect (btnAddPlaylist, "clicked",G_CALLBACK (addPlaylist),data);


    gtk_main ();
    
    return 0;
}
