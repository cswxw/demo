void test(){
	ASN1_OBJECT *obj1 = OBJ_txt2obj("1.2.840.113549.1.1.5",0);
	int a = OBJ_create("1.2.840.113549.1.1.5.2.3","this is sn","this is ln"); //返回nid
	ASN1_OBJECT *obj = OBJ_nid2obj(a);
	ASN1_OBJECT *obj2 = OBJ_txt2obj("1.2.840.113549.1.1.5.2.3",0);

}
