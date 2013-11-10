/* GIMP djvu plug-in
 * Copyright (C) 2000  Michael Natterer <mitch@gimp.org> (the "Author").
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

/* This code provides functionality for loading  djvu images and documents
 * in GIMP.  It utilizes the djvulibre library which is required
 * for compiling the plug-in.  Please email me at jeph_paul@yahoo.co.in
 * for any questions, comments, or bugs.  There can be a lot of
 * improvements to the code in terms of speed and memory usage
 * which I will concurently work on.  But any help will be greatly
 * appreciated.  Also, the UI is very basic and will be extended to 
 * enable more options .
 */

#include "config.h"

#include <string.h>
#include <malloc.h>
#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "main.h"
#include "interface.h"


#include "plugin-intl.h"

#define PROCEDURE_NAME        "gimp_djvu_load"
//#define PROCEDURE_NAME_SAVE   "gimp_djvu_save"

#define DATA_KEY_VALS    "plug_in_template"
#define DATA_KEY_UI_VALS "plug_in_template_ui"

#define PARASITE_KEY     "plug-in-template-options"


/*  Local function prototypes  */

static void query (void);
static void run (const gchar * name,
		 gint nparams,
		 const GimpParam * param,
		 gint * nreturn_vals, GimpParam ** return_vals);

static void INIT_Il8N ();

/*  Local variables  */

const PlugInVals default_vals = {
  0,
  1,
  2,
  0,
  FALSE
};

const PlugInImageVals default_image_vals = {
  0
};

const PlugInDrawableVals default_drawable_vals = {
  0
};

const PlugInUIVals default_ui_vals = {
  TRUE
};

/* 
static PlugInVals         vals;
static PlugInImageVals    image_vals;
static PlugInDrawableVals drawable_vals;
static PlugInUIVals       ui_vals;
*/


GimpPlugInInfo PLUG_IN_INFO = {
  NULL,				/* init_proc  */
  NULL,				/* quit_proc  */
  query,			/* query_proc */
  run,				/* run_proc   */
};

MAIN ()
     static void query (void)
{
  gchar *help_path;
  gchar *help_uri;

  static GimpParamDef load_args[] = {
    {GIMP_PDB_INT32, "run_mode", "Interactive, non-interactive"},
    {GIMP_PDB_STRING, "filename", "The name of the file to load"},
    {GIMP_PDB_STRING, "raw_filename", "The name of the file to load"},
  };

  static GimpParamDef load_return_vals[] = {
    {GIMP_PDB_IMAGE, "image", "Output image"}
  };
  static GimpParamDef save_args[] = {
    {GIMP_PDB_INT32, "run_mode", "Interactive, non-interactive"},
    {GIMP_PDB_IMAGE, "image", "Input image"},
    {GIMP_PDB_DRAWABLE, "drawable", "Drawable to save"},
    {GIMP_PDB_STRING, "filename",
     "The name of the file to save the image in"},
    {GIMP_PDB_STRING, "raw_filename", "The name entered"},
  };



  gimp_plugin_domain_register (PLUGIN_NAME, LOCALEDIR);

  help_path = g_build_filename (DATADIR, "help", NULL);
  help_uri = g_filename_to_uri (help_path, NULL, NULL);
  g_free (help_path);

  gimp_plugin_help_register
    ("http://developer.gimp.org/plug-in-template/help", help_uri);

  gimp_install_procedure (PROCEDURE_NAME,
			  "loads files in the djvu file format",
			  "loads files in the djvu file format",
			  "Jeph Paul",
			  "Jeph Paul",
			  "2006-2010",
			  N_("Djvu Document"),
			  NULL,
			  GIMP_PLUGIN,
			  G_N_ELEMENTS (load_args),
			  G_N_ELEMENTS (load_return_vals),
			  load_args, load_return_vals);

  gimp_register_file_handler_mime (PROCEDURE_NAME, "image/djvu");
  gimp_register_magic_load_handler (PROCEDURE_NAME, "djvu", "", "");
 /*

  gimp_install_procedure (PROCEDURE_NAME_SAVE,
			  "saves files in the djvu file format",
			  "saves files in the djvu file format",
			  "Jeph Paul",
			  "Jeph Paul",
			  "2006-2010",
			  N_("Djvu document"),
			  "GRAY, RGB",
			  GIMP_PLUGIN,
			  G_N_ELEMENTS (save_args), 0, save_args, NULL);

  gimp_register_file_handler_mime (PROCEDURE_NAME_SAVE, "image/jp2");
  gimp_register_save_handler (PROCEDURE_NAME_SAVE, "jp2,j2k", "");
  */
}

static void
run (const gchar * name,
     gint n_params,
     const GimpParam * param, gint * nreturn_vals, GimpParam ** return_vals)
{
  static GimpParam values[1];
  GimpRunMode run_mode;
  GimpPDBStatusType status = GIMP_PDB_SUCCESS;

  gint32 image_ID;
  gint32 drawable_ID;

  run_mode = param[0].data.d_int32;	
  *nreturn_vals = 1;
  *return_vals = values;
  values[0].type = GIMP_PDB_STATUS;
  values[0].data.d_status = GIMP_PDB_EXECUTION_ERROR;

  if (strcmp (name, PROCEDURE_NAME) == 0)
    {
      INIT_Il8N ();
      image_ID = load_djvu (param[1].data.d_string, run_mode, FALSE);

      if (image_ID != -1)
	{
	  *nreturn_vals = 2;
	  values[1].type = GIMP_PDB_IMAGE;
	  values[1].data.d_image = image_ID;
	}
      else
	{
	  printf ("did not open image");
	  status = GIMP_PDB_EXECUTION_ERROR;
	}
    }
/*
  else if (strcmp (name, PROCEDURE_NAME_SAVE) == 0)
    {
      image_ID = param[1].data.d_int32;
      drawable_ID = param[2].data.d_int32;
     // status = write_djvu(param[3].data.d_string, image_ID, drawable_ID);
    }
*/
  values[0].data.d_status = status;
}

INIT_Il8N ()
{
  bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#ifdef HAVE_BIND_TEXTDOMAIN_CODESET
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
#endif
  textdomain (GETTEXT_PACKAGE);
}
