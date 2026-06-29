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

#include "PrecisePredictor.h"
#include <algorithm>    // std::max

PrecisePredictor::PrecisePredictor(OrbMech::GlobalConstants globconst) : globcnst(globconst)
{
	R = V = Accel = _V(0, 0, 0);
	T = T0 = CSA = HMULT = DT = TMIN = TMAX = DT_NOM = STOPVA = 0.0;
	ISTOPS = INITE = 0;
	IEND = false;
	GMD = GMO = 0;
	HP = HD2 = H2D2 = H2D8 = HD6 = 0.0;
	RS = VS = _V(0, 0, 0);
	RCALC = RES1 = TS = VAR = 0.0;
}

void PrecisePredictor::RUN(const PrecisePredictorInputs& in, PrecisePredictorOutputs& out)
{
	Init(in);

	// Initialization pass in forcing function and editor
	EffectiveForcesRoutine();
	IntegrationTerminationControlRoutine();

	while (IEND == false)
	{
		Step();
		IntegrationTerminationControlRoutine();
	}

	out.SV_FINAL.R = R;
	out.SV_FINAL.V = V;
	out.SV_FINAL.GMT = T0 + T;
	out.SV_FINAL.CD = in.SV_INIT.CD;
	out.SV_FINAL.Area = in.SV_INIT.Area;
	out.SV_FINAL.Weight = in.SV_INIT.Weight;
	out.Error = 0;
	out.ISTOPS = ISTOPS;
}

void PrecisePredictor::Init(const PrecisePredictorInputs& in)
{
	R = in.SV_INIT.R;
	V = in.SV_INIT.V;
	T0 = in.SV_INIT.GMT;
	CSA = -0.5 * in.SV_INIT.CD * in.KFactor * in.SV_INIT.Area / in.SV_INIT.Weight;
	DT_NOM = DT = in.DT * in.HMULT;
	GMD = in.GMD;
	GMO = in.GMO;
	ISTOPS = in.ISTOPS;
	HMULT = in.HMULT;
	TMIN = in.TMIN;
	TMAX = in.TMAX;
	STOPVA = in.STOPVA;

	T = 0.0;
	INITE = 0;
	IEND = false;
}

void PrecisePredictor::EffectiveForcesRoutine()
{
	Accel = ACCEL(GMD, GMO, R, V, T0 + T);
}

void PrecisePredictor::IntegrationTerminationControlRoutine()
{
	double TIME, FUNCT;

	TIME = abs(T);

	// Termination control
	if (TMIN > TIME)
	{
		// Minimum time not passed
		IEND = false;
		return;
	}

	// Minimum time passed
	if (ISTOPS == 0)
	{
		RCALC = TIME - TMAX;
	}
	else
	{
		if (ISTOPS == 1)
		{
			// Radius
			FUNCT = length(R);
		}
		else if (ISTOPS == 2)
		{
			// Altitude
			FUNCT = OrbMech::H_ELLIPSOID(globcnst, R);
		}
		else if (ISTOPS == 3)
		{
			// Flight path angle
			FUNCT = asin(dotp(unit(R), unit(V)));
		}
		else if (ISTOPS == 4)
		{
			// Argument of latitude
			FUNCT = OrbMech::ArgLat(R, V);
		}
		else
		{
			//TBD
			FUNCT = 0.0;
		}

		RCALC = FUNCT - STOPVA;

		if (ISTOPS == 4)
		{
			//modulo between -PI and PI
			if (RCALC > 0)
			{
				RCALC = fmod(RCALC + MathConstants::PI, MathConstants::PI2) - MathConstants::PI;
			}
			else
			{
				RCALC = fmod(RCALC - MathConstants::PI, MathConstants::PI2) + MathConstants::PI;
			}
		}
	}

	// End control
	if (abs(RCALC) < 1e-9 || abs(DT) < 1e-8)
	{
		IEND = true;
		return;
	}

	if (INITE == 0)
	{
		// First Pass
		INITE = -1;
	}
	else if (INITE == -1)
	{
		// Not bounded
		if (RCALC * RES1 < 0.0)
		{
			//Found it. Go back to previous step
			RestoreVariables();
			VAR = DT;
			DT = (VAR * RES1) / (RES1 - RCALC);
			INITE = 1;
			return;
		}
	}
	else
	{
		// Bounded
		RestoreVariables();
		VAR = DT;
		DT = (VAR * RES1) / (RES1 - RCALC);

		// If the change in step length is too small, exit
		if (abs(VAR - DT) < 1e-8)
		{
			IEND = true;
		}
		return;
	}

	// TMAX check
	if (TIME > TMAX)
	{
		// Now try to find TMAX
		ISTOPS = 0;
		RestoreVariables();
		DT = HMULT * abs(TMAX - T);
		return;
	}
	StoreVariables();
}

void PrecisePredictor::Step()
{
	VECTOR3 alpha; //YS
	VECTOR3 beta; //YPS
	VECTOR3 F1, F2, F3;

	//Start
	if (DT - HP != 0.0)
	{
		HD2 = DT / 2.0;
		H2D2 = HD2 * DT;
		H2D8 = H2D2 / 4.0;
		HD6 = DT / 6.0;
		HP = DT;
	}

	//Save base and build state for 2nd derivative (2nd term)
	F1 = Accel;
	alpha = R;
	beta = V;
	R = alpha + beta * HD2 + F1 * H2D8;
	V = beta + F1 * HD2;
	T = T + HD2;
	//Get 2nd derivative (F2)
	EffectiveForcesRoutine();
	//Save F2 and build state for 2nd deriv evaluation F3
	F2 = Accel;
	V = beta + F2 * HD2;
	EffectiveForcesRoutine();
	F3 = Accel;
	R = alpha + beta * DT + F3 * H2D2;
	V = beta + F3 * DT;
	T = T + HD2;
	//Get 2nd deriv F4
	EffectiveForcesRoutine();
	//Weighted sum for state at tau + dt
	R = alpha + (beta + (F1 + F2 + F3) * HD6) * DT;
	V = beta + (F1 + (F2 + F3) * 2.0 + Accel) * HD6;
	//Final acceleration
	EffectiveForcesRoutine();
}

VECTOR3 PrecisePredictor::ACCEL(int GMD, int GMO, VECTOR3 R, VECTOR3 V, double T) const
{
	VECTOR3 ACCEL;
	double R_INV;

	CENTRAL(R, ACCEL, R_INV);

	if (GMD != 0)
	{
		MATRIX3 FIFTY;
		VECTOR3 G, D, R_EF, UR;

		G = D = _V(0, 0, 0);
		FIFTY = EARTH_FIXED_TO_TEG_COORD(T);
		R_EF = tmul(FIFTY, R);
		UR = R_EF * R_INV;

		ACCEL_EARTH_GRAV(UR, R_INV, GMO, GMD, FIFTY, G);
		if (GMD == 4)
		{
			double RHO;

			RHO = DENSITY(R);
			D = ACCEL_DRAG(R, V, RHO);
		}

		ACCEL = ACCEL + G + D;
	}

	return ACCEL;
}

void PrecisePredictor::CENTRAL(VECTOR3 R, VECTOR3& ACCEL, double& R_INV) const
{
	R_INV = 1.0 / length(R);
	ACCEL = -R * globcnst.EARTH_MU * pow(R_INV, 3);
}

MATRIX3 PrecisePredictor::EARTH_FIXED_TO_TEG_COORD(double TIME) const
{
	double LAM, CL, SL;

	LAM = globcnst.EARTH_RATE * TIME;
	CL = cos(LAM);
	SL = sin(LAM);

	return _M(CL, -SL, 0.0, SL, CL, 0.0, 0.0, 0.0, 1.0);
}

void PrecisePredictor::ACCEL_EARTH_GRAV(VECTOR3 UR, double R_INV, int GMO, int GMD, const MATRIX3& FIFTY, VECTOR3& G_VEC) const
{
	double MAT_A[5][2];
	double ZETA_REAL[5], ZETA_IMAG[5];
	double R0_ZERO, R0_N, AUXILIARY, F1, F2, F3, F4, DNM;
	int I, N, J, N1, L;

	//The following variables are to be set up to serve as starting	values for recursive relations used in the Pines formulation
	//A is a two-column array used for temporary storage of Legendre polynomials and derived Legendre functions (which are latitude-dependent terms), and
	//R0_Nis the distance-related term. AUXILIARY is an intermediate scalar variable.
	R0_ZERO = EARTH_RADIUS_GRAV * R_INV;
	R0_N = R0_ZERO * globcnst.EARTH_MU * R_INV * R_INV;
	MAT_A[0][1] = 3.0 * UR.z;
	MAT_A[1][1] = 3.0;
	ZETA_REAL[0] = 1.0;
	ZETA_IMAG[0] = 0.0;
	L = 1;
	AUXILIARY = 0.0;

	//Effects of tesseral harmonics, terms that depend on the vehicle's longitude
	for (I = 1; I <= GMO; I++)
	{
		ZETA_REAL[I] = UR.x * ZETA_REAL[I - 1] - UR.y * ZETA_IMAG[I - 1];
		ZETA_IMAG[I] = UR.x * ZETA_IMAG[I - 1] + UR.y * ZETA_REAL[I - 1];
	}
	for (N = 2; N <= GMD; N++)
	{
		//Derived Legendre functions by means of recursion formulas, multiplied by appropiate combinations of tesseral harmonics (Legendre polynomials shall be multiplied by
		//zonal harmonics coefficients), and stored as certain auxiliary variables F1-F4.
		MAT_A[N][0] = 0.0;
		MAT_A[N][1] = (2.0 * (double)N + 1.0) * MAT_A[N - 1][1];
		MAT_A[N - 1][0] = MAT_A[N - 1][1];
		MAT_A[N - 1][1] = UR.z * MAT_A[N][1];
		for (J = 2; J <= N; J++)
		{
			MAT_A[N - J][0] = MAT_A[N - J][1];
			MAT_A[N - J][1] = (UR.z * MAT_A[N - J + 1][1] - MAT_A[N - J + 1][0]) / ((double)J);
		}
		F1 = 0.0;
		F2 = 0.0;
		F3 = -MAT_A[0][0] * GRAV_ZONAL[N - 1];
		F4 = -MAT_A[0][1] * GRAV_ZONAL[N - 1];
		//If the maximum order of tesserals wanted has not been attained, do for N1=1 to N (these take into account contributions of tesseral and sectorial harmonics):
		if (N <= GMO)
		{
			for (N1 = 1; N1 <= N; N1++)
			{
				F1 = F1 + (double)N1 * MAT_A[N1 - 1][0] * (GRAV_C[L - 1] * ZETA_REAL[N1 - 1] + GRAV_S[L - 1] * ZETA_IMAG[N1 - 1]);
				F2 = F2 + (double)N1 * MAT_A[N1 - 1][0] * (GRAV_S[L - 1] * ZETA_REAL[N1 - 1] - GRAV_C[L - 1] * ZETA_IMAG[N1 - 1]);
				DNM = GRAV_C[L - 1] * ZETA_REAL[N1] + GRAV_S[L - 1] * ZETA_IMAG[N1];
				F3 = F3 + DNM * MAT_A[N1][0];
				F4 = F4 + DNM * MAT_A[N1][1];
				L++;
			}
		}
		//Multiply the sum of zonal and tesseral effects by appropiate distance-related factors, store the results as components of the acceleration vector G_VEC, and prepare for 
		//final computation by obtaining the intermediate scalar variable AUXILIARY, which accounts for an additional effect proportional to the unit radius vector UR.
		R0_N = R0_N * R0_ZERO;
		G_VEC.x = G_VEC.x + R0_N * F1;
		G_VEC.y = G_VEC.y + R0_N * F2;
		G_VEC.z = G_VEC.z + R0_N * F3;
		AUXILIARY = AUXILIARY + R0_N * F4;
	}
	//Lastly, the planet fixed acceleration vector shall be obtained and rotated to ecliptic coordinates
	G_VEC = G_VEC - UR * AUXILIARY;
	G_VEC = mul(FIFTY, G_VEC);
}

double PrecisePredictor::DENSITY(VECTOR3 R) const
{
	double ALT, DEN, CS;

	ALT = OrbMech::H_ELLIPSOID(globcnst, R);

	OrbMech::GLFDEN(ALT, DEN, CS);

	return DEN;
}

VECTOR3 PrecisePredictor::ACCEL_DRAG(VECTOR3 R, VECTOR3 V, double RHO) const
{
	VECTOR3 V_R;

	V_R = OrbMech::V_REL(globcnst, R, V);

	return V_R * CSA * RHO * length(V_R);
}

void PrecisePredictor::StoreVariables()
{
	RS = R;
	VS = V;
	TS = T;
	RES1 = RCALC;
}

void PrecisePredictor::RestoreVariables()
{
	R = RS;
	V = VS;
	T = TS;
	EffectiveForcesRoutine();
}