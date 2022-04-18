all:
	$(info Third party)
	$(MAKE) -C third_party
	$(info HW)
	$(MAKE) -C hw
	$(info Sim)
	$(MAKE) -C sim
	$(info Driver)
	$(MAKE) -C driver
	$(info Runtime)
	$(MAKE) -C runtime
	$(info Tests)
	$(MAKE) -C tests

clean:
	$(MAKE) -C hw clean
	$(MAKE) -C sim clean
	$(MAKE) -C driver clean
	$(MAKE) -C runtime clean
	$(MAKE) -C tests clean