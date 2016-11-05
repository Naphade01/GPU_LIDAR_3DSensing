#include <iostream> 
#include <math.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#define k 10
#define cube_size 130

using std::cout;
using std::endl;
using cv::Mat;

using namespace cv;

int color_patch_pts[1] = { 4 };
int sc_size = cube_size + 30;
int x_off = 220, y_off = 220;

//World coordinate axes
Mat x_ax = (cv::Mat_<double>(4, 1) << 400, 0, 0, 1);
Mat y_ax = (cv::Mat_<double>(4, 1) << 0, 400, 0, 1);
Mat z_ax = (cv::Mat_<double>(4, 1) << 0, 0, 400, 1);

//Cube points
Mat cb_1 = (cv::Mat_<double>(4, 1) << cube_size, 0, cube_size, 1);
Mat cb_2 = (cv::Mat_<double>(4, 1) << 0, 0, cube_size, 1);
Mat cb_3 = (cv::Mat_<double>(4, 1) << 0, cube_size, cube_size, 1);
Mat cb_4 = (cv::Mat_<double>(4, 1) << cube_size, cube_size, cube_size, 1);
Mat cb_5 = (cv::Mat_<double>(4, 1) << cube_size, 0, 0, 1);
Mat cb_6 = (cv::Mat_<double>(4, 1) << cube_size, cube_size, 0, 1);
Mat cb_7 = (cv::Mat_<double>(4, 1) << 0, cube_size, 0, 1);

//Decoration points
cv::Point patch_pts[1][4];

//Destination points for Affine transform
Point2f dstTri[3];

//Screen points
Mat sc_1 = (cv::Mat_<double>(4, 1) << sc_size, 0, 0, 1);
Mat sc_2 = (cv::Mat_<double>(4, 1) << sc_size, 0, sc_size - 30, 1);
Mat sc_3 = (cv::Mat_<double>(4, 1) << sc_size, sc_size - 20, sc_size - 30, 1);
Mat sc_4 = (cv::Mat_<double>(4, 1) << sc_size, sc_size - 20, 0, 1);

Mat cam_eye = (cv::Mat_<double>(3, 1) << 200, 170, 150);
Mat org = (cv::Mat_<double>(4, 1) << 0, 0, 0, 1);

double rho = sqrt((cam_eye.at<double>(0, 0)*cam_eye.at<double>(0, 0)) + (cam_eye.at<double>(1, 0)*cam_eye.at<double>(1, 0)) + (cam_eye.at<double>(2, 0)*cam_eye.at<double>(2, 0)));
double thetha = (acos(cam_eye.at<double>(0, 0) / sqrt((cam_eye.at<double>(0, 0) * cam_eye.at<double>(0, 0)) + (cam_eye.at<double>(1, 0) * cam_eye.at<double>(1, 0)))));
double phi = (acos(cam_eye.at<double>(2, 0) / rho));

double proj_plane_dist = 200;

// Transformation matrix
Mat trans_mat = (cv::Mat_<double>(4, 4) << -sin(thetha), cos(thetha), 0.0, 0.0,
	-cos(phi) * cos(thetha), -cos(phi) * sin(thetha), sin(phi), 0.0,
	-sin(phi) * cos(thetha), -sin(phi) * cos(thetha), -cos(phi), rho,
	0.0, 0.0, 0.0, 1.0);

cv::Point2d transform(Mat);
void draw_cube(Mat);
void draw_screen(Mat);
void surface_decoration(Mat);
Mat camera();
void image_project(Mat,Mat);

int main()
{
	Mat image = Mat::zeros(500, 500, CV_8UC3);
	Mat fin_image = Mat::zeros(500, 500, CV_8UC3);

	//cout << "rho" << rho << "thetha" << thetha << "phi" << phi << endl;
	//Transform 3D to 2D point 
	cv::Point2d org_2d = transform(org);
	//cout << "x = " << org_2d.x << " y = " << org_2d.y << endl;
	cv::Point2d x_ax_2d = transform(x_ax);
	//cout << "x = " << x_ax_2d.x << " y = " << x_ax_2d.y << endl;
	cv::Point2d y_ax_2d = transform(y_ax);
	//cout << "x = " << y_ax_2d.x << " y = " << y_ax_2d.y << endl;
	cv::Point2d z_ax_2d = transform(z_ax);
	//cout << "x = " << z_ax_2d.x << " y = " << z_ax_2d.y << endl;

	line(image, Point(org_2d.x, org_2d.y), Point(x_ax_2d.x, x_ax_2d.y), Scalar(110, 220, 0), 2, 8);
	line(image, Point(org_2d.x, org_2d.y), Point(y_ax_2d.x, y_ax_2d.y), Scalar(110, 220, 0), 2, 8);
	line(image, Point(org_2d.x, org_2d.y), Point(z_ax_2d.x, z_ax_2d.y), Scalar(110, 220, 0), 2, 8);

	draw_cube(image);
	draw_screen(image);
	//surface_decoration(image);
	Mat resize_image = camera();
	image_project(resize_image, image);
	cv::flip(image, fin_image, 0);
	imshow("Image", fin_image);

	waitKey(0);
	return 0;
}

cv::Point2d transform(Mat threed_mat)
{
	Mat cam_view_3d = trans_mat * threed_mat;

	cv::Point2d twod_pt(0, 0);
	twod_pt.x = proj_plane_dist * cam_view_3d.at<double>(0, 0) / cam_view_3d.at<double>(2, 0);
	twod_pt.x += x_off;

	twod_pt.y = proj_plane_dist * cam_view_3d.at<double>(1, 0) / cam_view_3d.at<double>(2, 0);
	twod_pt.y += y_off;

	return twod_pt;
}

void draw_cube(Mat image)
{
	cv::Point2d cb1_2d = transform(cb_1);
	cv::Point2d cb2_2d = transform(cb_2);
	cv::Point2d cb3_2d = transform(cb_3);
	cv::Point2d cb4_2d = transform(cb_4);
	cv::Point2d cb5_2d = transform(cb_5);
	cv::Point2d cb6_2d = transform(cb_6);
	cv::Point2d cb7_2d = transform(cb_7);

	line(image, Point(cb2_2d.x, cb2_2d.y), Point(cb1_2d.x, cb1_2d.y), Scalar(110, 220, 0), 2, 8);
	line(image, Point(cb1_2d.x, cb1_2d.y), Point(cb4_2d.x, cb4_2d.y), Scalar(110, 220, 0), 2, 8);
	line(image, Point(cb4_2d.x, cb4_2d.y), Point(cb3_2d.x, cb3_2d.y), Scalar(110, 220, 0), 2, 8);
	line(image, Point(cb3_2d.x, cb3_2d.y), Point(cb2_2d.x, cb2_2d.y), Scalar(110, 220, 0), 2, 8);
	line(image, Point(cb1_2d.x, cb1_2d.y), Point(cb5_2d.x, cb5_2d.y), Scalar(110, 220, 0), 2, 8);
	line(image, Point(cb5_2d.x, cb5_2d.y), Point(cb6_2d.x, cb6_2d.y), Scalar(110, 220, 0), 2, 8);
	line(image, Point(cb6_2d.x, cb6_2d.y), Point(cb4_2d.x, cb4_2d.y), Scalar(110, 220, 0), 2, 8);
	line(image, Point(cb6_2d.x, cb6_2d.y), Point(cb7_2d.x, cb7_2d.y), Scalar(110, 220, 0), 2, 8);
	line(image, Point(cb7_2d.x, cb7_2d.y), Point(cb3_2d.x, cb3_2d.y), Scalar(110, 220, 0), 2, 8);

}

void draw_screen(Mat image)
{
	cv::Point2d sc1_2d = transform(sc_1);
	cv::Point2d sc2_2d = transform(sc_2);
	cv::Point2d sc3_2d = transform(sc_3);
	cv::Point2d sc4_2d = transform(sc_4);

	dstTri[0].x = sc2_2d.x; dstTri[0].y = sc2_2d.y;
	dstTri[1].x = sc3_2d.x; dstTri[1].y = sc3_2d.y;
	dstTri[2].x = sc1_2d.x; dstTri[2].y = sc1_2d.y;

	patch_pts[0][0].x = sc2_2d.x;
	patch_pts[0][0].y = sc2_2d.y;

	patch_pts[0][1].x = sc1_2d.x;
	patch_pts[0][1].y = sc1_2d.y;

	patch_pts[0][2].x = sc4_2d.x;
	patch_pts[0][2].y = sc4_2d.y;

	patch_pts[0][3].x = sc3_2d.x;
	patch_pts[0][3].y = sc3_2d.y;

	line(image, Point(sc1_2d.x, sc1_2d.y), Point(sc2_2d.x, sc2_2d.y), Scalar(110, 220, 0), 2, 8);
	line(image, Point(sc2_2d.x, sc2_2d.y), Point(sc3_2d.x, sc3_2d.y), Scalar(110, 220, 0), 2, 8);
	line(image, Point(sc3_2d.x, sc3_2d.y), Point(sc4_2d.x, sc4_2d.y), Scalar(110, 220, 0), 2, 8);
	line(image, Point(sc4_2d.x, sc4_2d.y), Point(sc1_2d.x, sc1_2d.y), Scalar(110, 220, 0), 2, 8);
}

void surface_decoration(Mat image)
{
	const cv::Point* ptr[1] = { patch_pts[0] };

	if (k > cube_size)
	{
		cout << "Decoration larger than cube size, please change value of k" << endl;
	}
	else if (k == cube_size)
	{
		cv::fillPoly(image, ptr, color_patch_pts, 1, Scalar(0, 0, 255), 8);
	}
	else
	{
		double offset = (cube_size - k) / 2;

		//Color patch decoration points
		Mat d_1 = (cv::Mat_<double>(4, 1) << sc_size, offset, sc_size - 30 - offset, 1);
		Mat d_2 = (cv::Mat_<double>(4, 1) << sc_size, offset, offset, 1);
		Mat d_3 = (cv::Mat_<double>(4, 1) << sc_size, sc_size - 20 - offset, offset, 1);
		Mat d_4 = (cv::Mat_<double>(4, 1) << sc_size, sc_size - 20 - offset, sc_size - 30 - offset, 1);

		patch_pts[0][0] = transform(d_1);
		patch_pts[0][1] = transform(d_2);
		patch_pts[0][2] = transform(d_3);
		patch_pts[0][3] = transform(d_4);

		cv::fillPoly(image, ptr, color_patch_pts, 1, Scalar(0, 0, 255), 8);
	}
}

Mat camera()
{
	int key = 0;
	Mat frame, resize_frame;
	VideoCapture cap(0); //open camera no.0  0=internal 1=external

	while ((key = waitKey(30)) != 27) //wait 30 milliseconds and check for esc key
	{
		cap >> frame; //save captured image to frame variable
		imshow("Camera", frame); //show image on window named Camera

		if (key == 'c')
		{
			imshow("Captured", frame);
			resize(frame, resize_frame, Size(cube_size-1, cube_size-1), 0, 0, INTER_CUBIC);
			imshow("resized frame", resize_frame);
			return resize_frame;
		}
	}
}

void image_project(Mat resize_image, Mat orig_image)
{
	Size s_r_m = resize_image.size();
	
#if 1
	s_r_m.width = 170;
	s_r_m.height = 170;
	Mat temp_fin = Mat::zeros(500, 500, CV_8UC3);
	Mat temp = Mat::zeros(500, 500, CV_8UC3);
	Mat warp_mat(2, 3, CV_32FC1);

	
	int rows = resize_image.rows;
	int columns = resize_image.cols;

	Point2f srcTri[3];

	srcTri[0].x = 0; srcTri[0].y = 0;
	srcTri[1].x = cube_size; srcTri[2].y = 0;
	srcTri[2].x = 0; srcTri[2].y = cube_size;

	warp_mat = getAffineTransform(srcTri, dstTri);
	warpAffine(resize_image, temp, warp_mat, temp.size());
	cv::flip(temp, temp_fin, 0);

	Mat left(temp, Rect(dstTri[2].x-50, dstTri[2].y, s_r_m.width, s_r_m.height));
	Mat right(orig_image, Rect(dstTri[2].x-20, dstTri[2].y-20, s_r_m.width, s_r_m.height));
	left.copyTo(right);

	//Mat left(orig_image, Rect(101, 101, s_r_m.width, s_r_m.height));
	//resize_image.copyTo(left);

	imshow("Test", temp_fin);

#endif

#if 0
	Mat image_s = Mat::zeros(500, 500, CV_8UC3);
	line(image_s, Point(100,100), Point(100,230), Scalar(110, 220, 0), 2, 8);
	line(image_s, Point(100, 230), Point(230, 230), Scalar(110, 220, 0), 2, 8);
	line(image_s, Point(230, 230), Point(230, 100), Scalar(110, 220, 0), 2, 8);
	line(image_s, Point(230, 100), Point(100, 100), Scalar(110, 220, 0), 2, 8);

	line(image_s, Point(70, 70), Point(70, 260), Scalar(110, 220, 0), 2, 8);
	line(image_s, Point(70, 260), Point(260, 260), Scalar(110, 220, 0), 2, 8);
	line(image_s, Point(260, 260), Point(260, 70), Scalar(110, 220, 0), 2, 8);
	line(image_s, Point(260, 70), Point(70, 70), Scalar(110, 220, 0), 2, 8);

	Mat left(image_s, Rect(101, 101, s_r_m.width, s_r_m.height));
	resize_image.copyTo(left);

	imshow("Test", image_s);

#endif
	
}