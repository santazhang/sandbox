#include <wx/wx.h>
#include "wxhello.h"

BEGIN_EVENT_TABLE(wxhelloFrame, wxFrame)
	EVT_MENU(Menu_File_Quit, wxhelloFrame::OnQuit)
	EVT_MENU(Menu_File_About, wxhelloFrame::OnAbout)
END_EVENT_TABLE()

IMPLEMENT_APP(wxhelloapp)

bool wxhelloapp::OnInit() {
	wxhelloFrame *frame = new wxhelloFrame(wxT("Hello World"), wxPoint(50,50), wxSize(800,600));
	frame->Show(TRUE);
	SetTopWindow(frame);
	return TRUE;
}

wxhelloFrame::wxhelloFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame((wxFrame *)NULL, -1, title, pos, size) {
	wxMenu *menuFile = new wxMenu;
	
	menuFile->Append(Menu_File_About, wxT("&About..."));
	menuFile->AppendSeparator();
	menuFile->Append(Menu_File_Quit, wxT("E&xit"));
	
	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, wxT("&File"));
	
	SetMenuBar(menuBar);
	
	//CreateStatusBar();
	//SetStatusText(wxT("Welcome to Kdevelop wxWidgets app!"));
}

void wxhelloFrame::OnQuit(wxCommandEvent& WXUNUSED(event)) {
wxMessageBox(wxT("This is a wxWidgets Hello world sample"),
    wxT("About Hello World"), wxOK | wxICON_INFORMATION, this);
	Close(TRUE);
}

void wxhelloFrame::OnAbout(wxCommandEvent& WXUNUSED(event)) {
	wxMessageBox(wxT("This is a wxWidgets Hello world sample"),
		wxT("About Hello World"), wxOK | wxICON_INFORMATION, this);
}

