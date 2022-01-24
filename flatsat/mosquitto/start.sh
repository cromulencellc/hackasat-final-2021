#!/usr/bin/env bash

CMD="${@:-up}"

docker volume ls -q | grep '^streamsheets-data$' > /dev/null || docker volume create streamsheets-data

docker-compose \
	-f ./docker-compose.yml \
	$CMD
