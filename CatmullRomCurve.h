#pragma once
#ifndef INCLUDED_CATMULLROM_CURVE_EVALUATOR_H
#define INCLUDED_CATMULLROM_CURVE_EVALUATOR_H

#pragma warning(disable : 4786)  
#include "curveevaluator.h"
class CatmullRomCurveEvaluator :
    public CurveEvaluator
{
	void evaluateCurve(const std::vector<Point>& control_points,
		std::vector<Point>& evaluated_control_points,
		const float& animation_length,
		const bool& bWrap,
		const bool& bAdaptive,
		const double& dTension) const;

};

#endif
