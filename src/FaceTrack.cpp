#include <iostream>
#include <pthread.h>
#include <signal.h>
#include <math.h>
#include <sys/time.h>

#include <opencv2/opencv.hpp>

#include "SF_Controller.h"

cv::VideoCapture camera;

cv::CascadeClassifier face_cascade;
std::string face_cascade_name;

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

    std::cout << "width:" << width << "\t";
    std::cout << "height:" << height << "\t" << std::endl;

    face_cascade_name = "/usr/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml";

    if(!face_cascade.load(face_cascade_name))
    {
        std::cout << "can not find face_cascade_file!" << std::endl;
        exit(-1);
    }

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

    cv::Mat rawImage, grayImage;
    std::vector<cv::Rect> faces;

    init();

    float faceX, faceY;
    while(running)
    {
        gettimeofday(&startTime, NULL);

        camera.retrieve(rawImage);
        cv::cvtColor(rawImage, grayImage, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(grayImage, grayImage);

        faces.clear();
        face_cascade.detectMultiScale(grayImage, faces, 1.1,
            2, 0|cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));
        if(faces.empty())
        {
            std::cout << "Face lost!" << std::endl;
            Move_Control(M_Direction, STOP);
            Pitch_Control(60);
        }
        else
        {
            std::cout << "Face detect!" << faces.size() << std::endl;
            faceX = faces[0].x+faces[0].width*0.5;
            faceY = faces[0].y+faces[0].height*0.5;
            std::cout << "First face location: " << faceX << ", " << faceY << std::endl;

            if(faceX < 70)
            {
                Rotation_Control(5, 255);
            }
            else if(faceX > 90)
            {
                Rotation_Control(-5, 255);
            }

            Move_Control(1, 70);
        }

        gettimeofday(&stopTime, NULL);
        timeUse = stopTime.tv_sec - startTime.tv_sec + (stopTime.tv_usec - startTime.tv_usec)/1000000.0;
        std::cout << "Time: " <<timeUse << std::endl;
        if(timeUse < 0.25)
            usleep((0.25 - timeUse) * 1000000);
    }

    void* result;
    pthread_join(grabThread, &result);

    Move_Control(M_Direction, STOP);
    Pitch_Control(60);  

    std::cout << "Program exit!" << std::endl;

    return 0;
}