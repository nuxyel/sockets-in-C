#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h> // Para perror

#define PORT 8080
#define MAX_BUFFER_SIZE 1024
#define PASSWORD "senha123\n" // Ainda hardcoded, mas para fins de exemplo

void handle_client(int client_socket) {
    char buffer[MAX_BUFFER_SIZE];
    char line[MAX_BUFFER_SIZE];
    FILE *fp;
    ssize_t bytes_read;

    // Autenticação (ainda muito básica)
    memset(buffer, 0, MAX_BUFFER_SIZE);
    bytes_read = read(client_socket, buffer, MAX_BUFFER_SIZE - 1); // -1 para garantir espaço para null terminator
    if (bytes_read <= 0) {
        perror("Erro ao ler senha ou cliente desconectado");
        return;
    }
    buffer[bytes_read] = '\0'; // Garantir null termination

    if (strcmp(buffer, PASSWORD) != 0) {
        const char *msg = "Acesso Negado!\n";
        send(client_socket, msg, strlen(msg), 0);
        printf("Tentativa de acesso negado.\n");
        return;
    }
    const char *msg = "Autenticado. Digite 'exit' para sair.\n";
    send(client_socket, msg, strlen(msg), 0);
    printf("Cliente autenticado.\n");

    while (1) {
        memset(buffer, 0, MAX_BUFFER_SIZE);
        msg = "\nComando> ";
        send(client_socket, msg, strlen(msg), 0); // Envia um prompt

        bytes_read = read(client_socket, buffer, MAX_BUFFER_SIZE - 1);
        if (bytes_read <= 0) {
            perror("Cliente desconectado ou erro ao ler comando");
            break; // Sai do loop se o cliente desconectar
        }
        buffer[bytes_read] = '\0'; // Null-terminate a string lida

        // Remover o newline se presente
        buffer[strcspn(buffer, "\n")] = 0;

        // Comando de saída
        if (strcmp(buffer, "exit") == 0) {
            printf("Cliente solicitou saída.\n");
            break;
        }

        printf("Executando comando: '%s'\n", buffer);

        // ATENÇÃO: Ainda vulnerável a injeção de comandos!
        fp = popen(buffer, "r");
        if (fp == NULL) {
            perror("Erro ao executar comando");
            const char *error_msg = "Erro interno do servidor ao executar comando.\n";
            send(client_socket, error_msg, strlen(error_msg), 0);
            continue;
        }

        while (fgets(line, sizeof(line), fp) != NULL) {
            send(client_socket, line, strlen(line), 0);
        }

        if (pclose(fp) == -1) { // Verifica erro ao fechar pipe
            perror("Erro ao fechar pipe");
        }
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int opt = 1;

    // Criação do socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Erro na criação do socket");
        exit(EXIT_FAILURE);
    }

    // Opcional: Reutilizar o endereço e a porta
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Erro em setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind do socket à porta
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Erro no bind");
        exit(EXIT_FAILURE);
    }

    // Escuta por conexões
    if (listen(server_fd, 5) < 0) {
        perror("Erro no listen");
        exit(EXIT_FAILURE);
    }
    printf("Servidor escutando na porta %d...\n", PORT);

    while (1) { // Loop para aceitar múltiplas conexões (uma por vez com este modelo)
        printf("Aguardando nova conexão...\n");
        // Aceita uma nova conexão
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Erro no accept");
            // Não saia, apenas continue tentando aceitar novas conexões
            continue;
        }
        char *client_ip = inet_ntoa(address.sin_addr);
        int client_port = ntohs(address.sin_port);
        printf("Conexão aceita de %s:%d\n", client_ip, client_port);

        handle_client(new_socket); // Lida com o cliente conectado

        close(new_socket); // Fecha o socket do cliente
        printf("Conexão com %s:%d encerrada.\n", client_ip, client_port);
    }

    // Este código nunca será alcançado no loop while(1)
    // Para um servidor que encerra, você precisaria de uma forma de sinalizar o encerramento.
    close(server_fd);
    return 0;
}