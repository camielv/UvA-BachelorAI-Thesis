#pragma once

#include <Windows.h>
#include "opencv2/core/core.hpp"


namespace cv {

class ExtendedKalmanFilter
{
public:
    //! the default constructor
    ExtendedKalmanFilter();
    //! the full constructor taking the dimensionality of the state, of the measurement and of the control vector
	ExtendedKalmanFilter(int dynamParams, int measureParams, int controlParams=0, int type=CV_32F);
    //! re-initializes Kalman filter. The previous content is destroyed.
    void init(int dynamParams, int measureParams, int controlParams=0, int type=CV_32F);

    //! computes predicted state
    const Mat& predict(const Mat& control=Mat());
    //! updates the predicted state from the measurement
    const Mat& correct(const Mat& measurement);

	// NICK
	void lock();
	void release();

	void start(double time, float yaw);
	double difftime(double time);

    Mat statePre;           //!< predicted state (x'(k)): x(k)=A*x(k-1)+B*u(k)
    Mat statePost;          //!< corrected state (x(k)): x(k)=x'(k)+K(k)*(z(k)-H*x'(k))
    Mat transitionMatrix;   //!< state transition matrix (A)
    Mat controlMatrix;      //!< control matrix (B) (not used if there is no control)
    Mat measurementMatrix;  //!< measurement matrix (H)
    Mat processNoiseCov;    //!< process noise covariance matrix (Q)
    Mat measurementNoiseCov;//!< measurement noise covariance matrix (R)
    Mat errorCovPre;        //!< priori error estimate covariance matrix (P'(k)): P'(k)=A*P(k-1)*At + Q)*/
    Mat gain;               //!< Kalman gain matrix (K(k)): K(k)=P'(k)*Ht*inv(H*P'(k)*Ht+R)
    Mat errorCovPost;       //!< posteriori error estimate covariance matrix (P(k)): P(k)=(I-K(k)*H)*P'(k)
    
    // temporary matrices
    Mat temp1;
    Mat temp2;
    Mat temp3;
    Mat temp4;
    Mat temp5;

	// NICK
	HANDLE hMutex; // resource sharing
	float yaw_offset;
	double last_update;
	bool running;
};

}