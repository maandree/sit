# sit - simple issue tracker
# See LICENSE file for copyright and license details.

include config.mk

OBJ = sit


all: sit
sit: bin/sit

bin/%: obj/%.o
	@echo CC -o $@
	@mkdir -p bin
	@${CC} -o $@ $< ${LDFLAGS}

obj/%.o: src/%.c
	@echo CC -c $<
	@mkdir -p obj
	@${CC} -c -o $@ ${CPPFLAGS} ${CFLAGS} $<

clean:
	@echo cleaning
	@-rm -rf obj bin

.PHONY: all sit clean install uninstall

