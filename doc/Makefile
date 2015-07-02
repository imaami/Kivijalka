DOT := /usr/bin/dot

all: doc

.PHONY: doc
doc: doc-png

.PHONY: doc-png
doc-png: threads.gv
	@$(DOT) -Tpng -othreads.png threads.gv

.PHONY: clean
clean: clean-doc

.PHONY: clean-doc
clean-doc: clean-doc-png

.PHONY: clean-doc-png
clean-doc-png:
	@-rm *.png
