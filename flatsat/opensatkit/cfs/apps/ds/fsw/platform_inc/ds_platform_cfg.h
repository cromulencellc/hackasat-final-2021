/************************************************************************
** File:
**   $Id: ds_platform_cfg.h 1.17.1.3 2015/07/28 14:19:49EDT lwalling Exp  $
**
**  Copyright � 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** Purpose: 
**  The CFS Data Storage (DS) Application platform configuration header file
**
** Notes:
**
** $Log: ds_platform_cfg.h  $
** Revision 1.17.1.3 2015/07/28 14:19:49EDT lwalling 
** Update restoring task enable/disable state comments
** Revision 1.17.1.2 2015/07/28 14:14:45EDT lwalling 
** Add definition for DS_CDS_ENABLE_STATE to allow storage of enable/disable state in CDS
** Revision 1.17.1.1 2015/02/28 17:13:51EST sstrege 
** Added copyright information
** Revision 1.17 2010/11/12 11:33:29EST lwalling 
** Added platform definition for DS_PER_PACKET_PIPE_LIMIT
** Revision 1.16 2010/11/08 14:23:10EST lwalling 
** Add config definition for DS_MOVE_FILES
** Revision 1.15 2010/10/28 11:27:21EDT lwalling 
** Added definition for DS_FILE_HEADER_TYPE
** Revision 1.14 2010/10/26 16:44:37EDT lwalling 
** Move DS_MISSION_REV from local header to platform config file
** Revision 1.13 2010/10/26 16:23:42EDT lwalling 
** Restore default value for DS_DEF_ENABLE_STATE to enabled
** Revision 1.12 2010/10/26 16:17:49EDT lwalling 
** Move DS_DEF_ENABLE_STATE from local header to platform config file
** Revision 1.11 2010/10/26 15:18:48EDT lwalling 
** Change DS_SECS_PER_HK_CYCLE default value from 5 to 4
** Revision 1.10 2010/09/20 11:34:12EDT lwalling 
** Change existing command pipe name from CMD_PIPE to DS_CMD_PIPE
** Revision 1.9 2010/02/23 11:30:35EST lwalling 
** Change application name from DS_APP to DS per CFS naming convention
** Revision 1.8 2009/09/02 15:22:21EDT lwalling 
** Change source filenames to match table names
** Revision 1.7 2009/08/27 16:33:11EDT lwalling 
** Updates from source code review
** Revision 1.6 2009/08/04 14:08:25EDT lwalling 
** Minor cleanup prior to code review - change comments starting slash asterisk to slash asterisk asterisk
** Revision 1.5 2009/07/15 10:11:24EDT lwalling 
** Moved some definitions in platform cfg file to app hdr file, added more doxygen comments
** Revision 1.4 2009/05/26 13:46:15EDT lwalling 
** Initial version of public header files
** Revision 1.3 2009/04/18 09:36:12EDT dkobe 
** Corrected doxygen aliases used in code
** Revision 1.2 2008/12/04 09:41:22EST rmcgraw 
** DCR4669:1 Changed table names to match second field in table header
** Revision 1.1 2008/11/25 11:35:05EST rmcgraw 
** Initial revision
** Member added to CFS project
**
*************************************************************************/
#ifndef _ds_platform_cfg_h_
#define _ds_platform_cfg_h_

#include "cfe_platform_cfg.h" //dcm

/*
** File Table Indices
*/

#define FILE_ALL_EVENTS       0
#define FILE_CFE_APP_HK_PKTS  1
#define FILE_ALL_APP_HK_PKTS  2
#define FILE_ALL_APP_TLM_PKTS 3
#define FILE_ALL_HW_TLM_PKTS  4
#define FILE_CFE_APP_TLM_PKTS 5


/**
**  \dscfg Destination File Table -- logical table name
**
**  \par Description:
**       This parameter defines the name of the DS Destination File
**       Table when referenced via cFE Table Services.  Note that this
**       parameter is not a filename, it is the table specific portion
**       of the logical name.  The entire logical name for this table
**       is "DS.FILE_TBL".
**
**  \par Limits:
**       The string length (including string terminator) cannot exceed
**       #CFE_TBL_MAX_NAME_LENGTH.  (limit is not verified)
*/
#define DS_DESTINATION_TBL_NAME         "FILE_TBL"


/**
**  \dscfg Destination File Table -- default table filename
**
**  \par Description:
**       This parameter defines the default filename for the
**       Destination File Table.
**
**  \par Limits:
**       The string length (including string terminator) cannot exceed
**       #OS_MAX_PATH_LEN.  (limit is not verified)
*/
#define DS_DEF_DEST_FILENAME            "/cf/ds_file_tbl.tbl"


/**
**  \dscfg Destination File Table -- number of files
**
**  \par Description:
**       This parameter defines the size of the DS Destination File
**       Table by setting the number of file entries in the table.  The
**       number should be large enough to provide an entry for all the
**       destination files defined for the project.  Maintenance will
**       be simplified if file index 'n' always describes the same
**       file - even if that file is not in use at the present time.
**
**  \par Limits:
**       The number must be greater than zero but there is no upper
**       enforced limit for this parameter.
*/
#define DS_DEST_FILE_CNT               16


/**
**  \dscfg Destination File Table -- pathname buffer size
**
**  \par Description:
**       This parameter further defines the size of the Destination
**       File Table by setting the size of the pathname buffer for
**       each file entry.  Note that the buffer must contain both
**       the string and the string terminator - so the max string
**       length is one less than the buffer size.
**
**  \par Limits:
**       The buffer size must be greater than zero and a multiple
**       of four bytes for alignment.  The value cannot exceed the
**       maximum filename size allowed by the OS (#OS_MAX_PATH_LEN).
*/
#define DS_PATHNAME_BUFSIZE             OS_MAX_PATH_LEN


/**
**  \dscfg Destination File Table -- basename buffer size
**
**  \par Description:
**       This parameter further defines the size of the Destination
**       File Table by setting the size of the basename buffer for
**       each file entry.  Note that the buffer must contain both
**       the string and the string terminator - so the max string
**       length is one less than the buffer size.
**
**  \par Limits:
**       The buffer size must be greater than zero and a multiple
**       of four bytes for alignment.  The value cannot exceed the
**       maximum filename size allowed by the OS (#OS_MAX_PATH_LEN).
*/
#define DS_BASENAME_BUFSIZE             OS_MAX_PATH_LEN


/**
**  \dscfg Destination File Table -- extension buffer size
**
**  \par Description:
**       This parameter further defines the size of the Destination
**       File Table by setting the size of the extension buffer for
**       each file entry.  Note that the buffer must contain both
**       the string and the string terminator - so the max string
**       length is one less than the buffer size.
**
**  \par Limits:
**       The buffer size must be greater than zero and a multiple
**       of four bytes for alignment.  The value cannot exceed the
**       maximum filename size allowed by the OS (#OS_MAX_PATH_LEN).
*/
#define DS_EXTENSION_BUFSIZE            8


/**
**  \dscfg Packet Filter Table -- logical table name
**
**  \par Description:
**       This parameter defines the name of the DS Packet Filter Table
**       when referenced via cFE Table Services.  Note that this
**       parameter is not a filename, it is the table specific portion
**       of the logical name.  The entire logical name for this table
**       is "DS.FILTER_TBL".
**
**  \par Limits:
**       The string length (including string terminator) cannot exceed
**       #CFE_TBL_MAX_NAME_LENGTH.  (limit is not verified)
*/
#define DS_FILTER_TBL_NAME              "FILTER_TBL"


/**
**  \dscfg Packet Filter Table -- default table filename
**
**  \par Description:
**       This parameter defines the default filename for the
**       Packet Filter Table.
**
**  \par Limits:
**       The string length (including string terminator) cannot exceed
**       #OS_MAX_PATH_LEN.  (limit is not verified)
*/
#define DS_DEF_FILTER_FILENAME          "/cf/ds_filter_tbl.tbl"


/**
**  \dscfg Packet Filter Table -- number of packets
**
**  \par Description:
**       This parameter defines the size of the DS Packet Filter Table
**       by setting the number of packet entries in the table.  The
**       number should be large enough to provide an entry for each
**       command and telemetry packet subject to data storage.
**
**  \par Limits:
**       The number must be greater than zero but there is no upper
**       enforced limit for this parameter.  However, setting the
**       size equal to the number of packets that might be subject
**       to data storage, rather than the total number of packets
**       defined for this project, will reduce the table file size,
**       possibly significantly.
*/
#define DS_PACKETS_IN_FILTER_TABLE      256


/**
**  \dscfg Packet Filter Table -- filters per packet
**
**  \par Description:
**       This parameter further defines the size of the DS Packet
**       Filter Table by setting the number of filters per packet
**       entry.  This is the maximum number of destination files
**       to which a single packet can be written (at one time).
**
**  \par Limits:
**       The number of filters per packet must be greater than zero
**       and not greater than #DS_DEST_FILE_CNT.
*/
#define DS_FILTERS_PER_PACKET           4


/**
**  \dscfg Common Table File -- descriptor text buffer size
**
**  \par Description:
**       This parameter defines the size of the Descriptor Text
**       fields in both the Destination File Table and the Packet
**       Filter Table.  The buffer includes the string terminator.
**
**  \par Limits:
**       The buffer size must be greater than zero and a multiple
**       of four bytes for alignment.  There is no upper limit.
*/
#define DS_DESCRIPTOR_BUFSIZE           32


/**
**  \dscfg Filename Sequence Count -- number of digits
**
**  \par Description:
**       This parameter defines the number of filename sequence
**       count digits used when the filename type has been set to
**       "count" rather than "time".  Sequence counts are padded
**       with leading zero's to create fixed length strings.
**
**  \par Limits:
**       The number of sequence count digits must be greater than
**       zero - even if there is no intention of later setting
**       the filename type to "count".  This value should match
**       the definition for #DS_MAX_SEQUENCE_COUNT, below.
*/
#define DS_SEQUENCE_DIGITS              8


/**
**  \dscfg Filename Sequence Count -- max counter value
**
**  \par Description:
**       This parameter defines the maximum value a file sequence
**       count will reach before rollover.
**
**  \par Limits:
**       The value must be greater than zero and should not have
**       more digits than the number of sequence count digits
**       defined for #DS_SEQUENCE_DIGITS, above.
*/
#define DS_MAX_SEQUENCE_COUNT           99999999


/**
**  \dscfg Data Storage File -- total filename size
**
**  \par Description:
**       This parameter defines the maximum size of a filename after
**       combining the pathname, basename, sequence and extension.
**
**  \par Limits:
**       The buffer size must be greater than zero and a multiple
**       of four bytes for alignment.  The buffer size (including
**       string terminator) cannot exceed #OS_MAX_PATH_LEN.
*/
#define DS_TOTAL_FNAME_BUFSIZE          OS_MAX_PATH_LEN


/**
**  \dscfg Data Storage File -- cFE file header sub-type
**
**  \par Description:
**       This parameter defines a mission-specific value that is used
**       to identify a Data Storage file.
**
**  \par Limits:
**       The file header data type for the value is 32 bits unsigned,
**       thus the value can be anything from zero to 4,294,967,295.
**       (limit is not verified)
*/
#define DS_FILE_HDR_SUBTYPE             12345


/**
**  \dscfg Data Storage File -- cFE file header description
**
**  \par Description:
**       This parameter defines a mission-specific text string that
**       may be used to identify Data Storage files.
**
**  \par Limits:
**       The string length (including string terminator) cannot exceed
**       #CFE_FS_HDR_DESC_MAX_LEN.  (limit is not verified)
*/
#define DS_FILE_HDR_DESCRIPTION         "DS data storage file"


/**
**  \dscfg Data Storage File -- minimum size limit
**
**  \par Description:
**       This parameter defines the lower limit for commands and
**       table entries that define the size (in bytes) when a data
**       storage file is automatically closed.
**
**  \par Limits:
**       None -- however, the user should be cautioned that a very
**       small value will allow an size limit that closes files too
**       frequently, while a very large value will effectively
**       prevent files from ever being closed due to size.
*/
#define DS_FILE_MIN_SIZE_LIMIT          1024


/**
**  \dscfg Data Storage File -- minimum age limit
**
**  \par Description:
**       This parameter defines the lower limit for commands and
**       table entries that define the age (in seconds) when a data
**       storage file is automatically closed.
**
**  \par Limits:
**       None -- however, the user should be cautioned that a very
**       small value will allow an age limit that closes files too
**       frequently, while a very large value will effectively
**       prevent files from ever being closed due to age.
*/
#define DS_FILE_MIN_AGE_LIMIT           60


/**
**  \dscfg Application Pipe Name
**
**  \par Description:
**       This parameter defines a portion of the logical name used
**       during the creation of the DS input pipe.  The logical name
**       may also be used as an identifier when accessing status for
**       the pipe via cFE Software Bus Services.  The entire logical
**       name is "DS.DS_CMD_PIPE".
**
**  \par Limits:
**       The string length (including string terminator) cannot
**       exceed #OS_MAX_API_NAME.  (limit is not verified)
*/
#define DS_APP_PIPE_NAME                "DS_CMD_PIPE"


/**
**  \dscfg Application Pipe Depth 
**
**  \par Description:
**       This parameter defines the depth of the DS input pipe.  The
**       depth should be deep enough to accommodate all of the DS
**       command packets and all of the subscribed telemetry packets
**       that might be generated by applications with a priority
**       higher than the DS application.
**
**  \par Limits:
**		 The value must be greater than zero and cannot exceed the
**       definition of #CFE_SB_MAX_PIPE_DEPTH.
*/
#define DS_APP_PIPE_DEPTH               256


/**
**  \dscfg Make DS Tables Critical 
**
**  \par Description:
**       Set this parameter to a value of one to make the DS tables critical,
**       otherwise set to zero.
**
**  \par Limits
**       This parameter must be set to zero or one.
*/
#define DS_MAKE_TABLES_CRITICAL         0


/**
**  \dscfg Housekeeping Request Frequency
**
**  \par Description:
**       Set this parameter equal to the number of seconds between
**       housekeeping request commands.  This number is mission
**       specific and must match the frequency used by the source
**       of the command - often the scheduler task.  The value is
**       used by the DS application to measure file age and also
**       as a factor in the calculation of file growth rates.
**
**  \par Limits
**       This parameter must be greater than zero.
*/
#define DS_SECS_PER_HK_CYCLE            4


/**
**  \dscfg Default DS Packet Processor State 
**
**  \par Description:
**       Set this parameter to a value of one and DS will begin to
**       process packets immediately on startup.  Set the value to
**       zero and DS will ignore data storage packets until receipt
**       of a valid #DS_ENABLE command.
**
**  \par Limits
**       This parameter must be set to zero or one.
*/
#define DS_DEF_ENABLE_STATE             1


/**
**  \dscfg Save DS Packet Processor State in Critical Data Store
**
**  \par Description:
**       Set this parameter to a value of one and DS will restore
**       the previous Packet Processor enable/disable state after
**       a processor reset.  This setting does not affect the
**       Packet Processor enable/disable state set at power-on
**       which is controlled by #DS_DEF_ENABLE_STATE.  Set the
**       value to zero and DS will set the enable/disable state
**       as described for #DS_DEF_ENABLE_STATE following any reset.
**
**  \par Limits
**       This parameter must be set to zero or one.
*/
#define DS_CDS_ENABLE_STATE             1


/** \dscfg Mission specific version number for DS application
**  
**  \par Description:
**       An application version number consists of four parts:
**       major version number, minor version number, revision
**       number and mission specific revision number. The mission
**       specific revision number is defined here and the other
**       parts are defined in "ds_version.h".
**
**  \par Limits:
**       Must be defined as a numeric value that is greater than
**       or equal to zero.
*/
#define DS_MISSION_REV                  0


/**
**  \dscfg File Header Type Selection 
**
**  \par Description:
**       Set this parameter to select the type of file header that
**       will be the first record written to each Data Storage File.
**
**  \par Limits
**       This parameter must be set to one of the following:
**       0 = none -- set this value to have no file header
**       1 = CFE -- set this value to use CFE file headers
**       2 = GPM -- set this value to use GPM file headers
*/
#define DS_FILE_HEADER_TYPE             1


/**
**  \dscfg Move Files to Downlink Directory After Close Selection 
**
**  \par Description:
**       Set this parameter to enable the code and structures to
**       automatically move DS files to another directory after
**       closing the files.  The intended use for this setting is
**       to move files from a working directory into a directory
**       from which the files can be downlinked.  Note that even
**       after enabling this feature, files will not be moved if
**       the move pathname in the Destination File Table is null.
**
**  \par Limits
**       This parameter must be set to one of the following:
**       TRUE  = add move pathname field to Destination File Table
**       FALSE = do not add move pathname to Destination File Table
*/
#define DS_MOVE_FILES                   FALSE


/**
**  \dscfg Application Per Packet Pipe Limit
**
**  \par Description:
**       This parameter defines the per packet pipe limit.  This is
**       the max number of packets with the same Message ID that may
**       be in the DS input pipe at any one time.  This value should
**       be large enough to accommodate a burst of packets (usually
**       event packets) plus a suitable margin.
**
**  \par Limits:
**		 The value must be greater than zero and cannot exceed the
**       definition of #DS_APP_PIPE_DEPTH.
*/
#define DS_PER_PACKET_PIPE_LIMIT        50


#endif /* _ds_platform_cfg_h_ */

/************************/
/*  End of File Comment */
/************************/
