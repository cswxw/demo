import base64
f=open("sm2_sign.cer")
a=f.read()
f.close()
b=base64.b64encode(a)
f2=open("test.txt","w")
f2.write(b)
f2.close()
