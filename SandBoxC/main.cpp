


#include <iostream>
#include <vector>
#include <sstream> //this is where istringstream is defined

using namespace std;


// Couple de ET connecte par des XOR
struct Formule{
    unsigned int formuleL[512];
    unsigned int formuleR[512];
    unsigned int fsp=0;
   
   void add(unsigned int a,unsigned int b){
       formuleL[fsp]=a;
       formuleR[fsp]=b;
       fsp++;
   }
   
    void out(){
           for(int i = 0; i < fsp; i++)
            cout << "(" << formuleR[i] << "&" << formuleL[i] << ")^";       // print result  
           
           cout <<endl;
    }   
};

/* 512 bits hypothetiques */
struct hyp{
    unsigned int v[512/32];
    int nbBits;
    
    void clr(){
        for(int i = 0; i < 512 / 32; i++)
            v[i]=0;
    }
    
    void out(){
           for(int i = 0; i < nbBits / 32; i++)
            cout << hex << v[i] << " ";       // print result  
           
           cout <<endl;
    }
    
    hyp *clone(){
        hyp *res=new hyp;
          res->nbBits=nbBits;
          for(int i = 0; i < nbBits ; i++){
              res->setAt(i,bitAt(i));
          }
          
          
          return res;
    }
    
    unsigned int bitAt(int numBit){
        return ((v[numBit / 32] >> (numBit % 32))&1);
    }

    void setAt(unsigned int numBit,unsigned int k){
        unsigned int t=(unsigned int)k<<(numBit%32);
        unsigned int m=(unsigned int)1<<(numBit%32);

        unsigned int o=(v[numBit / 32]);
        o=o&~m;
        v[numBit / 32] = o | t;
    }    
    
    void copy(hyp *src){
          nbBits=src->nbBits;
          for(int i = 0; i < nbBits ; i++){
              setAt(i,src->bitAt(i));
          }        
    }
    
    unsigned int test(Formule *f){
        unsigned int r=0;
        for(int i=0;i<f->fsp;i++){
            int a=f->formuleL[i];
            int b=f->formuleR[i];
            r^=(bitAt(a)&bitAt(b));
        }
        return r;
    }
};

const int stackSize=812000*16;

/* Multiples hypotheses */
struct hypStack{
    
    
    hyp h[stackSize];
    bool tst[stackSize];
    unsigned int cp=0;
    
    hyp addedBit;
    
    int nbBit=512;
    
    void setNbBit(int nb){
        nbBit=nb;
    }
    
    void addBit(unsigned int index){
        // si vide, ajoute deux hypothese 0 partout + 1 sur la deuxieme pour le bit ajoute
        if (cp==0){
            addedBit.clr();
            addedBit.nbBits=nbBit;
            
            h[0].clr();
            h[0].nbBits=nbBit;
            h[1].copy(&h[0]);
            h[1].setAt(index,1);
            cp+=2;
            addedBit.setAt(index,1);
            return;
        }
        
        if(cp>= stackSize/2) {cout << "!!!!!!!!!!!!!!! hyp stack too big !!!!!!!!!!!!!!!!!!!!" << endl; int nni=0;int nnj=0; nni=nni/nnj;}
        
        bool exist=addedBit.bitAt(index)==1;
    
        // ajoute le bit dans addedBit s'il n'existe pas
        // si ajout du bit, alors duplication des hypotheses avec bit à 0 en haut, et bit à 1 en bas
        if(!exist){
            
            for(int i=0;i<cp;i++){
                h[cp+i].copy(&h[i]);
                h[cp+i].setAt(index,1);
            }
            cp+=cp;
            addedBit.setAt(index,1);
        }
    }
    
    void out(){
        cout << "nb Hypotheses " << cp << " bit set mask" << endl;
        addedBit.out();
        cout << "---------------" << endl;
        for(int i=0;i<cp;i++){
            h[i].out();
        }
    }
    
    void addBitFromFormula(Formule *form){
        for(int i=0;i<form->fsp;i++){
            addBit(form->formuleL[i]);
            addBit(form->formuleR[i]);
        }
    }
    
    void addFormula(Formule *form,unsigned int expectedVal){
        // ajoute les bits de la formule
        addBitFromFormula(form);
        cout << "nb Hyp avant application : " << cp << endl;
        
        // Elimine les hypotheses qui ne correspondent pas
        checkConstraint(form,expectedVal);
        cout << "nb Hyp apres application : " << cp << endl;
    }
    
    void checkConstraint(Formule *form,unsigned int expectedVal){
        //Marque les hypotheses qui ne correspondent pas a la contrainte

        int nb=0;
        for(int i=0;i<cp;i++){
            unsigned int appF=h[i].test(form);
            tst[i]=(appF==expectedVal);
            //cout << i << " Out " << expectedVal << " Form " << appF<< endl;
            if(tst[i]==false){ 
                nb++;
                //cout << i << " non fit " << endl;
            
            };
            
   
        }
        
        int decC=0;
        for(int i=0;i<cp;i++){
            while(tst[i+decC]==false && (i+decC)<cp){ decC++; }
            if(decC>0 && (i+decC < cp)){
                //cout << i<< " <- " << (i+decC) << endl;
                h[i].copy(&h[i+decC]);
            }
        }
        cp-=decC;
        
    }
    
};

Formule *buildFormule(int nbBits){
    Formule *formule=new Formule[nbBits];
    for (int i = 0; i < nbBits/2; i++)
    for (int j = 0; j < nbBits/2; j++){
        formule[i+j].add(i,j+nbBits/2);
        
        //set(b,i+j, x(b,i+j)^(x(a,i)&x(a,j+size)));
    }

    return formule;
}

hyp* fromString(string is){
    
      int size;
      std::istringstream sin(is);
  sin >> size;
  
  hyp* res=new hyp;
  res->nbBits=size*2;
  
   
    for (int i = 0; i < size / 16; i++) {   // Read size / 16 integers to a
      sin >> hex >> res->v[i];
    }
  
  if(size/16 < 1) sin >> hex >> res->v[0];

  return res;
  
}

void tst003(){
        
    string is="16\n 000073af 00000000";
    string out_theo="00000001 000073af\n"
       " 00000083 000000e5\n"
       " 000000e5 00000083\n"
       " 000073af 00000001\n";
    
    hyp *in=fromString(is);
      in->out();
      
      Formule *f = buildFormule(in->nbBits);
      
      for(int i=0;i<in->nbBits;i++){
         // cout << i <<") " << in->bitAt(i) << "<=>";
         // f[i].out();
      }
      hypStack *hh = new hypStack;
      for(int numForm=0;numForm<in->nbBits/2;numForm++){
          
          int i=numForm;
          cout <<"FORMULE "<< i <<") " << in->bitAt(i) << "<=>";
          f[i].out();
          cout  << "-----------------------" << endl;

        // Elimine les hypotheses qui ne correspondent pas
        hh->addFormula(&f[i],in->bitAt(i));       
        cout << "+++ remove non fit +++"<<endl;
          //hh->out();
          
          i=(-numForm+in->nbBits)-2;
          cout <<"FORMULE "<< i <<") " << in->bitAt(i) << "<=>";
          f[i].out();
          cout  << "-----------------------" << endl;

        // Elimine les hypotheses qui ne correspondent pas
        hh->addFormula(&f[i],in->bitAt(i));       
        cout << "+++ remove non fit +++"<<endl;
          //hh->out();          
      }      
      hh->out();
}

void tst002(){
      //string is="256\n 320a18d5 b61b13f6 1aaaa61c 0afe2a41 1a4ff107 84cc2efc 956ff31d fa595299 33749a7f 6cc9659d dc503569 ef4d0ef5 73b746c5 b8fb36d3 7616e9d6 b21251c4\n";
    
    string is="32\n 000073af 00000000";
    string out_theo="00000001 000073af\n"
       " 00000083 000000e5\n"
       " 000000e5 00000083\n"
       " 000073af 00000001\n";
    
    hyp *in=fromString(is);
      in->out();
      
      Formule *f = buildFormule(in->nbBits);
      
      for(int i=0;i<in->nbBits;i++){
         // cout << i <<") " << in->bitAt(i) << "<=>";
         // f[i].out();
      }
      hypStack hh;
      for(int i=0;i< 2/*in->nbBits*/;i++){
          cout <<"FORMULE "<< i <<") " << in->bitAt(i) << "<=>";
          f[i].out();
          cout  << "-----------------------" << endl;
        // ajoute les bits de la formule
        hh.addBitFromFormula(&f[i]);
        cout << "+++ Add bits +++";
                hh.out();
        
        // Elimine les hypotheses qui ne correspondent pas
        hh.checkConstraint(&f[i],in->bitAt(i));       
        cout << "+++ remove non fit +++";
          hh.out();
      }      
      
}

void tst001(){
      string is="256\n 320a18d5 b61b13f6 1aaaa61c 0afe2a41 1a4ff107 84cc2efc 956ff31d fa595299 33749a7f 6cc9659d dc503569 ef4d0ef5 73b746c5 b8fb36d3 7616e9d6 b21251c4\n";
      hyp *in=fromString(is);
      in->out();
      
      hyp *cl=in->clone();
      cl->out();
      
      hypStack hh;
      hh.addBit(3);
      hh.out();
      hh.addBit(6);
      hh.out();
}

int main()
{
    cout << "Main start" <<endl;
    //tstSetGet();
    //autoRef();cout<<endl;
    //funRefWithBits();cout<<endl;
   // funWithFormule();cout<<endl;
    //tst001();
    tst003();
    
    cout << "++++ Main end ++++"<<endl;
    return(0);
    //return funRef();
}

//--------------------------------------------------------------------

int x(unsigned int tab[],int numBit){
    return ((tab[numBit / 32] >> (numBit % 32))&1);
}


void set(unsigned int tab[],unsigned int numBit,unsigned int v){
    unsigned int t=(unsigned int)v<<(numBit%32);
    unsigned int m=(unsigned int)1<<(numBit%32);
    
    unsigned int o=(tab[numBit / 32]);
    o=o&~m;
    tab[numBit / 32] = o | t;
}


struct Hypmat{
    unsigned int bit[512];
    bool open[512];
    unsigned int fsp=0;
   
    void reset(int nb){
        for(int i=0;i<512;i++){
            bit[i]=0;
            open[i]=true;
        }
        fsp=nb;
    }
    
    bool r(int i){
        return open[i];
    }
    
    int v(int i){
        return bit[i];
    }
    
    void set(int i,unsigned int x){
        open[i]=true;
        bit[i]=x;
    }
    
    int closeSingleAndWithOne(Formule f){
        int det=1;
        
        if(det == 1){
            int res=0;
            for(int i=0;i<f.fsp;i++){
                if(r(f.formuleL[i]) ||r(f.formuleR[i]) ) {
                    res=i;
                    break;
                }
            }            
            
            int fl=f.formuleL[res];
            int fr=f.formuleR[res];
            open[fl]=false;
            open[fr]=false;
            bit[fl]=1;
            bit[fr]=1;
         
            return 2;
        }
    }
    
    int countOpened(){
        int res=0;
        for(int i=0;i<fsp;i++){
            if(r(i)) res++;
        }
        return res;
    }
    int countOpened(Formule f){
        int res=0;
        for(int i=0;i<f.fsp;i++){
            if(r(f.formuleL[i])) res++;
            if(r(f.formuleR[i])) res++;
        }
        return res;
    }  
    
    int countOpenedAnd(Formule f){
        int res=0;
        for(int i=0;i<f.fsp;i++){
            if(r(f.formuleL[i]) ||r(f.formuleR[i]) ) res++;
        }
        return res;
    }      
};

void reverseIt2(unsigned int* o,Formule *f,int nbBits){
    
}


void reverseIt(unsigned int* o,Formule *f,int nbBits){
    unsigned int* e = new unsigned int[nbBits/32];
    unsigned int* bstack = new unsigned int[nbBits/32];
    int sp=0;
    
    Hypmat h;
    
    h.reset(nbBits);
    
    cout << endl;
    cout << "reversed";
    cout << endl;
    
    int nbclosed=0;
    int oldClosed;
    
    bool showAll=true;
    
    if(showAll==true)
    for(int halfi=0;halfi<nbBits/2;halfi++){
            {
                int ind=halfi;

                int nbop=h.countOpened(f[ind]);
                int nbopAnd=h.countOpenedAnd(f[ind]);
                unsigned int getV=x(o,ind);
                //if(nbop>0 && nbop<=16)
                {

                    cout << ind << " o "<< getV <<" open "<< nbop <<" openAnd "<< nbopAnd<<endl;
                }
            }
            {
                int ind=nbBits-halfi-2;

                int nbop=h.countOpened(f[ind]);
                int nbopAnd=h.countOpenedAnd(f[ind]);
               // if(nbop>0 && nbop<=16)
                {
                    unsigned int getV=x(o,ind);
                    cout << ind << " o "<< getV <<" open "<< nbop <<" openAnd "<< nbopAnd<<endl;
                }

            }        



        }
    
    if(showAll==false)
    do{
        oldClosed=nbclosed;
        for(int halfi=0;halfi<nbBits/2;halfi++){

            {
                int ind=halfi;

                int nbop=h.countOpened(f[ind]);
                int nbopAnd=h.countOpenedAnd(f[ind]);
                unsigned int getV=x(o,ind);
                //if(nbop>0 && nbop<=16)
                {

                    cout << ind << " o "<< getV <<" open "<< nbop <<" openAnd "<< nbopAnd<<endl;
                }

                if(getV==1 && nbopAnd==1){
                    nbclosed+=h.closeSingleAndWithOne(f[ind]);
                } 

            }
            {
                int ind=nbBits-halfi-2;

                int nbop=h.countOpened(f[ind]);
                int nbopAnd=h.countOpenedAnd(f[ind]);
                if(nbop>0 && nbop<=16){
                    unsigned int getV=x(o,ind);
                    cout << ind << " o "<< getV <<" open "<< nbop <<" openAnd "<< nbopAnd<<endl;
                }

            }        



        }
        cout << "nbClosed "<< nbclosed << endl;
    }while(nbclosed!=oldClosed);
    
}

int funWithFormule(){
  int size;
  
   cout<<"funWithFormule"<<endl;
  string is="256\n 320a18d5 b61b13f6 1aaaa61c 0afe2a41 1a4ff107 84cc2efc 956ff31d fa595299 33749a7f 6cc9659d dc503569 ef4d0ef5 73b746c5 b8fb36d3 7616e9d6 b21251c4\n";
  // string is="32\n 00000001 000073af\n";
  std::istringstream sin(is);
  sin >> size;
 
  unsigned int* a = new unsigned int[size / 16]; // <- input tab to encrypt
  unsigned int* b = new unsigned int[size / 16]; // <- output tab
   Formule *formule=new Formule[size*2];
  for(int i=0;i<size* 2;i++){
      formule[i].fsp=0;
  }
  cout<<"funWithFormule begin read"<<endl;
   
    for (int i = 0; i < size / 16; i++) {   // Read size / 16 integers to a
      sin >> hex >> a[i];
    }

    for (int i = 0; i < size / 16; i++) {   // Write size / 16 zeros to b
      b[i] = 0;
    }	  
    for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++){
        formule[i+j].add(i,j+size);
        
        //set(b,i+j, x(b,i+j)^(x(a,i)&x(a,j+size)));
    }
  cout<<"funWithFormule apply formulaes"<<endl;
    for(int i=0;i<size*2;i++){
        //cout<<i<<endl;
        unsigned int xo=0;
        for(int k=0;k<formule[i].fsp;k++){
            unsigned int v= x(a,formule[i].formuleL[k]) & x(a,formule[i].formuleR[k]);
            xo^=v;
        }

        set(b,i, xo  );
    }

    for(int i = 0; i < size / 16; i++)
      cout << hex << b[i] << " ";       // print result  
  
    unsigned int nbBit=size*2;
    reverseIt2(b,formule,nbBit);
  
}

int writeFour(unsigned int tab[],int curBitPos){
    int v=x(tab,curBitPos++);
    v+=x(tab,curBitPos++)*2;
    v+=x(tab,curBitPos++)*4;
    v+=x(tab,curBitPos++)*8;
    
    char a=' ';
    switch(v){
        case 0:case 1:case 2:case 3:case 4:case 5:case 6:case 7:case 8:case 9:
            a='0'+v;
        break;
        case 10:case 11:case 12:case 13:case 14:case 15:
            a='a'+(v-10);
        break;        
    }
}

void tstSetGet(){
 int size;
  
  
  string is="256\n 320a18d5 b61b13f6 1aaaa61c 0afe2a41 1a4ff107 84cc2efc 956ff31d fa595299 33749a7f 6cc9659d dc503569 ef4d0ef5 73b746c5 b8fb36d3 7616e9d6 b21251c4\n";
  std::istringstream sin(is);
  sin >> size;
 
  unsigned int* a = new unsigned int[size / 16]; // <- input tab to encrypt
  unsigned int* b = new unsigned int[size / 16]; // <- output tab
  
   
    for (int i = 0; i < size / 16; i++) {   // Read size / 16 integers to a
      sin >> hex >> a[i];
    }

    for (int i = 0; i < size / 16; i++) {   // Write size / 16 zeros to b
      b[i] = 0;
    }	  
  
    for (int i = 0; i < size; i++) {   
        cout<<x(a,i);
        set(b,i,x(a,i));
        set(b,i+size,x(a,i+size));
    }  
  cout<<endl;
  
  cout<<"theorique"<<endl;
      for(int i = 0; i < size / 16; i++)
      cout << hex << a[i] << " ";       // print result  
  cout<<endl;
    cout<<"obtenu"<<endl;
      for(int i = 0; i < size / 16; i++)
      cout << hex << b[i] << " ";       // print result  
    cout<<endl;
}

int funRefWithBits(){
  int size;
  
  
  string is="256\n 320a18d5 b61b13f6 1aaaa61c 0afe2a41 1a4ff107 84cc2efc 956ff31d fa595299 33749a7f 6cc9659d dc503569 ef4d0ef5 73b746c5 b8fb36d3 7616e9d6 b21251c4\n";
  std::istringstream sin(is);
  sin >> size;
 
  unsigned int* a = new unsigned int[size / 16]; // <- input tab to encrypt
  unsigned int* b = new unsigned int[size / 16]; // <- output tab
  
   
    for (int i = 0; i < size / 16; i++) {   // Read size / 16 integers to a
      sin >> hex >> a[i];
    }

    for (int i = 0; i < size / 16; i++) {   // Write size / 16 zeros to b
      b[i] = 0;
    }	  
    for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++){
          set(b,i+j, x(b,i+j)^(x(a,i)&x(a,j+size)));
    }


    for(int i = 0; i < size / 16; i++)
      cout << hex << b[i] << " ";       // print result  
  
}


int funRef(){
      int size;
  
  
  cin >> size;
 
  unsigned int* a = new unsigned int[size / 16]; // <- input tab to encrypt
  unsigned int* b = new unsigned int[size / 16]; // <- output tab
 
  for (int i = 0; i < size / 16; i++) {   // Read size / 16 integers to a
    cin >> hex >> a[i];
  }

  for (int i = 0; i < size / 16; i++) {   // Write size / 16 zeros to b
    b[i] = 0;
  }	
 
  for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++)
      b[(i + j) / 32] ^= ( (a[i / 32] >> (i % 32)) &
		       (a[j / 32 + size / 32] >> (j % 32)) & 1 ) << ((i + j) % 32);   // Magic centaurian operation
 
  for(int i = 0; i < size / 16; i++)
    cout << hex << b[i] << " ";       // print result

 /* 
    Good luck humans     
 */
  return 0;
}

int autoRef(){
  int size;
  //auto instring="32\nb0c152f9 ebf2831f\n";
  //istringstream sin(instring);
  //string is="32\nb0c152f9 ebf2831f\n";
  //string is="32\n ebf2831f  b0c152f9\n";
  string is="256\n 320a18d5 b61b13f6 1aaaa61c 0afe2a41 1a4ff107 84cc2efc 956ff31d fa595299 33749a7f 6cc9659d dc503569 ef4d0ef5 73b746c5 b8fb36d3 7616e9d6 b21251c4\n";
  std::istringstream sin(is);
  sin >> size;
 
  unsigned int* a = new unsigned int[size / 16]; // <- input tab to encrypt
  unsigned int* b = new unsigned int[size / 16]; // <- output tab
 
  for (int i = 0; i < size / 16; i++) {   // Read size / 16 integers to a
    sin >> hex >> a[i];
  }

  for (int i = 0; i < size / 16; i++) {   // Write size / 16 zeros to b
    b[i] = 0;
  }	
 
  for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++)
      b[(i + j) / 32] ^= 
              ( 
                (
                a[i / 32] >> (i % 32)
                ) 
              &
               (
                  a[j / 32 + size / 32] >> (j % 32)
                ) 
              & 1 )
              << ((i + j) % 32);   // Magic centaurian operation
 
  for(int i = 0; i < size / 16; i++)
    cout << hex << b[i] << " ";       // print result

 /* 
    Good luck humans     
 */
  return 0;    
}

