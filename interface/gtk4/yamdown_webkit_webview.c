#include "yamdown.h"

struct _YamdownWebKitWebView {
  WebKitWebView parent;
  GFile *file;
  GtkTextBuffer *buffer;
};

G_DEFINE_TYPE (YamdownWebKitWebView, yamdown_webkit_webview, WEBKIT_TYPE_WEB_VIEW)

enum {
  CHANGE_FILE,
  OPEN_RESPONSE,
  NUMBER_OF_SIGNALS
};

static guint yamdown_webkit_webview_signals[NUMBER_OF_SIGNALS];


static void yamdown_webkit_webview_dispose(GObject* gobject) {
  YamdownWebKitWebView *wv = YAMDOWN_WEBKIT_WEBVIEW (gobject);

  if (G_IS_FILE(wv->file))
    g_clear_object(&wv->file);

  g_object_unref(wv->buffer);

  G_OBJECT_CLASS (yamdown_webkit_webview_parent_class)->dispose (gobject);
}


static void yamdown_webkit_webview_init(YamdownWebKitWebView *wv) {
  wv->file = NULL;
  wv->buffer = gtk_text_buffer_new(NULL);
  gtk_text_buffer_set_modified(wv->buffer, FALSE);
}


static void yamdown_webkit_webview_class_init(YamdownWebKitWebViewClass* class) {
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  object_class->dispose = yamdown_webkit_webview_dispose;
  yamdown_webkit_webview_signals[CHANGE_FILE] = g_signal_newv ("change-file",
                                 G_TYPE_FROM_CLASS (class),
                                 G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                                 NULL /* closure */,
                                 NULL /* accumulator */,
                                 NULL /* accumulator data */,
                                 NULL /* C marshaller */,
                                 G_TYPE_NONE /* return_type */,
                                 0     /* n_params */,
                                 NULL  /* param_types */);
  GType param_types[] = {G_TYPE_INT}; 
  yamdown_webkit_webview_signals[OPEN_RESPONSE] = g_signal_newv ("open-response",
                                 G_TYPE_FROM_CLASS (class),
                                 G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                                 NULL /* closure */,
                                 NULL /* accumulator */,
                                 NULL /* accumulator data */,
                                 NULL /* C marshaller */,
                                 G_TYPE_NONE /* return_type */,
                                 1     /* n_params */,
                                 param_types);
}


GFile *yamdown_webkit_webview_get_file(YamdownWebKitWebView* wv) {
  g_return_val_if_fail(YAMDOWN_IS_WEBKIT_WEBVIEW(wv), NULL);

  return g_file_dup(wv->file);
}


static void open_dialog_response(GtkWidget *dialog, gint response, YamdownWebKitWebView *wv) {
  GtkTextBuffer *tb = wv->buffer;
  GFile *file;
  char *contents;
  gsize length;
  GtkWidget *message_dialog;
  GError *err = NULL;

  if (response != GTK_RESPONSE_ACCEPT)
    g_signal_emit (wv, yamdown_webkit_webview_signals[OPEN_RESPONSE], 0, YAMDOWN_OPEN_RESPONSE_CANCEL);
  else if (! G_IS_FILE (file = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (dialog))))
    g_signal_emit (wv, yamdown_webkit_webview_signals[OPEN_RESPONSE], 0, YAMDOWN_OPEN_RESPONSE_ERROR);
  else if (! g_file_load_contents (file, NULL, &contents, &length, NULL, &err)) { /* read error */
    if (G_IS_FILE (file))
      g_object_unref (file);
    message_dialog = gtk_message_dialog_new (GTK_WINDOW (dialog), GTK_DIALOG_MODAL,
                                             GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                            "%s.\n", err->message);
    g_signal_connect (message_dialog, "response", G_CALLBACK (gtk_window_destroy), NULL);
    gtk_widget_show (message_dialog);
    g_error_free (err);
    g_signal_emit (wv, yamdown_webkit_webview_signals[OPEN_RESPONSE], 0, YAMDOWN_OPEN_RESPONSE_ERROR);
  } else {
    gtk_text_buffer_set_text (tb, contents, length);
    g_free (contents);
    if (G_IS_FILE (wv->file))
      g_object_unref (wv->file);
    wv->file = file;
    gtk_text_buffer_set_modified (tb, FALSE);
    g_signal_emit (wv, yamdown_webkit_webview_signals[OPEN_RESPONSE], 0, YAMDOWN_OPEN_RESPONSE_SUCCESS);
  }
  gtk_window_destroy (GTK_WINDOW (dialog));
}


void yamdown_webkit_webview_open(YamdownWebKitWebView *wv, GtkWidget *win) {
  g_return_if_fail (YAMDOWN_IS_WEBKIT_WEBVIEW (wv));
  g_return_if_fail (GTK_IS_WINDOW (win));

  GtkWidget *dialog;

  dialog = gtk_file_chooser_dialog_new ("Open file", GTK_WINDOW (win), GTK_FILE_CHOOSER_ACTION_OPEN,
                                        "Cancel", GTK_RESPONSE_CANCEL,
                                        "Open", GTK_RESPONSE_ACCEPT,
                                        NULL);
  g_signal_connect (dialog, "response", G_CALLBACK (open_dialog_response), wv);
  gtk_widget_show (dialog);
}


static void saveas_dialog_response (GtkWidget *dialog, gint response, YamdownWebKitWebView *wv) {
  GtkTextBuffer *tb = wv->buffer;
  GFile *file;

  if (response == GTK_RESPONSE_ACCEPT) {
    file = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (dialog));
    if (G_IS_FILE(file)) {
      wv->file = file;
      gtk_text_buffer_set_modified (tb, TRUE);
      g_signal_emit (wv, yamdown_webkit_webview_signals[CHANGE_FILE], 0);
      yamdown_webkit_webview_save(YAMDOWN_WEBKIT_WEBVIEW(wv));
    }
  }
  gtk_window_destroy (GTK_WINDOW (dialog));
}


void yamdown_webkit_webview_save(YamdownWebKitWebView *wv) {
  g_return_if_fail (YAMDOWN_IS_WEBKIT_WEBVIEW (wv));

  GtkTextBuffer *tb = wv->buffer;
  GtkTextIter start_iter;
  GtkTextIter end_iter;
  gchar *contents;
  GtkWidget *message_dialog;
  GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (wv), GTK_TYPE_WINDOW);
  GError *err = NULL;

  if (! gtk_text_buffer_get_modified (tb))
    return; /* no necessary to save it */
  else if (wv->file == NULL)
    yamdown_webkit_webview_saveas(wv);
  else {
    gtk_text_buffer_get_bounds (tb, &start_iter, &end_iter);
    contents = gtk_text_buffer_get_text (tb, &start_iter, &end_iter, FALSE);
    if (g_file_replace_contents (wv->file, contents, strlen (contents), NULL, TRUE, G_FILE_CREATE_NONE, NULL, NULL, &err))
      gtk_text_buffer_set_modified (tb, FALSE);
    else {
/* It is possible that wv->file is broken. */
/* It is a good idea to set wv->file to NULL. */
      if (G_IS_FILE (wv->file))
        g_object_unref (wv->file);
      wv->file =NULL;
      g_signal_emit (wv, yamdown_webkit_webview_signals[CHANGE_FILE], 0);
      gtk_text_buffer_set_modified (tb, TRUE);
      message_dialog = gtk_message_dialog_new (GTK_WINDOW (win), GTK_DIALOG_MODAL,
                                               GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                              "%s.\n", err->message);
      g_signal_connect (message_dialog, "response", G_CALLBACK (gtk_window_destroy), NULL);
      gtk_widget_show (message_dialog);
      g_error_free (err);
    }
  }
}


void yamdown_webkit_webview_saveas(YamdownWebKitWebView *wv) {
  g_return_if_fail (YAMDOWN_IS_WEBKIT_WEBVIEW (wv));

  GtkWidget *dialog;
  GtkFileChooser *chooser;
  GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (wv), GTK_TYPE_WINDOW);

  dialog = gtk_file_chooser_dialog_new ("Save file", GTK_WINDOW (win),
                                        GTK_FILE_CHOOSER_ACTION_SAVE,
                                        "_Cancel", GTK_RESPONSE_CANCEL,
                                        "_Save", GTK_RESPONSE_ACCEPT,
                                      NULL);  
  chooser = GTK_FILE_CHOOSER (dialog);
  gtk_file_chooser_set_current_name(chooser, "Untitled document");

  g_signal_connect (dialog, "response", G_CALLBACK (saveas_dialog_response), wv);
  gtk_widget_show (dialog);
}


GtkWidget* yamdown_webkit_webview_new_with_file(GFile *file) {
  g_return_val_if_fail (G_IS_FILE (file), NULL);

  GtkWidget *wv;
  GtkTextBuffer *tb;
  char *contents;
  gsize length;

  if (! g_file_load_contents (file, NULL, &contents, &length, NULL, NULL)) /* read error */
    return NULL;

  wv = yamdown_webkit_webview_new();
  tb = YAMDOWN_WEBKIT_WEBVIEW(wv)->buffer;
  gtk_text_buffer_set_text (tb, contents, length);
  g_free (contents);
  YAMDOWN_WEBKIT_WEBVIEW(wv)->file = g_file_dup (file);
  return wv;
}


GtkWidget* yamdown_webkit_webview_new (void) {
  GtkWidget *wv = GTK_WIDGET (g_object_new (YAMDOWN_TYPE_WEBKIT_WEBVIEW, NULL));
  WebKitSettings *settings = webkit_web_view_get_settings(WEBKIT_WEB_VIEW(wv));

  webkit_settings_set_enable_javascript(settings, true);
  webkit_settings_set_javascript_can_access_clipboard(settings, true);
  webkit_settings_set_allow_file_access_from_file_urls(settings, true);

  /* DEBUG */
  webkit_settings_set_enable_write_console_messages_to_stdout(settings, true);
  webkit_settings_set_enable_developer_extras(settings, true);
  /* END OF DEBUG */

  webkit_web_view_set_settings(WEBKIT_WEB_VIEW(wv), settings);
  webkit_web_view_load_uri(WEBKIT_WEB_VIEW(wv), "file:///media/D/projets/Lab/yamdown/html/index.html");

  return wv;
}
