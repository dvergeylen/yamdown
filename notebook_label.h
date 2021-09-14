#ifndef __YAMDOWN_NOTEBOOK_LABEL_H__
#define __YAMDOWN_NOTEBOOK_LABEL_H__

#include <gtk/gtk.h>

#define YAMDOWN_TYPE_NOTEBOOK_LABEL yamdown_notebook_label_get_type ()
G_DECLARE_FINAL_TYPE (YamdownNotebookLabel, yamdown_notebook_label, YAMDOWN, NOTEBOOK_LABEL, GtkBox)

struct _YamdownNotebookLabel
{
  GtkBox parent;
  GtkLabel* label;
  GtkButton* btn_close;
};

GtkWidget* yamdown_notebook_label_new();

#endif /* __YAMDOWN_NOTEBOOK_LABEL_H__ */
