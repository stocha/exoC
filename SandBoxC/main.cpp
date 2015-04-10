
#include <iostream>
#include <vector>
#include <sstream> //this is where istringstream is defined

using namespace std;


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

struct Formule{
    unsigned int formuleL[512];
    unsigned int formuleR[512];
    unsigned int fsp=0;
   
   void add(unsigned int a,unsigned int b){
       formuleL[fsp]=a;
       formuleR[fsp]=b;
       fsp++;
   }
};

int funWithFormule(){
  int size;
  
   cout<<"funWithFormule"<<endl;
  string is="256\n 320a18d5 b61b13f6 1aaaa61c 0afe2a41 1a4ff107 84cc2efc 956ff31d fa595299 33749a7f 6cc9659d dc503569 ef4d0ef5 73b746c5 b8fb36d3 7616e9d6 b21251c4\n";
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

int main()
{
    //tstSetGet();
    autoRef();cout<<endl;
    funRefWithBits();cout<<endl;
    funWithFormule();cout<<endl;
    return(0);
    //return funRef();
}