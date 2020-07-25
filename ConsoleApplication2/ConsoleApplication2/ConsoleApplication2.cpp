#include <opencv2/objdetect.hpp>  
#include <opencv2/highgui.hpp> 
#include <opencv2/imgproc.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>
#include <iostream>
#include "Blob.h"
#include "kalmanfilter.h"
#include <ctype.h>
#include <conio.h>
#include <fstream>
#define clears system("CLS");
using namespace cv;
using namespace std;
class Tracker
{
    VideoCapture Video;
    Mat Frame1;
    Mat Frame2;
public:
    int tracker(string, int);
};
class  Peopletracker :public  Tracker
{
    int c_code;
public:
    Peopletracker(string);
};
Peopletracker::Peopletracker(string s)
{
    c_code = 0;
    tracker(s, c_code);
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
    tracker(s, c_code);
}
int Tracker::tracker(string s, int c_code)
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
inline void hello()
{
    const char a = '\xCD';
    const char b = '\xBA';
    const char c = '\xC9';
    const char d = '\xBB';
    const char e = '\xC8';
    const char f = '\xBC';
    string str = "Welcome To TRACKER-DETECTOR";
    string margin = " ";
    string line(str.length() + 2 * margin.length(), a);
    cout << endl << endl;
    cout << "\t\t\t\t\t\t" << c << line << d << endl;
    cout << "\t\t\t\t\t\t" << b << margin << str << margin << b << endl;
    cout << "\t\t\t\t\t\t" << e << line << f << endl;
    system("PAUSE");
    clears;
}
class Account
{
protected:
    string name;
    char pass[32];
    int id;
public:
    Account();
    bool input();
};
Account::Account()
{
    id = 0;
}
bool Account::input()
{
    cin.ignore();
    cout << "************************************************************************************************************************" << endl;
    cout << "\t\t\t\t\tWelcome To Tracker and detector" << endl << endl;
    cout << "************************************************************************************************************************"<< endl;
    cout << "Enter The Username :";
    getline(cin, name);
    cout << "Enter The Password :";
    char a;
    int i;
    for (i = 0; ; )
    {
        a = _getch();
        if ((a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z') || (a >= '0' && a <= '9'))
        {
            pass[i] = a;
            ++i;
            cout << "*";
        }
        if (a == '\b' && i >= 1)//BACKSPACE KEY IS PRESSED
        {
            cout << "\b \b";//ERASE CHARACTER
            --i;
        }
        if (a == '\r')//ENTER KEY IS PRESSED
        {
            pass[i] = '\0';
            break;
        }
    }
    cout << endl;
    string password = "";
    for (i = 0; pass[i] != '\0'; i++)
    {
        password = password + pass[i];
    }
    string un, pw, filename;
    filename = name + ".txt";
    ifstream read(filename.c_str());
    getline(read, un);
    getline(read, pw);
    if (un == name && pw == password)
    {
        return true;
    }
    else
    {
        return false;
    }
}
inline int welcome()
{
    int a;
    clears;
    cout << "\t\t\t\t\t\tSELECT USAGE\n";
    cout << "\n\n\n\n\t\t\t\t\t\tENTER 1 Detect My Face!!!";
    cout << "\n\t\t\t\t\t\tENTER 2 Detect People!!!";
    cout << "\n\t\t\t\t\t\tENTER 3 Detect Cars!!!";
    cout << "\n\t\t\t\t\t\tENTER 4 Detect my mouse!!!";
    cout << "\n\t\t\t\t\t\tENTER 5 EXIT\n";
    cout << "enter your choice";
    cin >> a;
    int j = 0;
    while (a != 5)
    {
        if (a == 1)
        {
            Facedetection myface;
            myface.detect();
            a = 1;
            clears;
        }
        if (a == 2)
        {
            Peopletracker track("vtest.avi");
            a = 2;
            clears;
        }
        if (a == 3)
        {
            Cartracker detect("CarsDrivingUnderBridge.mp4");
            a = 3;
            clears;
        }
        if (a == 4)
        {
            Mouse detection;
            detection.detectdraw();
            a = 4;
            clears;
        }
        if (a != 1 || a != 2 || a != 3 || a != 4 || a != 5)
        {
            throw "DO enter the correct choice";
        }
        cout << "enter your choice";
        cin >> a;
    }
    if (a == 5)
    {
        return 1;
    }
}
inline void route()
{
    int i = 0;
    while (i == 0)
    {
        try
        {
            i = welcome();
        }
        catch (const char* msg)
        {
            cerr << msg << endl;
            system("PAUSE");
        }
    }
    i = 0;
}
class Newaccount :private Account
{
    long long int mobileno;
    char cpassword[32];
public:
    int generate();
    Newaccount();
    void ninput();
};
void Newaccount::ninput()
{
    int i;
    cin.ignore();
    cout << "************************************************************************************************************************" << endl;
    cout << "\t\t\t\t\tWelcome To Tracker and detector" << endl << endl;
    cout << "************************************************************************************************************************" << endl;
    cout << "Enter The Username :";
    getline(cin, name);
    cout << "Enter The Password :";
    char a; 
    for (i = 0; ; )
    {
        a =_getch();
        if ((a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z') || (a >= '0' && a <= '9'))
        {
            pass[i] = a;
            ++i;
            cout << "*";
        }
        if (a == '\b' && i >= 1)//BACKSPACE KEY IS PRESSED
        {
            cout << "\b \b";//ERASE CHARACTER
            --i;
        }
        if (a == '\r')//ENTER KEY IS PRESSED
        {
            pass[i] = '\0';
            break;
        }
    }
    cout << endl;
    cout << "Retype Password :";
    for (i = 0; ; )
    {
        a =_getch();
        if ((a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z') || (a >= '0' && a <= '9'))
        {
            cpassword[i] = a;
            ++i;
            cout << "*";
        }
        if (a == '\b' && i >= 1)//BACKSPACE KEY IS PRESSED
        {
            cout << "\b \b";//ERASE CHARACTER
            --i;
        }
        if (a == '\r')//ENTER KEY IS PRESSED
        {
            cpassword[i] = '\0';
            break;
        }
    }
    string password = "";
    for (i = 0; cpassword[i] != '\0'; i++)
    {
        password = password + cpassword[i];
    }
    cout << "\n\n< Press Any Key To Continue >" << endl;
    _getch();
    cout << "Enter Your Mobile Number :";
    cin >> mobileno;
    string filename;
    filename = name + ".txt";
    ofstream file;
    file.open(filename.c_str());
    file << name << endl << password << endl << mobileno;
    file.close();
    clears;
    cout << "\t\t\t\tSIGNUP SUCCESSFULL" << endl;
    system("PAUSE");
    clears;
    route();
}
Newaccount::Newaccount() :Account::Account()
{
    mobileno = 0;
}
class Routing :private Account
{
public:
    void Input();
};
void Routing::Input()
{
    int n;
    Newaccount a;
    Account b;
    do
    {
        clears;
        cout << "\t\t\t\t\t\tACCOUNT SELECTION\n";
        cout << "\n\n\n\n\t\t\t\t\t\t1]NEW USER";
        cout << "\n\t\t\t\t\t\t2]LOGIN";
        cout << "\n\t\t\t\t\t\t0]EXIT\n";
        cin >> n;
        switch (n)
        {
        case 1:
            clears;
            a.ninput();
            break;
        case 2:
            clears;
            bool status = b.input();
            if (!status)
            {
                cout << "\n<INVLAID USERNAME/PASSWORD!>\n";
                system("PAUSE");
                clears;
            }
            else
            {
                cout << "\n<LOGIN SUCCESSFULL!>\n";
                system("PAUSE");
                clears;
                route();
            }
            break;
        }
    } while (n != 0);
}
int main()
{
    hello();
    Routing r;
    r.Input();
    return 0;
}
