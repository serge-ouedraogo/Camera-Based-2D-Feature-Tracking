#include <numeric>
#include "matching2D.hpp"

//#include "structIO.hpp"

using namespace std;

// Find best matches for keypoints in two camera images based on several matching methods
void matchDescriptors(std::vector<cv::KeyPoint> &kPtsSource, std::vector<cv::KeyPoint> &kPtsRef, cv::Mat &descSource, cv::Mat &descRef,
                      std::vector<cv::DMatch> &matches, std::string descriptorType, std::string matcherType, std::string selectorType)
{
    // configure matcher
    bool crossCheck = false;
    cv::Ptr<cv::DescriptorMatcher> matcher;

    if (matcherType.compare("MAT_BF") == 0)
    {
      int normType;  
      if(descriptorType.compare("DES_BINARY") ==0)
      {
        normType = cv::NORM_HAMMING;
      }
      else if(descriptorType.compare("DES_HOG") ==0)
      {
        normType = cv::NORM_L2;
      }
      matcher = cv::BFMatcher::create(normType, crossCheck);
    }
    
    else if (matcherType.compare("MAT_FLANN") == 0)
    {
      
       if (descSource.type() != CV_32F)
        { // OpenCV bug workaround : convert binary descriptors to floating point due to a bug in current OpenCV implementation
            descSource.convertTo(descSource, CV_32F);
            descRef.convertTo(descRef, CV_32F);
        } 
        
      
      matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
    }

    // perform matching task
    if (selectorType.compare("SEL_NN") == 0)
    { // nearest neighbor (best match)
        double t = (double) cv::getTickCount();
        matcher->match(descSource, descRef, matches); // Finds the best match for each descriptor in desc1
        t  = ((double) cv::getTickCount() - t)/cv::getTickFrequency();
       cout << " (NN) with n=" << matches.size() << " matches in " << 1000 * t / 1.0 << " ms" << endl;
      
    }
    else if (selectorType.compare("SEL_KNN") == 0)
    { // k nearest neighbors (k=2)

      vector<vector<cv::DMatch>> knn_2;
      double t = (double) cv::getTickCount();
      matcher->knnMatch(descSource, descRef, knn_2, 2);
      t  = ((double) cv::getTickCount() - t)/cv::getTickFrequency();
       cout << " (kNN) with n=" << knn_2.size() << " matches in " << 1000 * t / 1.0 << " ms" << endl;
      
      //Filter matches using descriptor distance ratio test
      double minDist_ratio = 0.8;
      for(auto it = knn_2.begin(); it!=knn_2.end(); ++it)
      {
        if ((*it)[0].distance < minDist_ratio * (*it)[1].distance)
        {
          matches.push_back((*it)[0]);
        }
      }
      cout << "# keypoints removed = " << knn_2.size() - matches.size() << endl;
      
    }
}
// Use one of several types of state-of-art descriptors to uniquely identify keypoints
void descKeypoints(vector<cv::KeyPoint> &keypoints, cv::Mat &img, cv::Mat &descriptors, string descriptorType)
{
    // select appropriate descriptor
    cv::Ptr<cv::DescriptorExtractor> extractor;
    if (descriptorType.compare("BRISK") == 0)
    {
        int threshold = 30;        // FAST/AGAST detection threshold score.
        int octaves = 3;           // detection octaves (use 0 to do single scale)
        float patternScale = 1.0f; // apply this scale to the pattern used for sampling the neighbourhood of a keypoint.

        extractor = cv::BRISK::create(threshold, octaves, patternScale);
    }
    else if(descriptorType.compare("SIFT") == 0)
    {
      extractor = cv::xfeatures2d::SiftDescriptorExtractor::create();
    }
    else if(descriptorType.compare("BRIEF") == 0)
    {
      int bytes = 32;
      bool use_orientation = false;
      extractor = cv::xfeatures2d::BriefDescriptorExtractor::create	(bytes, use_orientation);
    }
  
    else if(descriptorType.compare("FREAK") == 0)
    {
      bool orientationNormalized=true; 
      bool scaleNormalized=true; 
      float patternScale=22.0f; 
      int nOctaves=4;
      
      extractor = cv::xfeatures2d::FREAK::create(orientationNormalized, scaleNormalized, patternScale, nOctaves);
    }
  
    
    else if(descriptorType.compare("ORB") == 0)
    {
      extractor = cv::ORB::create();
    }
  
    
    else if(descriptorType.compare("AKAZE") == 0) 
    {
      extractor = cv::AKAZE::create();
    }
    
  
  
    // perform feature description
    double t = (double)cv::getTickCount();
    extractor->compute(img, keypoints, descriptors);
    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    cout << descriptorType << " descriptor extraction in " << 1000 * t / 1.0 << " ms" << endl;
}

// Detect keypoints in image using the traditional Shi-Thomasi detector
void detKeypointsShiTomasi(vector<cv::KeyPoint> &keypoints, cv::Mat &img, bool bVis)
{
    // compute detector parameters based on image size
    int blockSize = 4;       //  size of an average block for computing a derivative covariation matrix over each pixel neighborhood
    double maxOverlap = 0.0; // max. permissible overlap between two features in %
    double minDistance = (1.0 - maxOverlap) * blockSize;
    int maxCorners = img.rows * img.cols / max(1.0, minDistance); // max. num. of keypoints

    double qualityLevel = 0.01; // minimal accepted quality of image corners
    double k = 0.04;

    // Apply corner detection
    double t = (double)cv::getTickCount();
    vector<cv::Point2f> corners;
    cv::goodFeaturesToTrack(img, corners, maxCorners, qualityLevel, minDistance, cv::Mat(), blockSize, false, k);

    // add corners to result vector
    for (auto it = corners.begin(); it != corners.end(); ++it)
    {

        cv::KeyPoint newKeyPoint;
        newKeyPoint.pt = cv::Point2f((*it).x, (*it).y);
        newKeyPoint.size = blockSize;
        keypoints.push_back(newKeyPoint);
    }
    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    cout << "Shi-Tomasi detection with n=" << keypoints.size() << " keypoints in " << 1000 * t / 1.0 << " ms" << endl;

    // visualize results
    if (bVis)
    {
        cv::Mat visImage = img.clone();
        cv::drawKeypoints(img, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        string windowName = "Shi-Tomasi Corner Detector Results";
        cv::namedWindow(windowName, 6);
        imshow(windowName, visImage);
        cv::waitKey(0);
    }
}

void detKeypointsHarris(vector<cv::KeyPoint> &keypoints, cv::Mat &img, bool bVis)
{
// Detector parameters
    int blockSize = 2;     // for every pixel, a blockSize × blockSize neighborhood is considered
    int apertureSize = 3;  // aperture parameter for Sobel operator (must be odd)
    int minResponse = 100; // minimum value for a corner in the 8bit scaled response matrix
    double k = 0.04;       // Harris parameter (see equation for details)

    // Detect Harris corners and normalize output
    cv::Mat dst, dst_norm, dst_norm_scaled;
    dst = cv::Mat::zeros(img.size(), CV_32FC1);
    cv::cornerHarris(img, dst, blockSize, apertureSize, k, cv::BORDER_DEFAULT);
    cv::normalize(dst, dst_norm, 0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat());
    cv::convertScaleAbs(dst_norm, dst_norm_scaled);

   
   

    // Look for prominent corners and instantiate keypoints
    //vector<cv::KeyPoint> keypoints;
    double maxOverlap = 0.0; // max. permissible overlap between two features in %, used during non-maxima suppression
    for (size_t j = 0; j < dst_norm.rows; j++)
    {
        for (size_t i = 0; i < dst_norm.cols; i++)
        {
            int response = (int)dst_norm.at<float>(j, i);
            //std::cout << "response =" << response << std::endl;
            if (response > minResponse)
            { // only store points above a threshold

                cv::KeyPoint newKeyPoint;
                newKeyPoint.pt = cv::Point2f(i, j);
                newKeyPoint.size = 2 * apertureSize;
                newKeyPoint.response = response;

                // perform non-maximum suppression (NMS) in local neighbourhood around new key point
                bool bOverlap = false;
                for (auto it = keypoints.begin(); it != keypoints.end(); ++it)
                {
                    
                    double kptOverlap = cv::KeyPoint::overlap(newKeyPoint, *it);
                    if (kptOverlap > maxOverlap)
                    {
                        bOverlap = true;
                        if (newKeyPoint.response > (*it).response)
                        {                      // if overlap is >t AND response is higher for new kpt
                            *it = newKeyPoint; // replace old key point with new one
                            break;             // quit loop over keypoints
                        }
                    }
                }
                if (!bOverlap)
                {                                     // only add new key point if no overlap has been found in previous NMS
                    keypoints.push_back(newKeyPoint); // store new keypoint in dynamic list
                }
            }
        } // eof loop over cols
    }     // eof loop over rows

     cout << "Harris detection with n=" << keypoints.size() << endl;  
    // visualize keypoints
    if (bVis)
    {
        cv::Mat visImage = img.clone();
        cv::drawKeypoints(img, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        string windowName = "Harris Corner Detector Results";
        cv::namedWindow(windowName, 6);
        imshow(windowName, visImage);
        cv::waitKey(0);
    }
}

//void detKeypointsFast(vector<cv::KeyPoint> &keypoints, cv::Mat &img, string detectorType, bool bVis)
void detKeypointsModern(vector<cv::KeyPoint> &keypoints, cv::Mat &img, string detectorType, bool bVis)
{
  int threshold = 30;
  bool nms = true;

  if(detectorType =="FAST")
  {
    cv::FastFeatureDetector::DetectorType type = cv::FastFeatureDetector::DetectorType::TYPE_9_16;
    
    double t = ((double)cv::getTickCount());
    
    cv::Ptr<cv::FastFeatureDetector> detector = cv::FastFeatureDetector::create(threshold, nms, type);
    detector->detect(img, keypoints);
    
    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    cout << "FAST detection with n=" << keypoints.size() << " keypoints in " << 1000 * t / 1.0 << " ms" << endl;
    
    // visualize results
    if (bVis)
    {
      cv::Mat visImage = img.clone();
      cv::drawKeypoints(img, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
      string windowName = "FAST Corner Detector Results";
      cv::namedWindow(windowName, 6);
      imshow(windowName, visImage);
      cv::waitKey(0);
    }
  }
  
  if(detectorType =="BRISK")
  {
    double t = ((double)cv::getTickCount());
    
    cv::Ptr<cv::FeatureDetector> detector = cv::BRISK::create();
    detector->detect(img, keypoints);
    
    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    cout << "BRISK detection with n=" << keypoints.size() << " keypoints in " << 1000 * t / 1.0 << " ms" << endl;
    
    // visualize results
    if (bVis)
    {
      cv::Mat visImage = img.clone();
      cv::drawKeypoints(img, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
      string windowName = " BRISK Corner Detector Results";
      cv::namedWindow(windowName, 6);
      imshow(windowName, visImage);
      cv::waitKey(0);
    } 

  }
  
  if(detectorType =="ORB")
  {
    double t = ((double)cv::getTickCount());
    
    cv::Ptr<cv::FeatureDetector> detector = cv::ORB::create();
    detector->detect(img, keypoints);
    
    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    cout << "ORB detection with n=" << keypoints.size() << " keypoints in " << 1000 * t / 1.0 << " ms" << endl;
    
    // visualize results
    if (bVis)
    {
      cv::Mat visImage = img.clone();
      cv::drawKeypoints(img, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
      string windowName = " ORB Corner Detector Results";
      cv::namedWindow(windowName, 6);
      imshow(windowName, visImage);
      cv::waitKey(0);
    } 
  }
  

  if(detectorType =="SIFT")
  {

    cv::Ptr<cv::FeatureDetector> detector = cv::xfeatures2d::SIFT::create();
    double t = ((double)cv::getTickCount());
    detector->detect(img, keypoints);
    
    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    cout << "SIFT detection with n=" << keypoints.size() << " keypoints in " << 1000 * t / 1.0 << " ms" << endl;

    if (bVis)
    {
      cv::Mat visImage = img.clone();
      cv::drawKeypoints(img, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
      string windowName = " SIFT Corner Detector Results";
      cv::namedWindow(windowName, 6);
      imshow(windowName, visImage);
      cv::waitKey(0);
    }   
  }
  
  if(detectorType =="AKAZE")
  {
    double t = ((double)cv::getTickCount());
    
    cv::Ptr<cv::FeatureDetector> detector = cv::AKAZE::create();
    detector->detect(img, keypoints);
    
    t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    cout << "AKAZE detection with n=" << keypoints.size() << " keypoints in " << 1000 * t / 1.0 << " ms" << endl;
    
    // visualize results
    if (bVis)
    {
      cv::Mat visImage = img.clone();
      cv::drawKeypoints(img, keypoints, visImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
      string windowName = " AKAZE Corner Detector Results";
      cv::namedWindow(windowName, 6);
      imshow(windowName, visImage);
      cv::waitKey(0);
    } 
  }
}
  