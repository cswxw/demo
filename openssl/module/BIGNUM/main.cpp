//简单的操作
/*
	BIGNUM *p = BN_new();
	BIO *out ;
	BN_hex2bn(&p,"aabbccddee");
	out = BIO_new_fp(stdout, BIO_NOCLOSE);
	BN_print(out,p);

*/
#include <stdio.h>  
#include <openssl/bn.h>  
#include <openssl/rand.h>  
#include <openssl/err.h>  
  
void bn_printf(BIGNUM * a, int n)  
{  
	printf(”0x”);  
	BN_print_fp(stdout, a);  
	if (n)  
		printf(”\n”);  
}  
  
void bn_hex_printf(BIGNUM * a)  
{  
	char *p = BN_bn2hex(a);  
	printf(”0x%s\n”, p);  
	OPENSSL_free(p);  
}  
  
void bn_dec_printf(BIGNUM * a)  
{  
	char *p = BN_bn2dec(a);  //取a的10进制值，返回一个字符串的指针。此指针要使用完后，手动使用OPENSSL_free释放
	printf(”%s\n”, p);  
	OPENSSL_free(p);  
}  
  
int main(int argc, char *argv[])  
{  
	int bits = 3;  
  
	if (argc > 1) {  
		bits = atoi(argv[1]);  
	}  
	if (bits <= 0) {  
		bits = 8;  
	}  
	printf(”bits: %d\n”, bits);  
  
	BN_CTX *ctx;  
	BIGNUM a, b, c, d;  
  
	ctx = BN_CTX_new();  
	if (ctx == NULL)  
		exit(1);  
  
	BN_init(&a);  // 初始化内存
	BN_init(&b);  
	BN_init(&c);  
	BN_init(&d);  
  
	BN_rand(&a, bits, 1, 1);  //BN_rand(BIGNUM *rnd, int bits, int top, int bottom)
	printf(”BN_rand a: ”);  
	bn_printf(&a, 1);  
	bn_hex_printf(&a);  
	bn_dec_printf(&a);  
  
	BN_add_word(&a, 1);  //大数a加上w，值储存在a中，返回1或者0
	printf(”BN_add_word a + 1\n”);  
	bn_printf(&a, 1);  
  
	BN_free(&a);  
	BN_init(&a);  
  
	BN_rand(&a, bits, 1, 0);  
	printf(”BN_rand a: ”);  
	bn_printf(&a, 1);  
  
	BN_rand(&b, bits, 0, 1);  
	printf(”BN_rand b: ”);  
	bn_printf(&b, 1);  
  
	BN_add(&c, &a, &b);  
	printf(”BN_add a + b\n”);  
	bn_printf(&a, 0);  
	printf(” + ”);  
	bn_printf(&b, 0);  
	printf(” = ”);  
	bn_printf(&c, 1);  
  
	BN_sub(&c, &a, &b);  //int BN_sub(BIGNUM *r, const BIGNUM *a, const BIGNUM *b); 计算a与b的差，值储存在r中。返回1或者0
	printf(”BN_sub a - b\n”);  
	bn_printf(&a, 0);  
	printf(” - ”);  
	bn_printf(&b, 0);  
	printf(” = ”);  
	bn_printf(&c, 1);  
  
	BN_mul(&c, &a, &b, ctx);  
	printf(”BN_mul a * b\n”);  
	bn_printf(&a, 0);  
	printf(” * ”);  
	bn_printf(&b, 0);  
	printf(” = ”);  
	bn_printf(&c, 1);  
  
	BN_div(&d, &c, &a, &b, ctx);  
	printf(”BN_div a / b\n”);  
	bn_printf(&a, 0);  
	printf(” / ”);  
	bn_printf(&b, 0);  
	printf(” = ”);  
	bn_printf(&d, 1);  
	printf(”remainder ”);  
	bn_printf(&c, 1);  
  
	BN_mod(&c, &a, &b, ctx);  
	printf(”BN_mod a %% b\n”);  
	bn_printf(&a, 0);  
	printf(” %% ”);  
	bn_printf(&b, 0);  
	printf(” = ”);  
	bn_printf(&c, 1);  
  
	BN_exp(&c, &a, &b, ctx);  //int BN_exp(BIGNUM *r, BIGNUM *a, BIGNUM *p, BN_CTX *ctx); 计算a的p次方，值储存在r中。返回1或者0
	printf(”BN_exp a ^ b\n”);  
	bn_printf(&a, 0);  
	printf(” ^ ”);  
	bn_printf(&b, 0);  
	printf(” = ”);  
	if (BN_num_bits(&c) < 64 * 8) {  
		bn_printf(&c, 1);  
	} else {  
		printf(”BN_num_bits(c) %d too long, not print\n”,  
			   BN_num_bits(&c));  
	}  
  
	BN_set_word(&a, 100);  
	printf(”BN_set_word a: ”);  
	bn_printf(&a, 1);  
  
	bn_printf(&a, 0);  
	if (BN_is_word(&a, 100)) {  
		printf(” is 100\n”);  
	} else {  
		printf(” is not 100\n”);  
	}  
  
	BN_set_negative(&a, 1);  
	printf(”BN_set_negative a: ”);  
	bn_dec_printf(&a);  
  
	BN_free(&a);  
	BN_init(&a);  
  
	BN_generate_prime_ex(&a, bits, 1, NULL, NULL, NULL);  
	printf(”BN_generate_prime_ex a: ”);  
	bn_printf(&a, 1);  
  
	bn_printf(&a, 0);  
	if (1 == BN_is_prime_ex(&a, BN_prime_checks, NULL, NULL)) {  
		printf(” is prime\n”);  
	} else {  
		printf(” is not prime\n”);  
	}  
  
	BN_add_word(&a, 2);  
	bn_printf(&a, 0);  
	if (1 == BN_is_prime_ex(&a, BN_prime_checks, NULL, NULL)) {  
		printf(” is prime\n”);  
	} else {  
		printf(” is not prime\n”);  
	}  
  
	BN_free(&a);  
	BN_free(&b);  
	BN_free(&c);  
	BN_free(&d);  
  
	BN_CTX_free(ctx);  
  
	return 0;  
}  
