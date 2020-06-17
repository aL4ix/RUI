#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/thread.h>
#include <SDL2/SDL.h>


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

class MyPanel : public wxPanel
{
public:
    MyPanel(wxWindow *parent) : wxPanel (parent, wxID_ANY){

        SDL_Init(SDL_INIT_EVERYTHING);
        window = SDL_CreateWindowFrom(this->GetHandle());
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

        m_timer.SetOwner(this);
        m_timer.Start(15);
        thread = new MyThread( this );

        cube.x = 0;
        cube.y = 0;
        cube.w = 20;
        cube.h = 20;

        if( thread->Create()==wxTHREAD_NO_ERROR )
        {
            thread->Run();
        }
    }
    ~MyPanel(){
        renderer = nullptr;
        SDL_Quit();
    }

    MyThread *thread;
    wxTimer m_timer;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Rect cube;

    bool isCubeGoingRight = true;
    bool isCubeGoingDown = true;

    bool sdl_quit = false;
    void OnThreadEvent(wxThreadEvent &event);
    void OnTimer(wxTimerEvent &event);

   DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(MyPanel, wxPanel)
    EVT_THREAD(wxID_ANY, MyPanel::OnThreadEvent)
    EVT_TIMER(-1,MyPanel::OnTimer)
END_EVENT_TABLE()


void MyPanel::OnThreadEvent(wxThreadEvent &event)
{

    SDL_Event evt;
	while (!sdl_quit){
        while (SDL_PollEvent(&evt)) {


            if ( evt.type == SDL_KEYDOWN )
            {
                std::cout << "KeyPress" << std::endl;
            }
        }

        SDL_Delay(16);

	}
    std::cout << "Quit" << std::endl;

}


void MyPanel::OnTimer(wxTimerEvent &event)
{
    /*This event seems to run even if the GUI is waiting on events. I use this kind of a update->render thread, while
    the input is handled by another thread.

    You see the GUI thread not blocking the rendering routine. It fills you with determination. */

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

    SDL_RenderClear( renderer );
    // Change color to blue
    SDL_SetRenderDrawColor( renderer, 0, 0, 255, 255 );
    // Render our "cube"
    SDL_RenderFillRect( renderer, &cube );
    // Change color to green
    SDL_SetRenderDrawColor( renderer, 0, 255, 0, 255 );
    // Render the changes above
    SDL_RenderPresent( renderer);

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
    CreateStatusBar(2);
    SetStatusText("Welcome to wxWidgets!");
#endif // wxUSE_STATUSBAR

    m_pan = new MyPanel(this);

}

// event handlers

void MyFrame::close(){

    m_pan->sdl_quit = true;
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
    frame->SetSize(wxSize(600,400));
    frame->Show(true);
    SetTopWindow(frame);
    return true;
}

IMPLEMENT_APP(MyApp);
