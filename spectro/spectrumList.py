import tkinter as tk
from tkinter import filedialog
from config import couleur

class Spectrum():
    def __init__(self, data, intTime, intNumber, transNum):
        self.data = data
        self.intTime = intTime
        self.intNumber = intNumber
        self.transNum = transNum
        self.spectrumSelectedCB = None

    def save(self):
        filename = filedialog.asksaveasfilename(initialdir = "~/",title = "Select file",filetypes = (("CSV files","*.csv"),("all files","*.*")))
        f = open(filename, "w")
        f.write("#Integration Time : "+str(self.intTime)+"\n")
        f.write("#Integration number : "+str(self.intNumber)+"\n")
        f.write("#Transmission number : "+str(self.transNum)+"\n")
        f.write("#wavelength\tIntensity\n")
        for v in zip(*self.data):
            f.write(str(v[0])+"\t"+str(v[1])+"\n")
        f.close()

class SpectrumList(tk.Listbox):
    def __init__(self,parent):
        super().__init__(parent,selectmode=tk.EXTENDED)
        self.spectrumList = []
        self.iSpectrum = 1
        self.config( bg=couleur['bg1'], relief=tk.FLAT, highlightcolor=couleur['bdFocus'], width=2)
        self.bind('<<ListboxSelect>>', self.selectionChanged)

    def addSpectrum(self, data, intTime, intNumber, transNum):
        self.spectrumList.append(Spectrum(data, intTime, intNumber, transNum))
        self.insert(tk.END, str(self.iSpectrum))
        self.iSpectrum+=1

    def selectSpectrum(self, specList):
        self.selection_clear(0, tk.END)
        for i in specList:
            self.selection_set(i)
        self.spectrumSelectedCB([self.spectrumList[int(i)] for i in self.curselection()])

    def selectionChanged(self,event):
        if self.spectrumSelectedCB:
            self.spectrumSelectedCB([self.spectrumList[int(i)] for i in self.curselection()])

    def saveSelectedPlot(self):
        num = int(self.curselection()[-1])
        self.spectrumList[num].save()
