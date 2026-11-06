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

#include <stdio.h>
#include "OrbMech.h"
#include "Constants.h"

namespace OrbMech
{
	GlobalConstants::GlobalConstants()
	{
		EARTH_MU = 398600640000000.0;
		EARTH_RADIUS_EQUATOR = 6378166.0;
		J2_GRAV = 1.0826271e-3;
		EARTH_RATE = 7.29211514646e-5;
		ELLIPT = 3.352329869e-3;

		FT = 2.0 * 26700.0;
		VEX = 316.0 * 9.80665;
	}

	MATRIX3 LVLH_Matrix(VECTOR3 R, VECTOR3 V)
	{
		VECTOR3 i, j, k;
		j = unit(crossp(V, R));
		k = unit(-R);
		i = crossp(j, k);
		return _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z); //rotation matrix to LVLH
	}

	void normalizeAngle(double& a, bool positive)
	{
		a = fmod(a, MathConstants::PI2);
		if (a < 0)
			a += MathConstants::PI2;
		if (!positive)
		{
			if (a > MathConstants::PI)
			{
				a -= MathConstants::PI2;
			}
		}
	}

	void PICSSC(bool vecinp, VECTOR3& R, VECTOR3& V, double& r, double& v, double& lat, double& lng, double& gamma, double& azi)
	{
		// gamma definition: -90 degrees < gamma < 90 degrees
		if (vecinp)
		{
			r = length(R);
			lat = asin(R.z / r);
			lng = atan2(R.y, R.x);
			if (lng < 0)
			{
				lng += MathConstants::PI2;
			}
			VECTOR3 TEMP = mul(_M(cos(lng) * cos(lat), sin(lng) * cos(lat), sin(lat), -sin(lng), cos(lng), 0, -sin(lat) * cos(lng), -sin(lat) * sin(lng), cos(lat)), V);
			v = length(TEMP);
			gamma = asin(TEMP.x / v);
			azi = atan2(TEMP.y, TEMP.z);
			if (azi < 0)
			{
				azi += MathConstants::PI2;
			}
		}
		else
		{
			R = _V(cos(lat) * cos(lng), cos(lat) * sin(lng), sin(lat)) * r;
			V = mul(_M(cos(lat) * cos(lng), -sin(lng), -sin(lat) * cos(lng), cos(lat) * sin(lng), cos(lng), -sin(lat) * sin(lng), sin(lat), 0, cos(lat)), _V(sin(gamma), cos(gamma) * sin(azi), cos(gamma) * cos(azi)) * v);
		}
	}

	void latlong_from_r(VECTOR3 R, double& lat, double& lng)
	{
		VECTOR3 u;

		u = unit(R);
		lat = atan2(u.z, sqrt(u.x * u.x + u.y * u.y));
		lng = atan2(u.y, u.x);
	}

	void ENSERT(double LATLS, double LNG, double PFA, double PFT, double RINS, double VINS, double GAMINS, double GMTLO, double YSMAX, double w_E, VECTOR3 IY, StateVector& sv)
	{
		// INPUTS:
		// LATLS = Geocentric launch site latitude, rad
		// LNG = Geographic launch site longitude, rad
		// PFA = Powered flight arc, rad
		// PFT = Powered flight time, sec
		// RINS = 
		// GMTLO = Liftoff time, sec
		// IY = Negative of orbital plane vector
		// OUTPUTS:
		// sv = State vector R, V, GMT

		MATRIX3 MATR1, MATR2, MATR3;
		VECTOR3 URLS, H, K, J;
		double LONG, PSI, WEDGE, BGAMM, DELYAW;

		LONG = LNG + w_E * GMTLO;

		URLS = _V(cos(LATLS) * cos(LONG), cos(LATLS) * sin(LONG), sin(LATLS));

		H = unit(-IY);
		K = unit(crossp(H, URLS));
		J = crossp(K, H);

		PSI = asin(dotp(H, URLS));
		WEDGE = asin(sin(PSI) / cos(PFA));
		BGAMM = asin(sin(PFA) / cos(PSI));
		DELYAW = abs(WEDGE) - YSMAX;
		if (YSMAX > 0.)
		{
			if (DELYAW <= 0.)
			{
				BGAMM = acos(cos(PFA) / cos(PSI));
				WEDGE = 0.0;
			}
			else
			{
				double cosx, x, y, sinG;

				cosx = cos(PFA) / cos(YSMAX);
				x = acos(cosx);
				y = asin(sin(PSI) / cosx);
				WEDGE = abs(y) - YSMAX;
				sinG = sin(x) / cos(PSI);
				BGAMM = acos(sqrt(1.0 - sinG * sinG));
			}
		}
		MATR1 = _M(J.x, J.y, J.z, K.x, K.y, K.z, H.x, H.y, H.z);
		MATR2 = mul(_M(cos(BGAMM), sin(BGAMM), 0., -sin(BGAMM), cos(BGAMM), 0., 0., 0., 1.), MATR1);
		if (DELYAW > 0.)
		{
			double phi;
			phi = PSI / abs(PSI) * WEDGE;
			MATR2 = mul(_M(cos(phi), 0., sin(phi), 0., 1., 0., -sin(phi), 0., cos(phi)), MATR2);
		}
		MATR3 = mul(_M(cos(GAMINS), -sin(GAMINS), 0., sin(GAMINS), cos(GAMINS), 0., 0., 0., 1.), MATR2);
		sv.R = _V(MATR2.m11, MATR2.m12, MATR2.m13) * RINS;
		sv.V = _V(MATR3.m21, MATR3.m22, MATR3.m23) * VINS;
		sv.GMT = GMTLO + PFT;
	}

	VECTOR3 CalculateEFIYVector(double Incl, double Lat, double Lng, bool north, double dt_bias)
	{
		// Calculate insertion orbital plane (IY) in Earth-fixed coordinates, taking into account Earth rotation
		// INPUTS:
		// Incl = Desired inclination, radians
		// Lat = Launch or present latitude, radians
		// Lng = Launch or present longitude, radians
		// north = launch heading is northerly, true = north, false = south
		// dt_bias = Time bias for rotating Earth. Length of time before the target orbit plane is over the landing site, seconds

		double bias, arg, beta, dlng, h;

		const double w_E = 7.292114942213369e-05;

		bias = w_E * dt_bias;
		arg = cos(Incl) / cos(Lat);

		//Limit to 1.0
		if (abs(arg) > 1.0)
		{
			if (arg > 0.0)
			{
				beta = MathConstants::PI05;
			}
			else
			{
				beta = -MathConstants::PI05;
			}
		}
		else
		{
			beta = asin(arg);
		}
		if (north == false)
		{
			beta = MathConstants::PI - beta;
		}
		dlng = atan2(sin(Lat), cos(beta) / sin(beta));
		if (dlng < 0)
		{
			dlng = dlng + MathConstants::PI2;
		}
		if (MathConstants::PI - beta < 0.0)
		{
			dlng = MathConstants::PI + dlng;
		}
		h = Lng - dlng + bias;
		if (h < 0)
		{
			h = h + MathConstants::PI2;
		}

		return -_V(sin(Incl) * sin(h), -sin(Incl) * cos(h), cos(Incl));
	}

	VECTOR3 GetURLS(double GMTLO, double lat, double lng, double w_E)
	{
		double LONG;

		LONG = lng + w_E * GMTLO;
		return _V(cos(lat) * cos(LONG), cos(lat) * sin(LONG), sin(lat));
	}

	double H_ELLIPSOID(const GlobalConstants& cnst, VECTOR3 R)
	{
		VECTOR3 EARTH_POLE;

		EARTH_POLE = _V(0, 0, 1);

		return length(R) - ((1.0 - cnst.ELLIPT) * cnst.EARTH_RADIUS_EQUATOR) / sqrt(1.0 + (pow(1.0 - cnst.ELLIPT, 2) - 1.0) * (1.0 - pow(dotp(unit(R), EARTH_POLE), 2)));
	}

	VECTOR3 V_REL(const GlobalConstants& cnst, VECTOR3 R, VECTOR3 V)
	{
		return V - crossp(_V(0, 0, 1), R) * cnst.EARTH_RATE;
	}

	double ArgLat(VECTOR3 R, VECTOR3 V)
	{
		VECTOR3 K, h, n;
		double u;

		K = _V(0, 0, 1);
		h = unit(crossp(R, V));
		n = crossp(K, h);
		u = acos(dotp(unit(n), unit(R)));
		if (R.z < 0)
		{
			u = MathConstants::PI2 - u;
		}
		return u;
	}

	MATRIX3 TEG_to_EF_Matrix(const GlobalConstants& cnst, double gmt)
	{
		double CL, SL;

		CL = cos(gmt * cnst.EARTH_RATE);
		SL = sin(gmt * cnst.EARTH_RATE);

		return _M(CL, SL, 0.0, -SL, CL, 0.0, 0.0, 0.0, 1.0);
	}

	void GLFDEN(double Z, double& DEN, double& CS)
	{
		//Z is the geometric altitude in meters, DEN is the output density in kg/m^2 and CS is the output speed of sound in m/s

		static const double HB[10] = { -5000.0, 0.0, 11000.0, 20000.0, 32000.0, 47000.0, 52000.0, 61000.0, 79000.0, 88743.0 };
		static const double ZB[14] = { 90000.0, 100000.0, 110000.0, 120000.0, 150000.0, 160000.0, 170000.0, 190000.0, 230000.0, 300000.0, 400000.0, 500000.0, 600000.0, 700000.0 };
		static const double TMB[24] = { 320.65, 288.15, 216.65, 216.65, 228.65, 270.65, 270.65, 252.65, 180.65, 180.65, 180.65, 210.65,
										260.65, 360.65, 960.65, 1110.65, 1210.65, 1350.65, 1550.65, 1830.65, 2160.65, 2420.65, 2590.65, 2700.65 };
		static const double ALP[24] = { 1.2087778e1, 1.1526088e1, 1.002712e1, 8.6079235, 6.7662077, 4.7086738, 4.0775458, 2.9019653, 3.7006732e-2, -1.8055744, -1.8055744,
			-3.504061, -4.9124564, -5.9828218, -7.5886378, -7.9035491, -8.183367, -8.6884559, -9.5726883, -1.0879634e1, -1.2421644e1, -1.3724116e1, -1.4879663e1, -1.594263e1 };
		static const double GB[14] = { 9.535, 9.505, 9.476, 9.447, 9.36, 9.331, 9.302, 9.246, 9.134, 8.942, 8.679, 8.428, 8.187, 7.956 };

		//Test for lower limit of atmosphere
		if (Z < 0.0)
		{
			Z = 0.0;
		}

		//Test for upper limit of atmosphere
		if (Z >= 700000.0)
		{
			//Above 700km density is set to zero and speed of sound is set to the 90km value
			DEN = 0.0;
			CS = 269.44;
			return;
		}

		double ALPP, TM, P, AL;
		int N;

		//Test for atmosphere layers for Z less than or equal to 90 km
		if (Z <= 90000.0)
		{
			double H;

			//Below 90km
			//Compute geopotential altitude H
			H = ((((((-9.5013649e-35 * Z) + 6.0621354e-28) * Z - 3.8667054e-21) * Z + 2.4656553e-14) * Z - 1.5731262e-7) * Z + 1.0) * Z;

			for (N = 0; N < 8; N++)
			{
				if (H <= HB[N + 1]) break;
			}

			//Compute AL, the temperature gradient
			AL = (TMB[N + 1] - TMB[N]) / (HB[N + 1] - HB[N]);
			//Compute TM, the molecular scale temperature and CS, the speed of sound
			TM = TMB[N] + AL * (H - HB[N]);
			CS = 20.04680273 * sqrt(TM);
			if (AL != 0.0)
			{
				//When AL is not zero, the natural log of pressure is computed as a function of AL
				ALPP = ALP[N] - 3.41631947e-2 * log((AL * (H - HB[N]) + TMB[N]) / TMB[N]) / AL;
			}
			else
			{
				//When ALP is zero, the natural log of pressure is computed without using AL
				ALPP = ALP[N] - 3.41631947e-2 * (H - HB[N]) / TMB[N];
			}
		}
		else
		{
			double G, GG;
			int M;

			//Above 90km
			//Set speed of sound equalo to the 90km value
			CS = 269.44;
			//Test Z against base layer altitudes
			for (M = 0; M < 12; M++)
			{
				if (Z <= ZB[M + 1]) break;
			}
			//Set layer number N to correct base layer value
			N = M + 10;
			//Compute AL, temperature gradient
			AL = (TMB[N + 1] - TMB[N]) / (ZB[M + 1] - ZB[M]);
			//G, acceleration of gravity at altitude Z
			G = (((((-5.5905936e-33) * Z + 2.972462e-26) * Z - 1.5167771e-19) * Z + 7.2539455e-13) * Z - 3.0854195e-6) * Z + 9.806665;
			//GG, approximation of G for extraction from within the integral
			GG = (G + GB[M]) / 2.0;
			//TM, molecular scale temperature
			TM = TMB[N] + AL * (Z - ZB[M]);
			if (AL != 0.0)
			{
				//When AL is not zero, the natural log of pressure is computed as a function of AL
				ALPP = ALP[N] - 3.48367635e-3 * GG * log((Z - ZB[M] + TMB[N] / AL) * AL / TMB[N]) / AL;
			}
			else
			{
				//When ALP is zero, the natural log of pressure is computed without using AL
				ALPP = ALP[N] - 3.48367635e-3 * GG * (Z - ZB[M]) / TMB[N];
			}
		}
		//Calculate pressure
		P = exp(ALPP);
		//Compute dewnsity in kg/meters-cubed
		DEN = 3.48367635e-3 * P / TM;
	}

	void GMT2String_DHHMMSSCS(char* buf, double GMT)
	{
		//Format: D:HH:MM:SS.SSS
		GMT = round(GMT * 1000.0) / 1000.0;
		sprintf(buf, "0:%02.0lf:%02.0lf:%06.3lf", floor(GMT / 3600.0), floor(fmod(GMT, 3600.0) / 60.0), fmod(GMT, 60.0)); // TBD
	}

	void GMT2String_HHMMSSCS(char* buf, double GMT)
	{
		//Format: HH:MM:SS.S
		GMT = round(GMT * 10.0) / 10.0;
		sprintf(buf, "%02.0lf:%02.0lf:%04.1lf", floor(GMT / 3600.0), floor(fmod(GMT, 3600.0) / 60.0), fmod(GMT, 60.0));
	}

	void GMT2String_MMSS(char* buf, double GMT)
	{
		//Format: MM:SS
		GMT = round(GMT);
		sprintf(buf, "%02.0lf:%02.0lf", floor(GMT / 60.0), fmod(GMT, 60.0));
	}

	NewtonsMethod2D::NewtonsMethod2D(double diff1, double diff2)
	{
		diff[0] = diff1;
		diff[1] = diff2;
	}

	void NewtonsMethod2D::Step(int i, double err1, double err2, double& x1, double& x2)
	{
		if (i == 0)
		{
			//Store errors, increment val1
			err0[0] = err1;
			err0[1] = err2;

			x1 += diff[0];
		}
		else if (i == 1)
		{
			// Calculate first partials
			Jacobian[0][0] = (err1 - err0[0]) / diff[0];
			Jacobian[1][0] = (err2 - err0[1]) / diff[0];

			x1 -= diff[0];
			x2 += diff[1];
		}
		else
		{
			if (i == 2)
			{
				// Second set of partials
				Jacobian[0][1] = (err1 - err0[0]) / diff[1];
				Jacobian[1][1] = (err2 - err0[1]) / diff[1];

				// Calculate inverse of Jacobian
				double det = Jacobian[0][0] * Jacobian[1][1] - Jacobian[0][1] * Jacobian[1][0];
				Jinv[0][0] = Jacobian[1][1] / det;
				Jinv[0][1] = -Jacobian[0][1] / det;
				Jinv[1][0] = -Jacobian[1][0] / det;
				Jinv[1][1] = Jacobian[0][0] / det;
			}

			// Inverse of Jacobian times errors
			Inc[0] = Jinv[0][0] * err1 + Jinv[0][1] * err2;
			Inc[1] = Jinv[1][0] * err1 + Jinv[1][1] * err2;

			// Change to x1 and x2
			x1 -= Inc[0];
			x2 -= Inc[1];
		}
	}
}