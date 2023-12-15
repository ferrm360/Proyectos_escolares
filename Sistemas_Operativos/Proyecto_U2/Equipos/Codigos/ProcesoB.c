#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CONNECTIONS 5

typedef struct {
    int socket;
    struct sockaddr_in client_addr;
} ConnectionInfo;

void *manejarConexion(void *arg)
{
    ConnectionInfo *info = (ConnectionInfo *)arg;
    char buffer[1024];

    printf("Conexión aceptada desde %s:%d\n", inet_ntoa(info->client_addr.sin_addr), ntohs(info->client_addr.sin_port));

    while (1)
    {
        ssize_t bytes_recv = recv(info->socket, buffer, sizeof(buffer), 0);
        if (bytes_recv <= 0)
        {
            perror("Error al recibir datos o conexión cerrada");
            break;
        }

        // Enviar mensaje al proceso C
        printf("Enviando mensaje a C: %s\n", buffer);

        // Crear un socket para la comunicación con el proceso C
        int socket_to_C = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_to_C == -1)
        {
            perror("Error al crear el socket para C");
            close(info->socket);
            free(info);
            pthread_exit(NULL);
        }

        struct sockaddr_in c_addr;
        c_addr.sin_family = AF_INET;
        c_addr.sin_port = htons(8081);  // Puerto para la comunicación con C
        c_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Dirección IP de localhost (puedes cambiarla según tus necesidades)

        // Conectar con el proceso C
        if (connect(socket_to_C, (struct sockaddr *)&c_addr, sizeof(c_addr)) == -1)
        {
            perror("Error al conectar con C");
            close(socket_to_C);
            continue;  // Puedes manejar esto según tus necesidades
        }

        // Enviar mensaje a C
        send(socket_to_C, buffer, bytes_recv, 0);

        // Recibir respuesta de C (ya en mayúsculas)
        recv(socket_to_C, buffer, sizeof(buffer), 0);
        printf("Respuesta de C: %s\n", buffer);

        // Enviar respuesta de C al cliente A
        printf("Enviando respuesta al cliente A... \n");
        send(info->socket, buffer, bytes_recv, 0);

        // Cerrar el socket de comunicación con C
        close(socket_to_C);
    }

    close(info->socket);
    free(info);
    pthread_exit(NULL);
}

int main()
{
    int server_socket, new_socket;
    struct sockaddr_in server_addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    pthread_t thread_id;
    ConnectionInfo *info;

    // Crear el socket del servidor
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la estructura sockaddr_in
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Enlazar el socket a la dirección y puerto
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Error al enlazar el socket");
        exit(EXIT_FAILURE);
    }

    // Escuchar por conexiones entrantes
    if (listen(server_socket, MAX_CONNECTIONS) == -1)
    {
        perror("Error al intentar escuchar");
        exit(EXIT_FAILURE);
    }

    printf("Esperando conexiones...\n");

    while (1)
    {
        // Aceptar la conexión entrante
        info = (ConnectionInfo *)malloc(sizeof(ConnectionInfo));

        if ((new_socket = accept(server_socket, (struct sockaddr *)&(info->client_addr), &addr_size)) == -1)
        {
            perror("Error al aceptar la conexión");
            free(info);
            exit(EXIT_FAILURE);
        }

        info->socket = new_socket;

        // Crear un hilo para manejar la conexión
        if (pthread_create(&thread_id, NULL, manejarConexion, (void *)info) != 0)
        {
            perror("Error al crear el hilo");
            free(info);
            exit(EXIT_FAILURE);
        }
    }

    close(server_socket);

    return 0;
}
