FROM python:3.7

WORKDIR /elle

COPY drake/requirements.txt .

RUN pip3 install -r requirements.txt

COPY . .
