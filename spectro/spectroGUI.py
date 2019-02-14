#!/usr/bin/python
import tkinter as tk
import spectrumPlot
import controlPanel
import spectrumList
import spectro
from config import *
import numpy as np


# Acquisition callback, revceives integration time,
# number of acquisitions per transfer and number of
# transfered acquistions
def acqSpectrumCB(intTime, acqNum, transNum):
    global s, plot, sl
    spectrum = np.zeros(NUMPIXELS)
    for i in range(transNum):
        spectrum += s.getSpectrum(acqNum, intTime)
    # spectrum = spectrum/transNum
    # spectrum = spectrum
    # s.correctImbalance(spectrum)
    wl = s.pixelToWavelength(np.array([i+1 for i in range(NUMPIXELS)]))
    sl.addSpectrum((wl, spectrum), intTime, acqNum, transNum) 
    plot.plotSpectrum(wl, spectrum)

# Delete all plotted data
def clearPlotCB():
    global plot
    plot.clear()

# Get dark values
def getDarkCB(intTime, acqNum, transNum):
    global s
    values = np.zeros(NUMPIXELS)
    for i in range(transNum):
        values += s.acqMultSpectrum(acqNum, intTime)
    s.setDark(values)
    

def spectrumSelectedCB(spectrumList):
    global plot
    plot.clear()
    for s in spectrumList:
        plot.plotSpectrum(*s.data)

def savePlotCB():
    global sl
    sl.saveSelectedPlot()


root = tk.Tk()
root.title("Spectormètre")
root.configure(background='white')

s = spectro.Spectro()
s.connect("/dev/ttyACM0" , 115200)

#create drawing area
plot = spectrumPlot.SpectrumPlot(root)

cp = controlPanel.ControlPanel(root)
cp.acqSpectrumCB = acqSpectrumCB
cp.clearPlotCB = clearPlotCB
cp.getDarkCB = getDarkCB
cp.savePlotCB = savePlotCB

sl = spectrumList.SpectrumList(root)
sl.spectrumSelectedCB = spectrumSelectedCB

cp.grid(row=0, column=0)
plot.grid(row=0, column=1)
sl.grid(row=0, column=2)


tk.mainloop()

s.disconnect()

