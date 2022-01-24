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
/*  Substantial contributions to this model provided                  */
/*  by Jeffrey Calixto, 2019 summer intern.                           */
/*                                                                    */
/*  Acceleration of a point A fixed in SC[Isc].B[0], expressed in     */
/*  B[0].  Due to 42's accounting of forces (esp. gravity), the       */
/*  gravity-gradient force accounting for the offset from             */
/*  SC[Isc].B[0].cm to A must be explicitly accounted for.  All other */
/*  gravity terms apply equally to A and B[0].  (Assuming gravity-    */
/*  gradient from non-spherical primary and 3rd-body forces is        */
/*  negligible.)  Surface forces are included in S->         */
void AccelerometerModel(struct SCType *S)
{
      struct AccelType *A;
      double p[3],wxp[3],wxwxp[3],axp[3],ab[3];
      double r,Coef,rhatn[3],rhat[3],rhatop;
      double accb[3],asnb[3];
      long j;
      struct BodyType *B; 
      struct FlexNodeType *FN;
      long Ia;
      double PrevBias,PrevDV,AccError;
      long Counts,PrevCounts;

      B = &S->B[0];

     for(Ia=0;Ia<S->Nacc;Ia++) { 
         A = &S->Accel[Ia];
         A->SampleCounter++; 
         if (A->SampleCounter >= A->MaxCounter) {
            A->SampleCounter = 0;

            /* Vector from cm of B0 to A */
            for(j=0;j<3;j++) 
               p[j] = A->PosB[j] - B->cm[j];

            /* abs and alfbn are byproducts of NbodyAttitudeRK4 */
            VxV(B->wn,p,wxp);
            VxV(B->wn,wxp,wxwxp);
            VxV(S->alfbn,p,axp);
            MxV(B->CN,S->abs,ab);

            /* Acceleration of a point fixed in an accelerating, rotating body */
            for(j=0;j<3;j++) accb[j] = ab[j]+axp[j]+wxwxp[j];

            /* Grav-grad force (see Hughes, p.246, eq (56)) */
            if (GGActive) {
               r = MAGV(S->PosN);
               Coef = -3.0*Orb[S->RefOrb].mu/(r*r*r);
               CopyUnitV(S->PosN,rhatn);
               MxV(B->CN,rhatn,rhat);
               rhatop = VoV(rhat,p);
               for(j=0;j<3;j++) {
                  accb[j] += Coef*(p[j]-3.0*rhat[j]*rhatop);
               }
            }

            /* Add acceleration of SC cm from external surface forces */
            MxV(B->CN,S->asn,asnb);
            for(j=0;j<3;j++) accb[j] += asnb[j];


            /* .. Add noise, etc. */
            /* this is the noise added for the accelerometer ( the Edited ) */

             
            if (S->FlexActive) {
               FN = &S->B[0].FlexNode[A->FlexNode];  
               A->TrueAcc = VoV(FN->TotTrnVel,A->Axis); /* TODO: Fix this */
            }
            else {
               A->TrueAcc = VoV(accb,A->Axis); 
            }                                        
            
            PrevBias = A->CorrCoef*A->Bias;
            A->Bias = PrevBias + A->BiasStabCoef*GaussianRandom(RNG);
            AccError = 0.5*(A->Bias+PrevBias) + A->DVRWCoef*GaussianRandom(RNG);
         
            A->MeasAcc = Limit(A->Scale*A->TrueAcc + AccError,
               -A->MaxAcc,A->MaxAcc); 
         
            PrevDV = A->DV; 
            A->DV = PrevDV + A->MeasAcc*A->SampleTime 
               + A->DVNoiseCoef*GaussianRandom(RNG);
         
            PrevCounts = (long) (PrevDV/A->Quant+0.5);
            Counts = (long) (A->DV/A->Quant+0.5);

            A->MeasAcc = ((double) (Counts - PrevCounts))*A->Quant/A->SampleTime;
            
            S->AC.Accel[Ia].Acc = A->MeasAcc;
         }
      }
}
/**********************************************************************/
void GyroModel(struct SCType *S)
{
      struct GyroType *G;
      struct FlexNodeType *FN;
      long Ig;
      double PrevBias,RateError,PrevAngle;
      long Counts,PrevCounts;
      
      for(Ig=0;Ig<S->Ngyro;Ig++) {
         G = &S->Gyro[Ig];
         
         G->SampleCounter++;
         if (G->SampleCounter >= G->MaxCounter) {
            G->SampleCounter = 0;
            
            if (S->FlexActive) {
               FN = &S->B[0].FlexNode[G->FlexNode];
               G->TrueRate = VoV(FN->TotAngVel,G->Axis);
            }
            else {
               G->TrueRate = VoV(S->B[0].wn,G->Axis);
            }
            
            PrevBias = G->CorrCoef*G->Bias;
            G->Bias = PrevBias + G->BiasStabCoef*GaussianRandom(RNG);
            RateError = 0.5*(G->Bias+PrevBias) + G->ARWCoef*GaussianRandom(RNG);
         
            G->MeasRate = Limit(G->Scale*G->TrueRate + RateError,
               -G->MaxRate,G->MaxRate);
         
            PrevAngle = G->Angle;
            G->Angle = PrevAngle + G->MeasRate*G->SampleTime 
               + G->AngNoiseCoef*GaussianRandom(RNG);
         
            PrevCounts = (long) (PrevAngle/G->Quant+0.5);
            Counts = (long) (G->Angle/G->Quant+0.5);

            G->MeasRate = ((double) (Counts - PrevCounts))*G->Quant/G->SampleTime;
            
            S->AC.Gyro[Ig].Rate = G->MeasRate;
         }
      }
}
/**********************************************************************/
void MagnetometerModel(struct SCType *S)
{
      struct MagnetometerType *MAG;
      long Counts,Imag;
      double Signal;
      
      for(Imag=0;Imag<S->Nmag;Imag++) {
         MAG = &S->MAG[Imag];
         
         MAG->SampleCounter++;
         if (MAG->SampleCounter >= MAG->MaxCounter) {
            MAG->SampleCounter = 0;
            
            Signal = MAG->Scale*VoV(S->bvb,MAG->Axis) 
               + MAG->Noise*GaussianRandom(RNG);
            Signal = Limit(Signal,-MAG->Saturation,MAG->Saturation); 
            Counts = (long) (Signal/MAG->Quant+0.5);
            MAG->Field = ((double) Counts)*MAG->Quant;
            
            S->AC.MAG[Imag].Field = MAG->Field;
         }
      }
}
/**********************************************************************/
/* This model credit Paul McKee, summer intern 2018                   */
void CssModel(struct SCType *S)
{
      struct CssType *CSS;
      long Counts, Icss; 
      double Signal; 
      double SoA; 
      double svb[3];
         
      for(Icss=0;Icss<S->Ncss;Icss++) {
         CSS = &S->CSS[Icss];
         
         CSS->SampleCounter++;
         if (CSS->SampleCounter >= CSS->MaxCounter) {
            CSS->SampleCounter = 0;
        
            if (S->Eclipse) {
               CSS->Valid = FALSE; 
               CSS->Illum = 0.0; 
            }
            else {
               MxV(S->B[CSS->Body].CN,S->svn,svb);
               SoA = VoV(svb,CSS->Axis);
               if (SoA > CSS->CosFov) {
                  /* Sun within FOV */
                  CSS->Valid = TRUE;
                  Signal = CSS->Scale*SoA;
                  Counts = (long) (Signal/CSS->Quant+0.5);
                  CSS->Illum = ((double) Counts)*CSS->Quant;
               }        
               else {
                  /* Sun not in FOV */
                  CSS->Valid = FALSE;
                  CSS->Illum = 0.0;   
               }
            }
         
            /* Copy into AC structure */
            S->AC.CSS[Icss].Valid = CSS->Valid;       
            S->AC.CSS[Icss].Illum = CSS->Illum;
         }       
      }
}
/**********************************************************************/
void FssModel(struct SCType *S)
{
      struct FssType *FSS;
      static struct RandomProcessType *FssNoise;
      double svs[3],SunAng[2],Signal;
      long Counts;
      static long First = 1;
      long Ifss,i;
      
      if (First) {
         First = 0;
         FssNoise = CreateRandomProcess(10);
      }
      
      for(Ifss=0;Ifss<S->Nfss;Ifss++) {
         FSS = &S->FSS[Ifss];
         
         FSS->SampleCounter++;
         if (FSS->SampleCounter >= FSS->MaxCounter) {
            FSS->SampleCounter = 0;
         
            if (S->Eclipse) {
               FSS->Valid = FALSE;
            }
            else {
               MxV(FSS->CB,S->svb,svs);
               SunAng[0] = atan2(svs[0],svs[2]);
               SunAng[1] = atan2(svs[1],svs[2]);
               if (fabs(SunAng[0]) < FSS->FovHalfAng[0] && 
                   fabs(SunAng[1]) < FSS->FovHalfAng[1] &&
                   svs[2] > 0.0) {
                  FSS->Valid = TRUE;
               }
               else {
                  FSS->Valid = FALSE;
               }
            }
            
            if (FSS->Valid) {
               for(i=0;i<2;i++) {
                  Signal = SunAng[i] + FSS->NEA*GaussianRandom(FssNoise);
                  Counts = (long) (Signal/FSS->Quant+0.5);
                  FSS->SunAng[i] = ((double) Counts)*FSS->Quant;
               }
            }
            else {
               FSS->SunAng[0] = 0.0;
               FSS->SunAng[1] = 0.0;
            }
            
            S->AC.FSS[Ifss].Valid = FSS->Valid;
            for(i=0;i<2;i++) S->AC.FSS[Ifss].SunAng[i] = FSS->SunAng[i];
         }
      }
}
/**********************************************************************/
void StarTrackerModel(struct SCType *S)
{
      struct StarTrackerType *ST;
      struct FlexNodeType *FN;
      static struct RandomProcessType *StNoise;
      struct WorldType *W;
      double qsn[4],Qnoise[4];
      double BoS,OrbRad,LimbAng,NadirVecB[3],BoN;
      double mvn[3],MoonDist,mvb[3],BoM;
      double qfb[4],qfn[4];
      static long First = 1;
      long Ist,i;
      
      if (First) {
         First = 0;
         StNoise = CreateRandomProcess(1);
      }
      
      for(Ist=0;Ist<S->Nst;Ist++) {
         ST = &S->ST[Ist];
         
         ST->SampleCounter++;
         if (ST->SampleCounter >= ST->MaxCounter) {
            ST->SampleCounter = 0;
         
            ST->Valid = TRUE;
            /* Sun Occultation? */
            BoS = VoV(ST->CB[2],S->svb);
            if (BoS > ST->CosSunExclAng) ST->Valid = FALSE;
            /* Earth Occultation? (Generalized to whatever world we're orbiting) */
            W = &World[Orb[S->RefOrb].World];
            OrbRad = MAGV(S->PosN);
            LimbAng = asin(W->rad/OrbRad);
            MxV(S->B[0].CN,S->CLN[2],NadirVecB);
            BoN = VoV(ST->CB[2],NadirVecB);
            if (BoN > cos(LimbAng + ST->EarthExclAng)) ST->Valid = FALSE;
            /* Moon Occultation? (Only worked out if orbiting Earth.  Customize as needed)*/
            if (Orb[S->RefOrb].World == EARTH) {
               for(i=0;i<3;i++) mvn[i] = World[LUNA].eph.PosN[i] - S->PosN[i];
               MoonDist = UNITV(mvn);
               LimbAng = asin(World[LUNA].rad/MoonDist);
               MxV(S->B[0].CN,mvn,mvb);
               BoM = VoV(ST->CB[2],mvb);
               if (BoM > cos(LimbAng+ST->MoonExclAng)) ST->Valid = FALSE;
            }
            if (ST->Valid) {
               if (S->FlexActive) {
                  FN = &S->B[0].FlexNode[ST->FlexNode];
                  for(i=0;i<3;i++) qfb[i] = 0.5*FN->ang[i];
                  qfb[3] = sqrt(1.0-qfb[0]*qfb[0]-qfb[1]*qfb[1]-qfb[2]*qfb[2]);
                  QxQ(qfb,S->B[0].qn,qfn);
                  QxQ(ST->qb,qfn,qsn);
               }
               else {
                  QxQ(ST->qb,S->B[0].qn,qsn);
               }
               /* Add Noise in ST frame */
               for(i=0;i<3;i++) Qnoise[i] = 0.5*ST->NEA[i]*GaussianRandom(StNoise);
               Qnoise[3] = 1.0;
               UNITQ(Qnoise);
               QxQ(Qnoise,qsn,ST->qn);
            }
            
            S->AC.ST[Ist].Valid = ST->Valid;
            for(i=0;i<4;i++) {
               S->AC.ST[Ist].qn[i] = ST->qn[i];
            }
         }
      }
}
/**********************************************************************/
void GpsModel(struct SCType *S)
{
      struct GpsType *GPS;
      static struct RandomProcessType *GpsNoise;
      double PosW[3],MagPosW;
      long Ig,i;
      static long First = 1;
      
      if (First) {
         First = 0;
         GpsNoise = CreateRandomProcess(2);
      }
      
      if (Orb[S->RefOrb].World == EARTH) {
         for(Ig=0;Ig<S->Ngps;Ig++) {
            GPS = &S->GPS[Ig];
            
            GPS->SampleCounter++;
            if (GPS->SampleCounter >= GPS->MaxCounter) {
               GPS->SampleCounter = 0;
               
               GPS->Valid = TRUE;
         
               GPS->Rollover = GpsRollover;
               GPS->Week = GpsWeek;
               GPS->Sec = GpsSecond + GPS->TimeNoise*GaussianRandom(GpsNoise);
      
               for(i=0;i<3;i++) {
                  GPS->PosN[i] = S->PosN[i] + GPS->PosNoise*GaussianRandom(GpsNoise);
                  GPS->VelN[i] = S->VelN[i] + GPS->VelNoise*GaussianRandom(GpsNoise);
               }
               MxV(World[EARTH].CWN,S->PosN,PosW);
               MxV(World[EARTH].CWN,GPS->PosN,GPS->PosW);
               MxV(World[EARTH].CWN,GPS->VelN,GPS->VelW);
               /* Subtract Earth rotation velocity */
               GPS->VelW[0] -= -World[EARTH].w*PosW[1];
               GPS->VelW[1] -=  World[EARTH].w*PosW[0];
         
               MagPosW = MAGV(GPS->PosW);
               GPS->Lng = atan2(GPS->PosW[1],GPS->PosW[0]);
               GPS->Lat = asin(GPS->PosW[2]/MagPosW);
               GPS->Alt = MagPosW - World[EARTH].rad;
               ECEFToWGS84(GPS->PosW,&GPS->WgsLat,&GPS->WgsLng,&GPS->WgsAlt);
               
               S->AC.GPS[Ig].Valid = GPS->Valid;
               S->AC.GPS[Ig].Rollover = GPS->Rollover;
               S->AC.GPS[Ig].Week = GPS->Week;
               S->AC.GPS[Ig].Sec = GPS->Sec;
               
               for(i=0;i<3;i++) {
                  S->AC.GPS[Ig].PosN[i] = GPS->PosN[i];
                  S->AC.GPS[Ig].VelN[i] = GPS->VelN[i];
                  S->AC.GPS[Ig].PosW[i] = GPS->PosW[i];
                  S->AC.GPS[Ig].VelW[i] = GPS->VelW[i];
               }
               S->AC.GPS[Ig].Lng = GPS->Lng;
               S->AC.GPS[Ig].Lat = GPS->Lat;
               S->AC.GPS[Ig].Alt = GPS->Alt;
               S->AC.GPS[Ig].WgsLng = GPS->WgsLng;
               S->AC.GPS[Ig].WgsLat = GPS->WgsLat;
               S->AC.GPS[Ig].WgsAlt = GPS->WgsAlt;
               
            }
         }
      }
      else {
         for(Ig=0;Ig<S->Ngps;Ig++) S->GPS[Ig].Valid = FALSE;
      }
}

/**********************************************************************/
/** Simple EPS model that is fairly naive with a simple battery charge/dischage model with
* look up table for battery voltage as function of state of charge (simple % amp-hour remaining)
*/
void EPSModel(struct SCType *S)
{     
   struct EPSType *EPS;
   struct AcType *AC;
   struct WhlType *W;
   struct MTBType *MTB;
   long Ieps, i;
   static long First = 1;
   double tempCurrent, tempVoltage;
   double PowerUsed = 0.0;
   double PowerGenerated = 0.0;
   double BatteryPower;
   double StateOfCharge;
   double PercentUse, TorqueUsed, WheelCurrent, WheelCurrentSingle, MtrCurrent, MtrCurrentSingle;

   /* Battery Lookup Table for 3S Lition Iion Pack */
   static double lithium_ion_voltage_table[] = {9.93, 11.04, 11.07, 11.13, 11.22, 11.28, 11.34, 11.34, 11.37, 11.43, 11.46, 11.52, 11.64, 11.76, 11.85, 11.94, 12.0, 12.15, 12.27, 12.39, 12.54 };
   static double lithium_ion_soc_table[] = {0, 5, 10, 15, 20, 25, 30, 25, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100};
   static unsigned int lithium_ion_table_size = 21;

   // Loop over each EPS (though this really doesn't make sense or have any meaning in the current model)
   for(Ieps=0;Ieps<S->Neps;Ieps++) {
      EPS = &S->EPS[Ieps];
      AC = &S->AC;

      if (First) {
         First = 0;
         StateOfCharge = LinInterp(lithium_ion_voltage_table, lithium_ion_soc_table, EPS->BatteryVoltage, lithium_ion_table_size) / 100;
         EPS->BatteryEnergy = StateOfCharge * (EPS->BatteryCapacity * 3600);
      }

      // IMU 
      if (EPS->IMU_Switch) {
         tempCurrent = EPS->IMU_I + (EPS->IMU_I * EPS->CurrentNoisePercentage * GaussianRandom(RNG));
         AC->EPS[Ieps].IMU_I = tempCurrent;
         tempVoltage = EPS->IMU_V + (EPS->IMU_V * EPS->VoltageNoisePercentage * GaussianRandom(RNG));
         AC->EPS[Ieps].IMU_V = tempVoltage;
         PowerUsed += tempCurrent * tempVoltage;
      } else {
         AC->EPS[Ieps].IMU_I = 0.0;
         AC->EPS[Ieps].IMU_V = 0.0;
      }
      
      // Star Tracker (assumsed to be sum total for all sensors)
      if (EPS->StarTracker_Switch) {
         tempCurrent = EPS->StarTracker_I + (EPS->StarTracker_I * EPS->CurrentNoisePercentage * GaussianRandom(RNG));
         AC->EPS[Ieps].StarTracker_I = tempCurrent;
         tempVoltage = EPS->StarTracker_V + (EPS->StarTracker_V * EPS->VoltageNoisePercentage * GaussianRandom(RNG));
         AC->EPS[Ieps].StarTracker_V = tempVoltage;
         PowerUsed += tempCurrent * tempVoltage;
      } else {
         AC->EPS[Ieps].StarTracker_I = 0.0;
         AC->EPS[Ieps].StarTracker_V = 0.0;
      }

      // CSS Sensors (assumsed to be sum total for all sensors)
      if (EPS->CSS_Switch) {
         tempCurrent = EPS->CSS_I + (EPS->CSS_I * EPS->CurrentNoisePercentage * GaussianRandom(RNG));
         AC->EPS[Ieps].CSS_I = tempCurrent;
         tempVoltage = EPS->CSS_V + (EPS->CSS_V * EPS->VoltageNoisePercentage * GaussianRandom(RNG));
         AC->EPS[Ieps].CSS_V = tempVoltage;
         PowerUsed += tempCurrent * tempVoltage;
      } else {
         AC->EPS[Ieps].CSS_I = 0.0;
      }

      // FSS Sensors (assumsed to be sum total for all sensors)
      if (EPS->FSS_Switch) {
         tempCurrent = EPS->FSS_I + (EPS->FSS_I * EPS->CurrentNoisePercentage * GaussianRandom(RNG));
         AC->EPS[Ieps].FSS_I = tempCurrent;
         tempVoltage = EPS->FSS_V + (EPS->FSS_V * EPS->VoltageNoisePercentage * GaussianRandom(RNG));
         AC->EPS[Ieps].FSS_V = tempVoltage;
         PowerUsed += tempCurrent * tempVoltage;
      } else {
         AC->EPS[Ieps].FSS_I = 0.0;
         AC->EPS[Ieps].FSS_V = 0.0;
      }

      // ADCS Subystem
      if (EPS->ADCS_Switch) {
         tempCurrent = EPS->ADCS_I + (EPS->ADCS_I * EPS->CurrentNoisePercentage * GaussianRandom(RNG));
         AC->EPS[Ieps].ADCS_I = tempCurrent;
         tempVoltage = EPS->ADCS_V + (EPS->ADCS_V * EPS->VoltageNoisePercentage * GaussianRandom(RNG));
         AC->EPS[Ieps].ADCS_V = tempVoltage;
         PowerUsed += tempCurrent * tempVoltage;
      } else {
         AC->EPS[Ieps].ADCS_I = 0.0;
         AC->EPS[Ieps].ADCS_V = 0.0; 
      }

      // TT&C COMM
      if (EPS->COM_Switch) {
         tempCurrent = EPS->COM_I + (EPS->COM_I * EPS->CurrentNoisePercentage * GaussianRandom(RNG));
         AC->EPS[Ieps].COM_I = tempCurrent;
         tempVoltage = EPS->COM_V + (EPS->COM_V * EPS->VoltageNoisePercentage * GaussianRandom(RNG));
         AC->EPS[Ieps].COM_V = tempVoltage;
         PowerUsed += tempCurrent * tempVoltage;
      } else {
         AC->EPS[Ieps].COM_I = 0.0;
         AC->EPS[Ieps].COM_V = 0.0;
      }

      // CDH Static Power Since it assumed always ON
      tempCurrent = EPS->CDH_I + (EPS->CDH_I * EPS->CurrentNoisePercentage * GaussianRandom(RNG));
      tempVoltage = EPS->CDH_V + (EPS->CDH_V * EPS->VoltageNoisePercentage * GaussianRandom(RNG));
      PowerUsed += tempCurrent * tempVoltage;
      AC->EPS[Ieps].CDH_I = tempCurrent;
      AC->EPS[Ieps].CDH_V = tempVoltage;


      /* Consumption by the wheels */
      if (EPS->Wheel_Switch) {
         WheelCurrent = 0.0;
         // Same voltage supply for all wheels
         tempVoltage = EPS->Wheel_V + (EPS->Wheel_V * EPS->VoltageNoisePercentage * GaussianRandom(RNG));

         // Loop over each wheel to get individual current as function of wheel torque
         for(i=0;i<S->Nw;i++) {
            W = &S->Whl[i];
            PercentUse = fabs(W->Trq) / W->Tmax;
            WheelCurrentSingle = (EPS->Wheel_I) * PercentUse; // Wheel current doesn't have noise since it is assumed to be tightly controlled
            // Calculate current on a per wheel basis
            // tempCurrent = WheelCurrentSingle + (WheelCurrentSingle * EPS->CurrentNoisePercentage * GaussianRandom(RNG));
            WheelCurrent += WheelCurrentSingle;
         }
         // Total wheel calculations
         PowerUsed += WheelCurrent * tempVoltage;
         AC->EPS[Ieps].Wheel_I = WheelCurrent;
         AC->EPS[Ieps].Wheel_V = tempVoltage;

      } else {
         AC->EPS[Ieps].Wheel_I = 0.0;
         AC->EPS[Ieps].Wheel_V = 0.0;
      }
      
      // Torque Rod MTRs
      if (EPS->MTR_Switch) {
         tempVoltage = EPS->MTR_V + (EPS->MTR_V * EPS->VoltageNoisePercentage * GaussianRandom(RNG));
         for(i=0;i<S->Nmtb;i++){
            MTB = &S->MTB[i];
            TorqueUsed = MAGV(MTB->Trq);
            PercentUse = fabs(TorqueUsed) / MTB->Mmax;
            MtrCurrentSingle = EPS->MTR_I * PercentUse; // MTR current doesn't have noise since it is assumed to be tightly controlled
            MtrCurrent += MtrCurrentSingle;
         }
         // Total MTR Current and Voltage
         PowerUsed += MtrCurrent * tempVoltage;
         AC->EPS[Ieps].MTR_I = MtrCurrent;
         AC->EPS[Ieps].MTR_V = tempVoltage;
      } else {
         AC->EPS[Ieps].MTR_I = 0.0;
         AC->EPS[Ieps].MTR_V = 0.0;
      }

      /*Consumption by the PAYLOAD*/
      if (EPS->PAYLOAD_Switch) {
         tempCurrent = EPS->PAYLOAD_I + (EPS->PAYLOAD_I * EPS->CurrentNoisePercentage * GaussianRandom(RNG));
         tempVoltage = EPS->PAYLOAD_V + (EPS->PAYLOAD_V * EPS->VoltageNoisePercentage * GaussianRandom(RNG));
         PowerUsed += tempVoltage * tempCurrent;
         AC->EPS[Ieps].PAYLOAD_I = tempCurrent;
         AC->EPS[Ieps].PAYLOAD_V = tempVoltage;
      } else {
         AC->EPS[Ieps].PAYLOAD_V = 0.0;
         AC->EPS[Ieps].PAYLOAD_I = 0.0;
      }
      

      /* Solar Array Power Generation */
      long Isolarr;
      struct SolarArrayType *SolArr;
      for(Isolarr=0;Isolarr<S->Nsolarr;Isolarr++) {
         SolArr = &S->SolArr[Isolarr];
         PowerGenerated += SolArr->PowerOut;
      }

      /* Power Balance */
      BatteryPower = PowerGenerated - PowerUsed;
      if (BatteryPower > 0) {
         // Charging
         EPS->BatteryEnergy += (BatteryPower * EPS->BatteryChargeEfficiency)*DTSIM; // J = W*s
      } else {
         // Discharge
         EPS->BatteryEnergy += (BatteryPower/EPS->RegulatorEfficiency)*DTSIM;
      }
      EPS->BatteryEnergy = Limit(EPS->BatteryEnergy, 0, EPS->BatteryCapacity * 3600);

      StateOfCharge = EPS->BatteryEnergy / (EPS->BatteryCapacity * 3600);

      // Make State of Charge Mistake Painful
      // if (StateOfCharge < 20) {
      //    AC->EPS[Ieps].PAYLOAD_Switch = 0;
      //    AC->EPS[Ieps].PAYLOAD_I = 0;
      //    AC->EPS[Ieps].PAYLOAD_V = 0;
      //    AC->EPS[Ieps].StarTracker_Switch = 0;
      //    AC->EPS[Ieps].StarTracker_I = 0;
      //    AC->EPS[Ieps].StarTracker_V = 0;
      //    AC->EPS[Ieps].FSS_Switch = 0;
      //    AC->EPS[Ieps].FSS_I = 0;
      //    AC->EPS[Ieps].FSS_V = 0;
      // }

      // Battery Voltage from Look Up Table
      EPS->BatteryVoltage = LinInterp(lithium_ion_soc_table, lithium_ion_voltage_table, StateOfCharge * 100, lithium_ion_table_size);
   
      // Battery Outputs
      AC->EPS[Ieps].BATTERY_SOC = StateOfCharge;
      AC->EPS[Ieps].BATTERY_V = EPS->BatteryVoltage;
      AC->EPS[Ieps].BATTERY_I = BatteryPower / EPS->BatteryVoltage;

      AC->EPS[Ieps].Wheel_Switch = EPS->Wheel_Switch;
      AC->EPS[Ieps].IMU_Switch = EPS->IMU_Switch;
      AC->EPS[Ieps].StarTracker_Switch = EPS->StarTracker_Switch;
      AC->EPS[Ieps].MTR_Switch = EPS->MTR_Switch;
      AC->EPS[Ieps].CSS_Switch = EPS->CSS_Switch;
      AC->EPS[Ieps].FSS_Switch = EPS->FSS_Switch;
      AC->EPS[Ieps].ADCS_Switch = EPS->ADCS_Switch;
      AC->EPS[Ieps].COM_Switch = EPS->COM_Switch;
      AC->EPS[Ieps].PAYLOAD_Switch = EPS->PAYLOAD_Switch;
   }
}

/**********************************************************************/
/* This model credit Luis F Llano, Software Dev Cromulence LLC 2021   */
void SolArrModel(struct SCType *S)
{
   struct SolarArrayType *SolArr;
   struct AcType *AC;
   long Isolarr; 
   double SoA;
   double SunAngle;
   double svb[3];
      
   for(Isolarr=0;Isolarr<S->Nsolarr;Isolarr++) {
      SolArr = &S->SolArr[Isolarr];
      AC = &S->AC;

      if (S->Eclipse) {
         SolArr->Valid = FALSE;
         SolArr->PowerOut = 0.0; 
      }
      else {
         MxV(S->B[SolArr->Body].CN, S->svn, svb);
         SoA = VoV(svb,SolArr->Axis);
         SunAngle = acos(SoA);
         if (SoA > SolArr->CosAng) {
            /* Sun within FOV */
            SolArr->Valid = TRUE;
            SolArr->PowerOut = SOLAR_IRRADIANCE * SolArr->SysEffLoss * SoA * SolArr->AreaCells;
         }        
         else {
            /* Sun not in FOV */
            SolArr->Valid = FALSE;
            SolArr->PowerOut = 0.0;   
         }
      }
      // SolArr->Voltage = 12000.0; /*milliVolts to keep them all in the same units*/
      SolArr->CurrentOut = SolArr->PowerOut / SolArr->Voltage; 
      /* Copy into AC structure */
      AC->SolArr[Isolarr].Valid = SolArr->Valid;       
      AC->SolArr[Isolarr].PowerOut = SolArr->PowerOut;
      AC->SolArr[Isolarr].CurrentOut = SolArr->CurrentOut;
      if (SolArr->Valid) {
         AC->SolArr[Isolarr].Voltage = SolArr->Voltage;
      } else {
         AC->SolArr[Isolarr].Voltage = 0.0;
      }
      
   }
}

/**********************************************************************/
/*  This function is called at the simulation rate.  Sub-sampling of  */
/*  sensors should be done on a case-by-case basis.                   */
void Sensors(struct SCType *S)
{

   double evn[3],evb[3];
   long i,j,k,DOF;
   struct AcType *AC;
   struct JointType *G;
   struct EPSType *EPS;
   
   AC = &S->AC;
   long Ieps;

   /* Taking care of on/off commands from cosmos*/
   for(Ieps=0;Ieps<S->Neps;Ieps++) {
      EPS = &S->EPS[Ieps];
      /* ADCS */   
      if (AC->EPS[Ieps].ADCS_Switch == 0) {
         EPS->ADCS_Switch = 0;
      } else if (AC->EPS->ADCS_Switch == 1) {
         EPS->ADCS_Switch = 1;
      }  
      /* Wheel */   
      if (!(AC->EPS[Ieps].Wheel_Switch)) {
         EPS->Wheel_Switch = 0;
      } else if (AC->EPS->Wheel_Switch) {
         EPS->Wheel_Switch = 1;
      }
      /* IMU */   
      if (AC->EPS[Ieps].IMU_Switch == 0) {
         EPS->IMU_Switch = 0;
      } else if (AC->EPS->IMU_Switch == 1) {
         EPS->IMU_Switch = 1;
      }
      /* StarTracker */   
      if (AC->EPS[Ieps].StarTracker_Switch == 0) {
         EPS->StarTracker_Switch = 0;
      } else if (AC->EPS->StarTracker_Switch == 1) {
         EPS->StarTracker_Switch = 1;
      }
      /* MTR */   
      if (AC->EPS[Ieps].MTR_Switch == 0) {
         EPS->MTR_Switch = 0;
      } else if (AC->EPS->MTR_Switch == 1) {
         EPS->MTR_Switch = 1;
      }
      /* CSS */   
      if (AC->EPS[Ieps].CSS_Switch == 0) {
         EPS->CSS_Switch = 0;
      } else if (AC->EPS->CSS_Switch == 1) {
         EPS->CSS_Switch = 1;
      }
      /* FSS */   
      if (AC->EPS[Ieps].FSS_Switch == 0) {
         EPS->FSS_Switch = 0;
      } else if (AC->EPS->FSS_Switch == 1) {
         EPS->FSS_Switch = 1;
      }
      /* COM */   
      if (AC->EPS[Ieps].COM_Switch == 0) {
         EPS->COM_Switch = 0;
      } else if (AC->EPS->COM_Switch == 1) {
         EPS->COM_Switch = 1;
      }
      /* PAYLOAD */   
      if (AC->EPS[Ieps].PAYLOAD_Switch == 0) {
         EPS->PAYLOAD_Switch = 0;
      } else if (AC->EPS->PAYLOAD_Switch == 1) {
         EPS->PAYLOAD_Switch = 1;
      }

   }

   /* ADCS off switch behavior */
   for(Ieps=0;Ieps<S->Neps;Ieps++) {
      EPS = &S->EPS[Ieps];
      if (EPS->ADCS_Switch == 0) {
         EPS->Wheel_Switch = 0;
         EPS->IMU_Switch = 0;
         EPS->StarTracker_Switch = 0;
         EPS->MTR_Switch = 0;
         EPS->CSS_Switch = 0;
         EPS->FSS_Switch = 0;
         // EPS->PAYLOAD_Switch = 0;
      }
   }
   /* Ephemeris */
   AC->EphValid = 1;
   for(i=0;i<3;i++) {
      AC->svn[i] = S->svn[i];
      AC->bvn[i] = S->bvn[i];
   }

   /* Accelerometer */
   if (S->Nacc > 0) {
      if (S->Neps == 0 ||  S->EPS->IMU_Switch == ON) {
         AccelerometerModel(S);
      }
   }

   /* Gyro */
   if (S->Ngyro == 0) {
      for (i=0;i<3;i++) AC->wbn[i] = S->B[0].wn[i];
   }
   else {
      if (S->Neps == 0 || S->EPS->IMU_Switch == ON) {
         GyroModel(S);
         /* IMU Quat Standin */
         double tempMat[3][3];
         double imuQB[4];
         for (i=0; i<3; i++) {
            for (j=0; j<3; j++) {
               tempMat[i][j] = AC->Gyro[i].Axis[j];
            }
         }

         C2Q(tempMat, imuQB);
         
         QxQT(S->B[0].qn, imuQB, AC->qbn);
      } 
   }
   
   /* Magnetometer */
   if (Orb[S->RefOrb].World == EARTH) {
      AC->MagValid = TRUE;
      if (S->Nmag == 0) {
         for(i=0;i<3;i++) AC->bvb[i] = S->bvb[i];
      }
      else {
         if (S->Neps == 0 || S->EPS->IMU_Switch == ON) {
            MagnetometerModel(S);
         } 
      }
   }
   else {
      AC->MagValid = FALSE;
   }
   
   /* Sun Sensors */
   if (S->Ncss == 0 && S->Nfss == 0) {
      if (S->Eclipse){
         AC->SunValid = FALSE;
      }
      else {
         AC->SunValid = TRUE;
         MxV(S->B[0].CN,S->svn,AC->svb);
      }
   }
   if (S->Ncss > 0) {
      if (S->Neps == 0 || S->EPS->CSS_Switch == ON) {
         CssModel(S);
      }
   }
   if (S->Nfss > 0) {
      if (S->Neps == 0 || S->EPS->FSS_Switch == ON) {
         FssModel(S);
      } 
   }
   
   /* Star Tracker */
   // if (S->Nst == 0) {
   //    for (i=0;i<4;i++) {
   //       AC->qbn[i] = S->B[0].qn[i];
   //    }
   // }
   // else {
   //    if (S->Neps == 0 || S->EPS->StarTracker_Switch == ON) {
   //       StarTrackerModel(S);
   //    }
   // }
   if (S->Nst > 0) {
      if (S->Neps == 0 || S->EPS->StarTracker_Switch == ON) {
         StarTrackerModel(S);
      }
      for (i=0;i<4;i++) {
         AC->qbn[i] = S->B[0].qn[i];
      }
   }


   /* GPS Receiver (or ephem model) */
   if (S->Ngps == 0) {
      AC->Time = DynTime;
      for(i=0;i<3;i++) {
         AC->PosN[i] = S->PosN[i];
         AC->VelN[i] = S->VelN[i];
      }
   }
   else {
      GpsModel(S);
   }
   
   /* Earth Sensor */
   for (i=0;i<3;i++) evn[i] = -S->PosN[i];
   UNITV(evn);
   MxV(S->B[0].CN,evn,evb);
   if (evb[2] > 0.866) {
      AC->ES.Valid = TRUE;
      AC->ES.Roll = evb[1];
      AC->ES.Pitch = -evb[0];
   }
   else {
      AC->ES.Valid = FALSE;
      AC->ES.Roll = 0.0;
      AC->ES.Pitch = 0.0;
   }

   /* Gimbal Angles */
   for (i=0;i<AC->Ng;i++) {
      G = &S->G[i];
      DOF = AC->G[i].RotDOF;
      for (j=0;j<DOF;j++) {
         AC->G[i].Ang[j] = G->Ang[j];
         AC->G[i].AngRate[j] = G->AngRate[j];
      }
      for(j=0;j<3;j++) {
         for(k=0;k<3;k++) {
            AC->G[i].COI[j][k] = G->COI[j][k];
         }
      }
      DOF = AC->G[i].TrnDOF;
      for (j=0;j<DOF;j++) {
         AC->G[i].Pos[j] = G->Pos[j];
         AC->G[i].PosRate[j] = G->PosRate[j];
      }
   }

   /* Wheel Tachs */
   for (i=0;i<S->Nw;i++) {
      AC->Whl[i].H = S->Whl[i].H;
      AC->Whl[i].w = S->Whl[i].w;
   }

   SolArrModel(S);
   EPSModel(S);
      
//      /* Formation Sensors */
//      for (i=0;i<3;i++) {
//         for (j=0;j<3;j++) FSW->CSF[i][j] = S->CF[i][j];
//         FSW->PosF[i] = S->PosF[i];
//         FSW->VelF[i] = S->VelF[i];
//      }

}

/* #ifdef __cplusplus
** }
** #endif
*/
