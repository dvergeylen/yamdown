#include "renderer.h"
#include "md4c/md4c-html.h"
#include "paneview.h"

/* This function returns <html> + <head> </head> + <body> content,
 * with css styling from preferences
 */
char* html_preamble() {
  return "<!DOCTYPE html><html><head><meta charset=\"utf-8\"></head><body>";
}

char* html_postamble() {
  return "</body></html>";
}

/* Callback from MD4C when a converted chunk is available for output */
void md4c_process_output(const MD_CHAR* chunk, MD_SIZE size, void* user_data) {
  YamdownPaneView* pv = YAMDOWN_PANE_VIEW(user_data);
  GtkTextIter iter;

  /* Append Content */
  GtkTextBuffer* webviewtb = gtk_text_view_get_buffer(pv->webviewtv);
  gtk_text_buffer_get_end_iter(webviewtb, &iter);
  gtk_text_buffer_insert(webviewtb, &iter, chunk, size);
}

void render_html(GtkTextBuffer* sourceviewtb, gpointer user_data) {
  YamdownPaneView* pv = YAMDOWN_PANE_VIEW(user_data);
  GtkTextBuffer* webviewtb = gtk_text_view_get_buffer(pv->webviewtv);
  GtkTextIter start_iter;
  GtkTextIter end_iter;
  gchar* contents;
  char* preamble;
  char* postamble;
  GtkTextIter iter;

  /* Retrieving content */
  gtk_text_buffer_get_bounds(sourceviewtb, &start_iter, &end_iter);
  contents = gtk_text_buffer_get_text(sourceviewtb, &start_iter, &end_iter, FALSE);
  gtk_text_buffer_set_modified(sourceviewtb, FALSE);

  /* Converting content */
  // CMARK LIB
  // char* html = cmark_markdown_to_html(contents, strlen(contents), CMARK_OPT_UNSAFE);
  // END OF CMARK LIB

  /* Applying Content to WebviewTextView */
  /* Preamble */
  preamble = html_preamble();
  gtk_text_buffer_set_text(webviewtb, preamble, strlen(preamble));

  /* Content */
  // CMARK LIB
  // gtk_text_buffer_get_end_iter(webviewtb, &iter);
  // gtk_text_buffer_insert(webviewtb, &iter, html, strlen(html));
  // END OF CMARK LIB

  // MD4C
  md_html(contents, strlen(contents), md4c_process_output, user_data, 0, 0);
  // END OF MD4C

  /* Postamble */
  postamble = html_postamble();
  gtk_text_buffer_get_end_iter(webviewtb, &iter);
  gtk_text_buffer_insert(webviewtb, &iter, postamble, strlen(postamble));

  gtk_text_buffer_set_modified(webviewtb, FALSE);

  g_free(contents);

  // CMARK LIB
  // g_free(html);
  // END OF CMARK LIB
}
