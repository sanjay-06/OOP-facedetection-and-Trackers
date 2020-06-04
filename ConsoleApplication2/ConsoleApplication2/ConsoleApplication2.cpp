#include <opencv2/objdetect.hpp>  
#include <opencv2/highgui.hpp> 
#include <opencv2/imgproc.hpp>
#include <opencv2/core/types_c.h>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>
#include <iostream>
#include "Blob.h"
#include "kalmanfilter.h"
using namespace cv;
using namespace std;
class Tracker
{
    VideoCapture Video;
    Mat Frame1;
    Mat Frame2;
public:
    int tracker(string,int);
}; 
class  Peopletracker:public  Tracker
{
    int c_code;
public:
    Peopletracker(string);
};
Peopletracker::Peopletracker(string s)
{
    c_code = 0;
    tracker(s,c_code);
}
class  Cartracker :public  Tracker
{
    int c_code;
public:
    Cartracker(string);
};
Cartracker::Cartracker(string s)
{
    c_code = 1;
    tracker(s,c_code);
}
int Tracker::tracker(string s,int c_code)
{
    Scalar SCALAR = Scalar(0, 0, 255);
    if (c_code == 1)
    {
        SCALAR = Scalar(0, 255, 0);
    }
    Video.open(s);
    if (!Video.isOpened())
    {
        cout << "\nerror reading video file" << endl;
        return 0;
    }
    Video.read(Frame1);
    Video.read(Frame2);
    char Check = 0;
    while (Check != 27)
    {
        vector<Blob> blobs;
        Mat Frame1Copy = Frame1.clone(), Frame2Copy = Frame2.clone(), Diff, Thresh;
        cvtColor(Frame1, Frame1Copy, COLOR_BGR2GRAY);
        cvtColor(Frame2, Frame2Copy, COLOR_BGR2GRAY);
        GaussianBlur(Frame1Copy, Frame1Copy, Size(5, 5), 0);
        GaussianBlur(Frame2Copy, Frame2Copy, Size(5, 5), 0);
        absdiff(Frame1Copy, Frame2Copy, Diff);
        threshold(Diff, Thresh, 30, 255.0, THRESH_BINARY);
        Mat structuringElement5x5 = getStructuringElement(MORPH_RECT, Size(5, 5));
        for (int i = 0; i < 3; i++)
            dilate(Thresh, Thresh, structuringElement5x5);
        Mat ThreshCopy = Thresh.clone();
        vector<vector<Point>> contours;
        findContours(ThreshCopy, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        vector<vector<Point>> convexHulls(contours.size());
        for (int i = 0; i < contours.size(); i++)
        {
            convexHull(contours[i], convexHulls[i]);
        }
        for (auto& convexHull : convexHulls)
        {
            Blob blob(convexHull);
            if (blob.boundingRect.area() > 100 && blob.dblAspectRatio >= 0.2 && blob.dblAspectRatio <= 1.2 &&
                blob.boundingRect.width > 15 &&
                blob.boundingRect.height > 20 &&
                blob.dblDiagonalSize > 30.0) {
                blobs.push_back(blob);
            }
        }
        convexHulls.clear();
        for (auto& blob : blobs)
        {
            convexHulls.push_back(blob.contour);
        }
        Frame2Copy = Frame2.clone();
        for (auto& blob : blobs)
        {
            rectangle(Frame2Copy, blob.boundingRect, SCALAR, 2);
        }
        imshow("NOTE!!!         CLICK ESCAPE TO EXIT", Frame2Copy);
        Frame1 = Frame2.clone();
        if ((Video.get(CAP_PROP_POS_FRAMES) + 1) < Video.get(CAP_PROP_FRAME_COUNT))
        {
            Video.read(Frame2);
        }
        else
        {
            cout << "end of video\n";
            break;
        }
        Check = waitKey(1);
    }
    if (Check != 27)
    {
        waitKey(0);
    }
    destroyAllWindows();
    return 0;
}
class Facedetection
{
	vector<Rect> faces, faces2;
public:
	Facedetection();
	void draw(Mat&, CascadeClassifier&, double);
	int detect();
};
Facedetection::Facedetection()
{
	cout << "Face Detection Started...." << endl;
}
void Facedetection::draw(Mat& img, CascadeClassifier& cascade, double scale)
{
	Mat gray, smallImg;
	cvtColor(img, gray, COLOR_BGR2GRAY);
	double fx = 1 / scale;
	resize(gray, smallImg, Size(), fx, fx, INTER_LINEAR);
	equalizeHist(smallImg, smallImg);
	cascade.detectMultiScale(smallImg, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
	for (int i = 0; i < faces.size(); i++)
	{
		Mat smallImgROI;
		Point pt1(faces[i].x, faces[i].y);
		Point pt2((faces[i].x + faces[i].height), (faces[i].y + faces[i].width));
		rectangle(img, pt1, pt2, Scalar(255, 0, 0), 2, 8, 0);
		smallImgROI = smallImg(faces[i]);
	}
	imshow("Face Detection", img);
}

int Facedetection::detect()
{
	string cascadeName, nestedCascadeName;
	VideoCapture capture;
	Mat frame, image;
	CascadeClassifier cascade, nestedCascade, json_file;
	double scale = 1;
	nestedCascade.load("C:\\Users\\Sanjay\\Desktop\\opencv\\build\\etc\\haarcascades\\haarcascade_eye_tree_eyeglasses.xml");
	cascade.load("C:\\Users\\Sanjay\\Desktop\\opencv\\build\\etc\\haarcascades\\haarcascade_frontalface_default.xml");
	capture.open(0);
	if (capture.isOpened())
	{
		while (1)
		{
			capture >> frame;
			if (frame.empty())
				break;
			Mat frame1 = frame.clone();
			draw(frame1, cascade, scale);
			char c = (char)waitKey(10);
            if (c == 27 || c == 'q' || c == 'Q')
            {
                destroyAllWindows();
                break;
            }
		}
	}
	else
		cout << "Could not Open Camera";
	return 0;
}
int main()
{
	int a;
	cout << "enter your choice";
	cin >> a;
    while (a != 5)
    {
        if (a == 1)
        {
            Facedetection myface;
            myface.detect();
        }
        if (a == 2)
        {
            Peopletracker track("vtest.avi");
        }
        if (a == 3)
        {
            Cartracker detect("CarsDrivingUnderBridge.mp4");
        }
        if (a == 4)
        {
            Mouse detection;
            detection.detectdraw();
        }
        cout << "enter your choice";
        cin >> a;
    }
}