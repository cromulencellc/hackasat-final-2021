

#include "FlatSatFsw.h"

/**
 * _FLATSAT_HARDWARE_FSW_ is used for CFS Builds
 * This is defined when building in opensatkit but not 42
 * Credits to Stoneking, Eric, {\it 42: A General-Purpose Spacecraft Simulation}, NASA Software Designation GSC-16720-1, 2010-2019.  https://sourceforge.net/projects/fortytwospacecraftsimulation, https://github.com/ericstoneking/42.
 * for algorithms and sensor models.
*/

#if defined(_FLATSAT_SIM_FSW_) && defined(_FLATSAT_HARDWARE_FSW_)
#error "Only __FLATSAT_SIM_FSW_ or _FLATSAT_HARDWARE_FSW_ can be defined"
#endif

#if defined(_FLATSAT_SIM_FSW_) || defined(_FLATSAT_HARDWARE_FSW_)
#include "adcs_hw_cfg.h"
#include "adcs_impl.h"
ADCS_IMPL_Class* ADCSImpl;
#endif

#ifdef _FLATSAT_STANDALONE_FSW_
#ifdef _ENABLE_MQTT_
void WriteToMqtt(MQTTClient client, struct AcType *AC, const char* topic);
void ReadFromMqtt(const char* Msg, size_t MsgLen, struct AcType *AC);
#else
extern void ReadFromSocket(SOCKET Socket, struct AcType *AC);
extern void WriteToSocket(SOCKET Socket, struct AcType *AC);
#endif
#endif

static const double xAxis[3] = {1.0, 0.0, 0.0};
static const double mxAxis[3] = {-1.0, 0.0, 0.0};
static const double yAxis[3] =  {0.0, 1.0, 0.0};
static const double myAxis[3] =  {0.0, -1.0, 0.0};
static const double zAxis[3] = {0.0, 0.0, 1.0};
static const double mzAxis[3] = {0.0, 0.0, -1.0};
static FILE* outfile = NULL;

#ifdef _FLATSAT_STANDALONE_FSW_
#ifdef _ENABLE_MQTT_
static int Port = 1883;
MQTTClient client = NULL;
#else
static SOCKET Socket;
static int Port = 10001;
#endif
#endif

#if defined(_FLATSAT_SIM_FSW_) || defined(_FLATSAT_HARDWARE_FSW_)
void FlatSatFswSetAdcsImpl(ADCS_IMPL_Class* ADCSImplInput) {
   ADCSImpl = ADCSImplInput;
}
#endif
/**********************************************************************/
/* This function copies needed parameters from the SC structure to    */
/* the AC structure.  This is a crude first pass.  It only allocates  */
/* memory for the structures, and counts on the data to be filled in  */
/* via messages.                                                      */
void FlatSatFswAllocateAC(struct AcType *AC)
{

   /* Clear Memory */
   memset(AC, 0, sizeof(struct AcType));

   /* Bodies */
   AC->Nb = 1;
   if (AC->Nb > 0) {
      AC->B = (struct AcBodyType *) calloc(AC->Nb,sizeof(struct AcBodyType));
   }

   /* Joints */
   AC->Ng = 0;
   if (AC->Ng > 0) {
      AC->G = (struct AcJointType *) calloc(AC->Ng,sizeof(struct AcJointType));
   }
   
   /* Wheels */
   AC->Nwhl = 3;
   if (AC->Nwhl > 0) {
      AC->Whl = (struct AcWhlType *) calloc(AC->Nwhl,sizeof(struct AcWhlType));
   }

   /* Magnetic Torquer Bars */
   AC->Nmtb = 3;
   if (AC->Nmtb > 0) {
      AC->MTB = (struct AcMtbType *) calloc(AC->Nmtb,sizeof(struct AcMtbType));
   }

   /* Thrusters */
   AC->Nthr = 0;
   if (AC->Nthr > 0) {
      AC->Thr = (struct AcThrType *) calloc(AC->Nthr,sizeof(struct AcThrType));
   }
   
   /* Control Moment Gyros */

   /* Gyro Axes */
   AC->Ngyro = 3;
   if (AC->Ngyro > 0) {
      AC->Gyro = (struct AcGyroType *) calloc(AC->Ngyro,sizeof(struct AcGyroType));
   }

   /* Magnetometer Axes */
   AC->Nmag = 3;
   if (AC->Nmag > 0) {
      AC->MAG = (struct AcMagnetometerType *) calloc(AC->Nmag,sizeof(struct AcMagnetometerType));
   }

   /* Coarse Sun Sensors */
   AC->Ncss = 6;
   if (AC->Ncss > 0) {
      AC->CSS = (struct AcCssType *) calloc(AC->Ncss,sizeof(struct AcCssType));
   }
   
   /* Fine Sun Sensors */
   // _ADCS_CHALLENGE_
   AC->Nfss = 1;
   if (AC->Nfss > 0) {
      AC->FSS = (struct AcFssType *) calloc(AC->Nfss,sizeof(struct AcFssType));
   }

   /* Star Trackers */
   AC->Nst = 1;
   if (AC->Nst > 0) {
      AC->ST = (struct AcStarTrackerType *) calloc(AC->Nst,sizeof(struct AcStarTrackerType));
   }

   /* GPS */
   AC->Ngps = 0;
   if (AC->Ngps > 0) {
      AC->GPS = (struct AcGpsType *) calloc(AC->Ngps,sizeof(struct AcGpsType)); 
   }     
   
   /* Accelerometer Axes */
   AC->Nacc = 3;
   if (AC->Nacc > 0) {
      AC->Accel = (struct AcAccelType *) calloc(AC->Nacc,sizeof(struct AcAccelType)); 
   }

   /* Analog Monitor */
   AC->Nanalog = 3;
   if (AC->Nanalog > 0) {
      AC->Analog = (struct AcAnalogMonitorType *) calloc(AC->Nanalog, sizeof(struct AcAnalogMonitorType));
   }

   /* EPS SIM ONLY */
   #ifdef _FLATSAT_STANDALONE_FSW_
   AC->Neps = 1;
   if (AC->Neps > 0) {
      AC->EPS = (struct AcEPSType *) calloc(AC->Neps, sizeof(struct AcEPSType));
   }
   AC->Nsolarr = 1;
   if (AC->Nsolarr > 0) {
      AC->SolArr = (struct AcSolarArrayType *) calloc(AC->Nsolarr, sizeof(struct AcSolarArrayType));
   }
   #endif

   memset(&AC->FsCtrl, 0, sizeof(AC->FsCtrl));
}

/**********************************************************************/
void FlatSatFswInitAC(struct AcType *AC)
{
   struct AcFlatSatCtrlType *C  = &AC->FsCtrl;
   AC->Init = 1;
   AC->ID = 0;
   /* Controllers */
   C->Init = 1;  
   C->CtrlMode = FSW_CTRL_DISABLED;
   C->MtbMode = FSW_MTB_DISABLED;
   AC->DT = 0.2; // Delta time between steps

   if (C->Debug == 1) {
      outfile = fopen("FswResults.txt", "w");
   }

   int i;

   double MOI[3][3] = {
      {0.065, 0, 0.0},
      {0.0, 0.085, 0.0},
      {0.0, 0.0, 0.03}
   };

   AC->mass = 9.0;
   memcpy(&AC->MOI, &MOI, sizeof(MOI));

   // Wheel characteristics
   for (i=0; i<AC->Nwhl; i++) {
      AC->Whl[i].Hmax = 30.6e-3;
      AC->Whl[i].Tmax = 2.3e-3;
      AC->Whl[i].J = 4.77465e-05; // Momentum storage (@6000 RPM): 30.0 mNms ... H = I*w
   }

   // Torque Rod Characteristics
   for(i=0; i<AC->Nmtb; i++) {
      AC->MTB[i].Mmax = 1.3387293996;
   }

   // Set Axis
   for (i=0; i<3; i++) {
      // Wheel
      AC->Whl[0].Axis[i] = xAxis[i];
      AC->Whl[1].Axis[i] = yAxis[i];
      AC->Whl[2].Axis[i] = zAxis[i];
      // Torque Rods
      AC->MTB[0].Axis[i] = xAxis[i];
      AC->MTB[1].Axis[i] = yAxis[i];
      AC->MTB[2].Axis[i] = zAxis[i];
      // Gyro
      AC->Gyro[0].Axis[i] = yAxis[i];
      AC->Gyro[1].Axis[i] = xAxis[i];
      AC->Gyro[2].Axis[i] = mzAxis[i];
      // Magnetnometer
      AC->MAG[0].Axis[i] = yAxis[i];
      AC->MAG[1].Axis[i] = xAxis[i];
      AC->MAG[2].Axis[i] = mzAxis[i];
      // CSS
      AC->CSS[0].Axis[i] = xAxis[i];
      AC->CSS[1].Axis[i] = yAxis[i];
      AC->CSS[2].Axis[i] = mxAxis[i];
      AC->CSS[3].Axis[i] = myAxis[i];
      AC->CSS[4].Axis[i] = mzAxis[i];
      AC->CSS[5].Axis[i] = zAxis[i];
      // Accel
      AC->Accel[0].Axis[i] = yAxis[i];
      AC->Accel[1].Axis[i] = xAxis[i];
      AC->Accel[2].Axis[i] = mzAxis[i];
   }

   // Set  DistVec;
   for(i=0; i<3; i++) {
      AC->Whl[0].DistVec[i] =  AC->Whl[0].Axis[i];
      AC->Whl[1].DistVec[i] =  AC->Whl[1].Axis[i];
      AC->Whl[2].DistVec[i] =  AC->Whl[2].Axis[i];
      AC->MTB[0].DistVec[i] =  AC->MTB[0].Axis[i];
      AC->MTB[1].DistVec[i] =  AC->MTB[1].Axis[i];
      AC->MTB[2].DistVec[i] =  AC->MTB[2].Axis[i];
   }
   // Set CSS Scale Value
   for (i = 0; i<AC->Ncss; i++) {
      AC->CSS[i].Scale = 0.9;
   }
   #ifdef _FLATSAT_HARDWARE_FSW_
   AC->CSS[0].Channel = CSS_YAW_0_XADC_IDX;
   AC->CSS[1].Channel = CSS_YAW_90_XADC_IDX;
   AC->CSS[2].Channel = CSS_YAW_180_XADC_IDX;
   AC->CSS[3].Channel = CSS_YAW_270_XADC_IDX;
   AC->CSS[4].Channel = CSS_Z_MINUS_XADC_IDX;
   AC->CSS[5].Channel = CSS_Z_XADC_IDX;
   #endif

   // Set Accelerometer Position
   // -0.044  0.012  -0.1
   for (i=0; i<3; i++) {
      AC->Accel[i].PosB[0] = -0.044;
      AC->Accel[i].PosB[1] = 0.012;
      AC->Accel[i].PosB[2] = -0.1;
   }

   // Setup Analog Monitor
   #if defined(_FLATSAT_HARDWARE_FSW_) || defined(_FLATSAT_SIM_FSW_)
   AC->Analog[0].ConversionScale = MONITOR_5V_SCALE;
   AC->Analog[0].UpperLimit = 5.2;
   AC->Analog[0].LowerLimit = 4.8;
   AC->Analog[0].Channel = MONITOR_5V_XADC_IDX;

   AC->Analog[1].ConversionScale = MONITOR_3V3_SCALE;
   AC->Analog[1].UpperLimit = 3.4;
   AC->Analog[1].LowerLimit = 3.2;
   AC->Analog[1].Channel = MONITOR_3V3_XADC_IDX;

   AC->Analog[2].ConversionScale = MONITOR_TEMP_SCALE;
   AC->Analog[2].UpperLimit = 100.0;
   AC->Analog[2].LowerLimit = 20.0;
   AC->Analog[2].Channel = MONITOR_TEMP_XADC_IDX;
   #endif

   // Star Tracker Body Mounting
   // double CB[3][3];
   A2C(213, 0, 180*D2R, 0, AC->ST[0].CB);
   C2Q(AC->ST[0].CB, AC->ST->qb);


   // Default Controls Params
   C->amax = 0.1;
   C->vmax = 1 * D2R;
   C->wc = 0.1*TWOPI;

   #ifdef _FLATSAT_STANDALONE_FSW_
      #ifdef _ENABLE_MQTT_
      client = InitMqttClient(hostname, Port, "42_FlatSatFsw", (void*) AC);
      StartMqttClient(client, MQTT_USER, MQTT_PASS, "42_MQTT_PUB");
      #else
      Socket = InitSocketClient(hostname,Port,1);
      if (Socket < 0) {
         ADCS_LOG_INFO("Waiting for 42 Server");
         return Status;
         // OS_TaskDelay(100);
      }
      #endif
   #endif

}

/******************************************************************************/
/* This function assumes FSS FOVs don't overlap, and FSS overwrites CSS */
void FssProcessing(struct AcType *AC)
{
      struct AcFssType *FSS;
      double tanx,tany,z;
      long Ifss,i;
      
      for(Ifss=0;Ifss<AC->Nfss;Ifss++) {
         FSS = &AC->FSS[Ifss];
         if (FSS->Valid) {
            AC->SunValid = 1;
            tanx = tan(FSS->SunAng[0]);
            tany = tan(FSS->SunAng[1]);
            z = 1.0/sqrt(1.0+tanx*tanx+tany*tany);            
            FSS->SunVecS[0] = z*tanx;
            FSS->SunVecS[1] = z*tany;
            FSS->SunVecS[2] = z;
            MTxV(FSS->CB,FSS->SunVecS,FSS->SunVecB);
            for(i=0;i<3;i++) AC->svb[i] = FSS->SunVecB[i];
         }
      }
}

/**********************************************************************/
/*  Some Simple Sensor Processing Functions                           */
/*  corresponding to the Sensor Models in 42sensors.c                 */
/*  Note!  These are simple, sometimes naive.  Use with care.         */
/**********************************************************************/

void FsGyroProcessing(struct AcType *AC)
{
      // printf("Gyro processing\n");
      struct AcGyroType *G;
      struct AcFlatSatCtrlType *C  = &AC->FsCtrl;
      double A0xA1[3];
      double A[3][3],b[3],Ai[3][3];
      double AtA[3][3] = {{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}};
      double Atb[3] = {0.0,0.0,0.0};
      double AtAi[3][3];
      long Ig,i,j;

   if (C->ImuGyroValid) {
      // #ifndef _FLATSAT_HARDWARE_FSW_
      //    for(i=0; i<3;i++) {
      //       AC->wbn[i] = AC->wbn[i];
      //    }
      //    return;
      // #endif
      if (AC->Ngyro == 0) {
         /* AC->wbn populated by true S->B[0].wn in 42sensors.c */
      }
      else if (AC->Ngyro == 1) {
         G = &AC->Gyro[0];
         for(i=0;i<3;i++) AC->wbn[i] = G->Rate*G->Axis[i];
      }
      else if (AC->Ngyro == 2) {
         VxV(AC->Gyro[0].Axis,AC->Gyro[1].Axis,A0xA1);
         for(i=0;i<3;i++) {
            A[0][i] = AC->Gyro[0].Axis[i];
            A[1][i] = AC->Gyro[1].Axis[i];
            A[2][i] = A0xA1[i]; 
         }
         b[0] = AC->Gyro[0].Rate;
         b[1] = AC->Gyro[1].Rate;
         b[2] = 0.0;
         MINV3(A,Ai);
         if (Ai == NULL) {
            #if defined(_FLATSAT_SIM_FSW_) || defined(_FLATSAT_HARDWARE_FSW_)
            CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_FSW_EID, CFE_EVS_ERROR, "GYRO Matrix Inversion Not Valid");
            #endif
            return;
         }
         MxV(Ai,b,AC->wbn);
      }
      else if (AC->Ngyro > 2) {
         /* Normal Equations */
         for(Ig=0;Ig<AC->Ngyro;Ig++) {
            G = &AC->Gyro[Ig];
            for(i=0;i<3;i++) {
               Atb[i] += G->Rate*G->Axis[i];
               for(j=0;j<3;j++) {
                  AtA[i][j] += G->Axis[i]*G->Axis[j];
               }
            }
         }
         MINV3(AtA,AtAi);
         if (AtAi == NULL) {
            #if defined(_FLATSAT_SIM_FSW_) || defined(_FLATSAT_HARDWARE_FSW_)
            CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_FSW_EID, CFE_EVS_ERROR, "GYRO Matrix Inversion Not Valid");
            #endif
            return;
         }
         MxV(AtAi,Atb,AC->wbn);
      }
   }
}

/**********************************************************************/
void FsMagnetometerProcessing(struct AcType *AC)
{
      // printf("Magnetometer processing\n");
      struct AcMagnetometerType *M;
      struct AcFlatSatCtrlType *C  = &AC->FsCtrl;
      double A0xA1[3];
      double A[3][3],b[3],Ai[3][3];
      double AtA[3][3] = {{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}};
      double Atb[3] = {0.0,0.0,0.0};
      double AtAi[3][3];
      long Im,i,j;

   if(C->ImuMagnetometerValid) {
      if (AC->Nmag == 0) {
         /* AC->bvb populated by true S->bvb in 42sensors.c */
      }
      else if (AC->Nmag == 1) {
         M = &AC->MAG[0];
         for(i=0;i<3;i++) AC->bvb[i] = M->Field*M->Axis[i];
      }
      else if (AC->Nmag == 2) {
         VxV(AC->MAG[0].Axis,AC->MAG[1].Axis,A0xA1);
         for(i=0;i<3;i++) {
            A[0][i] = AC->MAG[0].Axis[i];
            A[1][i] = AC->MAG[1].Axis[i];
            A[2][i] = A0xA1[i]; 
         }
         b[0] = AC->MAG[0].Field;
         b[1] = AC->MAG[1].Field;
         b[2] = 0.0;
         MINV3(A,Ai);
         if (Ai == NULL) {
            #if defined(_FLATSAT_SIM_FSW_) || defined(_FLATSAT_HARDWARE_FSW_)
            CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_FSW_EID, CFE_EVS_ERROR, "Magnetometer Matrix Inversion Not Valid");
            #endif
            return;
         }
         MxV(Ai,b,AC->bvb);
      }
      else if (AC->Nmag > 2) {
         /* Normal Equations */
         for(Im=0;Im<AC->Nmag;Im++) {
            M = &AC->MAG[Im];
            for(i=0;i<3;i++) {
               Atb[i] += M->Field*M->Axis[i];
               for(j=0;j<3;j++) {
                  AtA[i][j] += M->Axis[i]*M->Axis[j];
               }
            }
         }
         MINV3(AtA,AtAi);
         if (AtAi == NULL) {
            #if defined(_FLATSAT_SIM_FSW_) || defined(_FLATSAT_HARDWARE_FSW_)
            CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_FSW_EID, CFE_EVS_ERROR, "Magnetometer Matrix Inversion Not Valid");
            #endif
            return;
         }
         MxV(AtAi,Atb,AC->bvb);
      }
   }
}

/**********************************************************************/
void FsAccelProcessing(struct AcType *AC)
{
   // printf("Acceleromter processing\n");
   struct AcFlatSatCtrlType *C  = &AC->FsCtrl;
   struct AcAccelType *Accel;
   double A0xA1[3];
   double A[3][3],b[3],Ai[3][3];
   double AtA[3][3] = {{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}};
   double Atb[3] = {0.0,0.0,0.0};
   double AtAi[3][3];
   long Ig,i,j;

   if(C->ImuAccelerationValid) {
      if (AC->Nacc == 0) {
         /* No accelerometer processing */
      }
      else if (AC->Nacc == 1) {
         Accel = &AC->Accel[0];
         for(i=0;i<3;i++) AC->AccN[i] = Accel->Acc*Accel->Axis[i];
      }
      else if (AC->Nacc == 2) {
         VxV(AC->Accel[0].Axis,AC->Accel[1].Axis,A0xA1);
         for(i=0;i<3;i++) {
            A[0][i] = AC->Accel[0].Axis[i];
            A[1][i] = AC->Accel[1].Axis[i];
            A[2][i] = A0xA1[i]; 
         }
         b[0] = AC->Accel[0].Acc;
         b[1] = AC->Accel[1].Acc;
         b[2] = 0.0;
         MINV3(A,Ai);
         if (Ai == NULL) {
            #if defined(_FLATSAT_SIM_FSW_) || defined(_FLATSAT_HARDWARE_FSW_)
            CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_FSW_EID, CFE_EVS_ERROR, "Acceleromter Matrix Inversion Not Valid");
            #endif
            return;
         }
         MxV(Ai,b,AC->AccN);
      }
      else if (AC->Nacc > 2) {
         /* Normal Equations */
         for(Ig=0;Ig<AC->Nacc;Ig++) {
            Accel = &AC->Accel[Ig];
            for(i=0;i<3;i++) {
               Atb[i] += Accel->Acc*Accel->Axis[i];
               for(j=0;j<3;j++) {
                  AtA[i][j] += Accel->Axis[i]*Accel->Axis[j];
               }
            }
         }
         MINV3(AtA,AtAi);
         if (AtAi == NULL) {
            #if defined(_FLATSAT_SIM_FSW_) || defined(_FLATSAT_HARDWARE_FSW_)
            CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_FSW_EID, CFE_EVS_ERROR, "Acceleromter Matrix Inversion Not Valid");
            #endif
            return;
         }
         MxV(AtAi,Atb,AC->AccN);
      }
   } 
   // TODO - Come up with else case for when Accelerometer is bad

}


/**********************************************************************/
#ifdef _FLATSAT_HARDWARE_FSW_
int32_t FsImuProcessing(struct AcType *AC, ADCS_IMU_Class *IMU) {
   struct AcFlatSatCtrlType *C  = &AC->FsCtrl;

   // TODO - Need way to check that IMU is getting updated data. Check counters and if missed counter goes up too fast then reset?

   if (IMU->Device.active) {
      // Gyro Data
      AC->Gyro[0].Rate = IMU->Data[ADCS_IMU_GYRO_IDX].sensorValue.un.gyroscope.x;
      AC->Gyro[1].Rate = IMU->Data[ADCS_IMU_GYRO_IDX].sensorValue.un.gyroscope.y;
      AC->Gyro[2].Rate = IMU->Data[ADCS_IMU_GYRO_IDX].sensorValue.un.gyroscope.z;
      if (IMU->Data[ADCS_IMU_GYRO_IDX].sensorValue.status > 0) {
         C->ImuGyroValid = true;
      }

      // Magmetometer Data
      AC->MAG[0].Field = IMU->Data[ADCS_IMU_MAG_IDX].sensorValue.un.magneticField.x;
      AC->MAG[1].Field = IMU->Data[ADCS_IMU_MAG_IDX].sensorValue.un.magneticField.y;
      AC->MAG[2].Field = IMU->Data[ADCS_IMU_MAG_IDX].sensorValue.un.magneticField.z;
      if (IMU->Data[ADCS_IMU_MAG_IDX].sensorValue.status > 0) {
         C->ImuMagnetometerValid = true;
      }


      // Accelerometer Data
      AC->Accel[0].Acc = IMU->Data[ADCS_IMU_ACC_IDX].sensorValue.un.accelerometer.x;
      AC->Accel[1].Acc = IMU->Data[ADCS_IMU_ACC_IDX].sensorValue.un.accelerometer.y;
      AC->Accel[2].Acc = IMU->Data[ADCS_IMU_ACC_IDX].sensorValue.un.accelerometer.z;
      if (IMU->Data[ADCS_IMU_ACC_IDX].sensorValue.status > 0) {
         C->ImuAccelerationValid = true;
      }

         // IMU State
      if (C->UseImuState) {
         C->qimu[0] = IMU->Data[ADCS_IMU_STATE_IDX].sensorValue.un.rotationVector.i;
         C->qimu[1] = IMU->Data[ADCS_IMU_STATE_IDX].sensorValue.un.rotationVector.j;
         C->qimu[2] = IMU->Data[ADCS_IMU_STATE_IDX].sensorValue.un.rotationVector.k;
         C->qimu[3] = IMU->Data[ADCS_IMU_STATE_IDX].sensorValue.un.rotationVector.real;

         if (IMU->Data[ADCS_IMU_STATE_IDX].sensorValue.status > 0) {
            C->ImuStateValid = true;
         }
         if (C->Debug) {
            ADCS_LOG_INFO("Updating IMU State. Valid %d, qimu = [%f, %f, %f, %f]", C->ImuStateValid, C->qimu[0], C->qimu[1], C->qimu[2], C->qimu[3]);
         }
      }

   } else {
      C->ImuAccelerationValid = false;
      C->ImuGyroValid = false;
      C->ImuMagnetometerValid = false;
      C->ImuStateValid = false;
   }

   if(!(C->ImuGyroValid && C->ImuMagnetometerValid && C->ImuAccelerationValid) || !IMU->Device.active) {
      C->ImuError = true;
   }
}
#endif
/**********************************************************************/
void FsUseImuState (struct AcType *AC) {
   struct AcFlatSatCtrlType *C  = &AC->FsCtrl;
      if (C->UseImuState && C->ImuStateValid) {
         double tempMat[3][3];
         double qTemp[4];
         for (int i=0; i<3; i++) {
            for (int j=0; j<3; j++) {
               tempMat[i][j] = AC->Gyro[i].Axis[j];
            }
         }
         C2Q(tempMat, qTemp);
         QxQT(C->qimu, qTemp, AC->qbn);
         if (C->Debug) {
            ADCS_LOG_INFO("Updating QBN With IMU State. Qimu (%f, %f, %f, %f), QBN (%f, %f, %f, %f)",
               C->qimu[0], C->qimu[1], C->qimu[2], C->qimu[3],
               AC->qbn[0], AC->qbn[1], AC->qbn[2], AC->qbn[3]);
         }
   }
}

/**********************************************************************/
#if defined(_FLATSAT_HARDWARE_FSW_) || defined(_FLATSAT_SIM_FSW_)
void FsAnalogMonitorProcessing(struct AcType *AC, ADCS_XADC_Class *XADC) {
   // struct AcFlatSatCtrlType *C  = &AC->FsCtrl;
   for (int i=0; i<AC->Nanalog; i++) {
      AC->Analog[i].Value = XADC->Device.channels[AC->Analog[i].Channel].converted * AC->Analog[i].ConversionScale;
   }
   if(AC->Analog[0].Value >= AC->Analog[0].UpperLimit) {
      CFE_EVS_SendEvent(ADCS_IMPL_ERROR_FSW_EID, CFE_EVS_ERROR, "5V monitor over-voltage detected. 5V Measured: %f", AC->Analog[0].Value);
      AC->Analog[0].Error = 1;
   }
   if(AC->Analog[0].Value <= AC->Analog[0].LowerLimit) {
      CFE_EVS_SendEvent(ADCS_IMPL_ERROR_FSW_EID, CFE_EVS_ERROR, "5V monitor under-voltage detected. 5V Measured: %f", AC->Analog[0].Value);
      AC->Analog[0].Error = 2;
   }
   if(AC->Analog[1].Value >= AC->Analog[1].UpperLimit) {
      CFE_EVS_SendEvent(ADCS_IMPL_ERROR_FSW_EID, CFE_EVS_ERROR, "3.3V monitor over-voltage detected. 3.3V Measured: %f", AC->Analog[1].Value);
      AC->Analog[1].Error = 1;
   }
   if(AC->Analog[1].Value <= AC->Analog[1].LowerLimit) {
      CFE_EVS_SendEvent(ADCS_IMPL_ERROR_FSW_EID, CFE_EVS_ERROR, "3.3V monitor under-voltage detected. 3.3V Measured: %f", AC->Analog[1].Value);
      AC->Analog[1].Error = 2;
   }
   if(AC->Analog[2].Value >= AC->Analog[2].UpperLimit) {
      CFE_EVS_SendEvent(ADCS_IMPL_ERROR_FSW_EID, CFE_EVS_ERROR, "Temp monitor over-temp detected. Temp Measured: %f", AC->Analog[2].Value);
      AC->Analog[2].Error = 1;
   }
   if(AC->Analog[2].Value <= AC->Analog[2].LowerLimit) {
      CFE_EVS_SendEvent(ADCS_IMPL_ERROR_FSW_EID, CFE_EVS_ERROR, "Temp monitor under-temp detected. Temp Measured: %f", AC->Analog[2].Value);
      AC->Analog[2].Error = 2;
   }
}
#endif

/**********************************************************************/
#ifdef _FLATSAT_HARDWARE_FSW_
double XadcCssAnalogToIllumination(double miliVolts, double scale) {
   return (scale - miliVolts/1000);
}
#endif

/**********************************************************************/
#ifdef _FLATSAT_HARDWARE_FSW_
void FsCssCalibration(struct AcType *AC, ADCS_XADC_Class *XADC) {

   struct AcFlatSatCtrlType *C = &AC->FsCtrl;
   int i;
   if (C->CalibrateCss) {
      CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_FSW_EID, CFE_EVS_INFORMATION, "Starting CSS Calibration. Will use %d loop data", CSS_CAL_MEASUREMENT_COUNT);
      C->CalibrateCss = false;
      C->CssCalCounter = 0;
      int len = sizeof(double *) * NUM_CSS_SENSORS + sizeof(double) * NUM_CSS_SENSORS * CSS_CAL_MEASUREMENT_COUNT; 
      C->IllumCalArray = (double **)malloc(len);
      double *ptr = (double *)(C->IllumCalArray + NUM_CSS_SENSORS); 
      for(i = 0; i < NUM_CSS_SENSORS; i++) {
         C->IllumCalArray[i] = (ptr + CSS_CAL_MEASUREMENT_COUNT * i); 
      }
   }
   if (C->CssCalCounter < CSS_CAL_MEASUREMENT_COUNT) {
      for(i=0; i<NUM_CSS_SENSORS; i++) {
         C->IllumCalArray[i][C->CssCalCounter] = XadcCssAnalogToIllumination(XADC->Device.channels[AC->CSS[i].Channel].converted, AC->CSS[i].Scale);
      }
      C->CssCalCounter++;
   } else {
      for(i=0; i<NUM_CSS_SENSORS; i++) {
         AC->CSS[i].IllumBackground = array_mean(C->IllumCalArray[i], CSS_CAL_MEASUREMENT_COUNT);
         AC->CSS[i].Calibrated = true;
      }
      C->CssCalCounter = 0;
      free(C->IllumCalArray);
      CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_FSW_EID, CFE_EVS_INFORMATION, "CSS Calibration Completed after %d FSW loops", CSS_CAL_MEASUREMENT_COUNT);
   }

}
#endif

/**********************************************************************/
#ifdef _FLATSAT_HARDWARE_FSW_
void FsCssUpdate(struct AcType *AC, ADCS_XADC_Class *XADC) {
      // printf("CSS processing\n");
   struct AcFlatSatCtrlType *C = &AC->FsCtrl;
   if (XADC->Device.active) {
      if(C->CalibrateCss || C->CssCalCounter > 0) {
         FsCssCalibration(AC, XADC);
      }
      for (int i=0; i<AC->Ncss; i++) {
         AC->CSS[i].Illum = XadcCssAnalogToIllumination(XADC->Device.channels[AC->CSS[i].Channel].converted, AC->CSS[i].Scale);
         // ADCS_LOG_INFO("CSS Illum Idx %d: %f", i, AC->CSS[i].Illum);
         if ( (AC->CSS[i].Illum > (AC->CSS[i].IllumBackground + 0.05*AC->CSS[i].Scale)) && (AC->CSS[i].Illum > 1e-3) ) {
            AC->CSS[i].Valid = true;
            // ADCS_LOG_INFO("CSS Idx %d Valid", i);
         } else {
            AC->CSS[i].Valid = false;
            // ADCS_LOG_INFO("CSS Idx %d Invalid", i);
         }
      }
      // AC->CSS[0].Illum = XadcCssAnalogToIllumination(XADC->Device.channels[CSS_YAW_0_XADC_IDX].converted, AC->CSS[0].Scale);
      // AC->CSS[1].Illum = XadcCssAnalogToIllumination(XADC->Device.channels[CSS_YAW_90_XADC_IDX].converted, AC->CSS[1].Scale);
      // AC->CSS[2].Illum = XadcCssAnalogToIllumination(XADC->Device.channels[CSS_YAW_180_XADC_IDX].converted, AC->CSS[2].Scale);
      // AC->CSS[3].Illum = XadcCssAnalogToIllumination(XADC->Device.channels[CSS_YAW_270_XADC_IDX].converted, AC->CSS[3].Scale);
      // AC->CSS[4].Illum = XadcCssAnalogToIllumination(XADC->Device.channels[CSS_Z_MINUS_XADC_IDX].converted, AC->CSS[4].Scale);
   }
}
#endif

/**********************************************************************/
void FsCssProcessing(struct AcType *AC)
{

   struct AcCssType *Css;
   struct AcFlatSatCtrlType *C  = &AC->FsCtrl;
   double AtA[3][3] = {{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}};
   double Atb[3] = {0.0,0.0,0.0};
   double AtAi[3][3];
   double A[2][3],b[2];
   long Ic,i,j;
   long Nvalid = 0;
   double InvalidSVB[3] = {1.0,0.0,0.0}; /* Safe vector if SunValid == FALSE */

   if (AC->Ncss == 0) {
      /* AC->svb populated by true S->svb in 42sensors.c */
   }
   else {
      for(Ic=0;Ic<AC->Ncss;Ic++) {
         Css = &AC->CSS[Ic];
         if (Css->Valid) {
            Nvalid++;
            /* Normal equations, assuming Nvalid will end up > 2 */
            for(i=0;i<3;i++) {
               Atb[i] += Css->Axis[i]*Css->Illum/Css->Scale;
               
               for(j=0;j<3;j++) {
                  AtA[i][j] += Css->Axis[i]*Css->Axis[j];
               }
            }
            /* In case Nvalid ends up == 2 */
            for(i=0;i<3;i++) {
               A[0][i] = A[1][i];
               A[1][i] = Css->Axis[i];
            }
            b[0] = b[1];
            b[1] = Css->Illum/Css->Scale;
         }
      }
      if (Nvalid > 2) {
         AC->SunValid = TRUE;
         MINV3(AtA,AtAi);
         if(AtAi != NULL) {
            MxV(AtAi,Atb,AC->svb);
            UNITV(AC->svb);
         } else {
            #if defined(_FLATSAT_SIM_FSW_) || defined(_FLATSAT_HARDWARE_FSW_)
            CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_FSW_EID, CFE_EVS_ERROR, "Sun Vector Matrix Inversion Not Valid");
            #else
            ADCS_LOG_ERROR("Sun Vector Matrix Inversion Not Valid");
            #endif
            AC->SunValid = FALSE;
         }
      }
      else if (Nvalid == 2) {
         AC->SunValid = TRUE;
         for(i=0;i<3;i++) AC->svb[i] = b[0]*A[0][i] + b[1]*A[1][i];
         UNITV(AC->svb);
      }
      else if (Nvalid == 1) {
         AC->SunValid = TRUE;
         for(i=0;i<3;i++) AC->svb[i] = Atb[i];
         UNITV(AC->svb);
      }
      else {
         AC->SunValid = FALSE;
         for(i=0;i<3;i++) AC->svb[i] = InvalidSVB[i];
      }
   }

   if (AC->SunValid) {
      C->SunYaw = fmod(atan2(AC->svb[1], AC->svb[0]), TWOPI);
      if(C->SunYaw < 0) {
         C->SunYaw += TWOPI;
      }
   } else {
      #if defined(_FLATSAT_SIM_FSW_) || defined(_FLATSAT_HARDWARE_FSW_)
      CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_FSW_EID, CFE_EVS_DEBUG, "Sun Vector Not Valid");
      #else
      ADCS_LOG_ERROR("Sun Vector Not Valid");
      #endif
   }
}

/**********************************************************************/
#ifdef _FLATSAT_HARDWARE_FSW_
int32_t FsKdstProcessing(struct AcType *AC, ADCS_ST_Class *ST) {
   int32_t Status = -1;
   struct AcFlatSatCtrlType *C  = &AC->FsCtrl;
   C->StError = TRUE;
   static double DCM[3][3];
   static double angles[3];

   if(AC->Nst > 0 && ST->Active && kdst_is_active(ST->Kdst)) {
      if(ST->UseFakeSt && C->UseImuState) {
         memset(DCM, 0, sizeof(DCM));
         memset(angles, 0, sizeof(angles));
         Q2C(AC->qbn, DCM);
         C2A(213, DCM, &angles[0], &angles[1], &angles[2]);
         if (C->Debug) {
            ADCS_LOG_INFO("Suggest KDST Data. QBN(%f, %f, %f, %f) Angles(%f, %f, %f)",
               AC->qbn[0], AC->qbn[1], AC->qbn[2], AC->qbn[3],
               angles[0], angles[1], angles[2]);
         }
         if(kdst_update_udp_suggester(ST->Kdst, angles)) {
            OS_TaskDelay(100);
            if(kdst_update(ST->Kdst)) {
               st_data_t *cur_st_data = kdst_get_st_data_ptr(ST->Kdst);
               memset(DCM, 0, sizeof(DCM));
               memcpy(&angles, &ST->StData->cur_position.angles, sizeof(angles));
               A2C(213, angles[0], angles[1], angles[2], DCM);
               C2Q(DCM, AC->ST->qn);
               if(C->Debug) {
                  ADCS_LOG_INFO("KDST TLM Updated. Angles In (%f, %f, %f), Angles Out (%f, %f, %f)",
                  ST->StData->cur_position.angles[0], ST->StData->cur_position.angles[1], ST->StData->cur_position.angles[2],
                  angles[0], angles[1], angles[2]);
               }
               C->StError = FALSE;
               Status = CFE_SUCCESS;
            } else {
               ADCS_LOG_ERROR("KDST Update failed");
            }
         } else {
            ADCS_LOG_ERROR("KDST UDP Suggester Failed");
         }
      } else {
         ADCS_LOG_ERROR("ST and IMU Not Enabled");
      }
   }
   // else {
   //    ADCS_LOG_ERROR("ST is not active");
   // };

   return Status;
}
#endif


/**********************************************************************/
/* TODO: Weight measurements to reduce impact of "weak" axis */
void FsStarTrackerProcessing(struct AcType *AC)
{
      // printf("ST processing\n");

      long Ist,i;
      struct AcStarTrackerType *ST;
      struct AcFlatSatCtrlType *C  = &AC->FsCtrl;
      long Nvalid = 0;
      double qbn[4];
      
      if (AC->Nst == 0 || C->StError == TRUE) {
         /* AC->qbn populated by true S->B[0].qn in 42sensors.c */
         AC->StValid = FALSE;
      }
      else {
         /* Naive averaging */
         for(Ist=0;Ist<AC->Nst;Ist++) {
            ST = &AC->ST[Ist];
            if (ST->Valid) {
               for(i=0;i<4;i++) AC->qbn[i] = 0.0;
               Nvalid++;
               QTxQ(ST->qb,ST->qn,qbn);
               RECTIFYQ(qbn);
               for(i=0;i<4;i++) AC->qbn[i] = qbn[i];
            }
         }
         if (Nvalid > 0) {
            AC->StValid = TRUE;
            UNITQ(AC->qbn);
         }
         else {
            AC->StValid = FALSE;
            AC->qbn[3] = 1.0;
         }
      }
}

/**********************************************************************/
void FsGpsProcessing(struct AcType *AC)
{
      // printf("GPS processing\n");

      struct AcGpsType *G;
      double DaysSinceWeek,DaysSinceRollover,DaysSinceEpoch,JD;
      long i;

      if (AC->Ngps == 0) {
         /* AC->Time, AC->PosN, AC->VelN */
         /* populated in 42sensors.c */
      }
      else {
         G = &AC->GPS[0];
         /* GPS Time is seconds since 6 Jan 1980 00:00:00.0, which is JD = 2444244.5 */
         DaysSinceWeek = G->Sec/86400.0;
         DaysSinceRollover = DaysSinceWeek + 7.0*G->Week;
         DaysSinceEpoch = DaysSinceRollover + 7168.0*G->Rollover;
         JD = DaysSinceEpoch + 2444244.5;
         /* AC->Time is seconds since J2000, which is JD = 2451545.0 */
         AC->Time = (JD-2451545.0)*86400.0;

         /* Position, Velocity */
         for(i=0;i<3;i++) {
            AC->PosN[i] = AC->GPS[0].PosN[i];
            AC->VelN[i] = AC->GPS[0].VelN[i];
         }
         AC->EphValid = 1;
      }
}

/**********************************************************************/
#ifdef _FLATSAT_HARDWARE_FSW_
void FsWheelUpdate(struct AcType *AC, ADCS_RW_Class *RW) {
   int i;
   struct AcWhlType *W;
   for (i=0; i<RW->WheelCnt; i++) {
      W = &AC->Whl[i];
      if (RW->Wheel[i].data.active) {
         W->w = (double) cubewheel_get_speed(&RW->Wheel[i]) * RPM2RPS;
         W->H = (double) W->J * W->w;
      } else {
         W->w = 0.0;
         W->H = 0.0;
      }
   }
}
#endif

/**********************************************************************/
/*  End Sensor Processing Functions                                   */
/**********************************************************************/


/**********************************************************************/
/* Actuator Processing Functions                                */
/**********************************************************************/
#ifdef _FLATSAT_HARDWARE_FSW_
void FsWheelCommand(struct AcType *AC, ADCS_RW_Class *RW) {
   struct AcWhlType *W;
   struct AcFlatSatCtrlType *C  = &AC->FsCtrl;
   bool wheelStatus = true;
   long Iw;
   if (C->CtrlMode > FSW_CTRL_DISABLED) {
      for (Iw=0;Iw<RW->WheelCnt;Iw++) {
         if (RW->Wheel[Iw].data.active) { // TODO - Prevent wheel commands on error?
            W = &AC->Whl[Iw];
            float speed_rpm = Limit((float)(W->Wcmd * RPS2RPM), -6000, 6000);
            if (C->Debug) {
               ADCS_LOG_INFO("Send wheel command. Wheel Trq: %10.8f, Wheel Cmd Speed: %10.8f", W->Tcmd, W->Wcmd * RPS2RPM);
            }
            CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_FSW_EID, CFE_EVS_DEBUG, "FS Wheel Command. Idx %d, Wheel Speed: %f", Iw, speed_rpm);
            cubewheel_telecommand_t* tc = cubewheel_tc_set_reference_speed(speed_rpm);
            C->WheelSpeedCmdCounter++;
            wheelStatus = cubewheel_send_tc(&RW->Wheel[Iw], tc);
            if (!wheelStatus) {
               C->WheelError = true;
            }
         }
      }
   }
   // ADCS_LOG_INFO("FsWheelProcessing() Completed");
   // t = delta_wheel_speed * wheel_inertia / delta_t
}
#endif

void ConvertWheelTorquesToRpms(struct AcType *AC) {
   struct AcWhlType *W;
   long Iw;
   double DeltaHw;
   double DeltaWs; // Radians per sec
   double WheelCmdRpm;

   for(Iw=0;Iw<AC->Nwhl;Iw++) {
      W = &AC->Whl[Iw];
      W->Tcmd = Limit(-VoV(AC->Tcmd,W->DistVec),-0.95*W->Tmax, 0.95*W->Tmax);
      DeltaHw = W->Tcmd * AC->DT;
      DeltaWs = DeltaHw / W->J;
      WheelCmdRpm = round((W->w + DeltaWs) * RPS2RPM);
      W->Wcmd = WheelCmdRpm * RPM2RPS;
      W->Tcmd = (W->Wcmd - W->w)*(W->J/AC->DT);
   }
}

void FsWheelProcessing(struct AcType *AC)
{
   struct AcWhlType *W;
   struct AcFlatSatCtrlType *C  = &AC->FsCtrl;
   long Iw;
   // Convert closed loop torques to wheel speed commands
   switch (C->CtrlMode)
   {

   case FSW_CTRL_OPEN_LOOP_WHEEL:
      for(Iw=0;Iw<AC->Nwhl;Iw++) {
         W = &AC->Whl[Iw];
         W->Tcmd = (W->Wcmd - W->w)*(W->J/AC->DT);
      }
      break;

   #ifdef _FLATSAT_SIM_FSW_
   case FSW_CTRL_CLOSED_LOOP_LVLH:
      ConvertWheelTorquesToRpms(AC);
      break;
   #endif
   
   #ifdef _FLATSAT_HARDARE_FSW
   case FSW_CTRL_CLOSED_LOOP_SUN_TRACKING:
      ConvertWheelTorquesToRpms(AC);
      break;
   #endif

   default:
      for(Iw=0;Iw<AC->Nwhl;Iw++) {
         W = &AC->Whl[Iw];
         W->Tcmd = 0;
         W->Wcmd = 0;
      }
      break;
   }
}



/**********************************************************************/
void FsMtbProcessing(struct AcType *AC)
{
      struct AcMtbType *M;
      long Im;
      struct AcFlatSatCtrlType *C  = &AC->FsCtrl;
      if (C->MtbMode == FSW_MTB_AUTO) {
         for(Im=0;Im<AC->Nmtb;Im++) {
            M = &AC->MTB[Im];
            M->Mcmd = Limit(VoV(AC->Mcmd,M->DistVec),-M->Mmax,M->Mmax);
         }
      }
}

/**********************************************************************/
#ifdef _FLATSAT_HARDWARE_FSW_
void FsMtbCommand(struct AcType *AC, ADCS_MTR_Class *MTR) {
   
   // TODO convert Mcmd to duty cycle? Do we need a LPF on H-bridge output? How to do automatic control here?
   CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_FSW_EID, CFE_EVS_DEBUG, "FS Issue MTB Command");
   
}
#endif

/**********************************************************************/
/*  End Actuator Processing Functions                                 */
/**********************************************************************/

/**********************************************************************/
/*  TLM Update Functions                                              */
/**********************************************************************/

#if defined(_FLATSAT_SIM_FSW_) || defined(_FLATSAT_HARDWARE_FSW_)
void FsUpdateFswTelemetry(ADCS_IMPL_Class* ADCSImpl) {
   unsigned int i, j;
   struct AcType *AC = &ADCSImpl->AC;
   struct AcFlatSatCtrlType *C  = &AC->FsCtrl;
   ADCS_IMPL_FswPkt *TLM = &ADCSImpl->FswTlmPkt;
   ADCS_FSS_Class   *FSS = &ADCSImpl->FSS;

   /* Flag Tlm Updates */
   TLM->CtrlMode = (uint8_t) C->CtrlMode;
   TLM->AttitudeSource = (uint8_t) C->AttitudeSource;
   TLM->AttitudeSourceStepCnt = (uint16_t) C->AttitudeSourceStepCnt % USHRT_MAX;
   TLM->ErrorFlags[0] = (uint8_t) C->CtrlLoopError;
   TLM->ErrorFlags[1] = (uint8_t) C->ImuError;
   TLM->ErrorFlags[2] = (uint8_t) C->WheelError;
   TLM->ErrorFlags[3] = (uint8_t) C->MtbError;
   TLM->ErrorFlags[4] = (uint8_t) C->StError;
   TLM->ErrorFlags[5] = (uint8_t) C->GpsError;
   TLM->ErrorFlags[6] = (uint8_t) AC->Analog[0].Error;
   TLM->ErrorFlags[7] = (uint8_t) AC->Analog[1].Error;
   TLM->ErrorFlags[8] = (uint8_t) AC->Analog[2].Error;
   // Make ctrl error flags big endian so all bits are in order
   TLM->MtbMode = C->MtbMode;
   TLM->Flags[0] = (uint8_t) C->Debug;
   TLM->Flags[1] = (uint8_t) C->Init;
   TLM->Flags[2] = (uint8_t) AC->EphValid;
   TLM->Flags[3] = (uint8_t) AC->StValid;  
   TLM->Flags[4] = (uint8_t) AC->SunValid;
   TLM->Flags[5] = (uint8_t) C->ImuStateValid;
   TLM->Flags[6] = (uint8_t) C->UseImuState;
   TLM->Flags[7] = (uint8_t) C->ImuGyroValid;
   TLM->Flags[8] = (uint8_t) C->ImuMagnetometerValid;
   TLM->Flags[9] = (uint8_t) C->ImuAccelerationValid;

   // _ADCS_CHALLENGE_
   #if _FSS_CHALLENGE_ == 1
      TLM->Flags[10] = (uint8_t) C->FssStateValid;
   #else
      TLM->Flags[10] = (uint8_t)0;
   #endif // _FSS_CHALLENGE

   for(i=0; i<ADCSImpl->HW.DeviceCnt; i++) {
      TLM->HwTlmStatus[i] = ADCSImpl->HW.Devices[i].TlmEnabled;
   }

   /* Single Values */
   TLM->Kunl = C->Kunl;
   TLM->Vmax = C->vmax;
   TLM->Amax = C->amax;
   TLM->Wc = C->wc;
   TLM->Ixx = AC->MOI[0][0];
   TLM->Iyy = AC->MOI[1][1];
   TLM->Izz = AC->MOI[2][2];

   /* Quaternion/Vector Updates */
   for (i=0; i<4; i++) {
      TLM->qbr[i] = (float) AC->qbr[i];
      TLM->qbn[i] = (float) AC->qbn[i];
      TLM->qbl[i] = (float) C->qbl[i];
      TLM->qtr[i] = (float) C->qtr[i];
      if (i < 3) {
         TLM->wbn[i] = (float) AC->wbn[i];
         TLM->bvb[i] = (float) AC->bvb[i];
         TLM->wln[i] = (float) AC->wln[i];
         TLM->acc[i] = (float) AC->AccN[i];
         TLM->werr[i] = (float) C->werr[i];
         TLM->svb[i] = (float) AC->svb[i];
         TLM->Hvb[i] = (float) AC->Hvb[i];
      }
   }

   /* Wheel Command Vectors */
   TLM->WheelSpeedCmdCnt = C->WheelSpeedCmdCounter;

   #ifdef _FLATSAT_HARDWARE_FSW_
   TLM->SunYaw = C->SunYaw;
   TLM->SunYawTgt = C->SunYawTgt;
   TLM->SunYawDeadband = C->SunYawDeadband;
   TLM->SunYawRateDeadband = C->SunYawRateDeadband;
   if(AC->Nst > 0 && ADCSImpl->ST.Active && kdst_is_active(ADCSImpl->ST.Kdst)) {
      TLM->StTempMonitor = ADCSImpl->ST.StData->st_temperature;
   }
   for (i=0; i<ADCSImpl->RW.WheelCnt; i++) {
      TLM->Ww[i] = (float) AC->Whl[i].w;
      TLM->WwCmd[i] = (float) AC->Whl[i].Wcmd;
      TLM->Tcmd[i] = (float) AC->Whl[i].Tcmd;
      TLM->Hw[i] = (float) AC->Whl[i].H;
   }
   /* Magnetic Torque Rods */
   for (i=0; i<ADCSImpl->MTR.MtrCount; i++) {
      TLM->Mcmd[i] = (float) AC->MTB[i].Mcmd;
   }
      /* Magnetic Torque Rods */
   for (i=0; i<ADCSImpl->MTR.MtrCount; i++) {
      TLM->Mcmd[i] = (float) AC->MTB[i].Mcmd;
   }

   /* Analog Monitors */
   for (i=0; i<AC->Nanalog; i++) {
      TLM->AnalogMonitor[i] = AC->Analog[i].Value;
   }

   /* CSS */
   for(i=0; i<NUM_CSS_SENSORS; i++) {
      TLM->CssIllum[i] = AC->CSS[i].Illum;
      TLM->CssIllumBackground[i] = AC->CSS[i].IllumBackground;
   }
   
   #endif
   #ifdef _FLATSAT_SIM_FSW_

   for (i=0; i<AC->Nwhl; i++) {
      TLM->Ww[i] = (float) AC->Whl[i].w;
      TLM->WwCmd[i] = (float) AC->Whl[i].Wcmd;
      TLM->Tcmd[i] = (float) AC->Whl[i].Tcmd;
      TLM->Hw[i] = (float) AC->Whl[i].H;
   }

      /* Magnetic Torque Rods */
   for (i=0; i<AC->Nmtb; i++) {
      TLM->Mcmd[i] = (float) AC->MTB[i].Mcmd;
   }

   /* CSS */
   for(i=0; i<AC->Ncss; i++) {
      TLM->CssIllum[i] = AC->CSS[i].Illum;
      // TLM->CssIllumBackground[i] = AC->CSS[i].IllumBackground;
   }

   /* FSS */ 
   // _ADCS_CHALLENGE_
   #if _FSS_CHALLENGE_ == 1
      for(i=0; i<AC->Nfss; i++){
         for(j=0; j<ADCS_FSS_NUM_ANG; j++){
            TLM->FssSunAng[i][j] = AC->FSS[i].SunAng[j];
            FSS->data.SunAng[j]  = AC->FSS[i].SunAng[j];
         }
         for(j=0; j<ADCS_FSS_NUM_VEC; j++){
            FSS->data.SunVecS[j] = AC->FSS[i].SunVecS[j];
            FSS->data.SunVecB[j] = AC->FSS[i].SunVecB[j];
         }
      }
   #else
      for(i=0; i<AC->Nfss; i++){
         for(j=0; j<ADCS_FSS_NUM_ANG; j++){
            TLM->FssSunAng[i][j] = 0;
         }
      }
   #endif // _FSS_CHALLENGE

   /* Analog Monitors */
   for (i=0; i<AC->Nanalog; i++) {
      TLM->AnalogMonitor[i] = AC->Analog[i].Value;
   }

   #endif
   
   // Set J2000 Time
   TLM->CtrlTimeJ2000 = AC->Time;

   // Timestamp and Send Telemetry
   ADCS_LOG_DEBUG("Update ADCS FSW Telemetry");
   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) TLM);
   int32_t Status = CFE_SB_SendMsg((CFE_SB_Msg_t *) TLM);
   if (AC->FsCtrl.Debug) {
      CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_FSW_EID, CFE_EVS_DEBUG, "Send FSW TLM Pkt Status %d", Status);
   }

}
#endif
/**********************************************************************/
/*  End TLM Update Functions                                          */
/**********************************************************************/

/**********************************************************************/
/*  Helper Functions                                                  */
/**********************************************************************/

void EvalLvlhFsw(struct AcType *AC, FILE* outfile) {
   static char msg[256];
   static double Pitch, Roll, Yaw;
   struct AcFlatSatCtrlType *C  = &AC->FsCtrl;

   //static double PitchD, RollD, YawD;
   static double DCM[3][3];
   Q2C(C->qbl, DCM);
   C2A(213, DCM, &Pitch, &Roll, &Yaw);
   // C2A(213, AC->CLN, &PitchD, &RollD, &YawD);
   snprintf(msg, sizeof(msg), "%10.2f,%8.6f,%8.6f,%8.6f,%8.6f,%8.6f,%8.6f,%8.6f,%8.6f,%8.6f,%8.6f,%8.6f,%8.6f,%8.6f,%8.6f,%8.6f,%8.6f,%8.6f,%8.6f\n", 
                              AC->Time, 
                              (C->therr[0])*R2D, (C->therr[1])*R2D, (C->therr[2])*R2D, (C->werr[0])*R2D, (C->werr[0])*R2D, (C->werr[0])*R2D,
                              (Roll)*R2D, (Pitch)*R2D, (Yaw)*R2D, AC->wbn[0]*R2D, AC->wbn[1]*R2D, AC->wbn[2]*R2D, 
                              AC->Whl[0].w*RPS2RPM, AC->Whl[1].w*RPS2RPM, AC->Whl[2].w*RPS2RPM,
                              AC->Whl[0].Tcmd, AC->Whl[1].Tcmd, AC->Whl[2].Tcmd);
   printf("%s", msg);
   if (outfile != NULL) {
      fprintf(outfile, "%s", msg);
   }
}

#ifdef _FLATSAT_HARDWARE_FSW_
void EvalSunYawFsw(struct AcType *AC, FILE* outfile) {
   static char msg[256];
   static const long yawAxis = 2;
   static const long wheelIdx = 0;
   struct AcFlatSatCtrlType *C  = &AC->FsCtrl;
   snprintf(msg, sizeof(msg), "%10.2f,%8.6f,%8.6f,%8.6f,%8.6f,%8.6f,%8.6f\n", 
                                 AC->Time, C->SunYawTgt*R2D, C->SunYaw*R2D, 
                                 C->SunYawError*R2D, C->werr[yawAxis]*R2D, 
                                 AC->Whl[wheelIdx].Tcmd, AC->Whl[wheelIdx].w*RPS2RPM);
   printf("%s", msg);
   if (outfile != NULL) {
      fprintf(outfile, "%s", msg);
   }
}
#endif
/**********************************************************************/
/*  End Helper Functions                                 */
/**********************************************************************/

/**********************************************************************/
/*  Control Law Functions                                             */
/**********************************************************************/
int32_t LvlhControlLaw(struct AcType *AC) {

   struct AcFlatSatCtrlType *C  = &AC->FsCtrl;
   long i;
   // double tempMat[3][3];

   // CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_FSW_EID, CFE_EVS_DEBUG, "Run LVLH Control Law");
   if (C->CtrlModeInit) {
      #if defined(_FLATSAT_SIM_FSW_) || defined(_FLATSAT_HARDWARE_FSW_)
      CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_FSW_EID, CFE_EVS_INFORMATION, "ADCS Initialize LVLH Control Mode");
      #endif
      C->qtr[3] = 1.0;
      C->FrameTgt = FRAME_L;
      C->CtrlModeInit = FALSE;
   }


   if (!AC->StValid) {


   }

   if (C->FrameTgt == FRAME_L) {

      if (C->AttitudeSource > FSW_ATT_SOURCE_NONE ) {
         ADCS_LOG_DEBUG("Running LVLH Ctrl with valid attitude source");

         // Find Inertial to LVLH Rotation
         FindCLN(AC->PosN, AC->VelN, AC->CLN, AC->wln);
         C2Q(AC->CLN, AC->qln);
         RECTIFYQ(AC->qln);

         // Calculate Target to Inertial Quaterion by combinign LVLH target with LVLH to Inertial Rotation
         QTxQ(C->qtr,AC->qln, C->qtn);
         // Rectify Quaterion Target to Inertial
         RECTIFYQ(C->qtn);

         // Calculate Body to LVLH Quaterion
         QxQT(AC->qbn, AC->qln, C->qbl);
         // Rectify Body to LVLH Quaterion
         RECTIFYQ(C->qbl);

         // Calculate Body to Target Quaternion (qbr = q-body-to-requested)
         QxQT(AC->qbn, C->qtn, AC->qbr);
         // Rectify Body to Target Quaternion
         RECTIFYQ(AC->qbr);

         // Calculate state error signal
         Q2AngleVec(AC->qbr,C->therr);
         // printf("Theta Err: [1, 2, 3]: %f:%f:%f\n", C->therr[0]*R2D, C->therr[1]*R2D, C->therr[2]*R2D);

         // Calculate rate error signal
         for(i=0;i<3;i++) {
            C->werr[i] = AC->wbn[i] - AC->wln[i];
         }

         // Calculate Output Controls using Ramp Coast Glide Algorithm
         double alpha[3];
         VectorRampCoastGlide(C->therr,C->werr,C->wc,C->amax,C->vmax,alpha);
         for(i=0;i<3;i++) AC->Tcmd[i] = AC->MOI[i][i]*alpha[i];
      }
      else {
         ADCS_LOG_WARN("Cannot update LVLH Control without valid attitude solution\n");
         // for(i=0;i<3;i++) AC->qbr[i] = 0.0;
         // AC->qbr[3] = 1.0;
      } 
   } else {
      ADCS_LOG_WARN("Unsupported Frame Target for Requested Quaternion");
   }

   return 0;

}


#ifdef _FLATSAT_HARDWARE_FSW_
int32_t  SunYawTrackingControlLaw(struct AcType *AC) {
   // CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_FSW_EID, CFE_EVS_DEBUG, "Run Sun Yaw Track Control Law");
   struct AcFlatSatCtrlType *C  = &AC->FsCtrl;
   static const long yawAxis = 2;
   static const long wheelIdx = 0;

   if (C->CtrlModeInit) {
      // static const double wn = 0.3;
      // static const double zn = 0.7071; //0.7071 = Critial damp
      // static const double is = 20;
      // FindPDGains(AC->MOI[yawAxis][yawAxis], wn, zn, &C->Kr[yawAxis], &C->Kp[yawAxis]);
      // C->Ki[yawAxis] = 0.0;

      // C->Ki[yawAxis] = (C->Kp[yawAxis]*C->Kr[yawAxis])/(20*AC->MOI[yawAxis][yawAxis]);
      // C->MaxIerr[yawAxis] = 3e-3 / C->Ki[yawAxis];
      C->amax = 0.01;
      C->vmax = 1 * D2R;
      C->wc = 0.1*TWOPI;
      // C->ierr[yawAxis] = 0.0;
      C->CtrlModeInit = FALSE;
      C->SunYawTgt = C->SunYaw;
      C->SunYawDeadband = 0.01 * D2R;
      C->SunYawRateDeadband = 0.25 * D2R;
      // C->wbnMax[yawAxis] = 3 * D2R;
   }

   // Calculate Sun Yaw Error
   double SunYawError = C->SunYawTgt - C->SunYaw;

   if (SunYawError >= PI ) {
      SunYawError = -(TWOPI - SunYawError);
   } else if (SunYawError <= -PI) {
      SunYawError = TWOPI + SunYawError;
   }

   // C->ierr[yawAxis] = Limit(C->ierr[yawAxis] + AC->DT*SunYawError, -C->MaxIerr[yawAxis], C->MaxIerr[yawAxis]);
   C->werr[yawAxis] = AC->wbn[yawAxis];
   C->SunYawError = SunYawError;
   C->therr[yawAxis] = SunYawError;
   double SunYawErrorMag = fabs(SunYawError);
   double SunYawRateErrorMag = fabs(C->werr[yawAxis]);
 
   /* .. Attitude Control */
   double alpha[3] = {0.0, 0.0, 0.0};
   double Tcmd = 0;
   if (AC->SunValid) {
      if ((SunYawErrorMag > C->SunYawDeadband) || (SunYawRateErrorMag > C->SunYawRateDeadband)) {
         VectorRampCoastGlide(C->therr,C->werr,C->wc,C->amax,C->vmax,alpha);
         Tcmd = AC->MOI[yawAxis][yawAxis] * alpha[yawAxis];
         // Tcmd = C->Kr[yawAxis]*C->werr[yawAxis] + C->Kp[yawAxis]*SunYawError + C->Ki[yawAxis]*C->ierr[yawAxis];
         // Tcmd = Limit(Tcmd, -AC->Whl[wheelIdx].Tmax*0.95, AC->Whl[wheelIdx].Tmax*0.95);
      } else {
         ADCS_LOG_INFO("Deadband");
      }
   } else {
      #if defined(_FLATSAT_SIM_FSW_) || defined(_FLATSAT_HARDWARE_FSW_)
      CFE_EVS_SendEvent(ADCS_IMPL_ERROR_FSW_EID, CFE_EVS_ERROR,
         "Sun Vector not in valid state for Sun Yaw Control. Check telemetry (maybe enable HW telem)");
      #else
      ADCS_LOG_ERROR("Sun Vector not in valid state for Sun Yaw Control. Check telemetry (maybe enable HW telem)");
      #endif
   }
   AC->Tcmd[yawAxis] = Tcmd;
   // double Tcmd = 0;
   // if (AC->SunValid) {
   //    if ((SunYawErrorMag > C->SunYawDeadband) || (SunYawRateErrorMag > C->SunYawRateDeadband)) {
   //       Tcmd = C->Kr[yawAxis]*C->werr[yawAxis] + C->Kp[yawAxis]*SunYawError + C->Ki[yawAxis]*C->ierr[yawAxis];
   //       Tcmd = Limit(-Tcmd, -AC->Whl[wheelIdx].Tmax*0.95, AC->Whl[wheelIdx].Tmax*0.95);
   //    }
   // } else {
   //    #ifndef _FLATSAT_STANDALONE_FSW_
   //    CFE_EVS_SendEvent(ADCS_IMPL_ERROR_FSW_EID, CFE_EVS_ERROR,
   //       "Sun Vector not in valid state for Sun Yaw Control. Check telemetry (maybe enable HW telem)");
   //    #else
   //    ADCS_LOG_ERROR("Sun Vector not in valid state for Sun Yaw Control. Check telemetry (maybe enable HW telem)");
   //    #endif
   // }
   // int tcmdSign = (int) signum(Tcmd);
   // if (fabs(AC->wbn[yawAxis]) > C->wbnMax[yawAxis]) {
   //    if((tcmdSign ^ C->CtrlRateLimitSign) < 0 && (C->CtrlRateLimitSign != 0)) {
   //       C->CtrlRateLimitSign = 0;
   //    } else {
   //       C->CtrlRateLimitSign = tcmdSign;
   //    }
   // }

   // if (C->CtrlRateLimitSign == 0) {
   //    AC->Tcmd[yawAxis] = Tcmd;
   // } else {
   //    AC->Tcmd[yawAxis] = 0.0;
   // }

   if (C->Debug == 1) {
      ADCS_LOG_INFO("SunYawTgt: %12.10f, SunYaw: %12.10f, SunYawError: %12.10f, Werr: %12.10f, Alpha: %12.10f, Tcmd: %12.10f, Wheel Speed: %f", 
      C->SunYawTgt*R2D, C->SunYaw*R2D, SunYawError*R2D, C->werr[yawAxis]*R2D, alpha[yawAxis]*R2D, Tcmd, AC->Whl[wheelIdx].w*RPS2RPM);
   } 

   return 0;
}
#endif

/**********************************************************************/
/*  End Control Law Functions                                         */
/**********************************************************************/



// Called every timestep to do processing
int32_t FlatSatFsw(struct AcType *AC) {
 
   int32_t Status = -1; // TODO - Set to actual error value
   struct AcFlatSatCtrlType *C  = &AC->FsCtrl;
   static long i,j;

   if (C->Init) {
      C->Init = 0;
      C->UseImuState = 1;
      C->Kunl = 1.0E6;
      C->CtrlMode = FSW_CTRL_CLOSED_LOOP_LVLH;
      C->CtrlModeInit = TRUE;
      C->AttitudeSource = FSW_ATT_SOURCE_NONE;

      #ifndef _FLATSAT_HARDWARE_FSW_
      C->Debug = FALSE;
      C->ImuStateValid = TRUE;
      C->UseImuState = FALSE;
      C->ImuGyroValid = TRUE;
      C->ImuMagnetometerValid = TRUE;
      C->ImuAccelerationValid = TRUE;
      AC->EphValid = TRUE;
      #endif
      #if defined(_FLATSAT_SIM_FSW_) || defined(_FLATSAT_HARDWARE_FSW_)
      ADCSImpl->CtrlMode = C->CtrlMode;
      #endif

      ADCS_LOG_INFO("FlatSatFsw Init Complete. CtrlMode: %ld, UseImuState: %ld, Debug: %ld", C->CtrlMode, C->UseImuState, C->Debug);
      // if (C->Debug) {
      //    outfile = fopen("FswResults.txt", "w");
      // }
   }

   /* .. Reset Actuator States */
   for (i=0; i<3; i++) {
      AC->Tcmd[i] = 0;
      AC->Mcmd[i]= 0;
   }

   #ifdef _FLATSAT_HARDWARE_FSW_

   /* .. Get Updated Mode */
   C->CtrlMode = ADCSImpl->CtrlMode;
   /* .. Sensor Data Update */
   Status = FsImuProcessing(AC, &ADCSImpl->IMU);
   FsWheelUpdate(AC, &ADCSImpl->RW);
   FsCssUpdate(AC, &ADCSImpl->XADC);
   #endif

   #ifdef _FLATSAT_SIM_FSW_
   FsAnalogMonitorProcessing(AC, &ADCSImpl->XADC);
   #endif

   #ifdef _FLATSAT_STANDALONE_FSW_

   #ifdef _ENABLE_MQTT_
   while (C->SensorDataRecv != TRUE) {
      usleep(10);
   }
   // SensorDataWaitCnt = 0;
   ADCS_LOG_INFO("SensorDataRecvCnt: %lu", C->SensorDataRecvCnt);
   C->SensorDataRecv = FALSE;
   #else
   ReadFromSocket(Socket,AC);
   #endif

   #endif


   /* .. Sensor Data Processing */
   FsGyroProcessing(AC);
   FsMagnetometerProcessing(AC);
   FsAccelProcessing(AC);
   FsUseImuState(AC);
   #ifdef _FLATSAT_HARDWARE_FSW_
   FsKdstProcessing(AC, &ADCSImpl->ST);
   #endif
   FsCssProcessing(AC);
   
   // _ADCS_CHALLENGE_
   #if _FSS_CHALLENGE_ == 1
      FssProcessing(AC);
      C->FssStateValid = AC->FSS->Valid;
   #endif


   FsStarTrackerProcessing(AC);

   if (AC->StValid) {
      C->AttitudeSource = FSW_ATT_SOURCE_ST;
   } else {
      if (AC->SunValid && AC->EphValid) {
         C->AttitudeSource = FSW_ATT_SOURCE_SUN_MAG;
      } else {
         if (C->ImuGyroValid) {
            C->AttitudeSource = FSW_ATT_SOURCE_RATES;
         } else {
            C->AttitudeSource = FSW_ATT_SOURCE_NONE;
         }
      }
   }
   if (C->AttitudeSource != ADCSImpl->AttitudeSource) {
      C->AttitudeSourceStepCnt = 0;
   }

   /**
    * Attitude Solution Source Processing
   */
   if (C->AttitudeSource == FSW_ATT_SOURCE_SUN_MAG) {
      if (C->AttitudeSourceStepCnt == 0) {
         ADCS_LOG_WARN("ST Solution NOT Valid. Fallback to SUN-MAG Solution");
         CFE_EVS_SendEvent(ADCS_IMPL_ERROR_FSW_EID, CFE_EVS_ERROR, "ST Solution NOT Valid. Fallback to SUN-MAG Solution");
      }
         /* Fallback to Sun-TAM Attitude Determination */
      if (AC->SunValid) {
         TRIAD(AC->svn,AC->bvn,AC->svb,AC->bvb,AC->CBN);
         C2Q(AC->CBN, AC->qbn);
         UNITQ(AC->qbn);
         RECTIFYQ(AC->qbn);
      }
   } else if(C->AttitudeSource == FSW_ATT_SOURCE_RATES) {
      if (C->AttitudeSourceStepCnt == 0) {
         ADCS_LOG_WARN("ST and SUN-MAG Solutions Not Valid. Fallback to rate integration");
         CFE_EVS_SendEvent(ADCS_IMPL_ERROR_FSW_EID, CFE_EVS_ERROR, "ST and SUN-MAG Solutions Not Valid. Fallback to rate integration");
      }
      double qdot[4];
      QW2QDOT(C->qbn_prev,AC->wbn,qdot);
      for(i=0;i<4;i++) AC->qbn[i] += qdot[i]*AC->DT;
   }
   ADCSImpl->AttitudeSource = C->AttitudeSource;
   C->AttitudeSourceStepCnt++;

   
   /* .. Control Law Procesing */
   ADCS_LOG_DEBUG("ADCS Control Mode: %d", C->CtrlMode);
   switch (C->CtrlMode)
   {
   case FSW_CTRL_DISABLED:
      ADCS_LOG_WARN("ADCS Control Disabled");
      break;
   case FSW_CTRL_CLOSED_LOOP_LVLH:
      #ifdef _FLATSAT_SIM_FSW_
      LvlhControlLaw(AC);
      #else
      CFE_EVS_SendEvent(ADCS_IMPL_ERROR_FSW_EID, CFE_EVS_ERROR,
         "Control mode not implemened in this FSW release");
      #endif
      break;
   case FSW_CTRL_OPEN_LOOP_WHEEL:
      #if defined (_FLATSAT_SIM_FSW_) || defined(_FLATSAT_HARDWARE_FSW_)
      #else
      CFE_EVS_SendEvent(ADCS_IMPL_ERROR_FSW_EID, CFE_EVS_ERROR,
         "Control mode not implemened in this FSW release");
      #endif
      break;
   case FSW_CTRL_SUN_PAYLOAD_LVLH:
      #ifdef _FLATSAT_SIM_FSW_
      // LvlhControlLaw(AC);
      #else
      CFE_EVS_SendEvent(ADCS_IMPL_ERROR_FSW_EID, CFE_EVS_ERROR,
         "Control mode not implemened in this FSW release");
      #endif
      break;
   case FSW_CTRL_CLOSED_LOOP_SUN_TRACKING:
      #ifdef _FLATSAT_HARDWARE_FSW_
      SunYawTrackingControlLaw(AC);
      #else
      CFE_EVS_SendEvent(ADCS_IMPL_ERROR_FSW_EID, CFE_EVS_ERROR,
         "Control mode not implemened in this FSW release");
      #endif
      break;

   default:
      break;
   }

   
   /* .. Momentum Management */
   for(i=0;i<AC->Nwhl;i++) {
      AC->Hvb[i] = AC->MOI[i][i]*AC->wbn[i];
      for(j=0;j<AC->Nwhl;j++) AC->Hvb[i] += AC->Whl[j].Axis[i]*AC->Whl[j].H;
   }
   double HxB[3];
   VxV(AC->Hvb,AC->bvb,HxB);
   for(i=0;i<3;i++) AC->Mcmd[i] = C->Kunl*HxB[i];

   /* .. Actuator Data Processing */
   FsWheelProcessing(AC);
   FsMtbProcessing(AC);

   /* .. Save Attitude State */
   CFE_PSP_MemCpy(C->qbn_prev, AC->qbn, sizeof(AC->qbn));

   /* .. Eval FSW */
   if (C->Debug) {
      switch (C->CtrlMode)
      {
      case FSW_CTRL_DISABLED:
         ADCS_LOG_WARN("ADCS Control Disabled");
         break;
      case FSW_CTRL_CLOSED_LOOP_LVLH:
         #ifdef _FLATSAT_SIM_FSW_
         EvalLvlhFsw(AC, outfile);
         #else
         CFE_EVS_SendEvent(ADCS_IMPL_ERROR_FSW_EID, CFE_EVS_ERROR,
            "Control mode not implemened in this FSW release");
         #endif
         break;
      case FSW_CTRL_OPEN_LOOP_WHEEL:
      #if defined (_FLATSAT_SIM_FSW_) || defined(_FLATSAT_HARDWARE_FSW_)
         #else
         CFE_EVS_SendEvent(ADCS_IMPL_ERROR_FSW_EID, CFE_EVS_ERROR,
            "Control mode not implemened in this FSW release");
         #endif
         break;
      case FSW_CTRL_SUN_PAYLOAD_LVLH:
         #ifdef _FLATSAT_SIM_FSW_
         // LvlhControlLaw(AC);
         #else
         CFE_EVS_SendEvent(ADCS_IMPL_ERROR_FSW_EID, CFE_EVS_ERROR,
            "Control mode not implemened in this FSW release");
         #endif
         break;
      case FSW_CTRL_CLOSED_LOOP_SUN_TRACKING:
         #ifdef _FLATSAT_HARDWARE_FSW_
         EvalSunYawFsw(AC, outfile);
         #else
         CFE_EVS_SendEvent(ADCS_IMPL_ERROR_FSW_EID, CFE_EVS_ERROR,
            "Control mode not implemened in this FSW release");
         #endif
         break;

      default:
         break;
      }
   }


   /* .. Actuator Data Outputs */

   // HARDWARE CODE PATH
   #if defined(_FLATSAT_HARDWARE_FSW_)
   FsWheelCommand(AC, &ADCSImpl->RW);
   FsMtbCommand(AC, &ADCSImpl->MTR);
   
   // STANDALONE CODE PATH
   #elif defined(_FLATSAT_STANDALONE_FSW_)
   #ifdef _ENABLE_MQTT_
   WriteToMqtt(client, AC, "42_MQTT_RECV");
   #else
   WriteToSocket(Socket,AC);
   #endif

   // SIM CODE PATH
   #elif defined(_FLATSAT_SIM_FSW_)
   CFE_SB_Msg_t*  MsgPtr = NULL;
   MsgPtr = (CFE_SB_Msg_t*)&ADCSImpl->SimActuatorPkt;
   CFE_SB_InitMsg(MsgPtr, ADCS_SIM_ACTUATOR_DATA_MID, ADCS_SIM_ACTUATOR_PKT_LEN, TRUE);
   size_t actuator_pkt_size = AcTypeToString(&ADCSImpl->AC, ADCSImpl->SimActuatorPkt.Data);
   // ADCSImpl->SimActuatorPkt.DataLen = actuator_pkt_size;
   ADCS_LOG_DEBUG("Sending Actuator Pkt with size %d", actuator_pkt_size);
   if (MsgPtr != NULL && actuator_pkt_size > 0) {
      CFE_SB_TimeStampMsg(MsgPtr);
      Status = CFE_SB_SendMsg(MsgPtr);
      if (Status != CFE_SUCCESS) {
         ADCS_LOG_ERROR("Actuator data send failure");
      }

   }
   // Throw error if none are defined
   #else
      #error "Must have one of following definitions: _FLATSAT_HARDWARE_FSW_ or _FLATSAT_STANDALONE_FSW_ or _FLATSAT_SIM_FSW_"
   #endif

   

   #if defined(_FLATSAT_SIM_FSW_) || defined(_FLATSAT_HARDWARE_FSW_)
   /* Update FSW Telemetry Packet */
   FsUpdateFswTelemetry(ADCSImpl);
   #endif
   return Status;

}

void FlatSatFswEnableDebug(struct AcType *AC) {
   AC->FsCtrl.Debug = 1;
}




#ifdef  _FLATSAT_STANDALONE_FSW_
/**********************************************************************/
int main(int argc, char **argv)
{

      FILE *ParmDumpFile;
      char FileName[120];
      struct AcType AC;
      // SOCKET Socket;

      double timeInit = 0;
      double timeElapsed = 0;
      
      FlatSatFswAllocateAC(&AC);
      FlatSatFswEnableDebug(&AC);
      FlatSatFswInitAC(&AC);

      struct AcFlatSatCtrlType *C  = &AC.FsCtrl;
      /* Enable Debug */

      /* Load parms */
      AC.EchoEnabled = 1;
      AC.DT = 0.2;
      // ReadFromSocket(Socket,&AC);
      // timeInit = AC.Time;
      timeElapsed = 0;
      double totalTime = 100;
      double impulseTime = 10;
      double impulseMag = 5*D2R;
      int impulseAxis = 1;
      // static const double wn = 0.3;
      // static const double zn = 0.7071; //0.7071 = Critial damp
      // static const double is = 20;
      // double wc, vmax, amax;
      if (argc > 2) {
         sscanf(argv[1],"%lf",&C->wc);
         sscanf(argv[2],"%lf",&C->vmax);
         sscanf(argv[3],"%lf",&C->amax);
         sscanf(argv[4],"%lf",&totalTime);
         if(argc > 4) {
            sscanf(argv[5],"%lf",&impulseTime);
            sscanf(argv[6],"%lf",&impulseMag);
            sscanf(argv[7], "%d", &impulseAxis);
         }
      }

      C->wc = C->wc * TWOPI;
      C->vmax = C->vmax * D2R;
      C->amax = C->amax * D2R;

      printf("Using Inputs: Wc %f, Vmax %f, Amax %f\n", C->wc, C->vmax*R2D, C->amax*R2D);
      printf("Simulation: Total Time %f, Impulse Time %f, Impulse Size %f, Impulse Body Axis %d\n", totalTime, impulseTime, impulseMag, impulseAxis);

      // C->wc = wc;
      // FindPDGains(AC.MOI[yawAxis][yawAxis], wn, zn, &C->Kr[yawAxis], &C->Kp[yawAxis]);
      // C->Ki[yawAxis] = 0.0;
      // C->Ki[yawAxis] = (C->Kp[yawAxis]*C->Kr[yawAxis])/(is*AC.MOI[yawAxis][yawAxis]);
      
      // FlatSatFsw(&AC);
      
      // sprintf(FileName,"./Database/AcParmDump%02ld.txt",AC.ID);
      // ParmDumpFile = fopen(FileName,"wt");
      // WriteToFile(ParmDumpFile,&AC);
      // fclose(ParmDumpFile);
      // WriteToSocket(Socket,&AC);

      double DCM[3][3];
      double qtr[4];
      double impulseAngle = impulseMag*D2R;
      if (impulseAxis == 1) {
         A2C(213, 0.0, impulseAngle, 0.0, DCM);
      } else if (impulseAxis == 2)
      {
         A2C(213, impulseAngle, 0.0, 0.0, DCM);
      } else if (impulseAxis == 3)
      {
         A2C(213, 0.0, 0.0, impulseAngle, DCM);
      } else {
         ADCS_LOG_ERROR("Unknown axis");
         exit(1);
      }
      
      C2Q(DCM, qtr);
      while(timeElapsed < (totalTime)) {
         if(fabs(timeElapsed - impulseTime) < 1e-6 ) {
            for(int i=0; i<4; i++) C->qtr[i] = qtr[i];
         }
         FlatSatFsw(&AC);
         timeElapsed += AC.DT;
         printf("\n\nTime Elapsed: %f\n", timeElapsed);
         
      }
      FlatSatFsw(&AC);
      #ifdef _ENABLE_MQTT_
      CloseMqttClient(client);
      #endif
      printf("\n\n--- Complete ---\n\n");
      if (outfile != NULL) {
         fclose(outfile);
      }

      return(0);
}
#endif
/* #ifdef __cplusplus
** }
** #endif
*/

