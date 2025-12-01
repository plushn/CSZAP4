# HTTP serverを作ってみた

## C言語 + system call で HTTP Server を作成する課題

```bash
GET /calc?query=2+10 HTTP/1.1
```

という入力に対して、

```bash
HTTP/1.1 200 OK
Content-Length: 2

12
```

を返す。

---
## 使い方

1. サーバーをコンパイル

```bash
clang -o server server.c
# 署名 (Killed: 9 で強制終了されてしまうため署名する。自分のMacの環境だけかもしれません。)
codesign -s - -f server
```

2. サーバー実行

```bash
./server
```

3. 別のターミナルでクライアントをコンパイル

```bash
clang -o client client.c
# 同様に署名
codesign -s - -f client
```

4. クライアント実行

```bash
./server
```

5. 結果

```bash
サーバーに接続しました。
リクエスト送信:
GET /calc?query=10+2 HTTP/1.1


サーバーから受信:
HTTP/1.1 200 OK
Content-Length: 2

12
```
