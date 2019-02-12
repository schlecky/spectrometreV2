//*****************************************************************************
//
// Firmware du spectrometre V2 avec capteur CCD TCD1304
//
//*****************************************************************************

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_timer.h"
#include "inc/hw_ints.h"
#include "inc/hw_adc.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/usb.h"
#include "driverlib/adc.h"
#include "driverlib/udma.h"

#include "usblib/usblib.h"
#include "usblib/usbcdc.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdcdc.h"

#include "utils/uartstdio.h"

#include "usb_struct.h"

//*****************************************************************************
//
// Define pin to LED color mapping.
//
//*****************************************************************************

#define RED_LED   GPIO_PIN_1
#define BLUE_LED  GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3

#define DEFAULT_BIT_RATE        460800
#define NUM_PIXELS  3694
#define RX_SIZE  64

/* CPU Frequency in kHz*/
#define CPUFREQ 80000
/* Pixel clock frequency in kHz*/
#define PIXELFREQ 1000 


unsigned int g_ccdData[NUM_PIXELS] __attribute__ ((aligned(4)));
volatile int g_pixelNum;
volatile int recvIndex = 0;
volatile int dataToRcv = 0;
volatile tBoolean acq, trans, txOK, startAcq;
unsigned char buffer[RX_SIZE];

tDMAControlTable sDMAControlTable[64] __attribute__ ((aligned(1024)));

// Global flag indicating that a USB configuration has been set.
static volatile tBoolean g_bUSBConfigured = false;

void resetDMA(unsigned int * dst, unsigned int size);

// Get the communication parameters in use on the UART.
static void
GetLineCoding(tLineCoding *psLineCoding)
{
    psLineCoding->ulRate = DEFAULT_BIT_RATE;
    psLineCoding->ucDatabits = 8;
    psLineCoding->ucParity = USB_CDC_PARITY_NONE;
    psLineCoding->ucStop = USB_CDC_STOP_BITS_1;
}

//*****************************************************************************
//
// Handles CDC driver notifications related to control and setup of the device.
//
// \param pvCBData is the client-supplied callback pointer for this channel.
// \param ulEvent identifies the event we are being notified about.
// \param ulMsgValue is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the CDC driver to perform control-related
// operations on behalf of the USB host.  These functions include setting
// and querying the serial communication parameters, setting handshake line
// states and sending break conditions.
//
// \return The return value is event-specific.
//
//*****************************************************************************
unsigned long
ControlHandler(void *pvCBData, unsigned long ulEvent,
               unsigned long ulMsgValue, void *pvMsgData)
{
    // Which event are we being asked to process?
    switch(ulEvent)
    {
        // We are connected to a host and communication is now possible.
        case USB_EVENT_CONNECTED:
            //GPIOPinWrite(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED, GREEN_LED);
            g_bUSBConfigured = true;
            // Flush our buffers.
            break;

        // The host has disconnected.
        case USB_EVENT_DISCONNECTED:
            //GPIOPinWrite(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED, BLUE_LED);
            g_bUSBConfigured = false;
            break;

        // Return the current serial communication parameters.
        case USBD_CDC_EVENT_GET_LINE_CODING:
            GetLineCoding(pvMsgData);
            break;
      
        // Set the current serial communication parameters.
        case USBD_CDC_EVENT_SET_LINE_CODING:
            break;

        // Set the current serial communication parameters.
        case USBD_CDC_EVENT_SET_CONTROL_LINE_STATE:
            break;
           
        // Send a break condition on the serial line.
        case USBD_CDC_EVENT_SEND_BREAK:
            break;

        // Clear the break condition on the serial line.
        case USBD_CDC_EVENT_CLEAR_BREAK:
            break;

        // Ignore SUSPEND and RESUME for now.
        case USB_EVENT_SUSPEND:
        case USB_EVENT_RESUME:
            break;
        // We don't expect to receive any other events.  Ignore any that show
        // up in a release build or hang in a debug build.
        default:
#ifdef DEBUG
            while(1);
#else
            break;
#endif

    }

    return(0);
}
//*****************************************************************************
//
// Handles CDC driver notifications related to the transmit channel (data to
// the USB host).
//
// \param ulCBData is the client-supplied callback pointer for this channel.
// \param ulEvent identifies the event we are being notified about.
// \param ulMsgValue is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the CDC driver to notify us of any events
// related to operation of the transmit data channel (the IN channel carrying
// data to the USB host).
//
// \return The return value is event-specific.
//
//*****************************************************************************
unsigned long
TxHandler(void *pvCBData, unsigned long ulEvent, unsigned long ulMsgValue,
          void *pvMsgData)
{
    // Which event have we been sent?
    switch(ulEvent)
    {
        case USB_EVENT_TX_COMPLETE:
            txOK = true;
            // Since we are using the USBBuffer, we don't need to do anything here
            break;

        // We don't expect to receive any other events.  Ignore any that show
        // up in a release build or hang in a debug build.
        default:
#ifdef DEBUG
            while(1);
#else
            break;
#endif

    }
    return(0);
}
//*****************************************************************************
//
// Handles CDC driver notifications related to the receive channel (data from
// the USB host).
//
// \param ulCBData is the client-supplied callback data value for this channel.
// \param ulEvent identifies the event we are being notified about.
// \param ulMsgValue is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the CDC driver to notify us of any events
// related to operation of the receive data channel (the OUT channel carrying
// data from the USB host).
//
// \return The return value is event-specific.
//
//*****************************************************************************
unsigned long
RxHandler(void *pvCBData, unsigned long ulEvent, unsigned long ulMsgValue,
          void *pvMsgData)
{
    // Which event are we being sent?
    switch(ulEvent)
    {
        // A new packet has been received.
        case USB_EVENT_RX_AVAILABLE:
        {
              // Get one byte
              USBDCDCPacketRead((tUSBDCDCDevice *)&g_sCDCDevice, &buffer[recvIndex++], 1, true);
              // If we are waiting for more data
              if(dataToRcv>0){
                dataToRcv--;          // receive it
                if(dataToRcv == 0){
                  startAcq=true;      // start acquisition if enough data is received
                }
              } else {
                if(recvIndex == RX_SIZE){  // reset circular buffer
                  recvIndex = 0;
                  }
                // check last two bytes for startup sequence
                if((buffer[(recvIndex+RX_SIZE-2)%RX_SIZE]=='G') && (buffer[(recvIndex+RX_SIZE-1)%RX_SIZE]=='S')){
                  dataToRcv = 4; //4 bytes remaining for integration time
                  recvIndex = 0;
                }
            }
            break;
        }
        case USB_EVENT_DATA_REMAINING:
        {
            return(0);
        }

        case USB_EVENT_REQUEST_BUFFER:
        {
            return(0);
        }

        // We don't expect to receive any other events.  Ignore any that show
        // up in a release build or hang in a debug build.
        default:
#ifdef DEBUG
            while(1);
#else
            break;
#endif
    }

    return(0);
}

//*********************************************************************
// The interrupt handler for uDMA errors.  This interrupt will occur 
// if the
// uDMA encounters a bus error while trying to perform a transfer.  This
// handler just increments a counter if an error occurs.
//*********************************************************************
void uDMAErrorHandler(void)
{
    unsigned long ulStatus;
    // Check for uDMA error bit
    ulStatus = uDMAErrorStatusGet();
    GPIOPinWrite(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED, BLUE_LED);

    //GPIOPinWrite(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED, BLUE_LED);
    // If there is a uDMA error, then clear the error and increment
    // the error counter.
    if(ulStatus)
    {
       uDMAErrorStatusClear();
    }
}


/****************************************************************
 * Envoie le spectre
 ****************************************************************/
void transmitData(){
  unsigned int i=0;
  unsigned int k;
  volatile unsigned int toSend;

  GPIOPinWrite(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED, BLUE_LED);
#ifdef DEBUG
  UARTprintf("Transmission des données...\r\n");
#endif
  while(i<NUM_PIXELS){
    toSend = NUM_PIXELS-i>32?32:NUM_PIXELS-i;
    for(k=0;k<toSend;k++){
      buffer[2*k] = g_ccdData[i] & 0xFF;
      buffer[2*k+1] = (g_ccdData[i]>>8) & 0xFF;
      i++;
    }
    while(txOK==false){
        SysCtlDelay(1000);
    }
    SysCtlDelay(10000);
    txOK=false;
    unsigned int res = USBDCDCPacketWrite((tUSBDCDCDevice *)&g_sCDCDevice, 
        buffer, 2*toSend, true);
#ifdef DEBUG
    if(res==0){
    UARTprintf("Erreur");
    }
#endif
    //UARTprintf("%d,%d;",c[0], c[1] );
    //SysCtlDelay(1000);
  }
#ifdef DEBUG
  UARTprintf("OK\r\n");
#endif
  GPIOPinWrite(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED, RED_LED);
}


/****************************************************************
 * ADC Interrupt Handler
 ****************************************************************/
void ADCSeq0IntHandler(void){
  ADCIntClear(ADC0_BASE, 0);
  if(g_pixelNum<NUM_PIXELS){
    unsigned int toTransfer;
    if(NUM_PIXELS-g_pixelNum >=1000){
      toTransfer = 1000;
    } else {
      toTransfer = NUM_PIXELS-g_pixelNum;
    }
    //resetDMA(&g_ccdData[g_pixelNum], 1000);
    uDMAChannelTransferSet(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT,
        UDMA_MODE_BASIC,
        (void *)(ADC0_BASE + ADC_O_SSFIFO0 +  (0x20 * 0)),
        &g_ccdData[g_pixelNum], toTransfer);
    uDMAChannelEnable(UDMA_CHANNEL_ADC0);
    g_pixelNum+=toTransfer;
  } else {
    ADCSequenceDisable(ADC0_BASE,0);
    ADCIntDisable(ADC0_BASE, 0);
    IntMasterDisable();
    acq=false;
    IntMasterEnable();
  }
}


/****************************************************************
 * Acquisition start Interrupt Handler
 ****************************************************************/
void acqStartIntHandler(void){
  TimerIntClear(WTIMER1_BASE, TIMER_TIMA_TIMEOUT);
  resetDMA(&g_ccdData[0], 1000);
  g_pixelNum = 1000;
  TimerSynchronize(TIMER0_BASE, TIMER_0A_SYNC|TIMER_0B_SYNC );
  ADCSequenceEnable(ADC0_BASE,0);
  ADCIntEnable(ADC0_BASE,0);
  acq=true;
}

/******************************************************************
 * Configuration USB CDC
 ******************************************************************/
void configUSB(void){
    // Configure the required pins for USB operation.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    GPIOPinTypeUSBAnalog(GPIO_PORTD_BASE, GPIO_PIN_5 | GPIO_PIN_4);
    
    GPIOPinWrite(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED, RED_LED);
    // Not configured initially.
    g_bUSBConfigured = false;
    
    // Set the USB stack mode to Device mode with VBUS monitoring.
    USBStackModeSet(0, USB_MODE_DEVICE, 0);

    // Pass our device information to the USB library and place the device
    // on the bus.
    USBDCDCInit(0, (tUSBDCDCDevice *)&g_sCDCDevice);

    // Attend la connexion USB
    while(!g_bUSBConfigured){
    }
}




/******************************************************************
* Configure integration time (in µs).
* change SH period and ICG if T(SH) > acqTime
 ******************************************************************/
void setIntegrationTime(unsigned long time){
    unsigned long ulPeriodMaster;
    unsigned int clkFreq = PIXELFREQ; // 2MHz
    ulPeriodMaster = CPUFREQ/clkFreq;

    unsigned long integrationTime = time; // Temps d'intégration en µs
    unsigned int shPulse = 2; // SH Pulse width in µs
    unsigned long ulPeriodSH = integrationTime*80;
    unsigned long dutyCycleSH = shPulse*80;

    // Acquisition time 
    unsigned int acqTime = 2*NUM_PIXELS*4*ulPeriodMaster; 
    unsigned long ulPeriodICG;
    if(acqTime<ulPeriodSH){
      // Mode normal
      ulPeriodICG = ulPeriodSH;
    } else {
      // Electronic shutter ICG doit etre un multiple de SH
      ulPeriodICG = (acqTime/ulPeriodSH+1)*ulPeriodSH;
    }

    unsigned int icgPulse = 10;
    unsigned long dutyCycleICG = icgPulse*80;
  
    unsigned long t3SHICG = 40;  //delai ICG-SH 0.5µs

    TimerEnable(TIMER0_BASE, TIMER_A);
    TimerEnable(TIMER0_BASE, TIMER_B);
    TimerDisable(WTIMER0_BASE,TIMER_A);
    TimerDisable(WTIMER0_BASE,TIMER_B);
    TimerDisable(WTIMER1_BASE,TIMER_A);

    // Configuration des timers pour SH et ICG
    TimerLoadSet(WTIMER0_BASE, TIMER_B, ulPeriodICG -1);
    TimerMatchSet(WTIMER0_BASE, TIMER_B, dutyCycleICG);
    /*TimerControlLevel(WTIMER0_BASE, TIMER_B, true);*/
    // Décalage entre SH et ICG
    HWREG(WTIMER0_BASE + TIMER_O_TBV) = dutyCycleICG-dutyCycleSH-t3SHICG;
    TimerLoadSet(WTIMER0_BASE, TIMER_A, ulPeriodSH -1);
    TimerMatchSet(WTIMER0_BASE, TIMER_A, dutyCycleSH); // PWM

    
    // Interruption de début de mesure synchro ICG
    TimerLoadSet(WTIMER1_BASE, TIMER_A, ulPeriodICG-1);
    HWREG(WTIMER1_BASE + TIMER_O_TAV) = ulPeriodICG-1-dutyCycleSH-t3SHICG+1500;
    IntEnable(INT_WTIMER1A);
    TimerIntEnable(WTIMER1_BASE, TIMER_TIMA_TIMEOUT);

    TimerEnable(TIMER0_BASE, TIMER_A);
    TimerEnable(TIMER0_BASE, TIMER_B);
    TimerEnable(WTIMER0_BASE, TIMER_A);
    TimerEnable(WTIMER0_BASE, TIMER_B);
    TimerEnable(WTIMER1_BASE, TIMER_A);

    IntMasterEnable();
}



/******************************************************************
 * Configuration des timers
     * Timer0-A CCD clock (16bits)
     * Timer0-B Master Clock/4 : ADC-trigger (16bits)
     * WTimer0-A SH (32 bits)
     * WTimer0-B ICG (32 bits)
     * http://codeandlife.com/2012/10/30/stellaris-launchpad-pwm-tutorial/ 
 ******************************************************************/
void configTimers(void){
   /*Configuration des timers*/
    unsigned long ulPeriodMaster, dutyCycleMaster;
    unsigned int clkFreq = PIXELFREQ; // 2MHz
    ulPeriodMaster = CPUFREQ/clkFreq;
    dutyCycleMaster = ulPeriodMaster / 2;

    // Clk Pin 
    GPIOPinConfigure(GPIO_PB6_T0CCP0);
    GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_6);
    
    // SH Pin
    GPIOPinConfigure(GPIO_PC4_WT0CCP0);
    GPIOPinTypeTimer(GPIO_PORTC_BASE, GPIO_PIN_4);

    // ICG Pin
    GPIOPinConfigure(GPIO_PC5_WT0CCP1);
    GPIOPinTypeTimer(GPIO_PORTC_BASE, GPIO_PIN_5);

    
    IntMasterDisable();
    // Configure timer for master clock
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, 
                   TIMER_CFG_SPLIT_PAIR |
                   TIMER_CFG_A_PWM | 
                   TIMER_CFG_B_PERIODIC);
  
    TimerLoadSet(TIMER0_BASE, TIMER_A, ulPeriodMaster -1);
    TimerMatchSet(TIMER0_BASE, TIMER_A, dutyCycleMaster); 

    // Timer for ADC acquisition, one pixel every 4 clock cycle
    TimerLoadSet(TIMER0_BASE, TIMER_B, 4*ulPeriodMaster -1);
    TimerControlTrigger(TIMER0_BASE, TIMER_B, true);

    /*UARTprintf("Freq acq : %d kHz\n\r", 80000/(TimerLoadGet(TIMER0_BASE, TIMER_B)+1));*/
    
    // Configuration des timers pour SH et ICG
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0);
    TimerConfigure(WTIMER0_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM|TIMER_CFG_B_PWM);

    TimerControlLevel(WTIMER0_BASE, TIMER_B, true);
  
    // Interruption de début de mesure synchro ICG
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER1);
    
    TimerConfigure(WTIMER1_BASE,TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PERIODIC);
    IntMasterEnable();
    
}



/****************************************************************
    * Configuration DMA
    * DMA is used to transfer ADC samples to memory
****************************************************************/
void configDMA(){
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
  uDMAEnable();
  uDMAControlBaseSet(sDMAControlTable);

  // Configure attributes for DMA channel
  uDMAChannelAttributeDisable(UDMA_CHANNEL_ADC0,
      UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
      UDMA_ATTR_HIGH_PRIORITY |
      UDMA_ATTR_REQMASK);

  // Configure control parameters for DMA channel
  uDMAChannelControlSet(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT,
      UDMA_SIZE_32 | UDMA_SRC_INC_NONE |
      UDMA_DST_INC_32 | UDMA_ARB_1);
}

void resetDMA(unsigned int* dst, unsigned int size){
  uDMAChannelTransferSet(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT,
      UDMA_MODE_BASIC,
      (void *)(ADC0_BASE + ADC_O_SSFIFO0 +  (0x20 * 0)),
      dst, size);

  // Enable the DMA channel
  uDMAChannelEnable(UDMA_CHANNEL_ADC0);
}

/****************************************************************
    * Configuration ADC
****************************************************************/
void configADC(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_ADC0);
    SysCtlADCSpeedSet(SYSCTL_ADCSPEED_1MSPS);

    ADCSequenceDisable(ADC0_BASE, 0);
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_TIMER, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_IE|ADC_CTL_END|ADC_CTL_CH0);
 
    ADCSequenceEnable(ADC0_BASE, 0);
    //ADCIntEnable(ADC0_BASE, 0);
    IntEnable(INT_ADC0);
} 


/****************************************************************
    * Configuration des ports entrée-sortie
****************************************************************/
void configGPIO(void){
    // Configure PB6 as T0CCP0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    // ADC
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);

    // LEDs
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED);
}

/****************************************************************
    * Configuration UART debug
****************************************************************/
void configUART(void){
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTStdioInit(0);
}

/******************************************************************
 * Flush CCD with very short integration time
 ******************************************************************/
void flushCCD(){
  setIntegrationTime(200); //200 µs
  SysCtlDelay(400);
}

/******************************************************************
 * Get acquisition time from buffer received from USB UART
 *  
 ******************************************************************/
unsigned long integrationTimeFromBuffer(){
  return buffer[0]<<24|buffer[1]<<16|buffer[2]<<8|buffer[3] ;
}

int main(void)
{
    // Setup the system clock to run at 80 Mhz from PLL with crystal reference
    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|
                    SYSCTL_OSC_MAIN);

    configGPIO();
    configUSB();
    configUART();
    UARTprintf("Stellaris @ %u MHz\n\n\r", SysCtlClockGet() / 1000000);
    configADC();
    configDMA();
    configTimers();
    setIntegrationTime(100000);

    GPIOPinWrite(GPIO_PORTF_BASE, RED_LED|BLUE_LED|GREEN_LED, RED_LED);
     // Loop Forever

    txOK=true;
    //unsigned char ucChar[2] ;
    //testHandler();
    while(1)
    {
        if(startAcq){
          startAcq=false;
          /*flushCCD();*/
          unsigned long intTime = integrationTimeFromBuffer(); 
          setIntegrationTime(intTime);
          while(acq){}
          SysCtlDelay(1000); // Wait for DMA to finish ?
          transmitData();
        }
    }
}
