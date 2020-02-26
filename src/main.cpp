#include <opencv2/opencv.hpp>
#include <RemoteController.h>
#include <CascadeDetectorAdapter.h>

int main() {
    std::string path = "/home/cenmmy/CLionProjects/h264/src/assets/haarcascade_frontalface_alt2.xml";
    cv::Ptr<cv::CascadeClassifier> mainClassifier = cv::makePtr<cv::CascadeClassifier>(path);
    cv::Ptr<CascadeDetectorAdapter> mainDetector = cv::makePtr<CascadeDetectorAdapter>(mainClassifier);
    cv::Ptr<cv::CascadeClassifier> trackClassifier = cv::makePtr<cv::CascadeClassifier>(path);
    cv::Ptr<CascadeDetectorAdapter> trackingDetector = cv::makePtr<CascadeDetectorAdapter>(trackClassifier);
    cv::DetectionBasedTracker::Parameters parameters;
    cv::DetectionBasedTracker* tracker = new cv::DetectionBasedTracker(mainDetector, trackingDetector, parameters);
    tracker->run();
    RemoteController rc{};
    rc.active();
    rc.streamon();
    while(true) {
        cv::Mat image = rc.getFrame();
        cv::Mat gray;
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(gray, gray);
        std::vector<cv::Rect> faces;
        tracker->process(gray);
        tracker->getObjects(faces);
        cv::circle(image, cv::Point(image.cols / 2, image.rows / 2), 5, cv::Scalar(0, 0, 255));
        int x_distance = 0;
        int y_distance = 0;
        for (cv::Rect face : faces) {
            x_distance = image.cols / 2 - face.x;
            x_distance = image.rows / 2 - face.y;
            cv::rectangle(image, face, cv::Scalar(255, 0, 0));
            cv::line(image, cv::Point(face.x + face.width / 2, face.y + face.height / 2),
                    cv::Point(image.cols / 2, image.rows / 2), cv::Scalar(0, 0, 255));
        }
        cv::imshow("tello", image);
        char ch = cv::waitKey(10);
        if (ch == 'a') {
            std::cout << "takeoff" << std::endl;
            rc.takeoff();
        } else if (ch == 's') {
            std::cout << "land" << std::endl;
            rc.landing();
        } else if (ch == 'd') {
            std::cout << "up" << std::endl;
            rc.up();
        } else if (ch == 'f') {
            std::cout << "down" << std::endl;
            rc.down();
        } else if (ch == 'h') {
            std::cout << "forward" << std::endl;
            rc.forward();
        } else if (ch == 'j') {
            std::cout << "back" << std::endl;
            rc.back();
        } else if (ch == 'k') {
            std::cout << "left" << std::endl;
            rc.left();
        } else if (ch == 'l') {
            std::cout << "right" << std::endl;
            rc.right();
        }

        else if (ch == 'q') {
            rc.disconnect();
            break;
        }
        image.release();
        gray.release();
    }
    return 0;
}