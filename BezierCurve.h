#pragma once
#ifndef _BEZIERCURVE_H_
#define _BEZIERCURVE_H_

#pragma warning(disable : 4786)  

#include "CurveEvaluator.h"
#include "vec.h"

class BezierCurveEvaluator : public CurveEvaluator
{
public:
	void evaluateCurve(const std::vector<Point>& control_points,
		std::vector<Point>& evaluated_curve_points,
		const float& animation_length,
		const bool& bWarp,
		const bool& bAdaptive,
		const double& bTension) const;
	void AdaptiveBezier(std::vector<Point>& evaluated_curve_points, const Vec4d& px, const Vec4d& py) const;
};
#endif 