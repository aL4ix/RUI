#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/thread.h>
#include <SDL2/SDL.h>
#include "mingw.thread.h"

/********************************************************************
    SDL PANEL CLASS
*******************************************************************/

class MyPanel : public wxWindow
{
public:
    //MyThread *thread;
    std::thread *render_thread;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    bool sdl_quit = false;
    MyPanel(wxWindow *parent);
    ~MyPanel();
    void OnTimer(wxTimerEvent &event);
    void render();
};


MyPanel::MyPanel(wxWindow *parent) : wxWindow (parent, wxID_ANY)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindowFrom(this->GetHandle());
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    render_thread = new std::thread(&MyPanel::render, this);
}


MyPanel::~MyPanel()
{
    sdl_quit = true;
    render_thread->join();
    renderer = nullptr;
    SDL_Quit();
}


void MyPanel::render()
{

    SDL_Rect cube;
    SDL_Color color;
    SDL_Color red = {255,0,0};
    SDL_Color blue = {0,0,255};
    int dirx = 1;
    int diry = 1;
    color = blue;
    cube.x = 0;
    cube.y = 0;
    cube.w = 20;
    cube.h = 20;
    SDL_Event evt;

	while (!sdl_quit){
        while (SDL_PollEvent(&evt))
        {
            if ( evt.type == SDL_KEYDOWN )
            {
                std::cout << "Key Down";
                color = red;
            }
            if ( evt.type == SDL_KEYUP )
            {
                std::cout << "Key Up";
                color = blue;
            }
        }

         if(cube.x >= 600) {
            dirx = -1;
        } else if(cube.x <= 0) {
            dirx = 1;
        }
        if(cube.y >= 380) {
            diry = -1;
        } else if(cube.y <= 0) {
            diry = 1;
        }

        cube.x += (1 * dirx);
        cube.y += (1 * diry);

        SDL_RenderClear( renderer );
        // Render our "cube"
        SDL_SetRenderDrawColor( renderer,color.r, color.g, color.b, 255 );
        SDL_RenderFillRect( renderer, &cube );
        //Background
        SDL_SetRenderDrawColor( renderer, 0, 255, 0, 255 );
        // Render the changes above
        SDL_RenderPresent( renderer);
        SDL_Delay(16);
	}
}


/*******************************************************************
    MAIN FRAME CLASS
*******************************************************************/

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title);
    void OnQuit(wxCommandEvent& event);
    void OnClose (wxCloseEvent &event);

private:
		wxStaticText *m_st;
		MyPanel *m_pan;

    DECLARE_EVENT_TABLE()
};

enum
{
    // menu items
    Minimal_Quit = wxID_EXIT
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
END_EVENT_TABLE()


MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title)
{

    // set the frame icon
    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    fileMenu->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(1);
    SetStatusText("You see the GUI thread not blocking the rendering routine. It fills you with determination!");
#endif // wxUSE_STATUSBAR

    m_pan = new MyPanel(this);
}

void MyFrame::OnQuit(wxCommandEvent &event)
{
    // true is to force the frame to close
    Close(true);
}

/*******************************************************************
    APPLICATION CLASS
*******************************************************************/

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{

    if ( !wxApp::OnInit() )
        return false;

    // create the main application window
    MyFrame *frame = new MyFrame("Minimal wxWidgets App");
    frame->SetSize(wxSize(640,480));
    frame->Show(true);
    SetTopWindow(frame);
    return true;
}

IMPLEMENT_APP(MyApp);

