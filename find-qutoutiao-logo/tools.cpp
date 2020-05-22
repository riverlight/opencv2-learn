#include "tools.h"

float vmax(vector<float> v)
{
	int i;
	float fmax = 0;
	for (i = 0; i < v.size(); i++)
		if (v[i]>fmax)
			fmax = v[i];

	return fmax;
}

float vmin(vector<float> v)
{
	int i;
	float fmin = 100000;
	for (i = 0; i < v.size(); i++)
		if (v[i]<fmin)
			fmin = v[i];

	return fmin;
}

