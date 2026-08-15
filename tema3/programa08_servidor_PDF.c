#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>

#define PORT 4898
#define BACKLOG 1
#define PDF_PATH "holamundo.pdf"

int main(void)
{
  struct sockaddr_in servidor;
  struct sockaddr_in cliente;
  socklen_t longClient;
  int fd_s = -1, fd_c = -1;
  int opt = 1;

  // 1) socket()
  printf("1) Creando socket del servidor...\n");
  fd_s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (fd_s < 0)
  {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // Reutilizar puerto (opcional pero util en desarrollos)
  if (setsockopt(fd_s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
  {
    perror("setsockopt(SO_REUSEADDR)");
  }

  // 2) sockaddr_in servidor
  memset(&servidor, 0, sizeof(servidor));
  servidor.sin_family = AF_INET;
  servidor.sin_addr.s_addr = INADDR_ANY;
  servidor.sin_port = htons((u_short)PORT);
  memset(&(servidor.sin_zero), '\0', 8);

  // 3) bind()
  printf("2) Haciendo bind() al puerto %d...\n", PORT);
  if (bind(fd_s, (struct sockaddr *)&servidor, sizeof(servidor)) < 0)
  {
    perror("bind");
    close(fd_s);
    exit(EXIT_FAILURE);
  }

  // 4) listen()
  printf("3) Escuchando (listen)...\n");
  if (listen(fd_s, BACKLOG) < 0) {
        perror("listen");
        close(fd_s);
        exit(EXIT_FAILURE);
    }

  // 5) accept()
  longClient = sizeof(cliente);
  printf("4) Esperando conexion de un cliente...\n");
  fd_c = accept(fd_s, (struct sockaddr *)&cliente, &longClient);
  if (fd_c < 0)
  {
    perror("accept");
    close(fd_s);
    exit(EXIT_FAILURE);
  }

  // (Opcional) Resolver host del cliente
  struct hostent *info_cliente = gethostbyaddr((char *)&cliente.sin_addr, sizeof(struct in_addr), AF_INET);
  if (info_cliente && info_cliente->h_name)
  {
    printf("Conectado desde: %s\n", info_cliente->h_name);
  }
  else
  {
    char iptxt[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &cliente.sin_addr, iptxt, sizeof(iptxt));
    printf("Conectado desde IP: %s\n", iptxt);
  }

  // 6) Leer (y descartar) la solicitud HTTP del cliente (si llego desde un navegador)
  //    No necesitamos parsear; solo vaciamos el buffer de entrada para evitar datos pendientes.
  {
    char reqbuf[1024];
    // Usamos recv con MSG_DONTWAIT para no bloquear si no hay peticion (ej. clientes "raw").
    ssize_t n = recv(fd_c, reqbuf, sizeof(reqbuf), MSG_DONTWAIT);
    (void)n; // ignoramos el contenido
  }

  // 7) Abrir el archivo PDF y obtener su tamano
  int pdf_fd = open(PDF_PATH, O_RDONLY);
  if (pdf_fd < 0)
  {
    perror("open(hola_mundo.pdf)");
    // Enviar un 404 simple si no existe
    const char *hdr404 =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain; charset=utf-8\r\n"
            "Content-Length: 23\r\n"
            "Connection: close\r\n"
            "\r\n"
            "Archivo no encontrado.\n";
    send(fd_c, hdr404, strlen(hdr404), 0);
    close(fd_c);
    close(fd_s);
    return EXIT_FAILURE;
  }

  struct stat st;
  if (fstat(pdf_fd, &st) < 0)
  {
    perror("fstat");
    close(pdf_fd);
    close(fd_c);
    close(fd_s);
    return EXIT_FAILURE;
  }
  off_t pdf_size = st.st_size;

  // 8) Enviar cabeceras HTTP
  char header[512];
  int header_len = snprintf(
  header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/pdf\r\n"
        "Content-Length: %jd\r\n"
        "Cache-Control: no-store\r\n"
        "Connection: close\r\n"
        "\r\n",
        (intmax_t)pdf_size
  );
  if (header_len < 0 || header_len >= (int)sizeof(header)) {
    fprintf(stderr, "Error creando cabeceras HTTP\n");
    close(pdf_fd);
    close(fd_c);
    close(fd_s);
    return EXIT_FAILURE;
  }

  ssize_t sent = send(fd_c, header, header_len, 0);
  if (sent < 0)
  {
    perror("send(header)");
    close(pdf_fd);
    close(fd_c);
    close(fd_s);
    return EXIT_FAILURE;
  }

  // 9) Enviar el contenido del PDF (bucle robusto)
  {
    char buf[8192];
    ssize_t r;
    while ((r = read(pdf_fd, buf, sizeof(buf))) > 0)
    {
      char *p = buf;
      ssize_t left = r;
      while (left > 0)
      {
        ssize_t w = send(fd_c, p, left, 0);
        if (w < 0) {
          if (errno == EINTR)
            continue;
          perror("send(data)");
          close(pdf_fd);
          close(fd_c);
          close(fd_s);
          return EXIT_FAILURE;
        }
        left -= w;
        p += w;
      }
    }
    if (r < 0)
    {
      perror("read(pdf)");
      // cerramos igual
    }
  }

  // 10) Cerrar descriptores
  close(pdf_fd);
  printf("PDF enviado correctamente. Cerrando conexion.\n");
  close(fd_c);
  close(fd_s);
  return EXIT_SUCCESS;
}

