#plotting is derived from https://matplotlib.org/examples/user_interfaces/embedding_in_tk.html
import matplotlib
import tkinter as tk
from config import couleur
matplotlib.use('TkAgg')

from numpy import arange, sin, pi
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2Tk
# implement the default mpl key bindings
from matplotlib.backend_bases import key_press_handler

from matplotlib.figure import Figure

class SpectrumPlot(tk.Frame):
    def __init__(self, master):
        super().__init__(master)
        #create canvas
        self.f = Figure(figsize=(10, 5), dpi=100, tight_layout=True)
        self.a = self.f.add_subplot(111)

        # a tk.DrawingArea
        self.canvas = FigureCanvasTkAgg(self.f, master=self)
        self.canvas.draw()
        self.canvas.get_tk_widget().grid(row=1, columnspan=2)

        toolbarFrame = tk.Frame(master=self)
        toolbarFrame.grid(row=2,columnspan=2, sticky="w")
        toolbar1 = NavigationToolbar2Tk(self.canvas, toolbarFrame)
        toolbar1.config( bg=couleur['bg1'], relief=tk.FLAT, highlightcolor=couleur['bdFocus'])
        toolbarFrame.config( bg=couleur['bg1'], relief=tk.FLAT, highlightcolor=couleur['bdFocus'])

        def on_key_event(event):
            print('you pressed %s' % event.key)
            key_press_handler(event, canvas, toolbar)

        #canvas.mpl_connect('key_press_event', on_key_event)

    def plotSpectrum(self,wl,spectrum):
        self.a.plot(wl,spectrum)
        self.canvas.draw()

    def clear(self):
        self.a.clear()
        self.canvas.draw()

