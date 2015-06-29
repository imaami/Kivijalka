all: server

clean: clean-server

.PHONY: server
server:
	@$(MAKE) -C server server

.PHONY: clean-server
clean-server:
	@$(MAKE) -C server clean-server
