
#ifndef _WXHELLO_H_
#define _WXHELLO_H_

/**
 * @short Application Main Window
 * @author Santa Zhang <santa@santa-laptop>
 * @version 0.1
 */

class wxhelloapp : public wxApp {
 public:
  virtual bool OnInit();
};

class wxhelloFrame : public wxFrame {
 public:
  wxhelloFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);

 private:
  DECLARE_EVENT_TABLE()
};

enum {
  Menu_File_Quit = 100,
  Menu_File_About
};

#endif // _WXHELLO_H_
