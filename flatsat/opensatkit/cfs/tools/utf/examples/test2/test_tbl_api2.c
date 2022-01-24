/* File: test_tbl_api1.c
 * 
 * This test driver is used to demonstrate the execution
 * of a unit test of a table owner application.
 *
 * Output can be directed either to screen or to file.
 * To direct output to screen, 
 *    comment in '#define UTF_USE_STDOUT' statement in the
 *    utf_custom.h file.
 *
 * To direct output to file, 
 *    comment out '#define UTF_USE_STDOUT' statement in 
 *    utf_custom.h file.
 *
 * Changes
 * 2005/09/16 nschweiss Modified call to CreatePipe to pass PipeId* rather than 0.
 * 2005/09/14 Added CFE_ES_RegisterApp() call.
 * 2005/08/08 nschweiss Names of some API procedures changed in cFE 2.0.
 * 2005/08/10 nschweiss Made necessary changes for consistency with updated TBL API.
 * 2005/09/13 nschweiss Modified main to return an int, and resolve compiler warning.
 * 2005/09/13 nschweiss Added function prototype declarations for procedures 
 *                      to resolve compiler warnings.
 * 2005/10/05 nschweiss Augmented to read in test script.
 * 2005/11/30 nschweiss Replaced table init function with call
 * to CFE_TBL_EarlyInit.
 */ 

#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "cfe.h"
#include "cfe_tbl_task.h"
#include "utf_custom.h"      /* defines UTF_QUEUE_SIZE */
#include "utf_types.h"       /*defines UTF_queue_add() */
#include "cfe_time_utils.h"
#include "utf_cfe_sb.h"          /*defines UTF_set_sb_receive_return_status */
#include "tt_app.h"
#include "string.h"
#include "cfe_tbl_internal.h"
#include "cfe_es_cds.h"	/* for CFE_ES_CDS_EarlyInit */
#include "utf_osapi.h"
#include "utf_cfe.h"
#include "utf_cfe_sb.h"

#define CFE_TT_INVALID_CC 55
#define CFE_TT_INVALID_MID 0x12
#define MESSAGE_FORMAT_IS_CCSDS
#define CFE_TT_CMD_PIPE		1
#define CFE_TBL_NUMBER_OF_TABLES 16

extern TT_AppData_t  TT_AppData;

void UTF_SCRIPT_LoadTableFromGround(int argc,char *argv[])
{
	char Table_Name[30], File_Name[50];
/*	UTF_put_text("Entered UTF_SCRIPT_LoadTableFromGround\n"); */
	if (argc != 3)
	{
	   UTF_error("Error: Read %d args w/script cmd LOAD_TABLE_FROM_GROUND. Expected 2.\n",
	   argc -1 );
	   UTF_exit();
	}

	strcpy(Table_Name,argv[1]);
	strcpy(File_Name,argv[2]);
/*	UTF_put_text("Table_Name is %s\n",Table_Name);
//	UTF_put_text("File_Name is %s\n", File_Name); */
	UTF_TBL_LoadTableFromGround(Table_Name, File_Name);
	return;
}


void not_working(void)
{
	/* I never got this to work with the 'open' function call, */
	/* & so used fopen instead.  See Create_input_File0       */
	int to;
	int8 values[4] = {5,10,15,20};
	to = open("/ram/tt_table_initial_values.dat",  O_WRONLY | O_CREAT, 0644);
	if (to < 0)
	{
		UTF_error("Error opening file /tt_table_initial_values.dat to write\n");
		UTF_exit();
	}

	write(to, values, 4);	
	
	close(to);
}

void Create_Input_File0(void)
{
/*	FILE *to; */
	int8 values[4] = {5,10,15,20};
	char tableName[30];
	CFE_FS_Header_t    StdFileHeader;
    CFE_TBL_File_Hdr_t TblFileHeader;
    int32              FileDescriptor;
    int32              Status;
    int32              EndianCheck = 0x01020304;

/*	to = fopen("/tt_table_values0.dat",  "wb");
	if (to == 0)
	{
		UTF_error("Error opening file /tt_table_initial_values.dat to write\n");
		UTF_exit();
	}

	fwrite(values, sizeof(int8), 4, to);	
	
	fclose(to); */

	strcpy (tableName, "TT.FourNumbers");

	/* Clear Header of any garbage before copying content */
    CFE_PSP_MemSet(&StdFileHeader, 0, sizeof(CFE_FS_Header_t));
    CFE_PSP_MemSet(&TblFileHeader, 0, sizeof(CFE_TBL_File_Hdr_t));

	/* Create a new dump file, overwriting anything that may have existed previously */
    FileDescriptor = OS_creat("/ram/tt_table_values0.dat", OS_WRITE_ONLY);

    if (FileDescriptor >= OS_FS_SUCCESS)
    {
        /* Initialize the standard cFE File Header for the Dump File */
       	StdFileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
       	strcpy(&StdFileHeader.Description[0], "Table Load File");

       	/* Output the Standard cFE File Header to the Dump File */
       	Status = CFE_FS_WriteHeader(FileDescriptor, &StdFileHeader);

        if (Status == sizeof(CFE_FS_Header_t))
       	{
           	/* Initialize the Table Image Header for the Dump File */
           	CFE_PSP_MemCpy(TblFileHeader.TableName, tableName, CFE_TBL_MAX_FULL_NAME_LEN);
           	TblFileHeader.Offset = 0;
            TblFileHeader.NumBytes = 4;
    	    TblFileHeader.Reserved = 0;
            
           	/* Determine if this is a little endian processor */
           	if ((*(char *)&EndianCheck) == 0x04)
           	{
	        	CFE_TBL_ByteSwapTblHeader(&TblFileHeader);
    	    }

            /* Output the Table Image Header to the Dump File */
           	Status = OS_write(FileDescriptor, &TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));

           	/* Make sure the header was output completely */
           	if (Status == sizeof(CFE_TBL_File_Hdr_t))
           	{
	        	/* Output the requested data to the dump file */
    	        /* Output the active table image data to the dump file */
        	    Status = OS_write(FileDescriptor, values, 4);
            }
        }
        	    
        /* We are done writing the load file.  Close it. */
        OS_close(FileDescriptor);
    }
}


void Create_Input_File1(void)
{
/*	FILE *to; */
	int8 values[4] = {25,30,35,40};
	char tableName[30];
	CFE_FS_Header_t    StdFileHeader;
    CFE_TBL_File_Hdr_t TblFileHeader;
    int32              FileDescriptor;
    int32              Status;
    int32              EndianCheck = 0x01020304;

/*	to = fopen("/tt_table_values1.dat",  "wb");
	if (to == 0)
	{
		UTF_error("Error opening file /tt_table_values1.dat to write\n");
		UTF_exit();
	}

	fwrite(values, sizeof(int8), 4, to);	
	
	fclose(to); */
	
	strcpy (tableName, "TT.FourNumbers");

	/* Clear Header of any garbage before copying content */
    CFE_PSP_MemSet(&StdFileHeader, 0, sizeof(CFE_FS_Header_t));
    CFE_PSP_MemSet(&TblFileHeader, 0, sizeof(CFE_TBL_File_Hdr_t));

	/* Create a new dump file, overwriting anything that may have existed previously */
    FileDescriptor = OS_creat("/ram/tt_table_values1.dat", OS_WRITE_ONLY);

    if (FileDescriptor >= OS_FS_SUCCESS)
    {
        /* Initialize the standard cFE File Header for the Dump File */
       	StdFileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
       	strcpy(&StdFileHeader.Description[0], "Table Load File");

       	/* Output the Standard cFE File Header to the Dump File */
       	Status = CFE_FS_WriteHeader(FileDescriptor, &StdFileHeader);

        if (Status == sizeof(CFE_FS_Header_t))
       	{
           	/* Initialize the Table Image Header for the Dump File */
           	CFE_PSP_MemCpy(TblFileHeader.TableName, tableName, CFE_TBL_MAX_FULL_NAME_LEN);
           	TblFileHeader.Offset = 0;
            TblFileHeader.NumBytes = 4;
    	    TblFileHeader.Reserved = 0;
            
           	/* Determine if this is a little endian processor */
           	if ((*(char *)&EndianCheck) == 0x04)
           	{
	        	CFE_TBL_ByteSwapTblHeader(&TblFileHeader);
    	    }

            /* Output the Table Image Header to the Dump File */
           	Status = OS_write(FileDescriptor, &TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));

           	/* Make sure the header was output completely */
           	if (Status == sizeof(CFE_TBL_File_Hdr_t))
           	{
	        	/* Output the requested data to the dump file */
    	        /* Output the active table image data to the dump file */
        	    Status = OS_write(FileDescriptor, values, 4);
            }
        }
        	    
        /* We are done writing the load file.  Close it. */
        OS_close(FileDescriptor);
    }
}

void Create_Input_File2(void)
{
/*	FILE *to; */
	int8 values[4] = {45,50,55,60};
	char tableName[30];
	CFE_FS_Header_t    StdFileHeader;
    CFE_TBL_File_Hdr_t TblFileHeader;
    int32              FileDescriptor;
    int32              Status;
    int32              EndianCheck = 0x01020304;

/*	to = fopen("/tt_table_values2.dat",  "wb");
	if (to == 0)
	{
		UTF_error("Error opening file /tt_table_values2.dat to write\n");
		UTF_exit();
	}

	fwrite(values, sizeof(int8), 4, to);	
	
	fclose(to); */

	strcpy (tableName, "TT.FourNumbers");

	/* Clear Header of any garbage before copying content */
    CFE_PSP_MemSet(&StdFileHeader, 0, sizeof(CFE_FS_Header_t));
    CFE_PSP_MemSet(&TblFileHeader, 0, sizeof(CFE_TBL_File_Hdr_t));

	/* Create a new dump file, overwriting anything that may have existed previously */
    FileDescriptor = OS_creat("/ram/tt_table_values2.dat", OS_WRITE_ONLY);

    if (FileDescriptor >= OS_FS_SUCCESS)
    {
        /* Initialize the standard cFE File Header for the Dump File */
       	StdFileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
       	strcpy(&StdFileHeader.Description[0], "Table Load File");

       	/* Output the Standard cFE File Header to the Dump File */
       	Status = CFE_FS_WriteHeader(FileDescriptor, &StdFileHeader);

        if (Status == sizeof(CFE_FS_Header_t))
       	{
           	/* Initialize the Table Image Header for the Dump File */
           	CFE_PSP_MemCpy(TblFileHeader.TableName, tableName, CFE_TBL_MAX_FULL_NAME_LEN);
           	TblFileHeader.Offset = 0;
            TblFileHeader.NumBytes = 4;
    	    TblFileHeader.Reserved = 0;
            
           	/* Determine if this is a little endian processor */
           	if ((*(char *)&EndianCheck) == 0x04)
           	{
	        	CFE_TBL_ByteSwapTblHeader(&TblFileHeader);
    	    }

            /* Output the Table Image Header to the Dump File */
           	Status = OS_write(FileDescriptor, &TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));

           	/* Make sure the header was output completely */
           	if (Status == sizeof(CFE_TBL_File_Hdr_t))
           	{
	        	/* Output the requested data to the dump file */
    	        /* Output the active table image data to the dump file */
        	    Status = OS_write(FileDescriptor, values, 4);
            }
        }
        	    
        /* We are done writing the load file.  Close it. */
        OS_close(FileDescriptor);
    }

}

void Create_Input_File3(void)
{
/*	FILE *to; */
	int8 values[4] = {65,70,75,80};
	char tableName[30];
	CFE_FS_Header_t    StdFileHeader;
    CFE_TBL_File_Hdr_t TblFileHeader;
    int32              FileDescriptor;
    int32              Status;
    int32              EndianCheck = 0x01020304;

/*	to = fopen("/tt_table_values3.dat",  "wb");
	if (to == 0)
	{
		UTF_error("Error opening file /tt_table_values3.dat to write\n");
		UTF_exit();
	}

	fwrite(values, sizeof(int8), 4, to);	
	
	fclose(to); */

	strcpy (tableName, "TT.FourNumbers");

	/* Clear Header of any garbage before copying content */
    CFE_PSP_MemSet(&StdFileHeader, 0, sizeof(CFE_FS_Header_t));
    CFE_PSP_MemSet(&TblFileHeader, 0, sizeof(CFE_TBL_File_Hdr_t));

	/* Create a new dump file, overwriting anything that may have existed previously */
    FileDescriptor = OS_creat("/ram/tt_table_values3.dat", OS_WRITE_ONLY);

    if (FileDescriptor >= OS_FS_SUCCESS)
    {
        /* Initialize the standard cFE File Header for the Dump File */
       	StdFileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
       	strcpy(&StdFileHeader.Description[0], "Table Load File");

       	/* Output the Standard cFE File Header to the Dump File */
       	Status = CFE_FS_WriteHeader(FileDescriptor, &StdFileHeader);

        if (Status == sizeof(CFE_FS_Header_t))
       	{
           	/* Initialize the Table Image Header for the Dump File */
           	CFE_PSP_MemCpy(TblFileHeader.TableName, tableName, CFE_TBL_MAX_FULL_NAME_LEN);
           	TblFileHeader.Offset = 0;
            TblFileHeader.NumBytes = 4;
    	    TblFileHeader.Reserved = 0;
            
           	/* Determine if this is a little endian processor */
           	if ((*(char *)&EndianCheck) == 0x04)
           	{
	        	CFE_TBL_ByteSwapTblHeader(&TblFileHeader);
    	    }

            /* Output the Table Image Header to the Dump File */
           	Status = OS_write(FileDescriptor, &TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));

           	/* Make sure the header was output completely */
           	if (Status == sizeof(CFE_TBL_File_Hdr_t))
           	{
	        	/* Output the requested data to the dump file */
    	        /* Output the active table image data to the dump file */
        	    Status = OS_write(FileDescriptor, values, 4);
            }
        }
        	    
        /* We are done writing the load file.  Close it. */
        OS_close(FileDescriptor);
    }

}

void print_housekeeping (uint8 source, void *packet)
{

    TT_HkPacket_t *TT_HKStatusPkt =(TT_HkPacket_t *)packet;

    /* Output the TT housekeeping data */
    UTF_put_text("TT HOUSEKEEPING DATA:\n");

    UTF_put_text("command_counter                  = %d\n",   TT_HKStatusPkt->CmdCount);
    UTF_put_text("command_error_counter            = %d\n",   TT_HKStatusPkt->ErrCount);
    UTF_put_text("tbl update count 				= %d\n",   TT_AppData.HkPacket.UpdateCount);
    
}

void time_hook(void)
{
	UTF_set_sim_time(UTF_get_sim_time() + 1.0);
}


int main(void)
{
	char AppName[10];
	strcpy(AppName, "TT");
	
	/********************************/
	/* Set up to read in script    */
	/********************************/

	UTF_add_input_file(CFE_TT_CMD_PIPE, "ttscript1.in");
	TT_AppData.CmdPipe = CFE_TT_CMD_PIPE;  /* Hook for application code */
	UTF_add_special_command("LOAD_TABLE_FROM_GROUND", UTF_SCRIPT_LoadTableFromGround);
		
	/********************************/
	/* Set up output file           */
	/********************************/
	UTF_set_output_filename("test_tbl_api2.ActualOutput");
	UTF_set_packet_handler(CFE_TT_HK_TLM_MID, print_housekeeping);
	
	UTF_add_volume("/", "ram", FS_BASED, FALSE, FALSE, TRUE, "RAM", "/ram", 0);

	/**************************************************/
	/* Initialize time data structures                */
	/**************************************************/
	UTF_init_sim_time(0.0);
	UTF_OSAPI_set_function_hook(OS_GETLOCALTIME_HOOK, time_hook);

	/**************************************************/
	/* Call utility to register task TT with          */
	/* 		Executive Services.                         */
	/**************************************************/
	UTF_ES_InitAppRecords();
	UTF_ES_AddAppRecord("TT",0);	
    CFE_ES_RegisterApp();

	/* Create Input Files */
	Create_Input_File0();
	Create_Input_File1();
	Create_Input_File2();
	Create_Input_File3();

	/**************************************************/
	/* Initialize Table Services data structures      */
	/**************************************************/	
    /* Initialize the CDS */
    CFE_ES_CDS_EarlyInit();

	CFE_TBL_EarlyInit();

	/********************************/
	/* Call Main procedure for table owner application*/
	/********************************/	
	TT_AppMain();
	
	return 0;
}




