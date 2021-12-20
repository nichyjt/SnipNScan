// backend logic handler for openCV
#include "ocv.h"
#include <iostream>

// Utility function to check if the image's dominant (background) color is dark
bool isDominantDark(cv::Mat grayscaled){
    // Adapted from: https://stackoverflow.com/questions/15771512/compare-histograms-of-grayscale-images-in-opencv/15773817
    int bins = 256;
    int histSize[] = {bins};
    // Set ranges for histogram bins
    float lranges[] = {0, 256};
    const float* ranges[] = {lranges};
    // create matrix for histogram
    cv::Mat hist;
    int channels[] = {0};

    // create matrix for histogram visualization
    int const hist_height = 256;
    cv::Mat3b hist_image = cv::Mat3b::zeros(hist_height, bins);

    cv::calcHist(&grayscaled, 1, channels, cv::Mat(), hist, 1, histSize, ranges, true, false);

    double max_val=0;
    minMaxLoc(hist, 0, &max_val);

    // Visualize each bin (DEBUG)
    /*
    for(int b = 0; b < bins; b++) {
        float const binVal = hist.at<float>(b); // bin val 0->max_val
        int   const height = cvRound(binVal*hist_height/max_val);
        cv::line
            ( hist_image
            , cv::Point(b, hist_height-height), cv::Point(b, hist_height)
            , cv::Scalar(0,0,255)
            );
    }
    cv::imshow("test", hist_image);
    cv::imwrite("hist.png", hist_image);
    */

    // Trivial O(n) find max value
    int intensity = 0;
    int dominantBin = 0;
    for(int i=0; i<bins; ++i){
        float binVal = hist.at<float>(i); // bin val 0->max_val
        int relFreq = cvRound(binVal*hist_height/max_val); // Encoded 0-256
        if(relFreq > intensity){
            intensity = relFreq;
            dominantBin = i;
        }
    }
    // Greedy solution: Take midpt of intensity values as the binary trigger for dark/light mode
    return (dominantBin < (256/2));
}

// Handle QR code processing logic
std::string ProcessQR(cv::Mat mat){
    // Pre-process QR code
    cv::Mat gray;
    cv::cvtColor(mat, gray, cv::COLOR_BGR2GRAY);
    // Wrap mat in zbar data format
    zbar::Image zimg(gray.cols, gray.rows, "Y800", (uchar*) gray.data, gray.cols*gray.rows);
    zbar::ImageScanner zscanner;
    zscanner.set_config(zbar::ZBAR_QRCODE, zbar::ZBAR_CFG_ENABLE, 1);
    // Scan
    zscanner.scan(zimg);
    // Iterate through and build string from decoded results
    std::string output;
    for(auto symbol = zimg.symbol_begin(); symbol != zimg.symbol_end(); ++symbol){
        // Safety check
        if(symbol->get_type() == zbar::ZBAR_QRCODE){
            output.append(symbol->get_data());
        }
    }
    return output;
}

// Handle Image Text processing logic
std::string ProcessImg(cv::Mat mat){
    // Largely adapted from: https://docs.opencv.org/3.4/df/d0d/tutorial_find_contours.html
    // as well as https://www.geeksforgeeks.org/text-detection-and-extraction-using-opencv-and-ocr/
    // In the image, get contours and process w tess OCR
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    // Find edges in the image
    cv::Mat grayed;
    cv::cvtColor(mat, grayed, cv::COLOR_BGR2GRAY); //1 channel
    // DEBUG WRITE IMAGES
    // cv::imwrite("Original.png", mat);
    // cv::imwrite("Grayscaled.png", grayed);

    // NOTE: binary and binary_inv with OTSU does not generalise well to dark & light modes respectively.
    // Solution: 1. Use adapThresh to detect contours (generalises better to a mix of dark-light modes) 
    // 2. THRESH_MEAN is less noisy, so choose that
    // 3. Fine Tune with a const:
    // 3-1. Positive works best with detecting dark text on light
    // 3-2. Negative works best with detecting light text on dark

    // Dynamically set constant to get better results
    int c = 26;
    if(isDominantDark(grayed)){
        c = -42;
    }

    cv::Mat scannable;
    cv::adaptiveThreshold(grayed, scannable, 255, 
        cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY,
        25, c
        );
        
    // Denoise as redundancy
    cv::fastNlMeansDenoising(scannable, scannable);

    // Get Contours
    cv::Mat rectKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(21,21));
    cv::Mat dilated = scannable.clone();
    cv::dilate(scannable, dilated, rectKernel); // Inflate words to make it easier to contour
    cv::findContours(dilated, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    // cv::imwrite("processed.png", scannable); // Debug

    // Dealloc useless/spent matrices to save space
    dilated.release();
    grayed.release();

    // Prepare tesseract to scan & output 
    tesseract::TessBaseAPI* tapi = new tesseract::TessBaseAPI();
    tapi->Init(NULL, "eng", tesseract::OEM_DEFAULT);
    tapi->SetPageSegMode(tesseract::PSM_AUTO);
    std::string out;
    // Loop through each contour and process via OCR
    for(int idx = contours.size()-1; idx >= 0; --idx){
        cv::Rect roi = cv::boundingRect(contours[idx]); // region of interest
        cv::Mat cropped = scannable(roi);
        int ht = cropped.size().height;
        int wd = cropped.size().width;
        // imshow(std::to_string(idx), cropped); // debug
        tapi->SetImage(cropped.data, wd, ht, cropped.channels(), cropped.step1());
        tapi->SetSourceResolution(300);
        if(!(tapi->Recognize(0))){
            out.append(std::string(tapi->GetUTF8Text()));
        }
        tapi->Clear(); // Clear buffer(?) for next cropped img
    }
    tapi->End();
    return out;
}

std::string ProcessAuto(cv::Mat mat){
    // Process text based on automatic detection
    std::string output;
    output = ProcessQR(mat);
    if(output.length() <= 0){
        output = ProcessImg(mat);
    }
    return output;
}
