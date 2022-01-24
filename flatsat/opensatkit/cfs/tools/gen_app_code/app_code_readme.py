#========================================================================================
# File:    app_code_readme.py
# Author:  Tam Ngo/JSC
# Date:    2012-02-22
#========================================================================================

import os, app_code_configs, app_utils

#========================================================================================

# Global Variables - only global to this file
g_OutDir = "."

#========================================================================================

def inits():
    global g_OutDir

    g_OutDir = app_code_configs.get_outdir()

#========================================================================================

def construct_readme_content():
    content = """\
#######################################################################################
#
# File:  ReadMe.txt
# Desc:  This file describes the data flow of a CFS application.
#
#######################################################################################

A CFS application runs as a child thread in non-realtime OS, like Linux, and as a task
in realtime OS, like VxWorks.

Once invoked (automatically done by the CFE services), the application first initializes
its data, including creations and registrations of message pipes and tables, subscriptions
to other applications' output data, etc.  Then it enters a main loop, awaiting for
messages to respond to.  (From hereon, this will be referring to as listening mode.)

When in listening mode, there are 2 specific messages the application should respond to:

1) <app-name>_WAKEUP_MID

   This message is normally sent by the CFS message scheduler.  Only upon receiving
   this message would the application start doing work.  When woken up, the application
   will first process and respond to all incoming commands.  Then, it will process 
   all incoming data.  Next, it performs 1-cycle worth of data processing.  And last,
   it sends out its output data (<app-name>_OUT_DATA_MID) before going back to the 
   listening mode.

   The application's output data is published at its running rate.  It should contain
   data to be recorded on-board (not downlinked) and data to be consumed by other
   applications.

2) <app-name>_SEND_HK_MID

   This message is normally sent by the CFS message scheduler.  Upon receiving this
   message, the application sends out its latest housekepping data (<app-name>_HK_TLM_MID)
   to be downlinked to the ground.  Then it goes back to the listening mode.

   The application's housekeeping data is published only upon receiving an
   <app-name>_SEND_HK_MID message.  It should contain ONLY essential data that the ground
   system needs in order to run the mission effectively.


"""

    return content
    
#========================================================================================

def generate_readme(tgtApp):
    global g_OutDir

    # Open file
    filePath = os.path.join(g_OutDir, tgtApp.lower(), "fsw", "src", "ReadMe.txt")
    file_exist_flg = os.path.exists(filePath)
    fileHdl = app_utils.open_file_for_writing(filePath, False)

    # Write to file
    fileHdl.write(construct_readme_content())

    # Close file
    app_utils.close_file_from_writing(fileHdl, filePath, file_exist_flg)

#========================================================================================
# End of app_code_readme.py
#========================================================================================

