#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

struct Packet
{
    uint8_t header[5];
    char data[];
};

int main()
{
    int sockfd;
    struct sockaddr_in dest_addr;

    // Create a socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    // Set up the destination address
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(7);                         // Port number
    dest_addr.sin_addr.s_addr = inet_addr("192.168.0.10"); // Destination IP address

    // Open and read the JSON file
    FILE *file = fopen("missions.json", "r");
    if (!file)
    {
        perror("fopen");
        close(sockfd);
        exit(1);
    }

    // Calculate the dynamic buffer size
    size_t headerSize = sizeof(struct Packet) - sizeof(char); // Subtract 1 byte for flexible array member
    size_t maxBufferSize = 1024 - headerSize;

    struct Packet *packet = malloc(sizeof(struct Packet) + maxBufferSize);

    packet->header[0] = 0xAA; // First byte of the header
    packet->header[1] = 0xBB; // Second byte of the header
    packet->header[2] = 0xCC; // Third byte of the header
    packet->header[3] = 0xDD; // Fourth byte of the header
    packet->header[4] = 0xEE; // Fifth byte of the header

    size_t bytesRead;
    while ((bytesRead = fread(packet->data, 1, maxBufferSize, file)) > 0)
    {
        if (sendto(sockfd, packet, bytesRead + headerSize, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) == -1)
        {
            perror("sendto");
            fclose(file);
            close(sockfd);
            free(packet);
            exit(1);
        }
        usleep(1000); // Wait for a short period between packets
    }

    printf("File sent successfully.\n");

    // Clean up
    fclose(file);
    close(sockfd);
    free(packet);

    return 0;
}
