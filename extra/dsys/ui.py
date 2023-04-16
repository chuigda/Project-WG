import datetime
import tkinter as tk
from tkinter import ttk

from bin_rw import write_binseq
from cain_and_abel import Cain_And_Abel
from compdec import compress
from metadata import About_Text
from util import json_stringify
from worker import WebsocketWorker


class MainWindow(tk.Tk):
    def __init__(self):
        super().__init__()

        self.option_add("*font", "fixedsys 10")

        self.title("DUMMY SYSTEM (PROTOTYPE 00, TCK and RPL)")
        self.resizable(False, False)

        # controls
        frame0 = ttk.Frame()
        self.record_btn = ttk.Button(frame0, text="TCK", width=4, command=self.start_record)
        self.replay_btn = ttk.Button(frame0, text="RPL", width=4, command=self.start_replay)
        self.dummy_btn = ttk.Button(frame0, text="ATO", width=4)
        self.clear_btn = ttk.Button(frame0, text="CLR", width=4, command=self.clear_message_window)
        self.reset_btn = ttk.Button(frame0, text="RST", width=4, command=self.reset)
        self.about_btn = ttk.Button(frame0, text="INF", width=4, command=self.show_about)
        self.progress_ind = ttk.Progressbar(frame0, mode="indeterminate", maximum=50, value=0)
        
        self.mstrwarn = tk.Label(
            frame0,
            text="MSTRWARN",
            fg="black",
            bg="white",
            border=1,
            relief="solid",
            width=10
        )

        self.dummy_btn.configure(state=tk.DISABLED)

        self.record_btn.pack(side=tk.LEFT, padx=4, pady=4)
        self.replay_btn.pack(side=tk.LEFT, padx=4, pady=4)
        self.dummy_btn.pack(side=tk.LEFT, padx=4, pady=4)
        self.clear_btn.pack(side=tk.LEFT, padx=4, pady=4)
        self.reset_btn.pack(side=tk.LEFT, padx=4, pady=4)
        self.about_btn.pack(side=tk.LEFT, padx=4, pady=4)
        self.progress_ind.pack(side=tk.LEFT, expand=1, fill=tk.BOTH, padx=4, pady=4)
        self.mstrwarn.pack(side=tk.LEFT, padx=4, pady=4, fill=tk.Y)
        frame0.pack(expand=1, fill=tk.X)

        # first row
        frame1 = ttk.Frame()
        label1 = ttk.Label(frame1, text="VTS", width=3, anchor="w")
        self.vts_ws_addr_entry = ttk.Entry(frame1)
        self.vts_ws_addr_entry.insert(0, "ws://127.0.0.1:8001")
        label1.pack(side=tk.LEFT, padx=4, pady=4)
        self.vts_ws_addr_entry.pack(side=tk.LEFT, padx=4, pady=4, expand=1, fill=tk.BOTH)
        frame1.pack(expand=1, fill=tk.X)

        # second row
        frame2 = ttk.Frame()
        label2 = ttk.Label(frame2, text="LOG", width=3, anchor="w")
        self.log_file_entry = ttk.Entry(frame2)
        self.log_file_entry.insert(0, "./dsys.tck")
        label2.pack(side=tk.LEFT, padx=4, pady=4)
        self.log_file_entry.pack(side=tk.LEFT, padx=4, pady=4, expand=1, fill=tk.BOTH)
        frame2.pack(expand=1, fill=tk.X)

        # third row
        frame3 = ttk.Frame()
        label3 = ttk.Label(frame3, text="MDL", width=3, anchor="w")
        self.model_file_entry = ttk.Entry(frame3)
        self.model_file_entry.insert(0, "./dsys.onnx")

        self.model_file_entry.config(state=tk.DISABLED)

        label3.pack(side=tk.LEFT, padx=4, pady=4)
        self.model_file_entry.pack(side=tk.LEFT, padx=4, pady=4, expand=1, fill=tk.BOTH)
        frame3.pack(expand=1, fill=tk.X)

        # Status box
        frame4 = ttk.Frame()
        self.info_wnd = tk.Text(
            frame4,
            height=24,
            width=80,
            padx=4,
            pady=4
        )
        scroll = ttk.Scrollbar(frame4, command=self.info_wnd.yview)
        self.info_wnd.config(yscrollcommand=scroll.set)
        self.info_wnd.insert(tk.END, " --- INFO WINDOW ---\n")
        self.info_wnd.config(state=tk.DISABLED)
        self.info_wnd.pack(side=tk.LEFT, expand=1, fill=tk.X)
        scroll.pack(side=tk.RIGHT, fill=tk.Y)
        frame4.pack(expand=1, fill=tk.X, padx=4, pady=4)

        # websocket worker
        self.worker = WebsocketWorker()
        self.bin_log_file = None
        self.data_buffer = []
        self.has_mstrwarn = False

    def show_about(self):
        if self.has_mstrwarn:
            self.log_into_message_window("MSTRWARN is SET, CHK and RST it, then INF")
            return

        if self.info_wnd.get(1.0, tk.END).strip() == About_Text.strip():
            self.info_wnd.config(state=tk.NORMAL)
            self.info_wnd.delete(1.0, tk.END)
            self.info_wnd.insert(tk.END, Cain_And_Abel)
            self.info_wnd.config(state=tk.DISABLED)
            return

        self.info_wnd.config(state=tk.NORMAL)
        self.info_wnd.delete(1.0, tk.END)
        self.info_wnd.insert(tk.END, About_Text)
        self.info_wnd.config(state=tk.DISABLED)

    def start_record(self):
        if self.has_mstrwarn:
            self.log_into_message_window("MSTRWARN is SET, CHK and RST it, then TCK")
            return

        log_file_name = self.log_file_entry.get()
        try:
            self.bin_log_file = open(log_file_name, "wb")
        except Exception as e:
            self.log_into_message_window(" *** ERROR: cannot open file %s: %s: %s" % (
                log_file_name,
                e.__class__.__name__,
                e
            ))
            self.set_mstrwarn()
            return

        self.worker.connect_read(
            self.vts_ws_addr_entry.get(),
            lambda data: self.save_data(data),
            lambda: self.set_mstrwarn(),
            lambda: self.on_task_finished(),
            lambda x: self.log_into_message_window(x)
        )
        self.progress_ind.start(40)
        self.record_btn.configure(state=tk.DISABLED)
        self.replay_btn.configure(state=tk.DISABLED)
        self.log_file_entry.configure(state=tk.DISABLED)

    def start_replay(self):
        if self.has_mstrwarn:
            self.log_into_message_window("MSTRWARN is SET, CHK and RST it, then RPL")
            return

        log_file_name = self.log_file_entry.get()
        self.worker.connect_send(
            self.vts_ws_addr_entry.get(),
            log_file_name,
            lambda: self.set_mstrwarn(),
            lambda: self.on_task_finished(),
            lambda x: self.log_into_message_window(x)
        )
        self.progress_ind.start(40)
        self.record_btn.configure(state=tk.DISABLED)
        self.replay_btn.configure(state=tk.DISABLED)
        self.log_file_entry.configure(state=tk.DISABLED)

    def save_data(self, data: object):
        self.data_buffer.append(data)
        if len(self.data_buffer) >= 50:
            data_text = "\n".join([json_stringify(x) for x in self.data_buffer])
            compressed = compress(data_text)
            write_binseq(self.bin_log_file, compressed)
            self.data_buffer.clear()

    def log_into_message_window(self, message):
        time = datetime.datetime.now().strftime("%H:%M:%S")
        self.info_wnd.configure(state=tk.NORMAL)
        self.info_wnd.insert(tk.END, "[%s] %s\n" % (time, message))
        self.info_wnd.configure(state=tk.DISABLED)

    def clear_message_window(self):
        if self.has_mstrwarn:
            self.log_into_message_window("MSTRWARN is SET, CHK and RST it, then CLR")
            return
        self.info_wnd.configure(state=tk.NORMAL)
        self.info_wnd.delete(1.0, tk.END)
        self.info_wnd.insert(tk.END, " --- INFO WINDOW ---\n")
        self.info_wnd.configure(state=tk.DISABLED)

    def reset(self):
        self.mstrwarn.config(fg="black", bg="light grey")
        self.has_mstrwarn = False
        self.worker.stop_task()

        if self.bin_log_file:
            self.bin_log_file.close()
            self.bin_log_file = None
        self.record_btn.configure(state=tk.NORMAL)
        self.replay_btn.configure(state=tk.NORMAL)
        self.log_file_entry.configure(state=tk.NORMAL)
        self.progress_ind.stop()
        self.log_into_message_window("SYS RST success")

    def set_mstrwarn(self):
        self.mstrwarn.config(fg="white", bg="red3")
        self.has_mstrwarn = True

    def on_task_finished(self):
        if self.bin_log_file:
            self.bin_log_file.close()
            self.bin_log_file = None
        self.record_btn.configure(state=tk.NORMAL)
        self.log_file_entry.configure(state=tk.NORMAL)
        self.progress_ind.stop()
        self.log_into_message_window("TCK/RPL ended")
