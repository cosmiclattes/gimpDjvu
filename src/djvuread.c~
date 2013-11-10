#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <locale.h>
#include <fcntl.h>
#include <errno.h>

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "main.h"

#include "plugin-intl.h"

#if HAVE_PUTC_UNLOCKED
# undef putc
# define putc putc_unlocked
#endif

#ifndef i18n
# define i18n(x) (x)
# define I18N(x) (x)
#endif


#ifdef UNIX
# include <sys/time.h>
# include <sys/types.h>
# include <unistd.h>
#endif

#include "ddjvuapi.h"


int i;

ddjvu_context_t *ctx;
ddjvu_document_t *doc;
double       flag_scale = 150;
int          flag_size = -1;
int          flag_aspect = -1;
int          flag_subsample = -1;
int          flag_segment = 0;
int          pageno;


FILE *fsrc = NULL;
FILE *fout=NULL;
ddjvu_page_t *page;

static int get_file_format (char *filename);
/* Djvuapi events */

void
handle(int wait)
{
  const ddjvu_message_t *msg;
  if (!ctx)
    return;
  if (wait)
    msg = ddjvu_message_wait(ctx);
  while ((msg = ddjvu_message_peek(ctx)))
    {
      switch(msg->m_any.tag)
        {
        case DDJVU_ERROR:
          fprintf(stderr,"ddjvu: %s\n", msg->m_error.message);
          if (msg->m_error.filename)
            fprintf(stderr,"ddjvu: '%s:%d'\n", 
                    msg->m_error.filename, msg->m_error.lineno);
          exit(10);
        default:
          break;
        }
      ddjvu_message_pop(ctx);
    }
}


gint32
load_djvu (const gchar * filename, 
          GimpRunMode runmode, 
          gboolean preview)

{
  
  GimpPixelRgn rgn_in;
  GimpDrawable *drawable;
  gint32 volatile image_ID;
  gint32 layer_ID;
  int x1, y1, x2, y2, width, height;
   unsigned char *src = NULL;
  int file_length;



ctx = ddjvu_context_create("gimp");
//
doc = ddjvu_document_create_by_filename(ctx,filename, TRUE);



while (! ddjvu_document_decoding_done(doc))
    handle(TRUE);
  




  i = ddjvu_document_get_pagenum(doc);
  dialog( &pageno ,i);
//processing the page
  
page = ddjvu_page_create_by_pageno(doc, pageno-1);
while (! ddjvu_page_decoding_done(page))
    handle(TRUE);

if (ddjvu_page_decoding_error(page))
{
 fprintf(stderr,"unexpected error ");
 exit(10);
}

// ddjvu variables

  ddjvu_rect_t prect;
  ddjvu_rect_t rrect;
  ddjvu_format_style_t style;
  ddjvu_render_mode_t mode;
  ddjvu_format_t *fmt;
  int iw = ddjvu_page_get_width(page);
  int ih = ddjvu_page_get_height(page);
  int dpi = ddjvu_page_get_resolution(page);
  ddjvu_page_type_t type = ddjvu_page_get_type(page);
  char *image = 0;
  int rowsize;

//end of ddjvu variables
style= DDJVU_FORMAT_RGB24;
mode=DDJVU_RENDER_COLOR;
fmt = ddjvu_format_create(style, 0, 0);
ddjvu_format_set_row_order(fmt, 1);

      prect.w = iw;
      prect.h = ih;

       prect.x = 0;
       prect.y = 0;

       flag_scale=150;

     prect.w = (unsigned int) (iw * flag_scale) / dpi;
     prect.h = (unsigned int) (ih * flag_scale) / dpi;
       
      rrect = prect;

 rowsize = (rrect.w *3);
 image = (char*)malloc(rowsize * rrect.h);
  
//generating page
ddjvu_page_render(page, mode, &prect, &rrect, fmt, rowsize, image);

ddjvu_page_release(page);
char *s =image;
  /* create output image */

  width = prect.w;
  height =  prect.h;
  x1 = 0;
  y1 = 0;
  x2 = prect.w;
  y2 = prect.h;


  image_ID = gimp_image_new (width, height, GIMP_RGB);
  layer_ID = gimp_layer_new (image_ID, _("Background"),
			     width, height, GIMP_RGB, 100, GIMP_NORMAL_MODE);
  gimp_image_add_layer (image_ID, layer_ID, 0);
  drawable = gimp_drawable_get (layer_ID);	// initializes the drawable     
  gimp_drawable_mask_bounds (drawable->drawable_id, &x1, &y1, &x2, &y2);
  gimp_pixel_rgn_init (&rgn_in, drawable, x1, y1,
		       x2 - x1, y2 - y1, TRUE, TRUE);
  gimp_pixel_rgn_set_rect (&rgn_in,  image, 0, 0, width, height);
      //g_free (buf);
  gimp_drawable_flush (drawable);
  gimp_drawable_merge_shadow (drawable->drawable_id, TRUE);
  gimp_drawable_update (drawable->drawable_id, x1, y1, x2 - x1, y2 - y1);
  gimp_image_set_filename (image_ID, filename);
  return image_ID;
  image =NULL;
 

//not handling any error
if (doc)
    ddjvu_document_release(doc);
  if (ctx)
    ddjvu_context_release(ctx);
    
}
