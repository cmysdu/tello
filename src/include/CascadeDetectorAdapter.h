//
// Created by cenmmy on 20-2-25.
//

#ifndef TELLO_CASCADEDETECTORADAPTER_H
#define TELLO_CASCADEDETECTORADAPTER_H

#include <opencv2/opencv.hpp>

class CascadeDetectorAdapter: public cv::DetectionBasedTracker::IDetector {
public:
    CascadeDetectorAdapter(cv::Ptr<cv::CascadeClassifier> detector);
    void detect(const cv::Mat &Image, std::vector<cv::Rect>& objects);
    virtual ~CascadeDetectorAdapter() {

    }

private:
    CascadeDetectorAdapter();
    cv::Ptr<cv::CascadeClassifier> Detector;
};

#endif //TELLO_CASCADEDETECTORADAPTER_H
