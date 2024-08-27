#include <gtk/gtk.h>
#include <stdbool.h>

#define GRID_SIZE 3
#define PLAYERS 2
typedef enum { PLAYER_X, PLAYER_O } Player;

static Player current_player = PLAYER_X;
static GtkWidget *buttons[GRID_SIZE][GRID_SIZE];  // Grille des boutons
static const gchar *player_symbols[] = {"X", "O"};
static const gchar *player_colors[] = {"red", "blue"};

static void on_about_activate() {
    GtkWidget *dialog = gtk_message_dialog_new(NULL,
        GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
        "TicTacToe Version 1.0\n\nAuteur: Yves Chevallier");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

static void on_quit_activate() {
    gtk_main_quit();
}

static void on_restart_activate(GtkMenuItem *menuitem, gpointer user_data) {
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            gtk_button_set_label(GTK_BUTTON(buttons[i][j]), "");
            gtk_widget_set_sensitive(buttons[i][j], true);
        }
    }
    current_player = PLAYER_X;
}

static gboolean check_line(int start_row, int start_col, int delta_row, int delta_col) {
    const gchar *label = gtk_button_get_label(GTK_BUTTON(buttons[start_row][start_col]));
    if (label == NULL || g_strcmp0(label, "") == 0) {
        return false;
    }

    for (int i = 1; i < GRID_SIZE; i++) {
        int row = start_row + i * delta_row;
        int col = start_col + i * delta_col;
        if (g_strcmp0(label, gtk_button_get_label(GTK_BUTTON(buttons[row][col]))) != 0) {
            return false;
        }
    }

    return true;
}

static gboolean check_winner() {
    // Lines/Columns
    for (int i = 0; i < GRID_SIZE; i++)
        if (check_line(i, 0, 0, 1) || check_line(0, i, 1, 0))
            return true;

    // Diagonals
    if (check_line(0, 0, 1, 1) || check_line(0, GRID_SIZE - 1, 1, -1))
        return true;

    return false;
}

static void on_button_clicked(GtkButton *button, gpointer user_data) {
    // Styliser le texte avec du HTML
    gchar *markup = g_markup_printf_escaped("<span font_desc='20' weight='bold' foreground='%s'>%s</span>", player_colors[current_player], player_symbols[current_player]);
    gtk_button_set_label(button, player_symbols[current_player]);
    gtk_label_set_markup(GTK_LABEL(gtk_bin_get_child(GTK_BIN(button))), markup);
    g_free(markup);

    gtk_widget_set_sensitive(GTK_WIDGET(button), false);  // Désactiver le bouton après clic

    // Vérifier s'il y a un gagnant
    if (check_winner()) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Le joueur %s a gagné!", player_symbols[current_player]);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        on_restart_activate(NULL, NULL);  // Redémarrer le jeu après l'annonce
        return;
    }

    // Passer au joueur suivant
    current_player = 1 - current_player;
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *menubar;
    GtkWidget *fileMenu;
    GtkWidget *fileMenuItem;
    GtkWidget *aboutMenuItem;
    GtkWidget *quitMenuItem;
    GtkWidget *restartMenuItem;
    GtkWidget *grid;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "TicTacToe");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    menubar = gtk_menu_bar_new();
    fileMenu = gtk_menu_new();

    fileMenuItem = gtk_menu_item_new_with_label("Fichier");
    aboutMenuItem = gtk_menu_item_new_with_label("À propos");
    quitMenuItem = gtk_menu_item_new_with_label("Quitter");
    restartMenuItem = gtk_menu_item_new_with_label("Redémarrer");

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileMenuItem), fileMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), aboutMenuItem);
    gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), restartMenuItem);
    gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), quitMenuItem);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), fileMenuItem);

    gtk_box_pack_start(GTK_BOX(vbox), menubar, false, false, 0);

    grid = gtk_grid_new();
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), true);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), true);
    gtk_box_pack_start(GTK_BOX(vbox), grid, true, true, 0);

    // Créer les boutons de la grille
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            buttons[i][j] = gtk_button_new_with_label("");
            gtk_grid_attach(GTK_GRID(grid), buttons[i][j], j, i, 1, 1);
            g_signal_connect(buttons[i][j], "clicked", G_CALLBACK(on_button_clicked), NULL);
        }
    }

    g_signal_connect(aboutMenuItem, "activate", G_CALLBACK(on_about_activate), NULL);
    g_signal_connect(quitMenuItem, "activate", G_CALLBACK(on_quit_activate), NULL);
    g_signal_connect(restartMenuItem, "activate", G_CALLBACK(on_restart_activate), NULL);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.example.tictactoe", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
