import sys
from PyQt5 import QtCore
from PyQt5.QtCore import Qt, pyqtSlot
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
from viewmodel import Instruction, MainWindowViewModel

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

        # Add the layouts to the main window.

        main_layout.addLayout(self._create_buttons_layout(), 0, 0, 1, 3)
        main_layout.addLayout(self._create_instructions_layout(), 1, 0, 1, 1)
        main_layout.addLayout(self._create_stack_layout(), 1, 1, 1, 1)
        main_layout.addLayout(self._create_register_layout(), 1, 2, 1, 1)

        # Connect to signals so that the UI updates.

        self.view_model.signal_emulator_path.connect(self._update_emulator_path)
        self.view_model.signal_current_instruction.connect(self._update_current_instruction)
        self.view_model.signal_instructions.connect(self._update_instructions)


    def _update_emulator_path(self, path: str):
        self.label_emulator_path.setText(f"Emulator path: {path}")


    def _update_current_instruction(self, instruction: Instruction):
        self.layout_registers.children().clear()

        for register, value in instruction.get_registers_packaged().items():
            label_register = QLabel()
            label_value = QLabel()

            label_register.setText(register.upper())
            label_value.setText(f"{value}")

            self.layout_registers.addRow(label_register, label_value)


    def _update_instructions(self, instructions: list[Instruction]):
        for instruction in instructions:
            item = QListWidgetItem()

            item.setText(instruction.assembly)
            if instruction == self.view_model.current_instruction():
                item.setSelected(True)

            self.list_instructions.addItem(item)


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

        # Create widgets.

        button_run = QPushButton()
        button_run.setText("Run Executable")
        button_run.clicked.connect(button_run_clicked)

        button_load_emulator= QPushButton()
        button_load_emulator.setText("Load emulator path")
        button_load_emulator.clicked.connect(button_load_emulator_clicked)

        label_path = QLabel()

        # Add widgets to layout.

        layout.addWidget(button_run)
        layout.addWidget(button_load_emulator)
        layout.addWidget(label_path)

        # Register widgets as a local variables.

        self.label_emulator_path = label_path

        return layout


    def _create_instructions_layout(self) -> QLayout:
        layout = QVBoxLayout()

        # Create widgets.

        label_title = QLabel()
        label_title.setText("Instructions")

        list_instructions = QListWidget()

        # Add widgets to layout.

        layout.addWidget(label_title)
        layout.addWidget(list_instructions)

        # Register necessary widgets as local variables.

        self.instructions_list = list_instructions

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

        self.layout_registers = layout

        return layout

    def _create_stack_layout(self) -> QLayout:
        layout = QVBoxLayout()

        label_title = QLabel()
        label_title.setText("Stack")

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
