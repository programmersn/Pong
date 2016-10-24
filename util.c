/* util.c - 24/02/2012 */

#include "util.h"


/**------------------------- C O L O R S ----------------------------*/

void color_set_rgb (GdkColor *color, guint rgb)
{
    color->red   = (rgb & 0xFF0000) >> 8;
    color->green = (rgb & 0x00FF00)     ;
    color->blue  = (rgb & 0x0000FF) << 8;
}

void gc_set_rgb_fg (GdkGC *gc, guint rgb) {
    gdk_gc_set_rgb_fg_color (gc, static_color_from_rgb (rgb));
}

GdkColor *static_color_from_rgb (guint rgb) {
    static GdkColor color;
    color_set_rgb (&color, rgb);
    return &color;
}

guint rgb_from_color (GdkColor *color) {
    return (color->red   & 0xFF00) << 8 |
           (color->green & 0xFF00)      |
           (color->blue  & 0xFF00) >> 8 ;
}


/**------------------------- F R A M E S ----------------------------*/

GtkWidget *frame_new (gchar *label, gboolean shadowed) {
    GtkWidget *frame = gtk_frame_new (label);
    GtkShadowType type = shadowed ? GTK_SHADOW_ETCHED_IN : GTK_SHADOW_NONE;
    gtk_frame_set_shadow_type (GTK_FRAME (frame), type);
    return frame;
}


/**----------------------- S T A T U S B A R ------------------------*/

void statusbar_put (GtkStatusbar *bar, char *text) {
    gtk_statusbar_pop (bar, 0);
    gtk_statusbar_push (bar, 0, text);
}


/**-------------------------- I M A G E S ---------------------------*/

void pixbuf_get_size (GdkPixbuf *pix, gint *w, gint *h) {
	*w = gdk_pixbuf_get_width (pix);
	*h = gdk_pixbuf_get_height (pix);
}
	
GdkPixbuf* pixbuf_load (const char *filename) {
	GError *error = NULL;
	GdkPixbuf *pix;
	gint w, h;
	
	printf("Loading %s ...\n", filename); fflush(stdout);
	pix = gdk_pixbuf_new_from_file (filename, &error);
	if(!pix) {
		printf("error\n");
		fprintf(stderr, "Unable to read file \"%s\"", error->message);
		g_error_free(error);
		return NULL;
	}
	pixbuf_get_size (pix, &w, &h); printf("%dx%d\n", w, h);
	
	return pix;
}

void pixbuf_draw (GdkWindow *win, GdkGC *gc, GdkPixbuf *pix, gint x, 
						gint y) {
	gdk_draw_pixbuf(win, gc, pix, 0, 0, x, y, -1, -1, 
						GDK_RGB_DITHER_NONE, 0, 0);
}
