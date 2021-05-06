
#include "CatmullRomCurve.h"
#include <assert.h>
#include "mat.h"
#include "vec.h"
#include "modelerapp.h"

#define SAMPLE 30

void CatmullRomCurveEvaluator::evaluateCurve(const std::vector<Point>& control_points,std::vector<Point>& evaluated_control_points,
											const float& animation_length,const bool& bWrap,const bool& bAdaptive,const double& bTension) const
{
	evaluated_control_points.clear();

	std::vector<Point> curveEvaluator;

	if (bWrap)
	{


		Point start1 = Point((control_points.end() - 2)->x - animation_length,
			(control_points.end() - 2)->y);
		Point start2 = Point((control_points.end() - 1)->x - animation_length,
			(control_points.end() - 1)->y);
		Point end1 = Point((control_points.begin())->x + animation_length,
			(control_points.begin())->y);
		Point end2 = Point((control_points.begin() + 1)->x + animation_length,
			(control_points.begin() + 1)->y);
		curveEvaluator.push_back(start1);
		curveEvaluator.push_back(start2);
		curveEvaluator.insert(curveEvaluator.end(), control_points.begin(), control_points.end());
		curveEvaluator.push_back(end1);
		curveEvaluator.push_back(end2);

	}
	else
	{
		curveEvaluator.push_back(control_points.front());
		curveEvaluator.push_back(control_points.front());
		curveEvaluator.insert(curveEvaluator.end(), control_points.begin(), control_points.end());
		curveEvaluator.push_back(control_points.back());
		curveEvaluator.push_back(control_points.back());
	}

	const Mat4d basis = Mat4d(
		-1, 3, -3, 1,
		2, -5, 4, -1,
		-1, 0, 1, 0,
		0, 2, 0, 0) * bTension;

	for (int i = 0; i < curveEvaluator.size() - 3; i++)
	{
		Vec4d px = Vec4d(
			curveEvaluator[i].x,
			curveEvaluator[i + 1].x,
			curveEvaluator[i + 2].x,
			curveEvaluator[i + 3].x);
		Vec4d py = Vec4d(
			curveEvaluator[i].y,
			curveEvaluator[i + 1].y,
			curveEvaluator[i + 2].y,
			curveEvaluator[i + 3].y);
		for (int j= 0; j < SAMPLE; j++)
		{
			double t = (double)j / (double)SAMPLE;
			Vec4d v = Vec4d(t * t * t, t * t, t, 1);
			Point eval_point = Point(v * basis * px, v * basis * py);
			if (evaluated_control_points.empty() || eval_point.x > evaluated_control_points.back().x)
			{
				evaluated_control_points.push_back(eval_point);
			}
		}
	}
	if (!bWrap)
	{
		if (control_points.front().x < evaluated_control_points.front().x)
		{
			evaluated_control_points.push_back(control_points.front());
		}
		if (control_points.back().x > evaluated_control_points.back().x)
		{
			evaluated_control_points.push_back(control_points.back());
		}
		evaluated_control_points.push_back(Point(0, evaluated_control_points.front().y));
		evaluated_control_points.push_back(Point(animation_length, evaluated_control_points.back().y));
	}
}