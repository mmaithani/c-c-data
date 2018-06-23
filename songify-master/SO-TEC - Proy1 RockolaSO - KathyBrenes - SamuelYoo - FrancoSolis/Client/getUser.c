/**
* compile with the following command
* gcc -Wall -g getUser.c -o getUser `pkg-config --cflags gtk+-2.0` `pkg-config --libs gtk+-2.0`
*/

#include <gtk/gtk.h>
char username[100];

void setName(GtkWidget *object, GtkWidget** data) {
    strcpy(username,gtk_entry_get_text(GTK_ENTRY(data[0])));
    
    gtk_widget_destroy(data[1]);

}

void on_window_destroy (GtkWidget *object, gpointer data) {
    gtk_main_quit();
}//end on window destroy


int getUser() {
    GtkWidget *window;
    GtkWidget *label;
    GtkWidget *entry;
    GtkWidget *box;
    GtkWidget *btnLogin;

    GdkColor color;
    color.red = 0000;
    color.green = 0000;
    color.blue = 0;

    GdkColor color2;
    color2.red = 47;
    color2.green = 79;
    color2.blue = 79;

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
    label = gtk_label_new ("Escriba su nombre de usuario");
    gtk_box_pack_start (GTK_BOX (box), label, TRUE, FALSE, 5);
    gtk_widget_show(label);

     // Create an entry field and add it to the box
    entry = gtk_entry_new_with_max_length(10);
    gtk_box_pack_start (GTK_BOX (box), entry, TRUE, FALSE, 5);
    gtk_widget_show(entry);
    gtk_widget_grab_focus(entry);

    btnLogin = gtk_button_new_with_label("Iniciar");
    gtk_box_pack_start(GTK_BOX(box), btnLogin, TRUE, FALSE, 5);
    gtk_widget_show(btnLogin);

    gdk_color_parse ("#ffffff", &color);
    gtk_widget_modify_text(GTK_WIDGET(btnLogin),GTK_STATE_NORMAL, &color);  
    g_signal_connect (window, "destroy", G_CALLBACK (on_window_destroy), NULL);
    gtk_widget_set_tooltip_text (btnLogin, "Proporcione los datos indicados para realizar el inicio de sesión. ");
   
    GtkWidget** data = calloc (2,sizeof(GtkWidget*));
    data[0] = entry;
    data[1] = window;
    g_signal_connect (btnLogin, "clicked",G_CALLBACK (setName),data);

    gtk_main ();
    
    return 0;
}
