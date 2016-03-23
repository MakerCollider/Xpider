#include <iostream>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>

#include <opencv2/opencv.hpp>

#include "SF_Controller.h"

cv::VideoCapture camera;

bool running;
pthread_t grabThread;
pthread_mutex_t mutexLock;

void clear(int signo)
{
    std::cout << "Get exit signal" << std::endl;
    running = false;
}

void* grabFunc(void* in_data)
{
    while(running)
    {
        pthread_mutex_lock(&mutexLock);
        if(camera.isOpened())
        {
            camera.grab();
        }
        pthread_mutex_unlock(&mutexLock);
    }

    std::cout << "Grab thread exit." << std::endl;
}

void init()
{
    //Setup edi robot mraa control lib
    setup();

    //摄像头
    camera.open(0);

    if(!camera.isOpened())
    {
        std::cout << "Can not find camera!" << std::endl;
        exit(-1);
    }

    camera.set(cv::CAP_PROP_FRAME_WIDTH,160);
    camera.set(cv::CAP_PROP_FRAME_HEIGHT,120);

    double width = camera.get(cv::CAP_PROP_FRAME_WIDTH);
    double height = camera.get(cv::CAP_PROP_FRAME_HEIGHT);

    std::cout << "width:" << width << ", ";
    std::cout << "height:" << height << std::endl;

    running = true;
    pthread_mutex_init(&mutexLock, NULL);
    pthread_create(&grabThread, NULL, grabFunc, NULL);

    signal(SIGINT, clear);
    signal(SIGTERM, clear);
}

int main()
{
    double timeUse;
    struct timeval startTime, stopTime;

    cv::Mat rawImage;
    cv::Mat grayImage, thresImage, cannyImage;

    double area;
    bool hasCircles;
    int targetX, targetY;
    cv::Mat circleImage(160, 120, CV_8UC1);
    std::vector<cv::Vec3f> circles;
    std::vector<std::vector<cv::Point> > contours;

    init();

    while (running)
    {
        gettimeofday(&startTime, NULL);

        //初始化outputImage和读取摄像头
        circleImage.setTo(0);
        camera.retrieve(rawImage);

        //前期处理（灰度、自适应二值化、边缘检测）
        cv::cvtColor(rawImage, grayImage, cv::COLOR_RGB2GRAY);
        cv::adaptiveThreshold(grayImage, thresImage, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY,201,20);
        cv::Canny(thresImage, cannyImage, 150, 200);

        //检测轮廓
        cv::findContours(cannyImage, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
        hasCircles = false;
        for (int i = 0; i < contours.size(); i++)
        {
            //根据轮廓面积初步筛选
            area = fabs(cv::contourArea(contours[i]));
            if (area > 200)
            {
                //绘制轮廓并填充
                cv::drawContours(circleImage, contours, i, cv::Scalar(255, 255, 255), cv::FILLED);
                //检测是否有圆形
                cv::HoughCircles(circleImage, circles, cv::HOUGH_GRADIENT, 2, circleImage.rows / 2, 30, 15);
                if (circles.size() > 0)
                {
                    hasCircles = true;
                    targetX = circles[0][0];
                    targetY = circles[0][1];
                    std::cout << "Find Circles! " << std::endl;
                    std::cout << "Area: " << area << " " << std::endl;
                    std::cout << "Location: " << targetX << ", " << targetY << std::endl; 
                    break;
                }
            }

        }

        if(hasCircles)
        {
            if(targetX < 60)
            {
                Rotation_Control(20, 255);
            }
            else if(targetX > 100)
            {
                Rotation_Control(-20, 255);
            }

            Move_Control(1, 80);
        }
        else
        {
            std::cout << "No Circles!" << std::endl;
            Move_Control(M_Direction, STOP);
            Pitch_Control(60);
        }

        gettimeofday(&stopTime, NULL);
        timeUse = stopTime.tv_sec - startTime.tv_sec + (stopTime.tv_usec - startTime.tv_usec)/1000000.0;
        std::cout << "Time: " <<timeUse << std::endl;
        if(timeUse < 0.1)
            usleep((0.1 - timeUse) * 1000000);
    }

    void* result;
    pthread_join(grabThread, &result);

    Move_Control(M_Direction, STOP);
    Pitch_Control(60);  

    std::cout << "Program exit!" << std::endl;
    
    return 0;
}