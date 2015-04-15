# sit - simple issue tracker
# See LICENSE file for copyright and license details.

include config.mk

CMD = sit sit-affected sit-assignee sit-birth sit-category sit-close \
      sit-cve sit-due sit-duplicate sit-dump sit-death sit-init \
      sit-list sit-milestone sit-new sit-reporter sit-severity \
      sit-status sit-title sit-unaffected sit-unvote sit-vote \
      sit-votes


all: sit doc
doc: man

sit: $(foreach C,${CMD},bin/${C})
man: $(foreach C,${CMD},bin/${C}.1)

bin/%.1: doc/man1/%.1 src/%.c doc/man1/generate
	@echo MAN $*.1
	@mkdir -p bin
	@env VERSION=${VERSION} doc/man1/generate < $< > $@

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

.PHONY: all doc sit man clean install uninstall

