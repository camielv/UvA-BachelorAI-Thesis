#pragma once

#include "opencv2/core/core.hpp"

namespace cv {
	float RectRadius(const vector<Point3f>& corners);
	float Kabsch(Mat& P, Mat& Q, Mat& R);
	bool getMatSubset( const Mat& m1, const Mat& m2, Mat& ms1, Mat& ms2, int maxAttempts, CvRNG& rng, int *subset_idx );
	bool checkSubset( const Mat& m, int count );
	void dumpMatrix(const Mat &mat);
	double MatMax(const Mat &mat);
	double MatMin(const Mat &mat);
	int MatNegCount(const Mat &mat);
	double ColMin(const Mat &mat, int col);
	double ColMax(const Mat &mat, int col);
	void RotationMatrix3D(const Mat& src, Mat& dst, bool yawlast=true);
	void TransformationMatrix(const Mat& pos, const Mat& or, Mat& T);
	void CalcLinePlaneIntersection(const Mat& Plane, const Mat& PlaneNormal, const Mat& Line, const Mat& LineNormal, Mat& intersection);
	void CalcLinePositionAtDistance(const Mat& Line, const Mat& LineNormal, double d, Mat& intersection);
	void MatFloatToDouble(const Mat &in, Mat &out);
	void MatDoubleToFloat(const Mat &in, Mat &out);
	void MatSetDiag(Mat &mtx, float *diag);
}