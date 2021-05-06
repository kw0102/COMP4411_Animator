#include "BSplineCurve.h"
#include "BezierCurve.h"
#include "mat.h"
#include "vec.h"

#define SAMPLE 15

void BSplineCurveEvaluator::evaluateCurve(const std::vector<Point>& control_points, std::vector<Point>& evaluated_control_points,
	const float& animation_length, const bool& bWarp, const bool& bAdaptive, const double& bTension) const
{

	evaluated_control_points.clear();
	std::vector<Point> CurveEvaluator;

	if (bWarp)
	{

		CurveEvaluator.push_back(Point((control_points.end() - 2)->x - animation_length, (control_points.end() - 2)->y));
		CurveEvaluator.push_back(Point((control_points.end() - 1)->x - animation_length, (control_points.end() - 1)->y));
		CurveEvaluator.insert(CurveEvaluator.end(), control_points.begin(), control_points.end());
		CurveEvaluator.push_back(Point((control_points.begin())->x + animation_length, (control_points.begin())->y));
		CurveEvaluator.push_back(Point((control_points.begin() + 1)->x + animation_length, (control_points.begin() + 1)->y));
	}
	else
	{
		evaluated_control_points.push_back(Point(0, control_points[0].y));
		evaluated_control_points.push_back(Point(animation_length, control_points.back().y));
		CurveEvaluator.push_back(control_points[0]);
		CurveEvaluator.push_back(control_points[0]);
		CurveEvaluator.insert(CurveEvaluator.end(), control_points.begin(), control_points.end());
		CurveEvaluator.push_back(control_points.back());
		CurveEvaluator.push_back(control_points.back());
	}
	
	BezierCurveEvaluator bezierCurveEvaluator;

	const Mat4d convert = Mat4d
	   (1, 4, 1, 0,
		0, 4, 2, 0,
		0, 2, 4, 0,
		0, 1, 4, 1) / 6.0;

	for (int i = 0; i + 3 < CurveEvaluator.size(); ++i)
	{
		Vec4d bezier_x(CurveEvaluator[i].x, CurveEvaluator[i + 1].x, CurveEvaluator[i + 2].x, CurveEvaluator[i + 3].x);
		Vec4d bezier_y(CurveEvaluator[i].y, CurveEvaluator[i + 1].y, CurveEvaluator[i + 2].y, CurveEvaluator[i + 3].y);
		Vec4d convert_x = convert * bezier_x;
		Vec4d convert_y = convert * bezier_y;
		std::vector<Point> converter;

		for (int j = 0; j < 4; j++) {
			converter.push_back(Point(convert_x[j], convert_y[j]));

		}
			
		std::vector<Point> evaluated;
		bezierCurveEvaluator.evaluateCurve(converter, evaluated, animation_length, false, bAdaptive, bTension);
		evaluated_control_points.insert(evaluated_control_points.end(), evaluated.begin(), evaluated.end());


	}

}