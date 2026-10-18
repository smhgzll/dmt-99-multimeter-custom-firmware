# Offline entry points. This Makefile never flashes or connects to hardware.
PYTHON ?= python3
.PHONY: help firmware doom asic studio-tools test test-firmware test-doom test-asic test-rvswd audit
help:
	@echo 'firmware     POTAT-99 USB update BIN (build only)'
	@echo 'doom         DOOM USB update BIN + desktop simulator (requires local WAD)'
	@echo 'asic         standalone UART driver RV32 static library'
	@echo 'studio-tools native renderer and JPEG checker for Studio'
	@echo 'test         offline C/Python tests; Studio pytest is separate'
	@echo 'audit        public source/privacy and preserved artifact checks'
firmware:
	$(MAKE) -C reconstructed firmware-update
doom:
	$(MAKE) -C projects/doom all PYTHON='$(PYTHON)'
asic:
	$(MAKE) -C projects/asic/firmware
studio-tools:
	$(MAKE) -C reconstructed studio-tools
test: test-firmware test-doom test-asic test-rvswd
test-firmware:
	$(MAKE) -C reconstructed check
test-doom:
	$(MAKE) -C projects/doom test PYTHON='$(PYTHON)'
test-asic:
	cd projects/asic && PYTHONDONTWRITEBYTECODE=1 $(PYTHON) -m unittest discover -s tests -v
test-rvswd:
	cd projects/rvswd && CC='$(CC)' bash tests/run.sh
audit:
	PYTHONDONTWRITEBYTECODE=1 $(PYTHON) tools/audit_repository.py
