# Yamdown

### Getting Started

```bash
# Dependencies (GTK4 + WebKit2-5.0)
export PKG_CONFIG_PATH="/media/D/TEMP/webkit-gtk4/lib/pkgconfig:/media/D/TEMP/gtk4-lib/lib/x86_64-linux-gnu/pkgconfig"
export LD_LIBRARY_PATH="/media/D/TEMP/webkit-gtk4/lib:/media/D/TEMP/gtk4-lib/lib/x86_64-linux-gnu/"


# Compiling
cd interface
meson setup gccbuild .
cd gccbuild
ninja

# Starting
export GSETTINGS_SCHEMA_DIR=/media/D/TEMP/gtk4-lib/share/glib-2.0/schemas
./yamdown
```
