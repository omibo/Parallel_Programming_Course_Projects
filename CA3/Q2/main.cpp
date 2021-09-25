#include <string>
#include <sys/time.h>
#include "x86intrin.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>


cv::Mat parallelResImg, serialResImg;

void showImage(cv::Mat img, const char* name){
    cv::namedWindow(name, cv::WINDOW_AUTOSIZE);
    cv::imshow(name, img);
    cv::waitKey(0);
}

unsigned char saturatedAdd(unsigned char a, unsigned char b){
    unsigned char res = a + b;
    if (res < a){
        return (unsigned char) 255;
    }
    return res;
}

long addImageSerial(cv::Mat background, cv::Mat cover){
    serialResImg = background.clone();
    unsigned char *bgPointer = (unsigned char*) background.data;
    unsigned char *cvPointer = (unsigned char*) cover.data;
    unsigned char *resPointer = (unsigned char*) serialResImg.data;

	struct timeval startTime, endTime;
	gettimeofday(&startTime, NULL);

    for(int row=0; row<cover.rows; row++){
        for(int col=0; col<cover.cols; col++){
            *(resPointer + row*background.cols + col) = saturatedAdd(
                *(bgPointer + row*background.cols + col), (*(cvPointer + row*cover.cols + col)) >> 1);
        }
    }

    gettimeofday(&endTime, NULL);

    long seconds = (endTime.tv_sec - startTime.tv_sec);
	long micros = ((seconds * 1000000) + endTime.tv_usec) - (startTime.tv_usec);

    return seconds * 1000000 + micros;
}

long addImageParallel(cv::Mat background, cv::Mat cover){
    parallelResImg = background.clone();

    __m128i *bgPointer = (__m128i *) background.data;
    __m128i *cvPointer = (__m128i *) cover.data;
    __m128i *resPointer = (__m128i *) parallelResImg.data;
    __m128i bg1, cv1;
    __m128i constantShift = _mm_set1_epi8(0x7F);

	struct timeval startTime, endTime;
	gettimeofday(&startTime, NULL);

    for(int row=0; row<cover.rows; row++){
        for(int col=0; col<cover.cols/16; col++){
            bg1 = _mm_loadu_si128(bgPointer + row*background.cols/16 + col);
            cv1 = _mm_loadu_si128(cvPointer + row*cover.cols/16 + col);
            cv1 = _mm_and_si128(_mm_srli_epi16(cv1, 1), constantShift);
            cv1 = _mm_adds_epu8(cv1, bg1);           
            _mm_storeu_si128(resPointer + row*background.cols/16 + col, cv1);
        }
    }

    gettimeofday(&endTime, NULL);

    long seconds = (endTime.tv_sec - startTime.tv_sec);
	long micros = ((seconds * 1000000) + endTime.tv_usec) - (startTime.tv_usec);


    return seconds * 1000000 + micros;
}

int main()
{
    cv::Mat background = cv::imread("CA03__Q2__Image__01.png", cv::IMREAD_GRAYSCALE);
    cv::Mat cover = cv::imread("CA03__Q2__Image__02.png",  cv::IMREAD_GRAYSCALE);
    
    long smicros = addImageSerial(background, cover);
    long pmicros = addImageParallel(background, cover);

    printf("Serial implementation took %ld micro seconds\n\n", smicros);
    printf("Parallel implementation took %ld micro seconds\n\n", pmicros);
    printf("Speedup: %f\n\n", ((float) smicros / (float) pmicros));

    showImage(background, "BackgroundImage");
    showImage(cover, "CoverImage");
    showImage(serialResImg, "Serial Result");
    showImage( parallelResImg, "Parallel Result");
    return 0;
}