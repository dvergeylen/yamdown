#ifndef __YAMDOWN_PANE_VIEW_H__
#define __YAMDOWN_PANE_VIEW_H__

#include <gtk/gtk.h>

#define YAMDOWN_TYPE_PANE_VIEW yamdown_pane_view_get_type ()
G_DECLARE_FINAL_TYPE (YamdownPaneView, yamdown_pane_view, YAMDOWN, PANE_VIEW, GtkBox)

struct _YamdownPaneView {
  GtkBox parent;
  GFile* file;
  GtkTextView* sourceview;
  GtkTextView* webview;
};

GtkWidget* yamdown_pane_view_new();
GtkWidget* yamdown_pane_view_new_with_file(GFile* file);
void yamdown_pane_view_save(YamdownPaneView* pv);
void yamdown_pane_view_saveas(YamdownPaneView* pv);
void yamdown_pane_view_open(YamdownPaneView* pv);

#endif /* __YAMDOWN_PANE_VIEW_H__ */
