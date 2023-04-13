from ui import MainWindow
from tkinter import ttk


def main():
    w = MainWindow()
    s = ttk.Style()
    s.theme_use('default')
    s.configure('.', font=('SimSun', 10))
    w.mainloop()


if __name__ == "__main__":
    try:
        import ctypes
        ctypes.windll.shcore.SetProcessDpiAwareness(1)
    finally:
        pass

    main()
