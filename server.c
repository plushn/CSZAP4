#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUF_SIZE 1024

int main() {
    int server_sock, client_sock;
    struct sockaddr_in addr;
    int addr_len = sizeof(addr); // クライアントアドレスの長さ sizeof(型名): 指定したデータ型が占めるバイト数を返す
    char *buffer;
    char response[BUF_SIZE];

    // ==========================================
    // 1. サーバー処理
    // ==========================================
    
    // [socket] ソケットを作成
    server_sock = socket(AF_INET, SOCK_STREAM, 0);

    // アドレス情報を設定
    addr.sin_family = AF_INET;           // AF_INET: IPv4, AF_INET6: IPv6
    addr.sin_addr.s_addr = INADDR_ANY;   // どのアドレスでもOK '0.0.0.0'
    addr.sin_port = htons(8080);         // ポート8080

    // [bind] ソケットにアドレスを紐づけ
    if(bind(server_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0){
        perror("bind error");
        exit(1);
    }

    // [listen] 接続待ち状態にする
    if(listen(server_sock, 5) < 0){
        perror("listen error");
        exit(1);
    }

    printf("サーバー起動: 8080番ポートで待機中...\n");

    // ==========================================
    // 2. クライアント処理
    // ==========================================
    while (1) {
        // [accept] 接続を受け入れる
        client_sock = accept(server_sock, (struct sockaddr *)&addr, (socklen_t *)&addr_len);
        if (client_sock == -1) {
            perror("socket error");
            exit(EXIT_FAILURE);
        }

        // [read] データを受信
        buffer = (char *)malloc(BUF_SIZE);  // 動的メモリ確保。ヒープ領域にBUF_SIZE分のメモリを確保。
        if (buffer == NULL) { // メモリ確保失敗時の安全策
            perror("malloc error");
            close(client_sock);
            continue;
        }

        memset(buffer, 0, BUF_SIZE); // バッファを初期化する memset(): メモリ領域を指定する文字で埋める
        ssize_t read_size = read(client_sock, buffer, BUF_SIZE - 1);
        if (read_size < 0){
            perror("read error");
            free(buffer);
            close(client_sock);
            continue;
        }

        // --- ここから計算ロジック ---
        int num1, num2;
        char *query_pos = strstr(buffer, "query="); // "query=" の場所を探す
        
        if (query_pos != NULL) {
            if (sscanf(query_pos, "query=%d+%d", &num1, &num2) == 2){ // 数字を取り出す
                int ans = num1 + num2;

                char body_str[32];
                sprintf(body_str, "%d", ans);
                int len = strlen(body_str);

                // レスポンスを作成
                sprintf(response, 
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Length: %d\r\n"
                    "\r\n"
                    "%s", 
                    len, body_str); // 簡易的な長さ計算
                } else{
                    // Bad Request
                    sprintf(response, "HTTP/1.1 400 Bad Request\r\n\r\n");
                }
            } else {
                // Not Found
                sprintf(response, "HTTP/1.1 404 Not Found\r\n\r\n");
        }
        // -----------------------------------------------------------

        // [write] データを送信
        if (write(client_sock, response, strlen(response)) < 0) {
            perror("write error");
        }

        // [close] 接続を切る
        close(client_sock);

        free(buffer);
    }

    return 0;
}
