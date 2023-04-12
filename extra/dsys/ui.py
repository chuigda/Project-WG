import tkinter as tk
from tkinter import ttk
from cain_and_abel import Cain_And_Abel


class MainWindow(tk.Tk):
    def __init__(self):
        super().__init__()

        monospace_font = ("Courier Prime", 10)

        self.title("DUMMY SYSTEM (PROTOTYPE)")
        self.resizable(False, False)

        # controls
        frame0 = ttk.Frame()
        self.progressIndicator = ttk.Progressbar(frame0)
        self.progressIndicator.start(200)
        self.trackButton = ttk.Button(frame0, text="TRACK", width=8)
        self.replayButton = ttk.Button(frame0, text="REPLAY", width=8)
        self.trainButton = ttk.Button(frame0, text="TRAIN", width=8)
        self.dummyButton = ttk.Button(frame0, text="DUMMY", width=8)
        self.progressIndicator.pack(side=tk.LEFT, expand=1, fill=tk.BOTH, padx=4, pady=4)
        self.trackButton.pack(side=tk.LEFT, padx=4, pady=4)
        self.replayButton.pack(side=tk.LEFT, padx=4, pady=4)
        self.trainButton.pack(side=tk.LEFT, padx=4, pady=4)
        self.dummyButton.pack(side=tk.LEFT, padx=4, pady=4)
        frame0.pack(expand=1, fill=tk.X)

        # first row
        frame1 = ttk.Frame()
        label1 = ttk.Label(frame1, text="VTS WS ADDR", width=12, anchor="w")
        self.vtsWsAddr = ttk.Entry(frame1, font=monospace_font)
        self.vtsWsAddr.insert(0, "127.0.0.1:8000")
        label1.pack(side=tk.LEFT, padx=4, pady=4)
        self.vtsWsAddr.pack(side=tk.LEFT, padx=4, pady=4, expand=1, fill=tk.BOTH)
        frame1.pack(expand=1, fill=tk.X)

        # second row
        frame2 = ttk.Frame()
        label2 = ttk.Label(frame2, text="LOG FILE", width=12, anchor="w")
        self.logFile = ttk.Entry(frame2, font=monospace_font)
        self.logFile.insert(0, "~/dsys.log")
        label2.pack(side=tk.LEFT, padx=4, pady=4)
        self.logFile.pack(side=tk.LEFT, padx=4, pady=4, expand=1, fill=tk.BOTH)
        frame2.pack(expand=1, fill=tk.X)

        # third row
        frame3 = ttk.Frame()
        label3 = ttk.Label(frame3, text="MODEL FILE", width=12, anchor="w")
        self.modelFile = ttk.Entry(frame3, font=monospace_font)
        self.modelFile.insert(0, "~/dsys.onnx")
        label3.pack(side=tk.LEFT, padx=4, pady=4)
        self.modelFile.pack(side=tk.LEFT, padx=4, pady=4, expand=1, fill=tk.BOTH)
        frame3.pack(expand=1, fill=tk.X)

        # Status box
        frame4 = ttk.Frame()
        self.infoWindow = tk.Text(
            frame4,
            height=24,
            width=80,
            padx=4,
            pady=4,
            font=monospace_font
        )
        scroll = ttk.Scrollbar(frame4, command=self.infoWindow.yview)
        self.infoWindow.config(yscrollcommand=scroll.set)
        self.infoWindow.insert(tk.END, " --- INFO WINDOW ---")
        self.infoWindow.insert(tk.END, Cain_And_Abel)
        self.infoWindow.config(state=tk.DISABLED)
        self.infoWindow.pack(side=tk.LEFT, expand=1, fill=tk.X)
        scroll.pack(side=tk.RIGHT, fill=tk.Y)
        frame4.pack(expand=1, fill=tk.X, padx=4, pady=4)
