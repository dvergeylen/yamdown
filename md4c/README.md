These two files are taken from https://github.com/mity/md4c:

* src/md4c-html.h
* src/md4c-html.c

The goal is to modify as little as possible the functions to output efficient html.
The only two modifications will concern:

* Images, where an image are prefixed with the `base_uri`.
* Code blocks, where the content is passed to GtkSourceView.
