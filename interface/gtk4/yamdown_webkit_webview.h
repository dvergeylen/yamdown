#define YAMDOWN_TYPE_WEBKIT_WEBVIEW yamdown_webkit_webview_get_type ()
G_DECLARE_FINAL_TYPE (YamdownWebKitWebView, yamdown_webkit_webview, YAMDOWN, WEBKIT_WEBVIEW, WebKitWebView)


/* "open-response" signal responses */
enum {
  YAMDOWN_OPEN_RESPONSE_SUCCESS,
  YAMDOWN_OPEN_RESPONSE_CANCEL,
  YAMDOWN_OPEN_RESPONSE_ERROR
};

GFile* yamdown_webkit_webview_get_file(YamdownWebKitWebView* wv);

void yamdown_webkit_webview_open(YamdownWebKitWebView *wv, GtkWidget* win);

void yamdown_webkit_webview_save(YamdownWebKitWebView *wv);

void yamdown_webkit_webview_saveas(YamdownWebKitWebView *wv);

GtkWidget* yamdown_webkit_webview_new_with_file(GFile *file);

GtkWidget* yamdown_webkit_webview_new(void);
