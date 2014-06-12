#include <iostream>
#include <stdint.h>
#include <cmath>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/features2d/features2d.hpp"


using namespace cv;
using namespace std;

void findSquare(Mat *frame, vector<Point> &contour, int *valid)
{
	Point 	corners[4];
	Point 	center;
	float 	lateralEdge;
	float 	diagonal;

	Point up(640,0);
	Point down(0,480);
	Point left(640,0);
	Point right(0,480);

	for( int i = 0; i < 4; i++) 
	{
		corners[0] = Point(0,0);
	}
	center = Point(0,0);
	diagonal = 0.0f;
	lateralEdge = 0.0f;
	*valid = 0;

	for(size_t k = 0; k < contour.size(); k++)
	{
		if(left.x  > contour[k].x) 	left 	= contour[k];
		if(right.x < contour[k].x) 	right 	= contour[k];
		if(up.y    < contour[k].y) 	up 	= contour[k];
		if(down.y  > contour[k].y) 	down 	= contour[k];
	}

	center.x = (up.x + left.x + down.x + right.x) / 4;
	center.y = (up.y + left.y + down.y + right.y) / 4;

	float distance_rd = norm(right-down);
	float distance_ur = norm(up-right);
	float distance_lu = norm(left-up);
	float distance_dl = norm(down-left);

	diagonal = ( norm(up-down) + norm(left-right)) /2;
	lateralEdge = (distance_lu + distance_ur + distance_rd + distance_dl) / 4;

	float distance_max = lateralEdge * 1.2f;
	float distance_min = lateralEdge * 0.8f;

	if( (distance_lu > distance_min) && (distance_lu < distance_max) &&
		(distance_dl > distance_min) && (distance_dl < distance_max) &&
		(distance_ur > distance_min) && (distance_ur < distance_max) &&
		(distance_rd > distance_min) && (distance_rd < distance_max) &&
		(lateralEdge > 8.0f)) 
	{

		corners[0] = left;
		corners[1] = right;
		corners[2] = up;
		corners[3] = down;

		*valid = 1;
	}
	else
	{
		*valid = 0;
	}

	/*if(valid)
	{
		for(int i = 0; i < 4; i++)
		{
			circle(*frame, corners[i], 7, Scalar(0,0,255), 3);
		}
		// diagonal
		line(*frame, corners[0], corners[1], Scalar(255, 255, 255), 1);
		line(*frame, corners[2], corners[3], Scalar(255, 255, 255), 1);

		// lateral edge
		line(*frame, corners[1], corners[2], Scalar(0, 255, 0), 1);
		line(*frame, corners[3], corners[0], Scalar(0, 255, 0), 1);
		line(*frame, corners[1], corners[3], Scalar(0, 255, 0), 1);
		line(*frame, corners[0], corners[2], Scalar(0, 255, 0), 1);

		// Center
		circle(*frame, center, 5, Scalar(255, 0 ,0), 2);
	}*/
}

/** @function main */
int main(int argc, char** argv)
{
	int pos[2];
	int step[2];
	int field[2];
	int size;
	int valid;

	stringstream ss;	
	Mat frame;

	Mat frame_canny, frame_grey, frame_blur;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	VideoCapture cap(0); // open the default camera
	
	if(!cap.isOpened()) { // check if we succeeded
		cout << "Failed to open camera." << endl;
		return -1;
	}


	cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	cap >> frame;

	namedWindow("Game",1);
	moveWindow("Game",0,0);
	namedWindow("Grey_image",1);
	moveWindow("Grey_image",700,0);
	namedWindow("Gaussian_image",1);
	moveWindow("Gaussian_image",0,540);
	namedWindow("Canny_image",1);
	moveWindow("Canny_image",700,540);

	size = 10;
	pos[0] = frame.cols/2;
	pos[1] = frame.rows/2;
	field[0] = frame.cols;
	field[1] = frame.rows;

	srand(time(0));
	step[0] = (rand() % 21) - 20;
	step[1] = (rand() % 21) - 20;	

	for(;;)
	{
		cap >> frame; // get a new frame from camera
		flip(frame,frame,1);
		
		cvtColor(frame, frame_grey, CV_BGR2GRAY);
		threshold(frame_grey,frame_grey,100,255,CV_THRESH_BINARY_INV);
		imshow("Grey_image", frame_grey);

		GaussianBlur(frame_grey, frame_blur, Size(9,9), 2, 2);
	        imshow("Gaussian_image", frame_blur);
	
		Canny(frame_blur, frame_canny, 0, 30, 3);
	        imshow("Canny_image", frame_canny);

		/// Find contours
		findContours( frame_canny, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

		// Find squares
		for( size_t i = 0; i < contours.size(); i++) {
			findSquare(&frame, contours[i], &valid);
			if(valid) {
				/*ss.str("");
				ss << "valid";
				putText(frame, ss.str(),Point(190,80), 1, 1.15, Scalar(255,255,255), 1,CV_AA, false);*/
			}
		}			
	
		//Next steps:
		//1. Once we detect squares, draw the player's bar in the frame
		//2. When the ball interacts with the player's bar, the ball changes its direction
		//3. Make the game, points,...
					
		for(int i = 0; i < 2; i++) 
		{
			pos[i] += step[i];
			if(pos[i] < 0)
			{
				pos[i] = -pos[i];
				step[i] = -step[i];
			}
			else if(pos[i] > field[i]) 
			{
				pos[i] = (2 * field[i]) - pos[i];
				step[i] = -step[i];
			}

		}

		circle(frame, Point(pos[0], pos[1]),size, Scalar(0,0,255),size/2, CV_AA);
	
		/*ss.str("");
		ss << "pos[0]: " << pos[0] << " pos[1]: " << pos[1];
		putText(frame, ss.str(),Point(190,20), 1, 1.15, Scalar(255,255,255), 1,CV_AA, false);
		ss.str("");
		ss << "field.cols[0]: " << field[0] << " field.rows[1]: " << field[1];
		putText(frame, ss.str(),Point(190,40), 1, 1.15, Scalar(255,255,255), 1,CV_AA, false);
		ss.str("");
		ss << "step[0]: " << step[0] << " step[1]: " << step[1];
		putText(frame, ss.str(),Point(190,60), 1, 1.15, Scalar(255,255,255), 1,CV_AA, false);*/

		ss.str("");
		ss << "Player Left: 0    Player Right: 0";
		putText(frame, ss.str(),Point(180,20), 1, 1.15, Scalar(255,255,255), 1,CV_AA, false);

		imshow("Game", frame);

		if(waitKey(1) >= 0) break;
	}

	return 0;
}
