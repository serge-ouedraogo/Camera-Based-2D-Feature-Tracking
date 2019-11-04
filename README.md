# SFND 2D Feature Tracking

<img src="images/keypoints.png" width="820" height="248" />

The idea of the camera course is to build a collision detection system - that's the overall goal for the Final Project. As a preparation for this, you will now build the feature tracking part and test various detector / descriptor combinations to see which ones perform best. This mid-term project consists of four parts:

* First, you will focus on loading images, setting up data structures and putting everything into a ring buffer to optimize memory load. 
* Then, you will integrate several keypoint detectors such as HARRIS, FAST, BRISK and SIFT and compare them with regard to number of keypoints and speed. 
* In the next part, you will then focus on descriptor extraction and matching using brute force and also the FLANN approach we discussed in the previous lesson. 
* In the last part, once the code framework is complete, you will test the various algorithms in different combinations and compare them with regard to some performance measures. 

See the classroom instruction and code comments for more details on each of these parts. Once you are finished with this project, the keypoint matching part will be set up and you can proceed to the next lesson, where the focus is on integrating Lidar points and on object detection using deep-learning. 

## Dependencies for Running Locally
* cmake >= 2.8
  * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1 (Linux, Mac), 3.81 (Windows)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* OpenCV >= 4.1
  * This must be compiled from source using the `-D OPENCV_ENABLE_NONFREE=ON` cmake flag for testing the SIFT and SURF detectors.
  * The OpenCV 4.1.0 source code can be found [here](https://github.com/opencv/opencv/tree/4.1.0)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools](https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory in the top level directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./2D_feature_tracking`.


## Number of Keypoints for each detector

| Detector      | Number of keypoints      
| ------------- |:-------------------:|
| Shitomasi     | 13423               |
| Harris        | 1737                | 
| ORB           | 5000                |
| BRISK         | 27116               |
| SIFT          | 13862               |
| FAST          | 17874               |
| AKAZE         | 13429               |

## Number of Matches for various Detector/Descriptor Combinations

|               | SIFT | BRIEF | BRISK | ORB | FREAK | AKAZE |    
| ------------- |:-----|:------|:------|:----|:------|:-----:|
| Shitomasi     | 690  |  944  | 767   | 908 | 768   |  --   |
| Harris        | 121  |  173  | 142   | 162 | 144   |  --   |
| ORB           | 649  |  545  | 751   | 763 | 420   |  --   |
| BRISK         | 1298 |  1704 | 1570  | 1514| 1524  |  --   |
| SIFT          | 800  |  597  | 536   | --- | 506   |  --   |
| FAST          | 776  |  1099 | 899   | 1071| 878   |  --   |
| AKAZE         | 1270 |  1266 | 1215  | 1182| 1187  |  1259 |


## Keypoints detection + description total time in ms

|               | SIFT  | BRIEF    | BRISK   | ORB     | FREAK | AKAZE |    
| ------------- |:----- |:---------|:--------|:--------|:-------|:-----:|
| Shitomasi     | 172.37|  174.709 | 172.369 | 171.263 | 145.164|  --   |
| Harris        | 121   |  198.579 | 197.618 | 201.257 | 164.424|  --   |
| ORB           | 649   |  80.1225 | 80.2804 | 80.57   | 77.299 |  --   |
| BRISK         | 1298  |  3704    | 3721.4  | 3734.78 | 3724.6 |  --   |
| SIFT          | 800   |  1225.79 | 1247.47 | ---     | 1222.12|  --   |
| FAST          | 776   |  21.9668 | 14.4059 | 13.9378 | 14.1339|  --   |
| AKAZE         | 1270  |  840     | 811.007 | 801.153 | 736.619|  803  |


## Matches rate (number of matches per millisecond)

|               | SIFT | BRIEF    | BRISK   | ORB     | FREAK | AKAZE |    
| ------------- |:-----|:---------|:--------|:--------|:-------|:-----:|
| Shitomasi     | 3.6  |  5.4     | 4.5     | 5.30    | 5.29   |  --   |
| Harris        | 0.61 |  0.87    | 0.72    | 0.80    | 0.88   |  --   |
| ORB           | 7.52 |  6.80    | 9.35    | 9.47    | 5.43   |  --   |
| BRISK         | 0.35 |  0.46    | 0.42    | 0.41    | 0.41   |  --   |
| SIFT          | 0.80 | 0.49     | 0.43    | ---     | 0.41   |  --   |
| FAST          | 43.60|  50.03   | 62.40   | 76.84   | 62.13  |  --   |
| AKAZE         | 1.66 |  1.51    | 1.5     |1.47     | 1.61   |  1.57 |

## TOP 3 Combination (Detector + Descriptor): 
1. FAST + ORB
2. FAST + BRISK
3. FAST + FREAK