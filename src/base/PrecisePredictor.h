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

#include "../util/OrbMech.h"

struct PrecisePredictorInputs
{
	PrecisePredictorInputs()
	{
		TMIN = 0.0;
		TMAX = 10.0 * 24.0 * 3600.0;
		DT = 2.0;
		ISTOPS = 0;
		STOPVA = 0.0;
		HMULT = 1.0;
		KFactor = 1.0;
		GMD = 4;
		GMO = 4;
	}

	// Initial state vector
	OrbMech::StateVector SV_INIT;
	// Minimum time of integration (ignored if ISTOPS = 0)
	double TMIN;
	// Maximum time of integration (used for ISTOPS = 0)
	double TMAX;
	// Integration step size
	double DT;
	// Integration stop indicator. 0 = Time, 1 = radius, 2 = altitude, 3 = flightpath angle, 4 = argument of latitude, 5 = longitude, 6 = geocentric latitude
	int ISTOPS;
	// Stopping condition (other than ISTOPS = 0)
	double STOPVA;
	// Direction control
	double HMULT;
	// Drag multiplier
	double KFactor;
	// Gravity model
	int GMD;
	int GMO;
};

struct PrecisePredictorOutputs
{
	// Output state vector
	OrbMech::StateVector SV_FINAL;
	// Actual stopping condition
	int ISTOPS;
	int Error;
};

class PrecisePredictor
{
public:
	PrecisePredictor(OrbMech::GlobalConstants globconst);

	void RUN(const PrecisePredictorInputs& in, PrecisePredictorOutputs& out);
private:
	void Init(const PrecisePredictorInputs &in);
	void EffectiveForcesRoutine();
	void IntegrationTerminationControlRoutine();
	void Step();

	VECTOR3 ACCEL(int GMD, int GMO, VECTOR3 R, VECTOR3 V, double T) const;
	void CENTRAL(VECTOR3 R, VECTOR3& ACCEL, double& R_INV) const;
	MATRIX3 EARTH_FIXED_TO_TEG_COORD(double TIME) const;
	void ACCEL_EARTH_GRAV(VECTOR3 UR, double R_INV, int GMO, int GMD, const MATRIX3& FIFTY, VECTOR3& G_VEC) const;
	double DENSITY(VECTOR3 R) const;
	VECTOR3 ACCEL_DRAG(VECTOR3 R, VECTOR3 V, double RHO) const;

	void StoreVariables();
	void RestoreVariables();

	// INPUTS
	// Initial time
	double T0;
	// Integration step size
	double DT_NOM;
	double HMULT;
	int GMD, GMO;
	// Minimum time of integration (ignored if ISTOPS = 0)
	double TMIN;
	// Maximum time of integration (used for ISTOPS = 0)
	double TMAX;
	// Stopping condition (other than ISTOPS = 0)
	double STOPVA;

	// INTERNAL
	// Current state
	VECTOR3 R, V;
	// Current acceleration
	VECTOR3 Accel;
	// Time since start
	double T;
	// Drag multiplier
	double CSA;
	// Actual integration step length
	double DT;
	// STEP utility variables
	double HP, HD2, H2D2, H2D8, HD6;

	double RES1, RCALC, VAR;

	// Saved state
	VECTOR3 RS, VS;
	double TS;

	//Bounding variable. 0 = first pass, -1 = not bounded, 1 = bounded
	int INITE;
	// Integration stop indicator. 0 = Time, 1 = radius, 2 = altitude, 3 = flightpath angle, 4 = argument of latitude, 5 = longitude, 6 = geocentric latitude
	int ISTOPS;
	// Ending flag
	bool IEND;

	// CONSTANTS
	const double EARTH_RADIUS_GRAV = 6378140.0;
	const double GRAV_ZONAL[4] = { 0.0, 1.0826271e-3,-2.5358868e-6, -1.6246180e-6 };

	const double GRAV_C[9] = { -2.7635957e-10, 1.5711423e-6, 2.1907694e-6, 3.0466825e-7, 9.7966803e-8, -5.0552749e-7,  7.8842515e-8, 5.9073749e-8, -4.1542493e-9 };
	const double GRAV_S[9] = { -5.2357454e-9, -9.0231337e-7, 2.7267074e-7, -2.1259298e-7, 1.9681077e-7, -4.4125015e-7,  1.4818958e-7, -1.2140873e-8, 6.3163541e-9 };

	OrbMech::GlobalConstants globcnst;
};