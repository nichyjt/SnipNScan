#ifndef ID_H
#define ID_H

// UNIQUE IDENTIFIER OVERALL CONTROL

enum DROPMENU_MODE{
    DETECT_AUTO = 0,
    DETECT_QR = 1,
    DETECT_URL = 2,
};

enum{
    BUTTON_Snip = 1,
    CHOICE_Mode = 2,
    TEXT_Mode = 3,
    BUTTON_Copy = 4,
    BUTTON_Goto = 5,
    TEXT_Output = 6,
    WINDOW_MAIN = 7 
};

enum SNIP_UI {
    BUTTON_CANCEL = 10,
    BITMAP_SCREENSHOT = 11,
    WINDOW_SNIP = 12
};

#endif //ID