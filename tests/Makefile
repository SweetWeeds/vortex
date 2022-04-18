all: runtime regression opencl riscv unittest

runtime:
	$(info Runtime)
	$(MAKE) -C runtime

regression:
	$(info Regression)
	$(MAKE) -C regression

opencl:
	$(info OpenCL)
	$(MAKE) -C opencl

riscv:
	$(info RISC-V)
	$(MAKE) -C riscv

unittest:
	$(info unittest)
	$(MAKE) -C unittest

clean:
	$(MAKE) clean -C runtime
	$(MAKE) clean -C regression
	$(MAKE) clean -C opencl
	$(MAKE) clean -C riscv
	$(MAKE) clean -C unittest

.PHONY: all runtime regression opencl riscv unittest