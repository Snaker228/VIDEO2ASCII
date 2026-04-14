#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

const string ASCII_CHARS = " .,:;+*?%S#@";
int TARGET_FPS = 30;

string pixelToAscii(int brightness) {
    int index = (brightness * (ASCII_CHARS.length() - 1)) / 255;
    return string(1, ASCII_CHARS[index]);
}

void clearScreen() {
    cout << "\033[2J\033[1;1H";
}

int main(int argc, char* argv[]) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    int TARGET_WIDTH = w.ws_col;
    int TARGET_HEIGHT = w.ws_row;
    if ((argc != 3 || string(argv[1]) != "--video") && (argc != 5 || string(argv[3]) != "--fps")) {
        cerr << "Usage: " << argv[0] << " --video <video_file>" << endl;
        return 1;
    }

    if (argc == 5 && string(argv[3]) == "--fps") {
        TARGET_FPS = stoi(argv[4]);
    }

    string videoPath = argv[2];
    VideoCapture cap(videoPath);
    
    if (!cap.isOpened()) {
        cerr << "Error: Could not open video file: " << videoPath << endl;
        return 1;
    }

    double fps = cap.get(CAP_PROP_FPS);
    if (fps <= 0) fps = TARGET_FPS;
    
    int frameDelay = 1000 / TARGET_FPS;
    int totalFrames = cap.get(CAP_PROP_FRAME_COUNT);
    
    cout << "Processing video: " << videoPath << endl;
    cout << "Original FPS: " << fps << ", Target FPS: " << TARGET_FPS << endl;
    cout << "Target resolution: " << TARGET_WIDTH << "x" << TARGET_HEIGHT << endl;
    cout << "Total frames: " << totalFrames << endl;

    printf("Строки: %d, Столбцы: %d", w.ws_row, w.ws_col);
    for(int i = 0; i < argc; i++) {
        cout << "Аргумент " << i << ": " << argv[i] << endl;
    }
        
    
    Mat frame, gray, resized;
    // cap >> frame;
    // if(TARGET_WIDTH/TARGET_HEIGHT > frame.cols/frame.rows) {
    //     TARGET_WIDTH = ((frame.cols * TARGET_HEIGHT) / frame.rows) * 2;
    // } else {
    //     TARGET_HEIGHT = ((frame.rows * TARGET_WIDTH) / frame.cols)/2;
    // }
    cout << "Frame size: " << frame.cols << "x" << frame.rows << endl;
    cout << "Processing video: " << videoPath << endl;
    cout << "Original FPS: " << fps << ", Target FPS: " << TARGET_FPS << endl;
    cout << "Target resolution: " << TARGET_WIDTH << "x" << TARGET_HEIGHT << endl;
    cout << "Total frames: " << totalFrames << endl;
    // Control frame rate
    this_thread::sleep_for(chrono::milliseconds(1000));

    while (true) {
        auto start = std::chrono::steady_clock::now();
        cap >> frame;
        if (frame.empty()) break;

        // Convert to grayscale
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        
        // Resize to target dimensions
        resize(gray, resized, Size(TARGET_WIDTH, TARGET_HEIGHT), 0, 0, INTER_LINEAR);
        
        // Clear previous frame from screen
        clearScreen();
        
        // Convert each pixel to ASCII and print immediately
        for (int y = 0; y < resized.rows; y++) {
            string line;
            for (int x = 0; x < resized.cols; x++) {
                uchar brightness = resized.at<uchar>(y, x);
                line += pixelToAscii(brightness);
            }
            cout << line << endl;
        }
        
        // Control frame rate
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = end - start;
        this_thread::sleep_for(chrono::milliseconds(frameDelay-(int)(duration.count()*1000)));
    }

    cap.release();
    return 0;
}
