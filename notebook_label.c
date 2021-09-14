#include "notebook_label.h"

G_DEFINE_TYPE (YamdownNotebookLabel, yamdown_notebook_label, GTK_TYPE_BOX)


static void yamdown_notebook_label_dispose (GObject *gobject) {
  G_OBJECT_CLASS (yamdown_notebook_label_parent_class)->dispose (gobject);
}

static void yamdown_notebook_label_init (YamdownNotebookLabel *notebooklabel) {
  gtk_widget_init_template (GTK_WIDGET (notebooklabel));
}

static void yamdown_notebook_label_class_init (YamdownNotebookLabelClass *class) {
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  object_class->dispose = yamdown_notebook_label_dispose;
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/com/github/dvergeylen/yamdown/notebook_label.ui");

  // Bind the label and btn_close variables to the child objects of the XML.
  // id and variable names must match, as gtk_widget_class_bind_template_child is a macro
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), YamdownNotebookLabel, label);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), YamdownNotebookLabel, btn_close);
}

GtkWidget* yamdown_notebook_label_new() {
  return GTK_WIDGET (g_object_new (YAMDOWN_TYPE_NOTEBOOK_LABEL, NULL));
}
