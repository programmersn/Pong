/* info.c - 24/02/2012 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "font.h"
#include "info.h"
#include "gui.h"
#include "util.h"


/**--------------------------- I N I T ------------------------------*/

void info_init_default (Info *info) {
	printf ("info_init_default ...\n");
	
	info->anim        = 0;
	info->timeout1 	= 0; info->timeout2 	  = 0;
	info->area_height = 0; info->area_width  = 0;
	
	/* Les touches de contrôle */
	info->key_LT = 0; info->key_LB = 0;
	info->key_RT = 0; info->key_RB = 0;
	
	/* Caractéristiques de la balle */
	ball_init (info);
	
	/* Caractéristiques des raquettes */
	racquet_init (&info->rac_right);
	racquet_init (&info->rac_left);
	
	/* Etat du jeu */
	info->state = S_BEGIN;

	/* Scores des joueurs */
	info->score_right = 0;
	info->score_left = 0;
	
	/* Images */
	pix_load_all (info);
}


/**---------------------- A N I M A T I O N -------------------------*/

/* Intervalle de rafraîchissement de la zone de dessin */
#define TIMEOUT1 30	    

/* Intervalle de temps au bout duquel se fait l'incrémentation du 
    déplacement élementaire de la balle */
#define TIMEOUT2 10

gboolean on_timeout1 (gpointer U data) {
	Gui *g = gui_check(data);
/*	printf("L.%d on_timeout1...\n", __LINE__);*/
	area_redraw(g->area);
	
	return TRUE;
}

gboolean on_timeout2 (gpointer U data) {
	Gui *g = gui_check(data);
/*	printf("L.%d on_timeout2...\n", __LINE__);*/

	racket_next_step (g->info, &g->info->rac_right, g->info->area_height, 
							g->info->key_RT, g->info->key_RB);
	racket_next_step (g->info, &g->info->rac_left, g->info->area_height, 
							g->info->key_LT, g->info->key_LB);

	if(g->info->state == S_BEGIN)
		ball_prepare_service(g->info);
	else if(g->info->state == S_REBOUND)
		ball_next_step(g->info, g->statusbar);
	
	return TRUE;
}

void info_anim_start (Info *info, gpointer data) {
	if(!info->timeout1)
		info->timeout1 = g_timeout_add(TIMEOUT1, on_timeout1, data);
	if(!info->timeout2)
		info->timeout2 = g_timeout_add(TIMEOUT2, on_timeout2, data);
}

void info_anim_stop (Info *info) {
	if(info->timeout1) {
		g_source_remove (info->timeout1);
		info->timeout1 = 0;
	}
	if(info->timeout2) {
		g_source_remove (info->timeout2);
		info->timeout2 = 0;
	}
}

void area_redraw (GtkWidget *area) {
	gtk_widget_queue_draw (area);
}

/**--------------------------- B A L L -------------------------------*/

void ball_init (Info *info) {
	Ball *b = &info->ball;

	b->r = 15;

	b->x = b->r;
	b->y = b->r;

	b->dx = 2;
	b->dy = 2;
}

void ball_draw (GdkWindow *win, GdkGC *gc, Info *info) {
	Ball *b = &info->ball;
	
	gc_set_rgb_fg (gc, 0xFF0000);
	gdk_draw_arc (win, gc, TRUE, b->x - b->r, b->y - b->r, 
										  2*b->r, 2*b->r, 0, 360*64);
}

void ball_next_step (Info *info, GtkWidget *statusbar) {
	Ball *b = &info->ball;
	Racquet *rac_left = &info->rac_left, *rac_right = &info->rac_right;
	gint rac_width = 2*rac_right->rx, ry = rac_left->ry;
	gint w = info->area_width, h = info->area_height;

	/* Choix aléatoire de la direction de la balle */
	b->x += b->dx;
	b->y += b->dy;
/*	printf("b->x=%d, b->y=%d\n", b->x,b->y);*/
	
	if(info->pix_background) 
		pixbuf_get_size (info->pix_background, &w, &h);
	
	/* Si la balle touche l'une des raquettes */
	if(b->x + b->r >= w - rac_width - 2) {
	/* Raquette droite */
		if(b->y >= rac_right->y - ry && b->y <= rac_right->y + ry) {
			b->x = w - rac_width - 2 - b->r;
			b->dx = -b->dx;
		}
	}
	if(b->x - b->r < rac_width + 2) {
	/* Raquette gauche */
		if(b->y >= rac_left->y - ry && b->y <= rac_left->y + ry){
					b->x = rac_width + b->r;
			b->dx = -b->dx;
		}
	}

	/* Sortie horizontale de la zone de dessin (sans avoir touché la 
	    raquette) */
	
	if(b->y < rac_right->y - ry || b->y > rac_right->y + ry) {
	/* Sur le bord droit */
		if(b->x + b->r > w) {
			b->x = w - b->r;
			state_set (info, S_LOST, statusbar);
			/* Le joueur gauche gagne un point */
			info->score_left++;
		}
	}
	if(b->y < rac_left->y - ry || b->y > rac_left->y + ry) {
	/* Sur le bord gauche */
		if(b->x - b->r < 0) {
			b->x = b->r;
			state_set (info, S_LOST, statusbar);
			/* Le joueur droit gagne un point */
			info->score_right++;
		}
	}
	
	
	/* Sortie verticale de la zone de dessin */
	if(b->y - b->r < 0) {
		b->y  = b->r;
		b->dy = -b->dy;
	}
	if(b->y + b->r > h) {
		b->y  = h - b->r;
		b->dy = -b->dy;
	}
}

void ball_prepare_service (Info *info) {
	gint w = info->area_width, h = info->area_height;
	
	if(info->pix_background)
		pixbuf_get_size (info->pix_background, &w, &h);
	info->ball.x = w/2;
	info->ball.y = w/2;
}

/**-------------------------- R A C K E T ---------------------------*/

void racquet_init (Racquet *rac) {
	rac->x  = 100; rac->y  = 100;
	rac->rx = 4;   rac->ry = 35;
}

void racket_draw (GdkWindow *win, GdkGC *gc, Racquet *rac) {
	gc_set_rgb_fg (gc, 0x0000FF);
	gdk_draw_rectangle (win, gc, FALSE, 
							  rac->x - rac->rx, rac->y - rac->ry,
							  2*rac->rx       , 2*rac->ry);
}

void racket_adapt_right (Racquet *rac, gint width, gint height) {
	rac->x = width - rac->rx - 2;
	rac->y = rac->ry + 100;
	
	/* Sortie par le haut */
	if(rac->y - rac->ry < 0)
		rac->y = rac->ry;
		
	/* Sortie par le bas */
	if(rac->y + rac->ry > height)
		rac->y = height - rac->ry;
}

void racket_adapt_left (Racquet *rac, gint U width, gint height) {
	rac->x = rac->rx + 2;
	rac->y = rac->ry + 100;
	
	/* Sortie par le haut */
	if(rac->y - rac->ry < 0)
		rac->y = rac->ry;

	/* Sortie par le bas */
	if(rac->y + rac->ry > height)
		rac->y = height - rac->ry;
}

void racket_next_step (Info *info, Racquet *rac, gint area_height, 
							  gint key_T, gint key_B) {
	gint h = area_height, w;
	if(info->pix_background)
		pixbuf_get_size (info->pix_background, &w, &h);
	
	/* Si les 2 touches sont enfoncées */
	if(key_T && key_B) return;
	
	if(key_T && rac->y - rac->ry - 6 >= 0) {
		rac->y -= 6;
		return ;
	}
	if(key_B && rac->y + rac->ry + 6 <= h) {
		rac->y += 6;
		return;
	}
}


/**-------------------------- S T A T E -----------------------------*/

void state_show (Info *info, GtkWidget *statusbar) {
	if(!info->anim) {		/* Si l'animation est stoppée */
		statusbar_put (GTK_STATUSBAR(statusbar), "");
		return;
	}
	
	if(info->state == S_BEGIN)
		statusbar_put (GTK_STATUSBAR(statusbar), "Pour commencer à jouer,"
		               " appuyer sur \"espace\". Commandes du jeux : a, q"
		               ", p, l");
		
	else if(info->state == S_REBOUND)
		statusbar_put (GTK_STATUSBAR(statusbar), "Jeu en cours ...");
	
	else if(info->state == S_LOST)
		statusbar_put (GTK_STATUSBAR(statusbar), "Perdu !! Appuyer sur "
							"espace pour rejouer");

	else if(info->state == S_WON)
		statusbar_put (GTK_STATUSBAR(statusbar), "Gagné !! Appuyer sur "
							"espace pour rejouer");
}

void state_set (Info *info, guchar state, GtkWidget *statusbar) {
	info->state = state;
	state_show (info, statusbar);
}


/**---------------------- S C O R E ---------------------------------*/

#define FONT_SIZE 50

void draw_score (Info *info, GdkGC *gc, GtkWidget *area) { 
	gint x = info->area_width, y = info->area_height;
	PangoLayout *pango_layout = gtk_widget_create_pango_layout
	                            (area, NULL);
	
	gc_set_rgb_fg (gc, 0x0000FF);
	
	/* Chargement d'une fonte */
	font_set_name(pango_layout, "Homa, bold 20");
	
	/* Taille du texte */
	font_set_size (pango_layout, FONT_SIZE);
	
	/* Afficher le score des joueurs au milieu-haut de l'image ou
	    de la drawing area si l'image n'a pas été chargée */
	if(info->pix_background)
		pixbuf_get_size (info->pix_background, & x, & y);

	font_draw_text (area, gc, pango_layout, FONT_TL, x/3 , y/10,
						 " %d \t %d", info->score_left, info->score_right);

	g_object_unref(G_OBJECT(pango_layout));
}


/**------------------ L O A D   I M A G E S --------------------------*/

void pix_load_background (Info * info)
{
	info->pix_background = pixbuf_load ("Images/Eau_redim.jpg");	
}

void pix_load_racquet_right (Info * info)
{
	info->pix_rac_right= pixbuf_load  ("Images/Icone_poisson.png");
	if (info->pix_rac_right) { 
		int width, height;	
		pixbuf_get_size (info->pix_rac_right, &width, &height);
		info->rac_right.rx = width/2; 
		info->rac_right.ry = height/2;
	}
}

void pix_load_racquet_left (Info * info)
{
	info->pix_rac_left = pixbuf_load ("Images/poisson.png");
	if (info->pix_rac_left) { 
		int width, height;	
		pixbuf_get_size (info->pix_rac_left, &width, &height);
		info->rac_left.rx = width/2; 
		info->rac_left.ry = height/2;
	}
}

void pix_load_ball (Info * info)
{
	info->pix_ball = pixbuf_load 	     ("Images/ballon.png");
	if (info->pix_ball) {
		int width, height;	
		pixbuf_get_size (info->pix_ball, &width, &height);
		info->ball.r = width/2;
	}
}

void pix_load_all (Info *info) {
	pix_load_background (info);
	pix_load_racquet_right (info);
	pix_load_racquet_left (info);
	pix_load_ball (info);
}	
