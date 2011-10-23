#include "UnitTesting.h"
#include <iostream>

#include "LinearRegression.h" 
#include "LinearEquation.h"
#include "DataImporter.h"

/*
erwartete Ausgabe:
xValues: 5, 12, 34, 108,...
tVlaues: 13, 33, 93, 285,...
*/
void UnitTesting::test_createLookUpTables()
{
	vector<DataPoint> dataPoints;
	dataPoints.push_back(DataPoint(2.0, 2.0));
	dataPoints.push_back(DataPoint(3.0, 4.0));
	dataPoints.push_back(DataPoint(4.0, 2.0));
	dataPoints.push_back(DataPoint(1.0, 1.0));
	dataPoints.push_back(DataPoint(2.0, 4.0));

	unsigned int m = 4;
	LinearRegression linRegr(m, dataPoints);

	linRegr.createLookUpTables();

	vector<double> xValues = linRegr.getXValues();
	vector<double> tValues = linRegr.getTValues();

	cout << "X_Values: " << endl;
	for(unsigned int i = 0; i < m; ++i)
		cout << xValues[i] << ", ";

	cout << endl;

	cout << "T_Values: " << endl;
	for(int i = 0; i < m; ++i)
		cout << tValues[i] << ", ";
}

/*
erwartete Ausgabe:
diagonale Matrix links unten nach rechts oben
*/
void UnitTesting::test_createMatrixLinearEquationsSystem()
{
	vector<DataPoint> dataPoints;
	int m = 4;

	dataPoints.push_back(DataPoint(2.0, 2.0));
	dataPoints.push_back(DataPoint(3.0, 4.0));
	dataPoints.push_back(DataPoint(4.0, 2.0));
	dataPoints.push_back(DataPoint(1.0, 1.0));
	dataPoints.push_back(DataPoint(2.0, 4.0));

	LinearRegression linRegr(m, dataPoints);

	linRegr.createLookUpTables();
	linRegr.createMatrixLinearEquationsSystem();

	double** matrix = linRegr.getMatrix();

	for( int c = 0; c < m; ++c )
	{
		for( int r = 0; r < m; ++r )
			cout << matrix[c][r] << " ";
		cout << endl;
	}
}

/*
Erwartete Ausgabe:
3 7
2 8
3 9
3 8
*/
void UnitTesting::test_getDataPoints(QString& path)
{
	vector<DataPoint> dataPoints;

	DataImporter importer;
	importer.getDataPoints(path, dataPoints);

	for(int i = 0; i < dataPoints.size(); ++i)
		cout << dataPoints[i].x << " " << dataPoints[i].t << endl;
}

void UnitTesting::test_determinant()
{
	vector< vector<double> > matrix3(3);
	vector<double> v1_3(3), v2_3(3), v3_3(3);
	
	LinearEquation linEqua;

	v1_3[0] = 0.0; v1_3[1] = 1.0; v1_3[2] = 2.0;
	v2_3[0] = 3.0; v2_3[1] = 2.0; v2_3[2] = 1.0;
	v3_3[0] = 1.0; v3_3[1] = 1.0; v3_3[2] = 0.0;
	matrix3[0] = v1_3;
	matrix3[1] = v2_3;
	matrix3[2] = v3_3;
	cout << "1. det(3x3 matrix) = 3 = " << linEqua.determinant(matrix3) << endl;

	v1_3[0] = -2.0; v1_3[1] = 2.0; v1_3[2] = -3.0;
	v2_3[0] = -1.0; v2_3[1] = 1.0; v2_3[2] =  3.0;
	v3_3[0] =  2.0; v3_3[1] = 0.0; v3_3[2] = -1.0;
	matrix3[0] = v1_3;
	matrix3[1] = v2_3;
	matrix3[2] = v3_3;
	cout << "2. det(3x3 matrix) = 18 = " << linEqua.determinant(matrix3) << endl;

	vector< vector<double> > matrix4(4);
	vector<double> v1_4(4), v2_4(4), v3_4(4), v4_4(4);
	v1_4[0] =  1.0; v1_4[1] = 4.0; v1_4[2] = 0.0; v1_4[3] =  3.0;
	v2_4[0] = -1.0; v2_4[1] = 2.0; v2_4[2] = 0.0; v2_4[3] =  1.0;
	v3_4[0] =  0.0; v3_4[1] = 5.0; v3_4[2] = 4.0; v3_4[3] =  2.0;
	v4_4[0] =  1.0; v4_4[1] = 0.0; v4_4[2] = 0.0; v4_4[3] = -2.0;
	matrix4[0] = v1_4;
	matrix4[1] = v2_4;
	matrix4[2] = v3_4;
	matrix4[3] = v4_4;
	cout << "4. det(4x4 matrix) = -56 = " << linEqua.determinant(matrix4) << endl;
													
}

// Das Bsp. ist von http://de.wikipedia.org/wiki/Cramersche_Regel
// 82 * x1 + 45 * x2 + 9 * x3 = 1
// 27 * x1 + 16 * x2 + 3 * x3 = 1
//  9 * x1 +  5 * x2 + 1 * x3 = 0
void UnitTesting::test_solveLinearEquation()
{
	vector< vector<double> > matrix3(3);
	vector<double> v1_3(3), v2_3(3), v3_3(3), vector_3(3);

	vector<double> result;

	LinearEquation linEqua;

	v1_3[0] = 82.0; v1_3[1] = 45.0; v1_3[2] = 9.0;
	v2_3[0] = 27.0; v2_3[1] = 16.0; v2_3[2] = 3.0;
	v3_3[0] =  9.0; v3_3[1] =  5.0; v3_3[2] = 1.0;
	matrix3[0] = v1_3;
	matrix3[1] = v2_3;
	matrix3[2] = v3_3;

	vector_3[0] = 1.0; vector_3[1] = 1.0; vector_3[2] = 0.0;

	result = linEqua.solveLinearEquation(matrix3, vector_3);
	cout << "11. solve linear equation(3x3 matrix) = {1, 1, -14} = " << "{" << result[0] << ", " << result[1] << ", " << result[2] << "}" << endl;


	v1_3[0] =  1.0; v1_3[1] = 0.0; v1_3[2] =  4.0;
	v2_3[0] =  1.0; v2_3[1] = 1.0; v2_3[2] =  6.0;
	v3_3[0] = -3.0; v3_3[1] = 0.0; v3_3[2] = -10.0;
	matrix3[0] = v1_3;
	matrix3[1] = v2_3;
	matrix3[2] = v3_3;

	vector_3[0] = 2.0; vector_3[1] = 3.0; vector_3[2] = 4.0;

	result = linEqua.solveLinearEquation(matrix3, vector_3);
	cout << "12. solve linear equation(3x3 matrix) = {-18, -9, 5} = " << "{" << result[0] << ", " << result[1] << ", " << result[2] << "}" << endl;
}