#ifndef __YAMDOWN_SAVE_UTILS_H__
#define __YAMDOWN_SAVE_UTILS_H__

#include <gtk/gtk.h>
#include "notebook_label.h"

void update_tab_label(YamdownPaneView* pv, GFile* file) {
  char* filename;
  GtkWidget* notebook;
  GtkWidget* notebook_page;
  GtkWidget* label_box;
  GtkLabel* label;

  notebook_page = gtk_widget_get_parent(GTK_WIDGET(pv));
  notebook = gtk_widget_get_ancestor(GTK_WIDGET(pv), GTK_TYPE_NOTEBOOK);
  label_box = gtk_notebook_get_tab_label(GTK_NOTEBOOK(notebook), GTK_WIDGET(pv));
  label = YAMDOWN_NOTEBOOK_LABEL(label_box)->label;
  filename = g_file_get_basename(file);

  /* Retrieve label and assign filename content */
  g_object_set(label, "label", filename, NULL);
  // TODO: This produces a  Gtk-CRITICAL error
  // See: https://gitlab.gnome.org/GNOME/gtk/-/issues/2817
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook), notebook_page, label_box);

  /* Free */
  g_free(filename);
}

/* This functions saves a buffer in a given file, or displays an error in 'win' */
static gboolean save_file(GFile* file, GtkTextBuffer* tb, GtkWindow* win) {
  GtkTextIter start_iter;
  GtkTextIter end_iter;
  gchar* contents;
  gboolean stat;
  GtkWidget* message_dialog;
  GError* err = NULL;

  gtk_text_buffer_get_bounds(tb, &start_iter, &end_iter);
  contents = gtk_text_buffer_get_text(tb, &start_iter, &end_iter, FALSE);
  if (g_file_replace_contents(file, contents, strlen(contents), NULL, TRUE,
                              G_FILE_CREATE_NONE, NULL, NULL, &err)) {
    gtk_text_buffer_set_modified(tb, FALSE);
    stat = TRUE;
  } else {
    message_dialog =
        gtk_message_dialog_new(win, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR,
                               GTK_BUTTONS_CLOSE, "%s.\n", err->message);
    g_signal_connect(message_dialog, "response", G_CALLBACK(gtk_window_destroy),
                     NULL);
    gtk_widget_show(message_dialog);
    g_error_free(err);
    stat = FALSE;
  }
  g_free(contents);
  return stat;
}

/* This function is a callback for a save as dialog response.
 * It save the sourceview buffer into the chosen file and emits a 'CHANGE_FILE'
 * signal to allow update of tab's Label
 */
static void saveas_dialog_response(GtkWidget* dialog,
                                   gint response,
                                   YamdownPaneView* pv) {
  GtkTextView* sourceview = pv->sourceview;
  GtkTextBuffer* tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(sourceview));
  GtkWidget* win = gtk_widget_get_ancestor(GTK_WIDGET(pv), GTK_TYPE_WINDOW);
  GFile* file;

  if (response == GTK_RESPONSE_ACCEPT) {
    file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog));
    if (!G_IS_FILE(file))
      g_warning(
          "YamdownPaneView: gtk_file_chooser_get_file returns non GFile.\n");
    else if (save_file(file, tb, GTK_WINDOW(win))) {
      if (G_IS_FILE(pv->file))
        g_object_unref(pv->file);
      pv->file = file;

      /* Update Tab's Label */
      update_tab_label(pv, file);
    } else
      g_object_unref(file);
  }
  gtk_window_destroy(GTK_WINDOW(dialog));
}

#endif /* __YAMDOWN_SAVE_UTILS_H__ */