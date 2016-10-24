/* util.h - 24/02/2012 */

#ifndef UTIL__H
#define UTIL__H

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#define U __attribute__((unused))

#define HOMOGENEOUS TRUE
#define EXPAND      TRUE
#define FILL        TRUE

#define ON_SIGNAL(w,n,c,d) \
    (g_signal_connect(G_OBJECT(w), (n), G_CALLBACK(c), (d) ))


/* Colors */
void color_set_rgb (GdkColor *color, guint rgb);
void gc_set_rgb_fg (GdkGC *gc, guint rgb);
GdkColor *static_color_from_rgb (guint rgb);
guint rgb_from_color (GdkColor *color);


/* Frames */
GtkWidget *frame_new (gchar *label, gboolean shadowed);


/* Status bars */
void statusbar_put (GtkStatusbar *bar, char *text);

/* Load images */
void pixbuf_get_size (GdkPixbuf *pix, gint *w, gint *h);
GdkPixbuf* pixbuf_load (const char *filename);
void pixbuf_draw (GdkWindow *win, GdkGC *gc, GdkPixbuf *pix, gint x, 
						gint y);

#endif /** UTIL__H */

