////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <stdio.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#define DEBUG 1


using namespace cv;
using namespace std;

/*Function for mouse Callback*/
bool click;
Point P1, P2;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	switch (event) {
	case EVENT_LBUTTONDOWN:
		click = true;
		cout << "Left button pressed" << endl;
		P1.x = x;
		P1.y = y;
		P2.x = x;
		P2.y = y;
		break;

	case EVENT_LBUTTONUP:
		cout << "Left button released" << endl;
		if (click) {
			P2.x = x;
			P2.y = y;
		}
		break;

	default:
		break;
	}

}





int main(int argc, char** argv)
{
	Mat frame, frameHSV, frame_threshold, transformed;
	Mat frame_copy; /*Initial window*/
	Mat roi, hsv_roi, mask; /*region of interest*/
	Mat roi_hist, img_hist;


	VideoCapture capture("C:/images/2dball.mp4"); //CHANGE


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

	namedWindow("w", 1);

	//select region with mouse
	capture >> frame;
	frame.copyTo(frame_copy);
	imshow("Initialize", frame_copy);
	setMouseCallback("Initialize", CallBackFunc, NULL);

	cout << "Initialization window created" << endl;

	putText(frame_copy, "Select initial region and press ENTER", cvPoint(60, 30),
		FONT_HERSHEY_COMPLEX_SMALL, 2, cvScalar(0, 0, 0), 1, CV_AA);

	while (true) {
		rectangle(frame_copy, P1, P2, Scalar(0, 255, 255), 1, 8, 0);
		imshow("Initialize", frame_copy);

		if (waitKey(30) == 13)
		{
			cout << "ENTER key pressed by user" << endl;
			setMouseCallback("Initialize", NULL, NULL); /*No more mouse needed*/
			destroyWindow("Initialize");
			break;
		}
	}

	//Ajuste de direcciones de ventana por cuadrante
	Point p_ini, p_fin;
	if (P2.x < P1.x) p_ini.x = P2.x;
	else p_ini.x = P1.x;

	if (P2.y < P1.y) p_ini.y = P2.y;
	else p_ini.y = P1.y;

	p_fin.x = p_ini.x + abs(P1.x - P2.x);
	p_fin.y = p_ini.y + abs(P1.y - P2.y);

	//Preparación de la ROI
	Rect track_window(p_ini.x, p_ini.y, abs(p_ini.x - p_fin.x), abs(p_ini.y - p_fin.y));
	roi = frame(track_window);
	
	int hbins = 30, sbins = 30;
	int ch[] = { 0,0 };
	int histSize[] = { hbins, sbins }; /*Numero de valores de histograma*/
	float hranges[] = { 0, 179 };	   /*Hue - 0-180*/
	float sranges[] = { 0, 255 };	   /*Saturation  0-256*/
	const float* ranges[] = { hranges, sranges };

	cvtColor(roi, hsv_roi, COLOR_BGR2HSV);
	inRange(hsv_roi,Scalar(100, 0, 0), Scalar(255, 20, 20), mask);
	calcHist(&hsv_roi, 1, ch, mask, roi_hist, 2, histSize, ranges, true, false);
    normalize(roi_hist, roi_hist, (0,0), 255, NORM_MINMAX, -1, Mat()); //atencion - sin máscara

	if (DEBUG) {
		cout << roi_hist.rows << "  Rows" << endl;
		cout << roi_hist.cols << "  Columns" << endl;
	}

	for (; ; )
	{
		capture >> frame;
		if (frame.empty())
			break;
		blur(frame, frame, Size(3, 3));

		//transform video frame to HSV
		cvtColor(frame, frameHSV, COLOR_BGR2HSV);

		//Apply criteria, set calcBackProject and calculate meanShift
		TermCriteria  term_crit = TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_NUMBER, 10, 1);
		calcBackProject(&frameHSV, 1, ch, roi_hist, img_hist, ranges, 1, true);
		meanShift(img_hist, track_window, term_crit);



		rectangle(frameHSV, , , Scalar(0, 255, 255), 1, 8, 0);

		imshow("detection", frameHSV);

		////////////////OUTPUT////////////////
		/*//copy the transformed frame to output
		output.write(transformed);*/
		//////////////////////////////////////

		if (waitKey(30) == 27)
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}
	capture.release();
	//output.release();
	return 0;
}
