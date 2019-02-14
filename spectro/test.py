import spectro
import numpy as np
import matplotlib.pyplot as plt

s = spectro.Spectro()
def connect():
    s.connect("/dev/ttyACM0", 115200)

def continuousAcq(intTime):
    wl = s.pixelToWavelength(np.array([i+1 for i in range(3694)]))
    plt.ion()
    fig = plt.figure()
    ax = fig.add_subplot(111)
    plt.ylim(0,5500)
    line1, = ax.plot(wl, np.sin(wl), 'r*-')
    cont = True
    while(cont):
        try:
            # strTime = input("Temps d'acquisition : ")
            # try:
                # intTime = int(strTime)
            # except ValueError:
                # pass
            # intTime =int(input("Temps d'acquisition : "))
            d = s.acqMultSpectrum(1,intTime)
            line1.set_ydata(d)
            fig.canvas.draw()
            fig.canvas.flush_events()
        except KeyboardInterrupt:
            cont = False

def average(number, intTime):
    global numSpectre
    numSpectre+=1
    spectrum = np.zeros(3694)
    for i in range(number):
        spectrum += s.acqSpectrum(intTime)

    plt.plot(wl,spectrum/number)

def multAcq(number, intTime):
    global numSpectre
    numSpectre+=1
    spectrum = s.acqMultSpectrum(number,intTime)
    plt.plot(wl,spectrum/number)



connect()
wl = s.pixelToWavelength(np.array([i+1 for i in range(3694)]))
plt.ion()
plt.ylim(0,2500)
numSpectre = 0
continuousAcq(500)
# while(True):
    # command = input('')
    # if command[0] == 'a':
        # _,n,t = command.split(",")
        # n = int(n)
        # t = int(t)
        # average(n,t)
    # if command[0] == 'm':
        # _,n,t = command.split(",")
        # n = int(n)
        # t = int(t)
        # multAcq(n,t)
