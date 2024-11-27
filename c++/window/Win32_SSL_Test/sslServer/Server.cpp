

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <sys/types.h>

#include <winsock2.h>
#pragma comment(lib,"ws2_32")
#include "openssl/rsa.h"
#include "openssl/crypto.h"
#include "openssl/x509.h"
#include "openssl/pem.h"
#include "openssl/ssl.h"
#include "openssl/err.h"

/*所有需要的参数信息都在此处以#define的形式提供*/
#define CERTF "server.crt" /*服务端的证书(需经CA签名)*/
#define KEYF "server.key" /*服务端的私钥(建议加密存储)*/
#define CACERT "root.crt" /*CA 的证书*/
#define PORT 443 /*准备绑定的端口*/

#define CHK_NULL(x) if ((x)==NULL) exit (1)
#define CHK_ERR(err,s) if ((err)==-1) { perror(s); exit(1); }
#define CHK_SSL(err) if ((err)==-1) { ERR_print_errors_fp(stderr); exit(2); }
/*
static int
pass_cb(char *buf, int len, int verify, void *password)
{
	snprintf(buf, len, "123456");
	return strlen(buf);
}
SSL_CTX_set_default_passwd_cb(ctx, pass_cb);
等价于：
SSL_CTX_set_default_passwd_cb_userdata(ctx, "123456");
*/

void  msg_cb(int write_p, int version, int content_type, const void *buf, size_t len, SSL *ssl, void *arg)
{
	BIO *bio = (BIO*)arg;
	const char *str_write_p, *str_version, *str_content_type = "", *str_details1 = "", *str_details2 = "";

	str_write_p = write_p ? ">>>" : "<<<";

	switch (version)
	{
	case SSL2_VERSION:
		str_version = "SSL 2.0";
		break;
	case SSL3_VERSION:
		str_version = "SSL 3.0 ";
		break;
	case TLS1_VERSION:
		str_version = "TLS 1.0 ";
		break;
	default:
		str_version = "???";
	case DTLS1_VERSION:
		str_version = "DTLS 1.0 ";
		break;
	case DTLS1_BAD_VER:
		str_version = "DTLS 1.0 (bad) ";
		break;
	}

	if (version == SSL2_VERSION)
	{
		str_details1 = "???";

		if (len > 0)
		{
			switch (((const unsigned char*)buf)[0])
			{
			case 0:
				str_details1 = ", ERROR:";
				str_details2 = " ???";
				if (len >= 3)
				{
					unsigned err = (((const unsigned char*)buf)[1] << 8) + ((const unsigned char*)buf)[2];

					switch (err)
					{
					case 0x0001:
						str_details2 = " NO-CIPHER-ERROR";
						break;
					case 0x0002:
						str_details2 = " NO-CERTIFICATE-ERROR";
						break;
					case 0x0004:
						str_details2 = " BAD-CERTIFICATE-ERROR";
						break;
					case 0x0006:
						str_details2 = " UNSUPPORTED-CERTIFICATE-TYPE-ERROR";
						break;
					}
				}

				break;
			case 1:
				str_details1 = ", CLIENT-HELLO";
				break;
			case 2:
				str_details1 = ", CLIENT-MASTER-KEY";
				break;
			case 3:
				str_details1 = ", CLIENT-FINISHED";
				break;
			case 4:
				str_details1 = ", SERVER-HELLO";
				break;
			case 5:
				str_details1 = ", SERVER-VERIFY";
				break;
			case 6:
				str_details1 = ", SERVER-FINISHED";
				break;
			case 7:
				str_details1 = ", REQUEST-CERTIFICATE";
				break;
			case 8:
				str_details1 = ", CLIENT-CERTIFICATE";
				break;
			}
		}
	}

	if (version == SSL3_VERSION ||
		version == TLS1_VERSION ||
		version == DTLS1_VERSION ||
		version == DTLS1_BAD_VER)
	{
		switch (content_type)
		{
		case 20:
			str_content_type = "ChangeCipherSpec";
			break;
		case 21:
			str_content_type = "Alert";
			break;
		case 22:
			str_content_type = "Handshake";
			break;
		}

		if (content_type == 21) /* Alert */
		{
			str_details1 = ", ???";

			if (len == 2)
			{
				switch (((const unsigned char*)buf)[0])
				{
				case 1:
					str_details1 = ", warning";
					break;
				case 2:
					str_details1 = ", fatal";
					break;
				}

				str_details2 = " ???";
				switch (((const unsigned char*)buf)[1])
				{
				case 0:
					str_details2 = " close_notify";
					break;
				case 10:
					str_details2 = " unexpected_message";
					break;
				case 20:
					str_details2 = " bad_record_mac";
					break;
				case 21:
					str_details2 = " decryption_failed";
					break;
				case 22:
					str_details2 = " record_overflow";
					break;
				case 30:
					str_details2 = " decompression_failure";
					break;
				case 40:
					str_details2 = " handshake_failure";
					break;
				case 42:
					str_details2 = " bad_certificate";
					break;
				case 43:
					str_details2 = " unsupported_certificate";
					break;
				case 44:
					str_details2 = " certificate_revoked";
					break;
				case 45:
					str_details2 = " certificate_expired";
					break;
				case 46:
					str_details2 = " certificate_unknown";
					break;
				case 47:
					str_details2 = " illegal_parameter";
					break;
				case 48:
					str_details2 = " unknown_ca";
					break;
				case 49:
					str_details2 = " access_denied";
					break;
				case 50:
					str_details2 = " decode_error";
					break;
				case 51:
					str_details2 = " decrypt_error";
					break;
				case 60:
					str_details2 = " export_restriction";
					break;
				case 70:
					str_details2 = " protocol_version";
					break;
				case 71:
					str_details2 = " insufficient_security";
					break;
				case 80:
					str_details2 = " internal_error";
					break;
				case 90:
					str_details2 = " user_canceled";
					break;
				case 100:
					str_details2 = " no_renegotiation";
					break;
				}
			}
		}

		if (content_type == 22) /* Handshake */
		{
			str_details1 = "???";

			if (len > 0)
			{
				switch (((const unsigned char*)buf)[0])
				{
				case 0:
					str_details1 = ", HelloRequest";
					break;
				case 1:
					str_details1 = ", ClientHello";
					break;
				case 2:
					str_details1 = ", ServerHello";
					break;
				case 11:
					str_details1 = ", Certificate";
					break;
				case 12:
					str_details1 = ", ServerKeyExchange";
					break;
				case 13:
					str_details1 = ", CertificateRequest";
					break;
				case 14:
					str_details1 = ", ServerHelloDone";
					break;
				case 15:
					str_details1 = ", CertificateVerify";
					break;
				case 3:
					str_details1 = ", HelloVerifyRequest";
					break;
				case 16:
					str_details1 = ", ClientKeyExchange";
					break;
				case 20:
					str_details1 = ", Finished";
					break;
				}
			}
		}
	}

	BIO_printf(bio, "%s %s%s [length %04lx]%s%s\n", str_write_p, str_version, str_content_type, (unsigned long)len, str_details1, str_details2);

	if (len > 0)
	{
		size_t num, i;

		BIO_printf(bio, "   ");
		num = len;
#if 0
		if (num > 16)
			num = 16;
#endif
		for (i = 0; i < num; i++)
		{
			if (i % 16 == 0 && i > 0)
				BIO_printf(bio, "\n   ");
			BIO_printf(bio, " %02x", ((const unsigned char*)buf)[i]);
		}
		if (i < len)
			BIO_printf(bio, " ...");
		BIO_printf(bio, "\n");
	}
	(void)BIO_flush(bio);
}

void cb_ssl_info(const SSL *s, int where, int ret)
{
	const char *str1, *str2;
	int w;
	w = where & ~SSL_ST_MASK;
	str1 = (w & SSL_ST_CONNECT ? "SSL_connect" : (w & SSL_ST_ACCEPT ?
		"SSL_accept" : "undefined")),
		str2 = SSL_state_string_long(s);

	if (where & SSL_CB_LOOP)
		fprintf(stderr, "(%s) %s\n", str1, str2);
	else if (where & SSL_CB_EXIT) {
		if (ret == 0)
			fprintf(stderr, "(%s) failed in %s\n", str1, str2);
		/* In a non-blocking model, we get a few of these "error"s simply because we're
		* calling "reads" and "writes" on the state-machine that are virtual NOPs
		* simply to avoid wasting the time seeing if we *should* call them. Removing
		* this case makes the "-out_state" output a lot easier on the eye. */
#if 0
		else if (ret < 0)
			fprintf(fp_cb_ssl_info, "%s:error in %s\n", str1, str2);
#endif
	}
}
int main()
{
	int err;
	SOCKET listen_sd;
	SOCKET sd;
	struct sockaddr_in sa_serv;
	struct sockaddr_in sa_cli;
	int client_len;
	SSL_CTX* ctx;
	SSL* ssl;
	X509* client_cert;
	char* str = (char*)malloc(sizeof(char) * 1024);
	char buf[4096];
	SSL_METHOD *meth;
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
		printf("WSAStartup()fail:%d\n", GetLastError());
		return -1;
	}

	SSL_load_error_strings(); /*为打印调试信息作准备*/
	OpenSSL_add_ssl_algorithms(); /*初始化*/
	meth = (struct ssl_method_st *)TLSv1_server_method(); /*采用什么协议(SSLv2/SSLv3/TLSv1)在此指定*/

	ctx = SSL_CTX_new(meth);
	CHK_NULL(ctx);
	
	//SSL_VERIFY_FAIL_IF_NO_PEER_CERT    即使客户端没有发送证书 服务端和客户端也可以完成握手
	//SSL_VERIFY_PEERFAIL_IF_NO_PEER_CERT  客户端设置了就可以验证服务端的证书
	//SSL_VERIFY_NONE表示不验证
	SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL); /*验证与否*/   //制定证书验证方式的函数
	SSL_CTX_load_verify_locations(ctx, CACERT, NULL); /*若验证,则放置CA证书*/

	//SSL_CTX_set_default_passwd_cb_userdata(ctx, "123456");  //直接将口令设置好

	if (SSL_CTX_use_certificate_file(ctx, CERTF, SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		exit(3);
	}
	if (SSL_CTX_use_PrivateKey_file(ctx, KEYF, SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		exit(4);
	}

	if (!SSL_CTX_check_private_key(ctx)) {
		printf("Private key does not match the certificate public key\n");
		exit(5);
	}

	SSL_CTX_set_cipher_list(ctx, "RC4-MD5");
	SSL_CTX_set_info_callback(ctx, cb_ssl_info);
	/*开始正常的TCP socket过程.................................*/
	printf("Server:Begin TCP socket...\n");

	listen_sd = socket(AF_INET, SOCK_STREAM, 0);
	CHK_ERR(listen_sd, "socket");

	memset(&sa_serv, '\0', sizeof(sa_serv));
	sa_serv.sin_family = AF_INET;
	sa_serv.sin_addr.s_addr = INADDR_ANY;
	sa_serv.sin_port = htons(PORT);

	err = bind(listen_sd, (struct sockaddr*) &sa_serv, sizeof(sa_serv));
	CHK_ERR(err, "bind");

	/*接受TCP链接*/
	err = listen(listen_sd, 5);
	CHK_ERR(err, "listen");

	client_len = sizeof(sa_cli);
	sd = accept(listen_sd, (struct sockaddr*) &sa_cli, &client_len);
	CHK_ERR(sd, "accept");
	closesocket(listen_sd);

	printf("Connection from %s, port %d\n", inet_ntoa(sa_cli.sin_addr), sa_cli.sin_port);

	/*TCP连接已建立,进行服务端的SSL过程. */

	ssl = SSL_new(ctx);
	CHK_NULL(ssl);
	SSL_set_fd(ssl, sd);

	BIO *bio_c_out = BIO_new_fp(stdout, BIO_NOCLOSE);
	SSL_set_msg_callback(ssl, msg_cb);
	SSL_set_msg_callback_arg(ssl, bio_c_out);


	err = SSL_accept(ssl);
	printf("SSL_accept finished\n");
	CHK_SSL(err);

	/*打印所有加密算法的信息(可选)*/
	printf("SSL connection using %s\n", SSL_get_cipher(ssl));

	/*得到服务端的证书并打印些信息(可选) */
	client_cert = SSL_get_peer_certificate(ssl);
	if (client_cert != NULL) {
		printf("Client certificate:\n");

		str = X509_NAME_oneline(X509_get_subject_name(client_cert), 0, 0);
		CHK_NULL(str);
		printf("\tsubject: %s\n", str);
		/*free (str);*/
		memset(&str, '\0', sizeof(str));

		str = X509_NAME_oneline(X509_get_issuer_name(client_cert), 0, 0);
		CHK_NULL(str);
		printf("\t issuer: %s\n", str);
		/*free (str);*/
		memset(&str, '\0', sizeof(str));

		X509_free(client_cert);/*如不再需要,需将证书释放 */
	}
	else
		printf("Client does not have certificate.\n");

	/* 数据交换开始,用SSL_write,SSL_read代替write,read */
	err = SSL_read(ssl, buf, sizeof(buf) - 1);
	CHK_SSL(err);
	buf[err] = '\0';
	printf("Got %d chars:'%s'\n", err, buf);

	err = SSL_write(ssl, "I hear you.", strlen("I hear you."));
	CHK_SSL(err);

	/* 收尾工作*/
	free(str);
	shutdown(sd, 2);
	SSL_free(ssl);
	SSL_CTX_free(ctx);

	return 0;
}