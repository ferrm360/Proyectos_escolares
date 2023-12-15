#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>

#define PORT 8081 // Puerto para la comunicación con B

int main()
{
    int server_socket, new_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    char buffer[1024];

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
    if (listen(server_socket, 5) == -1)
    {
        perror("Error al intentar escuchar");
        exit(EXIT_FAILURE);
    }

    printf("Esperando conexiones...\n");

    while (1)
    {
        // Aceptar la conexión entrante de B
        if ((new_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size)) == -1)
        {
            perror("Error al aceptar la conexión de B");
            continue; // Continuar esperando conexiones
        }

        printf("Conexión aceptada desde B: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Ciclo para manejar múltiples solicitudes de B
        while (1)
        {
            // Recibir mensaje de B
            ssize_t bytes_recv = recv(new_socket, buffer, sizeof(buffer), 0);
            if (bytes_recv <= 0)
            {
                if (bytes_recv == 0)
                {
                    // Conexión cerrada por B de manera ordenada
                    printf("Conexión cerrada por B\n");
                }
                else
                {
                    perror("Error al recibir datos o conexión cerrada por B");
                }
                break; // Salir del ciclo interno si la conexión se cierra
            }

            printf("Mensaje de B: %s\n", buffer);

            // Convertir a mayúsculas
            int i = 0;
            while (buffer[i] != '\0')
            {
                buffer[i] = toupper(buffer[i]);
                i++;
            }

            // Enviar respuesta a B
            printf("Enviando respuesta a B: %s\n", buffer);
            send(new_socket, buffer, bytes_recv, 0);
        }

        // Cerrar socket de conexión con B
        close(new_socket);
    }

    
    close(server_socket);

    return 0;
}
