#!/usr/bin/env python3

from PyQt5.QtWidgets import QApplication
from window import MainWindow
from viewmodel import MainWindowViewModel
import sys


def main():
    app = QApplication(sys.argv)
    app.setApplicationName("Assembly Emulator GUI")

    view_model = MainWindowViewModel()
    window = MainWindow(view_model)
    window.show()

    app.exec()


if __name__ == "__main__":
    main()
