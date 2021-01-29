#ifndef __YAMDOWN_NOTEBOOK_H
#define __YAMDOWN_NOTEBOOK_H
void notebook_page_new(GtkNotebook *nb);

void notebook_page_new_with_file(GtkNotebook *nb, GFile *file);

void notebook_page_open(GtkNotebook *nb);

void notebook_page_save(GtkNotebook *nb);

#endif /* __YAMDOWN_NOTEBOOK_H */
