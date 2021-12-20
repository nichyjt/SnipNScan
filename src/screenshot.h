#ifndef SCREENSHOT
#define SCREENSHOT

#include "id.h"
#include "main.h"

class SnipWin : public wxFrame{
    public:
        SnipWin(wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size);
    private:
        void OnExit(wxCloseEvent& event); //handle premature exit
        void OnCancel(wxCommandEvent& event);
        void OnEnterWin(wxMouseEvent& event){
            wxSetCursor(*wxSTANDARD_CURSOR);
        }
        wxDECLARE_EVENT_TABLE();
};

class Canvas : public wxScrolledWindow{
    public:
    Canvas(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style);
    // Hold screenshot data
    wxBitmap screenshot;
    private:
    // Handle drawing of screenshot and other nonsense
    void OnPaint(wxPaintEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    bool active = false;
    wxPoint cropStartPt;
    wxPoint cropEndPt;
    wxPoint cropCurrPt;
    wxOverlay cropOverlay;
    wxRect GetSubRect();
    wxBitmap GetCroppedBitmap();
    wxBitmap takeScreenshot();
    void ValidateEndPt();
    // Pretty UI
    void OnEnterWin(wxMouseEvent& event){
        wxSetCursor(*wxCROSS_CURSOR);
    }
    wxDECLARE_EVENT_TABLE();
};

// UTIL PROTOTYPES
wxBitmap getScreenshot();

#endif //SCREENSHOT