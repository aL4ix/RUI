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

/*******************************************************************
    SDL THREAD THREAD CLASS
*******************************************************************/
// A thread class that will periodically send events to the GUI thread,
// we use this as the SDL Events thread.

class MyThread : public wxThread
{
protected:
   wxEvtHandler* m_parent;

public:
    MyThread(wxEvtHandler *parent) : wxThread(), m_parent(parent)
    {
    };

   ExitCode Entry();
};


wxThread::ExitCode MyThread::Entry()
{

    wxThreadEvent *evt = new wxThreadEvent(wxEVT_THREAD);
    m_parent->QueueEvent(evt);
    return (wxThread::ExitCode) 0;
}

/********************************************************************
    SDL PANEL CLASS
*******************************************************************/

class MyPanel : public wxWindow
{
public:
    MyThread *thread;
    std::thread *render_thread;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    bool sdl_quit = false;
    MyPanel(wxWindow *parent);
    ~MyPanel();
    void OnTimer(wxTimerEvent &event);
    void render();
};



MyPanel::MyPanel(wxWindow *parent) : wxWindow (parent, wxID_ANY){

    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindowFrom(this->GetHandle());
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    render_thread = new std::thread(&MyPanel::render, this);

}


MyPanel::~MyPanel(){
    sdl_quit = true;
    render_thread->join();
    renderer = nullptr;
    SDL_Quit();
}


void MyPanel::render(){


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
        //Something so freakin' weird: SDL Does not catch arrows or enter keys (among others) when inside thread... WHYYY!?
        //All other keys are catched just fine.

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
    void close();
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnClose (wxCloseEvent &event);

private:
		wxStaticText *m_st;
		MyPanel *m_pan;

    DECLARE_EVENT_TABLE()
};

enum
{
    // menu items
    Minimal_Quit = wxID_EXIT,
    Minimal_About = wxID_ABOUT
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
    EVT_CLOSE(MyFrame::OnClose)
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
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, "&About\tF1", "Show about dialog");

    fileMenu->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");

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

// event handlers

void MyFrame::close(){

    Destroy();

}
void MyFrame::OnQuit(wxCommandEvent &event)
{
    // true is to force the frame to close
    Close(true);
    close();
}

void MyFrame::OnClose(wxCloseEvent &event){
    close();
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
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

