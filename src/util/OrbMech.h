/***************************************************************************
This file is part of the Shuttle Ascent Design System.

The Shuttle Ascent Design System is free software: you can redistribute it
and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

The Shuttle Ascent Design System is distributed in the hope that it will
be useful,but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public Licensealong with
the Shuttle Ascent Design System. If not, see <https://www.gnu.org/licenses/>.

**************************************************************************/

#pragma once

#include <cmath>
#include "VectorMath.h"
#include "Constants.h"

namespace OrbMech
{
	struct StateVector
	{
		StateVector::StateVector()
		{
			R = V = _V(0, 0, 0);
			GMT = CD = Area = Weight = 0.0;
		}

		VECTOR3 R;
		VECTOR3 V;
		double GMT;
		double CD;
		double Area;
		double Weight;
	};

	struct GlobalConstants
	{
		GlobalConstants();

		double EARTH_MU;
		double EARTH_RADIUS_EQUATOR;
		double J2_GRAV;
		double EARTH_RATE;
		double ELLIPT;

		// Shuttle OMS performance data
		double FT;
		double VEX;
	};

	// CONVERSIONS
	
	// Rotation matrix from inertial to LVLH
	MATRIX3 LVLH_Matrix(VECTOR3 R, VECTOR3 V);
	// 
	void normalizeAngle(double& a, bool positive = true);
	// Spherical coordinates conversion
	void PICSSC(bool vecinp, VECTOR3& R, VECTOR3& V, double& r, double& v, double& lat, double& lng, double& gamma, double& azi);
	// Latitude and longitude from position vector
	void latlong_from_r(VECTOR3 R, double& lat, double& lng);

	// Subroutine to compute an insertion vector
	void ENSERT(double LATLS, double LNG, double PFA, double PFT, double RINS, double VINS, double GAMINS, double GMTLO, double YSMAX, double w_E, VECTOR3 IY, StateVector &sv);
	// Calculate IY vector
	VECTOR3 CalculateEFIYVector(double Incl, double Lat, double Lng, bool north, double dt_bias);
	// Inertial launch site vector
	VECTOR3 GetURLS(double GMTLO, double lat, double lng, double w_E);
	// Altitude above ellipsoid
	double H_ELLIPSOID(const GlobalConstants& cnst, VECTOR3 R);
	// Earth relative velocity
	VECTOR3 V_REL(const GlobalConstants& cnst, VECTOR3 R, VECTOR3 V);
	// Calculate argument of latitude
	double ArgLat(VECTOR3 R, VECTOR3 V);
	// TEG to Earth-fixed matrix
	MATRIX3 TEG_to_EF_Matrix(const GlobalConstants& cnst, double gmt);

	// DRAG
	void GLFDEN(double Z, double& DEN, double& CS);

	// STRING FORMATTING
	void GMT2String_DHHMMSSCS(char* buf, double GMT);
	void GMT2String_HHMMSSCS(char* buf, double GMT);
	void GMT2String_MMSS(char* buf, double GMT);

	// MATHS
	template <typename T> int sgn(T val) {
		return (T(0) < val) - (val < T(0));
	}

	// Two dimensional Newton's method
	class NewtonsMethod2D
	{
	public:
		NewtonsMethod2D(double diff1, double diff2);

		void Step(int i, double err1, double err2, double& x1, double& x2);
	protected:

		// Increments
		double diff[2];
		// Errors with initial guess
		double err0[2];

		double Jacobian[2][2];
		double Jinv[2][2];
		double Inc[2];
	};
}