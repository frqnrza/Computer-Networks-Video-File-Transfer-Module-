#include <gtk/gtk.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024


typedef struct {
   GtkWidget *window;
   GtkWidget *progress_bar;
   GtkWidget *status_label;
   char *save_directory;
} AppWidgets;


void receive_file(AppWidgets *app) {
   int sockfd, new_sockfd;
   struct sockaddr_in server_addr, client_addr;
   socklen_t client_len;
   char buffer[BUFFER_SIZE];
   FILE *file;
   long total_received = 0;
   int received_bytes;


   // Setup socket
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd < 0) {
       gtk_label_set_text(GTK_LABEL(app->status_label), "Socket creation failed!");
       return;
   }


   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = INADDR_ANY;
   server_addr.sin_port = htons(PORT);


   if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
       gtk_label_set_text(GTK_LABEL(app->status_label), "Binding failed!");
       close(sockfd);
       return;
   }


   listen(sockfd, 1);
   gtk_label_set_text(GTK_LABEL(app->status_label), "Waiting for sender...");


   client_len = sizeof(client_addr);
   new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
   if (new_sockfd < 0) {
       gtk_label_set_text(GTK_LABEL(app->status_label), "Connection failed!");
       close(sockfd);
       return;
   }


   // Open file to write
   char filepath[256];
   snprintf(filepath, sizeof(filepath), "%s/received_video.mp4", app->save_directory);
   file = fopen(filepath, "wb");
   if (!file) {
       gtk_label_set_text(GTK_LABEL(app->status_label), "Failed to open file for writing!");
       close(new_sockfd);
       close(sockfd);
       return;
   }


   gtk_label_set_text(GTK_LABEL(app->status_label), "Receiving file...");
   while ((received_bytes = recv(new_sockfd, buffer, BUFFER_SIZE, 0)) > 0) {
       if (strncmp(buffer, "EOF", 3) == 0) break;


       fwrite(buffer, 1, received_bytes, file);
       total_received += received_bytes;


       gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(app->progress_bar), 0.5);  // Dummy progress bar
       while (gtk_events_pending()) gtk_main_iteration();
   }


   gtk_label_set_text(GTK_LABEL(app->status_label), "File received successfully!");
   fclose(file);
   close(new_sockfd);
   close(sockfd);
}


void on_receive_clicked(GtkWidget *widget, gpointer data) {
   AppWidgets *app = (AppWidgets *)data;
   app->save_directory = g_get_home_dir();  // Hardcoded directory


   receive_file(app);
}


int main(int argc, char *argv[]) {
   gtk_init(&argc, &argv);


   AppWidgets *app = g_malloc(sizeof(AppWidgets));


   app->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
   gtk_window_set_title(GTK_WINDOW(app->window), "Receiver");
   gtk_container_set_border_width(GTK_CONTAINER(app->window), 10);


   GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
   gtk_container_add(GTK_CONTAINER(app->window), vbox);


   GtkWidget *receive_button = gtk_button_new_with_label("Start Receiving");
   g_signal_connect(receive_button, "clicked", G_CALLBACK(on_receive_clicked), app);
   gtk_box_pack_start(GTK_BOX(vbox), receive_button, FALSE, FALSE, 0);


   app->progress_bar = gtk_progress_bar_new();
   gtk_box_pack_start(GTK_BOX(vbox), app->progress_bar, FALSE, FALSE, 0);


   app->status_label = gtk_label_new("Click to start receiving.");
   gtk_box_pack_start(GTK_BOX(vbox), app->status_label, FALSE, FALSE, 0);


   gtk_widget_show_all(app->window);
   g_signal_connect(app->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);


   gtk_main();
   g_free(app);


   return 0;
}
