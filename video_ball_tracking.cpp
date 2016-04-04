////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <stdio.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#define DEBUG 1			//0 - VIDEO ININTERRUMPIDO, 1 - VIDEO FRAME A FRAME
#define MEANSHIFT 0		//0 - CAMSHIFT, 1 - MEANSHIFT
#define PI 3.1415

using namespace cv;
using namespace std;

/*Additional functions*/
void CallBackFunc(int event, int x, int y, int flags, void* reset);
void contours_display(Mat img, Mat display, int x_roi, int y_roi,int angle=0);

/*Variables for mouse Callback*/
bool click = false;
Point P1, P2, p_ini, p_fin;


int main(int argc, char** argv)
{
	Mat frame, frameHSV;
	Mat frame_copy, frame_crop; /*Initial window and cropped ROI*/
	int key = 0;

	VideoCapture capture(argv[1]);
	if (!capture.isOpened())
		throw "Error when reading video";

	capture >> frame;
	frame.copyTo(frame_copy);
	blur(frame_copy, frame_copy, Size(5, 5));
	cv::imshow("Video", frame_copy);
	setMouseCallback("Video", CallBackFunc);
	cout << "Initialization window created" << endl;

	//Capture initial ROI
	while (true) {
		putText(frame_copy, "Select initial region and press ENTER", cvPoint(60, 30),
		FONT_HERSHEY_COMPLEX_SMALL, 1, cvScalar(255, 255, 255), 1, CV_AA);
		rectangle(frame_copy, P1, P2, Scalar(0, 255, 255), 1, 8, 0);
		cv::imshow("Video", frame_copy);
		
		key = waitKey(30);
		if (key == 13)
		{
			cout << "ENTER key pressed by user" << endl;
			setMouseCallback("Initialize", NULL, NULL); /*No more mouse needed*/
			break;
		}
		else if (key == -1) {
			frame.copyTo(frame_copy);
		}
	}



	//Preparación de la ROI
	Mat roi, hsv_roi, mask; 
	Mat roi_hist;
	Rect track_window(p_ini, p_fin);
	TermCriteria  term_crit(CV_TERMCRIT_EPS | CV_TERMCRIT_NUMBER, 100, 1);
	int hbins = 30;
	int ch[] = {0};
	int histSize[] = { hbins};		
	float hranges[] = { 0, 180 };	   
	const float* ranges[] = { hranges };

	roi = frame(track_window);
	cvtColor(roi, hsv_roi, COLOR_BGR2HSV);
	inRange(hsv_roi,Scalar(0,55, 4), Scalar(20, 255, 143), mask);
	calcHist(&hsv_roi, 1, ch, mask, roi_hist, 1, histSize, ranges, true, false);
    normalize(roi_hist, roi_hist, 255, 0, NORM_MINMAX, -1, noArray());

	//Persecución de la ROI
	while(true) {

		capture >> frame;
		if (frame.empty())
			break;

		blur(frame, frame, Size(5, 5));
		cvtColor(frame, frameHSV, COLOR_BGR2HSV);
		inRange(frameHSV, Scalar(0, 55, 4), Scalar(20, 255, 143), frameHSV);

	//Calculate meanShift OR CamShift
		if (MEANSHIFT) {
			meanShift(frameHSV, track_window, term_crit);
			rectangle(frame, track_window, Scalar(0, 255, 255), 3, 8, 0);
			contours_display(frameHSV(track_window), frame, track_window.x, track_window.y);
		}
		else {
			RotatedRect camshift_track_window;
			Rect boundrect;
			Point2f pts[4];

			camshift_track_window = CamShift(frameHSV, track_window, term_crit);

			if (camshift_track_window.size.height == 0 && camshift_track_window.size.width == 0) {
				boundrect = Rect(Point(0, 0), Point(frame.cols, frame.rows));
				if (DEBUG) cout << "Tracked window dimensions were 0x0. Track window size readjusted to whole frame." << endl;
			}
			else {
				boundrect = camshift_track_window.boundingRect();
			}

			camshift_track_window.points(pts);
			for (int i = 0; i < 4; i++){
				line(frame, pts[i], pts[(i + 1) % 4], Scalar(0, 255, 0), 3, 8, 0);
			}
			contours_display(frameHSV(boundrect), frame, boundrect.x, boundrect.y, camshift_track_window.angle);
		}
	

		cv::imshow("Video", frame);

		if (DEBUG) key = waitKey(-1);
		else key = waitKey(30);
		if (key == 27)
		{
			cout << "ESC key pressed" << endl;
			break;
		}

	}
	capture.release();
	return 0;
}



void CallBackFunc(int event, int x, int y, int flags, void* reset)
{
	switch (event) {
	case EVENT_LBUTTONDOWN:

		cout << "Left button pressed" << endl;
		if (!click) {
			click = true;
			P1.x = x;
			P1.y = y;
			P2.x = x;
			P2.y = y;
		}

		break;

	case EVENT_MOUSEMOVE:
		if (click) {
			P2.x = x;
			P2.y = y;
		}
		break;

	case EVENT_LBUTTONUP:
		cout << "Left button released" << endl;
		if (click) {
			P2.x = x;
			P2.y = y;
			click = false;
		}
		break;

	default:
		break;
	}

	if (P2.x < P1.x) p_ini.x = P2.x;
	else p_ini.x = P1.x;

	if (P2.y < P1.y) p_ini.y = P2.y;
	else p_ini.y = P1.y;

	p_fin.x = p_ini.x + abs(P1.x - P2.x);
	p_fin.y = p_ini.y + abs(P1.y - P2.y);

}

void contours_display(Mat img, Mat display, int x_roi, int y_roi, int angle)
{
	Point offset(x_roi+cos(angle*PI/180), y_roi+sin(angle*PI/180));
	vector<vector<Point> > contours;
	RotatedRect box;

	findContours(img, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point());
	drawContours(display, contours, -1, Scalar(255, 200, 200),1,8,noArray(),2147483647,offset);
	
}
