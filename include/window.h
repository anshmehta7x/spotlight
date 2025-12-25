#ifndef WINDOW_H
#define WINDOW_H

#include <wx/wx.h>

#include "client.h"

class Window : public wxFrame {
public:
    Window();
    Client* searchClient;

private:
    wxPanel* panel;
    wxTextCtrl* textInput;
    std::string query;
    void onTextInput(wxCommandEvent& event);

};

#endif // WINDOW_H
