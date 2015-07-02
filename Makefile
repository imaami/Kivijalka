all: server doc

clean: clean-server clean-doc

.PHONY: server
server:
	@$(MAKE) -C server server

.PHONY: doc
doc:
	@$(MAKE) -C doc

.PHONY: clean-doc
clean-doc:
	@$(MAKE) -C doc clean
