#ifndef __NOTEBOOK_H
#define __NOTEBOOK_H

#include <gtk/gtk.h>
#include "paneview.h"
#include "notebook_label.h"

void notebook_page_new(GtkNotebook *nb);

void notebook_page_new_with_file(GtkNotebook *nb, GFile *file);

void notebook_page_open(GtkNotebook *nb);

void notebook_page_save(GtkNotebook *nb);

#endif /* __NOTEBOOK_H */
