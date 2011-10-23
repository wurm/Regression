#ifndef CHARTDIRECTOR_H
#define CHARTDIRECTOR_H

#include <memory>
#include "global.h"
#include "chartdir.h"

using namespace std;

class ChartDirector
{
public:
	ChartDirector(){}
	virtual ~ChartDirector(){};

	XYChart* createChart(double* xValues, double* tValues, unsigned int& number);
	XYChart* createChart();


private:

	//void datapoints2doublepointer(vector<DataPoint> datapoints, double x[], double t[]);

	XYChart* test();
	XYChart* splinefitting();
	XYChart* curve();
};
#endif