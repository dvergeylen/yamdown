#include "yamdown.h"

/* ---- interface helpers: new, open, save BUTTONS ---- */
static void new_clicked(GtkWidget* btn_new __attribute__((unused)),
                        GtkNotebook *notebook) {
  notebook_page_new(notebook);
}

static void open_clicked(GtkWidget* btn_open __attribute__((unused)),
                         GtkNotebook *notebook) {
  notebook_page_open(notebook);
}

static void save_clicked(GtkWidget* btn_save __attribute__((unused)),
                         GtkNotebook *notebook) {
  notebook_page_save(notebook);
}

/* N.B: 'close_clicked' is handled at the yamdown_notebook level */

/* ---- end of interface helpers ---- */


/* ---- events helpers: startup, activate, open EVENTS ---- */
static void app_activate (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION(application);
  GtkWidget *win;
  GtkNotebook *notebook;
  GtkWidget *notebook_widget;

  win = GTK_WIDGET(gtk_application_get_active_window(app));
  notebook_widget = gtk_widget_get_first_child(win);

  // order is not guaranteed
  while(!GTK_IS_NOTEBOOK(notebook_widget)) {
    notebook_widget = gtk_widget_get_next_sibling(notebook_widget);
  }
  notebook = GTK_NOTEBOOK(notebook_widget);

  notebook_page_new(notebook);
}

static void app_open(GApplication *application, GFile **files,
                    gint n_files, const gchar *hint __attribute__((unused))) {
  GtkApplication *app = GTK_APPLICATION(application);
  GtkWidget *win;
  GtkNotebook *notebook;
  GtkWidget *notebook_widget;
  int i;

  win = GTK_WIDGET(gtk_application_get_active_window(app));
  notebook_widget = gtk_widget_get_first_child(win);

  // order is not guaranteed
  while(!GTK_IS_NOTEBOOK(notebook_widget)) {
    notebook_widget = gtk_widget_get_next_sibling(notebook_widget);
  }
  notebook = GTK_NOTEBOOK(notebook_widget);

  for (i = 0; i < n_files; i++) {
    notebook_page_new_with_file(notebook, files[i]);
  }
  if (gtk_notebook_get_n_pages(notebook) == 0) {
    notebook_page_new(notebook);
  }
}

static void app_startup (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION(application);
  GtkWidget *win;
  GtkBuilder *build;
  GtkNotebook *notebook;
  GtkButton *btn_new;
  GtkButton *btn_open;
  GtkButton *btn_save;

  /* Loading interface from gresource file */
  build = gtk_builder_new_from_resource("/com/github/dvergeylen/yamdown/yamdown.ui");
  win = GTK_WIDGET(gtk_builder_get_object(build, "win"));

  /* Linking main window to interface */
  gtk_window_set_application(GTK_WINDOW(win), GTK_APPLICATION(app));

  /* Retrieving buttons from UI */
  btn_new   = GTK_BUTTON(gtk_builder_get_object(build, "btn-new"));
  btn_open  = GTK_BUTTON(gtk_builder_get_object(build, "btn-open"));
  btn_save  = GTK_BUTTON(gtk_builder_get_object(build, "btn-save"));
  notebook  = GTK_NOTEBOOK(gtk_builder_get_object(build, "notebook"));

  /* Linking buttons to appropriate functions */
  g_signal_connect(btn_new,  "clicked", G_CALLBACK(new_clicked),  notebook);
  g_signal_connect(btn_open, "clicked", G_CALLBACK(open_clicked), notebook);
  g_signal_connect(btn_save, "clicked", G_CALLBACK(save_clicked), notebook);

  g_object_unref(build);
  gtk_widget_show (win);
}
/* ---- end of event helpers ---- */


int application_new(int argc, char **argv) {
  GtkApplication *app;
  int stat;

  /* Instanciate a new GTK application,
   * flag specifies app can open text files */
  app = gtk_application_new("com.github.dvergeylen.yamdown",
                            G_APPLICATION_HANDLES_OPEN);

  /* Connect callbacks */
  g_signal_connect(app, "startup", G_CALLBACK(app_startup), NULL);
  g_signal_connect(app, "activate", G_CALLBACK(app_activate), NULL);
  g_signal_connect(app, "open", G_CALLBACK(app_open), NULL);

  /* Run */
  stat = g_application_run(G_APPLICATION(app), argc, argv);

  /* Release and return */
  g_object_unref(app);
  return stat;
}
