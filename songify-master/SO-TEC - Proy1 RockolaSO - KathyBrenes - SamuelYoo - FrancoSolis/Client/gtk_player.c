  // gcc -o gtk_player gtk_player.c `pkg-config --libs gtk+-2.0 libvlc` `pkg-config --cflags gtk+-2.0 libvlc`

/* License WTFPL http://sam.zoy.org/wtfpl/ */
/* Written by Vincent SchÃ¼ÃŸler */
#include <limits.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <vlc/vlc.h>
#include <unistd.h>

#define BORDER_WIDTH 6

enum
{
  SONG_NAME = 0,
  SONG_ARTIST,
  SONG_PATH, 
  NUM_COLS,

} ;

GtkTreeStore *store;
GtkWidget *comboBox;
GtkWidget *view; 
GtkCellRenderer *renderer;
GtkTreeIter   iterGLobal;


char* scanner(FILE *fp);
void refresh(char* buffer);
void destroy(GtkWidget *widget, gpointer data);
void player_widget_on_realize(GtkWidget *widget, gpointer data);
void on_open(GtkWidget *widget, gpointer data);
void open_media(const char* uri);
void play(void);
void pause_player(void);
void on_playpause(GtkWidget *widget, gpointer data);
void on_stop(GtkWidget *widget, gpointer data);
int newPlaylist();

libvlc_media_player_t *media_player;
libvlc_instance_t *vlc_inst;
GtkWidget *playpause_button, *imgPlayPause;;
char pathUrl[1024];
char user[100];
int sockfdGlobal;
gint index;
GtkWidget** allSongs;
char **allMp3, **allArtists;
//Tree************************


char* scannerCancion(FILE* fp){
	int c;
	int i = 1;
	char* palabra;
	palabra =(char*)calloc(i,1+sizeof(char)); 
	//c = getc(fp);  

 
	while((c = getc(fp))!=EOF){ 
		 
		if (isspace(c)){
			palabra[i-1]= c;
			i++;
			palabra =(char*)realloc(palabra, i*sizeof(char));
			if (!isspace(c=getc(fp))||!((c=getc(fp))==':')|| !((c=getc(fp))==';')||!((c=getc(fp))=='=')){
				palabra[i-1]= c;
				i++;https:
				palabra =(char*)realloc(palabra, i*sizeof(char));

			}else{
				continue;    
			}
			
		}else if (c==':' || c==';'||c =='='){   
			
	
			return palabra;   
		}else if (c == '-'){
			if ((c=getc(fp))=='>')
				return palabra;
		}else if  (c == '\n'){
			continue;
		}else{
			palabra[i-1]= c;
			i++;
			palabra =(char*)realloc(palabra, i*sizeof(char));
		} 
	}  
	return "EOF"; 
}

char loadFileMusica(FILE* fp, char row[3][1024]){

	char* c;
	int seguir = 1;
	int i; 
	char* palabra;


  
	c =scannerCancion(fp);
	if(strcmp(c, "EOF")){

		
		strcpy(row[0],c);
		c =scannerCancion(fp);
		strcpy(row[1],c);
		c =scannerCancion(fp);
		strcpy(row[2],c);
		return 1;
	}else{
	  return 0;
	}

		
	
	
   
}

void escogerMusica(int sockfdGlobal, char* pathServer){
	char buffer[256];
	
	strcpy(buffer,"2");
    
    
    
    write(sockfdGlobal,buffer,strlen(buffer)); 
    memset(buffer,0,strlen(buffer)); 
 
   
    read(sockfdGlobal,buffer,256); 
  
    
    memset(buffer,0,strlen(buffer)); 
    strcpy(buffer,pathServer);
    write(sockfdGlobal,buffer,strlen(buffer));
	memset(buffer,0,strlen(buffer)); 
    obtenerCancion();
}




void
  view_onRowActivated (GtkTreeView        *treeview,
					   GtkTreePath        *path,
					   GtkTreeViewColumn  *col,
					   gpointer           data)
  {
	GtkTreeModel *model;
	GtkTreeIter   iter;
	
	

	model = gtk_tree_view_get_model(view);

	if (gtk_tree_model_get_iter(model, &iter, path))
	{
	   gchar *name;
	  
	   char buffer[256];
	   char *pathServer;
	   pathServer = calloc(1024,sizeof(char));   
	   char buf[PATH_MAX + 1];
	   strcpy(pathUrl, "file://");
	   realpath("tempSong", buf);
	   strcat(pathUrl, buf); 
  
	   gtk_tree_model_get(model, &iter, SONG_NAME, &name, -1);
	   gtk_tree_model_get(model, &iter, SONG_PATH, &pathServer, -1);


	   escogerMusica(sockfdGlobal,pathServer);
	   open_media(pathUrl);
	   g_free(name);
	   iterGLobal = iter;
	}
  }



void
  view_forward (GtkTreeView        *treeview,
					   GtkTreePath        *path,
					   GtkTreeViewColumn  *col,
					   gpointer           data)
  {
	GtkTreeModel *model;	
	


	model = gtk_tree_view_get_model(view); 

 
	if (gtk_tree_model_iter_next(model,&iterGLobal))
	{
	   
	}else{
		gtk_tree_model_get_iter_first (model,&iterGLobal);
	}
	
	gchar *name; 
   
   char *pathServer;
   pathServer = calloc(1024,sizeof(char));  
   char buf[PATH_MAX + 1];
   strcpy(pathUrl, "file://");
   realpath("tempSong", buf);
   strcat(pathUrl, buf); 

   gtk_tree_model_get(model, &iterGLobal, SONG_NAME, &name, -1);
   gtk_tree_model_get(model, &iterGLobal, SONG_PATH, &pathServer, -1);

   escogerMusica(sockfdGlobal,pathServer);
   open_media(pathUrl);
   g_free(name);
   
}


static GtkTreeModel * create_and_fill_model (void)
{

  GtkTreeIter    iter;
   
  char buf[PATH_MAX+1];
  realpath("musicaCliente.txt",buf); 


  FILE * fp = fopen(buf,"ab+");
  char row[3][1024]; 
  scannerCancion(fp);
  
  store = gtk_tree_store_new (NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
  
  index = 0;
  while (loadFileMusica(fp,row)){

	gtk_tree_store_append (store, &iter,NULL);
	gtk_tree_store_set (store, &iter,
					  SONG_NAME, row[2],
					  SONG_ARTIST, row[1],
					  SONG_PATH, row[0], 
					  -1);

	index++;
  }

  
  return GTK_TREE_MODEL (store);
}



static GtkWidget *
create_view_and_model (void)
{
 
  GtkTreeModel        *model;
  GtkWidget           *view2; 
  gint offset;
  GtkTreeViewColumn *column;
  view2 = gtk_tree_view_new ();
  
  /* --- Column #1 --- */ 



  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view2),
											   -1,      
											   "Name",  
											   renderer,
											   "text", SONG_NAME,
											   NULL);

  /* --- Column #2 --- */

  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view2),
											   -1,      
											   "Artist",  
											   renderer,
											   "text", SONG_ARTIST,
											   NULL);
  renderer = gtk_cell_renderer_toggle_new();

 
  

  model = create_and_fill_model (); 


 
  gtk_tree_view_set_model (GTK_TREE_VIEW (view2), model);
  


  /* The tree view has acquired its own reference to the
   *  model, so we can drop ours. That way the model will
   *  be freed automatically when the tree view is destroyed */

  g_object_unref (model);

  return view2;
}



//************************************ 

void destroy(GtkWidget *widget, gpointer data) {
	gtk_main_quit();
}


char* scannerPlaylist(FILE* fp,char* tipo){
    int c;
    int i = 1;
    char* palabra;
    char* queEs;
    palabra =(char*)calloc(i,1+sizeof(char)); 
    queEs= calloc(2,sizeof(char));
 
    while((c = getc(fp))!=EOF){ 
         
        if (isspace(c)){
            palabra[i-1]= c;
            i++;
            palabra =(char*)realloc(palabra, i*sizeof(char));
            if (!isspace(c=getc(fp))||!((c=getc(fp))==':')|| !((c=getc(fp))==';')||!((c=getc(fp))=='=')){
                palabra[i-1]= c;
                i++;
                palabra =(char*)realloc(palabra, i*sizeof(char));
 
            }else{
                continue;    
            }
            
        }else if (c==':' || c==';'||c =='='){   
            strcpy(tipo,"cancion");
            return palabra;   
        }else if (c == '-'){
            if ((c=getc(fp))=='>'){
                strcpy(tipo,"playlist");
                return palabra; 
            }
                
        }else if  (c == '\n'){
            continue;
        }else{
            palabra[i-1]= c;
            i++;
            palabra =(char*)realloc(palabra, i*sizeof(char));
        } 
    } 
    strcpy(tipo, "EOF");
    return "EOF"; 
}


void on_open(GtkWidget *widget, gpointer data) {
	GtkWidget *dialog;
	dialog = gtk_file_chooser_dialog_new("Choose Media", data, GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
	if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		char *uri;
		uri = gtk_file_chooser_get_uri(GTK_FILE_CHOOSER(dialog));
		open_media(uri);
		g_free(uri);
	}
	gtk_widget_destroy(dialog);
}



void open_media(const char* uri) {  
	libvlc_media_t *media;
	media = libvlc_media_new_location(vlc_inst, uri);
	libvlc_media_player_set_media(media_player, media);
	play();
	libvlc_media_release(media);
}

void kill_media(){
	libvlc_media_player_stop(media_player);
	libvlc_media_player_release(media_player);
	libvlc_release(vlc_inst);

}
void on_playpause(GtkWidget *widget, gpointer data) {
	if(libvlc_media_player_is_playing(media_player) == 1) {
		pause_player();
	}
	else { 
		play();
	}
}

void on_stop(GtkWidget *widget, gpointer data) {
	pause_player();
	g_print("Canción detenida.\n");
	libvlc_media_player_stop(media_player);
}

void play(void) {
	libvlc_media_player_play(media_player);
	g_print("Canción siendo reproducida...\n");
	imgPlayPause = gtk_image_new_from_stock(GTK_STOCK_MEDIA_PAUSE, GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(playpause_button), imgPlayPause);
}

void pause_player(void) {
	libvlc_media_player_pause(media_player);
	g_print("Canción pausada.\n");
	imgPlayPause = gtk_image_new_from_stock(GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(playpause_button), imgPlayPause);
}
GtkWidget* create_combo_model(char* username) {
  GtkWidget* combo = gtk_combo_box_text_new ();
  char buf[PATH_MAX+1];
  realpath(username,buf);
	
  FILE * fp = fopen(buf,"r");
  rewind(fp);
 char *c;
 char tipo[50];
 c = calloc(1024,sizeof(char));
 strcpy(c,scannerPlaylist(fp,tipo));
 while (strcmp(c,"EOF")){

    if (!strcmp(tipo,"playlist")){
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), c);
    }

    memset(c,0,strlen(c));
    strcpy(c,scannerPlaylist(fp,tipo));
 }
  fclose(fp);
  return combo;
  
}


char* scanner(FILE* fp){
    int c;
    int i = 1;
    char* palabra;
    palabra =(char*)calloc(i,1+sizeof(char)); 
 
    while((c = getc(fp))!=EOF){ 
         
        if (isspace(c)){
            palabra[i-1]= c;
            i++;
            palabra =(char*)realloc(palabra, i*sizeof(char));
            if (!isspace(c=getc(fp))||!((c=getc(fp))==':')|| !((c=getc(fp))==';')||!((c=getc(fp))=='=')){
                palabra[i-1]= c;
                i++;
                palabra =(char*)realloc(palabra, i*sizeof(char));

            }else{
                continue;    
            }
            
        }else if (c==':' || c==';'||c =='='){   
            
            return palabra;   
        }else if (c == '-'){
            if ((c=getc(fp))=='>')
                return palabra;
        }else if  (c == '\n'){
            continue;
        }else{
            palabra[i-1]= c;
            i++;
            palabra =(char*)realloc(palabra, i*sizeof(char));
        } 
    }  
    return "EOF"; 
}




void randomSong(GtkWidget *widget, gpointer data){
	g_print("Reproduciendo canción aleatoria.");
	FILE *fp = fopen("musicaCliente.txt","r");
	if (fp==NULL){
		printf("something went wrong on random song.\n");
	}else{
		char *c;
		c = calloc(1024,sizeof(char));
 		strcpy(c,scanner(fp));
 		int cuantos = atoi(c);
 		time_t t;	
 		srand((unsigned) time(&t));
 		int randomNumber = 	rand() % cuantos;
 		int i ;
 		
 		for (i=0;i<randomNumber;i++){
 			strcpy(c,scanner(fp));
 			strcpy(c,scanner(fp));
 			strcpy(c,scanner(fp));
 		} 
 		strcpy(c,scanner(fp));
 		char buffer[256];
 		escogerMusica(sockfdGlobal,c);
 		char buf[PATH_MAX + 1];
	   strcpy(pathUrl, "file://"); 
	   realpath("tempSong", buf);
	   strcat(pathUrl, buf);
	   open_media(pathUrl);

	}

}
 

void extractPlaylist(GtkWidget *widget, gpointer data){
	char text[512];
	char buffer[256];
	FILE *fp =fopen("musicaCliente.txt","w");
	gtk_tree_store_clear(GTK_TREE_STORE(store));
	strcpy(text,gtk_combo_box_get_active_text (GTK_COMBO_BOX(comboBox)));
	strcpy(buffer,"desplegar playlist");
	write(sockfdGlobal,buffer,strlen(buffer));
	memset(buffer,0,strlen(buffer));
	read(sockfdGlobal,buffer,256);
	memset(buffer,0,strlen(buffer));
	strcpy(buffer,text);
	write(sockfdGlobal,buffer,strlen(buffer));
	memset(buffer,0,strlen(buffer));   
  	GtkTreeIter    iter;
  	GtkTreeModel* model;
	store = gtk_tree_store_new (NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	read(sockfdGlobal,buffer,256);
	fprintf(fp, "%s;",buffer);
	write(sockfdGlobal,buffer,strlen(buffer));
	memset(buffer,0,strlen(buffer));
	char artist[100];
	read(sockfdGlobal,buffer,256);
	while (strcmp(buffer,"termino")){
		fprintf(fp, "%s;",buffer);
		char mp3[100];
		strcpy(mp3,buffer);
		write(sockfdGlobal,buffer,strlen(buffer));
		memset(buffer,0,strlen(buffer));
		char artist[100];
		read(sockfdGlobal,buffer,256);
		fprintf(fp, "%s;",buffer);
		strcpy(artist,buffer);
		write(sockfdGlobal,buffer,strlen(buffer));
		memset(buffer,0,strlen(buffer));
		char title[100];
		
		read(sockfdGlobal,buffer,256);
		fprintf(fp, "%s;",buffer);
		strcpy(title,buffer);
		write(sockfdGlobal,buffer,strlen(buffer));
		memset(buffer,0,strlen(buffer));
		gtk_tree_store_append (store, &iter,NULL);
		gtk_tree_store_set (store, &iter,
					  SONG_NAME, title,
					  SONG_ARTIST, artist,
					  SONG_PATH, mp3,
					  -1);
		read(sockfdGlobal,buffer,256);

	}


	model = GTK_TREE_MODEL (store);
    gtk_tree_view_set_model (GTK_TREE_VIEW (data), model);
    g_object_unref(model);
    fclose(fp);
}

void refreshWindow(GtkWidget *object, GtkWidget** data) {
   	char buffer[256];
   	g_printf("Ventana refrescada.\n");
   	strcpy(buffer, "holis");
    gtk_widget_destroy(data[0]);  
    refresh(buffer);

}


void gtk_player( int sockfd,char* username) {
	GtkWidget *window,
			  *vbox,
			  *menubar,
			  *filemenu,
			  *fileitem,
			  *filemenu_openitem,
			  *hbuttonbox,
			  *stop_button, 
			  *label,
			  *refreshBtn,
			  *playlistBtn,
			  *forwardBtn,
			  *randomBtn,
			  *imgStop,
			  *imgNext,
			  *imgPrevious,
			  *imgRefresh;
	strcpy(user,username);
	sockfdGlobal = sockfd;
	gtk_init (NULL, NULL);
	// setup window
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window), 700, 700);
	g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);
	gtk_container_set_border_width (GTK_CONTAINER (window), 0);
	gtk_window_set_title(GTK_WINDOW(window), "Songify");

	//setup vbox
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	char message[1024];
	strcpy(message, "Bienvenido ");	
	strcat(message, username);
	label = gtk_label_new (message);
	gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
	gtk_widget_show(label);
	view = create_view_and_model ();
	gtk_container_add (GTK_CONTAINER (vbox), view);

	  //Double clickGtkWidget
  
	g_signal_connect(view, "row-activated", (GCallback) view_onRowActivated, sockfdGlobal);

	
	hbuttonbox = gtk_hbutton_box_new();
	gtk_container_set_border_width(GTK_CONTAINER(hbuttonbox), BORDER_WIDTH);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(hbuttonbox), GTK_BUTTONBOX_START);


	playpause_button = gtk_button_new();
	imgPlayPause = gtk_image_new_from_stock(GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(playpause_button), imgPlayPause);
	gtk_box_pack_start(GTK_BOX(hbuttonbox), playpause_button, FALSE, FALSE, 0);

	forwardBtn = gtk_button_new();
	imgNext = gtk_image_new_from_stock(GTK_STOCK_MEDIA_NEXT, GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(forwardBtn), imgNext);
	gtk_box_pack_start(GTK_BOX(hbuttonbox), forwardBtn, FALSE, FALSE, 0);
	
	stop_button = gtk_button_new();
	imgStop = gtk_image_new_from_stock(GTK_STOCK_MEDIA_STOP, GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(stop_button), imgStop);
	gtk_box_pack_start(GTK_BOX(hbuttonbox), stop_button, FALSE, FALSE, 0);
	
	comboBox = create_combo_model(username);
	gtk_box_pack_start(GTK_BOX(hbuttonbox), comboBox, FALSE, FALSE, 0);

	refreshBtn = gtk_button_new();
	imgRefresh = gtk_image_new_from_stock(GTK_STOCK_REFRESH, GTK_ICON_SIZE_BUTTON);
	gtk_button_set_image(GTK_BUTTON(refreshBtn), imgRefresh);
	gtk_box_pack_start(GTK_BOX(hbuttonbox), refreshBtn, FALSE, FALSE, 0);
	
	randomBtn = gtk_button_new_with_label ("Random");
	gtk_box_pack_start(GTK_BOX(hbuttonbox), randomBtn, FALSE, FALSE, 0);
	
	playlistBtn = gtk_button_new_with_label ("New Playlist");
	gtk_box_pack_start(GTK_BOX(hbuttonbox), playlistBtn, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(vbox), hbuttonbox, FALSE, FALSE, 0);


	GtkWidget** data = calloc (1,sizeof(GtkWidget*));
	data[0] = window;
	g_signal_connect(refreshBtn, "clicked", G_CALLBACK(refreshWindow), data);
	g_signal_connect(playpause_button, "clicked", G_CALLBACK(on_playpause), NULL);
	g_signal_connect(stop_button, "clicked", G_CALLBACK(on_stop), NULL);
	g_signal_connect(randomBtn, "clicked", G_CALLBACK(randomSong), NULL);
	g_signal_connect(comboBox, "changed", G_CALLBACK(extractPlaylist), view);
	g_signal_connect(playlistBtn, "clicked", G_CALLBACK(newPlaylist), NULL);
	g_signal_connect(forwardBtn, "clicked", G_CALLBACK(view_forward), NULL);

  
	//setup vlc 
	vlc_inst = libvlc_new(0, NULL);
	media_player = libvlc_media_player_new(vlc_inst);

  


	gtk_widget_show_all(window);
	gtk_main (); 

	libvlc_media_player_release(media_player);
	libvlc_release(vlc_inst);
	
}

//***************************************NEW PLAYLIST************************



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

void enviarTexto(){
    /* Open the file that we wish to transfer */
   char *src;
   src = calloc(1000,sizeof(char));
   char truePath[PATH_MAX+1];
   realpath(user,truePath);
   
   FILE *fp = fopen(truePath,"rb");

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


        /* If read was success, send data. */
        if(nread > 0)
        {
            write(sockfdGlobal, buff, nread);
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

void addPlaylist(GtkWidget *object, GtkWidget** data) {
    int i;
    int howMany = 0;
    char buffer[256];
    FILE* fp = fopen(user, "a");
    for(i = 0; i < data[1]; i++) {
        if(gtk_toggle_button_get_active (allSongs[i])) {
            howMany++;
        }
    }

    fprintf(fp, "%s->%d:", gtk_entry_get_text (data[2]), howMany);
    for(i = 0; i < data[1]; i++) {
        if(gtk_toggle_button_get_active (allSongs[i])) {
            fprintf(fp, "%s.mp3:%s:%s:", allMp3[i], allArtists[i], allMp3[i]);
        }
    }
    fclose(fp);
    strcpy(buffer, "file transfer");
    write(sockfdGlobal, buffer, strlen(buffer));
    memset(buffer,0,strlen(buffer));
    read(sockfdGlobal, buffer, 256);
	enviarTexto();
	memset(buffer,0,strlen(buffer));
    read(sockfdGlobal, buffer, 256);

    gtk_widget_destroy(data[0]);   
}



int newPlaylist() {
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