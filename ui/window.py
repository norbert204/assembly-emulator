import sys
from PyQt5.QtCore import Qt
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
from viewmodel import Instruction, MainWindowViewModel, EmulatorMode


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

        # Try to load emulator

        try:
            self.view_model.load_emulator("../asemu")
        except:
            self.show_error_dialog("Emulator loading error", "Emulator cannot be found. Please load it manually!")


    def _update_emulator_path(self, path: str):
        self.label_emulator_path.setText(path)


    def _update_current_instruction(self, instruction: Instruction):
        for k, v in instruction.get_registers_packaged().items():
            label_decimal, label_hex = self.labels_register[k]

            label_decimal.setText(f"{v}")
            label_hex.setText(f"{v:#x}")

        self.list_stack.clear()
        for i in instruction.stack_bytes_reverse:
            item = QListWidgetItem()
            item.setText(f"{i}")
            self.list_stack.addItem(item)


    def _update_instructions(self, instructions: list[Instruction]):
        self.list_instructions.clear()

        for instruction in instructions:
            item = QListWidgetItem()

            item.setText(instruction.assembly)
            if instruction.assembly == self.view_model.current_instruction().assembly:
                item.setSelected(True)

            self.list_instructions.addItem(item)
        self.list_instructions.setCurrentItem(self.list_instructions.itemAt(0, 0))

    # Layout creation

    def _create_menu_bar(self) -> QMenuBar:
        def load_emulator():
            file = self.open_file(name_filter="asemu")

            if not file:
                return

            try:
                self.view_model.load_emulator(file)
            except Exception as err:
                self.show_error_dialog("Emulator loading error!", str(err))


        def run_emulator(mode: EmulatorMode):
            file = self.open_file()

            if not file:
                return

            try:
                self.view_model.run_emulator(file, mode)
            except Exception as err:
                self.show_error_dialog("Code running error!", str(err))


        def run_from_output():
            file = self.open_file()

            if not file:
                return

            try:
                self.view_model.load_data_from_output(file)
            except Exception as err:
                self.show_error_dialog("Output loading error!", str(err))


        menu_bar = QMenuBar(self)

        # File menu

        menu_file = menu_bar.addMenu("&File")

        action_load = QAction("Load emulator", self)
        action_load.triggered.connect(load_emulator)

        action_exit = QAction("&Exit", self)
        action_exit.triggered.connect(QApplication.exit)

        # Run menu

        menu_run = menu_bar.addMenu("&Run")

        action_code = QAction("From source code", self)
        action_code.triggered.connect(lambda: run_emulator(EmulatorMode.CODE))

        action_executable = QAction("From executable", self)
        action_executable.triggered.connect(lambda: run_emulator(EmulatorMode.EXECUTABLE))

        action_ram = QAction("From RAM file", self)
        action_ram.triggered.connect(lambda: run_emulator(EmulatorMode.RAM_FILE))

        action_output = QAction("From output file", self)
        action_output.triggered.connect(run_from_output)

        # Register submenus

        menu_file.addAction(action_load)
        menu_file.addSeparator()
        menu_file.addAction(action_exit)

        menu_run.addAction(action_code)
        menu_run.addAction(action_executable)
        menu_run.addAction(action_ram)
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
        layout = QVBoxLayout()

        label_title = QLabel()
        label_title.setText("Registers")
        layout.addWidget(label_title)

        labels = dict()

        spacer_after = [ "RIP", "RDX", "RBP", "R15" ]
        small_spacer = QSpacerItem(0, 20, QSizePolicy.Minimum, QSizePolicy.Minimum)

        for k in self.view_model.current_instruction().get_registers_packaged().keys():
            row = QHBoxLayout()
            label_title = QLabel()
            label_title.setText(k.upper())

            label_value = QLabel()
            label_value.setAlignment(Qt.AlignRight)     # A few text editors might show this as an error.
            label_value_hex = QLabel()
            label_value_hex.setAlignment(Qt.AlignRight)

            row.addWidget(label_title)
            row.addWidget(label_value)
            row.addWidget(label_value_hex)

            layout.addLayout(row)
            labels[k] = (label_value, label_value_hex)

            if k.upper() in spacer_after:
                layout.addSpacerItem(small_spacer)

        self.labels_register = labels

        spacer = QSpacerItem(0, 10, QSizePolicy.Minimum, QSizePolicy.Expanding)
        layout.addSpacerItem(spacer)

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


    # Helpers

    def open_file(self, name_filter: str | None = None) -> str | None:
        dialog = QFileDialog()
        dialog.setFileMode(QFileDialog.ExistingFile)

        if name_filter:
            dialog.setNameFilter(name_filter)

        if dialog.exec() == QDialog.Accepted:
            return dialog.selectedFiles()[0]

        return None


    def show_error_dialog(self, title: str, text: str):
        message_box = QMessageBox()

        message_box.setWindowTitle(title)
        message_box.setInformativeText(text)

        message_box.setIcon(QMessageBox.Critical)

        message_box.exec()


    @staticmethod
    def load_css() -> str:
        with open("main.css", "r") as f:
            return f.read()


if __name__ == "__main__":
    print("This module shouldn't be run directly!", file=sys.stderr)
    sys.exit(1)
