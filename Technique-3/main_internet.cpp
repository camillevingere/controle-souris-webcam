#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <X11/Xlib.h>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

using namespace std;
using namespace cv;

#define PI 3.141592

cv::Point lastPoint;
cv::Point mousePoint;
std::vector<cv::Point> centers;
int radius;

/** Global variables */
String face_cascade_name = "haarcascade_frontalface_default.xml";
String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;

cv::Vec3f getEyeball(cv::Mat &eye, std::vector<cv::Vec3f> &circles)
{
        std::vector<int> sums(circles.size(), 0);
        for (int y = 0; y < eye.rows; y++)
        {
                uchar *ptr = eye.ptr<uchar>(y);
                for (int x = 0; x < eye.cols; x++)
                {
                        int value = static_cast<int>(*ptr);
                        for (int i = 0; i < circles.size(); i++)
                        {
                                cv::Point center((int)std::round(circles[i][0]), (int)std::round(circles[i][1]));
                                radius = (int)std::round(circles[i][2]);
                                if (std::pow(x - center.x, 2) + std::pow(y - center.y, 2) < std::pow(radius, 2))
                                {
                                        sums[i] += value;
                                }
                        }
                        ++ptr;
                }
        }
        int smallestSum = 9999999;
        int smallestSumIndex = -1;
        for (int i = 0; i < circles.size(); i++)
        {
                if (sums[i] < smallestSum)
                {
                        smallestSum = sums[i];
                        smallestSumIndex = i;
                }
        }
        return circles[smallestSumIndex];
}

cv::Point stabilize(std::vector<cv::Point> &points, int windowSize)
{
        float sumX = 0;
        float sumY = 0;
        int count = 0;
        for (int i = std::max(0, (int)(points.size() - windowSize)); i < points.size(); i++)
        {
                sumX += points[i].x;
                sumY += points[i].y;
                ++count;
        }
        if (count > 0)
        {
                sumX /= count;
                sumY /= count;
        }
        return cv::Point(sumX, sumY);
}

cv::Rect getLeftmostEye(std::vector<cv::Rect> &eyes)
{
        int leftmost = 99999999;
        int leftmostIndex = -1;
        for (int i = 0; i < eyes.size(); i++)
        {
                if (eyes[i].tl().x < leftmost)
                {
                        leftmost = eyes[i].tl().x;
                        leftmostIndex = i;
                }
        }
        return eyes[leftmostIndex];
}

void detectEyes(cv::Mat &frame, cv::CascadeClassifier &faceCascade, cv::CascadeClassifier &eyeCascade)
{
        cv::Mat grayscale;
        cv::cvtColor(frame, grayscale, cv::COLOR_BGR2GRAY); // convert image to grayscale
        cv::equalizeHist(grayscale, grayscale);             // enhance image contrast
        std::vector<cv::Rect> faces;
        faceCascade.detectMultiScale(grayscale, faces, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(150, 150));
        if (faces.size() == 0)
                return;                 // none face was detected
        cv::Mat face = frame(faces[0]); // crop the face
        std::vector<cv::Rect> eyes;
        eyeCascade.detectMultiScale(face, eyes, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(150, 150)); // same thing as above
        rectangle(frame, faces[0].tl(), faces[0].br(), cv::Scalar(255, 0, 0), 2);
        if (eyes.size() != 2)
                return; // both eyes were not detected
        for (cv::Rect &eye : eyes)
        {
                rectangle(frame, faces[0].tl() + eye.tl(), faces[0].tl() + eye.br(), cv::Scalar(0, 255, 0), 2);
        }
        cv::Rect eyeRect = getLeftmostEye(eyes);
        cv::Mat eye = face(eyeRect); // crop the leftmost eye
        cv::equalizeHist(eye, eye);
        std::vector<cv::Vec3f> circles;
        cv::HoughCircles(eye, circles, cv::HOUGH_GRADIENT, 1, eye.cols / 8, 250, 15, eye.rows / 8, eye.rows / 3);
        if (circles.size() > 0)
        {
                cv::Vec3f eyeball = getEyeball(eye, circles);
                cv::Point center(eyeball[0], eyeball[1]);
                centers.push_back(center);
                center = stabilize(centers, 5); // we are using the last 5
                cv::circle(frame, faces[0].tl() + eyeRect.tl() + center, radius, cv::Scalar(0, 0, 255), 2);
                cv::circle(eye, center, radius, cv::Scalar(255, 255, 255), 2);
                if (centers.size() > 1)
                {
                        cv::Point diff;
                        diff.x = (center.x - lastPoint.x) * 20;
                        diff.y = (center.x - lastPoint.y) * -30; // diff in y is higher because it's "harder" to move the eyeball up/down instead of left/right
                }
                lastPoint = center;
        }
        cv::imshow("Eye", eye);
}

void changeMouse(cv::Mat &frame, cv::Point &location)
{
        if (location.x > frame.cols)
                location.x = frame.cols;
        if (location.x < 0)
                location.x = 0;
        if (location.y > frame.rows)
                location.y = frame.rows;
        if (location.y < 0)
                location.y = 0;
        system(("xdotool mousemove " + std::to_string(location.x) + " " + std::to_string(location.y)).c_str());
}

int main()
{
        cv::VideoCapture cap(0); // the fist webcam connected to your PC
        if (!cap.isOpened())
        {
                std::cerr << "Webcam not detected." << std::endl;
                return -1;
        }
        cv::Mat frame;
        while (1)
        {
                cap >> frame;                // outputs the webcam image to a Mat
                cv::imshow("Webcam", frame); // displays the Mat
                if (cv::waitKey(30) >= 0)
                        break; // takes 30 frames per second. if the user presses any button, it stops from showing the webcam
        }
        //-- 1. Load the cascades
        if (!face_cascade.load(face_cascade_name))
        {
                cout << "--(!)Error loading face cascade\n";
                return -1;
        };
        if (!eyes_cascade.load(eyes_cascade_name))
        {
                cout << "--(!)Error loading eyes cascade\n";
                return -1;
        };
        while (1)
        {
                detectEyes(frame, face_cascade, eyes_cascade);
                changeMouse(frame, mousePoint);
                cv::imshow("Webcam", frame);
                if (cv::waitKey(30) >= 0)
                        break;
        }
        return 0;
}