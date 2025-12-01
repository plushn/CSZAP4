#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h> // IPアドレス変換用
#include <sys/socket.h>

#define PORT 8080
#define BUF_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char *buffer;
    
    char *message = "GET /calc?query=10+2 HTTP/1.1\r\n"
                    "\r\n"; // ← 空行でヘッダー終了の合図

    // ==========================================
    // 1. ソケット作成
    // ==========================================
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket error");
        exit(1);
    }

    // ==========================================
    // 2. 接続先の設定
    // ==========================================
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    
    // 文字列 "127.0.0.1" を、ネットワーク用のバイナリ形式に変換してセット
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("inet_pton error");
        exit(1);
    }

    // ==========================================
    // 3. サーバーに接続
    // ==========================================
    // connect()は、サーバーの accept() に対応する。
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect error");
        exit(1);    
    }
    printf("サーバーに接続しました。\n");

    // ==========================================
    // 4. リクエスト送信
    // ==========================================
    if (write(sock, message, strlen(message)) < 0) {
        perror("write error");
        exit(1);
    }
    printf("リクエスト送信: \n%s\n", message);

    // ==========================================
    // 5. レスポンス受信
    // ==========================================
    // 受信用のメモリ確保
    buffer = (char *)malloc(BUF_SIZE);
    if (buffer == NULL){
        perror("malloc error");
        exit(1);
    }
    
    memset(buffer, 0, BUF_SIZE);

    // サーバーからの返事を待つ
    int str_len = read(sock, buffer, BUF_SIZE - 1);
    if (str_len < 0){
        perror("read error");
        free(buffer);
        exit(1);
    };
        

    printf("サーバーから受信:\n%s\n", buffer);

    // 終了処理
    close(sock);
    free(buffer);

    return 0;
}
