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

.PHONY: tar
tar: clean-tar
	tar cJf kivijalka.tar.xz $(shell git ls-tree -r --name-only HEAD)

.PHONY: clean-tar
clean-tar:
	rm -f kivijalka.tar.xz
