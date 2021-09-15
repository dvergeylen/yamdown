#ifndef __YAMDOWN_DIALOG_RESPONSE_CALLBACKS_H__
#define __YAMDOWN_DIALOG_RESPONSE_CALLBACKS_H__

#include <gtk/gtk.h>
#include "paneview.h"
#include "utils.h"

static void open_dialog_response(GtkWidget* dialog,
                                 gint response,
                                 YamdownPaneView* pv) {
  GFile* file;
  char* contents;
  gsize length;
  GtkWidget* message_dialog;
  GError* err = NULL;
  GtkTextView* sourceview;
  GtkTextBuffer* tb;

  if ((response == GTK_RESPONSE_ACCEPT) &&
      G_IS_FILE(file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog)))) {
    if (!g_file_load_contents(file, NULL, &contents, &length, NULL,
                              &err)) { /* read error */
      if (G_IS_FILE(file))
        g_object_unref(file);
      message_dialog = gtk_message_dialog_new(
          GTK_WINDOW(dialog), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR,
          GTK_BUTTONS_CLOSE, "%s.\n", err->message);
      g_signal_connect(message_dialog, "response",
                       G_CALLBACK(gtk_window_destroy), NULL);
      gtk_widget_show(message_dialog);
      g_error_free(err);
    } else {
      /* Retrieve sourceview */
      sourceview = pv->sourceview;
      tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(sourceview));

      /* Set text to textview's buffer */
      gtk_text_buffer_set_text(tb, contents, length);
      gtk_text_buffer_set_modified(tb, FALSE);
      g_free(contents);

      if (G_IS_FILE(pv->file))
        g_object_unref(pv->file);
      pv->file = g_file_dup(file);

      /* Update Tab's Label */
      update_tab_label(pv, file);
    }
  }
  gtk_window_destroy(GTK_WINDOW(dialog));
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

#endif /* YAMDOWN_DIALOG_RESPONSE_CALLBACKS_H */