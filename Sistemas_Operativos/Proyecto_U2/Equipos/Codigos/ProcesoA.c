#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

void manejarConexion(int client_socket) {
    char buffer[1024];

    while (1) {
        // Enviar mensaje al servidor
        printf("Ingrese un mensaje para el servidor: ");
        fgets(buffer, sizeof(buffer), stdin);
        send(client_socket, buffer, sizeof(buffer), 0);

        // Recibir respuesta del servidor
        recv(client_socket, buffer, sizeof(buffer), 0);
        printf("Respuesta del servidor: %s\n", buffer);
    }

   
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;

    // Crear el socket del cliente
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la estructura sockaddr_in
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Conectar al servidor
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error al conectar al servidor");
        exit(EXIT_FAILURE);
    }

    printf("Conectado al servidor.\n");

    // Llamar a la función para manejar la conexión
    manejarConexion(client_socket);

    // Cerrar el socket del cliente (si es necesario cerrar la conexión al finalizar)
    close(client_socket);

    return 0;
}
