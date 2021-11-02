#include "paneview.h"
#include "dialog_response_callbacks.h"
#include "renderer.h"
#include "utils.h"

G_DEFINE_TYPE (YamdownPaneView, yamdown_pane_view, GTK_TYPE_BOX)


static void yamdown_pane_view_dispose(GObject* gobject) {
  YamdownPaneView* pv = YAMDOWN_PANE_VIEW(gobject);

  if (G_IS_FILE(pv->file))
    g_clear_object(&pv->file);

  g_clear_object(&pv->lang_manager);
  g_clear_object(&pv->style_scheme_manager);
  g_clear_object(&pv->webviewtv);

  G_OBJECT_CLASS(yamdown_pane_view_parent_class)->dispose(gobject);
}

/* Callback when a new character has been added/removed from sourceview */
void key_commit_cb(GtkTextBuffer* tb, gpointer user_data) {
  GtkTextBuffer* webviewtb;
  GtkTextIter start_iter;
  GtkTextIter end_iter;
  gchar* html;
  YamdownPaneView* pv = YAMDOWN_PANE_VIEW(user_data);

  /* Render html in webviewtb */
  render_html(tb, user_data);

  /* Retrieve HTML */
  webviewtb = gtk_text_view_get_buffer(pv->webviewtv);
  gtk_text_buffer_get_bounds(webviewtb, &start_iter, &end_iter);
  html = gtk_text_buffer_get_text(webviewtb, &start_iter, &end_iter, FALSE);

  /* Render HTML in webview */
  // TODO: null should be document's basedir
  webkit_web_view_load_html(pv->webview, html, NULL);

  g_free(html);
}

static void yamdown_pane_view_init(YamdownPaneView* paneview) {
  gtk_widget_init_template (GTK_WIDGET (paneview));
  GtkSourceBuffer* tb = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(paneview->sourceview)));

  /* Assign Markdown syntax highlighting to sourcebuffer */
  paneview->lang_manager = gtk_source_language_manager_get_default();
  paneview->lang = gtk_source_language_manager_get_language(paneview->lang_manager, "markdown");
  gtk_source_buffer_set_language (tb, paneview->lang);

  /* Assign theme to sourcebuffer */
  paneview->style_scheme_manager = gtk_source_style_scheme_manager_get_default();
  paneview->style_theme = gtk_source_style_scheme_manager_get_scheme(paneview->style_scheme_manager, "cobalt");
  gtk_source_buffer_set_style_scheme (tb, paneview->style_theme);

  /* Create GtkTextView webviewtv (used to display raw HTML) */
  paneview->webviewtv = GTK_TEXT_VIEW(gtk_text_view_new());

  g_signal_connect(GTK_TEXT_BUFFER(tb), "changed", G_CALLBACK(key_commit_cb), paneview);
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

  GtkSourceView* sourceview = pv->sourceview;
  GtkSourceBuffer* tb = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(sourceview)));
  GtkWidget* win = gtk_widget_get_ancestor(GTK_WIDGET(pv), GTK_TYPE_WINDOW);

  if (!gtk_text_buffer_get_modified(GTK_TEXT_BUFFER(tb)))
    return; /* no need to save it */
  else if (pv->file == NULL)
    yamdown_pane_view_saveas(pv);
  else if (!G_IS_FILE(pv->file))
    g_error("YamdownPaneView: The pointer pv->file isn't NULL nor GFile.\n");
  else
    save_file(pv->file, GTK_TEXT_BUFFER(tb), GTK_WINDOW(win));
}


GtkWidget* yamdown_pane_view_new() {
  return GTK_WIDGET (g_object_new (YAMDOWN_TYPE_PANE_VIEW, NULL));
}


GtkWidget* yamdown_pane_view_new_with_file(GFile* file) {
  g_return_val_if_fail(G_IS_FILE(file), NULL);

  GtkWidget* pv;
  GtkSourceView* sourceview;
  GtkSourceBuffer* tb;
  GtkTextBuffer* webviewtb;
  char* contents;
  gsize length;
  GtkTextIter start_iter;
  GtkTextIter end_iter;
  gchar* html;

  if (!g_file_load_contents(file, NULL, &contents, &length, NULL, NULL)) /* read error */
    return NULL;

  if ((pv = yamdown_pane_view_new()) != NULL) {
    sourceview = YAMDOWN_PANE_VIEW(pv)->sourceview;
    tb = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(sourceview)));
    YAMDOWN_PANE_VIEW(pv)->file = g_file_dup(file);

    /* Set text to textview's buffer */
    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(tb), contents, length);
    gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(tb), TRUE);
    gtk_source_buffer_set_highlight_syntax (tb, true);

    /* Set rendered HTML to textview's buffer */
    /* As we must "prepare" the HTML string, before giving it
     * to webkitwebview, we use the GtkTextView 'insert' method to
     * efficiently construct the string (with pre/post-ambles).
     * This is not an extra copy, as it is needed anyway
     */
    render_html(GTK_TEXT_BUFFER(tb), YAMDOWN_PANE_VIEW(pv));

    /* Retrieve HTML */
    webviewtb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(YAMDOWN_PANE_VIEW(pv)->webviewtv));
    gtk_text_buffer_get_bounds(webviewtb, &start_iter, &end_iter);
    html = gtk_text_buffer_get_text(webviewtb, &start_iter, &end_iter, FALSE);

    /* Render HTML in webview */
    // TODO: null should be document's basedir
    webkit_web_view_load_html(YAMDOWN_PANE_VIEW(pv)->webview, html, NULL);
  }
  g_free(contents);
  g_free(html);
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
