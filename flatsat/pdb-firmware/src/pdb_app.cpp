#include <pdb_app.h>
// #include <ADC.h>
// #include <ADC_util.h>
#include <Bounce.h>
#include "pdb_logger.hpp"

/**
 * Local Function Definitions
*/
bool PDB_NoOpCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);
bool PDB_ResetAppCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);
bool setChannelOutputStateCmd_(uint16_t UserDataLen, const CFE_SB_MsgPtr_t MsgPtr);

//Default address for the BQ25703A device
#define BQ25703ADevaddr    0x6B
const byte Lorro_BQ25703A::BQ25703Aaddr = BQ25703ADevaddr;
Bounce charge_enabled_sw = Bounce(SW1, 10);
Bounce comm_startup_sw = Bounce(SW2, 10);

PdbApp::PdbApp() {
    CFE_SB_InitMsg(&pdbHkPkt_, PDB_HK_TLM_MID, PDB_TLM_HK_LEN, TRUE);
    CFE_SB_InitMsg(&pdbChargerPkt_, PDB_CHARGER_TLM_MID, PDB_TLM_CHARGER_LEN, TRUE);
    ctrlExeCnt = 0;
}

void PdbApp::begin(void) {
    LOG_INFO("PDB Application Begin");


    /*
     * PWR GOOD IO
    */
    pinMode(VR_1_PWR_GOOD, INPUT);
    pinMode(VR_2_PWR_GOOD, INPUT);
    LOG_DEBUG("PDB PWR_GOOD Configured");

    /*
     * LED IO
    */
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    pinMode(LED4, OUTPUT);
    digitalWrite(LED1, HIGH); // OFF
    digitalWrite(LED2, HIGH); // OFF
    digitalWrite(LED3, HIGH); // OFF
    digitalWrite(LED4, HIGH); // OFF
    LOG_DEBUG("PDB LEDs Configured");
    /*
     * SWITCH IO
    */
    pinMode(SW1,INPUT); // Charge enable/disable
    pinMode(SW2,INPUT);  // COMM board on at startup enable/disable
    pinMode(SW3,INPUT); 
    pinMode(SW4,INPUT);
    LOG_DEBUG("PDB Switches Configured");
    /*
     * Initialize Analog Mux
    */
    mux_.begin(AMUX_EN, AMUX_S1, AMUX_S2, AMUX_S3);
    LOG_DEBUG("PDB Analog Mux Configured");

    /*
     * Initialize USBPD STUSB400
    */
    init_USBPD_();
    LOG_DEBUG("PDB USB-PD Configured");

    /*
     * Initialize BQ25703A Charge Controller
    */
    init_BQ25703A_();
    LOG_DEBUG("PDB Battery Charge Controller Configured");

    init_ADC_();
    LOG_DEBUG("PDB ADC Initialized");


    /*
     * OUTPUT CHANNEL IO
    */
    outputChannels.at(0).begin(OUPTUT_CH1_EN, ChannelState::ON, 0, true);
    outputChannels.at(1).begin(OUPTUT_CH2_EN, ChannelState::ON, 1, false);
    outputChannels.at(2).begin(OUPTUT_CH3_EN, ChannelState::OFF, 2, false);
    if(comm_startup_sw.read() ^ 1) {
        LOG_INFO("Starting COMM board at OUTPUT CH 4");
        outputChannels.at(3).begin(OUPTUT_CH4_EN, ChannelState::ON, 3, false);
    } else {
        outputChannels.at(3).begin(OUPTUT_CH4_EN, ChannelState::OFF, 3, false);
    }
    outputChannels.at(4).begin(OUPTUT_CH5_EN, ChannelState::OFF, 4, false);
    outputChannels.at(5).begin(OUPTUT_CH6_EN, ChannelState::OFF, 5, false);
    LOG_DEBUG("PDB Output Channels Configured");
    
    delay(2000);

    /*
     * Initialize Ethernet
    */
    std::string myIp = IP_ADDRESS;
    pdb_comms_.begin(myIp,PORT_RECV,PORT_SEND);

    LOG_INFO("PDB Application Begin Complete");
    
}

void PdbApp::init_ADC_(void) {
    adc_ = new ADC(); // adc object
    pinMode(ANALOG_CH_VOLT_IN, INPUT);
    pinMode(ANALOG_CH_CUR_IN, INPUT);
    pinMode(ANALOG_MISC_IN, INPUT);
    ///// ADC0 ////
    adc_->adc0->setAveraging(16); // set number of averages
    adc_->adc0->setResolution(10); // set bits of resolution
    adc_->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED); // change the conversion speed
    adc_->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED); // change the sampling speed

    ////// ADC1 /////
    adc_->adc1->setAveraging(16); // set number of averages
    adc_->adc1->setResolution(10); // set bits of resolution
    adc_->adc1->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED); // change the conversion speed
    adc_->adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED); // change the sampling speed
    
}

void PdbApp::init_BQ25703A_(void) {

    //Setting the max voltage that the charger will charge the batteries up to.
    //This value gets rounded to multiples of 16mV
    //When setting numerical registers, calling the set_ function
    //will also send the bytes to the device.
    BQ25703Areg.maxChargeVoltage.set_voltage( 12592 );
    delay( 10 );

    //Set the ADC on IBAT and PSYS to record values
    BQ25703Areg.chargeOption1.set_EN_IBAT( 1 );
    BQ25703Areg.chargeOption1.set_EN_PSYS( 1 );
    BQ25703A.writeRegEx( BQ25703Areg.chargeOption1 );
    delay( 10 );

    //Set ADC to make continuous readings. (uses more power)
    BQ25703Areg.aDCOption.set_ADC_CONV( 1 );
    //Set individual ADC registers to read. All have default off.
    BQ25703Areg.aDCOption.set_EN_ADC_VBUS( 1 );
    BQ25703Areg.aDCOption.set_EN_ADC_PSYS( 1 );
    BQ25703Areg.aDCOption.set_EN_ADC_IIN( 1 );
    BQ25703Areg.aDCOption.set_EN_ADC_IDCHG( 1 );
    BQ25703Areg.aDCOption.set_EN_ADC_ICHG( 1 );
    BQ25703Areg.aDCOption.set_EN_ADC_VSYS( 1 );
    BQ25703Areg.aDCOption.set_EN_ADC_VBAT( 1 );
    BQ25703A.writeRegEx( BQ25703Areg.aDCOption );
    delay( 10 );

    //Sets the charge current. This needs to be set before any charging of
    //the batteries starts, as it is defaulted to 0. Any value entered will
    //be rounded to multiples of 64mA.
    BQ25703Areg.chargeCurrent.set_current( 2048 );
    delay( 10 );

}

void PdbApp::init_USBPD_(void) {
    if(usb_pd_.begin(0x28), &Wire) {
        LOG_INFO("STUSB400 Initialization Successful");
        readUsbPdStatus_();
    } else {
        LOG_ERROR("STUSB400 Initialization Failed");
    }
}

bool PdbApp::sendTelemetry(void)
{
    ctrlExeCnt++;
    readChargerStatus_();
    readAnalogTelemetry_();
    sendHkTelemetry_();
    sendChargerTelemetry_();
    return TRUE;
}

int32_t PdbApp::processCommands(void)
{
    CFE_SB_Msg_t*   MsgPtr = NULL;

    bool  ValidCmd = FALSE;
    // char buff[100];

    MsgPtr = pdb_comms_.recvData();

    if(MsgPtr != NULL) {

        uint16_t MsgId = CFE_SB_GetMsgId(MsgPtr);
        uint16_t UserDataLen = CFE_SB_GetUserDataLength(MsgPtr);
        uint16_t FuncCode    = CFE_SB_GetCmdCode(MsgPtr);
        uint32_t Checksum    = CFE_SB_GetChecksum(MsgPtr);

        LOG_INFO("PDB Msg Recv: MsgId: 0x%04X, UserDataLen: %d, FuncCode: %d, Checksum: 0x%04X\n", MsgId, UserDataLen, FuncCode, Checksum);

        // sprintf(buff, "MsgId %u UserDataLen %u FuncCode %u Checksum %lu", MsgId, UserDataLen, FuncCode, Checksum);
        // outputSerial_->println(buff);

        if (CFE_SB_ValidateChecksum(MsgPtr) == TRUE) {
            switch (FuncCode)
            {
            case NOOP_CMD_FC:
                ValidCmd = noopCmd_(UserDataLen, MsgPtr);
                break;
            case RESET_CMD_FC:
                ValidCmd = resetAppCmd_(UserDataLen, MsgPtr);
                break;
            case SET_CHANNEL_OUTPUT:
                ValidCmd = setChannelOutputStateCmd_(UserDataLen, MsgPtr);
            default:
                break;
            }
        } else {
            LOG_INFO("Invalid Checksum");
        }

        if (ValidCmd) {
            validCmdCnt++;
        } else {
            invalidCmdCnt++;
        }
    }

    return ValidCmd;
}
/******************************************************************************
** Function: _noopCmd
**
*/
bool PdbApp::noopCmd_(uint16_t UserDataLen, const CFE_SB_MsgPtr_t MsgPtr)
{
    if ( UserDataLen == 0 ) {
        return TRUE;
        LOG_INFO("PDB NOOP Cmd Recv");
    } else {
        return FALSE;
    }

} /* End _noopCmd() */


/******************************************************************************
** Function: _resetAppCmd
**
*/
bool PdbApp::resetAppCmd_(uint16_t UserDataLen, const CFE_SB_MsgPtr_t MsgPtr)
{
    if (UserDataLen == 0 ) {
        validCmdCnt = 0;
        invalidCmdCnt = 0;
        LOG_INFO("PDB ResetApp Cmd Recv");
        return TRUE;
    } else {
        return FALSE;
    }

} /* End _resetAppCmd() */


/******************************************************************************
** Function: setChannelOutputStateCmd_
**
*/
bool PdbApp::setChannelOutputStateCmd_(uint16_t UserDataLen, const CFE_SB_MsgPtr_t MsgPtr)
{
    // char buff[100];
    bool ret = FALSE;
    if (UserDataLen == PDB_OUTPUT_CHANNEL_CMD_DATA_LEN ) {
        PDB_OutputChannelCmdPkt* cmd = (PDB_OutputChannelCmdPkt*) MsgPtr;
        LOG_INFO("Recieved Set Output Channel State Command. Channel: %d, State: %d", cmd->ChannelId, cmd->State);
        if (cmd->ChannelId >= 0 && cmd->ChannelId <= outputChannels.size()-1) {
            ChannelState newState = static_cast<ChannelState>(cmd->State);
            outputChannels.at(cmd->ChannelId).setState(newState);
            ret = TRUE;
        } else {
            LOG_ERROR("Invalid Outut Channel ID");
        }
    } else {
        LOG_ERROR("Invalide UserDataLength");
    }
    return ret;
}

/******************************************************************************
** Function: sendHkTelemetry_
**
*/
int PdbApp::sendHkTelemetry_(void)
{
    int Status = 0;

    pdbHkPkt_.ValidCmdCnt = validCmdCnt;
    pdbHkPkt_.InvalidCmdCnt = invalidCmdCnt;
    pdbHkPkt_.CtrlExeCnt = ctrlExeCnt;

    size_t i= 0;
    for(i=0; i<outputChannels.size(); i++){
        pdbHkPkt_.ChannelStateSet[i] = outputChannels.at(i).getState();
        pdbHkPkt_.ChannelStateRead[i] = outputChannels.at(i).getGpioState();
    }

    for(i=0; i<6; i++) {
        pdbHkPkt_.ChannelAnalogVoltage[i] = channelAnalogVoltageRaw[i];
        // pdbHkPkt_.ChannelAnalogVoltage[i].converted = channelAnalogVoltage[i];00
        if(i<=5) {
            pdbHkPkt_.ChannelAnalogCurrent[i] = channelAnalogCurrentRaw[i];
            // pdbHkPkt_.ChannelAnalogCurrent[i].converted = channelAnalogCurrent[i];
            pdbHkPkt_.SystemAnalogVoltage[i] = systemAnalogVoltageRaw[i];
            // pdbHkPkt_.SystemAnalogVoltage[i].converted = systemAnalogVoltage[i];
        }
    }

    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &pdbHkPkt_);
    CFE_SB_GenerateChecksum((CFE_SB_MsgPtr_t)&pdbHkPkt_);
    Status = pdb_comms_.sendData(&pdbHkPkt_,PDB_TLM_HK_LEN);

    return Status;

}

/******************************************************************************
** Function: sendChargerTelemetry_
**
*/
int PdbApp::sendChargerTelemetry_(void)
{
    int Status = 0;

    pdbChargerPkt_.ChargeEnabled = chargeEnabled;
    pdbChargerPkt_.MaxChargeVoltage = maxChargeVoltage;
    pdbChargerPkt_.MaxChargeCurrent = maxChargeCurrent;
    pdbChargerPkt_.SystemPowerUtilization = systemPowerUtilization;
    pdbChargerPkt_.VBUS = vBUS;
    pdbChargerPkt_.VBAT = vBAT;
    pdbChargerPkt_.VSYS = vSYS;
    pdbChargerPkt_.ActualChargeCurrent = actualChargeCurrent;
    pdbChargerPkt_.ActualDischargeCurrent = actualDischargeCurrent;

    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &pdbChargerPkt_);
    CFE_SB_GenerateChecksum((CFE_SB_MsgPtr_t)&pdbChargerPkt_);
    Status = pdb_comms_.sendData(&pdbChargerPkt_,PDB_TLM_CHARGER_LEN);

    return Status;

}

/******************************************************************************
** Function: readChargerStatus_(void)
**
*/
bool PdbApp::readChargerStatus_(void) {
    
    chargeEnabled = (charge_enabled_sw.read() ^ 1);
    // Turn on LED2 if charging disabled via switch
    if (chargeEnabled) {
        digitalWrite(LED2, HIGH);
    } else {
        digitalWrite(LED2, LOW);
    }

    //Sets the charge current. This needs to be set before any charging of
    //the batteries starts, as it is defaulted to 0. Any value entered will
    //be rounded to multiples of 64mA.
    if(!chargeEnabled) {
        BQ25703Areg.chargeCurrent.set_current( 0 );
    } else {
        BQ25703Areg.chargeCurrent.set_current( 2048 );
    }

    maxChargeVoltage = BQ25703Areg.maxChargeVoltage.get_voltage();
    delayNanoseconds(50);
    maxChargeCurrent = BQ25703Areg.chargeCurrent.get_current();
    delayNanoseconds(50);
    systemPowerUtilization = BQ25703Areg.aDCVBUSPSYS.get_sysPower();
    delayNanoseconds(50);
    vBUS = BQ25703Areg.aDCVBUSPSYS.get_VBUS();
    delayNanoseconds(50);
    vBAT = BQ25703Areg.aDCVSYSVBAT.get_VBAT();
    delayNanoseconds(50);
    vSYS = BQ25703Areg.aDCVSYSVBAT.get_VSYS();
    delayNanoseconds(50);
    actualChargeCurrent = BQ25703Areg.aDCIBAT_ICHG.get_ICHG();
    delayNanoseconds(50);
    actualDischargeCurrent = BQ25703Areg.aDCIBAT_IDCHG.get_IDCHG();

    // Turn on LED1 if charging
    if (actualChargeCurrent > 0) {
        digitalWrite(LED1, LOW);
    } else {
        digitalWrite(LED1, HIGH);
    }

    return true;

}

void PdbApp::readUsbPdStatus_(void) {

    // Set common capabilities
    // usb_pd_.setUsbCommCapable(false);

    // Only Negotiate for 20.0V or 12.0V
    // usb_pd_.setPowerAbove5vOnly(1);

    /* PDO1
   - Voltage fixed at 5V
   - Current value for PDO1 0-5A, if 0 used, FLEX_I value is used
   - Under Voltage Lock Out (setUnderVoltageLimit) fixed at 3.3V
   - Over Voltage Lock Out (setUpperVoltageLimit) 5-20%
   */
    // usb_pd_.setCurrent(1,6.0);
    // usb_pd_.setUpperVoltageLimit(1,20);
    /* PDO2
    - Voltage 5-20V
    - Current value for PDO2 0-5A, if 0 used, FLEX_I value is used
    - Under Voltage Lock Out (setUnderVoltageLimit) 5-20%
    - Over Voltage Lock Out (setUpperVoltageLimit) 5-20%
    */
    // usb_pd_.setVoltage(2,15.0);
    // usb_pd_.setCurrent(2,2.5);
    // usb_pd_.setLowerVoltageLimit(2,20);
    // usb_pd_.setUpperVoltageLimit(2,20);

    /* PDO3
   - Voltage 5-20V
   - Current value for PDO3 0-5A, if 0 used, FLEX_I value is used
   - Under Voltage Lock Out (setUnderVoltageLimit) 5-20%
   - Over Voltage Lock Out (setUpperVoltageLimit) 5-20%
    */
    // usb_pd_.setVoltage(3,20.0);
    // usb_pd_.setCurrent(3,1.5);
    // usb_pd_.setLowerVoltageLimit(3,20);
    // usb_pd_.setUpperVoltageLimit(3,20);

    // Read settings
    usb_pd_.read();
 
    /* Read settings for PDO1
         - Voltage fixed at 5V
         - Current value for PDO1 0-5A, if 0 used, FLEX_I value is used
         - Under Voltage Lock Out fixed at 3.3V (but will always return 0)
         - Over Voltage Lock Out 5-20%
    */
    usbPdData.usbpd1.Voltage = usb_pd_.getVoltage(1);
    usbPdData.usbpd1.Current = usb_pd_.getCurrent(1);
    usbPdData.usbpd1.LowerVoltageLimit = usb_pd_.getLowerVoltageLimit(1);
    usbPdData.usbpd1.UpperVoltageLimit = usb_pd_.getUpperVoltageLimit(1);

    /* Read settings for PDO2
        - Voltage 5-20V
        - Current value for PDO2 0-5A, if 0 used, FLEX_I value is used
        - Under Voltage Lock Out 5-20%
        - Over Voltage Lock Out 5-20%
    */
    usbPdData.usbpd2.Voltage = usb_pd_.getVoltage(2);
    usbPdData.usbpd2.Current = usb_pd_.getCurrent(2);
    usbPdData.usbpd2.LowerVoltageLimit = usb_pd_.getLowerVoltageLimit(2);
    usbPdData.usbpd2.UpperVoltageLimit = usb_pd_.getUpperVoltageLimit(2);

    /* Read settings for PDO3
         - Voltage 5-20V
         - Current value for PDO3 0-5A, if 0 used, FLEX_I value is used
         - Under Voltage Lock Out 5-20%
         - Over Voltage Lock Out 5-20%
    */
    usbPdData.usbpd3.Voltage = usb_pd_.getVoltage(3);
    usbPdData.usbpd3.Current = usb_pd_.getCurrent(3);
    usbPdData.usbpd3.LowerVoltageLimit = usb_pd_.getLowerVoltageLimit(3);
    usbPdData.usbpd3.UpperVoltageLimit = usb_pd_.getUpperVoltageLimit(3);


    /* Read the flex current value (FLEX_I) */
    usbPdData.flexCurrent = usb_pd_.getFlexCurrent();

    /* Read the External Power capable bit */
    usbPdData.externalPowerCapable = usb_pd_.getExternalPower();

    /* Read the usb_pd_ Communication capable bit */
    usbPdData.usbCommCapable = usb_pd_.getUsbCommCapable();

    /* Read the POWER_OK pins configuration */
    usbPdData.configOk = usb_pd_.getConfigOkGpio();

    /* Read the GPIO pin configuration */
    usbPdData.gpioCtrl = usb_pd_.getGpioCtrl();

    /* Read the bit that enables VBUS_EN_SNK pin only when power is greater than 5V */
    usbPdData.pwrOnlyAbove5V = usb_pd_.getPowerAbove5vOnly();

    /* Read bit that controls if the Source or Sink device's
         operating current is used in the RDO message */
    usbPdData.reqSrcCurrent = usb_pd_.getReqSrcCurrent();
}


bool PdbApp::readAnalogTelemetry_(void) {
    bool retState = true;
    int i;
    int value;
    LOG_DEBUG("Read Analog Telemetry Start");
    for (i=0; i<MAX_MUX_CH_TO_READ; i++) {
        mux_.enableChannel(i);
        // LOG_DEBUG("READ VOLTAGE ANALOG");
        // delay(10);
        value = adc_->analogRead(ANALOG_CH_VOLT_IN);
        if (value != ADC_ERROR_VALUE) {
            channelAnalogVoltageRaw[i] = (uint16_t) value;      
        }
            
        // LOG_DEBUG("READ CURRENT ANALOG");
        value = adc_->analogRead(ANALOG_CH_CUR_IN);
        if (value != ADC_ERROR_VALUE) {
            channelAnalogCurrentRaw[i] = (uint16_t) value;      
        }
        // LOG_DEBUG("READ SYS ANALOG");
        value = adc_->analogRead(ANALOG_MISC_IN);
        if (value != ADC_ERROR_VALUE) {
            systemAnalogVoltageRaw[i] = (uint16_t) value; 
        }
        // LOG_DEBUG("Analog ADC Meas: %d, %d, %d", channelAnalogVoltageRaw[i], channelAnalogCurrentRaw[i], systemAnalogVoltageRaw[i]);
        // if(adc_->adc0->fail_flag != ADC_ERROR::CLEAR) {
        //     serialOutput->printf("ADC0 Error, MUX CH %d: %s\n", i, getStringADCError(adc_->adc0->fail_flag));
        //     retState = false;
        // }
        // if(adc_->adc1->fail_flag != ADC_ERROR::CLEAR) {
        //     serialOutput->printf("ADC1 Error, MUX CH %d: %s\n", i, getStringADCError(adc_->adc1->fail_flag));
        //     retState = false;
        // }
    }
    adc_->resetError();
    LOG_DEBUG("Read Analog Telemetry Complete");
    for (i=0; i<MAX_MUX_CH_TO_READ; i++) {
        channelAnalogVoltage[i] = 2*(channelAnalogVoltageRaw[i] *3.3)/adc_->adc0->getMaxValue();
        if(i <= 5 ) {
            systemAnalogVoltage[i] = (systemAnalogVoltageRaw[i] * 3.3)/adc_->adc1->getMaxValue();
            channelAnalogCurrent[i] = (channelAnalogCurrentRaw[i] * 3.3)/adc_->adc0->getMaxValue();
        }

    }
    // channelAnalogCurrent[6] = (channelAnalogCurrentRaw[i] * 3.3)/adc_->adc0->getMaxValue();
    systemAnalogVoltage[0] = (1000/99.1) * systemAnalogVoltage[0]; 
    systemAnalogVoltage[1] = (1000/99.1) * systemAnalogVoltage[1]; 
    systemAnalogVoltage[2] = (1000/99.1) * systemAnalogVoltage[2]; 
    systemAnalogVoltage[3] = (2.0) * systemAnalogVoltage[3]; 
    systemAnalogVoltage[4] = (2.0) * systemAnalogVoltage[4];
    systemAnalogVoltage[5] = (1000/99.1) * systemAnalogVoltage[2]; 

    return retState;
}