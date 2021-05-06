#include "BezierCurve.h"
#include "mat.h"
#include "vec.h"

#define SAMPLE 150

void BezierCurveEvaluator::AdaptiveBezier(std::vector<Point>& evaluated_control_points, const Vec4d& px, const Vec4d& py) const
{

	Point V[4];
	for (int i = 0; i < 4; ++i)
		V[i] = Point(px[i], py[i]);

	bool flatEnough = ((V[0].distance(V[1]) + V[1].distance(V[2]) + V[2].distance(V[3])) / V[0].distance(V[3]) < (1 + CurveEvaluator::s_fFlatnessEpsilon));

	if (flatEnough)
	{
		evaluated_control_points.push_back(V[0]);
		evaluated_control_points.push_back(V[3]);

	}
	else
	{
		
		Point V0[3];
		V0[0] = Point((V[0].x + V[1].x) / 2, (V[0].y + V[1].y) / 2);
		V0[1] = Point((V[1].x + V[2].x) / 2, (V[1].y + V[2].y) / 2);
		V0[2] = Point((V[2].x + V[3].x) / 2, (V[2].y + V[3].y) / 2);
		Point V1[2];
		V1[0] = Point((V0[0].x + V0[1].x) / 2, (V0[0].y + V0[1].y) / 2);
		V1[1] = Point((V0[1].x + V0[2].x) / 2, (V0[1].y + V0[2].y) / 2);
		Point Q((V1[0].x + V1[1].x) / 2, (V1[0].y + V1[1].y) / 2);
		AdaptiveBezier(evaluated_control_points, Vec4d(V[0].x, V0[0].x, V1[0].x, Q.x), Vec4d(V[0].y, V0[0].y, V1[0].y, Q.y));
		AdaptiveBezier(evaluated_control_points, Vec4d(Q.x, V1[1].x, V0[2].x, V[3].x), Vec4d(Q.y, V1[1].y, V0[2].y, V[3].y));
	}
}
void BezierCurveEvaluator::evaluateCurve(const std::vector<Point>& control_points, std::vector<Point>& evaluated_curve_points, 
	                                     const float& animation_length, const bool& bWarp, const bool& bAdaptive, const double& bTension) const
{
	evaluated_curve_points.clear();
	std::vector<Point> curveEvaluator;
	//assign start and end 
	curveEvaluator.assign(control_points.begin(), control_points.end()); 

	//Warp: 
	//    A: If selected node is on the curve, 
	//       the value will be determined by two value on the left and right value on curve
	//    B: Else it is not on curve,
	//       use linear interpolation 

	if (bWarp) {
		curveEvaluator.push_back(Point(control_points.front().x + animation_length, control_points.front().y));
	}

	int i;
	bool wrapped = false;


	for (i=0; i + 3 < curveEvaluator.size(); i += 3)
	{
		//push starting points and end points to the curve
		evaluated_curve_points.push_back(curveEvaluator[i]); 
		evaluated_curve_points.push_back(curveEvaluator[i + 3]); 

		const Vec4d px(curveEvaluator[i].x, curveEvaluator[i + 1].x, curveEvaluator[i + 2].x, curveEvaluator[i + 3].x);
		const Vec4d py(curveEvaluator[i].y, curveEvaluator[i + 1].y, curveEvaluator[i + 2].y, curveEvaluator[i + 3].y);

		//Non Adaptive
		if (!bAdaptive)
		{
			const Mat4d baseMatrix
			(-1, 3, -3, 1,
				3, -6, 3, 0,
				-3, 3, 0, 0,
				1, 0, 0, 0);
			//bezier curve formulation 
			for (int i = 0; i < SAMPLE; i++)
			{
				const double v = i / (double)SAMPLE;
				Vec4d T(v * v * v, v * v, v, 1);
				Point bezier_point(T * baseMatrix * px, T * baseMatrix * py);

				if (bWarp && bezier_point.x > animation_length)
				{
					//if within the setting range
					const float x_mod = fmod(bezier_point.x, animation_length);
					//check if it is already warped
					if (!wrapped)
					{
						const Point prev_point(control_points.back());
						evaluated_curve_points.push_back(Point(prev_point.x - animation_length, prev_point.y));
						evaluated_curve_points.push_back(bezier_point);
						wrapped = true;
					}
					bezier_point.x = x_mod;
				}
				evaluated_curve_points.push_back(bezier_point);
			}

		}
		else {
			//Adaptive
			//Using recursion 
			AdaptiveBezier(evaluated_curve_points, px, py);
		}
			
	}
	//print the number of points generated for a curve 
	printf("Evaluated Points : %d\n", evaluated_curve_points.size());

	//push back rest points
	
	for (; i < control_points.size(); i++) {
		evaluated_curve_points.push_back(control_points[i]);
	}

	if (bWarp)
	{
		if (!wrapped)
		{
			const float v = control_points.front().x + animation_length - control_points.back().x;
			const float percent = 1.0f - control_points.front().x / v;
			const float y = control_points.back().y + percent * (control_points[0].y - control_points.back().y);
			evaluated_curve_points.push_back(Point(animation_length, y));
			evaluated_curve_points.push_back(Point(0.0, y));
		}
	}
	else
	{
		evaluated_curve_points.push_back(Point(0, control_points[0].y));
		evaluated_curve_points.push_back(Point(animation_length, control_points.back().y));
	}


}