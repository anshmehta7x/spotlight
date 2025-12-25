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

    wxButton* syncButton;
    void onSyncClicked(wxCommandEvent& event);

    wxTextCtrl* textInput;
    wxScrolledWindow* scrollableOutput;
    wxBoxSizer* resultsSizer;


    std::string query;
    void onTextInput(wxCommandEvent& event);

};

#endif // WINDOW_H
