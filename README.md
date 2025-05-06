# e3rsa: RSA 密钥生成、加密与签名工具

- 默认将 `p,q,n,e,d` 写入 `generated_txts/` 目录
- 测试时使用的密钥和数据文件位于 `txts/` 目录：`n.txt`、`e.txt`、`d.txt`（密钥），`plain.txt`（明文），`cipher.txt`、`sign.txt`（期望输出）

## 功能

1. 生成大素数 p、q （均 > 1e10），计算并输出公钥 `{e, n}` 与私钥 `{d, n}`。
2. 使用公钥加密明文，生成密文。
3. 使用私钥签名明文，生成签名。

所有大整数均以 **十六进制字符串** 形式写入和读取。

## 目录结构

```
RSAimplementation/
├── Makefile       # 包含 test-encrypt、test-sign
├── main.c
├── rsa.c
├── rsa.h
├── README.md
├── LICENSE
├── generated_txts/ # 默认输出 p.txt, q.txt, n.txt, e.txt, d.txt
└── txts/           # 测试文件: plain.txt, cipher.txt, sign.txt
```

## 依赖

- GCC 或兼容的 C 编译器
- GMP（GNU Multiple Precision Arithmetic Library）
- pkg-config（可选，用于在 Linux/macOS 上自动检测 GMP）

## 编译

在项目根目录执行：

```bash
make
```

Makefile 会使用 `pkg-config --cflags --libs gmp` 检测 GMP。若系统无 pkg-config，可手动安装或修改 Makefile 中的 `CFLAGS` 和 `LDLIBS`。

## 使用

直接运行可执行文件, 会生成 `p.txt`、`q.txt`、`n.txt`、`e.txt`、`d.txt`， 并放到 `generated_txts`文件夹下。:  
```bash
./e3rsa
`

```bash
e3rsa -p plainfile -n nfile [-e efile] [-d dfile] -c cipherfile
```
参数：
`-p plainfile` 指定明文文件的位置和名称
`-n nfile` 指定存放整数 n 的文件的位置和名称
`-e efile` 在数据加密时，指定存放整数 e 的文件的位置和名称
`-d dfile` 在数字签名时，指定存放整数 d 的文件的位置和名称
`-c cipherfile` 指定密文文件的位置和名称

## 快速测试

- 加密测试：

  ```bash
  make test-encrypt
  ```
  对 `txts/plain.txt` 加密，并自动比较输出 `txts/cipher_out.txt` 与 `txts/cipher.txt`。

- 签名测试：

  ```bash
  make test-sign
  ```
  对 `txts/plain.txt` 签名，并自动比较输出 `txts/sign_out.txt` 与 `txts/sign.txt`。

## 其他使用示例

```bash
# 自定义加密输出文件
./e3rsa -p txts/plain.txt -n generated_txts/n.txt -e generated_txts/e.txt -c output_cipher.txt

# 自定义签名输出文件
./e3rsa -p txts/plain.txt -n generated_txts/n.txt -d generated_txts/d.txt -c output_sign.txt
```

## 清理

```bash
make clean
```  
该命令将删除可执行文件和 `generated_txts/` 目录。

## 注意事项

- 输入的十六进制字符串大小写均可识别。
- 若需处理二进制或分块大文件，可自行扩展读写逻辑。
