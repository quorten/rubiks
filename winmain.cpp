// Windows API + WGL main startup for the Rubik's Cube.
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include <windows.h>
#include <mmsystem.h>
#include <GL/gl.h>

#include "Vector.h"
#include "GfxOpenGL.h"
#include "Rubiks.h"

bool exiting = false;
bool drawFrame = false;
bool renderLoop = false;
bool leftArrowDwn = false, rightArrowDwn = false; //Arrow keys
bool upArrowDwn = false, dwnArrowDwn = false;
const int windowWidth = 800;
const int windowHeight = 600;
const int windowBits = 32;

bool fullscreen = false;
HDC hDC;
int lastTime;

GfxOpenGL* g_glRender = NULL;
RubiksCube* g_rubiksCube = NULL;

void RubiksSaveErrorDlg()
{
	MessageBox(NULL, "Your Rubik's Cube could not be saved.", NULL,
		MB_OK | MB_ICONERROR);
}

void RubiksLoadErrorDlg()
{
	MessageBox(NULL,
		"Your saved Rubik's Cube data was invalid and error corrected.\n"
		"It will not be the same as you last had it.",
		"Information", MB_OK | MB_ICONEXCLAMATION);
}

void SetupPixelFormat(HDC hDC)
{
	int pixelFormat;

	PIXELFORMATDESCRIPTOR pfd =
	{	
		sizeof(PIXELFORMATDESCRIPTOR),	// size
			1,							// version
			PFD_SUPPORT_OPENGL |		// OpenGL window
			PFD_DRAW_TO_WINDOW |		// render to window
			PFD_DOUBLEBUFFER,			// support double-buffering
			PFD_TYPE_RGBA,				// color type
			32,							// prefered color depth
			0, 0, 0, 0, 0, 0,			// color bits (ignored)
			0,							// no alpha buffer
			0,							// alpha bits (ignored)
			0,							// accumulation buffer
			0, 0, 0, 0,					// accum bits (ignored)
			24,							// depth buffer
			0,							// stencil buffer
			0,							// no auxiliary buffers
			PFD_MAIN_PLANE,				// main layer
			0,							// reserved
			0, 0, 0,					// no layer, visible, damage masks
	};

	pixelFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, pixelFormat, &pfd);
}

inline void EnableRenderLoop()
{
	if (renderLoop == false)
	{
		renderLoop = true;
		lastTime = timeGetTime();
	}
}

inline void DisableRenderLoop()
{
	if ((leftArrowDwn || rightArrowDwn ||
		dwnArrowDwn || upArrowDwn) == false)
		renderLoop = false;
}

LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HGLRC hRC;
	static bool lBtnDown = false, rBtnDown = false;
	int height, width;

	// dispatch messages
	switch (uMsg)
	{	
	case WM_CREATE:			// window creation
		hDC = GetDC(hWnd);
		SetupPixelFormat(hDC);
		//SetupPalette();
		hRC = wglCreateContext(hDC);
		wglMakeCurrent(hDC, hRC);
		PostMessage(hWnd, WM_PAINT, 0, 0);
		break;

	case WM_DESTROY:			// window destroy
	case WM_QUIT:
	case WM_CLOSE:					// windows is closing

		// deselect rendering context and delete it
		wglMakeCurrent(hDC, NULL);
		wglDeleteContext(hRC);
		ReleaseDC(hWnd, hDC);

		// send WM_QUIT to message queue
		PostQuitMessage(0);
		break;

	case WM_SIZE:
		height = HIWORD(lParam);		// retrieve width and height
		width = LOWORD(lParam);

		g_glRender->SetupProjection(width, height);
		if (wParam == SIZE_RESTORED || wParam == SIZE_MAXIMIZED)
			SendMessage(hWnd, WM_PAINT, 0, 0);

		break;

	case WM_ACTIVATEAPP:		// activate app
		break;

	case WM_CAPTURECHANGED:
		if ((HWND)lParam != hWnd)
		{
			lBtnDown = false;
			rBtnDown = false;
		}
		break;

	case WM_PAINT:				// paint
		drawFrame = true;
		break;

	case WM_LBUTTONDOWN:		// left mouse button
		//Capture the mouse
		SetCapture(hWnd);
		g_rubiksCube->FindMousePick((short)LOWORD(lParam), (short)HIWORD(lParam));
		PostMessage(hWnd, WM_PAINT, 0, 0);
		lBtnDown = true;
		break;

	case WM_RBUTTONDOWN:		// right mouse button
		//Capture the mouse
		SetCapture(hWnd);
		g_glRender->MouseRotate((short)LOWORD(lParam), (short)HIWORD(lParam), true);
		rBtnDown = true;
		break;

	case WM_MOUSEMOVE:			// mouse movement
		if (lBtnDown)
		{
			g_rubiksCube->RotateFace((short)LOWORD(lParam), (short)HIWORD(lParam));
			PostMessage(hWnd, WM_PAINT, 0, 0);
		}
		if (rBtnDown)
		{
			g_glRender->MouseRotate((short)LOWORD(lParam), (short)HIWORD(lParam), false);
			PostMessage(hWnd, WM_PAINT, 0, 0);
		}
		break;

	case WM_LBUTTONUP:			// left button release
		ReleaseCapture();
		lBtnDown = false;
		break;

	case WM_RBUTTONUP:			// right button release
		ReleaseCapture();
		rBtnDown = false;
		break;

	case WM_KEYUP:
		switch (wParam)
		{
		case VK_UP:
			upArrowDwn = false;
			if (dwnArrowDwn == false)
				g_glRender->RotatingUp(false);
			DisableRenderLoop();
			break;
		case VK_DOWN:
			dwnArrowDwn = false;
			if (upArrowDwn == false)
				g_glRender->RotatingDown(false);
			DisableRenderLoop();
			break;
		case VK_LEFT:
			leftArrowDwn = false;
			if (rightArrowDwn == false)
				g_glRender->RotatingLeft(false);
			DisableRenderLoop();
			break;
		case VK_RIGHT:
			rightArrowDwn = false;
			if (leftArrowDwn == false)
				g_glRender->RotatingRight(false);
			DisableRenderLoop();
			break;
		case VK_CONTROL:
			g_glRender->SetRotSpeed(false);
			break;
		}
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case 'Q':
			g_rubiksCube->SetFace(WHITE);
			break;
		case 'A':
			g_rubiksCube->SetFace(YELLOW);
			break;
		case 'W':
			g_rubiksCube->SetFace(GREEN);
			break;
		case 'S':
			g_rubiksCube->SetFace(BLUE);
			break;
		case 'E':
			g_rubiksCube->SetFace(RED);
			break;
		case 'D':
			g_rubiksCube->SetFace(ORANGE);
			break;
		case 'R':
			g_rubiksCube->RotateFace90(true);
			PostMessage(hWnd, WM_PAINT, 0, 0);
			break;
		case 'F':
			g_rubiksCube->RotateFace90(false);
			PostMessage(hWnd, WM_PAINT, 0, 0);
			break;
		case 'Z':
			g_rubiksCube->AlignRubiksCube();
			PostMessage(hWnd, WM_PAINT, 0, 0);
			break;
		case 'N':
			g_rubiksCube->NewRubiksCube();
			PostMessage(hWnd, WM_PAINT, 0, 0);
			break;
		case VK_UP:
			upArrowDwn = true;
			g_glRender->RotatingUp(true);
			EnableRenderLoop();
			break;
		case VK_DOWN:
			dwnArrowDwn = true;
			g_glRender->RotatingDown(true);
			EnableRenderLoop();
			break;
		case VK_LEFT:
			leftArrowDwn = true;
			g_glRender->RotatingLeft(true);
			EnableRenderLoop();
			break;
		case VK_RIGHT:
			rightArrowDwn = true;
			g_glRender->RotatingRight(true);
			EnableRenderLoop();
			break;
		case VK_CONTROL:
			g_glRender->SetRotSpeed(true);
			break;
		}
		break;

		default:
			break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	WNDCLASSEX windowClass;		// window class
	HWND		 hwnd;			// window handle
	MSG			 msg;				// message
	DWORD		 dwExStyle;		// Window Extended Style
	DWORD		 dwStyle;			// Window Style
	RECT		 windowRect;

	g_glRender = new GfxOpenGL;
	g_rubiksCube = new RubiksCube;

	windowRect.left=(long)0;						// Set Left Value To 0
	windowRect.right=(long)windowWidth;	// Set Right Value To Requested Width
	windowRect.top=(long)0;							// Set Top Value To 0
	windowRect.bottom=(long)windowHeight;	// Set Bottom Value To Requested Height

	// fill out the window class structure
	windowClass.cbSize			= sizeof(WNDCLASSEX);
	windowClass.style			= CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc		= MainWindowProc;
	windowClass.cbClsExtra		= 0;
	windowClass.cbWndExtra		= 0;
	windowClass.hInstance		= hInstance;
	windowClass.hIcon			= LoadIcon(hInstance, (LPCTSTR)101);// default icon
	windowClass.hCursor			= LoadCursor(NULL, IDC_ARROW);		// default arrow
	windowClass.hbrBackground	= NULL;								// don't need background
	windowClass.lpszMenuName	= NULL;								// no menu
	windowClass.lpszClassName	= "GLClass";
	windowClass.hIconSm			= NULL;

	// register the windows class
	if (!RegisterClassEx(&windowClass))
	{
		delete g_rubiksCube;
		delete g_glRender;
		return 0;
	}

	if (fullscreen)								// fullscreen?
	{
		DEVMODE dmScreenSettings;					// device mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);	
		dmScreenSettings.dmPelsWidth = windowWidth;			// screen width
		dmScreenSettings.dmPelsHeight = windowHeight;			// screen height
		dmScreenSettings.dmBitsPerPel = windowBits;				// bits per pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// 
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			// setting display mode failed, switch to windowed
			MessageBox(NULL, "Display mode failed", NULL, MB_OK);
			fullscreen = FALSE;	
		}
	}

	if (fullscreen)								// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;					// Window Extended Style
		dwStyle=WS_POPUP;						// Windows Style
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;					// Windows Style
	}

	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// class registered, so now create our window
	hwnd = CreateWindowEx(0,	 // extended style
		"GLClass",									// class name
		"Rubik's Cube", // app name
		dwStyle | WS_CLIPCHILDREN |
		WS_CLIPSIBLINGS,
		CW_USEDEFAULT, CW_USEDEFAULT,				 // x,y coordinate
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top, // width, height
		NULL,											 // handle to parent
		NULL,											 // handle to menu
		hInstance,									// application instance
		NULL);											// no extra params

	// check if window creation failed (hwnd would equal NULL)
	if (!hwnd)
		goto cleanup;

	ShowWindow(hwnd, SW_SHOW);			// display the window
	UpdateWindow(hwnd);					// update the window

	if (!g_glRender->Init())
		goto cleanup;

	g_rubiksCube->Init();

	drawFrame = true; // draw the first frame
	while (!exiting)
	{
		if (drawFrame || renderLoop)
		{
			int thisTime = timeGetTime();

			g_glRender->Prepare((float)(thisTime - lastTime)/1000.0f);
			lastTime = thisTime;

			g_glRender->Render();
			SwapBuffers(hDC);
			drawFrame = false;
		}

		while (!exiting && (!(drawFrame || renderLoop) ||
							PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE)))
		{
			if (GetMessage (&msg, NULL, 0, 0))
			{
				TranslateMessage (&msg);
				DispatchMessage (&msg);
			}
			else
				exiting = true;
		}
	}

cleanup:
	delete g_rubiksCube;
	delete g_glRender;

	if (fullscreen)
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
	}

	return (int)msg.wParam;
}
