#include "yamdown.h"

int main(int argc, char** argv) {
  int stat;

  gtk_source_init();
  stat = application_new(argc, argv);
  gtk_source_finalize();

  return stat;
}
