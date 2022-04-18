all:
	$(info sim:simx)
	$(MAKE) -C simx
	$(info sim:rtlsim)
	$(MAKE) -C rtlsim
	$(info sim:vlsim)
	$(MAKE) -C vlsim
	
clean:
	$(MAKE) -C simx clean
	$(MAKE) -C rtlsim clean
	$(MAKE) -C vlsim clean