#ifndef MAIN_H
#define MAIN_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

// #include <wx/wx.h>
#include <wx/display.h>
#include "wx/overlay.h"
#include "id.h"

// Common stdlib utils
#include <iostream>
#include <strings.h>
#include <regex>

// Main Window Declaration 
class MainWin : public wxFrame{
    public:
    // Constructor
        MainWin(wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style = wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER & ~wxMAXIMIZE_BOX);
    // Helper Methods
        void ProcessData(wxBitmap data);
    private:
    // Create handler prototype fns
        void OnAbout(wxCommandEvent& event);
        void OnSnip(wxCommandEvent& event);
        void OnCopy(wxCommandEvent& event);
        void OnOpenLink(wxTextUrlEvent& event);
        // Handle data processing & offload to opencv
        // Pretty data
        void OnEnterWin(wxMouseEvent& event){
            wxSetCursor(*wxSTANDARD_CURSOR);
        } 
        wxBitmap data;
    // IMPT: for a class to handle user events, declare event table
        wxDECLARE_EVENT_TABLE(); 
};

// Prototype fns
wxTextCtrl* getOutputTextbox();
void write_output_text(std::string&);

#endif //MAIN