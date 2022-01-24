/* Note:  This file was autogenerated by Database/JsonToIPC.py */
/*        See Database/Readme.txt for details. */
/*        If you are hand-modifying this file, keep in mind that */
/*        your work will be lost if you run JsonToIPC.py again. */

#include <stdlib.h>
#include <math.h>
#include "AcTypes.h"
#include "mathkit.h"
#include "dcmkit.h"
#include "fswkit.h"
#include "iokit.h"
#include "timekit.h"

/**********************************************************************/
void WriteToSocket(SOCKET Socket, struct AcType *AC)
{

      long Isc,i;
      char AckMsg[5] = "Ack\n";
      char Msg[16384];
      long MsgLen = 0;
      long LineLen;
      char line[512];

      Isc = AC->ID;

      for(i=0;i<AC->Ng;i++) {
         sprintf(line,"SC[%ld].AC.G[%ld].Cmd.Ang = %18.12le %18.12le %18.12le\n",
            Isc,i,
            AC->G[i].Cmd.Ang[0],
            AC->G[i].Cmd.Ang[1],
            AC->G[i].Cmd.Ang[2]);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

      }

      for(i=0;i<AC->Nwhl;i++) {
         sprintf(line,"SC[%ld].AC.Whl[%ld].Tcmd = %18.12le\n",
            Isc,i,
            AC->Whl[i].Tcmd);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

      }

      for(i=0;i<AC->Nmtb;i++) {
         sprintf(line,"SC[%ld].AC.MTB[%ld].Mcmd = %18.12le\n",
            Isc,i,
            AC->MTB[i].Mcmd);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

      }

      for(i=0;i<AC->Nthr;i++) {
         sprintf(line,"SC[%ld].AC.Thr[%ld].PulseWidthCmd = %18.12le\n",
            Isc,i,
            AC->Thr[i].PulseWidthCmd);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

      }

      sprintf(line,"SC[%ld].AC.Cmd.Ang = %18.12le %18.12le %18.12le\n",
         Isc,
         AC->Cmd.Ang[0],
         AC->Cmd.Ang[1],
         AC->Cmd.Ang[2]);
      LineLen = strlen(line);
      memcpy(&Msg[MsgLen],line,LineLen);
      MsgLen += LineLen;
      if (AC->EchoEnabled) printf("%s",line);

      if (AC->ParmDumpEnabled) {
         sprintf(line,"SC[%ld].AC.ID = %ld\n",
            Isc,
            AC->ID);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.EchoEnabled = %ld\n",
            Isc,
            AC->EchoEnabled);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.ParmLoadEnabled = %ld\n",
            Isc,
            AC->ParmLoadEnabled);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.ParmDumpEnabled = %ld\n",
            Isc,
            AC->ParmDumpEnabled);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.Nb = %ld\n",
            Isc,
            AC->Nb);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.Ng = %ld\n",
            Isc,
            AC->Ng);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.Nwhl = %ld\n",
            Isc,
            AC->Nwhl);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.Nmtb = %ld\n",
            Isc,
            AC->Nmtb);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.Nthr = %ld\n",
            Isc,
            AC->Nthr);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.Ncmg = %ld\n",
            Isc,
            AC->Ncmg);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.Ngyro = %ld\n",
            Isc,
            AC->Ngyro);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.Nmag = %ld\n",
            Isc,
            AC->Nmag);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.Ncss = %ld\n",
            Isc,
            AC->Ncss);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.Nfss = %ld\n",
            Isc,
            AC->Nfss);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.Nst = %ld\n",
            Isc,
            AC->Nst);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.Ngps = %ld\n",
            Isc,
            AC->Ngps);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.Nacc = %ld\n",
            Isc,
            AC->Nacc);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.Neps = %ld\n",
            Isc,
            AC->Neps);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.Nsolarr = %ld\n",
            Isc,
            AC->Nsolarr);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.mass = %18.12le\n",
            Isc,
            AC->mass);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.cm = %18.12le %18.12le %18.12le\n",
            Isc,
            AC->cm[0],
            AC->cm[1],
            AC->cm[2]);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.MOI = %18.12le %18.12le %18.12le %18.12le %18.12le %18.12le %18.12le %18.12le %18.12le\n",
            Isc,
            AC->MOI[0][0],
            AC->MOI[0][1],
            AC->MOI[0][2],
            AC->MOI[1][0],
            AC->MOI[1][1],
            AC->MOI[1][2],
            AC->MOI[2][0],
            AC->MOI[2][1],
            AC->MOI[2][2]);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         for(i=0;i<AC->Nb;i++) {
            sprintf(line,"SC[%ld].AC.B[%ld].mass = %18.12le\n",
               Isc,i,
               AC->B[i].mass);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.B[%ld].cm = %18.12le %18.12le %18.12le\n",
               Isc,i,
               AC->B[i].cm[0],
               AC->B[i].cm[1],
               AC->B[i].cm[2]);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.B[%ld].MOI = %18.12le %18.12le %18.12le %18.12le %18.12le %18.12le %18.12le %18.12le %18.12le\n",
               Isc,i,
               AC->B[i].MOI[0][0],
               AC->B[i].MOI[0][1],
               AC->B[i].MOI[0][2],
               AC->B[i].MOI[1][0],
               AC->B[i].MOI[1][1],
               AC->B[i].MOI[1][2],
               AC->B[i].MOI[2][0],
               AC->B[i].MOI[2][1],
               AC->B[i].MOI[2][2]);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

         }

         for(i=0;i<AC->Ng;i++) {
            sprintf(line,"SC[%ld].AC.G[%ld].IsUnderActiveControl = %ld\n",
               Isc,i,
               AC->G[i].IsUnderActiveControl);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.G[%ld].IsSpherical = %ld\n",
               Isc,i,
               AC->G[i].IsSpherical);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.G[%ld].RotDOF = %ld\n",
               Isc,i,
               AC->G[i].RotDOF);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.G[%ld].TrnDOF = %ld\n",
               Isc,i,
               AC->G[i].TrnDOF);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.G[%ld].RotSeq = %ld\n",
               Isc,i,
               AC->G[i].RotSeq);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.G[%ld].TrnSeq = %ld\n",
               Isc,i,
               AC->G[i].TrnSeq);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.G[%ld].CGiBi = %18.12le %18.12le %18.12le %18.12le %18.12le %18.12le %18.12le %18.12le %18.12le\n",
               Isc,i,
               AC->G[i].CGiBi[0][0],
               AC->G[i].CGiBi[0][1],
               AC->G[i].CGiBi[0][2],
               AC->G[i].CGiBi[1][0],
               AC->G[i].CGiBi[1][1],
               AC->G[i].CGiBi[1][2],
               AC->G[i].CGiBi[2][0],
               AC->G[i].CGiBi[2][1],
               AC->G[i].CGiBi[2][2]);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.G[%ld].CBoGo = %18.12le %18.12le %18.12le %18.12le %18.12le %18.12le %18.12le %18.12le %18.12le\n",
               Isc,i,
               AC->G[i].CBoGo[0][0],
               AC->G[i].CBoGo[0][1],
               AC->G[i].CBoGo[0][2],
               AC->G[i].CBoGo[1][0],
               AC->G[i].CBoGo[1][1],
               AC->G[i].CBoGo[1][2],
               AC->G[i].CBoGo[2][0],
               AC->G[i].CBoGo[2][1],
               AC->G[i].CBoGo[2][2]);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.G[%ld].AngGain = %18.12le %18.12le %18.12le\n",
               Isc,i,
               AC->G[i].AngGain[0],
               AC->G[i].AngGain[1],
               AC->G[i].AngGain[2]);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.G[%ld].AngRateGain = %18.12le %18.12le %18.12le\n",
               Isc,i,
               AC->G[i].AngRateGain[0],
               AC->G[i].AngRateGain[1],
               AC->G[i].AngRateGain[2]);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.G[%ld].PosGain = %18.12le %18.12le %18.12le\n",
               Isc,i,
               AC->G[i].PosGain[0],
               AC->G[i].PosGain[1],
               AC->G[i].PosGain[2]);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.G[%ld].PosRateGain = %18.12le %18.12le %18.12le\n",
               Isc,i,
               AC->G[i].PosRateGain[0],
               AC->G[i].PosRateGain[1],
               AC->G[i].PosRateGain[2]);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.G[%ld].MaxAngRate = %18.12le %18.12le %18.12le\n",
               Isc,i,
               AC->G[i].MaxAngRate[0],
               AC->G[i].MaxAngRate[1],
               AC->G[i].MaxAngRate[2]);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.G[%ld].MaxPosRate = %18.12le %18.12le %18.12le\n",
               Isc,i,
               AC->G[i].MaxPosRate[0],
               AC->G[i].MaxPosRate[1],
               AC->G[i].MaxPosRate[2]);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.G[%ld].MaxTrq = %18.12le %18.12le %18.12le\n",
               Isc,i,
               AC->G[i].MaxTrq[0],
               AC->G[i].MaxTrq[1],
               AC->G[i].MaxTrq[2]);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.G[%ld].MaxFrc = %18.12le %18.12le %18.12le\n",
               Isc,i,
               AC->G[i].MaxFrc[0],
               AC->G[i].MaxFrc[1],
               AC->G[i].MaxFrc[2]);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

         }

         for(i=0;i<AC->Ngyro;i++) {
            sprintf(line,"SC[%ld].AC.Gyro[%ld].Axis = %18.12le %18.12le %18.12le\n",
               Isc,i,
               AC->Gyro[i].Axis[0],
               AC->Gyro[i].Axis[1],
               AC->Gyro[i].Axis[2]);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

         }

         for(i=0;i<AC->Nmag;i++) {
            sprintf(line,"SC[%ld].AC.MAG[%ld].Axis = %18.12le %18.12le %18.12le\n",
               Isc,i,
               AC->MAG[i].Axis[0],
               AC->MAG[i].Axis[1],
               AC->MAG[i].Axis[2]);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

         }

         for(i=0;i<AC->Ncss;i++) {
            sprintf(line,"SC[%ld].AC.CSS[%ld].Channel = %ld\n",
               Isc,i,
               AC->CSS[i].Channel);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.CSS[%ld].Body = %ld\n",
               Isc,i,
               AC->CSS[i].Body);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.CSS[%ld].Calibrated = %ld\n",
               Isc,i,
               AC->CSS[i].Calibrated);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.CSS[%ld].Axis = %18.12le %18.12le %18.12le\n",
               Isc,i,
               AC->CSS[i].Axis[0],
               AC->CSS[i].Axis[1],
               AC->CSS[i].Axis[2]);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.CSS[%ld].Scale = %18.12le\n",
               Isc,i,
               AC->CSS[i].Scale);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.CSS[%ld].IllumBackground = %18.12le\n",
               Isc,i,
               AC->CSS[i].IllumBackground);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

         }

         for(i=0;i<AC->Nfss;i++) {
            sprintf(line,"SC[%ld].AC.FSS[%ld].qb = %18.12le %18.12le %18.12le %18.12le\n",
               Isc,i,
               AC->FSS[i].qb[0],
               AC->FSS[i].qb[1],
               AC->FSS[i].qb[2],
               AC->FSS[i].qb[3]);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.FSS[%ld].CB = %18.12le %18.12le %18.12le %18.12le %18.12le %18.12le %18.12le %18.12le %18.12le\n",
               Isc,i,
               AC->FSS[i].CB[0][0],
               AC->FSS[i].CB[0][1],
               AC->FSS[i].CB[0][2],
               AC->FSS[i].CB[1][0],
               AC->FSS[i].CB[1][1],
               AC->FSS[i].CB[1][2],
               AC->FSS[i].CB[2][0],
               AC->FSS[i].CB[2][1],
               AC->FSS[i].CB[2][2]);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

         }

         for(i=0;i<AC->Nst;i++) {
            sprintf(line,"SC[%ld].AC.ST[%ld].qb = %18.12le %18.12le %18.12le %18.12le\n",
               Isc,i,
               AC->ST[i].qb[0],
               AC->ST[i].qb[1],
               AC->ST[i].qb[2],
               AC->ST[i].qb[3]);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.ST[%ld].CB = %18.12le %18.12le %18.12le %18.12le %18.12le %18.12le %18.12le %18.12le %18.12le\n",
               Isc,i,
               AC->ST[i].CB[0][0],
               AC->ST[i].CB[0][1],
               AC->ST[i].CB[0][2],
               AC->ST[i].CB[1][0],
               AC->ST[i].CB[1][1],
               AC->ST[i].CB[1][2],
               AC->ST[i].CB[2][0],
               AC->ST[i].CB[2][1],
               AC->ST[i].CB[2][2]);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

         }

         for(i=0;i<AC->Nacc;i++) {
            sprintf(line,"SC[%ld].AC.Accel[%ld].PosB = %18.12le %18.12le %18.12le\n",
               Isc,i,
               AC->Accel[i].PosB[0],
               AC->Accel[i].PosB[1],
               AC->Accel[i].PosB[2]);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.Accel[%ld].Axis = %18.12le %18.12le %18.12le\n",
               Isc,i,
               AC->Accel[i].Axis[0],
               AC->Accel[i].Axis[1],
               AC->Accel[i].Axis[2]);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

         }

         for(i=0;i<AC->Nwhl;i++) {
            sprintf(line,"SC[%ld].AC.Whl[%ld].Axis = %18.12le %18.12le %18.12le\n",
               Isc,i,
               AC->Whl[i].Axis[0],
               AC->Whl[i].Axis[1],
               AC->Whl[i].Axis[2]);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.Whl[%ld].DistVec = %18.12le %18.12le %18.12le\n",
               Isc,i,
               AC->Whl[i].DistVec[0],
               AC->Whl[i].DistVec[1],
               AC->Whl[i].DistVec[2]);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.Whl[%ld].Tmax = %18.12le\n",
               Isc,i,
               AC->Whl[i].Tmax);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.Whl[%ld].Hmax = %18.12le\n",
               Isc,i,
               AC->Whl[i].Hmax);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

         }

         for(i=0;i<AC->Nmtb;i++) {
            sprintf(line,"SC[%ld].AC.MTB[%ld].Axis = %18.12le %18.12le %18.12le\n",
               Isc,i,
               AC->MTB[i].Axis[0],
               AC->MTB[i].Axis[1],
               AC->MTB[i].Axis[2]);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.MTB[%ld].DistVec = %18.12le %18.12le %18.12le\n",
               Isc,i,
               AC->MTB[i].DistVec[0],
               AC->MTB[i].DistVec[1],
               AC->MTB[i].DistVec[2]);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.MTB[%ld].Mmax = %18.12le\n",
               Isc,i,
               AC->MTB[i].Mmax);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

         }

         for(i=0;i<AC->Nthr;i++) {
            sprintf(line,"SC[%ld].AC.Thr[%ld].PosB = %18.12le %18.12le %18.12le\n",
               Isc,i,
               AC->Thr[i].PosB[0],
               AC->Thr[i].PosB[1],
               AC->Thr[i].PosB[2]);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.Thr[%ld].Axis = %18.12le %18.12le %18.12le\n",
               Isc,i,
               AC->Thr[i].Axis[0],
               AC->Thr[i].Axis[1],
               AC->Thr[i].Axis[2]);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.Thr[%ld].rxA = %18.12le %18.12le %18.12le\n",
               Isc,i,
               AC->Thr[i].rxA[0],
               AC->Thr[i].rxA[1],
               AC->Thr[i].rxA[2]);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

            sprintf(line,"SC[%ld].AC.Thr[%ld].Fmax = %18.12le\n",
               Isc,i,
               AC->Thr[i].Fmax);
            LineLen = strlen(line);
            memcpy(&Msg[MsgLen],line,LineLen);
            MsgLen += LineLen;
            if (AC->EchoEnabled) printf("%s",line);

         }

         sprintf(line,"SC[%ld].AC.FsCtrl.Kunl = %18.12le\n",
            Isc,
            AC->FsCtrl.Kunl);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.FsCtrl.wc = %18.12le\n",
            Isc,
            AC->FsCtrl.wc);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.FsCtrl.amax = %18.12le\n",
            Isc,
            AC->FsCtrl.amax);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.FsCtrl.vmax = %18.12le\n",
            Isc,
            AC->FsCtrl.vmax);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.FsCtrl.FrameTgt = %ld\n",
            Isc,
            AC->FsCtrl.FrameTgt);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.FsCtrl.qtr = %18.12le %18.12le %18.12le %18.12le\n",
            Isc,
            AC->FsCtrl.qtr[0],
            AC->FsCtrl.qtr[1],
            AC->FsCtrl.qtr[2],
            AC->FsCtrl.qtr[3]);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.FsCtrl.qtn = %18.12le %18.12le %18.12le %18.12le\n",
            Isc,
            AC->FsCtrl.qtn[0],
            AC->FsCtrl.qtn[1],
            AC->FsCtrl.qtn[2],
            AC->FsCtrl.qtn[3]);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.FsCtrl.qbl = %18.12le %18.12le %18.12le %18.12le\n",
            Isc,
            AC->FsCtrl.qbl[0],
            AC->FsCtrl.qbl[1],
            AC->FsCtrl.qbl[2],
            AC->FsCtrl.qbl[3]);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.FsCtrl.qimu = %18.12le %18.12le %18.12le %18.12le\n",
            Isc,
            AC->FsCtrl.qimu[0],
            AC->FsCtrl.qimu[1],
            AC->FsCtrl.qimu[2],
            AC->FsCtrl.qimu[3]);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.FsCtrl.SunYaw = %18.12le\n",
            Isc,
            AC->FsCtrl.SunYaw);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.FsCtrl.SunYawTgt = %18.12le\n",
            Isc,
            AC->FsCtrl.SunYawTgt);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.FsCtrl.SunYawDeadband = %18.12le\n",
            Isc,
            AC->FsCtrl.SunYawDeadband);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.FsCtrl.SunYawRateDeadband = %18.12le\n",
            Isc,
            AC->FsCtrl.SunYawRateDeadband);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

         sprintf(line,"SC[%ld].AC.FsCtrl.SunYawError = %18.12le\n",
            Isc,
            AC->FsCtrl.SunYawError);
         LineLen = strlen(line);
         memcpy(&Msg[MsgLen],line,LineLen);
         MsgLen += LineLen;
         if (AC->EchoEnabled) printf("%s",line);

      }

      sprintf(line,"[EOF]\n\n");
      if (AC->EchoEnabled) printf("%s",line);

      LineLen = strlen(line);
      memcpy(&Msg[MsgLen],line,LineLen);
      MsgLen += LineLen;
      send(Socket,Msg,strlen(Msg),0);

      /* Wait for Ack */
      recv(Socket,AckMsg,5,0);
}