import sys

from PyQt5.QtCore import Qt
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
from viewmodel import MainWindowViewModel

class MainWindow(QWidget):
    def __init__(self, view_model: MainWindowViewModel):
        super(MainWindow, self).__init__()

        self.view_model = view_model

        self.setGeometry(100, 100, 1280, 800)
        self.setMinimumSize(1280, 800)
        self.setWindowTitle(f"Assembly Emulator GUI")
        self.setStyleSheet(MainWindow.load_css())

        main_layout = QGridLayout()
        main_layout.setColumnStretch(0, 1)
        main_layout.setColumnStretch(1, 1)
        main_layout.setColumnStretch(2, 1)
        main_layout.setHorizontalSpacing(20)

        self.setLayout(main_layout)

        main_layout.addLayout(self._create_buttons_layout(), 0, 0, 1, 3)
        main_layout.addLayout(self._create_instructions_layout(), 1, 0, 2, 1)
        main_layout.addLayout(self._create_stack_layout(), 1, 1, 1, 1)
        main_layout.addLayout(self._create_memory_layout(), 2, 1, 1, 1)
        main_layout.addLayout(self._create_register_layout(), 1, 2, 2, 1)

    def _create_buttons_layout(self) -> QLayout:
        def button_run_clicked():
            dialog = QFileDialog()
            dialog.setFileMode(QFileDialog.AnyFile)

            dialog.exec()

            file = dialog.selectedFiles()[0]

            try:
                self.view_model.run_executable(file)
            except ValueError as err:
                message_box = QMessageBox()
                message_box.setText("Executable running error!")
                message_box.setInformativeText(f"{err}")

                message_box.setIcon(QMessageBox.Critical)

                message_box.exec()

        def button_load_emulator_clicked():
            dialog = QFileDialog()
            dialog.setFileMode(QFileDialog.AnyFile)

            dialog.exec()

            file = dialog.selectedFiles()[0]

            try:
                self.view_model.load_emulator(file)
            except ValueError as err:
                message_box = QMessageBox()
                message_box.setText("Emulator loading error!")
                message_box.setInformativeText(f"{err}")

                message_box.setIcon(QMessageBox.Critical)

                message_box.exec()

        layout = QHBoxLayout()

        button_run = QPushButton()
        button_run.setText("Run Executable")
        button_run.clicked.connect(button_run_clicked)

        layout.addWidget(button_run)

        button_load_emulator= QPushButton()
        button_load_emulator.setText("Load emulator path")
        button_load_emulator.clicked.connect(button_load_emulator_clicked)

        layout.addWidget(button_load_emulator)

        return layout

    def _create_instructions_layout(self) -> QLayout:
        layout = QVBoxLayout()

        label_title = QLabel()
        label_title.setText("Instructions")

        layout.addWidget(label_title)

        list_instructions = QListWidget()

        layout.addWidget(list_instructions)

        return layout

    def _create_register_layout(self) -> QLayout:
        layout = QFormLayout()
        layout.setHorizontalSpacing(20)

        label_title = QLabel()
        label_title.setText("Registers")

        layout.addWidget(label_title)

        current_instruction = self.view_model.current_instruction()

        for register, value in current_instruction.get_registers_packaged().items():
            label_register = QLabel()
            label_value = QLabel()

            label_register.setText(register.upper())
            label_value.setText(f"{value}")

            layout.addRow(label_register, label_value)

        return layout

    def _create_stack_layout(self) -> QLayout:
        layout = QVBoxLayout()

        label_title = QLabel()
        label_title.setText("Stack")

        layout.addWidget(label_title)

        list_stack = QListWidget()
        layout.addWidget(list_stack)

        return layout

    def _create_memory_layout(self) -> QLayout:
        layout = QVBoxLayout()

        label_title = QLabel()
        label_title.setText("Memory")

        layout.addWidget(label_title)

        list_stack = QListWidget()
        layout.addWidget(list_stack)

        return layout

    @staticmethod
    def load_css() -> str:
        with open("main.css", "r") as f:
            return f.read()


if __name__ == "__main__":
    print("This module shouldn't be run directly!", file=sys.stderr)
    sys.exit(1)
