/* sync_vault.x - RPC interface to execute a script with hostname argument */

program SYNC_VAULT_PROG {
    version SYNC_VAULT_VERS {
        string EXEC_SCRIPT(string) = 1;
    } = 1;
} = 0x24551111;
