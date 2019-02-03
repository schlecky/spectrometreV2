import serial
import numpy as np

class Spectro:
    def __init__(self):
        self.integrationTime = np.uint32(1000)
        self.connected = False
        self.offset = 0
        self.pas = 1e-6
        self.focale = 50e-3
        self.sin_i = 0.3
    
    # Connect to serial port
    def connect(self, device, baudrate=115200):
        # Try to open the serial port
        try:
            self.ser = serial.Serial(device, baudrate)  
            self.connected = True
        except serial.SerialException as e:
            print("Spectromètre","Problème de connexion" + str(e))
    
    # Close serial port
    def disconnect(self):
        if self.connected:
            self.ser.close()

    # Convert the pixel values from the ccd to intensity values
    # data is a numpy array of pixel values, function returns a
    # numpy array of intensities
    def valToIntens(self,data):
        # return (data[10]+data[11])/2 + self.offset - data
        return 4000 + self.offset - data

    # Convert pixel number to wavelength
    def pixelToWavelength(self,pixels):
        Q0 = 38.4  # Angle to the center of ccd from lens
        a = 1e3    # grating step
        f = 50e3   # Focal length of lens
        wpix = 8   # Width of one pixel

        return a*np.sin(Q0+np.arcsin((-pixels+3694/2)*wpix/f));

    # Get spectrum from spectrometer
    def acqSpectrum(self, integrationTime=-1):
        if not self.connected:
            print("Spectromètre","Le spectromètre n'est pas connecté" + str(e))
            return None

        if integrationTime == -1:
            integrationTime = self.integrationTime
        else:
            self.integrationTime = np.uint32(integrationTime)
    
        try:
            self.ser.flush()

            #Transmit key
            data = bytearray(b'GS') # Get spectrum
            self.ser.write(data)

            txIntTime = bytearray(b"\x00"*4)
            #split 32-bit integers to be sent into 8-bit data
            txIntTime[0] = (self.integrationTime >> 24) & 0xff
            txIntTime[1] = (self.integrationTime >> 16) & 0xff
            txIntTime[2] = (self.integrationTime >> 8) & 0xff
            txIntTime[3] = self.integrationTime & 0xff

            #Transmit integration time
            self.ser.write(txIntTime)
    
            #wait for the firmware to return data
            rxData8 = self.ser.read(7388)

            #combine received bytes into 16-bit data
            rxData16 = [0]*3694
            for rxi in range(3694):
                rxData16[rxi] = (rxData8[2*rxi+1] << 8) + rxData8[2*rxi]
            
            self.data = self.valToIntens(np.array(rxData16))

            return self.data
        except serial.SerialException as e:
            print("Spectromètre","Problème de connexion série" + str(e))
            self.connected = False


