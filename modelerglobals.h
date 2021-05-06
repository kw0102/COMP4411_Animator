#ifndef _MODELER_GLOBALS_H
#define _MODELER_GLOBALS_H

#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502
#endif

// This is a list of the controls for the SampleModel
// We'll use these constants to access the values 
// of the controls from the user interface.
enum SampleModelControls
{
	XPOS, YPOS, ZPOS,
	LIGHT_X, LIGHT_Y, LIGHT_Z,
	HEIGHT,
	EYE_X, EYE_Y,
	LHAND_UPPER_X, LHAND_UPPER_Y, LHAND_UPPER_Z, LHAND_LOWER,
	RHAND_UPPER_X, RHAND_UPPER_Y, RHAND_UPPER_Z, RHAND_LOWER,
	LLEG_UPPER_X, LLEG_UPPER_Y, LLEG_UPPER_Z, LLEG_LOWER,
	RLEG_UPPER_X, RLEG_UPPER_Y, RLEG_UPPER_Z, RLEG_LOWER,
	IK_LIMB, IK_TARGET_X, IK_TARGET_Y, IK_TARGET_Z, IK_MIN, IK_MAX,
	JELLY_MOVE, SNAIL_MOVE,
	NUMCONTROLS
};

// Colors
#define COLOR_RED		1.0f, 0.0f, 0.0f
#define COLOR_GREEN		0.0f, 1.0f, 0.0f
#define COLOR_BLUE		0.0f, 0.0f, 1.0f

// We'll be getting the instance of the application a lot; 
// might as well have it as a macro.
#define VAL(x) (ModelerApplication::Instance()->GetControlValue(x))
#define SET(x, y) (ModelerApplication::Instance()->SetControlValue(x, y))

#endif