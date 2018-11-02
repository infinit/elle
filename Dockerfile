FROM python:3.7

WORKDIR /elle

COPY drake/requirements.txt .

RUN pip3 install -r requirements.txt

COPY . .

# since Drake writes all its temp files right into the build dir,
# we have to have it on the .dockerignore file
# but at the same time we also want build scripts in the image, so...
RUN git checkout _build
# (yes, this is a bit shitty, maybe we want to have drake use a working
# dir well separated from anything checked in - this would also allow
# first building the base image with just the dependencies compiled, then
# compiling elle itself, thus using the build cache much more
# efficiently)

RUN cd _build/linux64 && ./drake //build
