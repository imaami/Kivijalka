all: server

.PHONY: server
server: ws

.PHONY: ws
ws:
	@$(MAKE) -C ws wsserver

.PHONY: clean
clean: clean-server

.PHONY: clean-server
clean-server: clean-ws


.PHONY: clean-ws
clean-ws:
	@$(MAKE) -C ws clean-wsserver
