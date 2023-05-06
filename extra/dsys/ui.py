import datetime
import tkinter as tk
from tkinter.messagebox import askyesno
from pathlib import Path
from tkinter import ttk
# import winsound

from cain_and_abel import Cain_And_Abel
from metadata import About_Text
from worker import WebsocketWorker


class MainWindow(tk.Tk):
    def __init__(self):
        super().__init__()

        self.option_add("*font", "fixedsys 10")

        self.title("DUMMY SYSTEM (PROTOTYPE 01)")
        self.resizable(False, False)

        # controls
        frame0 = ttk.Frame()
        self.track_btn = ttk.Button(frame0, text="TCK", width=4, command=self.start_track)
        self.replay_btn = ttk.Button(frame0, text="RPL", width=4, command=self.start_replay)
        self.ap1_btn = ttk.Button(frame0, text="AP/C", width=4, command=self.start_autopilot_mode1)
        self.lin_cyc_btn = ttk.Button(frame0, text="LIN", width=4, command=self.switch_lin_cyc)
        self.bep_sil_btn = ttk.Button(frame0, text="BEP", width=4, command=self.switch_bep_sln)
        self.disconnect_btn = ttk.Button(frame0, text="DIS", width=4, command=self.disconnect)
        self.clear_btn = ttk.Button(frame0, text="CLR", width=4, command=self.clear_message_window)
        self.reset_btn = ttk.Button(frame0, text="RST", width=4, command=self.reset)
        self.about_btn = ttk.Button(frame0, text="INF", width=4, command=self.show_about)
        self.progress_ind = ttk.Progressbar(frame0, mode="indeterminate", maximum=100, value=0)
        
        self.mstrwarn = tk.Label(
            frame0,
            text="MSTRWARN",
            fg="black",
            bg="white",
            border=1,
            relief="solid",
            width=10
        )

        self.disconnect_btn.configure(state=tk.DISABLED)

        self.track_btn.pack(side=tk.LEFT, padx=4, pady=4)
        self.replay_btn.pack(side=tk.LEFT, padx=4, pady=4)
        self.ap1_btn.pack(side=tk.LEFT, padx=4, pady=4)
        self.lin_cyc_btn.pack(side=tk.LEFT, padx=4, pady=4)
        self.bep_sil_btn.pack(side=tk.LEFT, padx=4, pady=4)
        self.disconnect_btn.pack(side=tk.LEFT, padx=4, pady=4)
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

        # Status box
        frame3 = ttk.Frame()
        self.info_wnd = tk.Text(
            frame3,
            height=24,
            width=80,
            padx=4,
            pady=4
        )
        scroll = ttk.Scrollbar(frame3, command=self.info_wnd.yview)
        self.info_wnd.config(yscrollcommand=scroll.set)
        self.info_wnd.insert(tk.END, " --- INFO WINDOW ---\n")
        self.info_wnd.config(state=tk.DISABLED)
        self.info_wnd.pack(side=tk.LEFT, expand=1, fill=tk.X)
        scroll.pack(side=tk.RIGHT, fill=tk.Y)
        frame3.pack(expand=1, fill=tk.X, padx=4, pady=4)

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

    def start_track(self):
        if self.has_mstrwarn:
            self.log_into_message_window("MSTRWARN is SET, CHK and RST it, then TCK")
            return

        log_file_name = self.log_file_entry.get()
        if Path(log_file_name).exists():
            if not askyesno("File Exists", "File already exists, overwrite?"):
                return

        self.worker.connect_read(
            self.vts_ws_addr_entry.get(),
            log_file_name,
            lambda: self.set_mstrwarn(),
            lambda: self.on_task_finished("TCK"),
            lambda x: self.log_into_message_window(x)
        )
        self.start_worker_update_inputs()

    def start_replay(self):
        if self.has_mstrwarn:
            self.log_into_message_window("MSTRWARN is SET, CHK and RST it, then RPL")
            return

        log_file_name = self.log_file_entry.get()
        self.worker.connect_send(
            self.vts_ws_addr_entry.get(),
            log_file_name,
            lambda: self.set_mstrwarn(),
            lambda: self.on_task_finished("RPL"),
            lambda x: self.log_into_message_window(x)
        )
        self.start_worker_update_inputs()

    def start_autopilot_mode1(self):
        if self.has_mstrwarn:
            self.log_into_message_window("MSTRWARN is SET, CHK and RST it, then AP1")
            return

        log_file_name = self.log_file_entry.get()
        self.worker.connect_send(
            self.vts_ws_addr_entry.get(),
            log_file_name,
            lambda: self.set_mstrwarn(),
            lambda: self.on_task_finished("AP1"),
            lambda x: self.log_into_message_window(x),
            filt=True,
            cyc=self.lin_cyc_btn["text"] == "CYC"
        )
        self.start_worker_update_inputs()

    def switch_lin_cyc(self):
        self.lin_cyc_btn.configure(text="LIN" if self.lin_cyc_btn["text"] == "CYC" else "CYC")

    def switch_bep_sln(self):
        self.bep_sil_btn.configure(text="BEP" if self.bep_sil_btn["text"] == "SIL" else "SIL")

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

    def disconnect(self):
        self.worker.stop_task()
        self.stop_worker_update_inputs()
        self.log_into_message_window("SYS DIS success")

    def reset(self):
        self.mstrwarn.config(fg="black", bg="white")
        self.has_mstrwarn = False
        self.worker.stop_task()
        self.worker.reset_tokens()
        self.stop_worker_update_inputs()
        self.log_into_message_window("SYS RST success")

    def set_mstrwarn(self):
        self.mstrwarn.config(fg="white", bg="red3")
        self.has_mstrwarn = True
        if self.bep_sil_btn["text"] == "BEP":
            # winsound.PlaySound("./resc/masterwarn.wav", winsound.SND_FILENAME | winsound.SND_ASYNC)
            pass

    def on_task_finished(self, task_name):
        self.stop_worker_update_inputs()
        self.log_into_message_window("%s ended" % task_name)
        if task_name == "AP1" or task_name == "AP2":
            if not self.has_mstrwarn and self.bep_sil_btn["text"] == "BEP":
                pass
                # winsound.PlaySound("./resc/apdisconnect.wav", winsound.SND_FILENAME | winsound.SND_ASYNC)

    def start_worker_update_inputs(self):
        self.track_btn.configure(state=tk.DISABLED)
        self.replay_btn.configure(state=tk.DISABLED)
        self.ap1_btn.configure(state=tk.DISABLED)
        self.lin_cyc_btn.configure(state=tk.DISABLED)
        self.vts_ws_addr_entry.configure(state=tk.DISABLED)
        self.log_file_entry.configure(state=tk.DISABLED)
        self.disconnect_btn.configure(state=tk.NORMAL)
        self.progress_ind.start(20)

    def stop_worker_update_inputs(self):
        self.track_btn.configure(state=tk.NORMAL)
        self.replay_btn.configure(state=tk.NORMAL)
        self.ap1_btn.configure(state=tk.NORMAL)
        self.lin_cyc_btn.configure(state=tk.NORMAL)
        self.vts_ws_addr_entry.configure(state=tk.NORMAL)
        self.log_file_entry.configure(state=tk.NORMAL)
        self.disconnect_btn.configure(state=tk.DISABLED)
        self.progress_ind.stop()
