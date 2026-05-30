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

#include <vector>
#include <string>
#include "../util/OrbMech.h"

typedef union
{
	int iVal;
	double dVal;
} ADSArray;

struct OMSTargetSet
{
	double DTIG;
	double C1;
	double C2;
	double HTGT;
	double THETA;
};

class AscentDesignSystem
{
public:
	AscentDesignSystem();

	void PerformAscentSimulation(const std::vector<ADSArray>& arr, std::vector<std::string> &out);
	void Export(const std::vector<ADSArray>& arr, std::vector<std::string>& out);
	int ConvergeHA(const std::vector<ADSArray>& arr, double HA_desired_NM, std::vector<std::string>& out);
	int ConvergeHP(const std::vector<ADSArray>& arr, double HP_desired_NM, std::vector<std::string>& out);
private:

	struct SimInputs
	{
		double Incl;			// MECO inclination, radians
		double Lat;				// Geocentric launchpad latitude, radians
		double Lng;				// Geographic launchpad longitude, radians
		bool north;				// flag for launch direction. true = northerly, false = southerly
		double dt_bias;			// Launch time bias for orbital plane, seconds
		double PFA;				// Powered flight arc, radians
		double PFT;				// Powered flight time, seconds
		double RINS;			// Radius of insertion, meters
		double VINS;			// Velocity of insertion, meters/second
		double GAMINS;			// Flight path angle of insertion, radians
		double Chaser_CD;		// Chaser coefficient of drag, nd
		double Chaser_Area;		// Chaser area, meters^2
		double Chaser_Weight;	// Chaser insertion weight, kg
		double ET_CD;			// External tank coefficient of drag, nd
		double ET_Area;			// External tank area, meters^2
		double ET_Weight;		// External tank insertion weight, kg

		double ET_Sep_DTIG;		// Time from MECO to ET sep, seconds
		VECTOR3 ET_Sep_DV;		// ET Sep Delta V, meters/second

		bool direct;			// Direct insertion flag. true = direct, false = standard

		double MPS_Dump_DTIG;	// Time from MECO to MPS dump, seconds
		VECTOR3 MPS_Dump_DV;	// MPS Dump Delta V, meters/second

		OMSTargetSet OMS1;		// OMS-1 PEG-4 targets
		OMSTargetSet OMS2;		// OMS-2 PEG-4 targets
	};

	struct SimOutputs
	{
		VECTOR3 RLS_TEG;
		OrbMech::StateVector sv_MECO;
		OrbMech::StateVector sv_ET_sep_before;
		OrbMech::StateVector sv_ET_sep_after;
		OrbMech::StateVector sv_MPS_dump_before;
		OrbMech::StateVector sv_MPS_dump_after;
		OrbMech::StateVector sv_OMS1_before;
		OrbMech::StateVector sv_OMS1_after;
		OrbMech::StateVector sv_OMS2_before;
		OrbMech::StateVector sv_OMS2_after;
		OrbMech::StateVector sv_OMS2_perigee; // 180 degrees of orbital travel after OMS-2 cutoff
		double TGO_OMS1, TGO_OMS2;
		VECTOR3 VGO_OMS1, VGO_OMS2;
		OrbMech::StateVector sv_ET_impact;
		bool ETDidImpact;
	};

	// MAIN
	int SimulateAscent(const SimInputs &in, SimOutputs &out) const;

	// FUNCTIONS TO CONVERGE PEG4 MANEUVERS
	// OMS-1 Standard Insertion iteration for desired apogee height
	int ConvergeStandardInsertionOMS1(SimInputs& in, double HA_desired, SimOutputs& out, double& HTGT, double& THETA) const;
	// For direct insertion, vary the insertion velocity to achieve a desired apogee altitude
	int DIIterateOnInsertionVelocity(SimInputs& in, double HA_desired, SimOutputs& out) const;
	// Given a state vector at perigee, impulsively adjust the altitude 180 degree later
	int ImpulsiveOMS2(const OrbMech::StateVector sv_apogee, double HP, double& dv_OMS2) const;
	// Calculate the actual TIG from an impulsive TIG
	double EXDV(double T, double DV, double W0, double W_dot, double V_e) const;
	// Given desired LVLH VGX and VGZ components for a burn, converge the required HTGT and THETA to achieve the same burn
	int ConvergePEG4Burn(const OrbMech::StateVector& sv_TIG, VECTOR3 RLS_TEG, double HTGT_guess, double THETA_guess, double VGX_LVLH, double VGZ_LVLH, double& HTGT, double& THETA, bool &bCircular) const;

	// UTILITY
	SimInputs ConvertInputs(const std::vector<ADSArray>& arr) const;
	void WriteOutputs(const SimInputs& simin, SimOutputs& simout, std::vector<std::string>& out);
	void ORB_ALT_TIME_TSK(VECTOR3 R, VECTOR3 V, double& HA, double& HP, double& TT_X, int& TXX_FLAG) const;
	double InertialtoEarthFixedLongitude(double lng_iner, double t) const;
	double EarthFixedToInertialLongitude(double lng, double t) const;

	int coast(OrbMech::StateVector sv_in, double dt, OrbMech::StateVector& sv_out) const;
	int FindApogee(OrbMech::StateVector sv1, OrbMech::StateVector& sv_a) const;

	// OUTPUT
	void PrintStateVector(OrbMech::StateVector sv, std::vector<std::string>& out);
	void PrintBurnData(OrbMech::StateVector sv, VECTOR3 VGO, double TGO, std::vector<std::string>& out);

	std::string strtemp;
	char Buffer[256];

	OrbMech::GlobalConstants globcnst;
};