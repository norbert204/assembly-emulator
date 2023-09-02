import sys
from PyQt5 import QtCore
from PyQt5.QtCore import Qt, pyqtSlot
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
from viewmodel import Instruction, MainWindowViewModel


class MainWindow(QMainWindow):
    def __init__(self, view_model: MainWindowViewModel):
        super(MainWindow, self).__init__()

        self.view_model = view_model

        self.resize(1280, 800)
        self.setMinimumSize(1280, 800)
        self.setWindowTitle(f"Assembly Emulator GUI")
        self.setStyleSheet(MainWindow.load_css())

        central_widget = QWidget()

        main_layout = QGridLayout()
        main_layout.setColumnStretch(0, 1)
        main_layout.setColumnStretch(1, 1)
        main_layout.setColumnStretch(2, 1)
        main_layout.setHorizontalSpacing(20)

        central_widget.setLayout(main_layout)

        self.setCentralWidget(central_widget)

        # Add the layouts to the main window.

        main_layout.addLayout(self._create_toolbar_layout(), 0, 0, 1, 3)
        main_layout.addLayout(self._create_instructions_layout(), 1, 0, 1, 1)
        main_layout.addLayout(self._create_stack_layout(), 1, 1, 1, 1)
        main_layout.addLayout(self._create_register_layout(), 1, 2, 1, 1)

        self.setMenuBar(self._create_menu_bar())

        # Connect to signals so that the UI updates.

        self.view_model.signal_emulator_path.connect(self._update_emulator_path)
        self.view_model.signal_current_instruction.connect(self._update_current_instruction)
        self.view_model.signal_instructions.connect(self._update_instructions)


    def _update_emulator_path(self, path: str):
        self.label_emulator_path.setText(f"Emulator path: {path}")


    def _update_current_instruction(self, instruction: Instruction):
        self.label_rip.setText(f"{instruction.rip}")
        self.label_rax.setText(f"{instruction.rax}")
        self.label_rbx.setText(f"{instruction.rbx}")
        self.label_rcx.setText(f"{instruction.rcx}")
        self.label_rdx.setText(f"{instruction.rdx}")
        self.label_rdi.setText(f"{instruction.rdi}")
        self.label_rsi.setText(f"{instruction.rsi}")
        self.label_rsp.setText(f"{instruction.rsp}")
        self.label_rbp.setText(f"{instruction.rbp}")
        self.label_r8.setText(f"{instruction.r8}")
        self.label_r9.setText(f"{instruction.r9}")
        self.label_r10.setText(f"{instruction.r10}")
        self.label_r11.setText(f"{instruction.r11}")
        self.label_r12.setText(f"{instruction.r12}")
        self.label_r13.setText(f"{instruction.r13}")
        self.label_r14.setText(f"{instruction.r14}")
        self.label_r15.setText(f"{instruction.r15}")
        self.label_cf.setText(f"{instruction.cf}")
        self.label_of.setText(f"{instruction.of}")
        self.label_sf.setText(f"{instruction.sf}")
        self.label_zf.setText(f"{instruction.zf}")


    def _update_instructions(self, instructions: list[Instruction]):
        self.list_instructions.clear()

        for instruction in instructions:
            item = QListWidgetItem()

            item.setText(instruction.assembly)
            if instruction.assembly == self.view_model.current_instruction().assembly:
                item.setSelected(True)

            self.list_instructions.addItem(item)
        self.list_instructions.setCurrentItem(self.list_instructions.itemAt(0, 0))


    def _create_menu_bar(self) -> QMenuBar:
        def load_emulator():
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

        def run_from_executable():
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

        def run_from_output():
            dialog = QFileDialog()
            dialog.setFileMode(QFileDialog.AnyFile)

            dialog.exec()

            file = dialog.selectedFiles()[0]

            try:
                self.view_model.load_run_data(file)
            except Exception as err:
                message_box = QMessageBox()
                message_box.setText("Executable running error!")
                message_box.setInformativeText(f"{err}")

                message_box.setIcon(QMessageBox.Critical)

                message_box.exec()

        menu_bar = QMenuBar(self)

        # File menu

        menu_file = menu_bar.addMenu("&File")

        action_load = QAction("Load emulator", self)
        action_load.triggered.connect(load_emulator)

        action_exit = QAction("&Exit", self)
        action_exit.triggered.connect(QApplication.exit)

        # Run menu

        menu_run = menu_bar.addMenu("&Run")

        action_executable = QAction("From executable", self)
        action_executable.triggered.connect(run_from_executable)

        action_output = QAction("From output file", self)
        action_output.triggered.connect(run_from_output)

        # Register submenus

        menu_file.addAction(action_load)
        menu_file.addSeparator()
        menu_file.addAction(action_exit)

        menu_run.addAction(action_executable)
        menu_run.addAction(action_output)

        return menu_bar


    def _create_toolbar_layout(self) -> QLayout:
        layout = QHBoxLayout()
        layout.setSpacing(10)

        # Create widgets.

        label_path = QLabel()
        label_path.setText("Emulator path:")
        label_path_value = QLabel()

        spacer = QSpacerItem(10, 0, QSizePolicy.Expanding, QSizePolicy.Minimum)

        # Add widgets to layout.

        layout.addWidget(label_path)
        layout.addWidget(label_path_value)
        layout.addSpacerItem(spacer)

        # Register widgets as a instance variables.

        self.label_emulator_path = label_path_value

        return layout


    def _create_instructions_layout(self) -> QLayout:
        def selection_changed():
            self.view_model.change_current_instruction(self.list_instructions.selectedIndexes()[0].row())

        layout = QVBoxLayout()

        # Create widgets.

        label_title = QLabel()
        label_title.setText("Instructions")

        list_instructions = QListWidget()
        list_instructions.itemSelectionChanged.connect(selection_changed)

        # Add widgets to layout.

        layout.addWidget(label_title)
        layout.addWidget(list_instructions)

        # Register necessary widgets as instance variables.

        self.list_instructions = list_instructions

        return layout


    def _create_register_layout(self) -> QLayout:
        layout = QFormLayout()
        layout.setHorizontalSpacing(20)

        # Create widgets.

        label_title = QLabel()
        label_title.setText("Registers")

        label_rip = QLabel()
        label_rip.setText("RIP")
        label_rip_value = QLabel()

        label_rax = QLabel()
        label_rax.setText("RAX")
        label_rax_value = QLabel()

        label_rbx = QLabel()
        label_rbx.setText("RBX")
        label_rbx_value = QLabel()

        label_rcx = QLabel()
        label_rcx.setText("RCX")
        label_rcx_value = QLabel()

        label_rdx = QLabel()
        label_rdx.setText("RDX")
        label_rdx_value = QLabel()

        label_rdi = QLabel()
        label_rdi.setText("RDI")
        label_rdi_value = QLabel()

        label_rsi = QLabel()
        label_rsi.setText("RSI")
        label_rsi_value = QLabel()

        label_rsp = QLabel()
        label_rsp.setText("RSP")
        label_rsp_value = QLabel()

        label_rbp = QLabel()
        label_rbp.setText("RBP")
        label_rbp_value = QLabel()

        label_r8 = QLabel()
        label_r8.setText("R8")
        label_r8_value = QLabel()

        label_r9 = QLabel()
        label_r9.setText("R9")
        label_r9_value = QLabel()

        label_r10 = QLabel()
        label_r10.setText("R10")
        label_r10_value = QLabel()

        label_r11 = QLabel()
        label_r11.setText("R11")
        label_r11_value = QLabel()

        label_r12 = QLabel()
        label_r12.setText("R12")
        label_r12_value = QLabel()

        label_r13 = QLabel()
        label_r13.setText("R13")
        label_r13_value = QLabel()

        label_r14 = QLabel()
        label_r14.setText("R14")
        label_r14_value = QLabel()

        label_r15 = QLabel()
        label_r15.setText("R15")
        label_r15_value = QLabel()

        label_cf = QLabel()
        label_cf.setText("CF")
        label_cf_value = QLabel()

        label_of = QLabel()
        label_of.setText("OF")
        label_of_value = QLabel()

        label_sf = QLabel()
        label_sf.setText("SF")
        label_sf_value = QLabel()

        label_zf = QLabel()
        label_zf.setText("ZF")
        label_zf_value = QLabel()

        # Add widgets to layout.

        layout.addWidget(label_title)

        layout.addRow(label_rip, label_rip_value)

        layout.addRow(label_rax, label_rax_value)
        layout.addRow(label_rbx, label_rbx_value)
        layout.addRow(label_rcx, label_rcx_value)
        layout.addRow(label_rdx, label_rdx_value)

        layout.addRow(label_rdi, label_rdi_value)
        layout.addRow(label_rsi, label_rsi_value)
        layout.addRow(label_rsp, label_rsp_value)
        layout.addRow(label_rbp, label_rbp_value)

        layout.addRow(label_r8, label_r8_value)
        layout.addRow(label_r9, label_r9_value)
        layout.addRow(label_r10, label_r10_value)
        layout.addRow(label_r11, label_r11_value)
        layout.addRow(label_r12, label_r12_value)
        layout.addRow(label_r13, label_r13_value)
        layout.addRow(label_r14, label_r14_value)
        layout.addRow(label_r15, label_r15_value)

        layout.addRow(label_cf, label_cf_value)
        layout.addRow(label_of, label_of_value)
        layout.addRow(label_sf, label_sf_value)
        layout.addRow(label_zf, label_zf_value)

        # Register widgets as a instance variables.

        self.label_rip = label_rip_value
        self.label_rax = label_rax_value
        self.label_rbx = label_rbx_value
        self.label_rcx = label_rcx_value
        self.label_rdx = label_rdx_value
        self.label_rdi = label_rdi_value
        self.label_rsi = label_rsi_value
        self.label_rsp = label_rsp_value
        self.label_rbp = label_rbp_value
        self.label_r8 = label_r8_value
        self.label_r9 = label_r9_value
        self.label_r10 = label_r10_value
        self.label_r11 = label_r11_value
        self.label_r12 = label_r12_value
        self.label_r13 = label_r13_value
        self.label_r14 = label_r14_value
        self.label_r15 = label_r15_value
        self.label_cf = label_cf_value
        self.label_of = label_of_value
        self.label_sf = label_sf_value
        self.label_zf = label_zf_value

        return layout


    def _create_stack_layout(self) -> QLayout:
        layout = QVBoxLayout()

        # Create widgets.

        label_title = QLabel()
        label_title.setText("Stack")

        list_stack = QListWidget()

        # Add widgets to layout.

        layout.addWidget(label_title)
        layout.addWidget(list_stack)

        # Register necessary widgets as instance variables.

        self.list_stack = list_stack

        return layout


    @staticmethod
    def load_css() -> str:
        with open("main.css", "r") as f:
            return f.read()


if __name__ == "__main__":
    print("This module shouldn't be run directly!", file=sys.stderr)
    sys.exit(1)
