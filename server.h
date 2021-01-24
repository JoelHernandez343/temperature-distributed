/**
 * Joel Hernández @ 2020
 * Github: https://github.com/JoelHernandez343
 * 
 */

void server_process();
void initialize_server();
void initialize_signals();
void bind_data(char buffer[5 * 32], char *parameters[5]);
void signal_handler(int sig);
void client_handler(int socket);