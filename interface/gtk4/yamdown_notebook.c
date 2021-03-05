#include "yamdown.h"

/* The returned string should be freed with g_free() when no longer needed. */
static gchar* get_untitled () {
  static int c = -1;
  if (++c == 0)
    return g_strdup_printf("Untitled");
  else
    return g_strdup_printf ("Untitled-%u", c);
}

static void file_changed (YamdownWebKitWebView *wv, GtkNotebook *nb) {
  GFile *file;
  char *filename;
  GtkWidget *scr;
  GtkWidget *label;

  file = yamdown_webkit_webview_get_file(wv);
  scr = gtk_widget_get_parent (GTK_WIDGET (wv));
  if (G_IS_FILE (file))
    filename = g_file_get_basename(file);
  else
    filename = get_untitled();

  // TODO: find the existing label instead of creating a new one
  label = gtk_label_new (filename);
  gtk_notebook_set_tab_label (nb, scr, label);
  g_object_unref(file);
  g_free (filename);
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

/* Save the contents in the current page */
void notebook_page_save(GtkNotebook *nb) {
  gint i;
  GtkWidget *wv;

  i = gtk_notebook_get_current_page (nb);
  wv = gtk_notebook_get_nth_page (nb, i);
  yamdown_webkit_webview_save (YAMDOWN_WEBKIT_WEBVIEW(wv));
}

static void notebook_page_build (GtkNotebook *nb, GtkWidget *wv, char *filename) {
  GtkNotebookPage *nbp;

  GtkWidget *boxh;
  GtkWidget *label;
  GtkWidget *btn_close;
  GtkStyleContext *context;
  gint i;

  boxh = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  label = gtk_label_new(filename);

  /* Create button and bind 'clicked' signal */
  btn_close = gtk_button_new_from_icon_name("window-close-symbolic");
  g_signal_connect(GTK_BUTTON(btn_close), "clicked", G_CALLBACK(close_clicked), nb);

  /* Set hexpand and halign values to fill space */
  gtk_widget_set_hexpand(boxh,      TRUE);
  gtk_widget_set_hexpand(label,     TRUE);
  gtk_widget_set_hexpand(btn_close, FALSE);

  gtk_widget_set_halign(label,     GTK_ALIGN_FILL);
  gtk_widget_set_halign(btn_close, GTK_ALIGN_END);

  /* Set the custom css on the close button */
  context = gtk_widget_get_style_context(btn_close);
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

  /* Append Label and close button to horizontal box */
  gtk_box_append(GTK_BOX (boxh), label);
  gtk_box_append(GTK_BOX (boxh), btn_close);

  /* Append notebook page to notebook, with horizontal box as page "title" */
  i = gtk_notebook_append_page (nb, wv, boxh);
  nbp = gtk_notebook_get_page (nb, wv);
  g_object_set (nbp, "tab-expand", TRUE, NULL);
  gtk_notebook_set_current_page (nb, i);
  g_signal_connect (YAMDOWN_WEBKIT_WEBVIEW (wv), "change-file", G_CALLBACK (file_changed), nb);
}

static void open_response (YamdownWebKitWebView* wv, gint response, GtkNotebook *nb) {
  GFile *file;
  char *filename;

  if (response != YAMDOWN_OPEN_RESPONSE_SUCCESS) {
    g_object_ref_sink (wv);
    g_object_unref (wv);
  } else if (! G_IS_FILE (file = yamdown_webkit_webview_get_file(wv))) {
    g_object_ref_sink (wv);
    g_object_unref (wv);
  } else {
    filename = g_file_get_basename (file);
    g_object_unref (file);
    notebook_page_build (nb, GTK_WIDGET(wv), filename);
  }
}

void notebook_page_open (GtkNotebook *nb) {
  g_return_if_fail(GTK_IS_NOTEBOOK (nb));

  GtkWidget *wv;

  wv = yamdown_webkit_webview_new();
  g_signal_connect (YAMDOWN_WEBKIT_WEBVIEW(wv), "open-response", G_CALLBACK (open_response), nb);
  yamdown_webkit_webview_open (YAMDOWN_WEBKIT_WEBVIEW(wv), gtk_widget_get_ancestor (GTK_WIDGET (nb), GTK_TYPE_WINDOW));
}

void notebook_page_new_with_file (GtkNotebook *nb, GFile *file) {
  g_return_if_fail(GTK_IS_NOTEBOOK (nb));
  g_return_if_fail(G_IS_FILE (file));

  GtkWidget *wv;
  char *filename;

  if ((wv = yamdown_webkit_webview_new_with_file(file)) == NULL)
    return; /* read error */
  filename = g_file_get_basename (file);
  notebook_page_build (nb, wv, filename);
}

void notebook_page_new (GtkNotebook *nb) {
  g_return_if_fail(GTK_IS_NOTEBOOK (nb));

  GtkWidget *wv;
  char *filename;

  wv = yamdown_webkit_webview_new();
  filename = get_untitled ();
  notebook_page_build(nb, wv, filename);
}

