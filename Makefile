# sit - simple issue tracker
# See LICENSE file for copyright and license details.

include config.mk

CMD = sit sit-affected sit-assignee sit-birth sit-category sit-cve \
      sit-due sit-death sit-init sit-milestone sit-reporter \
      sit-severity sit-status sit-title sit-unaffected sit-unvote \
      sit-vote sit-votes


all: sit
sit: $(foreach C,${CMD},bin/${C})

bin/%: obj/%.o obj/util.o
	@echo CC -o $@
	@mkdir -p bin
	@${CC} -o $@ $^ ${LDFLAGS}

obj/%.o: src/%.c src/*.h
	@echo CC -c $<
	@mkdir -p obj
	@${CC} -c -o $@ ${CPPFLAGS} ${CFLAGS} $<

clean:
	@echo cleaning
	@-rm -rf obj bin

.PHONY: all sit clean install uninstall

