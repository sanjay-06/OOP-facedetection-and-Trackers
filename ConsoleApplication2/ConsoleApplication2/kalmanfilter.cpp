#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>

#include<iostream>
using namespace cv;
using namespace std;
void mouseMoveCallback(int event, int x, int y, int flags, void* userData);
void drawCross(Mat& img, Point center, Scalar color);
const Scalar SCALAR_WHITE = Scalar(255.0, 255.0, 255.0);
const Scalar SCALAR_BLUE = Scalar(255.0, 0.0, 0.0);
const Scalar SCALAR_GREEN = Scalar(0.0, 200.0, 0.0);
Point ptActualMousePosition(0, 0);
int draw()
{
    KalmanFilter kalmanFilter(4, 2, 0);
    float fltTransitionMatrixValues[4][4] = { { 1, 0, 1, 0 },
                                              { 0, 1, 0, 1 },
                                              { 0, 0, 1, 0 },
                                              { 0, 0, 0, 1 } };
    kalmanFilter.transitionMatrix = Mat(4, 4, CV_32F, fltTransitionMatrixValues);

    float fltMeasurementMatrixValues[2][4] = { { 1, 0, 0, 0 },
                                               { 0, 1, 0, 0 } };
    kalmanFilter.measurementMatrix = Mat(2, 4, CV_32F, fltMeasurementMatrixValues);
    setIdentity(kalmanFilter.processNoiseCov, Scalar::all(0.0001));
    setIdentity(kalmanFilter.measurementNoiseCov, Scalar::all(10));
    setIdentity(kalmanFilter.errorCovPost, Scalar::all(0.1));
    Mat imgBlank(700, 900, CV_8UC3, Scalar::all(0));
    vector< Point> predictedMousePositions;
    vector< Point> actualMousePositions;
    vector< Point> correctedMousePositions;
    namedWindow("imgBlank");
    setMouseCallback("imgBlank", mouseMoveCallback);
    while (true)
    {
        Mat matPredicted = kalmanFilter.predict();
        Point ptPredicted((int)matPredicted.at<float>(0), (int)matPredicted.at<float>(1));
        Mat matActualMousePosition(2, 1, CV_32F, Scalar::all(0));
        matActualMousePosition.at<float>(0, 0) = (float)ptActualMousePosition.x;
        matActualMousePosition.at<float>(1, 0) = (float)ptActualMousePosition.y;
        Mat matCorrected = kalmanFilter.correct(matActualMousePosition);
        Point ptCorrected((int)matCorrected.at<float>(0), (int)matCorrected.at<float>(1));
        predictedMousePositions.push_back(ptPredicted);
        actualMousePositions.push_back(ptActualMousePosition);
        correctedMousePositions.push_back(ptCorrected);
        drawCross(imgBlank, ptPredicted, SCALAR_BLUE);
        drawCross(imgBlank, ptActualMousePosition, SCALAR_WHITE);
        drawCross(imgBlank, ptCorrected, SCALAR_GREEN);
        for (int i = 0; i < predictedMousePositions.size() - 1; i++)
        {
            line(imgBlank, predictedMousePositions[i], predictedMousePositions[i + 1], SCALAR_BLUE, 1, 8, 0);
        }
        for (int i = 0; i < actualMousePositions.size() - 1; i++)
        {
            line(imgBlank, actualMousePositions[i], actualMousePositions[i + 1], SCALAR_WHITE, 1, 8, 0);
        }
        for (int i = 0; i < correctedMousePositions.size() - 1; i++)
        {
            line(imgBlank, correctedMousePositions[i], correctedMousePositions[i + 1], SCALAR_GREEN, 1, 8, 0);
        }
        imshow("imgBlank", imgBlank);
        char c = (char)waitKey(10);
        if (c == 'q' || c == 'Q')
            break;
        imgBlank = Scalar::all(0);
    }
    return 0;
}
void mouseMoveCallback(int event, int x, int y, int flags, void* userData)
{
    if (event == EVENT_MOUSEMOVE)
    {
        cout << "mouse move at " << x << ", " << y << "\n";
        ptActualMousePosition.x = x;
        ptActualMousePosition.y = y;
    }
}
void drawCross(Mat& img, Point center, Scalar color)
{
    line(img, Point(center.x - 5, center.y - 5), Point(center.x + 5, center.y + 5), color, 2);
    line(img, Point(center.x + 5, center.y - 5), Point(center.x - 5, center.y + 5), color, 2);
}