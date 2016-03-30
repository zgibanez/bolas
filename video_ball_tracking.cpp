////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <stdio.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#define DEBUG 0 //0 - VIDEO ININTERRUMPIDO, 1 - VIDEO FRAME A FRAME
#define MEANSHIFT 1  //0 - CAMSHIFT 1 - MEANSHIFT

using namespace cv;
using namespace std;

/*Function for mouse Callback*/
bool click=false;
Point P1, P2;

void CallBackFunc(int event, int x, int y, int flags, void* reset);

/*Function to draw ball*/
void ballfinder(Mat& img, Mat& display, int x_window, int y_window, int H_low, int H_high)
{
	Mat img_threshold, img_close;
	vector<Vec3f> circles; /*This vector stores x,y and radius of the circles found with Hough Transform*/
	
    //Apply threshold by fcn input
	inRange(img, Scalar(H_low, 0, 0), Scalar(H_high, 255, 255), img_threshold);

	//Close threshold image
	erode(img_threshold, img_close, getStructuringElement(MORPH_ELLIPSE, Size(8, 8))); /*Close?*/
	dilate(img_close, img_close, getStructuringElement(MORPH_ELLIPSE, Size(6, 6)));
	imshow("tracked ROI", img_close);

	//Find circles
	HoughCircles(img_threshold, circles, CV_HOUGH_GRADIENT, 1, img_threshold.rows / 8, 200, 30, 0, 0);

	//Draw and count circles in original
	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0])+x_window, cvRound(circles[i][1]+y_window));
		int radius = cvRound(circles[i][2]); /*Take the radius from circles detected*/
											 // circle center
		circle(display, center, 3, Scalar(255, 255, 255), -1, 8, 0);
		// circle outline
		circle(display, center, radius, Scalar(0, 0, 255), 3, 8, 0);

	}

}


int main(int argc, char** argv)
{
	Mat frame, frameHSV;
	Mat frame_copy, frame_crop; /*Initial window and cropped ROI*/
	Mat roi, hsv_roi, mask; /*region of interest*/
	Mat roi_hist, img_backproj;
	int reset, *preset=&reset;
	

	VideoCapture capture(argv[1]); //CHANGE
	if (!capture.isOpened())
		throw "Error when reading video";

	/* /////////////////OUTPUT////////////
	//Preparing output:
	VideoWriter output;
	string filename = "C:/images/output.mp4";
	int fcc = CV_FOURCC('D', 'I', 'V', 'X');
	double fps = 20;
	Size frameSize(capture.get(CV_CAP_PROP_FRAME_WIDTH), capture.get(CV_CAP_PROP_FRAME_HEIGHT));

	output = VideoWriter(filename, fcc, fps, frameSize);
	/////////////////OUTPUT////////////*/


	///////// MOUSE //////////
	capture >> frame;
	frame.copyTo(frame_copy);
	imshow("Initialize", frame_copy);
	setMouseCallback("Initialize", CallBackFunc, preset);

	cout << "Initialization window created" << endl;


	int key = 0;
	while (true) {
		putText(frame_copy, "Select initial region and press ENTER", cvPoint(60, 30),
		FONT_HERSHEY_COMPLEX_SMALL, 2, cvScalar(0, 0, 0), 1, CV_AA);
		rectangle(frame_copy, P1, P2, Scalar(0, 255, 255), 1, 8, 0);
		imshow("Initialize", frame_copy);
		key = waitKey(30);

		if (key == 13)
		{
			cout << "ENTER key pressed by user" << endl;
			setMouseCallback("Initialize", NULL, NULL); /*No more mouse needed*/
			destroyWindow("Initialize");
			break;
		}
		else if (key == -1) frame.copyTo(frame_copy);
	}

	//Ajuste de direcciones de ventana por cuadrante
	Point p_ini, p_fin;
	if (P2.x < P1.x) p_ini.x = P2.x;
	else p_ini.x = P1.x;

	if (P2.y < P1.y) p_ini.y = P2.y;
	else p_ini.y = P1.y;

	p_fin.x = p_ini.x + abs(P1.x - P2.x);
	p_fin.y = p_ini.y + abs(P1.y - P2.y);
	///////// FIN MOUSE //////////



	//Preparación de la ROI: NOTA, ANTES TENIA SATURACION
	Rect track_window(p_ini.x, p_ini.y, abs(p_ini.x - p_fin.x), abs(p_ini.y - p_fin.y));
	RotatedRect camshift_track_window;
	Point2f pts[4];
	TermCriteria  term_crit(CV_TERMCRIT_EPS | CV_TERMCRIT_NUMBER, 100, 1);
	
	//For contours
	Mat threshold_output;
	double thresh; /*Threshold for drawing contour of ROI*/
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	
	//For histogram
	int hbins = 30;
	int ch[] = {0};
	int histSize[] = { hbins}; /*Numero de valores de histograma*/
	float hranges[] = { 0, 180 };	   /*Hue - 0-180*/
	const float* ranges[] = { hranges };

	roi = frame(track_window);
	cvtColor(roi, hsv_roi, COLOR_BGR2HSV);
	inRange(hsv_roi,Scalar(0, 60, 30), Scalar(60, 255, 255), mask);
	calcHist(&hsv_roi, 1, ch, mask, roi_hist, 1, histSize, ranges, true, false);
    normalize(roi_hist, roi_hist, 255, 0, NORM_MINMAX, -1, noArray()); //atencion - sin máscara


	for (; ; )
	{
		capture >> frame;
		if (frame.empty())
			break;
		blur(frame, frame, Size(3, 3));

		//transform video frame to HSV
		cvtColor(frame, frameHSV, COLOR_BGR2HSV);
		calcBackProject(&frameHSV, 1, ch, roi_hist, img_backproj, ranges, 1, true);


		//Set calcBackProject and calculate meanShift/CamShift
		if (MEANSHIFT) {
			meanShift(img_backproj, track_window, term_crit);
			rectangle(frame, track_window, Scalar(0, 255, 255), 3, 8, 0);
			//ballfinder(frame(track_window), frame, track_window.x, track_window.y, 0, 15);
		}
		else {
			camshift_track_window = CamShift(img_backproj, track_window, term_crit);
			camshift_track_window.points(pts);
			for (int i = 0; i < 4; i++)
				line(frame, pts[i], pts[(i + 1) % 4], Scalar(0, 255, 0),3,8,0);
			}

		imshow("detection", frame);

		////////////////OUTPUT////////////////
		/*//copy the transformed frame to output
		output.write(transformed);*/
		//////////////////////////////////////
		if (DEBUG) key = waitKey(-1);
		else key = waitKey(30);

		if (key == 27)
		{
			cout << "ESC key pressed" << endl;
			break;
		}

	}
	capture.release();
	//output.release();
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

}
