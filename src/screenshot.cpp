#include "screenshot.h"

/* ----- CANVAS CLASS SETUP ----- */
wxBEGIN_EVENT_TABLE(Canvas, wxScrolledWindow)
EVT_PAINT(Canvas::OnPaint)
EVT_MOTION(Canvas::OnMouseMove)
EVT_LEFT_UP(Canvas::OnMouseUp)
EVT_LEFT_DOWN(Canvas::OnMouseDown)
EVT_ENTER_WINDOW(Canvas::OnEnterWin)
wxEND_EVENT_TABLE()

Canvas::Canvas(wxWindow* parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style):wxScrolledWindow(parent, id, pos, size,  wxHSCROLL | wxVSCROLL | wxNO_FULL_REPAINT_ON_RESIZE){
    // std::cout << "Screenshot taken" << std::endl;
    screenshot = takeScreenshot();
    SetVirtualSize(screenshot.GetSize());
    SetScrollRate(10,10);
    SetBackgroundColour(*wxWHITE);
}

// Canvas handlers and funcs
void Canvas::OnPaint(wxPaintEvent& event){
    wxClientDC dc(this);
    PrepareDC(dc);
    dc.DrawBitmap(screenshot, wxDefaultPosition, true);
}
void Canvas::OnMouseDown(wxMouseEvent& event){
    int x, y, xx, yy ;
    event.GetPosition(&x,&y);
    CalcUnscrolledPosition(x, y, &xx, &yy);
    cropStartPt = wxPoint(xx, yy);
    cropCurrPt = cropStartPt;
    active = true;
    CaptureMouse();
}
void Canvas::OnMouseUp(wxMouseEvent& event){
    if(active){
        ReleaseMouse();
        // Below syntax meant to avoid assertion error in .Reset call
        {
            wxClientDC dc( this );
            PrepareDC( dc );
            wxDCOverlay overlaydc( cropOverlay, &dc );
            overlaydc.Clear();
        }
        cropOverlay.Reset();
        cropEndPt = CalcUnscrolledPosition(event.GetPosition());
        // Ensure no errors
        ValidateEndPt();
        active = false;
        // Only crop and move on to next state iff cropped area != approximate straight line
        if(!(abs(cropStartPt.x-cropEndPt.x) < 10 || abs(cropStartPt.y-cropEndPt.y) < 10)){
           wxBitmap cropBmp = GetCroppedBitmap();
           MainWin* mainwin = (MainWin*) wxWindow::FindWindowById(WINDOW_MAIN);
           mainwin->ProcessData(cropBmp);
           SnipWin* snipwin = (SnipWin*) wxWindow::FindWindowById(WINDOW_SNIP);
           snipwin->Close();
        }else{
            std::string msg;
            msg.append("Selected area is too small, try again!");
            wxMessageBox( msg, "Information", wxOK | wxICON_INFORMATION );
        }
        
    }
}
void Canvas::OnMouseMove(wxMouseEvent& event){
    wxClientDC dc(this);
    PrepareDC(dc);
    this->PrepareDC(dc);
    wxPoint pos = event.GetPosition();

    long x = dc.DeviceToLogicalX( pos.x );
    long y = dc.DeviceToLogicalY( pos.y );

    if (active){
        int x,y, xx, yy ;
        event.GetPosition(&x,&y);
        CalcUnscrolledPosition( x, y, &xx, &yy );
        cropCurrPt = wxPoint( xx , yy ) ;
        wxRect newrect ( cropStartPt , cropCurrPt ) ;

        wxClientDC dc( this ) ;
        PrepareDC( dc ) ;

        wxDCOverlay overlaydc( cropOverlay, &dc );
        overlaydc.Clear();
        #ifdef __WXMAC__
        dc.SetPen( *wxGREY_PEN );
        dc.SetBrush( wxColour( 192,192,192,64 ) );
        #else
        dc.SetPen( wxPen( *wxLIGHT_GREY, 2 ) );
        dc.SetBrush( *wxTRANSPARENT_BRUSH );
        #endif
        dc.DrawRectangle( newrect );
    }
}

// Reverse engineer the start-end pts to get a wxRect for SubBitMap
wxRect Canvas::GetSubRect(){
    // Correct start-endpt for a rect is defined as starting @ Top Left, ending @ Bottom Right
    // Following ifelse statements purposely verbose for clarity
    // Invariant: The start and end points CANNOT be a line.
    // Case 1: Displacement = South East 
    if(cropStartPt.x < cropEndPt.x && cropStartPt.y < cropEndPt.y){
        return wxRect(cropStartPt, cropEndPt);
    }
    // Case 2: Displacement = North East 
    if(cropStartPt.x < cropEndPt.x && cropStartPt.y > cropEndPt.y){
        return wxRect(
            wxPoint(cropStartPt.x, cropEndPt.y),
            wxPoint(cropEndPt.x, cropStartPt.y));
    }
    // Case 3: Displacement = North West
    if(cropStartPt.x > cropEndPt.x && cropStartPt.y > cropEndPt.y){
        return wxRect(cropEndPt, cropStartPt);
    }
    //Case 4: Displacement = South West
    if(cropStartPt.x > cropEndPt.x && cropStartPt.y < cropEndPt.y){
        return wxRect(
            wxPoint(cropEndPt.x, cropStartPt.y),
            wxPoint(cropStartPt.x, cropEndPt.y)
        );
    }
    // Should not reach this point, but force default to case 1 for safety
    return wxRect(cropStartPt, cropEndPt);
}

wxBitmap Canvas::GetCroppedBitmap(){
    wxRect subRect = GetSubRect();
    wxBitmap croppedBitmap = screenshot.GetSubBitmap(subRect);
    // croppedBitmap.SaveFile("cropbm.jpg", wxBITMAP_TYPE_JPEG);
    return croppedBitmap;
}

void Canvas::ValidateEndPt(){
    // Force validate endpoint coords for out of range rectangles
    int ht, wd;
    this->GetVirtualSize(&wd, &ht);
    if(cropEndPt.x < 0) cropEndPt.x = 0;
    if(cropEndPt.x > wd) // gtk complains if x==wd. Set 1 px lesser
        cropEndPt.x = wd-1;
    if(cropEndPt.y < 0) cropEndPt.y = 0;
    if(cropEndPt.y > ht) // gtk compains if y==ht. Set 1 px lesser
        cropEndPt.y = ht - 1;
}

/* ----- SNIPWIN CLASS SETUP ----- */
wxBEGIN_EVENT_TABLE(SnipWin, wxFrame)
EVT_ENTER_WINDOW(SnipWin::OnEnterWin)
EVT_BUTTON(BUTTON_CANCEL, SnipWin::OnCancel)
EVT_CLOSE(SnipWin::OnExit)
wxEND_EVENT_TABLE()

SnipWin::SnipWin(wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size):wxFrame(NULL, id, title, pos, size){
    // Take a snapshot of the current desktop and display it
    wxBoxSizer* SnipWinLayout = new wxBoxSizer(wxVERTICAL);
    // Cancel Button Logic
    SnipWinLayout->Add(
        new wxButton(this, BUTTON_CANCEL, _T("CANCEL"), wxDefaultPosition, wxDefaultSize, 100),
        0, wxLEFT | wxTOP, 10
    );
    wxButton* btn = (wxButton*) wxWindow::FindWindowById(BUTTON_CANCEL);
    btn->SetBackgroundColour(wxColor(255,99,71));
    btn->SetForegroundColour(wxColor(*wxBLACK));
    // Canvas Logic (Containing Bitmap)
    SnipWinLayout->Add(
        new Canvas(this, BITMAP_SCREENSHOT, wxDefaultPosition, size, wxHSCROLL|wxVSCROLL),
        0, wxALL, 10
    );
    SetSizer(SnipWinLayout);
}

void SnipWin::OnCancel(wxCommandEvent& event){
    Close();
}

// Called when Close() or 'x' is clicked
void SnipWin::OnExit(wxCloseEvent& event){
    MainWin* mainwin = (MainWin*) wxWindow::FindWindowById(WINDOW_MAIN);
    mainwin->Show();
    Canvas* canvas = (Canvas*) wxWindow::FindWindowById(BITMAP_SCREENSHOT);
    this->Destroy();
}

/* SCREENSHOT LOGIC */
wxBitmap Canvas::takeScreenshot(){
    // Take a screenshot of the desktop and return its bitmap
    // Adapted from https://forums.wxwidgets.org/viewtopic.php?p=32313
    wxWindow::Refresh();
    wxWindow::Update();
    ::wxYield();
    wxMilliSleep(250);
    wxScreenDC screen;
    wxCoord screenWidth, screenHeight;
    screen.GetSize(&screenWidth, &screenHeight);
    // Init bitmap of screen size
    wxBitmap ss(screenWidth, screenHeight, -1);
    wxMemoryDC memDC;
    memDC.SelectObject(ss);
    memDC.Blit( 0, //Copy to this X coordinate
				0, //Copy to this Y coordinate
				screenWidth, //Copy this width
				screenHeight, //Copy this height
				&screen, //From where do we copy?
				0, //What's the X offset in the original DC?
				0  //What's the Y offset in the original DC?
			);
    // Framework bug causes multi-ss to fail, so.. 
    // ..blit screen as a workaround: https://github.com/wxWidgets/Phoenix/issues/259
    screen.Blit(0,0,screenWidth,screenHeight,&screen,0,0);
	memDC.SelectObject(wxNullBitmap);
    // Debug: save file for manual processing
	// ss.SaveFile("screenshot.jpg",wxBITMAP_TYPE_JPEG);
    return ss;
}
