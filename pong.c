/* pong.c - 24/02/2012 */

#include <stdlib.h>
#include "info.h"
#include "gui.h"

int main (int argc, char *argv[])
{
    Info info;
    Gui gui;
    srand(time(NULL));

    gtk_init (&argc, &argv);
    info_init_default (&info);
    gui_init (&gui, &info);
    gtk_widget_show_all (gui.window);

    gtk_main ();
    return 0;
}
