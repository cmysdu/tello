//
// Created by cenmmy on 20-2-25.
//
#include <CascadeDetectorAdapter.h>

CascadeDetectorAdapter::CascadeDetectorAdapter(cv::Ptr<cv::CascadeClassifier> detector):
    IDetector(), Detector(detector) {

}

void CascadeDetectorAdapter::detect(const cv::Mat &Image, std::vector<cv::Rect> &objects) {
    Detector->detectMultiScale(Image, objects, scaleFactor, minNeighbours, 0, minObjSize, maxObjSize);
}