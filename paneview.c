#include "paneview.h"
#include "dialog_response_callbacks.h"
#include "utils.h"

G_DEFINE_TYPE (YamdownPaneView, yamdown_pane_view, GTK_TYPE_BOX)


static void yamdown_pane_view_dispose(GObject* gobject) {
  YamdownPaneView* pv = YAMDOWN_PANE_VIEW(gobject);

  if (G_IS_FILE(pv->file))
    g_clear_object(&pv->file);

  G_OBJECT_CLASS(yamdown_pane_view_parent_class)->dispose(gobject);
}


static void yamdown_pane_view_init (YamdownPaneView *paneview) {
  gtk_widget_init_template (GTK_WIDGET (paneview));

  GtkTextBuffer* tb = gtk_text_view_get_buffer(paneview->sourceview);
  g_signal_connect(tb, "changed", G_CALLBACK(key_commit_cb), paneview);
}


static void yamdown_pane_view_class_init (YamdownPaneViewClass *class) {
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  object_class->dispose = yamdown_pane_view_dispose;
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/com/github/dvergeylen/yamdown/paneview.ui");

  // Bind the sourceview and webview variables to the child objects of the XML.
  // id and variable names must match, as gtk_widget_class_bind_template_child is a macro
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), YamdownPaneView, sourceview);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), YamdownPaneView, webview);
}


/* This function saves a given tab content to a user chosen file (via dialog) */
void yamdown_pane_view_saveas(YamdownPaneView* pv) {
  g_return_if_fail(YAMDOWN_IS_PANE_VIEW(pv));

  GtkWidget* dialog;
  GtkWidget* win = gtk_widget_get_ancestor(GTK_WIDGET(pv), GTK_TYPE_WINDOW);

  dialog = gtk_file_chooser_dialog_new(
    "Save file", GTK_WINDOW(win), GTK_FILE_CHOOSER_ACTION_SAVE,
    "Cancel", GTK_RESPONSE_CANCEL,
    "Save", GTK_RESPONSE_ACCEPT, NULL);
  g_signal_connect(dialog, "response", G_CALLBACK(saveas_dialog_response), pv);
  gtk_widget_show(dialog);
}

/* This function saves a given tab content to file (calls 'save_file()' helper) */
void yamdown_pane_view_save(YamdownPaneView* pv) {
  g_return_if_fail(YAMDOWN_IS_PANE_VIEW(pv));

  GtkTextView* sourceview = pv->sourceview;
  GtkTextBuffer* tb = gtk_text_view_get_buffer(sourceview);
  GtkWidget* win = gtk_widget_get_ancestor(GTK_WIDGET(pv), GTK_TYPE_WINDOW);

  if (!gtk_text_buffer_get_modified(tb))
    return; /* no need to save it */
  else if (pv->file == NULL)
    yamdown_pane_view_saveas(pv);
  else if (!G_IS_FILE(pv->file))
    g_error("YamdownPaneView: The pointer pv->file isn't NULL nor GFile.\n");
  else
    save_file(pv->file, tb, GTK_WINDOW(win));
}


GtkWidget* yamdown_pane_view_new() {
  return GTK_WIDGET (g_object_new (YAMDOWN_TYPE_PANE_VIEW, NULL));
}


GtkWidget* yamdown_pane_view_new_with_file(GFile* file) {
  g_return_val_if_fail(G_IS_FILE(file), NULL);

  GtkWidget* pv;
  GtkTextView* sourceview;
  GtkTextBuffer* tb;
  char* contents;
  gsize length;

  if (!g_file_load_contents(file, NULL, &contents, &length, NULL, NULL)) /* read error */
    return NULL;

  if ((pv = yamdown_pane_view_new()) != NULL) {
    sourceview = YAMDOWN_PANE_VIEW(pv)->sourceview;
    tb = gtk_text_view_get_buffer(sourceview);
    YAMDOWN_PANE_VIEW(pv)->file = g_file_dup(file);

    /* Set text to textview's buffer */
    gtk_text_buffer_set_text(tb, contents, length);
    gtk_text_buffer_set_modified(tb, FALSE);

    // DEBUG --------------------------
    sourceview = YAMDOWN_PANE_VIEW(pv)->webview;
    tb = gtk_text_view_get_buffer(sourceview);
    /* Set text to textview's buffer */
    gtk_text_buffer_set_text(tb, contents, length);
    gtk_text_buffer_set_modified(tb, FALSE);
    // END OF DEBUG -------------------
  }
  g_free(contents);
  return pv;
}


void yamdown_pane_view_open(YamdownPaneView* pv){
  g_return_if_fail(YAMDOWN_IS_PANE_VIEW(pv));

  GtkWidget* win = gtk_widget_get_ancestor(GTK_WIDGET(pv), GTK_TYPE_WINDOW);
  GtkWidget* dialog;

  dialog = gtk_file_chooser_dialog_new(
      "Open file", GTK_WINDOW(win), GTK_FILE_CHOOSER_ACTION_OPEN, "Cancel",
      GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL);

  g_signal_connect(dialog, "response", G_CALLBACK(open_dialog_response), pv);
  gtk_widget_show(dialog);
}