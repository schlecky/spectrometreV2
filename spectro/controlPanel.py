import tkinter as tk
from config import couleur

class ControlPanel(tk.Frame):
    lblAcqTime = None
    edtAcqTime = None
    strAcqTime = None
    lblAvgNum = None
    edtAvgNum = None
    strAvgNum = None
    edtAvgAcqNum = None
    strAvgAcqNum = None

    btnAcq = None
    btnClear = None
    btnGetDark = None

    acqSpectrumCB = None
    clearPlotCB = None
    getDarkCB = None
    savePlotCB = None
    startContinuousAcqCB = None
    stopContinuousAcqCB = None


    def __init__(self, parent):
        super().__init__(parent)
        self.config(bg=couleur['bg1'], takefocus=1)
        self.lblAcqTime = tk.Label(self, text="Exposition (µs): ", justify=tk.CENTER)
        self.lblAcqTime.config(fg=couleur['fg'], bg=couleur['bg1'], relief=tk.FLAT, highlightcolor=couleur['bdFocus'])
        self.strAcqTime = tk.StringVar()
        self.strAcqTime.set("10000")
        self.edtAcqTime = tk.Entry(self, textvariable=self.strAcqTime)
        self.edtAcqTime.config(fg=couleur['fg'], bg=couleur['bg1'], relief=tk.FLAT, highlightcolor=couleur['bdFocus'], width=7)
        self.lblAvgNum = tk.Label(self, text="Nacq/trans. : ", justify=tk.CENTER)
        self.lblAvgNum.config(fg=couleur['fg'], bg=couleur['bg1'], relief=tk.FLAT, highlightcolor=couleur['bdFocus'])
        self.strAvgNum = tk.StringVar()
        self.strAvgNum.set("10")
        self.edtAvgNum = tk.Entry(self, textvariable=self.strAvgNum)
        self.edtAvgNum.config(fg=couleur['fg'], bg=couleur['bg1'], relief=tk.FLAT, highlightcolor=couleur['bdFocus'], width=7)
        self.lblAvgAcqNum = tk.Label(self, text="Ntrans : ", justify=tk.CENTER)
        self.lblAvgAcqNum.config(fg=couleur['fg'], bg=couleur['bg1'], relief=tk.FLAT, highlightcolor=couleur['bdFocus'])
        self.strAvgAcqNum = tk.StringVar()
        self.strAvgAcqNum.set("1")
        self.edtAvgAcqNum = tk.Entry(self, textvariable=self.strAvgAcqNum)
        self.edtAvgAcqNum.config(fg=couleur['fg'], bg=couleur['bg1'], relief=tk.FLAT, highlightcolor=couleur['bdFocus'], width=7)

        self.btnAcq = tk.Button(self, text="Go !")
        self.btnAcq.config(fg=couleur['fg'], bg=couleur['bg1'], relief=tk.FLAT, highlightcolor=couleur['bdFocus'], width=12, command=self.startAcq)
        self.btnClear = tk.Button(self, text="Clear")
        self.btnClear.config(fg=couleur['fg'], bg=couleur['bg1'], relief=tk.FLAT, highlightcolor=couleur['bdFocus'], width=12, command=self.clearPlot)
        self.btnSave = tk.Button(self, text="Save")
        self.btnSave.config(fg=couleur['fg'], bg=couleur['bg1'], relief=tk.FLAT, highlightcolor=couleur['bdFocus'], width=12, command=self.savePlot)

        self.contAcqVar = tk.IntVar()
        self.btnContAcq = tk.Checkbutton(self, text="Cont. Acq.", variable=self.contAcqVar, command=self.contAcqChanged)
        # self.btnContAcq.var = self.contAcqVar
        # self.btnGetDark = tk.Button(self, text="Dark")
        # self.btnGetDark.config(fg=couleur['fg'], bg=couleur['bg1'], relief=tk.FLAT, highlightcolor=couleur['bdFocus'], width=12, command=self.getDark)

        self.lblAcqTime.grid(row=1, column=0)
        self.edtAcqTime.grid(row=1, column=1)
        self.lblAvgNum.grid(row=2, column=0)
        self.edtAvgNum.grid(row=2, column=1)
        self.lblAvgAcqNum.grid(row=3, column=0)
        self.edtAvgAcqNum.grid(row=3, column=1)
        self.btnAcq.grid(row=4, column=0)
        self.btnClear.grid(row=4, column=1)
        self.btnSave.grid(row=5, column=0)
        self.btnSave.grid(row=5, column=0)
        self.btnContAcq.grid(row=5, column=1)
        # self.btnGetDark.grid(row=5,column=1)

    def startAcq(self):
        intTime = int(self.strAcqTime.get())
        acqNum = int(self.strAvgNum.get())
        transNum = int(self.strAvgAcqNum.get())
        self.acqSpectrumCB(intTime, acqNum, transNum)

    def clearPlot(self):
        self.clearPlotCB()

    def getDark(self):
        intTime = int(self.strAcqTime.get())
        acqNum = int(self.strAvgNum.get())
        transNum = int(self.strAvgAcqNum.get())
        self.getDarkCB(intTime, acqNum, transNum)

    def savePlot(self):
        self.savePlotCB()

    def contAcqChanged(self):
        intTime = int(self.strAcqTime.get())
        acqNum = int(self.strAvgNum.get())
        if self.contAcqVar.get() == 1:
            self.startContinuousAcqCB(intTime, acqNum)
        else:
            self.stopContinuousAcqCB()
