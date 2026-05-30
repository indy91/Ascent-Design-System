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

#include "PEG4A.h"

PEG4A::PEG4A(OrbMech::GlobalConstants globconst) : globcnst(globconst), pred(globconst)
{
	RT = _V(0, 0, 0);
}

int PEG4A::OMSBurnPrediction(OrbMech::StateVector sv, VECTOR3 RLS_TEG, double C1, double C2, double HTGT, double THETA, double FT, double VEX)
{
	sv_GD = sv;
	this->C1 = C1;
	this->C2 = C2;
	this->HTGT = HTGT;
	this->THETA = THETA;
	this->FT = FT;
	this->VEX = VEX;

	RT = HTHETA_M50_TGT_TSK(RLS_TEG);

	RP = sv_GD.R;
	VP = sv_GD.V;
	TP = sv_GD.GMT;
	THETA_DOT = sqrt(globcnst.EARTH_MU / (pow(length(sv_GD.R), 3)));
	LAMDXZ = THETA_DOT * K_LAMDXZ;
	TGO = 0;
	VGO = _V(0, 0, 0);
	sv_temp = sv_GD;

	SCONV = false;

	//Init
	if (VelocityToBeGainedSubtask()) return 1;

	ATR = FT / sv_GD.Weight;

	while (SCONV == false)
	{
		VGOMAG = length(VGO);
		TimeToGoSubtask();
		ThrustIntegralSubtask();
		ReferenceThrustVectorsSubtask();
		BurnoutStateVectorPredictionSubtask();
		if (VelocityToBeGainedSubtask()) return 1;
		ConvergenceCheckSubtask();
	}

	VECTOR3 RJ2, VJ2, RC3, VC3;
	sv_temp.R = RC1;
	sv_temp.V = VC1;

	predin.SV_INIT = sv_temp;
	predin.ISTOPS = 0;
	predin.TMAX = TGO;
	predin.GMD = 2;
	predin.GMO = 0;

	pred.RUN(predin, predout);

	RC3 = predout.SV_FINAL.R;
	VC3 = predout.SV_FINAL.V;
	VJ2 = VC3 - VC2;
	RJ2 = RC3 - RC2;

	// Output state vector
	sv_out.R = RP + RJ2;
	sv_out.V = VD + VJ2;
	sv_out.GMT = sv_GD.GMT + TGO;
	sv_out.Area = sv_GD.Area;
	sv_out.CD = sv_GD.CD;
	sv_out.Weight = sv_GD.Weight - FT / VEX * TGO;

	return 0;
}

void PEG4A::GetOutputA(VECTOR3& VGO, double& TGO, OrbMech::StateVector& sv) const
{
	VGO = this->VGO;
	TGO = this->TGO;
	sv = sv_out;
}

VECTOR3 PEG4A::HTHETA_M50_TGT_TSK(VECTOR3 RLS_TEG) const
{
	VECTOR3 IDR, IR, IRT, RT;
	double THETA_LS, DTHETA, RTMAG;

	IDR = unit(crossp(sv_GD.R, crossp(sv_GD.V, sv_GD.R)));
	IR = unit(sv_GD.R);
	THETA_LS = atan2(-dotp(RLS_TEG, IDR), dotp(RLS_TEG, IR));
	if (THETA_LS < 0)
	{
		THETA_LS = THETA_LS + MathConstants::PI2;
	}
	DTHETA = THETA - THETA_LS;
	IRT = IR * cos(DTHETA) + IDR * sin(DTHETA);
	RTMAG = HTGT + globcnst.EARTH_RADIUS_EQUATOR;
	RT = IRT * RTMAG;
	return RT;
}

int PEG4A::VelocityToBeGainedSubtask()
{
	double DTCOAST, RHOMAG;

	IY = unit(crossp(VP, RP));
	//RT = RT - IY * dot(RT, IY);
	if (LTVCON_TSK(RP, RT, IY, C1, C2, VD)) return 1;
	DTCOAST = THETA / THETA_DOT;
	RHOMAG = 1.0 / (1.0 + 0.75 * TGO / DTCOAST);
	VMISS = VP - VD;
	VGO = VGO - VMISS * RHOMAG;
	return 0;
}

void PEG4A::TimeToGoSubtask()
{
	VRATIO = VGOMAG / (6.0 * VEX);
	TGO = VGOMAG / (ATR * (1.0 + 3.0 * VRATIO + 3.0 * VRATIO * VRATIO));
	TP = sv_GD.GMT + TGO;
}

void PEG4A::ThrustIntegralSubtask()
{
	JOL = 0.5 * TGO * (1.0 + VRATIO);
	S = 0.5 * VGOMAG * TGO * (1.0 - VRATIO);
	QPRIME = VGOMAG * TGO * TGO / 12.0;
}

void PEG4A::ReferenceThrustVectorsSubtask()
{
	LAM = VGO / VGOMAG;
	LAMD = crossp(LAM, IY) * LAMDXZ;
}

void PEG4A::BurnoutStateVectorPredictionSubtask()
{
	VECTOR3 RGO, RGRAV, VGRAV;

	RGO = LAMD * QPRIME + LAM * S;
	RC1 = sv_GD.R - RGO / 10.0 - VGO * TGO / 30.0;
	VC1 = sv_GD.V + RGO * 6.0 / 5.0 / TGO - VGO / 10.0;
	sv_temp.R = RC1;
	sv_temp.V = VC1;

	predin.SV_INIT = sv_temp;
	predin.TMAX = TP - predin.SV_INIT.GMT;
	predin.GMD = 0;
	predin.GMO = 0;


	pred.RUN(predin, predout);

	RC2 = predout.SV_FINAL.R;
	VC2 = predout.SV_FINAL.V;
	VGRAV = VC2 - VC1;
	RGRAV = RC2 - RC1 - VC1 * TGO;
	RP = sv_GD.R + sv_GD.V * TGO + RGRAV + RGO;
	VP = sv_GD.V + VGRAV + VGO;
}

void PEG4A::ConvergenceCheckSubtask()
{
	VGOMAG = length(VGO);
	if (length(VMISS) <= KMISS * VGOMAG)
	{
		SCONV = true;
	}
	else
	{
		SCONV = false;
	}
}

int PEG4A::LTVCON_TSK(VECTOR3 RD, VECTOR3 R1, VECTOR3 IY, double C1, double C2, VECTOR3& VD) const
{
	double R0_MAG, R1_MAG, K, Z, W, A, B, C, THETA, SIN_THETA, D, VH1, VR1;

	R0_MAG = length(RD);
	R1_MAG = length(R1);
	K = (R1_MAG - R0_MAG) / R0_MAG;
	Z = R0_MAG * R1_MAG - dotp(RD, R1);

	W = -dotp(crossp(RD, R1), IY) / Z;
	A = K * (1.0 + W * W) + 2.0 * (1.0 - C2 * W);
	B = C1 * W;
	C = 2.0 * globcnst.EARTH_MU / R1_MAG;
	THETA = MathConstants::PI + atan2(-2.0 * W, 1.0 - W * W);
	SIN_THETA = 2.0 * W / (1.0 * W * W);
	D = B * B + A * C;

	if (D < 0.0) return 1;

	VH1 = C / (-B + sqrt(D));
	VR1 = C1 + C2 * VH1;
	VD = (RD * (K * W * VH1 - VR1) + crossp(RD, IY) * (1.0 + K) * VH1) / R0_MAG;

	return 0;
}