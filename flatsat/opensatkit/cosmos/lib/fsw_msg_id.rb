###############################################################################
# Message Identifiers
#
# Notes:
#   1. All parameters types are strings since the typical caller context is to
#      use the message ID in a cmd or tlm string definition
# 
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require "erb"

module Fsw
   module MsgId

      # 
      # Default value that shouldn't be used but is made available so an assignment
      # can be made in ambiguous situations. This value should be rejected by the 
      # cFS since it is unused and it will be an obvious message id value error.
      #
      UNUSED_MID = "0x0000"  

      ##########################
      ## cFE App Message IDs  ##
      ##########################

      # Executive Services
      
      CFE_ES_CMD_MID          = "0x1806"
      CFE_ES_HK_TLM_MID       = "0x0800"
      CFE_ES_APP_TLM_MID      = "0x080B"
      CFE_ES_SHELL_TLM_MID    = "0x080F"
      CFE_ES_MEMSTATS_TLM_MID = "0x0810"

      # Executive Services (CPU2)
      CFE_ES2_CMD_MID          = "0x1826"
      CFE_ES2_HK_TLM_MID       = "0x0820"
      CFE_ES2_APP_TLM_MID      = "0x082B"
      CFE_ES2_SHELL_TLM_MID    = "0x082F"
      CFE_ES2_MEMSTATS_TLM_MID = "0x0830"
  
      # Event Services
      
      CFE_EVS_CMD_MID         = "0x1801"
      CFE_EVS_HK_TLM_MID      = "0x0801"
      CFE_EVS_EVENT_MSG_MID   = "0x0808"

      CFE_EVS2_CMD_MID         = "0x1812"
      CFE_EVS2_HK_TLM_MID      = "0x0811"
      CFE_EVS2_EVENT_MSG_MID   = "0x0818"
  
      # Software Bus
      
      CFE_SB_CMD_MID          = "0x1803"  
      CFE_SB_HK_TLM_MID       = "0x0803"
      CFE_SB_STATS_TLM_MID    = "0x080A"
      CFE_SB_ALLSUBS_TLM_MID  = "0x080D"
      CFE_SB_ONESUB_TLM_MID   = "0x080E"
  
      # Table Service
      
      CFE_TBL_CMD_MID         = "0x1804"
      CFE_TBL_HK_TLM_MID      = "0x0804"
      CFE_TBL_REG_TLM_MID     = "0x080C"
  
      # Time Service
      
      CFE_TIME_CMD_MID        = "0x1805"
      CFE_TIME_HK_TLM_MID     = "0x0805"
      CFE_TIME_DIAG_TLM_MID   = "0x0806"

      #########################
      ## cFS App Message IDs ##
      #########################
  
      # CFDP
  
      CF_CMD_MID              = "0x18B3"
      CF_SEND_HK_MID          = "0x18B4" 
      CF_WAKE_UP_REQ_CMD_MID  = "0x18B5"
      CF_SPARE1_CMD_MID       = "0x18B6"
      CF_SPARE2_CMD_MID       = "0x18B7"
      CF_SPARE3_CMD_MID       = "0x18B8"
      CF_SPARE4_CMD_MID       = "0x18B9"
      CF_SPARE5_CMD_MID       = "0x18BA"
      CF_INCOMING_PDU_MID     = "0x1FFD"

      CF_HK_TLM_MID           = "0x08B0"
      CF_TRANS_TLM_MID        = "0x08B1"
      CF_CONFIG_TLM_MID       = "0x08B2"
      CF_SPARE0_TLM_MID       = "0x08B3"
      CF_SPARE1_TLM_MID       = "0x08B4"
      CF_SPARE2_TLM_MID       = "0x08B5"
      CF_SPARE3_TLM_MID       = "0x08B6"
      CF_SPARE4_TLM_MID       = "0x08B7"
      
      CF_SPACE_TO_GND_PDU_MID = "0x0FFD"

      #########################
  
      # CFDP2
  
      CF2_CMD_MID              = "0x18C3"
      CF2_SEND_HK_MID          = "0x18C4" 
      CF2_WAKE_UP_REQ_CMD_MID  = "0x18C5"
      CF2_SPARE1_CMD_MID       = "0x18C6"
      CF2_SPARE2_CMD_MID       = "0x18C7"
      CF2_SPARE3_CMD_MID       = "0x18C8"
      CF2_SPARE4_CMD_MID       = "0x18C9"
      CF2_SPARE5_CMD_MID       = "0x18CA"
      CF2_INCOMING_PDU_MID     = "0x1FFC"

      CF2_HK_TLM_MID           = "0x08C0"
      CF2_TRANS_TLM_MID        = "0x08C1"
      CF2_CONFIG_TLM_MID       = "0x08C2"
      CF2_SPARE0_TLM_MID       = "0x08C3"
      CF2_SPARE1_TLM_MID       = "0x08C4"
      CF2_SPARE2_TLM_MID       = "0x08C5"
      CF2_SPARE3_TLM_MID       = "0x08C6"
      CF2_SPARE4_TLM_MID       = "0x08C7"
      
      CF2_SPACE_TO_GND_PDU_MID = "0x0FFC"
      
      # Checksum
  
      CS_CMD_MID              = "0x189F"  
      CS_HK_TLM_MID           = "0x08A4"

      # Data Storage

      DS_CMD_MID              = "0x18BB"
      DS_HK_TLM_MID           = "0x08B8"
      DS_DIAG_TLM_MID         = "0x08B9"
  
  
      # File Manager  
  
      FM_CMD_MID              = "0x188C"
      FM_HK_TLM_MID           = "0x088A"
      FM_FILE_INFO_TLM_MID    = "0x088B"  
      FM_DIR_LIST_TLM_MID     = "0x088C"
      FM_OPEN_FILES_TLM_MID   = "0x088D"
      FM_FREE_SPACE_TLM_MID   = "0x088E"

      FM2_CMD_MID              = "0x184C"
      FM2_HK_TLM_MID           = "0x084A"
      FM2_FILE_INFO_TLM_MID    = "0x084B"  
      FM2_DIR_LIST_TLM_MID     = "0x084C"
      FM2_OPEN_FILES_TLM_MID   = "0x084D"
      FM2_FREE_SPACE_TLM_MID   = "0x084E"

      # Housekeeping
  
      HK_CMD_MID              = "0x189A"
      HK_HK_TLM_MID           = "0x089B"
      HK_COMBINED_PKT1_MID    = "0x089C"
      HK_COMBINED_PKT2_MID    = "0x089D"
      HK_COMBINED_PKT3_MID    = "0x089E"
      HK_COMBINED_PKT4_MID    = "0x089F"

      # Health & Safety
  
      HS_CMD_MID              = "0x18AE"
      HS_HK_TLM_MID           = "0x08AD"

      # Limit Checker
  
      LC_CMD_MID              = "0x18A4"
      LC_SAMPLE_AP_MID        = "0x18A6"
      LC_HK_TLM_MID           = "0x08A7"
  
      # Memory Dwell  
  
      MD_CMD_MID              = "0x1890"
      MD_HK_TLM_MID           = "0x0890"
      MD_DWELL_PKT_MID_1      = "0x0891"
      MD_DWELL_PKT_MID_2      = "0x0892"
      MD_DWELL_PKT_MID_3      = "0x0893"
      MD_DWELL_PKT_MID_4      = "0x0894"

      # Memory Manager
  
      MM_CMD_MID              = "0x1888"
      MM_HK_TLM_MID           = "0x0887"
  
      # Stored Command

      SC_CMD_MID              = "0x18A9"
      SC_1HZ_WAKEUP_MID       = "0x18AB"
      SC_HK_TLM_MID           = "0x08AA"
  
      #########################
      ## Kit App Message IDs ##
      #########################
 
      # Benchmark
  
      BM_CMD_MID     = "0x19F0"
      BM_HK_TLM_MID	= "0x09F0"
      
      # Kit Command Ingest
  
      KIT_CI_CMD_MID    = "0x1884"
      KIT_CI_HK_TLM_MID = "0x0884"

      # Kit Scheduler
  
      KIT_SCH_CMD_MID    = "0x1895"
      KIT_SCH_HK_TLM_MID = "0x0899"
  
      # Kit Telemetry Output

      KIT_TO_CMD_MID           = "0x1880"
      KIT_TO_HK_TLM_MID        = "0x0880"
      KIT_TO_DATA_TYPE_TLM_MID = "0x0881"
      KIT_TO_PKT_TBL_TLM_MID   = "0x0882"


      # OpenSatKit Create App tool
      #
      # - Use cFE's Sample App MIDs as defaults
      #   for the Create App tool  
      #

      OSK_GENAPP_CMD_MID    = "0x1883"
      OSK_GENAPP_HK_TLM_MID = "0x0883"


      ################
      ##   EPHEM    ##
      ################
      EPHEM_CMD_MID           = "0x19DC"
      EPHEM_HK_TLM_MID        = "0x09E2"
      EPHEM_TLM_EPHEM_MID     = "0x09E3"


      ################
      ##    ADCS    ##
      ################
      ADCS_CMD_MID           = "0x19DF"
      ADCS_TLM_HK_MID        = "0x09E5"
      ADCS_TLM_FSW_MID       = "0x09E6"
      ADCS_HW_IMU_TLM_MID    = "0x09E8"
      ADCS_HW_MTR_TLM_MID    = "0x09E9"
      ADCS_HW_RW_TLM_MID     = "0x09EA"
      ADCS_HW_ANALOG_TLM_MID = "0x09EB"
      ADCS_HW_ST_TLM_MID     = "0x09EC"
      ADCS_HW_FSS_TLM_MID    = "0x09ED"

      ################
      ##  SLA_TLM   ##
      ################
      SLA_CMD_MID            = "0x1E08"
      SLA_TLM_TLM_HK_MID     = "0x0E08"

      ################
      ##    COMM    ##
      ################
      COMM_CMD_MID           = "0x19F6"
      COMM_TLM_HK_MID        = "0x09F6"
      COMM_MOD_TLM_MID       = "0x09F7"
      COMM_DEMOD_TLM_MID     = "0x09F8"

      # Trivial File Transfer Protocol
  
      TFTP_CMD_MID    = "0x1900"
      TFTP_HK_TLM_MID = "0x0900" 
   
      # Trivial File Transfer Protocol 2

      TFTP2_CMD_MID    = "0x1902"
      TFTP2_HK_TLM_MID = "0x0902" 

      ################
      ##  SBN-LITE  ##
      ################

      SBN_LITE_CMD_MID    = "0x18FA"
      SBN_LITE_HK_TLM_MID = "0x08FC"
      SBN_LITE_SEND_HK_MID = "0x18FC"
      SBN_LITE_PKT_TBL_TLM_MID = "0x08FE"

      ################
      ##  SBN-LITE2 ##
      ################

      SBN_LITE2_CMD_MID    = "0x18FB"
      SBN_LITE2_HK_TLM_MID = "0x08FF"
      SBN_LITE2_SEND_HK_MID = "0x18FF"
      SBN_LITE2_PKT_TBL_TLM_MID = "0x0903"

      ############
      ##  MQTT  ##
      ############

      MQTT_CMD_MID       = "0x1F50"
      MQTT_HK_TLM_MID    = "0x0F50"
      MQTT_STR32_TLM_MID = "0x0F51"
      
      ###########
      ##  MQTT  ##
      ############

      MQTT2_CMD_MID       = "0x1F52"
      MQTT2_HK_TLM_MID    = "0x0F52"
      MQTT2_STR32_TLM_MID = "0x0F53"

      # Hardware Bus
  
      HB_CMD_MID    = "0x1882"
      HB_HK_TLM_MID = "0x0885"

      ###########
      ##  PDB  ##
      ###########

      PDB_CMD_MID    = "0x1E01"
      PDB_HK_TLM_MID = "0x0E01"
      PDB_CHARGER_TLM_MID = "0x0E02"

      ###############
      ##  EPS MGR  ##
      ###############

      EPS_MGR_CMD_MID = "0x1910"
      EPS_MGR_HK_TLM_MID  = "0x0910"
      EPS_MGR_FSW_TLM_MID = "0x0911"

      #####################
      ## Utility Methods ##
      #####################

      def self.get_msg_val(msg_id)
         begin
            raise ArgumentError, 'Message id argument is not a string' unless msg_id.is_a? String
            if defined? (msg_id) 
               @msg_val = eval(msg_id)  # This will raise an exception if not defined
            end
         rescue
            @msg_val = UNUSED_MID
         end
    
         return @msg_val
    
      end # get_msg_val()
    
   end # module MsgeId
end # module Fsw

