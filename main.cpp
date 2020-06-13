#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <SDL2/SDL.h>
#include <iostream>

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("RUIClass");

SDL_Rect cube;
SDL_Renderer* renderer = NULL;
bool isCubeGoingRight = true;
bool isCubeGoingDown = true;

int Render(void *data) {
    for(int i=0; i<1000; i++) {
        //Animation
        if(isCubeGoingRight) {
            ++cube.x;
        } else {
            --cube.x;
        }
        if(isCubeGoingDown) {
            ++cube.y;
        } else {
            --cube.y;
        }
        if(cube.x >= 500) {
            isCubeGoingRight = false;
        } else if(cube.x <= 0) {
            isCubeGoingRight = true;
        }
        if(cube.y >= 300) {
            isCubeGoingDown = false;
        } else if(cube.y <= 0) {
            isCubeGoingDown = true;
        }
    // Clear the window and make it all green
    SDL_RenderClear( renderer );
    // Change color to blue
    SDL_SetRenderDrawColor( renderer, 0, 0, 255, 255 );
    // Render our "cube"
    SDL_RenderFillRect( renderer, &cube );
    // Change color to green
    SDL_SetRenderDrawColor( renderer, 0, 255, 0, 255 );
    // Render the changes above
    SDL_RenderPresent( renderer);
    SDL_Delay( 16 );
    }

}

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_HREDRAW | CS_VREDRAW;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("RUI"),       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           600,                 /* The programs width */
           400,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );
    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    // WinAPI CODE: A menu example
    HMENU hMenu, hSubMenu;
    hMenu = CreateMenu();
    hSubMenu = CreatePopupMenu();
    AppendMenu(hSubMenu, MF_STRING, 9001, "E&xit");
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&File");
    hSubMenu = CreatePopupMenu();
    AppendMenu(hSubMenu, MF_STRING, 9002, "&Spam");
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&Edit");
    SetMenu(hwnd, hMenu);

    /*
    //Original message loop, this was replaced by an SDL loop
    while (GetMessage (&messages, NULL, 0, 0))
    {
        // Translate virtual-key messages into character messages
        TranslateMessage(&messages);
        // Send message to WindowProcedure
        DispatchMessage(&messages);
    }
    */

    // FROM HERE IS NORMAL CODE
    // Init everything
    if ( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
	{
		std::cout << " Failed to initialize SDL: " << SDL_GetError() << std::endl;
		return false;
	}
	SDL_Window* sdl_window = SDL_CreateWindowFrom(hwnd);
	if (sdl_window == NULL) {
        std::cerr << "SDL CreateWindowFrom ERROR: " << SDL_GetError() << std::endl;
        return 0;
    }
    renderer = SDL_CreateRenderer(sdl_window, -1, 0);
    if (renderer == NULL) {
        std::cerr << "SDL: failed to create renderer: " << SDL_GetError();
    }

    // Init vars

    cube.x = 0;
	cube.y = 0;
	cube.w = 20;
	cube.h = 20;
    bool loop = true;
    //Stuff for the animation
    SDL_Thread* threadID = SDL_CreateThread(Render, "Render", NULL);

    //Our very own main loop
	while ( loop )
	{
		SDL_Event event;
		while ( SDL_PollEvent( &event ) )
		{
			if ( event.type == SDL_QUIT )
				loop = false;
			else if ( event.type == SDL_KEYDOWN )
			{
				switch ( event.key.keysym.sym )
				{
					case SDLK_RIGHT:
						cube.x += 2;
						break;
					case SDLK_LEFT:
						cube.x -= 2;
						break;
					case SDLK_DOWN:
						cube.y += 2;
						break;
					case SDLK_UP:
						cube.y -= 2;
						break;
					default :
						break;
				}
			}
		}



        //Render();

		// Add a 16msec delay to make our game run at ~60 fps
		SDL_Delay( 16 );
	}


    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //std::cout << 'E' << message << ' ';
    switch (message)                  /* handle the messages */
    {
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        /*case WM_SYSCOMMAND:
            // disallow screen saver while emulating (doesn't work if password protected)
            if ((((wParam & 0xFFF0) == SC_SCREENSAVE) || ((wParam & 0xFFF0) == SC_MONITORPOWER)))
                return 0;*/
        /*case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                //Render();
                EndPaint(hwnd, &ps);
            }
            break;*/
        default:                      /* for messages that we don't deal with */
            LRESULT a = DefWindowProc (hwnd, message, wParam, lParam);
            //std::cout << 'X' << message << std::endl;
            return a;
    }

    return 0;
}
