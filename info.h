/** info.h - 24/02/2012 */

#ifndef INFO__H
#define INFO__H

#include <gtk/gtk.h>

enum { S_BEGIN, S_REBOUND, S_LOST, S_WON };

typedef struct {
	gint x, y;			/* Coordonnées du centre de la balle */
	gint dx, dy;		/* Déplacement élémentaire */
	gint r;				/* Rayon */
} Ball;

typedef struct {
	gint x, y;		/* Coordonnées du centre de la raquette */
	gint rx, ry;	/* Rayons en x et en y */
} Racquet;

typedef struct {
	gint anim;
	gint timeout1, timeout2;
	gint area_width, area_height;
	gint key_LT, key_LB, key_RT, key_RB;
	guchar state;
	gint score_right, score_left;
	GdkPixbuf *pix_ball, *pix_rac_right, *pix_rac_left, *pix_background;
	Ball ball;
	Racquet rac_left, rac_right;
} Info;

/** Initialisation des champs de Info */
void info_init_default (Info *info);

/** Animation */
void info_anim_start (Info *info, gpointer data);
void info_anim_stop  (Info *info);
gboolean on_timeout1 (gpointer data);
gboolean on_timeout2 (gpointer data);
void area_redraw     (GtkWidget *area);

/** Balle */
void ball_init 			  (Info *info);
void ball_draw 			  (GdkWindow *win, GdkGC *gc, Info *info);
void ball_next_step       (Info *info, GtkWidget *statusbar);
void ball_prepare_service (Info *info);

/** Raquette */
void racquet_init       (Racquet *rac);
void racket_draw        (GdkWindow *win, GdkGC *gc, Racquet *rac);
void racket_adapt_right (Racquet *rac, gint width, gint height);
void racket_adapt_left  (Racquet *rac, gint width, gint height);
void racket_next_step   (Info *info, Racquet *rac, gint area_height, 
								 gint key_T, gint key_B);

/** State */
void state_show (Info *info, GtkWidget *statusbar);
void state_set  (Info *info, guchar state, GtkWidget *statusbar);

/** Afficher le score */
void draw_score (Info *info, GdkGC *gc, GtkWidget *area);

/** Charger les images */
void pix_load_all (Info *info);

#endif /* INFO__H */
