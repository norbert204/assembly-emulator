import sys

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


class MainWindowViewModel():
    def __init__(self):
        self.instructions = list()
        self._current_instruction = 0
        self.emulator_path = None

    def current_instruction(self) -> Instruction:
        try:
            return self.instructions[self._current_instruction]
        except IndexError:
            return Instruction()

    def run_executable(self, path: str):
        if self.emulator_path is None:
            raise ValueError("Path to emulator is not specified.")

        pass

    def load_emulator(self, path: str):
        pass


if __name__ == "__main__":
    print("This module shouldn't be run directly!", file=sys.stderr)
    sys.exit(1)
