#pragma once
#ifndef _BSPLINECURVE_H_
#define _BSPLINECURVE_H_

#pragma warning(disable : 4786)  

#include "CurveEvaluator.h"
#include "BezierCurve.h"

class BSplineCurveEvaluator : public CurveEvaluator
{
public:
	void evaluateCurve(const std::vector<Point>& control_points,
		std::vector<Point>& evaluated_control_points,
		const float& animation_length,
		const bool& bWarp,
		const bool& bAdaptive,
		const double& bTension) const;
};

#endif