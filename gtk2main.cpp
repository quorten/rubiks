// Platform independent main startup for the Rubiks Cube.
// Uses GTK+ and GtkGLExt

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include <gtk/gtkgl.h>

#ifdef G_OS_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include <ctime>

#include "Vector.h"
#include "GfxOpenGL.h"
#include "Rubiks.h"

static gboolean animate = FALSE;
bool left_arrow_dwn = false, right_arrow_dwn = false; // Arrow keys
bool up_arrow_dwn = false, dwn_arrow_dwn = false;
static clock_t last_time = 0;
GLfloat teapot_scale = 0.0f; // Special easter egg

GfxOpenGL* g_glRender = NULL;
RubiksCube* g_rubiksCube = NULL;

static void idle_add (GtkWidget *widget);
static void idle_remove (GtkWidget *widget);

void RubiksSaveErrorDlg()
{
	GtkWidget* dialog;
	dialog = gtk_message_dialog_new (NULL,
			 GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO,
			 GTK_BUTTONS_CLOSE, "Your Rubik's Cube could not be saved.");
	gtk_window_set_title (GTK_WINDOW (dialog), "Error");
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

void RubiksLoadErrorDlg()
{
	GtkWidget* dialog;
	dialog = gtk_message_dialog_new (NULL,
			 GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO,
			 GTK_BUTTONS_CLOSE, "Your saved Rubik's Cube data was invalid and error corrected.\nIt will not be the same as you last had it.");
	gtk_window_set_title (GTK_WINDOW (dialog), "Information");
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

static void EnableRenderLoop(GtkWidget* widget)
{
	if (!animate)
	{
		animate = TRUE;
		idle_add (widget);
	}
}

static void DisableRenderLoop(GtkWidget* widget)
{
	if ((left_arrow_dwn || right_arrow_dwn ||
		dwn_arrow_dwn || up_arrow_dwn) == false)
	{
		animate = FALSE;
		idle_remove (widget);
		gdk_window_invalidate_rect (widget->window, &widget->allocation, FALSE);
	}
}

static void
realize (GtkWidget *widget, gpointer data)
{
	GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
	GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);

	/*** OpenGL BEGIN ***/
	if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext))
		return;

	if (!g_glRender->Init())
	{
		//delete g_glRender;
		gtk_main_quit ();
		return;
	}
	g_rubiksCube->Init();

	gdk_gl_drawable_gl_end (gldrawable);
	/*** OpenGL END ***/

	return;
}

static gboolean
configure_event (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
	GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);

	unsigned w = widget->allocation.width;
	unsigned h = widget->allocation.height;

	/*** OpenGL BEGIN ***/
	if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext))
		return FALSE;

	g_glRender->SetupProjection(w, h);

	gdk_gl_drawable_gl_end (gldrawable);
	/*** OpenGL END ***/

	return TRUE;
}

static gboolean
expose_event (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
	GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);

	/*** OpenGL BEGIN ***/
	if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext))
		return FALSE;

#ifdef G_OS_WIN32
	/* On Windows, `clock ()' actually measures "wall clock" monotonic
	   elapsed real time rather than processor time.  Also, this is
	   easier to compile than `clock_gettime ()' because that comes
	   from `winpthread.dll' rather than `msvcrt.dll'.  */
	clock_t this_time = clock ();
	float dt = (float)(last_time - this_time) / CLOCKS_PER_SEC;
#else
	struct timespec tp;
	clock_gettime(CLOCK_MONOTONIC, &tp);
	clock_t this_time = tp.tv_sec * 1000 + tp.tv_nsec / 1000000;
	float dt = (float)(last_time - this_time) / 1000.0f;
#endif
	g_glRender->Prepare(dt);
	last_time = this_time;
	g_glRender->Render();
#ifdef IDLE_BUG
	// Kludge for bug workaround.
	// The bug is somewhere in an Ubuntu 11.04 GNU/Linux system.
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glColor3f(0.75f, 1.0f, 0.9f);
	gdk_gl_draw_teapot(TRUE, teapot_scale);
	glShadeModel(GL_FLAT);
	glPopAttrib();
#endif

	// Swap buffers
	if (gdk_gl_drawable_is_double_buffered (gldrawable))
		gdk_gl_drawable_swap_buffers (gldrawable);
	else
		glFlush ();

	gdk_gl_drawable_gl_end (gldrawable);
	/*** OpenGL END ***/

	return TRUE;
}

static gboolean
button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	if (event->button == 1)
	{
		g_rubiksCube->FindMousePick((int)event->x, (int)event->y);
		gdk_window_invalidate_rect (widget->window, &widget->allocation, FALSE);
	}
	if (event->button == 3)
		g_glRender->MouseRotate((int)event->x, (int)event->y, true);

	return FALSE;
}

static gboolean
button_release_event (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	return FALSE;
}

static gboolean
motion_notify_event (GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
	gboolean redraw = FALSE;

	if (event->state & GDK_BUTTON1_MASK)
    {
		g_rubiksCube->RotateFace((int)event->x, (int)event->y);
		redraw = TRUE;
    }

	if (event->state & GDK_BUTTON3_MASK)
    {
		g_glRender->MouseRotate((int)event->x, (int)event->y, false);
		redraw = TRUE;
    }

	if (redraw && !animate)
		gdk_window_invalidate_rect (widget->window, &widget->allocation, FALSE);

	return TRUE;
}

static gboolean
scroll_event (GtkWidget *widget, GdkEventScroll *event, gpointer data)
{
	if (event->direction == GDK_SCROLL_UP)
		teapot_scale -= 0.1f;
	if (event->direction == GDK_SCROLL_DOWN)
		teapot_scale += 0.1f;
	gdk_window_invalidate_rect (widget->window, &widget->allocation, FALSE);
}

static gboolean
key_press_event (GtkWidget *widget, GdkEventKey *event, gpointer data)
{
	switch (event->keyval)
    {
    case GDK_Escape:
		gtk_main_quit ();
		break;
	case 'q':
		g_rubiksCube->SetFace(WHITE);
		break;
	case 'a':
		g_rubiksCube->SetFace(YELLOW);
		break;
	case 'w':
		g_rubiksCube->SetFace(GREEN);
		break;
	case 's':
		g_rubiksCube->SetFace(BLUE);
		break;
	case 'e':
		g_rubiksCube->SetFace(RED);
		break;
	case 'd':
		g_rubiksCube->SetFace(ORANGE);
		break;
	case 'r':
		g_rubiksCube->RotateFace90(true);
		gdk_window_invalidate_rect (widget->window, &widget->allocation, FALSE);
		break;
	case 'f':
		g_rubiksCube->RotateFace90(false);
		gdk_window_invalidate_rect (widget->window, &widget->allocation, FALSE);
		break;
	case 'z':
		g_rubiksCube->AlignRubiksCube();
		gdk_window_invalidate_rect (widget->window, &widget->allocation, FALSE);
		break;
	case 'n':
		g_rubiksCube->NewRubiksCube();
		gdk_window_invalidate_rect (widget->window, &widget->allocation, FALSE);
		break;
	case GDK_Up:
		up_arrow_dwn = true;
		g_glRender->RotatingUp(true);
		EnableRenderLoop(widget);
		break;
	case GDK_Down:
		dwn_arrow_dwn = true;
		g_glRender->RotatingDown(true);
		EnableRenderLoop(widget);
		break;
	case GDK_Left:
		left_arrow_dwn = true;
		g_glRender->RotatingLeft(true);
		EnableRenderLoop(widget);
		break;
	case GDK_Right:
		right_arrow_dwn = true;
		g_glRender->RotatingRight(true);
		EnableRenderLoop(widget);
		break;
	case GDK_Control_L:
	case GDK_Control_R:
		g_glRender->SetRotSpeed(true);
		break;
    default:
		return FALSE;
    }

	return TRUE;
}

static gboolean
key_release_event (GtkWidget *widget, GdkEventKey *event, gpointer data)
{
	switch (event->keyval)
    {
	case GDK_Up:
		up_arrow_dwn = false;
		if (dwn_arrow_dwn == false)
			g_glRender->RotatingUp(false);
		DisableRenderLoop(widget);
		break;
	case GDK_Down:
		dwn_arrow_dwn = false;
		if (up_arrow_dwn == false)
			g_glRender->RotatingDown(false);
		DisableRenderLoop(widget);
		break;
	case GDK_Left:
		left_arrow_dwn = false;
		if (right_arrow_dwn == false)
			g_glRender->RotatingLeft(false);
		DisableRenderLoop(widget);
		break;
	case GDK_Right:
		right_arrow_dwn = false;
		if (left_arrow_dwn == false)
			g_glRender->RotatingRight(false);
		DisableRenderLoop(widget);
		break;
	case GDK_Control_L:
	case GDK_Control_R:
		g_glRender->SetRotSpeed(false);
		break;

    default:
		return FALSE;
    }

	return TRUE;
}

static gboolean
idle (GtkWidget *widget)
{
	/* Invalidate the whole window. */
	gdk_window_invalidate_rect (widget->window, &widget->allocation, FALSE);

	/* Update synchronously. */
	gdk_window_process_updates (widget->window, FALSE);

	return TRUE;
}

static guint idle_id = 0;

static void
idle_add (GtkWidget *widget)
{
	if (idle_id == 0)
    {
		idle_id = g_idle_add_full (GDK_PRIORITY_REDRAW,
								   (GSourceFunc) idle, widget, NULL);
#ifdef G_OS_WIN32
		/* On Windows, `clock ()' actually measures "wall clock"
		   monotonic elapsed real time rather than processor time.
		   Also, this is easier to compile than `clock_gettime ()'
		   because that comes from `winpthread.dll' rather than
		   `msvcrt.dll'.  */
		last_time = clock ();
#else
		struct timespec tp;
		clock_gettime(CLOCK_MONOTONIC, &tp);
		last_time = tp.tv_sec * 1000 + tp.tv_nsec / 1000000;
#endif
    }
}

static void
idle_remove (GtkWidget *widget)
{
	if (idle_id != 0)
    {
		g_source_remove (idle_id);
		idle_id = 0;
    }
}

static gboolean
map_event (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	if (animate)
		idle_add (widget);

	return TRUE;
}

static gboolean
unmap_event (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	idle_remove (widget);

	return TRUE;
}

static gboolean
visibility_notify_event (GtkWidget *widget, GdkEventVisibility *event,
						 gpointer data)
{
	if (animate)
    {
		if (event->state == GDK_VISIBILITY_FULLY_OBSCURED)
			idle_remove (widget);
		else
			idle_add (widget);
    }

	return TRUE;
}

/* Toggle animation.*/
static void
toggle_animation (GtkWidget *widget)
{
	animate = !animate;

	if (animate)
    {
		idle_add (widget);
    }
	else
    {
		idle_remove (widget);
		gdk_window_invalidate_rect (widget->window, &widget->allocation, FALSE);
    }
}

static void
print_gl_config_attrib (GdkGLConfig *glconfig,
                        const gchar *attrib_str,
                        int          attrib,
                        gboolean     is_boolean)
{
  int value;

  g_print ("%s = ", attrib_str);
  if (gdk_gl_config_get_attrib (glconfig, attrib, &value))
    {
      if (is_boolean)
        g_print ("%s\n", value == TRUE ? "TRUE" : "FALSE");
      else
        g_print ("%d\n", value);
    }
  else
    g_print ("*** Cannot get %s attribute value\n", attrib_str);
}

static void
examine_gl_config_attrib (GdkGLConfig *glconfig)
{
  g_print ("\nOpenGL visual configurations :\n\n");

  g_print ("gdk_gl_config_is_rgba (glconfig) = %s\n",
           gdk_gl_config_is_rgba (glconfig) ? "TRUE" : "FALSE");
  g_print ("gdk_gl_config_is_double_buffered (glconfig) = %s\n",
           gdk_gl_config_is_double_buffered (glconfig) ? "TRUE" : "FALSE");
  g_print ("gdk_gl_config_is_stereo (glconfig) = %s\n",
           gdk_gl_config_is_stereo (glconfig) ? "TRUE" : "FALSE");
  g_print ("gdk_gl_config_has_alpha (glconfig) = %s\n",
           gdk_gl_config_has_alpha (glconfig) ? "TRUE" : "FALSE");
  g_print ("gdk_gl_config_has_depth_buffer (glconfig) = %s\n",
           gdk_gl_config_has_depth_buffer (glconfig) ? "TRUE" : "FALSE");
  g_print ("gdk_gl_config_has_stencil_buffer (glconfig) = %s\n",
           gdk_gl_config_has_stencil_buffer (glconfig) ? "TRUE" : "FALSE");
  g_print ("gdk_gl_config_has_accum_buffer (glconfig) = %s\n",
           gdk_gl_config_has_accum_buffer (glconfig) ? "TRUE" : "FALSE");

  g_print ("\n");

  print_gl_config_attrib (glconfig, "GDK_GL_USE_GL",           GDK_GL_USE_GL,           TRUE);
  print_gl_config_attrib (glconfig, "GDK_GL_BUFFER_SIZE",      GDK_GL_BUFFER_SIZE,      FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_LEVEL",            GDK_GL_LEVEL,            FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_RGBA",             GDK_GL_RGBA,             TRUE);
  print_gl_config_attrib (glconfig, "GDK_GL_DOUBLEBUFFER",     GDK_GL_DOUBLEBUFFER,     TRUE);
  print_gl_config_attrib (glconfig, "GDK_GL_STEREO",           GDK_GL_STEREO,           TRUE);
  print_gl_config_attrib (glconfig, "GDK_GL_AUX_BUFFERS",      GDK_GL_AUX_BUFFERS,      FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_RED_SIZE",         GDK_GL_RED_SIZE,         FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_GREEN_SIZE",       GDK_GL_GREEN_SIZE,       FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_BLUE_SIZE",        GDK_GL_BLUE_SIZE,        FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_ALPHA_SIZE",       GDK_GL_ALPHA_SIZE,       FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_DEPTH_SIZE",       GDK_GL_DEPTH_SIZE,       FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_STENCIL_SIZE",     GDK_GL_STENCIL_SIZE,     FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_ACCUM_RED_SIZE",   GDK_GL_ACCUM_RED_SIZE,   FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_ACCUM_GREEN_SIZE", GDK_GL_ACCUM_GREEN_SIZE, FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_ACCUM_BLUE_SIZE",  GDK_GL_ACCUM_BLUE_SIZE,  FALSE);
  print_gl_config_attrib (glconfig, "GDK_GL_ACCUM_ALPHA_SIZE", GDK_GL_ACCUM_ALPHA_SIZE, FALSE);

  g_print ("\n");
}

int
main (int argc, char *argv[])
{
	GdkGLConfig *glconfig;
	gint major, minor;

	GtkWidget *window;
	GtkWidget *drawing_area;

	// Initialize GTK.
	gtk_init (&argc, &argv);

	// Initialize GtkGLExt.
	gtk_gl_init (&argc, &argv);

	/*
	 * Query OpenGL extension version.
	 */

	gdk_gl_query_version (&major, &minor);
	g_print ("\nOpenGL extension version - %d.%d\n",
			 major, minor);

	/*
	 * Configure OpenGL-capable visual.
	 */

	// Try double-buffered visual
	// Need 24-bit depth buffer
	glconfig = gdk_gl_config_new_by_mode (
		(GdkGLConfigMode)(GDK_GL_MODE_RGB | GDK_GL_MODE_DEPTH |
						  GDK_GL_MODE_DOUBLE));
	if (glconfig == NULL)
    {
		g_print ("*** Cannot find the double-buffered visual.\n");
		g_print ("*** Trying single-buffered visual.\n");

		// Try single-buffered visual
		glconfig = gdk_gl_config_new_by_mode (
			(GdkGLConfigMode)(GDK_GL_MODE_RGB | GDK_GL_MODE_DEPTH));
		if (glconfig == NULL)
		{
			g_print ("*** No appropriate OpenGL-capable visual found.\n");
			return 1;
		}
    }

	examine_gl_config_attrib (glconfig);

	// Configure platform independent code.
	g_glRender = new GfxOpenGL;
	g_rubiksCube = new RubiksCube;

	/*
	 * Top-level window.
	 */

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "Rubik's Cube");

	// Get automatically redrawn if any of their children changed allocation.
	gtk_container_set_reallocate_redraws (GTK_CONTAINER (window), TRUE);

	g_signal_connect (G_OBJECT (window), "delete_event",
					  G_CALLBACK (gtk_main_quit), NULL);

	/*
	 * Drawing area for drawing OpenGL scene.
	 */

	drawing_area = gtk_drawing_area_new ();
	gtk_container_add (GTK_CONTAINER (window), drawing_area);
	gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);

	// Set OpenGL-capability to the widget.
	gtk_widget_set_gl_capability (drawing_area,
								  glconfig,
								  NULL,
								  TRUE,
								  GDK_GL_RGBA_TYPE);

	gtk_widget_add_events (drawing_area,
						   GDK_BUTTON1_MOTION_MASK    |
						   GDK_BUTTON3_MOTION_MASK    |
						   GDK_BUTTON_PRESS_MASK      |
						   GDK_BUTTON_RELEASE_MASK    |
						   GDK_VISIBILITY_NOTIFY_MASK);

	g_signal_connect_after (G_OBJECT (drawing_area), "realize",
							G_CALLBACK (realize), NULL);
	g_signal_connect (G_OBJECT (drawing_area), "configure_event",
					  G_CALLBACK (configure_event), NULL);
	g_signal_connect (G_OBJECT (drawing_area), "expose_event",
					  G_CALLBACK (expose_event), NULL);

	g_signal_connect (G_OBJECT (drawing_area), "button_press_event",
					  G_CALLBACK (button_press_event), NULL);
	g_signal_connect (G_OBJECT (drawing_area), "button_release_event",
					  G_CALLBACK (button_release_event), NULL);
	g_signal_connect (G_OBJECT (drawing_area), "motion_notify_event",
					  G_CALLBACK (motion_notify_event), NULL);
	g_signal_connect (G_OBJECT (drawing_area), "scroll_event",
					  G_CALLBACK (scroll_event), NULL);

	g_signal_connect (G_OBJECT (drawing_area), "map_event",
					  G_CALLBACK (map_event), NULL);
	g_signal_connect (G_OBJECT (drawing_area), "unmap_event",
					  G_CALLBACK (unmap_event), NULL);
	g_signal_connect (G_OBJECT (drawing_area), "visibility_notify_event",
					  G_CALLBACK (visibility_notify_event), NULL);

	g_signal_connect_swapped (G_OBJECT (window), "key_press_event",
							  G_CALLBACK (key_press_event), drawing_area);
	g_signal_connect_swapped (G_OBJECT (window), "key_release_event",
							  G_CALLBACK (key_release_event), drawing_area);

	gtk_widget_show (drawing_area);

	gtk_widget_show (window);

	// Main loop.
	gtk_main ();

	delete g_rubiksCube;
	delete g_glRender;

	return 0;
}
