import spectro
import numpy as np
import matplotlib.pyplot as plt

s = spectro.Spectro()
def connect():
    s.connect("/dev/ttyACM1", 115200)

def continuousAcq(intTime):
    wl = s.pixelToWavelength(np.array([i+1 for i in range(3694)]))
    plt.ion()
    fig = plt.figure()
    ax = fig.add_subplot(111)
    plt.ylim(0,2500)
    line1, = ax.plot(wl, np.sin(wl), 'r-')
    try:
        while(True):
            d = s.acqSpectrum(intTime)
            line1.set_ydata(d)
            fig.canvas.draw()
            fig.canvas.flush_events()
    except KeyboardInterrupt:
        return None

connect()
continuousAcq(10000)
