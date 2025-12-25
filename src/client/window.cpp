#include "window.h"
#include <unordered_set>

Window::Window()
    : wxFrame(nullptr, wxID_ANY, wxT("Spotlight"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))
{
    panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    textInput = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(1000, 30));
    mainSizer->Add(textInput, 0, wxALL | wxEXPAND, 10);

    syncButton = new wxButton(panel, wxID_ANY, "SYNC");
    mainSizer->Add(syncButton, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);
    syncButton->Bind(wxEVT_BUTTON, &Window::onSyncClicked, this);

    scrollableOutput = new wxScrolledWindow(panel, wxID_ANY);
    resultsSizer = new wxBoxSizer(wxVERTICAL);
    scrollableOutput->SetSizer(resultsSizer);

    mainSizer->Add(scrollableOutput, 1, wxALL | wxEXPAND, 10);

    scrollableOutput->SetScrollRate(5, 5);
    scrollableOutput->Show(false);

    panel->SetSizer(mainSizer);

    mainSizer->Fit(this);

    textInput->Bind(wxEVT_TEXT, &Window::onTextInput, this);

    this->Center();
}

void Window::onTextInput(wxCommandEvent &event) {
    const wxString text = textInput->GetValue();
    query = text.ToStdString();

    resultsSizer->Clear(true);

    bool hasResults = false;
    std::unordered_set<std::string> seenPaths;

    // Helper lambda to add results
    auto addResult = [&](const std::string& filename, const std::string& absolute_path) {
        wxPanel* cell = new wxPanel(scrollableOutput, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
        wxBoxSizer* cellSizer = new wxBoxSizer(wxVERTICAL);

        wxStaticText* nameLabel = new wxStaticText(cell, wxID_ANY, filename);
        wxFont nameFont = nameLabel->GetFont();
        nameFont.SetWeight(wxFONTWEIGHT_BOLD);
        nameFont.SetPointSize(nameFont.GetPointSize() + 1);
        nameLabel->SetFont(nameFont);

        wxStaticText* pathLabel = new wxStaticText(cell, wxID_ANY, absolute_path);
        pathLabel->SetForegroundColour(wxColour(105, 105, 105));

        cellSizer->Add(nameLabel, 0, wxLEFT | wxTOP | wxRIGHT, 5);
        cellSizer->Add(pathLabel, 0, wxLEFT | wxBOTTOM | wxRIGHT, 5);

        cell->SetSizer(cellSizer);
        resultsSizer->Add(cell, 0, wxALL | wxEXPAND, 3);
        hasResults = true;
    };

    if (!query.empty()) {
        auto trieResults = searchClient->trieSearch(query);
        for (const auto& res : trieResults) {
            if (seenPaths.find(res.absolute_path) == seenPaths.end()) {
                addResult(res.filename, res.absolute_path);
                seenPaths.insert(res.absolute_path);
            }
        }

        auto indexResults = searchClient->indexSearch(query);
        for (const auto& res : indexResults) {
            if (seenPaths.find(res.absolute_path) == seenPaths.end()) {
                addResult(res.filename, res.absolute_path);
                seenPaths.insert(res.absolute_path);
            }
        }
    }

    if (hasResults) {
        scrollableOutput->Show(true);
        scrollableOutput->SetMinSize(wxSize(-1, 300));
    } else {
        scrollableOutput->Show(false);
        scrollableOutput->SetMinSize(wxSize(-1, 0));
    }

    panel->GetSizer()->Layout();
    panel->GetSizer()->Fit(this);
}

void Window::onSyncClicked(wxCommandEvent& event) {
    // system("/usr/local/bin/indexer");
    // to do
}
