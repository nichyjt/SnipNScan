#include "main.h"
#include "id.h"
#include "screenshot.h"
#include "ocv.h"
// Nicholas Yek © 2021

/* ----- MAIN ----- 
* This file contains the primary UI logic of the app using wxWidgets
* Its main function is to control main UI as well as states between screenshot phase
*/

// Prototype fns
wxTextCtrl* getOutputTextbox();
void write_output_text(std::string&);

// Define a new class from wxapp to initialise window
class MyApp : public wxApp {
    public:
    // Called on application startup
        virtual bool OnInit(); // Note: virtual allows for overriding
};

// Reference to MainWin constructor
MainWin::MainWin(wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):wxFrame(NULL, id, title, pos, size, style){
    /* ----- MAINWINDOW UI DECLARATIONS ----- */
    // TOPBAR MENU
    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    // info tab
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuHelp, "&Info" );
    SetMenuBar(menuBar);
    // BOTTOM BAR
    CreateStatusBar();
    SetStatusText( "" );
    // LAYOUT CONTAINERS
    wxBoxSizer* TopSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* TopCtlCont = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* BtmCtlCont = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* BtmBtnCont = new wxBoxSizer(wxVERTICAL);
    
    /* ----- BUTTONS AND UI STUFF ----- */

    // 20px padding
    TopCtlCont->Add(20, 10, 0);
    TopCtlCont->Add(
        new wxButton(this, BUTTON_Snip, _T("Scan Stuff!"), wxDefaultPosition, wxSize(20,50), 0),
        1, wxALIGN_LEFT, 10
    );
    wxButton* btn = (wxButton*) wxWindow::FindWindowById(BUTTON_Snip);
    btn->SetBackgroundColour(wxColor(152,251,152));
    btn->SetForegroundColour(wxColor(*wxBLACK));
    // Padding
    TopCtlCont->Add(90, 10, 0);
    TopCtlCont->Add(
        new wxStaticText(this, TEXT_Mode, "Detect Mode:", wxDefaultPosition, wxDefaultSize, 0),
        1, wxALIGN_CENTER, 10
    );
    TopCtlCont->Add(10,10,0);
    const wxString choices[3] = {"Auto", "QR Code", "URL/Text"};
    TopCtlCont->Add(
        new wxChoice(this, CHOICE_Mode, wxDefaultPosition, wxDefaultSize,
         3, choices),
        1, wxALIGN_RIGHT | wxALIGN_CENTER, 10
    );
    // Init Default Option
    wxChoice* choice = (wxChoice*) wxWindow::FindWindowById(CHOICE_Mode);
    choice->SetSelection(0);
    TopCtlCont->Add(20, 10, 0);

    BtmBtnCont->Add(20,10,0);
    BtmBtnCont->Add(
        new wxButton(this, BUTTON_Copy, _T("Copy"), wxDefaultPosition, wxDefaultSize, 0),
        0, wxALL | wxALIGN_CENTER, 5
    );
    // Add Padding
    BtmBtnCont->Add(10,10,0);
    BtmCtlCont->Add(
        new wxTextCtrl(this, TEXT_Output, "Hey there!\n\nClick \"Scan Stuff\" to get open the snipping tool to scan your text!",
         wxDefaultPosition, wxSize(100,100), 
        wxTE_READONLY | wxTE_BESTWRAP | wxTE_MULTILINE | wxTE_AUTO_URL | wxTE_PROCESS_TAB),
        1, wxALL, 10
    );
    BtmCtlCont->Add(
        BtmBtnCont, 0, wxEXPAND, 10
    );
    TopSizer->Add(0,10,0);
    TopSizer->Add(
        TopCtlCont, 0, wxEXPAND, 10
    );
    TopSizer->Add(
        BtmCtlCont, 1, wxEXPAND, 10
    );
    SetSizer(TopSizer);
}


/* ----- MAIN WINDOW EVENT HANDLER LOGIC ----- */

void MainWin::OnAbout(wxCommandEvent& event){
    std::string msg;
    msg.append("SnipNScan, a lightweight QR code and text/url scanner.\nPowered by OpenCV and Tesseract.\n\n");
    msg.append("Developed by Nicholas @ github.com/nichyjt/ \n\n");
    msg.append("Licensed under GPLv3");
    wxMessageBox( msg, "About", wxOK | wxICON_INFORMATION );
}

void MainWin::OnSnip(wxCommandEvent& event){
    SnipWin* tmp = (SnipWin*) wxWindow::FindWindowById(WINDOW_SNIP);
    wxDisplay* display = new wxDisplay(wxDisplay::GetFromWindow(this));
    wxRect screen = display->GetClientArea();
    wxSize size = screen.GetSize();
    this->Hide();
    SnipWin* frame = new SnipWin(WINDOW_SNIP, "Drag Over The Area to Scan!", wxPoint(10,10), size);
    frame->Show();
}

void MainWin::OnCopy(wxCommandEvent& event){
    wxTextCtrl* txtbox = getOutputTextbox();
    txtbox->SelectAll();
    if(txtbox->CanCopy()){
        txtbox->Copy();
        SetStatusText("Text copied to clipboard!");
    }else{
        SetStatusText("Error copying. Did you scan anything yet?");
    }
}

void MainWin::OnOpenLink(wxTextUrlEvent& event){
    const wxMouseEvent& ev = event.GetMouseEvent();
    wxSetCursor(wxCursor(wxCURSOR_HAND));
    if(ev.Moving())
        return; //ignore
    if(ev.LeftUp()){
        long start = event.GetURLStart(), end = event.GetURLEnd();
        wxTextCtrl* tctl = (wxTextCtrl*) this->FindWindowById(TEXT_Output);
        wxString urlstr = tctl->GetValue().Mid(start,end-start);
        // wxLogMessage("Click detected %s", urlstr);
        wxLaunchDefaultBrowser(urlstr);
    }
}

// Event Handler Declaration (macro) For mainwindow
wxBEGIN_EVENT_TABLE(MainWin, wxFrame)
    EVT_MENU(wxID_ABOUT, MainWin::OnAbout)
    EVT_BUTTON(BUTTON_Snip, MainWin::OnSnip)
    EVT_BUTTON(BUTTON_Copy, MainWin::OnCopy)
    EVT_TEXT_URL(TEXT_Output, MainWin::OnOpenLink)
    EVT_ENTER_WINDOW(MainWin::OnEnterWin)
wxEND_EVENT_TABLE()

// Main Driver Function (macro)
wxIMPLEMENT_APP(MyApp);

// DRIVER LOGIC FOR MAIN WINDOW
bool MyApp::OnInit(){
    MainWin* frame = new MainWin(WINDOW_MAIN, "SnipNScan", wxPoint(100,100), wxSize(450,280), wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER & ~wxMAXIMIZE_BOX);
    // std::cout << frame->GetId() << std::endl;
    frame->Show(true);
    wxInitAllImageHandlers();
    return true;
}


/* ----- PASSING DATA BETWEEN STATES LOGIC ----- */

wxTextCtrl* getOutputTextbox(){
    // Get reference to output textbox. For DRYness.
    return (wxTextCtrl*) wxWindow::FindWindowById(TEXT_Output);
}

cv::Mat matFromWx(wxBitmap bmp){
    wxImage ibmp = bmp.ConvertToImage();
    unsigned char* rawdata = ibmp.GetData();
    ibmp.SaveFile("cropbm.bmp", wxBITMAP_TYPE_BMP);
    cv::Mat img(cv::Size(ibmp.GetWidth(), ibmp.GetHeight()), CV_8UC3, rawdata);
    cvtColor(img, img, cv::COLOR_RGB2BGR);
    return img;
}

void ValidateTextLength(std::string& str){
    // Handle UI logic for str of length 0 or other funny ctrl chars
    if(str.length()<=0 || str.find_first_not_of(" \t\n\v\f\r") == std::string::npos){
        std::cout << "Empty" << std::endl;
        MainWin* mwref = (MainWin*) wxWindow::FindWindowById(WINDOW_MAIN);
        wxMessageDialog* dia = new wxMessageDialog(mwref, 
        "Nothing found. Try scanning again in a larger area!", "Nothing Found");
        dia->ShowModal();
        return;
    }
}

void write_output_text(std::string& str){
    // Writes input str to the UI output textbox
    wxTextCtrl* txtbox = getOutputTextbox();
    txtbox->Clear();
    txtbox->SetInsertionPoint(0);
    // Handle non-ascii characters and multiple \n chars..
    // .. to ensure compatability with textctl
    // Note: Not factored out into another function as it will..
    // ..add O(n) in time and waste some space
    // char prev;
    // for(char c : str){
    //     if(isascii(c)){
    //         // Do not print consecutive newlines
    //         if(!(prev == '\n' && c == prev))
    //             (*txtbox) << c;
    //         prev = c;
    //     }
    // }
    wxString wxstr = wxString::FromUTF8(str.c_str());
    txtbox->WriteText(wxstr);
}

// DATA PROCESSING CONTROL FUNCTION
void MainWin::ProcessData(wxBitmap data){
    // Do cv::Mat conversion here to prevent data loss
    // cv::Mat tends to gain noise for some reason...
    // ...when the conversion process is done via an external fn
    wxImage ibmp = data.ConvertToImage();
    cv::Mat mat(cv::Size(ibmp.GetWidth(), ibmp.GetHeight()), CV_8UC3, ibmp.GetData());
    cvtColor(mat, mat, cv::COLOR_RGB2BGR);
    // Start processing logic
    wxChoice* choice = (wxChoice*) wxWindow::FindWindowById(CHOICE_Mode);
    std::string output;
    switch(choice->GetSelection()){
        case DETECT_AUTO:
        output = ProcessAuto(mat);
        break;
        case DETECT_QR:
        output = ProcessQR(mat);
        break;
        case DETECT_URL:
        output = ProcessImg(mat);
        break;
        default:
            std::cout << "Error in Process Data switch" << std::endl;
        break;
    }
    ValidateTextLength(output);
    write_output_text(output);
}