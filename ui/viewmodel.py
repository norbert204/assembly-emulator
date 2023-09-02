import os
import sys
import subprocess

from PyQt5.QtCore import QObject, pyqtSignal

class Instruction():
    def __init__(
            self,
            assembly: str = "",
            rip: int = 0,
            rax: int = 0,
            rbx: int = 0,
            rcx: int = 0,
            rdx: int = 0,
            rdi: int = 0,
            rsi: int = 0,
            rsp: int = 0,
            rbp: int = 0,
            r8: int = 0,
            r9: int = 0,
            r10: int = 0,
            r11: int = 0,
            r12: int = 0,
            r13: int = 0,
            r14: int = 0,
            r15: int = 0,
            cf: int = 0,
            of: int = 0,
            sf: int = 0,
            zf: int = 0,
            stack_size: int = 0,
            stack_bytes_reverse: list[int] = list()
        ):

        self.assembly = assembly
        self.rip = rip
        self.rax = rax
        self.rbx = rbx 
        self.rcx = rcx
        self.rdx = rdx 
        self.rdi = rdi
        self.rsi = rsi
        self.rsp = rsp
        self.rbp = rbp
        self.r8 = r8
        self.r9 = r9
        self.r10 = r10
        self.r11 = r11
        self.r12 = r12
        self.r13 = r13
        self.r14 = r14
        self.r15 = r15
        self.cf = cf
        self.of = of 
        self.sf = sf
        self.zf = zf
        self.stack_size = stack_size
        self.stack_bytes_reverse = stack_bytes_reverse

    def get_registers_packaged(self) -> dict[str, int]:
        """
        This function is only a helper for the UI drawing.
        It makes it so we have to write less boiler-plate code.
        """
        return {
            "rip" : self.rip,
            "rax" : self.rax,
            "rbx" : self.rbx,
            "rcx" : self.rcx,
            "rdx" : self.rdx,
            "rdi" : self.rdi,
            "rsi" : self.rsi,
            "rsp" : self.rsp,
            "rbp" : self.rbp,
            "r8" : self.r8,
            "r9" : self.r9,
            "r10" : self.r10,
            "r11" : self.r11,
            "r12" : self.r12,
            "r13" : self.r13,
            "r14" : self.r14,
            "r15" : self.r15,
            "cf" : self.cf,
            "of" : self.of,
            "sf" : self.sf,
            "zf" : self.zf,
        }


class MainWindowViewModel(QObject):
    signal_emulator_path = pyqtSignal(str)
    signal_current_instruction = pyqtSignal(Instruction)
    signal_instructions = pyqtSignal(list)

    def __init__(self):
        super(QObject, self).__init__()

        self.instructions = list()
        self._current_instruction = 0
        self.emulator_path = None


    def current_instruction(self) -> Instruction:
        try:
            return self.instructions[self._current_instruction]
        except IndexError:
            return Instruction()


    def change_current_instruction(self, index: int):
        if index < 0 and index >= len(self.instructions):
            raise ValueError("Index of instruction must be in bounds")

        self._current_instruction = index
        self.signal_current_instruction.emit(self.current_instruction())


    def run_executable(self, path: str):
        if self.emulator_path is None:
            raise ValueError("Path to emulator is not specified.")

        process = subprocess.Popen([self.emulator_path, path], stdout=subprocess.PIPE)
        _, output_error = process.communicate(timeout=120)

        if process.returncode != 0:
            raw_error_output = output_error.decode().strip()
            raise RuntimeError(f"Error during emulator run: {raw_error_output}")

        self.load_run_data()


    def load_emulator(self, path: str):
        if not os.access(path, os.X_OK):
            raise ValueError("Specified file is not an executable")

        process = subprocess.Popen([path, "--gui-info"], stdout=subprocess.PIPE)
        output, _ = process.communicate(timeout=3)

        raw_output = output.decode().strip()

        if process.returncode != 0 or raw_output != "asemu":
            raise ValueError("Specified file is not Assembly Emulator")

        self.emulator_path = path

        self.signal_emulator_path.emit(self.emulator_path)

 
    def load_run_data(self):
        with open("/tmp/asemu_output", "r") as f:
            self.instructions = list()
            for line in f:
                split = line.split()

                stack_size = int(split[21])

                stack_bytes_reverse = []

                if stack_size:
                    stack_bytes_reverse=[int(x) for x in split[22:-1]]

                instruction = Instruction(
                    assembly=split[-1],
                    rip=int(split[0]),
                    rax=int(split[1]),
                    rbx=int(split[2]),
                    rcx=int(split[3]),
                    rdx=int(split[4]),
                    rdi=int(split[5]),
                    rsi=int(split[6]),
                    rsp=int(split[7]),
                    rbp=int(split[8]),
                    r8=int(split[9]),
                    r9=int(split[10]),
                    r10=int(split[11]),
                    r11=int(split[12]),
                    r12=int(split[13]),
                    r13=int(split[14]),
                    r14=int(split[15]),
                    r15=int(split[16]),
                    cf=int(split[17]),
                    of=int(split[18]),
                    sf=int(split[19]),
                    zf=int(split[20]),
                    stack_size=stack_size,
                    stack_bytes_reverse=stack_bytes_reverse)

                self.instructions.append(instruction)

        # Temporary fix
        if self.instructions:
            self.instructions[0].assembly = "BEGIN"
            self.instructions[-1].assembly = "END"

        self.signal_instructions.emit(self.instructions)
        self.signal_current_instruction.emit(self.current_instruction())


if __name__ == "__main__":
    print("This module shouldn't be run directly!", file=sys.stderr)
    sys.exit(1)
