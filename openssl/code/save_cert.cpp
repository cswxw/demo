 //该函数用于保存PEM证书文件
static int SaveCertificate(X509 *m_pxCert,const char * certPath)
 {
 	BIO	* out = BIO_new_file(certPath,"wb");
 	int res;
 	res=PEM_write_bio_X509(out,m_pxCert);
 	BIO_free(out);
 	return (res > 0);
}
//der证书
static int SaveCertificateDer(X509 *m_pxCert,const char * certPath)
 {
 	BIO	* out = BIO_new_file(certPath,"wb");
 	int res;
 	res=i2d_X509_bio(out,m_pxCert);
 	BIO_free(out);
 	return (res > 0);
}


