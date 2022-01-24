###############################################################################
# Kit Scheduler (KIT_SCH) Application Functional Test
#
# Notes:
#   1. Loaded by kit test suite so it can assume common utilities have been 
#      preloaded.
#
# License:
#   Written by David McComas, licensed under the copyleft GNU General Public
#   License (GPL).
#
###############################################################################

require 'app_func_test'

class KitSchFuncTest < Cosmos::Test
 
   include AppFuncTest
   
   def initialize
      super()
      app_func_test_init("KIT_SCH", Fsw::MsgId::KIT_SCH_CMD_MID)
   end

   def setup
      status_bar("setup")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
      wait(2)
   end

   def teardown
      status_bar("teardown")
      puts "Running #{Cosmos::Test.current_test_suite}:#{Cosmos::Test.current_test}:#{Cosmos::Test.current_test_case}"
      wait(2)
   end

   def helper_method
   end

end # Class KitSchFuncTest
