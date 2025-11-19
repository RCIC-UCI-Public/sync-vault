# Compiler and flags
CC = gcc
CFLAGS = -Wall -I/usr/include/tirpc
LDFLAGS = -ltirpc

# RPC Files
RPCX = sync_vault.x
HEADERS = sync_vault.h
CLIENT_STUB = sync_vault_clnt.c
SERVER_STUB = sync_vault_svc.c
CLIENT_BIN = sync_vault_client
SERVER_BIN = sync_vault_server
CLIENT_SRC = sync_vault_client.c
SERVER_SRC = sync_vault_server.c

# Separate wait-for-file binary
WAIT_FOR_FILE_BIN = wait-for-file
WAIT_FOR_FILE_SRC = wait-for-file.c

# Phony targets
.PHONY: all clean distclean install

all: $(CLIENT_BIN) $(SERVER_BIN) $(WAIT_FOR_FILE_BIN)

# Generate header and RPC sources
$(HEADERS): $(RPCX)
	rpcgen -C -h $(RPCX) -o $(HEADERS)

$(CLIENT_STUB): $(RPCX)
	rpcgen -C -l $(RPCX) -o $(CLIENT_STUB)

$(SERVER_STUB): $(RPCX)
	rpcgen -C -m $(RPCX) -o $(SERVER_STUB)

# Build RPC client binary
$(CLIENT_BIN): $(CLIENT_SRC) $(CLIENT_STUB) $(HEADERS)
	$(CC) -o $@ $(CLIENT_SRC) $(CLIENT_STUB) $(CFLAGS) $(LDFLAGS)

# Build RPC server binary
$(SERVER_BIN): $(SERVER_SRC) $(SERVER_STUB) $(HEADERS)
	$(CC) -o $@ $(SERVER_SRC) $(SERVER_STUB) $(CFLAGS) $(LDFLAGS)

# Build wait-for-file binary
$(WAIT_FOR_FILE_BIN): $(WAIT_FOR_FILE_SRC)
	$(CC) -o $@ $(WAIT_FOR_FILE_SRC) $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(CLIENT_BIN) $(SERVER_BIN) $(WAIT_FOR_FILE_BIN) $(HEADERS) $(CLIENT_STUB) $(SERVER_STUB)

distclean: clean

install:
	mkdir -p $(DESTDIR)/usr/bin
	cp -a $(CLIENT_BIN) $(SERVER_BIN) $(WAIT_FOR_FILE_BIN) $(DESTDIR)/usr/bin/
