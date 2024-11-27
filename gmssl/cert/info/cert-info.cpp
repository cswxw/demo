//show x509 subject info and issuer info
void ShowCerts(X509 *cert)  
{  
	char *line;
    if (cert != NULL) {  
        printf("cert info:\n");  
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);  
        printf("cert: %s\n", line);   
        free(line);  
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);  
        printf("issuer: %s\n", line);  
        free(line);  

    } else  
        printf("[error]not cert info\n");  
}

