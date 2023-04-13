from ui import MainWindow
from tkinter import ttk
from async_tkinter_loop import async_mainloop


def main():
    w = MainWindow()
    s = ttk.Style()
    s.theme_use('default')
    s.configure('.', font=('fixedsys', 10))
    async_mainloop(w)


if __name__ == "__main__":
    try:
        import ctypes
        ctypes.windll.shcore.SetProcessDpiAwareness(1)
    finally:
        pass

    main()
