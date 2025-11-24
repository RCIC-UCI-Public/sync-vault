# Copyright (C) 2006-2018 Free Software Foundation, Inc.
# This Makefile.am is free software; the Free Software Foundation
# gives unlimited permission to copy, distribute and modify it.

AM_CFLAGS =  -DSERVICE_NAME=\"sync_vault_server\"

CLEANFILES = sync_vault.h sync_vault_clnt.c
sbin_PROGRAMS = sync_vault_server sync_vault_client wait-for-file
sbin_SCRIPTS = catarg.sh
sync_vault_server_SOURCES = sync_vault_server.c sync_vault_svc.c 
sync_vault_client_SOURCES = sync_vault_client_custom.c sync_vault_clnt.c
wait_for_file_SOURCES = wait-for-file.c

NEEDHEADER = $(sbin_PROGRAMS:=.o)

$(NEEDHEADER): sync_vault.h

sync_vault.h: sync_vault.x
	rpcgen -N -h -o $@ $^ 

sync_vault_clnt.c: sync_vault.x
	rpcgen -N -l -o $@ $^



