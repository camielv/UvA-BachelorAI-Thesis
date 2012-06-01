#include "opencv_helpers.h"
#include <cv.hpp>


namespace cv {

float RectRadius(const vector<Point3f>& corners)
{
	float minX, minY, maxX, maxY;
	minX = minY = FLT_MAX;
	maxX = maxY = -FLT_MAX;

	for (size_t i = 0; i < corners.size(); i++)
	{
		if (corners[i].x < minX)
			minX = corners[i].x;

		if (corners[i].x > maxX)
			maxX = corners[i].x;

		if (corners[i].y < minY)
			minY = corners[i].y;

		if (corners[i].y > maxY)
			maxY = corners[i].y;
	}

	float w = std::abs(maxX - minX) * 0.5f;
	float h = std::abs(maxY - minY) * 0.5f;

	return std::sqrt(w*w + h*h);
}


float Kabsch(Mat& P, Mat& Q, Mat& R)
{
	if (P.channels() > 2)
	{
		Mat _P = P.clone();
		Mat _Q = Q.clone();

		P = Mat(_P.rows, _P.cols, CV_32FC2);
		Q = Mat(_Q.rows, _Q.cols, CV_32FC2);

		size_t src_size = _P.elemSize();
		size_t dst_size = P.elemSize();

		for (int i = 0; i < P.cols * P.rows; i++)
		{
			memcpy_s(P.data + i * dst_size, dst_size, _P.data + i * src_size, dst_size);
			memcpy_s(Q.data + i * dst_size, dst_size, _Q.data + i * src_size, dst_size);
		}
	}

	P -= mean(P);
	Q -= mean(Q);

	P = P.reshape(1);
	Q = Q.reshape(1);

	float weight = 1.0f / P.rows;

	Mat A = (P.t() * weight) * Q;

	Mat D = Mat::eye(2, 2, CV_32F);

	if (determinant(A) < 0.0)
		D.at<float>(1,1) = -1.0;

	Mat w, u, vt;
	SVD::compute(A, w, u, vt);

	R = u * D * vt;

	return R.at<float>(0, 1);
}

bool getMatSubset( const Mat& m1, const Mat& m2, Mat& ms1, Mat& ms2, int maxAttempts, CvRNG& rng, int *subset_idx )
{
	bool checkPartialSubsets = false;

    cv::AutoBuffer<int> _idx(3);
    int* idx = _idx;
    int i = 0, j, k, idx_i, iters = 0;
    int type = CV_MAT_TYPE(m1.type()), elemSize = CV_ELEM_SIZE(type);
    const int *m1ptr = (int*) m1.data, *m2ptr = (int*) m2.data;
    int *ms1ptr = (int*)  ms1.data, *ms2ptr = (int*) ms2.data;
    int count = m1.cols*m1.rows;

    elemSize /= sizeof(int);

    for(; iters < maxAttempts; iters++)
    {
        for( i = 0; i < 3 && iters < maxAttempts; )
        {
            idx[i] = idx_i = cvRandInt(&rng) % count;
            for( j = 0; j < i; j++ )
                if( idx_i == idx[j] )
                    break;
            if( j < i )
                continue;

			// NICK
			subset_idx[i] = idx_i;

            for( k = 0; k < elemSize; k++ )
            {
                ms1ptr[i*elemSize + k] = m1ptr[idx_i*elemSize + k];
                ms2ptr[i*elemSize + k] = m2ptr[idx_i*elemSize + k];
            }
            if( checkPartialSubsets && (!checkSubset( ms1, i+1 ) || !checkSubset( ms2, i+1 )))
            {
                iters++;
                continue;
            }
            i++;
        }
        if( !checkPartialSubsets && i == 3 &&
            (!checkSubset( ms1, i ) || !checkSubset( ms2, i )))
            continue;
        break;
    }

    return i == 3 && iters < maxAttempts;
}

bool checkSubset( const Mat& m, int count )
{
    int j, k, i, i0, i1;
	bool checkPartialSubsets = true;
    //CvPoint2D32f* ptr = (CvPoint2D32f*) &m.data;
	CvPoint3D32f* ptr = (CvPoint3D32f*) &m.data;

    //assert( CV_MAT_TYPE(m.type()) == CV_32FC2 );
	assert( CV_MAT_TYPE(m.type()) == CV_32FC3 );
    
    if( checkPartialSubsets )
        i0 = i1 = count - 1;
    else
        i0 = 0, i1 = count - 1;
    
    for( i = i0; i <= i1; i++ )
    {
        // check that the i-th selected point does not belong
        // to a line connecting some previously selected points
        for( j = 0; j < i; j++ )
        {
            float dx1 = ptr[j].x - ptr[i].x;
            float dy1 = ptr[j].y - ptr[i].y;
            for( k = 0; k < j; k++ )
            {
                float dx2 = ptr[k].x - ptr[i].x;
                float dy2 = ptr[k].y - ptr[i].y;
                if( fabs(dx2*dy1 - dy2*dx1) <= FLT_EPSILON*(fabs(dx1) + fabs(dy1) + fabs(dx2) + fabs(dy2)))
                    break;
            }
            if( k < j )
                break;
        }
        if( j < i )
            break;
    }

    return i >= i1;
}


void dumpMatrix(const Mat &mat) { 
	const int t = mat.type(); 
	for (int i = 0; i < mat.rows; i++) { 
		for (int j = 0; j < mat.cols; j++) { 
			switch (t) { 
			case CV_32F: 
				printf(/*"%6.4f, "*/"%f, ", mat.at<float> (i, j)); 
				break; 
			case CV_64F: 
				printf("%6.4f, ", mat.at<double> (i, j)); 
				break; 
			case CV_8UC1:
				printf("%i, ", (int) mat.at<char> (i, j)); 
				break; 
			} 
		} 
		printf("\n"); 
	} 
	printf("\n");
} 

double MatMax(const Mat &mat)
{
	double max = 0.0;
	double *vals = (double*) mat.data;
	int elements = mat.rows * mat.cols;

	for (int i = 0; i < elements; i++)
	{
		if (vals[i] > max)
			max = vals[i];
	}

	return max;
}

double MatMin(const Mat &mat)
{
	double min = 0.0;
	double *vals = (double*) mat.data;
	int elements = mat.rows * mat.cols;

	for (int i = 0; i < elements; i++)
	{
		if (vals[i] < min)
			min = vals[i];
	}

	return min;
}

int MatNegCount(const Mat &mat)
{
	int neg_count = 0;
	double *vals = (double*) mat.data;
	int elements = mat.rows * mat.cols;

	for (int i = 0; i < elements; i++)
	{
		if (vals[i] < 0.0)
			neg_count++;
	}

	return neg_count;
}

double ColMin(const Mat &mat, int col)
{
	double min = FLT_MAX;

	for (int i = 0; i < mat.rows; i++)
	{
		if (mat.at<double> (i, col) < min)
			min = mat.at<double> (i, col);
	}

	return min;
}

double ColMax(const Mat &mat, int col)
{
	double max = FLT_MIN;

	for (int i = 0; i < mat.rows; i++)
	{
		if (mat.at<double> (i, col) > max)
			max = mat.at<double> (i, col);
	}

	return max;
}

void RotationMatrix3D(const Mat& src_m, Mat& dst_m, bool yawlast)
{
	//float* dst = (float*) dst_m.data;
	float* src = (float*) src_m.data; // angles in rad

	float CosRx = cos(src[0]);
	float CosRy = cos(src[1]);
	float CosRz = cos(src[2]);
	float SinRx = sin(src[0]);
	float SinRy = sin(src[1]);
	float SinRz = sin(src[2]);

	// http://www.codecogs.com/pages/forums/pagegen.php?id=455
	// http://www.intechopen.com/source/pdfs/11307/InTech-Visual_slam_and_moving_object_detection_for_a_small_size_humanoid_robot.pdf
	// http://46dogs.blogspot.com/2011/04/right-handed-rotation-matrix-for.html

	/* Note: this matrix effectively yaws first, then pitches, then rolls.  For the opposite order (and the one I ended up using), look below.*/
	if (!yawlast)
	{
		dst_m.at<float>(0, 0) = CosRy * CosRz;
		dst_m.at<float>(0, 1) = -SinRz * CosRy;
		dst_m.at<float>(0, 2) = SinRy;

		dst_m.at<float>(1, 0) = CosRz * SinRy * SinRx + SinRz * CosRx;
		dst_m.at<float>(1, 1) = -SinRz * SinRy * SinRx + CosRz * CosRx;
		dst_m.at<float>(1, 2) = -CosRy * SinRx;

		dst_m.at<float>(2, 0) = -CosRz * SinRy * CosRx + SinRz * SinRx;
		dst_m.at<float>(2, 1) = SinRz * SinRy * CosRx + CosRz * SinRx;
		dst_m.at<float>(2, 2) = CosRy * CosRx;
	}

	/* Note: this matrix effectively rolls first, then pitches, then yaws.  This turned out to be the one I used for my system.*/
	else
	{
		dst_m.at<float>(0, 0) = CosRy * CosRz;
		dst_m.at<float>(0, 1) = -SinRz * CosRx + CosRz * SinRy * SinRx;
		dst_m.at<float>(0, 2) = SinRx * SinRz + CosRz * SinRy * CosRx;

		dst_m.at<float>(1, 0) = CosRy * SinRz;
		dst_m.at<float>(1, 1) = CosRz * CosRx + SinRy * SinRz * SinRx;
		dst_m.at<float>(1, 2) = -SinRx * CosRz + SinRy * SinRz * CosRx;

		dst_m.at<float>(2, 0) = -SinRy;
		dst_m.at<float>(2, 1) = CosRy * SinRx;
		dst_m.at<float>(2, 2) = CosRy * CosRx;
	}
}


void TransformationMatrix(const Mat& pos, const Mat& or, Mat& T)
{
	RotationMatrix3D(or, T);

	T.at<float>(0, 3) = pos.at<float>(0);
	T.at<float>(1, 3) = pos.at<float>(1);
	T.at<float>(2, 3) = pos.at<float>(2);
}


void CalcLinePlaneIntersection(const Mat& Plane, const Mat& PlaneNormal, const Mat& Line, const Mat& LineNormal, Mat& intersection)
{
	double d = 0;
	double t = (d - PlaneNormal.dot(Line)) / PlaneNormal.dot(LineNormal);
	addWeighted(Line, 1.0, LineNormal, t, 0.0, intersection);
}


void CalcLinePositionAtDistance(const Mat& Line, const Mat& LineNormal, double d, Mat& intersection)
{
	addWeighted(Line, 1.0, LineNormal, d, 0.0, intersection);
}


void MatFloatToDouble(const Mat &in, Mat &out)
{
	for (int i = 0; i < in.rows; i++)
	{
		for (int j = 0; j < in.cols; j++)
		{
			out.at<double>(i, j) = (double) in.at<float>(i, j);
		}
	}
}


void MatDoubleToFloat(const Mat &in, Mat &out)
{
	for (int i = 0; i < in.rows; i++)
	{
		for (int j = 0; j < in.cols; j++)
		{
			out.at<float>(i, j) = (float) in.at<double>(i, j);
		}
	}
}

void MatSetDiag(Mat &mtx, float *diag)
{
	int rows = mtx.rows;

	for (int i = 0; i < rows; i++)
		mtx.at<float>(i, i) = diag[i];
}

}