#include <iostream>
#include <cv.h>
#include <highgui.h>

int main(int argc, char *argv[])
{
    CvCapture *capture = cvCaptureFromCAM(CV_CAP_ANY);
    if (!capture) {
        std::cerr << "failed to create capture device\n";
        return -1;
    }

    std::cerr << "creating window\n";

    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 320);
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 240);

    cvNamedWindow("mywindow", CV_WINDOW_AUTOSIZE);

    while (true) {
        IplImage *frame = cvQueryFrame(capture);
        if (!frame) {
            std::cerr << "failed to query frame from camera\n";
            break;
        }
        cvShowImage("mywindow", frame);
    }

    cvReleaseCapture(&capture);
    cvDestroyWindow("example");
    return 0;
}

