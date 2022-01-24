/* 42.i */
%module wrapper42
%{
/* Put header files here or function declarations like below */
#include "42.h"
#include "42types.h"
#include "AcTypes.h"
%}

%include "42.h"
%include "42types.h"
%include "AcTypes.h"

/*
extern void InitSim(int argc, char **argv);
extern void CmdInterpreter(void);
extern void InitInterProcessComm(void);
extern long SimStep(void);
extern double RealRunTime(double *RealTimeDT, double LSB);
extern void ReportProgress(void);
extern void ManageFlags(void);
void Ephemerides(void);
void ZeroFrcTrq(void);
void Environment(struct SCType *S)
void Perturbations(struct SCType *S)
void Sensors(struct SCType *S)
void FlightSoftWare(struct SCType *S)
*/