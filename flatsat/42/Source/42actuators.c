/*    This file is distributed with 42,                               */
/*    the (mostly harmless) spacecraft dynamics simulation            */
/*    created by Eric Stoneking of NASA Goddard Space Flight Center   */

/*    Copyright 2010 United States Government                         */
/*    as represented by the Administrator                             */
/*    of the National Aeronautics and Space Administration.           */

/*    No copyright is claimed in the United States                    */
/*    under Title 17, U.S. Code.                                      */

/*    All Other Rights Reserved.                                      */


#include "42.h"

/* #ifdef __cplusplus
** namespace _42 {
** using namespace Kit;
** #endif
*/

/**********************************************************************/
void ThrModel(double *PulseWidthCmd,double DT,struct ThrType *Thr,
   struct SCType *S)
{
      struct FlexNodeType *FN;
      double r[3];
      long i;

      if (*PulseWidthCmd > DT) {
         Thr->F = Thr->Fmax;
         *PulseWidthCmd -= DT;
      }
      else {
         Thr->F = (*PulseWidthCmd/DT)*Thr->Fmax;
         *PulseWidthCmd = 0.0;
      }

      if (Thr->F < 0.0) Thr->F = 0.0;
      if (Thr->F > Thr->Fmax) Thr->F = Thr->Fmax;

      Thr->Frc[0] = Thr->F*Thr->A[0];
      Thr->Frc[1] = Thr->F*Thr->A[1];
      Thr->Frc[2] = Thr->F*Thr->A[2];

      for(i=0;i<3;i++) r[i] = Thr->PosB[i] - S->B[Thr->Body].cm[i];
      VxV(r,Thr->Frc,Thr->Trq);

      if (S->FlexActive) {
         FN = &S->B[0].FlexNode[Thr->FlexNode];
         for(i=0;i<3;i++) {
            FN->Trq[i] += Thr->Trq[i];
            FN->Frc[i] += Thr->Frc[i];
         }
      }
}
/**********************************************************************/
void WhlModel(double Tcmd,struct WhlType *W,struct SCType *S)
{
      struct FlexNodeType *FN;
      long i;

      W->Trq = Tcmd;
      if (W->Trq < -W->Tmax) W->Trq = -W->Tmax;
      if (W->Trq >  W->Tmax) W->Trq =  W->Tmax;
      if (W->Trq < 0.0 && W->H <= -W->Hmax) W->Trq = 0.0;
      if (W->Trq > 0.0 && W->H >=  W->Hmax) W->Trq = 0.0;

      if (S->FlexActive) {
         FN = &S->B[0].FlexNode[W->FlexNode];
         for(i=0;i<3;i++) FN->Trq[i] += W->Trq*W->A[i];
      }

}
/**********************************************************************/
void MTBModel(double bvb[3],double Mcmd, double Mmax, double A[3],
              double *M, double Trq[3])
{

      *M = Mcmd;
      if(*M < -Mmax) *M = -Mmax;
      if(*M >  Mmax) *M =  Mmax;

      Trq[0] = *M * (A[1]*bvb[2]-A[2]*bvb[1]);
      Trq[1] = *M * (A[2]*bvb[0]-A[0]*bvb[2]);
      Trq[2] = *M * (A[0]*bvb[1]-A[1]*bvb[0]);

}
/**********************************************************************/
void GimbalModel(long DOF,double Rate[3],double Ang[3],
                 double RateCmd[3],double AngCmd[3],
                 double RateGain[3],double AngGain[3],
                 double MaxRate[3], double MaxTrq[3], double Trq[3])
{
      double DesiredRate,AngErr;
      long i;

      for(i=0;i<DOF;i++) {
         AngErr = fmod(Ang[i] - AngCmd[i],TwoPi);
         if (AngErr > Pi) AngErr -= TwoPi;
         if (AngErr < -Pi) AngErr += TwoPi;
         if (RateGain[i] != 0.0) {
            DesiredRate = Limit(RateCmd[i] - AngGain[i]/RateGain[i]*AngErr,
                                -MaxRate[i],MaxRate[i]);
         }
         else {
            DesiredRate = Limit(RateCmd[i],-MaxRate[i],MaxRate[i]);
         }
         Trq[i] = Limit(-RateGain[i]*(Rate[i]-DesiredRate),
                        -MaxTrq[i],MaxTrq[i]);
      }
}
/**********************************************************************/
void TranslationalModel(long DOF, double Rate[3], double Pos[3],
                        double RateCmd[3], double PosCmd[3],
                        double RateGain[3], double PosGain[3],
                        double MaxRate[3], double MaxFrc[3], double Frc[3])
{
   double DesiredRate,PosErr;
   long i;

   for(i=0;i<DOF;i++) {
      PosErr = Pos[i] - PosCmd[i];
      DesiredRate = Limit(RateCmd[i] - PosGain[i]/RateGain[i]*PosErr,
                          -MaxRate[i],MaxRate[i]);
      Frc[i] = Limit(-RateGain[i]*(Rate[i]-DesiredRate),
                     -MaxFrc[i],MaxFrc[i]);
   }
}
/**********************************************************************/
void ThrusterPlumeFrcTrq(struct SCType *S)
{
      /* Plume Parameters */
      double Temp = 100.0; /* WAG */
      double R = 8.134; /* J/(K*mol) */
      double Beta = 1.0/sqrt(2.0*R*Temp);
      double Ve = 100.0; /* WAG */
      double s = Beta*Ve;
      double y = s;
      double A1 = exp(-y*y)+sqrt(Pi)*y*(1.0+erf(y));
      double mdot = 0.1; /* WAG */
      double Coef = mdot/(Beta*A1*Pi);
      /* Other variables */
      struct ThrType *T;
      struct BodyType *B;
      struct GeomType *G;
      struct PolyType *P;
      double PosThrN[3],PosThrB[3],AxisN[3],CPB[3][3];
      double AoN,PosB[3],PosP[3],MagPos,Phat[3],cosphi;
      double w,w2,Wpoly,TotalCoef,FrcP[3],FrcB[3],r[3],TrqB[3],FrcN[3];
      long Ithr,Ipoly,Ib,i;

      for(Ithr=0;Ithr<S->Nthr;Ithr++) {
         T = &S->Thr[Ithr];
         if (T->F > 0.0) { /* Check that this is legit */

            /* Find Force and Torque on each Body */
            for(Ib=0;Ib<S->Nb;Ib++) {
               B = &S->B[Ib];
               G = &Geom[B->GeomTag];

               /* Find thruster location, axis in B */
               MTxV(S->B[0].CN,T->PosB,PosThrN);
               for(i=0;i<3;i++) PosThrN[i] += S->B[0].pn[i] - B->pn[i];
               MxV(B->CN,PosThrN,PosThrB);
               /* Note that plume axis is opposite T->A */
               /* CPB is DCM from B to Plume (P) frame */
               MTxV(S->B[0].CN,T->A,AxisN);
               MxV(B->CN,AxisN,CPB[0]);
               for(i=0;i<3;i++) CPB[0][i] = -CPB[0][i];
               PerpBasis(CPB[0],CPB[1],CPB[2]);

               /* Find force and torque on each illuminated polygon */
               for(Ipoly=0;Ipoly<G->Npoly;Ipoly++) {
                  P = &G->Poly[Ipoly];
                  if (strcmp(Matl[P->Matl].Label,"INTERIOR")) { /* Plume doesn't see interior polys */
                     AoN = VoV(CPB[0],P->Norm);
                     if (AoN < 0.0) {  /* Plume doesn't see polys facing away */
                        /* Find plume pressure (momentum flux) at poly centroid */
                        for(i=0;i<3;i++) PosB[i] = P->Centroid[i] - PosThrB[i];
                        MxV(CPB,PosB,PosP);
                        if (PosP[0] > 0.0) { /* Ignore backflow */
                           MagPos = CopyUnitV(PosP,Phat);
                           cosphi = Phat[0];
                           w = s*cosphi;
                           w2 = w*w;
                           Wpoly = (w2+2.5)*w*exp(-w2);
                           Wpoly += (0.75+3.0*w2+w2*w2)*sqrt(Pi)*(1.0+erf(w));
                           Wpoly *= exp(w2-s*s);
                           TotalCoef = P->Area*Coef*cosphi*Wpoly/(MagPos*MagPos);

                           for(i=0;i<3;i++) FrcP[i] = TotalCoef*Phat[i];
                           MTxV(CPB,FrcP,FrcB);

                           /* Find plume force in B frame */
                           for(i=0;i<3;i++) r[i] = P->Centroid[i]-B->cm[i];
                           VxV(r,FrcB,TrqB);
                           MTxV(B->CN,FrcB,FrcN);
                           for(i=0;i<3;i++) {
                              B->Frc[i] += FrcN[i];
                              B->Trq[i] += TrqB[i];
                           }
                        }
                     }
                  }
               }
            }
         }
      }
}
/**********************************************************************/
/**
 * TThis actuator model just zeros torques on MTRS and Wheels per switch state
/* This model credit Luis F Llano, Software Dev Cromulence LLC 2021  
*/
void EPSACTModel(struct SCType *S)
{      
      struct WhlType *W;
      struct MTBType *MTB;
      struct EPSType *EPS;
      struct AcType *AC;
      
      long i, j;
      long numeps;

      for(numeps=0;numeps<S->Neps;numeps++) {
         EPS = &S->EPS[numeps];
         AC = &S->AC;

         // Set wheel torques to zero when switch is off
         if(EPS->Wheel_Switch == 0) {
            for (i=0;i<S->Nw;i++) {
               W = &S->Whl[i];
               W->Trq = 0;
            }
         }

         // Set MTR torques to zero when switch is off
         if (EPS->MTR_Switch == 0) {
            for(i=0;i<S->Nmtb;i++){
               MTB = &S->MTB[i];
               for (j=0; j<3; j++) {
                  MTB->Trq[j] = 0;
               }
            }
         }

         /**
         * Update Switch Models in EPS Structure
         */
         EPS->COM_Switch = AC->EPS[numeps].COM_Switch;
         EPS->ADCS_Switch = AC->EPS[numeps].ADCS_Switch;
         EPS->Wheel_Switch = AC->EPS[numeps].Wheel_Switch;
         EPS->IMU_Switch = AC->EPS[numeps].Wheel_Switch;
         EPS->StarTracker_Switch = AC->EPS[numeps].StarTracker_Switch;
         EPS->MTR_Switch = AC->EPS[numeps].MTR_Switch;
         EPS->CSS_Switch = AC->EPS[numeps].CSS_Switch;
         EPS->FSS_Switch = AC->EPS[numeps].FSS_Switch;
         EPS->PAYLOAD_Switch = AC->EPS[numeps].PAYLOAD_Switch;
      }
}


/**********************************************************************/
/*  This function is called at the simulation rate.  Sub-sampling of  */
/*  actuators should be done on a case-by-case basis.                 */

void Actuators(struct SCType *S)
{

      long i,j;
      double FrcN[3];
      struct AcType *AC;
      struct JointType *G;
      struct AcJointType *AG;
      struct ThrType *Thr;
      struct EPSType *EPS;

      AC = &S->AC;
      long numeps;

      /* ADCS off switch behavior */
      for(numeps=0;numeps<S->Neps;numeps++) {
         EPS = &S->EPS[numeps];
         if (EPS->ADCS_Switch == 0 ) {
            EPS->Wheel_Switch = 0;
            EPS->MTR_Switch = 0;
            EPS->PAYLOAD_Switch = 0;
         }
      }

      /* Ideal Actuators */
      for(j=0;j<3;j++) {
         S->B[0].Frc[j] += AC->IdealFrc[j];
         S->B[0].Trq[j] += AC->IdealTrq[j];
      }
      /* Wheels */
      if (S->Neps == 0 || S->EPS->Wheel_Switch == ON) {
         for(i=0;i<S->Nw;i++) {
            WhlModel(AC->Whl[i].Tcmd,&S->Whl[i],S);
         }
      }
      /* MTBs */
      if (S->Neps == 0 || S->EPS->MTR_Switch == ON) {
         for(i=0;i<S->Nmtb;i++){
            MTBModel(S->bvb,AC->MTB[i].Mcmd,S->MTB[i].Mmax,
                     S->MTB[i].A,&S->MTB[i].M,
                     S->MTB[i].Trq);
            for(j=0;j<3;j++){
               S->B[0].Trq[j] += S->MTB[i].Trq[j];
            }
         }
      }
      /* Gimbal Drives */
      for(i=0;i<AC->Ng;i++) {
         G = &S->G[i];
         AG = &AC->G[i];
         if (AG->IsUnderActiveControl == ON) {
            /* PD Gimbal Control */
            GimbalModel(G->RotDOF,AG->AngRate,AG->Ang,
                        AG->Cmd.AngRate,AG->Cmd.Ang,
                        AG->AngRateGain,AG->AngGain,
                        AG->MaxAngRate,AG->MaxTrq,G->Trq);
            /* Ideal Kinematic Gimbal Control */
            for(j=0;j<G->RotDOF;j++) {
               G->RateCmd[j] = AG->Cmd.AngRate[j];
               G->AngCmd[j] = AG->Cmd.Ang[j];
            }
         }
      }

      /* Thrusters */
      for(i=0;i<S->Nthr;i++) {
         Thr = &S->Thr[i];
         ThrModel(&AC->Thr[i].PulseWidthCmd,DTSIM,Thr,S);
         MTxV(S->B[Thr->Body].CN,Thr->Frc,FrcN);
         for(j=0;j<3;j++) {
            S->B[Thr->Body].Trq[j] += Thr->Trq[j];
            S->B[Thr->Body].Frc[j] += FrcN[j];
         }
      }
      if (ThrusterPlumesActive) {
         ThrusterPlumeFrcTrq(S);
      }

   /* EPS Actuator Model Last to take into account switch states to zero out actuators that are off */
      EPSACTModel(S);
}

/* #ifdef __cplusplus
** }
** #endif
*/

