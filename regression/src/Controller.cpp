#include "Controller.h"
#include "LinearRegression.h"
#include "Regularization.h"

vector<double> Controller::linearRegressionByOptimalM( vector<DataPoint>& datapoints )
{
	// 3 - 10 coefficients
	unsigned int mMin = 3;
	unsigned int mMax = 10;

	return linearRegressionByOptimalM(datapoints, mMax, mMin);
}

vector<double> Controller::linearRegressionByOptimalM( vector<DataPoint>& datapoints, const unsigned int mMax, const unsigned int mMin )
{
	// Erms for trainingpoints
	vector<double> ermsTraining(mMax);
	// Erms for testpoints
	vector<double> ermsTest(mMax);

	return linearRegressionByOptimalM(datapoints, ermsTraining, ermsTest, mMax, mMin);
}

vector<double> Controller::linearRegressionByOptimalM( vector<DataPoint>& datapoints, vector<double>& ermsTraining, vector<double>& ermsTest, const unsigned int mMax, const unsigned int mMin )
{
	unsigned int number = datapoints.size();
	// ca 10%
	unsigned int ratio = 10;
	vector<DataPoint> trainingDatapoints(number / ratio);
	// saving results
	vector<vector<double>> coefficientsVectors(mMax);

	// linear regression, optimization
	LinearRegression regression;
	Regularization regularization;

	// optimal m, delta between errors
	unsigned int optimal_M;
	double delta = DBL_MAX;

	// 10% for calculating coefficients, 90% for optimization
	for(unsigned int i = 0; i < trainingDatapoints.size(); ++i)
	{
		unsigned int index = i * ratio;
		trainingDatapoints[i] = datapoints[index];
		index = index == 0 ? 0 : index -1;
		datapoints.erase(datapoints.begin() + index);
	}

	// linear regression
	for( unsigned int m = mMin; m < mMax; ++m )
	{
		// calculation coeeficients by training datapoints
		vector<double> coefficients = regression.calculateCoefficients(m, trainingDatapoints);
		// error for both sets and saving errors and coefficients
		double ermsTr = regularization.calcErms(trainingDatapoints, coefficients);
		ermsTraining[m] = ermsTr;
		double ermsTe = regularization.calcErms(datapoints, coefficients);
		ermsTest[m] = ermsTe;
		coefficientsVectors[m - mMin] = coefficients;
		// output on console
		cout << "M = " << m << endl;
		cout << "ERMS Training: " << ermsTr << endl;
		cout << "ERMS Test: " << ermsTe << endl << endl;

		// optimal m results from little delta between test and training error
		double curDelta = sqrt( pow((ermsTr - ermsTe), 2.0) );
		if( curDelta < delta )
		{
			optimal_M = m;
			delta = curDelta;
		}
	}

	return coefficientsVectors[optimal_M - mMin];
}