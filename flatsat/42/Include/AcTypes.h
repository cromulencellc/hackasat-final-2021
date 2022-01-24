/*    This file is distributed with 42,                               */
/*    the (mostly harmless) spacecraft dynamics simulation            */
/*    created by Eric Stoneking of NASA Goddard Space Flight Center   */

/*    Copyright 2010 United States Government                         */
/*    as represented by the Administrator                             */
/*    of the National Aeronautics and Space Administration.           */

/*    No copyright is claimed in the United States                    */
/*    under Title 17, U.S. Code.                                      */

/*    All Other Rights Reserved.                                      */


#ifndef __ACTYPES_H__
#define __ACTYPES_H__

/*
** #ifdef __cplusplus
** namespace _42 {
** #endif
*/

/* Variable tags for message building are delimited by [~ ~] */
/* > : Send from 42 to standalone */
/* < : Send from standalone to 42 */
/* = : Send both ways (eg. 42 Tx to 42 Rx) */
/* ! : Read from command file */
/* Example: [~!=~] means this variable can be read from command, and is sent both ways */

struct CmdVecType {
   /*~ Internal Variables ~*/
   long Mode;
   long Frame;
   long TrgType;
   long TrgWorld;
   long TrgSC;
   long TrgBody;
   double N[3]; /* Components in N */
   double W[3]; /* Components in W */
   double L[3]; /* Components in L */
   double R[3]; /* Components in R */
   double T[3]; /* Components in T */
   double wn[3]; /* Angular velocity in N, expressed in N */
};

struct CmdType {
   /*~ Internal Variables ~*/
   long Parm;
   long Frame;
   double AngRate[3];
   double Ang[3]; /* [~<~] */
   double PosRate[3];
   double Pos[3];
   long RotSeq;
   double qrl[4]; /* [~!~] */
   double qrn[4]; /* [~!~] */
   double wrn[3];
   double SpinRate;
   double Hvr[3];
   double Hvn[3];
   double OldCRN[3][3];

   /*~ Structures ~*/
   struct CmdVecType PriVec;
   struct CmdVecType SecVec;
};

struct AcBodyType {
   /*~ Parameters ~*/
   double mass; /* [[kg]] */
   double cm[3]; /* [[m]] */
   double MOI[3][3]; /* [[kg-m^2]] */
};

struct AcJointType {
   /*~ Parameters ~*/
   long IsUnderActiveControl;
   long IsSpherical;
   long RotDOF;
   long TrnDOF;
   long RotSeq;
   long TrnSeq;
   double CGiBi[3][3];
   double CBoGo[3][3];
   double AngGain[3];
   double AngRateGain[3];
   double PosGain[3];
   double PosRateGain[3];
   double MaxAngRate[3];
   double MaxPosRate[3];
   double MaxTrq[3];
   double MaxFrc[3];

   /*~ Internal Variables ~*/
   double Ang[3]; /* [[rad]] [~>~] */
   double AngRate[3];
   double Pos[3];
   double PosRate[3];
   double COI[3][3];
   
   /*~ Structures ~*/
   struct CmdType Cmd;
};

struct AcGyroType {
   /*~ Parameters ~*/
   double Axis[3];

   /*~ Internal Variables ~*/
   double Rate; /* [[rad/sec]] [~>~] */
};

struct AcMagnetometerType {
   /*~ Parameters ~*/
   double Axis[3];

   /*~ Internal Variables ~*/
   double Field; /* [[Tesla]] [~>~] */
};

struct AcCssType {
   /*~ Parameters ~*/
   long Channel;
   long Body;
   long Calibrated;
   double Axis[3];
   double Scale;
   double IllumBackground;

   /*~ Internal Variables ~*/
   long Valid; /* [~>~] */
   double Illum; /* [~>~] */
};


struct AcFssType {
   /*~ Parameters ~*/
   double qb[4];
   double CB[3][3];

   /*~ Internal Variables ~*/
   long Valid;       /* [~>~] */
   double SunAng[2]; /* [[rad]] [~>~] */
   double SunVecS[3];/* [~>~] */
   double SunVecB[3];/* [~>~] */
};

struct AcStarTrackerType {
   /*~ Parameters ~*/
   double qb[4];
   double CB[3][3];

   /*~ Internal Variables ~*/
   long Valid;   /* [~>~] */
   double qn[4]; /* [~>~] */
   double qbn[4];/* [~>~] */
};

struct AcGpsType {
   /*~ Internal Variables ~*/
   long Valid; /* [~>~] */
   long Rollover; /* [~>~] */
   long Week; /* [~>~] */
   double Sec; /* [~>~] */
   double PosN[3]; /* [[m]] [~>~] */
   double VelN[3]; /* [[m/s]] [~>~] */
   double PosW[3]; /* [[m]] [~>~] */
   double VelW[3]; /* [[m/s]] [~>~] */
   double Lng; /* Geocentric [[rad]] [~>~] */
   double Lat; /* Geocentric [[rad]] [~>~] */
   double Alt; /* Geocentric [[m]] [~>~] */
   double WgsLng; /* Geodetic, WGS-84 [[rad]] [~>~] */
   double WgsLat; /* Geodetic, WGS-84 [[rad]] [~>~] */
   double WgsAlt; /* Geodetic, WGS-84 [[m]] [~>~] */
};

struct AcAccelType {
   /*~ Parameters ~*/
   double PosB[3];
   double Axis[3];

   /*~ Internal Variables ~*/
   double Acc; /* [[m/s^2]] [~>~] */
};

struct AcEarthSensorType {
   /*~ Internal Variables ~*/
   long Valid;
   double Roll;
   double Pitch;
};

struct AcWhlType {
   /*~ Parameters ~*/
   double Axis[3];
   double DistVec[3];
   double J; /*[[kg-m^2]] [~>~]*/
   double Tmax;
   double Hmax;

   /*~ Internal Variables ~*/
   double w; /* [[rad/sec]] [~>~] */
   double H; /* [[Nms]] [~>~] */
   double Tcmd; /* [[N-m]] [~<~] */
   double Wcmd;
};

struct AcMtbType {
   /*~ Parameters ~*/
   double Axis[3];
   double DistVec[3];
   double Mmax;

   /*~ Internal Variables ~*/
   double Mcmd; /* [[A-m^2]] [~<~] */
};

struct AcThrType {
   /*~ Parameters ~*/
   double PosB[3];
   double Axis[3];
   double rxA[3];
   double Fmax;

   /*~ Internal Variables ~*/
   double Fcmd;
   double PulseWidthCmd; /* [[sec]] [~<~] */
};

struct AcPrototypeCtrlType {
   /*~ Parameters ~*/
   double wc;
   double amax;
   double vmax;
   double Kprec;
   double Knute;

   /*~ Internal Variables ~*/
   long Init;
   double Tcmd[3];
   double qbr[4];
   double therr[3];
   double werr[3];
};

struct AcAdHocCtrlType {
   /*~ Parameters ~*/
   double Kr[3];
   double Kp[3];

   /*~ Internal Variables ~*/
   long Init;
   double therr[3];
   double werr[3];
   double Tcmd[3];
};

struct AcSpinnerCtrlType {
   /*~ Parameters ~*/
   double Ispin;
   double Itrans; 
   double SpinRate;
   double Knute;
   double Kprec;


   /*~ Internal Variables ~*/
   long Init;
   double Bold1,Bold2;
   double xold,yold;
   double rvn[3],rvb[3];
   double Tcmd[3];
   double Mcmd[3];
};

struct AcMomBiasCtrlType {
   /*~ Internal Variables ~*/
   long Init;
};

struct AcThreeAxisCtrlType {
   /*~ Parameters ~*/
   double Kr[3];
   double Kp[3];
   double Kunl;
   
   /*~ Internal Variables ~*/
   long Init;
   double Tcmd[3];
   double Hwcmd[3]; 
};

struct AcIssCtrlType {
   /*~ Parameters ~*/
   double Kr[3];
   double Kp[3];
   double Tmax;

   /*~ Internal Variables ~*/
   long Init;
   double therr[3];
   double werr[3];
};

struct AcCmgCtrlType {
   /*~ Parameters ~*/
   double Kr[3];
   double Kp[3];
   
   /*~ Internal Variables ~*/
   long Init;
   double therr[3],werr[3];
   double Tcmd[3];
   double AngRateCmd[4];
};

struct AcThrCtrlType {
   /*~ Parameters ~*/
   double Kw[3];
   double Kth[3];
   double Kv;
   double Kp;

   /*~ Internal Variables ~*/
   long Init;
};

struct AcCfsCtrlType {
   /*~ Parameters ~*/
   double Kr[3];
   double Kp[3];
   double Kunl;
   
   /*~ Internal Variables ~*/
   long Init;
   double therr[3];
   double werr[3];
};

struct AcThrSteerCtrlType {
   /*~ Parameters ~*/
   double Kr[3];
   double Kp[3];

   /*~ Internal Variables ~*/
   long Init;
   double therr[3];
   double werr[3];
   double ierr[3];
   double Tcmd[3];
};


struct AcFlatSatCtrlType {
   
   /*~ Flags ~*/
   long ImuGyroValid;
   long ImuMagnetometerValid;
   long ImuAccelerationValid;
   long ImuStateValid;
   long UseImuState;
   long CalibrateCss;
   long Init;
   long Debug;
   long CtrlLoopError;
   long ImuError;
   long WheelError;
   long MtbError;
   long StError;
   long GpsError;

   long CtrlMode;
   long CtrlModeInit;
   // int CtrlRateLimitSign;
   long MtbMode;
   long CssCalCounter;
   long WheelSpeedCmdCounter;
   long SensorDataRecv;
   long SensorDataRecvCnt;
   double **IllumCalArray;

   /*~ Parameters ~*/
   // double Kr[3];
   // double Kp[3];
   // double Ki[3];
   double Kunl;
   // double MaxIerr[3];
   double wc;
   double amax;
   double vmax;

   long FrameTgt;
   double qtr[4];
   double qtn[4];
   double qbl[4];
   double qimu[4];
   double SunYaw;
   double SunYawTgt;
   double SunYawDeadband;
   double SunYawRateDeadband;
   double SunYawError;

   /*~ Internal Variables ~*/
   // double wbnMax[3];
   double therr[3];
   double werr[3]; // Body rate error
   // double ierr[3];

};

struct AcMqttCtrlType {
   long CtrlReceived;
   double LastSensorDataTime; // Last sensor send time in seconds since J2000

};

struct AcEPSType {
   /*~ External FSW Variable ~*/
   double Wheel_I;               /* [[A]] [~>~] */
   double IMU_I;                 /* [[A]] [~>~] */
   double StarTracker_I;         /* [[A]] [~>~] */
   double MTR_I;                 /* [[A]] [~>~] */
   double CSS_I;                 /* [[A]] [~>~] */
   double FSS_I;                 /* [[A]] [~>~] */
   double ADCS_I;                /* [[A]] [~>~] */
   double CDH_I;                 /* [[A]] [~>~] */
   double COM_I;                 /* [[A]] [~>~] */
   double PAYLOAD_I;             /* [[A]] [~>~] */
   double BATTERY_I;             /* [[A]] [~>~] */
   double MTR_V;                 /* [[V]] [~>~] */
   double Wheel_V;               /* [[V]] [~>~] */
   double IMU_V;                 /* [[V]] [~>~] */
   double StarTracker_V;         /* [[V]] [~>~] */
   double CSS_V;                 /* [[V]] [~>~] */
   double FSS_V;                 /* [[V]] [~>~] */
   double ADCS_V;                /* [[V]] [~>~] */
   double COM_V;                 /* [[V]] [~>~] */
   double CDH_V;                 /* [[V]] [~>~] */
   double PAYLOAD_V;             /* [[V]] [~>~] */
   double BATTERY_V;             /* [[V]] [~>~] */
   double BATTERY_SOC;          /* [[State of Charge]] [~>~]*/
   long Wheel_Switch;            /* ON=1/OFF=0 [~=~]*/
   long IMU_Switch;              /* ON=1/OFF=0 [~=~]*/
   long StarTracker_Switch;      /* ON=1/OFF=0 [~=~]*/
   long MTR_Switch;              /* ON=1/OFF=0 [~=~]*/
   long CSS_Switch;              /* ON=1/OFF=0 [~=~]*/
   long FSS_Switch;              /* ON=1/OFF=0 [~=~]*/
   long ADCS_Switch;             /* ON=1/OFF=0 [~=~]*/
   long COM_Switch;              /* ON=1/OFF=0 [~=~]*/
   long PAYLOAD_Switch;          /* ON=1/OFF=0 [~=~]*/
};

struct AcSolarArrayType {
   /*~ Internal Variables ~*/
   long Valid;                /*[~>~]*/
   double PowerOut;           /*[[mW]] [~>~]*/
   double CurrentOut;         /*[[mA]] [~>~]*/
   double Voltage;            /*[[V]] [~>~] */
};

struct AcType {
   /*~ Parameters ~*/
   long ID; /* Spacecraft ID */
   long EchoEnabled; /* For IPC */
   long ParmLoadEnabled;
   long ParmDumpEnabled;
   long Nb;
   long Ng;
   long Nwhl;
   long Nmtb;
   long Nthr;
   long Ncmg;
   long Ngyro; 
   long Nmag;
   long Ncss;
   long Nfss;
   long Nst;
   long Ngps;
   long Nacc;
   long Neps;
   long Nsolarr;

   double DynTime; /* [~>~] */
   double DT; /* [~>~] */
   double mass;
   double cm[3];
   double MOI[3][3];
   
   /*~ Inputs ~*/
   double Time;  /* Time since J2000 [[sec]] [~=~] */
   long Mode;
   double wbn[3];  /* [~>~] */
   double qbn[4];  /* [~>~] */
   double CBN[3][3];
   double CLN[3][3];
   double wln[3];
   double qln[4];
   double svn[3]; /* [~>~] */
   double svb[3]; 
   double bvn[3]; /* [~>~] */
   double bvb[3];
   double PosN[3]; /* [~>~] */
   double VelN[3]; /* [~>~] */
   double AccN[3];
   long SunValid;
   long MagValid;
   long EphValid;
   long StValid;
   
   /*~ Outputs ~*/
   long ReqMode;

   double Tcmd[3];
   double Mcmd[3];
   double Fcmd[3];
   
   double IdealTrq[3];
   double IdealFrc[3];
   
   /*~ Internal Variables ~*/
   long Init;
   double qbr[4];
   double Hvb[3];

   /*~ Structures ~*/

   /* Dynamics */
   struct AcBodyType *B;   /* [*Nb*] */
   struct AcJointType *G;  /* [*Ng*] */
   
   /* Sensors */
   struct AcGyroType *Gyro;        /* [*Ngyro*] */
   struct AcMagnetometerType *MAG; /* [*Nmag*] */
   struct AcCssType *CSS;          /* [*Ncss*] */
   struct AcFssType *FSS;          /* [*Nfss*] */
   struct AcStarTrackerType *ST;   /* [*Nst*] */
   struct AcGpsType *GPS;          /* [*Ngps*] */
   struct AcAccelType *Accel;      /* [*Nacc*] */
   struct AcEarthSensorType ES;
   struct AcEPSType *EPS;          /* [*Neps*] */
   struct AcSolarArrayType *SolArr;/* [*Nsolarr*] */
   /* Actuators */
   struct AcWhlType *Whl; /* [*Nwhl*] */
   struct AcMtbType *MTB; /* [*Nmtb*] */
   struct AcThrType *Thr; /* [*Nthr*] */
   
   /* Control Modes */
   struct AcPrototypeCtrlType PrototypeCtrl;
   struct AcAdHocCtrlType AdHocCtrl;
   struct AcSpinnerCtrlType SpinnerCtrl;
   struct AcMomBiasCtrlType MomBiasCtrl;
   struct AcThreeAxisCtrlType ThreeAxisCtrl;
   struct AcIssCtrlType IssCtrl;
   struct AcCmgCtrlType CmgCtrl;
   struct AcThrCtrlType ThrCtrl;
   struct AcCfsCtrlType CfsCtrl;
   struct AcFlatSatCtrlType FsCtrl;
   struct AcThrSteerCtrlType ThrSteerCtrl;
   struct AcMqttCtrlType MqttCtrl;
   
   struct CmdType Cmd;
   
};

/*
** #ifdef __cplusplus
** }
** #endif
*/

#endif /* __ACTYPES_H__ */
