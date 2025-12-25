#include "window.h"

Window::Window()
    : wxFrame(nullptr, wxID_ANY, wxT("Spotlight"), wxDefaultPosition, wxSize(360, 40))
{
    panel = new wxPanel(this, wxID_ANY);

    textInput = new wxTextCtrl(panel, wxID_ANY,
                              "",
                              wxPoint(10, 10),
                              wxSize(350, 30));

    textInput->Bind(wxEVT_TEXT, &Window::onTextInput, this);

}


void Window::onTextInput(wxCommandEvent &event) {
    wxString text = textInput->GetValue();
    query = text.ToStdString();
    std::cout<< "INVERTED INDEX RESULTS "<< std::endl;
    searchClient->indexSearch(query);

    std::cout<< "TRIE RESULTS "<< std::endl;
    searchClient->trieSearch(query);
    std::cout << std::endl;

}



