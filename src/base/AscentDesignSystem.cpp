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

#include "AscentDesignSystem.h"
#include "../util/OrbMech.h"
#include "PrecisePredictor.h"
#include "PEG4A.h"
#include <cmath>

AscentDesignSystem::AscentDesignSystem()
{
	sprintf_s(Buffer, "");
}

void AscentDesignSystem::PerformAscentSimulation(const std::vector<ADSArray>& arr, std::vector<std::string> &out)
{
	SimInputs simin;
	SimOutputs simout;

	simin = ConvertInputs(arr);

	SimulateAscent(simin, simout);

	// Output
	WriteOutputs(simin, simout, out);
}

void AscentDesignSystem::Export(const std::vector<ADSArray>& arr, std::vector<std::string>& out)
{
	SimInputs simin;

	simin = ConvertInputs(arr);

	out.clear();

	out.push_back("Launch Window/Targeting Processor Inputs:");

	/*
	char Buf1[128], Buf2[128];
	const double DT_ET_SEP = 6.0; // TBD

	OrbMech::GMT2String_MMSS(Buf1, simin.dt_bias);
	sprintf_s(Buffer, "K11,-05:00,05:00,%s,%c$", Buf1, simin.north ? 'N' : 'S');
	out.push_back(Buffer);

	OrbMech::GMT2String_MMSS(Buf1, simin.ET_Sep_DTIG);
	sprintf_s(Buffer, "K12,%.1lf,39B,%.3lf,%.3lf,%.1lf,%.1lf,%.1lf,%s$",
		simin.Chaser_Weight / MathConstants::LBM_TO_KG, simin.Lat * MathConstants::DEG, simin.Lng * MathConstants::DEG,
		simin.ET_Sep_DV.x / MathConstants::FOOT_TO_M, simin.ET_Sep_DV.y / MathConstants::FOOT_TO_M, simin.ET_Sep_DV.z / MathConstants::FOOT_TO_M, Buf1);
	out.push_back(Buffer);

	OrbMech::GMT2String_MMSS(Buf1, simin.PFT);
	OrbMech::GMT2String_MMSS(Buf2, simin.MPS_Dump_DTIG);
	sprintf_s(Buffer, "K13,%.1lf,%.1lf,%.3lf,%.3lf,%s,14.000,0,14586.51,10138.04,100.5313,%s,YES,%.1lf,%.1lf,%.1lf,%s$",
		simin.RINS / MathConstants::FOOT_TO_M, simin.VINS / MathConstants::FOOT_TO_M, simin.GAMINS * MathConstants::DEG,
		simin.PFA * MathConstants::DEG, Buf1, simin.direct ? "DI" : "SI",
		simin.MPS_Dump_DV.x / MathConstants::FOOT_TO_M, simin.MPS_Dump_DV.y / MathConstants::FOOT_TO_M, simin.MPS_Dump_DV.z / MathConstants::FOOT_TO_M, Buf2);
	out.push_back(Buffer);

	OrbMech::GMT2String_MMSS(Buf1, simin.OMS1.DTIG - DT_ET_SEP);
	sprintf_s(Buffer, "K17,1,1,%.2lf,%.2lf,%.1lf,%.1lf,%s$",
		simin.OMS1.HTGT / MathConstants::NAUTICAL_MILE_TO_M, simin.OMS1.THETA * MathConstants::DEG, simin.OMS1.C1 * MathConstants::FOOT_TO_M, simin.OMS1.C2, Buf1);
	out.push_back(Buffer);

	OrbMech::GMT2String_MMSS(Buf1, simin.OMS2.DTIG - DT_ET_SEP);
	sprintf_s(Buffer, "K17,1,2,%.2lf,%.2lf,%.1lf,%.1lf,%s$",
		simin.OMS2.HTGT / MathConstants::NAUTICAL_MILE_TO_M, simin.OMS2.THETA * MathConstants::DEG, simin.OMS2.C1 * MathConstants::FOOT_TO_M, simin.OMS2.C2, Buf1);
	out.push_back(Buffer);

	*/

	out.push_back("");
	sprintf_s(Buffer, "PFA %.1lf", simin.PFA * MathConstants::DEG);
	out.push_back(Buffer);

	OrbMech::GMT2String_HHMMSSCS(Buffer, simin.PFT);
	strtemp.assign(Buffer);
	out.push_back("PFT " + strtemp);

	sprintf_s(Buffer, "RAD %.1lf", simin.RINS / MathConstants::FOOT_TO_M);
	out.push_back(Buffer);

	sprintf_s(Buffer, "VEL %.1lf", simin.VINS / MathConstants::FOOT_TO_M);
	out.push_back(Buffer);

	sprintf_s(Buffer, "FPA %.6lf", simin.GAMINS * MathConstants::DEG);
	out.push_back(Buffer);

	out.push_back("");
	out.push_back("OMS-1");
	OrbMech::GMT2String_HHMMSSCS(Buffer, simin.OMS1.DTIG);
	strtemp.assign(Buffer);
	out.push_back("DTIG " + strtemp);
	sprintf_s(Buffer, "C1 %.1lf", simin.OMS1.C1 / MathConstants::FOOT_TO_M);
	out.push_back(Buffer);
	sprintf_s(Buffer, "C2 %.1lf", simin.OMS1.C2);
	out.push_back(Buffer);
	sprintf_s(Buffer, "HT %.1lf", simin.OMS1.HTGT / MathConstants::NAUTICAL_MILE_TO_M);
	out.push_back(Buffer);
	sprintf_s(Buffer, "THETA-T %.1lf", simin.OMS1.THETA * MathConstants::DEG);
	out.push_back(Buffer);

	out.push_back("");
	out.push_back("OMS-2");
	OrbMech::GMT2String_HHMMSSCS(Buffer, simin.OMS2.DTIG);
	strtemp.assign(Buffer);
	out.push_back("DTIG " + strtemp);
	sprintf_s(Buffer, "C1 %.1lf", simin.OMS2.C1 / MathConstants::FOOT_TO_M);
	out.push_back(Buffer);
	sprintf_s(Buffer, "C2 %.1lf", simin.OMS2.C2);
	out.push_back(Buffer);
	sprintf_s(Buffer, "HT %.1lf", simin.OMS2.HTGT / MathConstants::NAUTICAL_MILE_TO_M);
	out.push_back(Buffer);
	sprintf_s(Buffer, "THETA-T %.1lf", simin.OMS2.THETA * MathConstants::DEG);
	out.push_back(Buffer);

	out.push_back("");
	out.push_back("I-Loads:");

	sprintf_s(Buffer, "%.1lf %.1lf", simin.OMS1.DTIG - 18.0, simin.OMS2.DTIG - 18.0);
	strtemp.assign(Buffer);
	out.push_back("DTIG_OMS " + strtemp);

	sprintf_s(Buffer, "%.1lf %.1lf", simin.OMS1.HTGT / MathConstants::FOOT_TO_M, simin.OMS2.HTGT / MathConstants::FOOT_TO_M);
	strtemp.assign(Buffer);
	out.push_back("HTGT_OMS " + strtemp);

	sprintf_s(Buffer, "%.6lf %.6lf", simin.OMS1.THETA, simin.OMS2.THETA);
	strtemp.assign(Buffer);
	out.push_back("THETA_OMS " + strtemp);

	sprintf_s(Buffer, "%.1lf %.1lf", simin.OMS1.C1 / MathConstants::FOOT_TO_M, simin.OMS2.C1 / MathConstants::FOOT_TO_M);
	strtemp.assign(Buffer);
	out.push_back("C1_OMS " + strtemp);

	sprintf_s(Buffer, "%.1lf %.1lf", simin.OMS1.C2, simin.OMS2.C2);
	strtemp.assign(Buffer);
	out.push_back("C2_OMS " + strtemp);

	out.push_back("");
	out.push_back("SSV Legacy MECO:");
	sprintf_s(Buffer, "%.2lf", simin.Incl * MathConstants::DEG);
	strtemp.assign(Buffer);
	out.push_back("Inclination (deg) " + strtemp);
	sprintf_s(Buffer, "%.0lf", simin.RINS - 6.37101e6);
	strtemp.assign(Buffer);
	out.push_back("Altitude (m) " + strtemp);
	sprintf_s(Buffer, "%.5lf", simin.VINS);
	strtemp.assign(Buffer);
	out.push_back("Velocity (m/s) " + strtemp);
	sprintf_s(Buffer, "%.5lf", simin.GAMINS * MathConstants::DEG);
	strtemp.assign(Buffer);
	out.push_back("Flight Path Angle (deg) " + strtemp);

	out.push_back("Earth-Fixed IY:");
	VECTOR3 IY = OrbMech::CalculateEFIYVector(simin.Incl, simin.Lat, simin.Lng, simin.north, simin.dt_bias);
	sprintf_s(Buffer, "%.8lf %.8lf %.8lf", IY.x, IY.y, IY.z);
	strtemp.assign(Buffer);
	out.push_back("IY Vector " + strtemp);
}

int AscentDesignSystem::ConvergeHA(const std::vector<ADSArray>& arr, double HA_desired_NM, std::vector<std::string>& out)
{
	SimInputs simin;
	SimOutputs simout;
	double HA_desired;
	int Error;

	simin = ConvertInputs(arr);

	HA_desired = HA_desired_NM * MathConstants::NAUTICAL_MILE_TO_M;

	if (simin.direct)
	{
		Error = DIIterateOnInsertionVelocity(simin, HA_desired, simout);
		if (Error) return 1;

		// Output
		WriteOutputs(simin, simout, out);

		out.push_back("");
		out.push_back("Required insertion velocity:");

		sprintf_s(Buffer, "%.1lf", simin.VINS / 0.3048);
		out.push_back(Buffer);
	}
	else
	{
		double HTGT, THETA;

		Error = ConvergeStandardInsertionOMS1(simin, HA_desired, simout, HTGT, THETA);
		if (Error) return 1;

		// Simulate again with new targets
		simin.OMS1.HTGT = HTGT;
		simin.OMS1.THETA = THETA;

		Error = SimulateAscent(simin, simout);
		if (Error) return Error;

		// Output
		WriteOutputs(simin, simout, out);

		out.push_back("");
		out.push_back("Required OMS-1 targets:");
		out.push_back("C1: 0.0");
		out.push_back("C2: 0.0");
		sprintf_s(Buffer, "HT: %.1lf", HTGT / MathConstants::NAUTICAL_MILE_TO_M);
		out.push_back(Buffer);
		sprintf_s(Buffer, "THETA-T: %.1lf", THETA * MathConstants::DEG);
		out.push_back(Buffer);
	}

	return 0;
}

int AscentDesignSystem::ConvergeHP(const std::vector<ADSArray>& arr, double HP_desired_NM, std::vector<std::string>& out)
{
	SimInputs simin;
	SimOutputs simout;
	OrbMech::StateVector sv_a, sv_TIG;
	double HP_desired, dv_OMS2, TIG, HTGT, THETA, OMS2_DTIG;
	int Error;
	bool bCircular;

	HP_desired = HP_desired_NM * MathConstants::NAUTICAL_MILE_TO_M;

	simin = ConvertInputs(arr);

	Error = SimulateAscent(simin, simout);
	if (Error) return 1;

	// Propagate state vector to apogee
	Error = FindApogee(simout.sv_MPS_dump_after, sv_a);
	if (Error) return 1;

	// Then: 180 degrees of argument of latitude travel later is the target HP
	// Iterate on impulsive DV until HP is converged
	Error = ImpulsiveOMS2(sv_a, HP_desired, dv_OMS2);
	if (Error) return 1;
	VECTOR3 VGO = tmul(OrbMech::LVLH_Matrix(sv_a.R, sv_a.V), _V(dv_OMS2, 0.0, 0.0));

	// Calculate actual TIG
	TIG = EXDV(sv_a.GMT, dv_OMS2, simin.Chaser_Weight, globcnst.FT / globcnst.VEX, globcnst.VEX);
	coast(sv_a, TIG - sv_a.GMT, sv_TIG);

	// Generate PEG targets
	VECTOR3 VGO_LVLH = mul(OrbMech::LVLH_Matrix(sv_TIG.R, sv_TIG.V), VGO);
	Error = ConvergePEG4Burn(sv_TIG, simout.RLS_TEG, HP_desired, simin.OMS1.THETA + MathConstants::PI, VGO_LVLH.x, VGO_LVLH.z, HTGT, THETA, bCircular);
	if (Error) return Error;

	// Calculate OMS-2 DTIG
	OMS2_DTIG = sv_TIG.GMT - simout.sv_MECO.GMT;

	// Simulate ascent again with updated OMS-2 targets
	simin.OMS2.DTIG = OMS2_DTIG;
	simin.OMS2.HTGT = HTGT;
	simin.OMS2.THETA = THETA;

	Error = SimulateAscent(simin, simout);
	if (Error) return 1;

	// Output
	WriteOutputs(simin, simout, out);

	out.push_back("");
	out.push_back("Required OMS-2 targets:");
	OrbMech::GMT2String_DHHMMSSCS(Buffer, round(OMS2_DTIG)); // Round to next second
	strtemp = "DTIG: ";
	strtemp.append(Buffer);
	out.push_back(strtemp);
	out.push_back("C1: 0.0");
	out.push_back("C2: 0.0");
	sprintf_s(Buffer, "HT: %.1lf", HTGT / MathConstants::NAUTICAL_MILE_TO_M);
	out.push_back(Buffer);
	sprintf_s(Buffer, "THETA-T: %.1lf", THETA * MathConstants::DEG);
	out.push_back(Buffer);
	if (bCircular)
	{
		out.push_back("Warning: Orbit near circular, output THETA-T is initial guess value");
	}

	return 0;
}

int AscentDesignSystem::SimulateAscent(const SimInputs& in, SimOutputs& out) const
{
	PrecisePredictorInputs predin;
	PrecisePredictorOutputs predout;
	OrbMech::StateVector sv_MECO, sv_ET_sep_before, sv_ET_sep_after, sv_MPS_dump_before, sv_MPS_dump_after, sv_OMS2_before, sv_OMS1_before, sv_OMS1_after, sv_OMS2_after, sv_OMS2_perigee;
	VECTOR3 IY, RLS_TEG, VGO_OMS1, VGO_OMS2;
	double TGO_OMS1, TGO_OMS2;

	// Assume constant for now
	predin.GMD = 4;
	predin.GMO = 4;
	predin.DT = 2.0;
	predin.TMIN = 0.0;
	predin.HMULT = 1.0;
	predin.KFactor = 0.0; // Disable drag up to OMS-1/MPS dump for now because attitude and flow regime likely give a lot less drag

	// Calculate insertion orbital plane
	IY = OrbMech::CalculateEFIYVector(in.Incl, in.Lat, in.Lng, in.north, in.dt_bias);

	// Simulate launch
	OrbMech::ENSERT(in.Lat, in.Lng, in.PFA, in.PFT, in.RINS, in.VINS, in.GAMINS, 0.0, 10000000.0, globcnst.EARTH_RATE, IY, sv_MECO);

	//Store rest of inputs
	sv_MECO.CD = in.Chaser_CD;
	sv_MECO.Area = in.Chaser_Area;
	sv_MECO.Weight = in.Chaser_Weight + in.ET_Weight;

	// Values for PEG4
	RLS_TEG = OrbMech::GetURLS(0.0, in.Lat, in.Lng, globcnst.EARTH_RATE);

	PrecisePredictor pred(globcnst);
	PEG4A peg(globcnst);

	// MECO to ET sep
	predin.SV_INIT = sv_MECO;
	predin.TMAX = in.ET_Sep_DTIG;
	pred.RUN(predin, predout);
	sv_ET_sep_before = predout.SV_FINAL;

	// Apply ET sep DV
	sv_ET_sep_after = sv_ET_sep_before;
	sv_ET_sep_after.V += tmul(OrbMech::LVLH_Matrix(sv_ET_sep_before.R, sv_ET_sep_before.V), in.ET_Sep_DV);

	// Drop ET
	sv_ET_sep_after.Weight -= in.ET_Weight;

	// Direct or standard insertion?
	if (in.direct)
	{
		// ET tank sep to MPS Dump
		predin.SV_INIT = sv_ET_sep_after;
		predin.TMAX = (sv_MECO.GMT + in.MPS_Dump_DTIG) - sv_ET_sep_after.GMT;
		pred.RUN(predin, predout);
		sv_MPS_dump_before = predout.SV_FINAL;

		// ET tank sep to OMS-1
		predin.SV_INIT = sv_ET_sep_after;
		predin.TMAX = (sv_MECO.GMT + in.OMS1.DTIG) - sv_ET_sep_after.GMT;
		pred.RUN(predin, predout);
		sv_OMS1_before = predout.SV_FINAL;

		// Take drag into account from here on
		predin.KFactor = 1.0;

		// Apply MPS Dump
		sv_MPS_dump_after = sv_MPS_dump_before;
		sv_MPS_dump_after.V += tmul(OrbMech::LVLH_Matrix(sv_MPS_dump_before.R, sv_MPS_dump_before.V), in.MPS_Dump_DV);

		// MPS Dump to OMS-2
		predin.SV_INIT = sv_MPS_dump_after;
		predin.TMAX = (sv_MECO.GMT + in.OMS2.DTIG) - sv_MPS_dump_after.GMT;
		pred.RUN(predin, predout);
		sv_OMS2_before = predout.SV_FINAL;

		// Predict what OMS-1 would look like
		peg.OMSBurnPrediction(sv_OMS1_before, RLS_TEG, in.OMS1.C1, in.OMS1.C2, in.OMS1.HTGT, in.OMS1.THETA, globcnst.FT, globcnst.VEX);
		peg.GetOutputA(VGO_OMS1, TGO_OMS1, sv_OMS1_after);
	}
	else
	{
		// ET tank sep to OMS-1
		predin.SV_INIT = sv_ET_sep_after;
		predin.TMAX = (sv_MECO.GMT + in.OMS1.DTIG) - sv_ET_sep_after.GMT;
		pred.RUN(predin, predout);
		sv_OMS1_before = predout.SV_FINAL;

		// Take drag into account from here on
		predin.KFactor = 1.0;

		// OMS-1
		peg.OMSBurnPrediction(sv_OMS1_before, RLS_TEG, in.OMS1.C1, in.OMS1.C2, in.OMS1.HTGT, in.OMS1.THETA, globcnst.FT, globcnst.VEX);
		peg.GetOutputA(VGO_OMS1, TGO_OMS1, sv_OMS1_after);

		// OMS-1 to OMS-2
		predin.SV_INIT = sv_OMS1_after;
		predin.TMAX = (sv_MECO.GMT + in.OMS2.DTIG) - sv_OMS1_after.GMT;
		pred.RUN(predin, predout);
		sv_OMS2_before = predout.SV_FINAL;

		// Set MPS dump state vectors
		sv_MPS_dump_before = sv_OMS1_before;
		sv_MPS_dump_after = sv_OMS1_after;
	}

	// OMS-2
	peg.OMSBurnPrediction(sv_OMS2_before, RLS_TEG, in.OMS2.C1, in.OMS2.C2, in.OMS2.HTGT, in.OMS2.THETA, globcnst.FT, globcnst.VEX);
	peg.GetOutputA(VGO_OMS2, TGO_OMS2, sv_OMS2_after);

	// Propagate to 180 degrees from OMS-2
	double U_ign, U_cut, U_avg, U_D;

	U_ign = OrbMech::ArgLat(sv_OMS2_before.R, sv_OMS2_before.V);
	U_cut = OrbMech::ArgLat(sv_OMS2_after.R, sv_OMS2_after.V);
	if (abs(U_ign - U_cut) > MathConstants::PI)
	{
		// TBD
	}
	U_avg = (U_ign + U_cut) / 2.0;

	U_D = U_avg + MathConstants::PI;
	if (U_D >= MathConstants::PI2)
	{
		U_D -= MathConstants::PI2;
	}

	predin.SV_INIT = sv_OMS2_after;
	predin.TMIN = 10.0 * 60.0;
	predin.TMAX = 10.0 * 3600.0;
	predin.ISTOPS = 4;
	predin.STOPVA = U_D;

	pred.RUN(predin, predout);

	if (predout.Error || predout.ISTOPS != 4)
	{
		// Didn't find point
		return 1;
	}
	sv_OMS2_perigee = predout.SV_FINAL;

	// External Tank impact
	predin.SV_INIT.R = sv_MECO.R;
	predin.SV_INIT.V = sv_MECO.V;
	predin.SV_INIT.GMT = sv_MECO.GMT;
	predin.SV_INIT.CD = in.ET_CD;
	predin.SV_INIT.Area = in.ET_Area;
	predin.SV_INIT.Weight = in.ET_Weight;
	predin.TMIN = 0.0;
	predin.TMAX = 90.0 * 60.0;
	predin.DT = 2.0;
	predin.ISTOPS = 2; // Altitude
	predin.STOPVA = 0.0; // Impact
	predin.GMD = 4;
	predin.GMO = 4;
	predin.KFactor = 1.0;

	pred.RUN(predin, predout);

	if (predout.Error)
	{
		// Error
		return 1;
	}

	// Write outputs
	out.RLS_TEG = RLS_TEG;
	out.sv_MECO = sv_MECO;
	out.sv_ET_sep_before = sv_ET_sep_before;
	out.sv_ET_sep_after = sv_ET_sep_after;
	out.sv_MPS_dump_before = sv_MPS_dump_before;
	out.sv_MPS_dump_after = sv_MPS_dump_after;
	out.sv_OMS1_before = sv_OMS1_before;
	out.sv_OMS1_after = sv_OMS1_after;
	out.sv_OMS2_before = sv_OMS2_before;
	out.sv_OMS2_after = sv_OMS2_after;
	out.sv_OMS2_perigee = sv_OMS2_perigee;
	out.TGO_OMS1 = TGO_OMS1;
	out.TGO_OMS2 = TGO_OMS2;
	out.VGO_OMS1 = VGO_OMS1;
	out.VGO_OMS2 = VGO_OMS2;
	out.ETDidImpact = (predout.ISTOPS == 2);
	out.sv_ET_impact = predout.SV_FINAL;

	return 0;
}

int AscentDesignSystem::ConvergeStandardInsertionOMS1(SimInputs& in, double HA_desired, SimOutputs& out, double& HTGT, double& THETA) const
{
	// TIG is fixed. Burn constrained to be along velocity vector.
	// x1 = HTGT
	// x2 = THETA

	// f1 = apogee height
	// f2 = angle between VGO and preburn velocity vector

	OrbMech::StateVector sv_OMS1_before, sv_cut, sv_apogee;
	MATRIX3 M_TEG_LVLH;
	VECTOR3 VGO, VGO_LVLH;
	double TGO, fpa_ignition, fpa_OMS1, HA_actual, HA_err, fpa_err;
	int Error, i, imax;

	PEG4A peg4a(globcnst);
	OrbMech::NewtonsMethod2D newton(1.0 * MathConstants::NAUTICAL_MILE_TO_M, 1.0 * MathConstants::RAD);

	// Simulate to OMS-1
	Error = SimulateAscent(in, out);
	if (Error) return Error;

	// Initial state vector
	sv_OMS1_before = out.sv_OMS1_before;

	// Conic height target as initial guess
	HTGT = HA_desired;

	// Empirical first guess for theta
	THETA = 120.0 * MathConstants::RAD + (HA_desired - 100.0 * MathConstants::NAUTICAL_MILE_TO_M) * ((180.0 - 120.0) * MathConstants::RAD) / ((190.0 - 100.0) * MathConstants::NAUTICAL_MILE_TO_M);

	// Inertial/LVLH DV vector conversion
	M_TEG_LVLH = OrbMech::LVLH_Matrix(sv_OMS1_before.R, sv_OMS1_before.V);

	// Set up iteration
	i = 0;
	imax = 10;

	fpa_ignition = MathConstants::PI05 - acos(dotp(unit(sv_OMS1_before.R), unit(sv_OMS1_before.V)));

	do
	{
		// Simulate PEG-4
		Error = peg4a.OMSBurnPrediction(sv_OMS1_before, out.RLS_TEG, 0.0, 0.0, HTGT, THETA, globcnst.FT, globcnst.VEX);
		if (Error) return Error;
		peg4a.GetOutputA(VGO, TGO, sv_cut);

		// VGO in LVLH coordinates
		VGO_LVLH = mul(M_TEG_LVLH, VGO);

		// Calculate direction of OMS-1 burn
		fpa_OMS1 = atan2(-VGO_LVLH.z, VGO_LVLH.x);

		// Find apogee
		Error = FindApogee(sv_cut, sv_apogee);
		if (Error) return Error;
		// Calculate altitude
		HA_actual = length(sv_apogee.R) - globcnst.EARTH_RADIUS_EQUATOR;

		// Calculate errors
		HA_err = HA_actual - HA_desired;
		fpa_err = fpa_OMS1 - fpa_ignition;

		// Check if converged
		if (abs(HA_err) < 1.0 && abs(fpa_err) < 0.0001 * MathConstants::RAD)
		{
			return 0;
		}

		// Newton method
		newton.Step(i, HA_err, fpa_err, HTGT, THETA);
		
		i++;
	} while (i < imax);

	// 

	return 0;
}

int AscentDesignSystem::DIIterateOnInsertionVelocity(SimInputs& in, double HA_desired, SimOutputs& out) const
{
	// INPUTS:
	// HA_desired = desired apogee altitude, meters
	// OUTPUTS:
	// out = simulation output data
	// return value = error if other than 0

	OrbMech::StateVector sv_apogee;
	double DV_SLOPE, HA_actual, dh, dv;
	int i, imax, Error;

	DV_SLOPE = 3413.5; //Rule of thumb, 1 ft/s of DV raises orbit by 1 NM
	i = 0;
	imax = 20;

	do
	{
		SimulateAscent(in, out);

		// Find apogee after MPS dump
		Error = FindApogee(out.sv_MPS_dump_after, sv_apogee);
		if (Error) return 1;
		// Calculate altitude
		HA_actual = length(sv_apogee.R) - globcnst.EARTH_RADIUS_EQUATOR;
		// Calculate height difference
		dh = HA_desired - HA_actual;

		// Small enough?
		if (abs(dh) < 1.0)
		{
			// Success
			return 0;
		}

		//Calculate change in velocity
		dv = dh / DV_SLOPE;
		// Apply dv
		in.VINS += dv;

		// Iteration counter
		i++;
	} while (i < imax);

	return 1;
}

int AscentDesignSystem::ImpulsiveOMS2(const OrbMech::StateVector sv_apogee, double HP_DESIRED, double& dv_OMS2) const
{
	PrecisePredictorInputs predin;
	PrecisePredictorOutputs predout;
	OrbMech::StateVector sv_postburn, sv_OMS2_perigee;
	double U_apogee, U_D, dv, HP, dh, DV_SLOPE;
	int i, imax;

	PrecisePredictor pred(globcnst);

	U_apogee = OrbMech::ArgLat(sv_apogee.R, sv_apogee.V);

	U_D = U_apogee + MathConstants::PI;
	if (U_D >= MathConstants::PI2)
	{
		U_D -= MathConstants::PI2;
	}

	DV_SLOPE = 3413.5; //Rule of thumb, 1 ft/s of DV raises orbit by 1 NM
	i = 0;
	imax = 20;
	dv = 100.0 * MathConstants::FOOT_TO_M;
	sv_postburn = sv_apogee;

	do
	{
		// Apply DV
		sv_postburn.V = sv_apogee.V + tmul(OrbMech::LVLH_Matrix(sv_apogee.R, sv_apogee.V), _V(dv, 0.0, 0.0));

		predin.SV_INIT = sv_postburn;
		predin.TMIN = 10.0 * 60.0;
		predin.TMAX = 10.0 * 3600.0;
		predin.ISTOPS = 4;
		predin.STOPVA = U_D;

		pred.RUN(predin, predout);

		if (predout.Error || predout.ISTOPS != 4)
		{
			// Didn't find point
			return 1;
		}
		sv_OMS2_perigee = predout.SV_FINAL;
		HP = length(sv_OMS2_perigee.R) - globcnst.EARTH_RADIUS_EQUATOR;

		// Calculate altitude error
		dh = HP_DESIRED - HP;

		//Calculate change in velocity
		dv += dh / DV_SLOPE;

		i++;
	} while (abs(dh) > 1.0 && i < imax);

	if (i >= imax) return 1;

	dv_OMS2 = dv;
	return 0;
}

double AscentDesignSystem::EXDV(double T, double DV, double W0, double W_dot, double V_e) const
{
	double WF, T_BURN, T_C, TIG;

	// Compute the estimated burn time
	WF = W0 * exp(-DV / V_e);
	T_BURN = abs((WF - W0) / W_dot);

	// Compute the burn backup time
	T_C = -T_BURN / log(W0 / WF) + W0 / W_dot;

	// Compute the burn ignition time
	TIG = T - T_C;

	return TIG;
}

int AscentDesignSystem::ConvergePEG4Burn(const OrbMech::StateVector& sv_TIG, VECTOR3 RLS_TEG, double HTGT_guess, double THETA_guess, double VGX_LVLH, double VGZ_LVLH, double& HTGT, double& THETA, bool& bCircular) const
{
	// INPUTS:
	// sv_TIG: State vector at time of ignition
	// RLS_TEG: Launch site position vector in inertial coordinates
	// HTGT_guess: Estimated required height target
	// THETA_guess: Estimated required theta angle
	// VGX_LVLH: Require VGX component in LVLH coordinates
	// VGZ_LVLH: Require VGZ component in LVLH coordinates
	// OUTPUT:
	// HTGT: Converged height target
	// THETA: Converged thete angle
	// bCircular: If true, orbit is near circular and the output THETA was not changed from the THETA_guess initial guess
	// return value: error if non-zero

	OrbMech::StateVector sv_cut;
	MATRIX3 M_TEG_LVLH;
	VECTOR3 VGO, VGO_LVLH;
	double r_OMS2, r_TGT, TGO, d_vgx, d_vgz, dHTGT, dTHETA, dv_converged;
	int Error, i, imax;

	PEG4A peg4a(globcnst);

	i = 0;
	imax = 10;

	dHTGT = 1.0 * MathConstants::NAUTICAL_MILE_TO_M;
	dTHETA = 1.0 * MathConstants::RAD;
	dv_converged = 0.05 * MathConstants::FOOT_TO_M;

	OrbMech::NewtonsMethod2D newton(dHTGT, dTHETA);

	// Special logic needed for near circular target orbit
	r_OMS2 = length(sv_TIG.R);
	r_TGT = HTGT_guess + globcnst.EARTH_RADIUS_EQUATOR;
	if (abs(r_OMS2 - r_TGT) < 5.0 * 1852.0)
	{
		bCircular = true;
	}
	else
	{
		bCircular = false;
	}

	HTGT = HTGT_guess;
	THETA = THETA_guess;
	M_TEG_LVLH = OrbMech::LVLH_Matrix(sv_TIG.R, sv_TIG.V);

	do
	{
		// Simulate PEG-4
		Error = peg4a.OMSBurnPrediction(sv_TIG, RLS_TEG, 0.0, 0.0, HTGT, THETA, globcnst.FT, globcnst.VEX);
		if (Error) return Error;
		peg4a.GetOutputA(VGO, TGO, sv_cut);

		// Calculate VGO_LVLH at TIG
		VGO_LVLH = mul(M_TEG_LVLH, VGO);

		// Calculate errors
		d_vgx = VGX_LVLH - VGO_LVLH.x;
		d_vgz = VGZ_LVLH - VGO_LVLH.z;

		if (i > 2 && abs(d_vgx) < dv_converged && abs(d_vgz) < dv_converged)
		{
			return 0;
		}

		newton.Step(i, d_vgx, d_vgz, HTGT, THETA);

		// Near a circular orbit THETA does not have much of an influence on the DV vector, so keep it constant
		if (i >= 2 && bCircular)
		{
			THETA = THETA_guess;
		}
		i++;
	} while (i < imax);

	// No error for maximum iteration when orbit is circular
	if (bCircular == false && i >= imax) return 1;

	return 0;
}

AscentDesignSystem::SimInputs AscentDesignSystem::ConvertInputs(const std::vector<ADSArray>& arr) const
{
	SimInputs in;

	in.Incl = arr[0].dVal * MathConstants::RAD;
	in.Lat = arr[1].dVal * MathConstants::RAD;
	in.Lng = arr[2].dVal * MathConstants::RAD;
	in.north = arr[3].iVal == 0 ? true : false;
	in.dt_bias = arr[4].dVal;
	in.PFA = arr[5].dVal * MathConstants::RAD;
	in.PFT = arr[6].dVal;
	in.RINS = arr[7].dVal * MathConstants::NAUTICAL_MILE_TO_M + 6378166.0;
	in.VINS = arr[8].dVal * MathConstants::FOOT_TO_M;
	in.GAMINS = arr[9].dVal * MathConstants::RAD;
	in.ET_Sep_DV = _V(arr[10].dVal, arr[11].dVal, arr[12].dVal) * MathConstants::FOOT_TO_M;
	in.ET_Sep_DTIG = arr[13].dVal;

	in.Chaser_CD = arr[29].dVal;
	in.Chaser_Area = arr[30].dVal * pow(MathConstants::FOOT_TO_M, 2);
	in.Chaser_Weight = arr[31].dVal * MathConstants::LBM_TO_KG;
	in.ET_CD = arr[32].dVal;
	in.ET_Area = arr[33].dVal * pow(MathConstants::FOOT_TO_M, 2);
	in.ET_Weight = arr[34].dVal * MathConstants::LBM_TO_KG;
	in.direct = arr[14].iVal == 0 ? false : true;

	in.MPS_Dump_DV = _V(arr[15].dVal, arr[16].dVal, arr[17].dVal) * MathConstants::FOOT_TO_M;
	in.MPS_Dump_DTIG = arr[18].dVal;

	in.OMS1.DTIG = arr[19].dVal;
	in.OMS1.C1 = arr[20].dVal * MathConstants::FOOT_TO_M;
	in.OMS1.C2 = arr[21].dVal;
	in.OMS1.HTGT = arr[22].dVal * MathConstants::NAUTICAL_MILE_TO_M;
	in.OMS1.THETA = arr[23].dVal * MathConstants::RAD;
	in.OMS2.DTIG = arr[24].dVal;
	in.OMS2.C1 = arr[25].dVal * MathConstants::FOOT_TO_M;
	in.OMS2.C2 = arr[26].dVal;
	in.OMS2.HTGT = arr[27].dVal * MathConstants::NAUTICAL_MILE_TO_M;
	in.OMS2.THETA = arr[28].dVal * MathConstants::RAD;

	return in;
}

void AscentDesignSystem::WriteOutputs(const SimInputs& simin, SimOutputs& simout, std::vector<std::string>& out)
{
	out.clear();
	out.push_back("MECO");
	PrintStateVector(simout.sv_MECO, out);
	out.push_back("");
	out.push_back("ET Sep");
	PrintStateVector(simout.sv_ET_sep_after, out);
	out.push_back("");

	if (simin.direct)
	{
		out.push_back("OMS-1 (Not required)");
		PrintBurnData(simout.sv_OMS1_before, simout.VGO_OMS1, simout.TGO_OMS1, out);
		out.push_back("");
		out.push_back("MPS Dump");
		PrintStateVector(simout.sv_MPS_dump_after, out);
		out.push_back("");
	}
	else
	{
		out.push_back("OMS-1");
		PrintStateVector(simout.sv_OMS1_after, out);
		PrintBurnData(simout.sv_OMS1_before, simout.VGO_OMS1, simout.TGO_OMS1, out);
		out.push_back("");
	}

	out.push_back("Pre OMS-2");
	PrintStateVector(simout.sv_OMS2_before, out);
	PrintBurnData(simout.sv_OMS2_before, simout.VGO_OMS2, simout.TGO_OMS2, out);
	out.push_back("");
	out.push_back("Post OMS-2");
	PrintStateVector(simout.sv_OMS2_after, out);
	out.push_back("");
	out.push_back("180 deg after OMS-2");
	PrintStateVector(simout.sv_OMS2_perigee, out);

	out.push_back("");
	out.push_back("ET Impact");
	if (simout.ETDidImpact)
	{
		double lat, lng_iner, lng;
		char Buffer2[64];

		OrbMech::latlong_from_r(simout.sv_ET_impact.R, lat, lng_iner);
		lng = InertialtoEarthFixedLongitude(lng_iner, simout.sv_ET_impact.GMT);

		OrbMech::GMT2String_HHMMSSCS(Buffer2, simout.sv_ET_impact.GMT);
		sprintf_s(Buffer, "T+%s LAT %.2lf LNG %.2lf", Buffer2, lat * MathConstants::DEG, lng * MathConstants::DEG);
		out.push_back(Buffer);
	}
	else
	{
		out.push_back("No impact!");
	}
}

void AscentDesignSystem::PrintStateVector(OrbMech::StateVector sv, std::vector<std::string>& out)
{
	double HA, HP, r, v, decl, ran, gamma, azi, lng, TT_X;
	int TXX_FLAG;
	char Buffer2[64];

	ORB_ALT_TIME_TSK(sv.R, sv.V, HA, HP, TT_X, TXX_FLAG);

	OrbMech::GMT2String_HHMMSSCS(Buffer2, sv.GMT);

	sprintf_s(Buffer, "T+%s HA %.1lf HP %.1lf NM ", Buffer2, HA / MathConstants::NAUTICAL_MILE_TO_M, HP / MathConstants::NAUTICAL_MILE_TO_M);
	strtemp.assign(Buffer);
	
	OrbMech::GMT2String_MMSS(Buffer2, TT_X);
	if (TXX_FLAG == 1)
	{
		sprintf_s(Buffer, "TTA %s", Buffer2);
	}
	else if (TXX_FLAG == 2)
	{
		sprintf_s(Buffer, "TTP %s", Buffer2);
	}
	else
	{
		sprintf_s(Buffer, "TTC");
	}
	strtemp.append(Buffer);
	out.push_back(strtemp);

	OrbMech::PICSSC(true, sv.R, sv.V, r, v, decl, ran, gamma, azi);

	lng = InertialtoEarthFixedLongitude(ran, sv.GMT);

	sprintf_s(Buffer, "R %.1lf ALT %.1lf VEL %.1lf LAT %.2lf LNG %.2lf GAM %.4lf AZI %.2lf",
		r/MathConstants::NAUTICAL_MILE_TO_M, (r - globcnst.EARTH_RADIUS_EQUATOR) / MathConstants::NAUTICAL_MILE_TO_M, length(sv.V) / MathConstants::FOOT_TO_M,
		decl * MathConstants::DEG, lng * MathConstants::DEG, gamma * MathConstants::DEG, azi * MathConstants::DEG);
	out.push_back(Buffer);
}

void AscentDesignSystem::PrintBurnData(OrbMech::StateVector sv, VECTOR3 VGO, double TGO, std::vector<std::string>& out)
{
	MATRIX3 M_LVLH;
	VECTOR3 VGO_LVLH;

	M_LVLH = OrbMech::LVLH_Matrix(sv.R, sv.V);

	VGO_LVLH = mul(M_LVLH, VGO) / MathConstants::FOOT_TO_M;

	sprintf_s(Buffer, "DV %.1lf TGO %.1lf sec VGO %+.1lf %+.1lf %+.1lf ", length(VGO_LVLH), TGO, VGO_LVLH.x, VGO_LVLH.y, VGO_LVLH.z);
	out.push_back(Buffer);
}

void AscentDesignSystem::ORB_ALT_TIME_TSK(VECTOR3 R, VECTOR3 V, double& HA, double& HP, double& TT_X, int& TXX_FLAG) const
{
	// INPUTS:
	// R = Position vector, meters (near equatorial coordinates)
	// V = Velocity vector, meters/second (near equatorial coordinates)
	// OUTPUTS:
	// HA = Apogee altitude, meters
	// HP = Perigee altitude, meters
	// TT_X = Time to apsis
	// TXX_FLAG: 1 = TTA, 2 = TTP, 3 = TTC

	VECTOR3 EARTH_POLE, R_UNIT;
	double DELT_H_CIRC, RMAG, RDOT, AM, P, K, MM, NN, S_INC, NU, M, N, E, DELTA, KK;

	EARTH_POLE = _V(0, 0, 1);
	DELT_H_CIRC = 5.0 * MathConstants::NAUTICAL_MILE_TO_M;

	RMAG = length(R);
	R_UNIT = R / RMAG;
	RDOT = dotp(V, R_UNIT);

	AM = globcnst.EARTH_MU * RMAG / (2.0 * globcnst.EARTH_MU - RMAG * dotp(V, V));
	P = RMAG * (2.0 - RMAG / AM - RMAG * RDOT * RDOT / globcnst.EARTH_MU);

	K = globcnst.J2_GRAV * pow(globcnst.EARTH_RADIUS_EQUATOR, 2) / (4.0 * P);

	MM = dotp(EARTH_POLE, R_UNIT);
	NN = dotp(unit(V - R_UNIT * RDOT), EARTH_POLE);
	S_INC = MM * MM + NN * NN;

	AM = AM - (globcnst.J2_GRAV * pow(globcnst.EARTH_RADIUS_EQUATOR, 2) * pow(AM, 2) / pow(RMAG, 3)) * (1.0 - 3.0 * pow(MM, 2));
	NU = AM * sqrt(AM / globcnst.EARTH_MU);

	RMAG = RMAG + K * (pow(MM, 2) - pow(NN, 2));
	RDOT = RDOT + 4.0 * K * MM * NN / NU;
	AM = AM - K * (2.0 - 3.0 * S_INC);

	M = RMAG * RDOT / sqrt(globcnst.EARTH_MU * AM);
	N = 1.0 - RMAG / AM;

	E = sqrt(pow(M, 2) + pow(N, 2));

	DELTA = K * (NN * NN - MM * MM);

	if (2.0 * AM * E < DELT_H_CIRC)
	{
		TXX_FLAG = 3;
		TT_X = 0.0;
	}
	else
	{
		TT_X = ((1.0 + OrbMech::sgn(RDOT)) * MathConstants::PI05 + M - atan2(M, N)) * NU;
		M = sqrt(1.0 - pow(E, 2)) * M / (E * (1.0 - N));
		N = (N - pow(E, 2)) / (E * (1.0 - N));
		if (RDOT >= 0.0)
		{
			TXX_FLAG = 1;
		}
		else
		{
			TXX_FLAG = 2;
		}
		DELTA = DELTA * (pow(N, 2) - pow(M, 2)) + 4.0 * M * MM * N * NN * K;
		KK = (AM * AM * E) / (4.0 * P);

		// To prevent numerical problems in applying this correction term (KK), a check is performed to	ensure that it is in the allowable range(if KK > K S_INC)
		if (KK > K * S_INC)
		{
			double DELTB;

			DELTB = 2.0 * K * (M * N * (MM * MM - NN * NN) - MM * NN * (M * M - N * N));
			if (DELTB != 0)
			{
				//For a nonzero value of DELTB, compute the time correction term (KK) and the time-to-next-apsis (TT_X)
				KK = -(OrbMech::sgn(RDOT) * KK + DELTA) / DELTB;
				TT_X = TT_X + (1.0 + 2.0 * OrbMech::sgn(RDOT) * E) * NU / (KK + OrbMech::sgn(KK) * sqrt(KK * KK + 2.0));

				//If the time-to-next-apsis (TT_X) is negative add half the period to this time and update the TXX_FLAG
				if (TT_X < 0)
				{
					TT_X = TT_X + MathConstants::PI * NU;
					TXX_FLAG = 1 + (TXX_FLAG % 2);
				}
			}
		}
	}
	
	HA = (AM * (1.0 + E) + DELTA - globcnst.EARTH_RADIUS_EQUATOR);
	HP = (AM * (1.0 - E) + DELTA - globcnst.EARTH_RADIUS_EQUATOR);
}

double AscentDesignSystem::InertialtoEarthFixedLongitude(double lng_iner, double t) const
{
	double lng;

	lng = lng_iner - (globcnst.EARTH_RATE * t);
	OrbMech::normalizeAngle(lng, false);
	return lng;
}

double AscentDesignSystem::EarthFixedToInertialLongitude(double lng, double t) const
{
	double lng_iner;

	lng_iner = lng + globcnst.EARTH_RATE * t;
	OrbMech::normalizeAngle(lng_iner, false);
	return lng_iner;
}

int AscentDesignSystem::coast(OrbMech::StateVector sv_in, double dt, OrbMech::StateVector& sv_out) const
{
	PrecisePredictorInputs predin;
	PrecisePredictorOutputs predout;

	predin.SV_INIT = sv_in;
	predin.TMAX = dt;

	if (dt < 0.0)
	{
		predin.HMULT = -1.0;
		predin.TMAX = abs(dt);
	}

	PrecisePredictor pred(globcnst);

	pred.RUN(predin, predout);

	if (predout.Error || predout.ISTOPS != 0) return 1;

	sv_out = predout.SV_FINAL;
	return 0;
}

int AscentDesignSystem::FindApogee(OrbMech::StateVector sv1, OrbMech::StateVector& sv_a) const
{
	PrecisePredictorInputs predin;
	PrecisePredictorOutputs predout;

	predin.SV_INIT = sv1;
	predin.ISTOPS = 3;
	predin.STOPVA = 0.0;
	predin.TMIN = 10.0 * 60.0;

	PrecisePredictor pred(globcnst);

	pred.RUN(predin, predout);

	// Not found?
	if (predout.Error || predout.ISTOPS != 3) return 1;

	// Success
	sv_a = predout.SV_FINAL;
	return 0;
}