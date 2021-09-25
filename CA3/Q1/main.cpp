#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include "x86intrin.h"
#include <cstdlib>
#include <sys/time.h>

using namespace cv;
using namespace std;

void show_image(cv::Mat img, string title) {
	cv::namedWindow(title, cv::WINDOW_AUTOSIZE);
	cv::imshow(title, img);
	waitKey(0);
}

void get_diff_image_serial(cv::Mat img1, cv::Mat img2, cv::Mat& diff_img, long& smicros) {
	unsigned int NCOLS = img1.cols;
	unsigned int NROWS = img1.rows;
	unsigned char *img1_data = (unsigned char *) img1.data;
	unsigned char *img2_data = (unsigned char *) img2.data;
	unsigned char *diff_img_data = (unsigned char *) diff_img.data;
	struct timeval start, end;
	gettimeofday(&start, NULL);
	for (int row = 0; row < NROWS; row++)
		for (int col = 0; col < NCOLS; col++)
			*(diff_img_data + row * NCOLS + col) = abs(*(img1_data + row * NCOLS + col) - *(img2_data + row * NCOLS + col));
	gettimeofday(&end, NULL);
	long seconds = (end.tv_sec - start.tv_sec);
	smicros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
	printf("Serial implementation took %ld micro seconds\n\n", smicros);
}

void get_diff_image_parallel(cv::Mat img1, cv::Mat img2, cv::Mat& diff_img, long& pmicros) {
	unsigned int NCOLS = img1.cols;
	unsigned int NROWS = img1.rows;
	__m128i *image1;
	__m128i *image2;
	__m128i *diff_img_data;
	__m128i m1, m2, m3;
	struct timeval start, end;

	image1 = (__m128i *) img1.data;
	image2 = (__m128i *) img2.data;
	diff_img_data = (__m128i *) diff_img.data;

	gettimeofday(&start, NULL);
	for (int i = 0; i < NROWS; i++)
		for (int j = 0; j < NCOLS / 16; j++)
		{
			m1 = _mm_loadu_si128(image1 + i * NCOLS/16 + j) ;
			m2 = _mm_loadu_si128(image2 + i * NCOLS/16 + j) ;
			m3 = _mm_abs_epi8 (_mm_sub_epi8 (m1, m2));
			_mm_storeu_si128 (diff_img_data + i * NCOLS/16 + j, m3);
		}
	gettimeofday(&end, NULL);
	long seconds = (end.tv_sec - start.tv_sec);
	pmicros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
	printf("Parallel implementation took %ld micro seconds\n\n", pmicros);
}

int main( )
{
	cv::Mat img1 = cv::imread("CA03__Q1__Image__01.png", IMREAD_GRAYSCALE);
	cv::Mat img2 = cv::imread("CA03__Q1__Image__02.png", IMREAD_GRAYSCALE);
	unsigned int NCOLS = img1.cols;
	unsigned int NROWS = img1.rows;
	cv::Mat diff_img_serial (NROWS, NCOLS, CV_8U);
	cv::Mat diff_img_parallel (NROWS, NCOLS, CV_8U);

	long pmicros;
	long smicros;

	get_diff_image_serial(img1, img2, diff_img_serial, smicros);
	get_diff_image_parallel(img1, img2, diff_img_parallel, pmicros);
	printf("Speedup: %f\n\n", ((float) smicros / (float) pmicros));

	show_image(diff_img_serial, "Serial Output");
	show_image(diff_img_parallel, "Parallel Output");

	return 0;
}