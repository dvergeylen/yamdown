#include <unistd.h>
#include "notebook.h"

/* The returned string should be freed with g_free() when no longer needed. */
static gchar* get_untitled () {
  static int c = -1;
  if (++c == 0)
    return g_strdup_printf("Untitled");
  else
    return g_strdup_printf ("Untitled-%u", c);
}


static void close_clicked(GtkWidget* btn_close __attribute__((unused)),
                          GtkNotebook *notebook) {
  GtkWidget *win;
  gint i;

  /* Close Window if latest page, close page otherwise */
  if (gtk_notebook_get_n_pages(notebook) == 1) {
    win = gtk_widget_get_parent(GTK_WIDGET(notebook));
    gtk_window_destroy(GTK_WINDOW(win));
  } else {
    i = gtk_notebook_get_current_page(notebook);
    gtk_notebook_remove_page(GTK_NOTEBOOK(notebook), i);
  }
}


static void notebook_page_build (GtkNotebook *nb, GtkWidget *pv, char *filename) {
  GtkNotebookPage *nbp;
  GtkWidget *label_box;
  GtkLabel  *label;
  GtkButton* btn_close;
  GtkStyleContext *context;
  gint i;

  label_box = yamdown_notebook_label_new();

  /* Retrieve label and assign filename content */
  label = YAMDOWN_NOTEBOOK_LABEL(label_box)->label;
  g_object_set(label, "label", filename, NULL);

  /* Retrieve close button and bind 'clicked' signal */
  btn_close  = YAMDOWN_NOTEBOOK_LABEL(label_box)->btn_close;
  g_signal_connect(btn_close, "clicked", G_CALLBACK(close_clicked), nb);

  /* Set the custom css on the close button */
  context = gtk_widget_get_style_context(GTK_WIDGET(btn_close));
  gtk_style_context_add_class(context, "small-button");
  GtkCssProvider *cssProvider = gtk_css_provider_new();
  const char *data = ".small-button {"
                      "  padding: 2px 4px;"
                      "  border-style: none;"
                      "}"
                      ".small-button:not(:hover) {"
                      "  background: none;"
                      "  border: none;"
                      "}";
  gtk_css_provider_load_from_data(cssProvider, data, -1);
  gtk_style_context_add_provider(context,
                                 GTK_STYLE_PROVIDER(cssProvider),
                                 GTK_STYLE_PROVIDER_PRIORITY_USER);

  /* Append notebook page to notebook, with label_box as page "title" */
  i = gtk_notebook_append_page (nb, pv, label_box);
  nbp = gtk_notebook_get_page (nb, pv);
  g_object_set (nbp, "tab-expand", FALSE, NULL);
  g_object_set (nbp, "tab-fill", FALSE, NULL);
  g_object_set (nbp, "detachable", TRUE, NULL);
  g_object_set(nbp, "reorderable", TRUE, NULL);

  gtk_notebook_set_current_page (nb, i);
}


/* Save the contents in the current page */
void notebook_page_save(GtkNotebook* nb) {
  g_return_if_fail(GTK_IS_NOTEBOOK(nb));

  gint i;
  GtkWidget* pv;

  i = gtk_notebook_get_current_page(nb);
  pv = gtk_notebook_get_nth_page(nb, i);
  yamdown_pane_view_save(YAMDOWN_PANE_VIEW(pv));
}


/* Opens a dialog to load a new page (in current tab) */
void notebook_page_open (GtkNotebook *nb) {
  g_return_if_fail(GTK_IS_NOTEBOOK(nb));

  GtkWidget* pv;
  char* filename;

  /* Create an empty tab */
  pv = yamdown_pane_view_new();
  filename = get_untitled();
  notebook_page_build(nb, pv, filename);

  /* Update content with dialog provided file */
  yamdown_pane_view_open(YAMDOWN_PANE_VIEW(pv));
}


/* Opens a new empty tab */
void notebook_page_new(GtkNotebook* nb) {
  g_return_if_fail(GTK_IS_NOTEBOOK(nb));

  GtkWidget* pv;
  char* filename;

  pv = yamdown_pane_view_new();
  filename = get_untitled();
  notebook_page_build(nb, pv, filename);

  const gchar* base_uri  = "file://";
  gchar* dirname = getcwd(NULL, 0);
  YAMDOWN_PANE_VIEW(pv)->base_uri = g_malloc(sizeof(gchar) * (strlen(base_uri) + strlen(dirname) + 1));
  strcpy(YAMDOWN_PANE_VIEW(pv)->base_uri, base_uri);
  strcat(YAMDOWN_PANE_VIEW(pv)->base_uri, dirname);
  g_free(dirname);

  g_print(YAMDOWN_PANE_VIEW(pv)->base_uri);
}


/* Opens a new tab and loads given file (used when filenames on cli) */
void notebook_page_new_with_file (GtkNotebook *nb, GFile *file) {
  g_return_if_fail(GTK_IS_NOTEBOOK (nb));
  g_return_if_fail(G_IS_FILE (file));

  GtkWidget* pv;
  char* filename;

  if ((pv = yamdown_pane_view_new_with_file(file)) == NULL)
    return; /* read error */

  filename = g_file_get_basename(file);
  notebook_page_build(nb, pv, filename);
}

