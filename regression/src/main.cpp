#include <QApplication>
#include "global.h"
#include "ChartDirector.h"
#include "QViewChart.h"
#include "DataImporter.h"

#include "UnitTesting.h"

void datapoints2doublepointer(vector<DataPoint> datapoints, double* x, double* t)
{
	for( int i = 0; i < datapoints.size(); ++i )
	{
		x[i] = (double)(datapoints[i].x);
		t[i] = (double)(datapoints[i].t);
	}
}

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);

   QViewChart view;
  
   view.show();
   QString filename = view.openDirectory();

   // get data points from file
   vector<DataPoint> dataPoints;
   DataImporter importer;
   importer.getDataPoints(filename, dataPoints);

   // chartdirector expected double pointer
   unsigned int number = dataPoints.size();
   double* xValues = new double[number];
   double* tValues = new double[number];
   datapoints2doublepointer(dataPoints, xValues, tValues);

   // create and show chart
   ChartDirector chartdir;
   view.setChart( chartdir.createChart(xValues, tValues, number) );

   // testing
   //UnitTesting ui;
   //ui.test_createLookUpTables();
   //ui.test_createMatrixLinearEquationsSystem();
   //ui.test_getDataPoints(filename);

   delete[] xValues, tValues;
   return app.exec();
}



