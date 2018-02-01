DOCKERIZE:=1

all: check

ifeq ($(DOCKERIZE),0)
check:
	cd _build && ./drake //check
else
check: image/trusty-check
	USER=$$USER && docker run --rm --volume $$PWD:$$PWD --workdir $$PWD --env DRAKE_RAW mefyl/drake/trusty-check sh -e -c "useradd --uid $$(id -u) $$USER && chown $$USER:$$USER $$(getent passwd $$USER | cut -d : -f 6) && su $$USER -c 'make DOCKERIZE=0 check'"
endif

image/%:
	docker build . --target $(patsubst image/%,%,$@) --tag mefyl/drake/$(patsubst image/%,%,$@)
