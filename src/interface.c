/* GIMP Plug-in Template
 * Copyright (C) 2000-2004  Michael Natterer <mitch@gimp.org> (the "Author").
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of the Author of the
 * Software shall not be used in advertising or otherwise to promote the
 * sale, use or other dealings in this Software without prior written
 * authorization from the Author.
 */

#include "config.h"

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "main.h"
#include "interface.h"

#include "plugin-intl.h"


/*  Constants  */

#define SCALE_WIDTH        180
#define SPIN_BUTTON_WIDTH   75
#define RANDOM_SEED_WIDTH  100


/*  Local function prototypes  */

static gboolean   dialog_image_constraint_func (gint32    image_id,
                                                gpointer  data);


/*  Public functions  */

gboolean
dialog (gint * rate,gint total_pages)
{
  GtkWidget *dlg;
  GtkWidget *main_vbox,*vbox2,*hbox,*spinner,*label;
  GtkWidget *frame;
  GtkWidget *table;
  GtkWidget *coordinates;
  GtkObject *adj;
  gint       row;
  gboolean   run = FALSE;

  gimp_ui_init (PLUGIN_NAME, TRUE);

  dlg = gimp_dialog_new (_("Page Parameters"), PLUGIN_NAME,
                         NULL, 0,
			 gimp_standard_help_func, "gimp_djvu_load",
			 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			 GTK_STOCK_OK,     GTK_RESPONSE_OK,
			 NULL);

  main_vbox = gtk_vbox_new (FALSE, 12);
  gtk_container_set_border_width (GTK_CONTAINER (main_vbox), 12);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dlg)->vbox), main_vbox);

  /*  gimp_scale_entry_new() examples  */

  frame = gimp_frame_new (_("Page Parameters"));
  gtk_box_pack_start (GTK_BOX (main_vbox), frame, FALSE, FALSE, 0);
  gtk_widget_show (frame);
/*
  table = gtk_table_new (3, 3, FALSE);
  gtk_table_set_col_spacings (GTK_TABLE (table), 6);
  gtk_table_set_row_spacings (GTK_TABLE (table), 2);
  gtk_container_add (GTK_CONTAINER (frame), table);
  gtk
*/
  row = 0;

  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (main_vbox), hbox, TRUE, TRUE, 5);
  gtk_widget_show(hbox);
  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 5);
  gtk_widget_show(vbox2);
  label = gtk_label_new ("Page No:");
  gtk_misc_set_alignment (GTK_MISC (label), 0, 0.5);
  gtk_box_pack_start (GTK_BOX (vbox2), label, FALSE, TRUE, 0);
  gtk_widget_show(label);
  adj = (GtkAdjustment *) gtk_adjustment_new (1, 1, total_pages, 1,  10, 0.0);
  spinner = gtk_spin_button_new (adj, 0, 0);
  gtk_spin_button_set_wrap (GTK_SPIN_BUTTON (spinner), TRUE);
  gtk_box_pack_start (GTK_BOX (vbox2), spinner, FALSE, TRUE, 0);
  gtk_widget_show(spinner);
/*  
  
   adj = (GtkAdjustment *) gtk_adjustment_new (1998.0, 1997, 2100.0,
					      1.0, 100.0, 0.0);
 

 /* adj = gimp_scale_entry_new (GTK_TABLE (table), 0, row++,
			      _("Compression Rate:"), SCALE_WIDTH, 
                              SPIN_BUTTON_WIDTH,
			      *rate, 0, 100, 1, 10, 0,
			      TRUE, 0, 0,
			      _("Dummy scale entry 1"), NULL);
*/
  g_signal_connect (adj, "value_changed",
                    G_CALLBACK (gimp_int_adjustment_update),
                    rate);

  /*  Show the main containers  */


  gtk_widget_show (main_vbox);
  gtk_widget_show (dlg);

  run = (gimp_dialog_run (GIMP_DIALOG (dlg)) == GTK_RESPONSE_OK);

  gtk_widget_destroy (dlg);

  return run;
}
