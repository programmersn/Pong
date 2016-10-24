/* gui.c - 24/02/2012 */

#include <gdk/gdkkeysyms.h>  /* pour keyvals tq GDK_q */
#include <gtk/gtk.h>
#include "gui.h"
#include "util.h"

/**------------------------ I N I T ---------------------------------*/

#define GUI_MAGIC 0x46EA7E05

void gui_init (Gui *g, Info *info)
{
    printf ("gui_init ...\n");
    g->magic = GUI_MAGIC;
    g->info = info;
    gui_init_window    (g);
    gui_init_layout    (g);
    gui_init_top       (g);
    gui_init_area      (g);
    gui_init_statusbar (g);
}


/* Cette fonction permet de tester si le data que l'on a recuperé dans
 * un handler de signaux contient bien un gui ; sinon, cela veut dire 
 * que :
 *  - soit on a oublié de transmettre gui dans g_signal_connect,
 *  - soit on s'est trompé dans le nombre de parametres du handler de
 * signaux.
*/

Gui *gui_check (gpointer data)
{
    if (data == NULL) { 
        fprintf (stderr, "gui_check: NULL data\n");
        return NULL; 
    }
    if (((Gui *)data)->magic != GUI_MAGIC) {
        fprintf (stderr, "gui_check: bad magic number\n"); 
        return NULL; 
    }
    return data;
}


/*------------------------- W I N D O W S ----------------------------*/

#define WIN1_TITLE  "Jeu de Pong"
#define WIN1_W   600
#define WIN1_H   600

#define PLAYFIELD_DEFAULT_WIDTH  600
#define PLAYFIELD_DEFAULT_HEIGHT 600

void on_window_destroy (GtkWidget U *widget, gpointer U data)
{
    /* Gui *g = gui_check (data); */

    printf ("on_window_destroy\n");
    gtk_main_quit ();
}

gboolean on_window_delete_event (GtkWidget U *widget, GdkEvent U *event, 
											gpointer U data)
{
    /* Gui *g = gui_check (data); */

    printf ("on_window_delete_event\n");
    return FALSE;
}


void gui_init_window (Gui *g)
{
    g->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (g->window), WIN1_TITLE);
    gtk_window_set_default_size (GTK_WINDOW (g->window), WIN1_W, WIN1_H);

    ON_SIGNAL (g->window, "delete-event", on_window_delete_event, g);
    ON_SIGNAL (g->window, "destroy",      on_window_destroy,      g);
}


/*-------------------------- L A Y O U T -----------------------------*/


void gui_init_layout (Gui *g) {
    Layout *l = &g->layout;

    l->main_box = gtk_vbox_new (!HOMOGENEOUS, 0);
    l->top_box  = gtk_hbox_new (!HOMOGENEOUS, 0);

    gtk_container_add (GTK_CONTAINER (g->window), l->main_box);
    gtk_box_pack_start (GTK_BOX (l->main_box), l->top_box, 
								!EXPAND, !FILL, 0);
}


/*----------------------------- T O P ----------------------------*/

void on_quit_clicked (GtkButton U *widget, gpointer U data) {
    /*Gui *g = gui_check (data);*/

    gtk_main_quit ();
}

void on_start_clicked (GtkButton *widget, gpointer data) {
	Gui *g = gui_check (data);
	Info *info = g->info;
	
	if(!info->anim) {
		info->anim = 1;
		gtk_button_set_label(widget, "Stop");
		info_anim_start (info, g);
		/* Demander le focus clavier */
		gtk_widget_grab_focus (g->area);
	}
	else {
		info->anim = 0;
		gtk_button_set_label(widget, "Start");
		info_anim_stop (info);
	}
}

void gui_init_top (Gui *g) {
	Layout *l = &g->layout;
	GtkWidget *tmp, *start;

	/* Quit */
	tmp = gtk_button_new_with_label ("Quit");
	gtk_box_pack_start (GTK_BOX (l->top_box), tmp, !EXPAND, !FILL, 0);
	ON_SIGNAL (tmp, "clicked", on_quit_clicked, g);
		
	/* Start/Stop */
	start = gtk_button_new_with_label("Start");
	gtk_box_pack_start(GTK_BOX(l->top_box), start, !EXPAND, !FILL, 0);
	ON_SIGNAL (start, "clicked", on_start_clicked, g);
	
}


/*---------------------- D R A W I N G   A R E A --------------------*/

void
gui_get_playfield_size (Gui * g, gint * width, gint * height)
{
	if(! g->info->pix_background) {
		GtkAllocation alloc;
		gtk_widget_get_allocation (g->area, & alloc);
		* width = alloc.width;
		* height= alloc.height;
	} else {
		pixbuf_get_size (g->info->pix_background, width, height);
	}
}

void
gui_draw_playfield (Gui * g, GdkGC * gc)
{
	int width, height;
	gui_get_playfield_size (g, &width, &height);
	if ( ! g->info->pix_background) { 
		gc_set_rgb_fg (gc, 0xFFFFEE);
		gdk_draw_rectangle (g->area->window, gc, TRUE, 0, 0, width, height);
	}
	else {
		pixbuf_draw (g->area->window, gc, g->info->pix_background, 0, 0);
	}	
}

void
gui_draw_ball (Gui * g, GdkGC * gc)
{
	if ( ! g->info->pix_ball) {
		ball_draw (g->area->window, gc, g->info);
	} else {
		pixbuf_draw (g->area->window, gc, g->info->pix_ball, 
						 g->info->ball.x - g->info->ball.r, 
						 g->info->ball.y - g->info->ball.r); 
	}
}

void
gui_draw_racquet_right (Gui * g, GdkGC * gc)
{
	if( ! g->info->pix_rac_right) {
		racket_draw (g->area->window, gc, & g->info->rac_right);
	} else {
		pixbuf_draw (g->area->window, gc, g->info->pix_rac_right, 
						 g->info->rac_right.x - g->info->rac_right.rx, 
						 g->info->rac_right.y - g->info->rac_right.ry); 
	}
}

void
gui_draw_racquet_left (Gui * g, GdkGC * gc)
{
	if( ! g->info->pix_rac_left) {
		racket_draw (g->area->window, gc, & g->info->rac_left);
	} else {
		pixbuf_draw (g->area->window, gc, g->info->pix_rac_left, 
						 g->info->rac_left.x - g->info->rac_left.rx, 
						 g->info->rac_left.y - g->info->rac_left.ry); 
	}
}

void
gui_draw_scene (Gui * g, GdkGC * gc)
{
	gui_draw_playfield     (g, gc);
	gui_draw_ball          (g, gc);
	gui_draw_racquet_right (g, gc);
	gui_draw_racquet_left  (g, gc);
	
	draw_score (g->info, gc, g->area);
}

gboolean 
on_area_expose (GtkWidget *widget, GdkEventExpose U *e, gpointer U data)
{
	GdkGC *gc= gdk_gc_new (widget->window);
	Gui *g = gui_check(data);
	gui_draw_scene (g, gc);
	g_object_unref (gc);
	return TRUE;  			/* L'evenement a ete traité */
}

gboolean 
on_area_but_press (GtkWidget U *w, GdkEventButton *e, gpointer U data) {
    Gui *g = gui_check (data);

	printf ("on_area_but_press  x = %d  y = %d  b = %d\n", 
           (int)e->x, (int)e->y, e->button);

	gtk_widget_grab_focus(g->area);
	return TRUE;  /* L'evenement a ete traité */
}

gboolean 
on_area_but_release (GtkWidget U *w, GdkEventButton *e, gpointer U data)   
{
    /* Gui *g = gui_check (data); */

    printf ("on_area_but_release  x = %d  y = %d  b = %d\n", 
           (int) e->x, (int) e->y, e->button);

    return TRUE;  /* L'evenement a ete traite' */
}

gboolean 
on_area_motion (GtkWidget U *w, GdkEventMotion *e, gpointer U data) {
    /* Gui *g = gui_check (data); */
    printf ("on_area_motion  x = %d  y = %d\n", (int) e->x, (int) e->y);
    return TRUE;  /* L'evenement a ete traité */
}

gboolean 
on_area_key_press (GtkWidget U *w, GdkEventKey *e, gpointer U data) {
	Gui *g = gui_check (data);
	Info *info = g->info;

	/* e->keyval contient le symbole de la touche pressée */
	switch (e->keyval) {
		case GDK_Escape : 
			gtk_main_quit (); 
			break;		
		case GDK_space :
			if(!info->anim) break;
			if(g->info->state == S_BEGIN) 
				state_set (g->info, S_REBOUND, g->statusbar);
			else if(g->info->state == S_LOST || g->info->state == S_WON) 
				state_set (g->info, S_BEGIN, g->statusbar);
			break;
		case GDK_a : 
			info->key_LT = 1;
			printf ("on_area_key_press    \"%s\"  length = %d "
					  "keyval = GDK_%s\n",
					  e->string, e->length, gdk_keyval_name(e->keyval)); 
			break;
		case GDK_q :
			info->key_LB = 1;
			printf ("on_area_key_press    \"%s\"  length = %d "
					  "keyval = GDK_%s\n",
					  e->string, e->length, gdk_keyval_name(e->keyval)); 
			break;
		case GDK_p :
			info->key_RT = 1;
			printf ("on_area_key_press    \"%s\"  length = %d "
					  "keyval = GDK_%s\n",
					  e->string, e->length, gdk_keyval_name(e->keyval)); 
			break;
		case GDK_l :
			info->key_RB = 1;
			printf ("on_area_key_press    \"%s\"  length = %d "
					  "keyval = GDK_%s\n",
					  e->string, e->length, gdk_keyval_name(e->keyval)); 
			break;
		default :  
			printf ("on_area_key_press    \"%s\"  length = %d "
					  "keyval = GDK_%s\n",
					  e->string, e->length, gdk_keyval_name(e->keyval)); 
			break;
	}

	return TRUE;  /* L'evenement a ete traité */
}

gboolean 
on_area_key_release (GtkWidget U *w, GdkEventKey U *e, gpointer U data) 
{
	Gui *g = gui_check (data);
	Info *info = g->info;
	/* e->keyval contient le symbole de la touche relachee */
	switch (e->keyval) {
		case GDK_a :
			info->key_LT = 0;
			printf ("on_area_key_press    \"%s\"  length = %d "
					  "keyval = GDK_%s\n",
					  e->string, e->length, gdk_keyval_name(e->keyval)); 
			break;
		case GDK_q :
			info->key_LB = 0;
			printf ("on_area_key_press    \"%s\"  length = %d "
					  "keyval = GDK_%s\n",
					  e->string, e->length, gdk_keyval_name(e->keyval)); 
			break;
		case GDK_p :
			info->key_RT = 0;
			printf ("on_area_key_press    \"%s\"  length = %d "
					  "keyval = GDK_%s\n",
					  e->string, e->length, gdk_keyval_name(e->keyval)); 
			break;
		case GDK_l :
			info->key_RB = 0;
			printf ("on_area_key_press    \"%s\"  length = %d "
					  "keyval = GDK_%s\n",
					  e->string, e->length, gdk_keyval_name(e->keyval)); 
			break;
		default :  
			printf ("on_area_key_release  \"%s\"  length = %d "
					  " keyval = GDK_%s\n",
						e->string, e->length, gdk_keyval_name(e->keyval)); 
			break;
    }

    return TRUE;  /* L'evenement a ete traité */
}

gboolean 
on_area_configure (GtkWidget U *area, GdkEventConfigure *e, 
						 gpointer data) {
	Gui *g = gui_check (data);
	int width, height;
	
	g->info->area_width = e->width;
	g->info->area_height = e->height;

	gui_get_playfield_size (g, & width, & height);
	racket_adapt_right(& g->info->rac_right, width, height);
	racket_adapt_left (& g->info->rac_left, width, height);

	return TRUE;	/* L'évènement a été traité */
}

void gui_init_area (Gui *g) {
	Layout *l = &g->layout;

	g->area = gtk_drawing_area_new ();
	gtk_box_pack_start (GTK_BOX (l->main_box), g->area, EXPAND, FILL, 3);

	if (g->info->pix_background) {
		int width, height;
		pixbuf_get_size (g->info->pix_background, & width, & height);	
		gtk_widget_set_size_request (g->area, width, height);
	}

	GTK_WIDGET_SET_FLAGS  (g->area, GTK_CAN_FOCUS);
	gtk_widget_add_events (g->area,  GDK_BUTTON_MOTION_MASK  |
												GDK_BUTTON_PRESS_MASK   |
												GDK_BUTTON_RELEASE_MASK |
												GDK_KEY_PRESS_MASK      |
												GDK_KEY_RELEASE_MASK    |
												GDK_EXPOSURE_MASK);

	ON_SIGNAL (g->area, "expose-event",         on_area_expose,      g);
	ON_SIGNAL (g->area, "button-press-event",   on_area_but_press,   g);
	ON_SIGNAL (g->area, "button-release-event", on_area_but_release, g);
	ON_SIGNAL (g->area, "motion-notify-event",  on_area_motion,      g);
	ON_SIGNAL (g->area, "key-press-event",      on_area_key_press,   g);
	ON_SIGNAL (g->area, "key-release-event",    on_area_key_release, g);
	ON_SIGNAL (g->area, "configure_event",      on_area_configure,   g);
}


/**----------------------- S T A T U S B A R -------------------------*/

void gui_init_statusbar (Gui *g) {
	Layout *l = &g->layout;
 
	g->statusbar = gtk_statusbar_new ();
	statusbar_put (GTK_STATUSBAR (g->statusbar), "Salamu 'aleikum");
	gtk_box_pack_start (GTK_BOX (l->main_box), g->statusbar, 
							  !EXPAND, !FILL, 0);
}


/**------------------------------------------------------------------*/

