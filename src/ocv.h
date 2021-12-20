#ifndef OCV_H
#define OCV_H

#include <opencv2/opencv.hpp> //debug
#include <opencv2/core.hpp>
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "tesseract/baseapi.h"
#include "leptonica/allheaders.h" //tesseract dependency
#include "strings.h"
#include "vector"

// Prototype Functions
std::string ProcessAuto(cv::Mat);
std::string ProcessQR(cv::Mat);
std::string ProcessImg(cv::Mat);

#endif //OCV_H