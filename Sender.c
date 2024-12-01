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
    GtkWidget *file_chooser;
    GtkWidget *ip_entry;
    GtkWidget *progress_bar;
    GtkWidget *percentage_label;
    GtkWidget *status_label;
    char *file_path;
    char *server_ip;
    long total_file_size;
} AppWidgets;

long get_file_size(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) return -1;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);

    return size;
}

void send_file(AppWidgets *app) {
    int sockfd;
    struct sockaddr_in server_addr;
    FILE *file;
    char buffer[BUFFER_SIZE];
    long bytes_sent = 0;
    int read_bytes;

    // Get file size
    file = fopen(app->file_path, "rb");
    if (!file) {
        gtk_label_set_text(GTK_LABEL(app->status_label), "Failed to open file!");
        return;
    }
    app->total_file_size = get_file_size(app->file_path);

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        gtk_label_set_text(GTK_LABEL(app->status_label), "Socket creation failed!");
        fclose(file);
        return;
    }

    // Setup server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, app->server_ip, &server_addr.sin_addr) <= 0) {
        gtk_label_set_text(GTK_LABEL(app->status_label), "Invalid IP address!");
        close(sockfd);
        fclose(file);
        return;
    }

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        gtk_label_set_text(GTK_LABEL(app->status_label), "Connection failed!");
        close(sockfd);
        fclose(file);
        return;
    }

    // Send file contents
    gtk_label_set_text(GTK_LABEL(app->status_label), "Sending file...");
    while ((read_bytes = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        send(sockfd, buffer, read_bytes, 0);
        bytes_sent += read_bytes;

        // Update progress bar and percentage
        double fraction = (double)bytes_sent / app->total_file_size;
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(app->progress_bar), fraction);

        char percentage[20];
        snprintf(percentage, sizeof(percentage), "%.2f%%", fraction * 100);
        gtk_label_set_text(GTK_LABEL(app->percentage_label), percentage);

        while (gtk_events_pending()) gtk_main_iteration();
    }

    // Send EOF marker
    strcpy(buffer, "EOF");
    send(sockfd, buffer, strlen(buffer), 0);

    gtk_label_set_text(GTK_LABEL(app->status_label), "File sent successfully!");
    fclose(file);
    close(sockfd);
}

void on_send_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *app = (AppWidgets *)data;
    app->file_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(app->file_chooser));
    app->server_ip = (char *)gtk_entry_get_text(GTK_ENTRY(app->ip_entry));

    if (!app->file_path || strlen(app->server_ip) == 0) {
        gtk_label_set_text(GTK_LABEL(app->status_label), "Select file and enter IP!");
        return;
    }

    send_file(app);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    AppWidgets *app = g_malloc(sizeof(AppWidgets));

    app->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(app->window), "Sender");
    gtk_container_set_border_width(GTK_CONTAINER(app->window), 10);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(app->window), vbox);

    app->file_chooser = gtk_file_chooser_button_new("Choose File", GTK_FILE_CHOOSER_ACTION_OPEN);
    gtk_box_pack_start(GTK_BOX(vbox), app->file_chooser, FALSE, FALSE, 0);

    GtkWidget *ip_label = gtk_label_new("Enter Receiver IP:");
    gtk_box_pack_start(GTK_BOX(vbox), ip_label, FALSE, FALSE, 0);

    app->ip_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), app->ip_entry, FALSE, FALSE, 0);

    GtkWidget *send_button = gtk_button_new_with_label("Send File");
    g_signal_connect(send_button, "clicked", G_CALLBACK(on_send_clicked), app);
    gtk_box_pack_start(GTK_BOX(vbox), send_button, FALSE, FALSE, 0);

    app->progress_bar = gtk_progress_bar_new();
    gtk_box_pack_start(GTK_BOX(vbox), app->progress_bar, FALSE, FALSE, 0);

    app->percentage_label = gtk_label_new("0.00%");
    gtk_box_pack_start(GTK_BOX(vbox), app->percentage_label, FALSE, FALSE, 0);

    app->status_label = gtk_label_new("Select file and enter IP to send.");
    gtk_box_pack_start(GTK_BOX(vbox), app->status_label, FALSE, FALSE, 0);

    gtk_widget_show_all(app->window);
    g_signal_connect(app->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_main();
    g_free(app);

    return 0;
}