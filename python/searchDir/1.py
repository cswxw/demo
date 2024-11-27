

import os

def mkdir(path):

    import os
 

    path=path.strip()

    path=path.rstrip("\\")
 

    isExists=os.path.exists(path)
 

    if not isExists:

        os.makedirs(path) 
 
        #print path+' success'
        return True
    else:

        #print path+' has exist'
        return False

def writeNew(old,new,newdir):
    print old, ' '  , new
    f=open(old,"r")
    mkdir(newdir)
    f2=open(new,"w")
    for line in f:
        if len(line) >=8:
            f2.write(line)
    f.close();
    f2.close();
        
def filesort(old , new,newdir):
    print old, ' '  , new
    f=open(old,"r")
    mkdir(newdir)
    f2=open(new,"w")

    lists=f.readlines();
    
    lists = list(set(lists))
    
    lists.sort();
    f2.writelines(lists);
    
    f.close();
    f2.close();

rootdir= "d:\dict3_n"
#newdir= "d:\dict3_new"


def filter(a,b):
    lists=list(set(b).difference(set(a).intersection(b)))
    lists.sort();
    return lists;

def getAllfilepath(path):
    lists=[]
    for dirpath,dirnames,filenames in os.walk(path):
        for file in filenames:
            fullpath=os.path.join(dirpath,file)
            lists.append(fullpath)
    return lists;


dic = getAllfilepath(rootdir)


for i in range(0,len(dic)):
	f=open(dic[i],"r")
	a=f.readlines();
	f.close()
	print dic[i]
	for j in range(i+1,len(dic)):
	    #b
		old2 = dic[j]
		f2=open(old2,"r")
		b=f2.readlines();
		f2.close()
		#new = os.path.join(root.replace(rootdir, newdir),files[j])
		#tempdir = root.replace(rootdir,newdir);
		#print old,' ',old2 , ' ' ,  new
		#mkdir(tempdir)
		print '\t',old2
		f2=open(old2,"w")
		f2.writelines(filter(a,b));
		f2.close()


'''
dic = os.walk(rootdir);
for root, dirs, files in dic:
    for i in range(0,len(files)):
        # writeNew(os.path.join(root,file),os.path.join(root.replace(rootdir,newdir),file),root.replace(rootdir,newdir))
        #filesort(os.path.join(root,file),os.path.join(root.replace(rootdir,newdir),file),root.replace(rootdir,newdir))

        # check
        #if i+1 >= len(files):
        #    break;
        
        #a
        old = os.path.join(root,files[i]);
        f=open(old,"r")
        a=f.readlines();
        print old
        for j in range(i+1,len(files)):
            #b
            old2 = os.path.join(root,files[j]);    
            f2=open(old2,"r")
            b=f2.readlines();
            f2.close()
            
            #new = os.path.join(root.replace(rootdir, newdir),files[j])
            #tempdir = root.replace(rootdir,newdir);
            #print old,' ',old2 , ' ' ,  new
            #mkdir(tempdir)
            print '\t',old2
            
            f2=open(old2,"w")
            f2.writelines(filter(a,b));
            f2.close()

            
            
        f.close()
        
  '''          
            
                    




        
