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
#include "PrecisePredictor.h"

class PEG4A
{
public:
	PEG4A(OrbMech::GlobalConstants globconst);
	int OMSBurnPrediction(OrbMech::StateVector sv, VECTOR3 RLS_TEG, double C1, double C2, double HTGT, double THETA, double FT, double VEX);
	void GetOutputA(VECTOR3& VGO, double& TGO, OrbMech::StateVector &sv) const;
private:
	// PEG SUBROUTINES
	VECTOR3 HTHETA_M50_TGT_TSK(VECTOR3 RLS_TEG) const;
	int VelocityToBeGainedSubtask();
	void TimeToGoSubtask();
	void ThrustIntegralSubtask();
	void ReferenceThrustVectorsSubtask();
	void BurnoutStateVectorPredictionSubtask();
	void ConvergenceCheckSubtask();

	// UTILITIES
	int LTVCON_TSK(VECTOR3 RD, VECTOR3 R1, VECTOR3 IY, double C1, double C2, VECTOR3& VD) const;

	// INPUT

	// Ignition state vector
	OrbMech::StateVector sv_GD;
	// PEG target
	double C1, C2, HTGT, THETA;
	double FT, VEX;

	// LOCAL
	// Temporary state vectors for predictor
	OrbMech::StateVector sv_temp;
	// Target position vector
	VECTOR3 RT;
	// Cutoff state vector
	VECTOR3 RP, VP;
	double TP;
	double TGO;
	VECTOR3 VGO;
	double THETA_DOT, LAMDXZ;
	bool SCONV;
	VECTOR3 IY;
	VECTOR3 VMISS;
	double VGOMAG;
	double VRATIO;
	double ATR;
	double JOL, S, QPRIME;
	VECTOR3 LAM, LAMD;
	VECTOR3 RC1, VC1, RC2, VC2;
	double MBO;

	PrecisePredictor pred;
	PrecisePredictorInputs predin;
	PrecisePredictorOutputs predout;

	// OUTPUT
	OrbMech::StateVector sv_out;
	VECTOR3 VD;

	// CONSTANTS

	OrbMech::GlobalConstants globcnst;
	const double KMISS = 0.0001; // 0.01; Change to converge HP better
	const double K_LAMDXZ = 0.0;
};